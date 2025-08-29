/* Copyright (C) 2025-2026 Stefan-Mihai MOGA
This file is part of NetVoyager application developed by Stefan-Mihai MOGA.
Diagnose network issues instantly with real-time ping and traceroute tools in a sleek, user-friendly interface.

NetVoyager is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

NetVoyager is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
NetVoyager. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// NetVoyager.h : main header file for the NetVoyager application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CNetVoyagerApp:
// See NetVoyager.cpp for the implementation of this class
//

class CNetVoyagerApp : public CWinAppEx
{
public:
	CNetVoyagerApp() noexcept;

	// Our specific command line options
	CString m_sHostToResolve;
	CString m_sLocalBoundAddress;
	bool m_bResolveAddressesToHostnames;
	bool m_bPingTillStopped;
	int m_nRequestsToSend;
	UCHAR m_nTTL;
	UCHAR m_nTOS;
	WORD m_wDataRequestSize;
	DWORD m_dwTimeout;
	bool m_bDontFragment;
	bool m_bIPv6;
	UCHAR m_nHopCount;
	UCHAR m_nPings;

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	static CString AddressToString(const SOCKADDR* pSockAddr, int nSockAddrLen, int nFlags, UINT* pnSocketPort);
	static CString GetErrorMessage(DWORD dwError);
	static CString RTTAsString(DWORD dwRTT);
	static CString GetIpErrorString(IP_STATUS dwError);

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CNetVoyagerApp theApp;
