/*
Module : tracer.h
Purpose: Interface for a C++ wrapper class to encapsulate "TraceRoute" functionality on Windows
Created: PJN / 17-11-1998

Copyright (c) 1998 - 2022 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code.

*/


/////////////////////////// Macros / Defines //////////////////////////////////

#pragma once

#ifndef __TRACER_H__
#define __TRACER_H__

#ifndef CTRACEROUTE_EXT_CLASS
#define CTRACEROUTE_EXT_CLASS
#endif //#ifndef CTRACEROUTE_EXT_CLASS


/////////////////////////// Includes //////////////////////////////////////////

#ifndef _VECTOR_
#pragma message("To avoid this message, you should put vector in your pre compiled header (normally stdafx.h)")
#include <vector>
#endif //#ifndef _VECTOR_
#ifndef _STRING_
#pragma message("To avoid this message, you should put string in your pre compiled header (normally stdafx.h)")
#include <string>
#endif //#ifndef _STRING_


/////////////////////////// Classes ///////////////////////////////////////////

struct CTRACEROUTE_EXT_CLASS CHostTraceSingleReplyv4
{
	DWORD dwError; //GetLastError for this replier
	SOCKADDR_IN Address; //The IP address of the replier
	unsigned long RTT; //Round Trip time in milliseconds for this replier
};

struct CTRACEROUTE_EXT_CLASS CHostTraceMultiReplyv4
{
	DWORD dwError; //GetLastError for this host
	SOCKADDR_IN Address; //The IP address of the replier
	DWORD minRTT; //Minimum round trip time in milliseconds
	DWORD avgRTT; //Average round trip time in milliseconds
	DWORD maxRTT; //Maximum round trip time in milliseconds
};

struct CTRACEROUTE_EXT_CLASS CHostTraceSingleReplyv6
{
	DWORD dwError; //GetLastError for this replier
	SOCKADDR_IN6 Address; //The IP address of the replier
	unsigned long RTT; //Round Trip time in milliseconds for this replier
};

struct CTRACEROUTE_EXT_CLASS CHostTraceMultiReplyv6
{
	DWORD dwError; //GetLastError for this host
	SOCKADDR_IN6 Address; //The IP address of the replier
	DWORD minRTT; //Minimum round trip time in milliseconds
	DWORD avgRTT; //Average round trip time in milliseconds
	DWORD maxRTT; //Maximum round trip time in milliseconds
};

//The actual class which does the Trace Route
class CTRACEROUTE_EXT_CLASS CTraceRoute
{
public:
	//Typedefs
	using CReplyv4 = std::vector<CHostTraceMultiReplyv4>;
	using CReplyv6 = std::vector<CHostTraceMultiReplyv6>;
#ifdef _UNICODE
	using String = std::wstring;
#else
	using String = std::string;
#endif //#ifdef _UNICODE

	//Constructors / Destructors
	CTraceRoute() = default;
	CTraceRoute(const CTraceRoute&) = delete;
	CTraceRoute(CTraceRoute&&) = delete;
	virtual ~CTraceRoute() = default;

	//Methods
	CTraceRoute& operator=(const CTraceRoute&) = delete;
	CTraceRoute& operator=(CTraceRoute&&) = delete;
	bool Tracev4(_In_z_ LPCTSTR pszHostName, _Inout_ CReplyv4& trr, _In_ UCHAR nHopCount = 30, _In_ DWORD dwTimeout = 5000, _In_ DWORD dwPingsPerHost = 3, _In_ WORD wDataSize = 32, _In_ UCHAR nTOS = 0, _In_ bool bDontFragment = false, _In_ bool bFlagReverse = false, _In_opt_z_ LPCTSTR pszLocalBoundAddress = nullptr);
	bool Tracev6(_In_z_ LPCTSTR pszHostName, _Inout_ CReplyv6& trr, _In_ UCHAR nHopCount = 30, _In_ DWORD dwTimeout = 5000, _In_ DWORD dwPingsPerHost = 3, _In_ WORD wDataSize = 32, _In_ UCHAR nTOS = 0, _In_ bool bDontFragment = false, _In_ bool bFlagReverse = false, _In_opt_z_ LPCTSTR pszLocalBoundAddress = nullptr);
	virtual bool OnPingResult(_In_ int nPingNum, _In_ const CHostTraceSingleReplyv4& htsr);
	virtual bool OnSingleHostResult(_In_ int nHostNum, _In_ const CHostTraceMultiReplyv4& htmr);
	virtual bool OnPingResult(_In_ int nPingNum, _In_ const CHostTraceSingleReplyv6& htsr);
	virtual bool OnSingleHostResult(_In_ int nHostNum, _In_ const CHostTraceMultiReplyv6& htmr);

protected:
	//Methods
	static String AddressToString(const SOCKADDR* pSockAddr, int nSockAddrLen, int nFlags, UINT* pnSocketPort);
	virtual bool Pingv4(_In_z_ LPCTSTR pszHostName, _Inout_ CHostTraceSingleReplyv4& htsr, _In_ UCHAR nTTL, _In_ DWORD dwTimeout, _In_ WORD wDataSize, _In_ UCHAR nTOS, _In_ bool bDontFragment, _In_ bool bFlagReverse, _In_opt_z_ LPCTSTR pszLocalBoundAddress);
	virtual bool Pingv6(_In_z_ LPCTSTR pszHostName, _Inout_ CHostTraceSingleReplyv6& htsr, _In_ UCHAR nTTL, _In_ DWORD dwTimeout, _In_ WORD wDataSize, _In_ UCHAR nTOS, _In_ bool bDontFragment, _In_ bool bFlagReverse, _In_opt_z_ LPCTSTR pszLocalBoundAddress);
};

#endif //#ifndef __TRACER_H__
