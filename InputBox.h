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

#pragma once

/**
 * @class CInputBox
 * @brief Dialog for entering a hostname and network operation options.
 */
class CInputBox : public CDialogEx
{
	DECLARE_DYNAMIC(CInputBox)

public:
	CInputBox(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CInputBox();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUTBOX };
#endif

public:
	CString m_strHostname;                  ///< Target hostname or IP address
	BOOL m_bResolveAddressesToHostnames;    ///< If TRUE, resolve IP addresses to hostnames
	BOOL m_bIPv6;                           ///< If TRUE, use IPv6 instead of IPv4

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
