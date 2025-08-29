/*
Module : tracer.cpp
Purpose: Implementation for a C++ wrapper class to encapsulate "TraceRoute" functionality on Windows
Created: PJN / 17-11-1998
History: PJN / 25-02-2002 1. Updated copyright information
                          2. Class now uses #includes CPing class rather than implemented its
                          own implementation of Ping
                          3. General code tidy up and review
         PJN / 05-05-2002 1. Uses latest CPing class, now by default uses ICMP method in CPing class.
                          2. Sample app provided with tracer now actually observes the "-d" command
                          line option 
                          3. Fixed a bug where a pinging error would cause the tracer loop to exit
                          instead of reporting a non-pingable device
         PJN / 07-06-2008 1. Updated copyright details
                          2. Addition of TRACEROUTE_EXT_CLASS define to the classes to allow the classes 
                          to be more easily incorporated into extension DLLs
                          3. Updated the code to work with latest version of the author's CPing class
                          4. Updated the sample app to compile cleanly in VC 2005
                          5. The code has now been updated to support VC 2005 or later only
                          6. Code now uses newer C++ style casts instead of C style casts
                          7. Code now compiles cleanly using Code Analysis (/analyze)
                          8. Updated the code to clean compile on VC 2008
                          9. Updated documentation to use the same style as the web site
          PJN / 08-05-016 1. Updated copyright details.
                          2. Updated the sample project settings to more modern default values.
                          3. Updated the code to compile cleanly on VC 2010 - 2015.
                          4. Added support for IPv6 in addition to IPv4. This means that the minimum 
                          supported version of VC which supports CTraceRoute is now VC 2008. The sample 
                          now ships with a VC 2008 solution.
                          5. Added SAL annotations to all the code.
                          6. Reworked the classes to optionally compile without MFC. By default the 
                          classes now use STL classes and idioms but if you define CTRACEROUTE_MFC_EXTENSTIONS
                          the classes will revert back to the MFC behaviour.
         PJN / 26-12-2017 1. Updated copyright details.
                          2. Replaced CString::operator LPC*STR() calls throughout the codebase with
                          CString::GetString calls
                          3. Replaced NULL throughout the codebase with nullptr. This means that the minimum
                          requirement for the code is now VC 2010.
         PJN / 03-11-2018 1. Updated copyright details.
                          2. Fixed a number of C++ core guidelines compiler warnings. These changes mean that the 
                          code will now only compile on VC 2017 or later.
                          3. Removed all code which used GetProcAddress in the sample app.
                          4. Removed code which supported CTRACEROUTE_MFC_EXTENSTIONS code path
         PJN / 05-05-2019 1. Updated copyright details.
                          2. Updated the code to clean compile on VC 2019
         PJN / 10-11-2019 1. Updated initialization of various structs to use C++ 11 list initialization
                          2. Replaced BOOL with bool in a number of places throughout the codebase
         PJN / 29-12-2019 1. Fixed various Clang-Tidy static code analysis warnings in the code.
         PJN / 28-03-2020 1. Updated copyright details.
                          2. Fixed more Clang-Tidy static code analysis warnings in the code.
         PJN / 18-03-2022 1. Updated copyright details.
                          2. Updated the code to use C++ uniform initialization for all variable declarations.

Copyright (c) 1998 - 2022 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


///////////////////////////////// Includes ////////////////////////////////////

#include "pch.h"
#include "tracer.h"
#include "ping.h" //If you get a compilation error about this missing header file, then you need to download my CPing class from http://www.naughter.com/ping.html
#ifndef _INC_LIMITS
#pragma message("To avoid this message please put limits.h in your pre compiled header (usually stdafx.h)")
#include <limits.h>
#endif //#ifndef _INC_LIMITS


///////////////////////////////// Implementation //////////////////////////////

CTraceRoute::String CTraceRoute::AddressToString(const SOCKADDR* pSockAddr, int nSockAddrLen, int nFlags, UINT* pnSocketPort)
{
	//What will be the return value from this function
	String sSocketAddress;

	int nResult{ 0 };
#ifdef _UNICODE
	CStringW sName;
	nResult = GetNameInfoW(pSockAddr, nSockAddrLen, sName.GetBuffer(NI_MAXHOST), NI_MAXHOST, nullptr, 0, nFlags);
	sName.ReleaseBuffer();
#else
	CStringA sName;
	nResult = getnameinfo(pSockAddr, nSockAddrLen, sName.GetBuffer(NI_MAXHOST), NI_MAXHOST, nullptr, 0, nFlags);
	sName.ReleaseBuffer();
#endif
	if (nResult == 0)
	{
		sSocketAddress = sName;
		if (pnSocketPort != nullptr)
			*pnSocketPort = ntohs(SS_PORT(&pSockAddr));
	}

	return sSocketAddress;
}

bool CTraceRoute::Tracev4(_In_z_ LPCTSTR pszHostName, _Inout_ CReplyv4& trr, _In_ UCHAR nHopCount, _In_ DWORD dwTimeout, _In_ DWORD dwPingsPerHost, _In_ WORD wDataSize, _In_ UCHAR nTOS, _In_ bool bDontFragment, _In_ bool bFlagReverse, _In_opt_z_ LPCTSTR pszLocalBoundAddress)
{
	//Validate our parameters
#pragma warning(suppress: 26477)
	ATLASSERT(pszHostName != nullptr);
#pragma warning(suppress: 26477)
	ATLASSERT(nHopCount > 0);

	//Set the output parameter to a sane default
	trr.clear();

	if (dwPingsPerHost == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	//Do the address lookup
	ATL::CSocketAddr lookup;
	const int nError{ lookup.FindAddr(pszHostName, 0, 0, AF_INET, 0, 0) };
	if (nError != 0)
	{
		SetLastError(nError);
		return false;
	}

	//Note we always use the first address returned from ATL::CSocketAddr::FindAddr. If you want to use a different
	//Address then do the lookup yourself and pass in the correct value in the "pszHostName" parameter
	const ADDRINFOT* pAddress{ lookup.GetAddrInfoList() };
#pragma warning(suppress: 26477)
	ATLASSUME(pAddress != nullptr);
#pragma warning(suppress: 26477)
	ATLASSERT(pAddress->ai_family == AF_INET);
#pragma warning(suppress: 26489 26490)
	auto pDestAddress{ reinterpret_cast<const sockaddr_in*>(pAddress->ai_addr) };
#pragma warning(suppress: 26472)
	String sDestAddress{ AddressToString(pAddress->ai_addr, static_cast<int>(pAddress->ai_addrlen), NI_NUMERICHOST, nullptr) };

	//Iterate through all the hop count values
	bool bReachedHost{ false };
	for (UCHAR i{ 1 }; i <= nHopCount && !bReachedHost; i++)
	{
		CHostTraceMultiReplyv4 htrr;
		htrr.dwError = ERROR_SUCCESS;
		htrr.minRTT = ULONG_MAX;
		htrr.avgRTT = 0;
		htrr.maxRTT = 0;

		//Iterate through all the pings for each host
		DWORD totalRTT{ 0 };
		CHostTraceSingleReplyv4 htsr{};
		bool bPingError{ false };
		for (DWORD j{ 0 }; j < dwPingsPerHost && !bPingError; j++)
		{
			if (Pingv4(sDestAddress.c_str(), htsr, i, dwTimeout, wDataSize, nTOS, bDontFragment, bFlagReverse, pszLocalBoundAddress))
			{
				//Accumulate the total RTT
				totalRTT += htsr.RTT;

				//Store away the RTT's
				if (htsr.RTT < htrr.minRTT)
					htrr.minRTT = htsr.RTT;
				if (htsr.RTT > htrr.maxRTT)
					htrr.maxRTT = htsr.RTT;

				//Call the virtual function
				if (!OnPingResult(j + 1, htsr))
				{
					SetLastError(ERROR_CANCELLED);
					return false;
				}
			}
			else
			{
				htrr.dwError = GetLastError();
				bPingError = true;
			}
		}
		memcpy_s(&htrr.Address, sizeof(htrr.Address), &htsr.Address, sizeof(htsr.Address));
		if (htrr.dwError == 0)
			htrr.avgRTT = totalRTT / dwPingsPerHost;
		else
		{
			htrr.minRTT = 0;
			htrr.avgRTT = 0;
			htrr.maxRTT = 0;
		}

		//Call the virtual function
		if (!OnSingleHostResult(i, htrr))
		{
			SetLastError(ERROR_CANCELLED);
			return false;
		}

		//Add to the list of hosts
#pragma warning(suppress: 26489)
		trr.push_back(htrr);

		//Have we reached the final host?
#pragma warning(suppress: 26489)
		if (memcmp(&pDestAddress->sin_addr, &htrr.Address.sin_addr, sizeof(htrr.Address.sin_addr)) == 0)
			bReachedHost = true;
	}

	return true;
}

bool CTraceRoute::Tracev6(_In_z_ LPCTSTR pszHostName, _Inout_ CReplyv6& trr, _In_ UCHAR nHopCount, _In_ DWORD dwTimeout, _In_ DWORD dwPingsPerHost, _In_ WORD wDataSize, _In_ UCHAR nTOS, _In_ bool bDontFragment, _In_ bool bFlagReverse, _In_opt_z_ LPCTSTR pszLocalBoundAddress)
{
	//Validate our parameters
#pragma warning(suppress: 26477)
	ATLASSERT(pszHostName != nullptr);
#pragma warning(suppress: 26477)
	ATLASSERT(nHopCount > 0);

	//Set the output parameter to a sane default
	trr.clear();

	if (dwPingsPerHost == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	//Do the address lookup
	ATL::CSocketAddr lookup;
	const int nError{ lookup.FindAddr(pszHostName, 0, 0, AF_INET6, 0, 0) };
	if (nError != 0)
	{
		SetLastError(nError);
		return false;
	}

	//Note we always use the first address returned from ATL::CSocketAddr::FindAddr. If you want to use a different
	//Address then do the lookup yourself and pass in the correct value in the "pszHostName" parameter
	const ADDRINFOT* pAddress{ lookup.GetAddrInfoList() };
#pragma warning(suppress: 26477)
	ATLASSUME(pAddress != nullptr);
#pragma warning(suppress: 26477)
	ATLASSERT(pAddress->ai_family == AF_INET6);
#pragma warning(suppress: 26489 26490)
	auto pDestAddress{ reinterpret_cast<const sockaddr_in6*>(pAddress->ai_addr) };
#pragma warning(suppress: 26472)
	String sDestAddress{ AddressToString(pAddress->ai_addr, static_cast<int>(pAddress->ai_addrlen), NI_NUMERICHOST, nullptr) };

	//Iterate through all the hop count values
	bool bReachedHost{ false };
	for (UCHAR i{ 1 }; i <= nHopCount && !bReachedHost; i++)
	{
		CHostTraceMultiReplyv6 htrr;
		htrr.dwError = ERROR_SUCCESS;
		htrr.minRTT = ULONG_MAX;
		htrr.avgRTT = 0;
		htrr.maxRTT = 0;

		//Iterate through all the pings for each host
		DWORD totalRTT{ 0 };
		CHostTraceSingleReplyv6 htsr{};
		bool bPingError{ false };
		for (DWORD j{ 0 }; j < dwPingsPerHost && !bPingError; j++)
		{
			if (Pingv6(sDestAddress.c_str(), htsr, i, dwTimeout, wDataSize, nTOS, bDontFragment, bFlagReverse, pszLocalBoundAddress))
			{
				//Accumulate the total RTT
				totalRTT += htsr.RTT;

				//Store away the RTT's
				if (htsr.RTT < htrr.minRTT)
					htrr.minRTT = htsr.RTT;
				if (htsr.RTT > htrr.maxRTT)
					htrr.maxRTT = htsr.RTT;

				//Call the virtual function
				if (!OnPingResult(j + 1, htsr))
				{
					SetLastError(ERROR_CANCELLED);
					return false;
				}
			}
			else
			{
				htrr.dwError = GetLastError();
				bPingError = true;
			}
		}
		memcpy_s(&htrr.Address, sizeof(htrr.Address), &htsr.Address, sizeof(htsr.Address));
		if (htrr.dwError == 0)
			htrr.avgRTT = totalRTT / dwPingsPerHost;
		else
		{
			htrr.minRTT = 0;
			htrr.avgRTT = 0;
			htrr.maxRTT = 0;
		}

		//Call the virtual function
		if (!OnSingleHostResult(i, htrr))
		{
			SetLastError(ERROR_CANCELLED);
			return false;
		}

		//Add to the list of hosts
#pragma warning(suppress: 26489)
		trr.push_back(htrr);

		//Have we reached the final host ?
#pragma warning(suppress: 26489)
		if (memcmp(&pDestAddress->sin6_addr, &htrr.Address.sin6_addr, sizeof(htrr.Address.sin6_addr)) == 0)
			bReachedHost = true;
	}

	return true;
}

bool CTraceRoute::Pingv4(_In_z_ LPCTSTR pszHostName, _Inout_ CHostTraceSingleReplyv4& htsr, _In_ UCHAR nTTL, _In_ DWORD dwTimeout, _In_ WORD wDataSize, _In_ UCHAR nTOS, _In_ bool bDontFragment, _In_ bool bFlagReverse, _In_opt_z_ LPCTSTR pszLocalBoundAddress)
{
	CPingReplyv4 pr;
	CPing ping;
#pragma warning(suppress: 26486)
	const bool bSuccess{ ping.PingUsingICMPv4(pszHostName, pr, nTTL, dwTimeout, wDataSize, nTOS, bDontFragment, bFlagReverse, pszLocalBoundAddress) };
	if (bSuccess)
	{
		//Ping was successful, copy over the pertinent info into the return structure
		memcpy_s(&htsr.Address, sizeof(htsr.Address), &pr.Address, sizeof(pr.Address));
		htsr.RTT = pr.RTT;
	}

	//return the status
	return bSuccess;
}

bool CTraceRoute::Pingv6(_In_z_ LPCTSTR pszHostName, _Inout_ CHostTraceSingleReplyv6& htsr, _In_ UCHAR nTTL, _In_ DWORD dwTimeout, _In_ WORD wDataSize, _In_ UCHAR nTOS, _In_ bool bDontFragment, _In_ bool bFlagReverse, _In_opt_z_ LPCTSTR pszLocalBoundAddress)
{
	CPingReplyv6 pr;
	CPing ping;
#pragma warning(suppress: 26486)
	const bool bSuccess{ ping.PingUsingICMPv6(pszHostName, pr, nTTL, dwTimeout, wDataSize, nTOS, bDontFragment, bFlagReverse, pszLocalBoundAddress) };
	if (bSuccess)
	{
		//Ping was successful, copy over the pertinent info into the return structure
		memcpy_s(&htsr.Address, sizeof(htsr.Address), &pr.Address, sizeof(pr.Address));
		htsr.RTT = pr.RTT;
	}

	//return the status
	return bSuccess;
}

#pragma warning(suppress: 26440)
bool CTraceRoute::OnPingResult(_In_ int /*nPingNum*/, _In_ const CHostTraceSingleReplyv4& /*htsr*/)
{
	//Default behaviour is just to return true to allow the trace route to continue
	return true;
}

#pragma warning(suppress: 26440)
bool CTraceRoute::OnPingResult(_In_ int /*nPingNum*/, _In_ const CHostTraceSingleReplyv6& /*htsr*/)
{
	//Default behaviour is just to return true to allow the trace route to continue
	return true;
}

#pragma warning(suppress: 26440)
bool CTraceRoute::OnSingleHostResult(_In_ int /*nHostNum*/, _In_ const CHostTraceMultiReplyv4& /*htmr*/)
{
	//Default behaviour is just to return true to allow the trace route to continue
	return true;
}

#pragma warning(suppress: 26440)
bool CTraceRoute::OnSingleHostResult(_In_ int /*nHostNum*/, _In_ const CHostTraceMultiReplyv6& /*htmr*/)
{
	//Default behaviour is just to return true to allow the trace route to continue
	return true;
}
