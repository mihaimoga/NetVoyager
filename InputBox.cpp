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

/**
 * @file InputBox.cpp
 * @brief Implements the CInputBox dialog for entering a hostname and network operation options.
 */

#include "pch.h"
#include "NetVoyager.h"
#include "InputBox.h"

IMPLEMENT_DYNAMIC(CInputBox, CDialogEx)

/**
 * @brief Constructs a CInputBox dialog.
 * @param pParent Parent window pointer.
 */
CInputBox::CInputBox(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INPUTBOX, pParent)
	, m_strHostname(_T(""))
	, m_bResolveAddressesToHostnames(FALSE)
	, m_bIPv6(FALSE)
{
}

/**
 * @brief Destructor.
 */
CInputBox::~CInputBox()
{
}

/**
 * @brief Exchanges data between the dialog controls and member variables.
 * @param pDX Pointer to the data exchange object.
 */
void CInputBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_HOSTNAME, m_strHostname);
	DDX_Check(pDX, IDC_RESOLVE_ADDRESSES, m_bResolveAddressesToHostnames);
	DDX_Check(pDX, IDC_FORCE_IPV6, m_bIPv6);
}

BEGIN_MESSAGE_MAP(CInputBox, CDialogEx)
END_MESSAGE_MAP()

// CInputBox message handlers
