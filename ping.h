/*
Module : ping.h
Purpose: Interface for a C++ wrapper class to encapsulate "Ping" on Windows
Created: PJN / 10-06-1998

Copyright (c) 1998 - 2022 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code.

*/


/////////////////////////// Macros / Defines ////////////////////////////////

#pragma once

#ifndef __PING_H__
#define __PING_H__

#ifndef __ATL_SOCKET__
#pragma message("To avoid this message please put atlsocket.h in your pre compiled header (normally stdafx.h)")
#include <atlsocket.h>
#endif //#ifndef __ATL_SOCKET__

#ifndef _ICMP_INCLUDED_
#pragma message("To avoid this message please put icmpapi.h in your pre compiled header (normally stdafx.h)")
#include <icmpapi.h>
#endif //#ifndef _ICMP_INCLUDED_

#ifndef _VECTOR_
#pragma message("To avoid this message please put vector in your pre compiled header (normally stdafx.h)")
#include <vector>
#endif //#ifndef _VECTOR_

#ifndef CPING_EXT_CLASS
#define CPING_EXT_CLASS
#endif //#ifndef CPING_EXT_CLASS

#ifndef CPING_EXT_API
#define CPING_EXT_API
#endif //#ifndef CPING_EXT_API


/////////////////////////// Classes ///////////////////////////////////////////

struct CPING_EXT_CLASS CPingReplyv4
{
	//Constructors / Destructors
	CPingReplyv4() noexcept;
	CPingReplyv4(const CPingReplyv4&) = delete;
	CPingReplyv4(CPingReplyv4&&) = delete;
	~CPingReplyv4() = default;

	//Methods
	CPingReplyv4& operator=(const CPingReplyv4&) = delete;
	CPingReplyv4& operator=(CPingReplyv4&&) = delete;
	_NODISCARD const ICMP_ECHO_REPLY* GetICMP_ECHO_REPLY() noexcept;

	//Member variables
	SOCKADDR_IN Address; //The IP address of the replier
	unsigned long RTT; //Round Trip time in Milliseconds
	unsigned long EchoReplyStatus; //here will be status of the last ping if successful
	std::vector<BYTE> Reply; //The buffer for the ICMP_ECHO_REPLY / ICMPV6_ECHO_REPLY
};


struct CPING_EXT_CLASS CPingReplyv6
{
	//Constructors / Destructors
	CPingReplyv6() noexcept;
	CPingReplyv6(const CPingReplyv6&) = delete;
	CPingReplyv6(CPingReplyv6&&) = delete;
	~CPingReplyv6() = default;

	//Methods
	CPingReplyv6& operator=(const CPingReplyv6&) = delete;
	CPingReplyv6& operator=(CPingReplyv6&&) = delete;
	_NODISCARD const ICMPV6_ECHO_REPLY* GetICMPV6_ECHO_REPLY() noexcept;

	//Member variables
	SOCKADDR_IN6 Address; //The IP address of the replier
	unsigned long RTT; //Round Trip time in Milliseconds
	unsigned long EchoReplyStatus; //here will be status of the last ping if successful
	std::vector<BYTE> Reply; //The buffer for the ICMP_ECHO_REPLY / ICMPV6_ECHO_REPLY
};


class CPING_EXT_CLASS CPing
{
public:
	//Constructors / Destructors
	CPing() = default;
	CPing(const CPing&) = delete;
	CPing(CPing&&) = delete;
	virtual ~CPing() = default;

	//Methods
	CPing& operator=(const CPing&) = delete;
	CPing& operator=(CPing&&) = delete;
	bool PingUsingICMPv4(_In_z_ LPCTSTR pszHostName, _Inout_ CPingReplyv4& pr, _In_ UCHAR nTTL = 10, _In_ DWORD dwTimeout = 5000, _In_ WORD wDataSize = 32, _In_ UCHAR nTOS = 0, _In_ bool bDontFragment = false, _In_ bool bFlagReverse = false, _In_opt_z_ LPCTSTR pszLocalBoundAddress = nullptr) const;
	bool PingUsingICMPv6(_In_z_ LPCTSTR pszHostName, _Inout_ CPingReplyv6& pr, _In_ UCHAR nTTL = 10, _In_ DWORD dwTimeout = 5000, _In_ WORD wDataSize = 32, _In_ UCHAR nTOS = 0, _In_ bool bDontFragment = false, _In_ bool bFlagReverse = false, _In_opt_z_ LPCTSTR pszLocalBoundAddress = nullptr) const;

protected:
	//Methods
	virtual void FillIcmpData(_Out_writes_bytes_(dwRequestSize) BYTE* pRequestData, _In_ DWORD dwRequestSize) const;
};

#endif //#ifndef __PING_H__
