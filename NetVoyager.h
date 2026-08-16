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
	CString m_sHostToResolve;           // Hostname or IP address to ping/trace
	CString m_sLocalBoundAddress;       // Local interface address to bind the socket to (empty = default)
	bool m_bResolveAddressesToHostnames; // When true, reverse-resolve IP addresses to hostnames in results
	bool m_bPingTillStopped;            // When true, ping continuously until the user stops; otherwise use m_nRequestsToSend
	int m_nRequestsToSend;              // Number of ICMP echo requests to send (used when m_bPingTillStopped is false)
	UCHAR m_nTTL;                       // Time-To-Live value set on outgoing packets (limits hop count)
	UCHAR m_nTOS;                       // Type-Of-Service / DSCP byte for QoS prioritisation
	WORD m_wDataRequestSize;            // Payload size (bytes) of each ICMP echo request
	DWORD m_dwTimeout;                  // Per-request timeout in milliseconds before treating as a loss
	bool m_bDontFragment;               // When true, sets the DF (Don't Fragment) bit on IP packets
	bool m_bIPv6;                       // When true, use ICMPv6 / IPv6; otherwise use ICMPv4 / IPv4
	UCHAR m_nHopCount;                  // Maximum number of hops (TTL limit) for traceroute
	UCHAR m_nPings;                     // Number of probes sent per hop during traceroute

// Overrides
public:
	virtual BOOL InitInstance();    // Application startup: initialises controls, sockets, OLE and the document template
	virtual int ExitInstance();     // Application shutdown: releases OLE resources and calls base cleanup

// Implementation
	// Converts a socket address structure to a human-readable IP address string
	static CString AddressToString(const SOCKADDR* pSockAddr, int nSockAddrLen, int nFlags, UINT* pnSocketPort);
	// Returns the system error message for a Win32 error code
	static CString GetErrorMessage(DWORD dwError);
	// Formats a round-trip time value as a string (e.g. "<1ms" or "25ms")
	static CString RTTAsString(DWORD dwRTT);
	// Returns the descriptive string for an IP_STATUS error code from the ICMP API
	static CString GetIpErrorString(IP_STATUS dwError);

	virtual void PreLoadState();    // Called before state is loaded; registers the edit context menu
	virtual void LoadCustomState(); // Hook for restoring additional persisted application state
	virtual void SaveCustomState(); // Hook for persisting additional application state

	afx_msg void OnAppAbout();      // Displays the About dialog in response to Help > About
	DECLARE_MESSAGE_MAP()
};

extern CNetVoyagerApp theApp;
