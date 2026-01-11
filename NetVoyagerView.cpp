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

// NetVoyagerView.cpp : implementation of the CNetVoyagerView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "NetVoyager.h"
#endif

#include "NetVoyagerDoc.h"
#include "NetVoyagerView.h"
#include "InputBox.h"
#include "ping.h"
#include "tracer.h"
#include <filesystem>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CStringA W2UTF8(_In_NLS_string_(nLength) const wchar_t* pszText, _In_ int nLength)
{
	// First call the function to determine how much space we need to allocate
	int nUTF8Length{ WideCharToMultiByte(CP_UTF8, 0, pszText, nLength, nullptr, 0, nullptr, nullptr) };

	// If the calculated length is zero, then ensure we have at least room for a null terminator
	if (nUTF8Length == 0)
		nUTF8Length = 1;

	// Now recall with the buffer to get the converted text
	CStringA sUTF;
#pragma warning(suppress: 26429)
	char* const pszUTF8Text{ sUTF.GetBuffer(nUTF8Length + 1) }; // include an extra byte because we may be null terminating the string ourselves
	int nCharsWritten{ WideCharToMultiByte(CP_UTF8, 0, pszText, nLength, pszUTF8Text, nUTF8Length, nullptr, nullptr) };

	// Ensure we null terminate the text if WideCharToMultiByte doesn't do it for us
	if (nLength != -1)
	{
#pragma warning(suppress: 26477 26496)
		ATLASSUME(nCharsWritten <= nUTF8Length);
#pragma warning(suppress: 26481)
		pszUTF8Text[nCharsWritten] = '\0';
	}
	sUTF.ReleaseBuffer();

	return sUTF;
}

CStringW UTF82W(_In_NLS_string_(nLength) const char* pszText, _In_ int nLength)
{
	// First call the function to determine how much space we need to allocate
	int nWideLength{ MultiByteToWideChar(CP_UTF8, 0, pszText, nLength, nullptr, 0) };

	// If the calculated length is zero, then ensure we have at least room for a null terminator
	if (nWideLength == 0)
		nWideLength = 1;

	//Now recall with the buffer to get the converted text
	CStringW sWideString;
#pragma warning(suppress: 26429)
	wchar_t* pszWText{ sWideString.GetBuffer(nWideLength + 1) }; //include an extra byte because we may be null terminating the string ourselves
	int nCharsWritten{ MultiByteToWideChar(CP_UTF8, 0, pszText, nLength, pszWText, nWideLength) };

	//Ensure we null terminate the text if MultiByteToWideChar doesn't do it for us
	if (nLength != -1)
	{
#pragma warning(suppress: 26477 26496)
		ATLASSUME(nCharsWritten <= nWideLength);
#pragma warning(suppress: 26481)
		pszWText[nCharsWritten] = '\0';
	}
	sWideString.ReleaseBuffer();

	return sWideString;
}

// CNetVoyagerView

IMPLEMENT_DYNCREATE(CNetVoyagerView, CView)

BEGIN_MESSAGE_MAP(CNetVoyagerView, CView)
	// Standard printing commands
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_PING, &CNetVoyagerView::OnPing)
	ON_COMMAND(ID_TRACE_ROUTE, &CNetVoyagerView::OnTraceRoute)
	ON_UPDATE_COMMAND_UI(ID_PING, &CNetVoyagerView::OnUpdatePing)
	ON_UPDATE_COMMAND_UI(ID_TRACE_ROUTE, &CNetVoyagerView::OnUpdateTraceRoute)
END_MESSAGE_MAP()

// CNetVoyagerView construction/destruction

CNetVoyagerView::CNetVoyagerView() noexcept
{
	m_nThreadID = 0;
}

CNetVoyagerView::~CNetVoyagerView()
{
}

BOOL CNetVoyagerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CNetVoyagerView drawing

void CNetVoyagerView::OnDraw(CDC* /*pDC*/)
{
	CNetVoyagerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

// CNetVoyagerView diagnostics

#ifdef _DEBUG
void CNetVoyagerView::AssertValid() const
{
	CView::AssertValid();
}

void CNetVoyagerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CNetVoyagerDoc* CNetVoyagerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNetVoyagerDoc)));
	return (CNetVoyagerDoc*)m_pDocument;
}
#endif //_DEBUG

// CNetVoyagerView message handlers

void CNetVoyagerView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	this->ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE, 0, 0);
	this->ModifyStyle(WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME | WS_BORDER, 0, 0);

	m_pWebBrowser = std::make_unique<CWebBrowser>();

	if (m_pWebBrowser != nullptr)
	{
		CRect rectClient;
		GetClientRect(rectClient);

		m_pWebBrowser->CreateAsync(
			WS_VISIBLE | WS_CHILD,
			rectClient,
			this,
			1,
			[this]() {
				m_pWebBrowser->SetParentView(this);
				m_pWebBrowser->DisablePopups();
				m_pWebBrowser->Navigate(L"https://www.ip-address.ro/status.html", nullptr);

				m_pWebBrowser->RegisterCallback(CWebBrowser::CallbackType::TitleChanged, [this]() {
					CString title = m_pWebBrowser->GetTitle();

					if (GetDocument() != nullptr)
					{
						GetDocument()->SetTitle(title);
					}

					AfxGetMainWnd()->SetWindowText(title);
				});
			});
	}
}

void CNetVoyagerView::OnDestroy()
{
	m_pWebBrowser.reset();

	CView::OnDestroy();
}

void CNetVoyagerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CRect rectClient;
	GetClientRect(rectClient);

	if (m_pWebBrowser != nullptr)
		m_pWebBrowser->Resize(cx, cy);
}

bool g_bThreadRunning = false;
TCHAR g_lpszOutputString[0x1000] = { 0, };
DWORD WINAPI PING_ThreadProc(LPVOID lpParam)
{
	bool bSuccess{ false };
	const CPing p;
	CPingReplyv4 prv4;
	CPingReplyv6 prv6;
	int nRequestsSent{ 0 };

	CNetVoyagerView* pNetVoyagerView = reinterpret_cast<CNetVoyagerView*>(lpParam);
	ASSERT(pNetVoyagerView != nullptr);

	_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Pinging %s with %u bytes of data"), theApp.m_sHostToResolve.GetString(), theApp.m_wDataRequestSize);
	TRACE(_T("%s\n"), g_lpszOutputString);
	pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
	// pNetVoyagerView->m_pWebBrowser->Reload();

	while (g_bThreadRunning)
	{
		if (theApp.m_bIPv6)
		{
			if (theApp.m_sLocalBoundAddress.IsEmpty())
#pragma warning(suppress: 26486)
				bSuccess = p.PingUsingICMPv6(theApp.m_sHostToResolve, prv6, theApp.m_nTTL, theApp.m_dwTimeout,
					theApp.m_wDataRequestSize, theApp.m_nTOS, theApp.m_bDontFragment);
			else
#pragma warning(suppress: 26486)
				bSuccess = p.PingUsingICMPv6(theApp.m_sHostToResolve, prv6, theApp.m_nTTL, theApp.m_dwTimeout,
					theApp.m_wDataRequestSize, theApp.m_nTOS, theApp.m_bDontFragment, false, theApp.m_sLocalBoundAddress);
		}
		else
		{
			if (theApp.m_sLocalBoundAddress.IsEmpty())
#pragma warning(suppress: 26486)
				bSuccess = p.PingUsingICMPv4(theApp.m_sHostToResolve, prv4, theApp.m_nTTL, theApp.m_dwTimeout,
					theApp.m_wDataRequestSize, theApp.m_nTOS, theApp.m_bDontFragment);
			else
#pragma warning(suppress: 26486)
				bSuccess = p.PingUsingICMPv4(theApp.m_sHostToResolve, prv4, theApp.m_nTTL, theApp.m_dwTimeout,
					theApp.m_wDataRequestSize, theApp.m_nTOS, theApp.m_bDontFragment, false, theApp.m_sLocalBoundAddress);
		}

		++nRequestsSent;
		if (bSuccess)
		{
#pragma warning(suppress: 26490)
			const SOCKADDR* pAddress{ theApp.m_bIPv6 ? reinterpret_cast<SOCKADDR*>(&prv6) : reinterpret_cast<SOCKADDR*>(&prv4) };
			const int nAddressLen{ theApp.m_bIPv6 ? static_cast<int>(sizeof(prv6)) : static_cast<int>(sizeof(prv4)) };
			const unsigned long& nRTT{ theApp.m_bIPv6 ? prv6.RTT : prv4.RTT };
			const unsigned long& nEchoReplyStatus{ theApp.m_bIPv6 ? prv6.EchoReplyStatus : prv4.EchoReplyStatus };

			CString sHost;
			if (theApp.m_bResolveAddressesToHostnames)
				sHost = theApp.AddressToString(pAddress, nAddressLen, 0, nullptr);
			if (sHost.GetLength())
			{
				if (nEchoReplyStatus == IP_SUCCESS)
#pragma warning(suppress: 26472)
					_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Reply from %s [%s], bytes=%d, time=%s TTL=%d"), theApp.AddressToString(pAddress, nAddressLen, NI_NUMERICHOST, nullptr).GetString(), sHost.GetString(), static_cast<int>(theApp.m_wDataRequestSize), theApp.RTTAsString(nRTT).GetString(), static_cast<int>(theApp.m_nTTL));
				else
					_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Reply from %s [%s]: %s"), theApp.AddressToString(pAddress, nAddressLen, NI_NUMERICHOST, nullptr).GetString(), sHost.GetString(), theApp.GetIpErrorString(nEchoReplyStatus).GetString());
				TRACE(_T("%s\n"), g_lpszOutputString);
				pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
				// pNetVoyagerView->m_pWebBrowser->Reload();
			}
			else
			{
				if (nEchoReplyStatus == IP_SUCCESS)
#pragma warning(suppress: 26472)
					_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Reply from %s, bytes=%d, time=%s TTL=%d"), theApp.AddressToString(pAddress, nAddressLen, NI_NUMERICHOST, nullptr).GetString(), static_cast<int>(theApp.m_wDataRequestSize), theApp.RTTAsString(nRTT).GetString(), static_cast<int>(theApp.m_nTTL));
				else
					_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Reply from %s: %s"), theApp.AddressToString(pAddress, nAddressLen, NI_NUMERICHOST, nullptr).GetString(), theApp.GetIpErrorString(nEchoReplyStatus).GetString());
				TRACE(_T("%s\n"), g_lpszOutputString);
				pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
				// pNetVoyagerView->m_pWebBrowser->Reload();
			}
		}
		else
		{
			const DWORD dwError{ GetLastError() };
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("%s"), theApp.GetErrorMessage(dwError).GetString());
			TRACE(_T("%s\n"), g_lpszOutputString);
			pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
			// pNetVoyagerView->m_pWebBrowser->Reload();
		}

		// Prepare for the next loop around?
		if (!theApp.m_bPingTillStopped)
		{
			if (nRequestsSent == theApp.m_nRequestsToSend)
				break;
		}
	}

	return 0;
}

// Class derived to implement Trace Route
class CMyTraceRoute : public CTraceRoute
{
	bool OnSingleHostResult(int nHostNum, const CHostTraceMultiReplyv4& htmr) override;
	bool OnSingleHostResult(int nHostNum, const CHostTraceMultiReplyv6& htmr) override;
public:
	CNetVoyagerView* m_pNetVoyagerView{ nullptr };
};

bool CMyTraceRoute::OnSingleHostResult(int nHostNum, const CHostTraceMultiReplyv4& htmr)
{
	if (htmr.dwError == 0)
	{
		CString sHost;
		if (theApp.m_bResolveAddressesToHostnames)
#pragma warning(suppress: 26490)
			sHost = theApp.AddressToString(reinterpret_cast<const SOCKADDR*>(&htmr.Address), sizeof(htmr.Address), NI_NAMEREQD, nullptr);
#pragma warning(suppress: 26490)
		CString sIPAddress{ theApp.AddressToString(reinterpret_cast<const SOCKADDR*>(&htmr.Address), sizeof(htmr.Address), NI_NUMERICHOST, nullptr) };

		if (sHost.GetLength())
		{
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t%s\t%s\t%s\t%s [%s]"), nHostNum, theApp.RTTAsString(htmr.minRTT).GetString(), theApp.RTTAsString(htmr.avgRTT).GetString(),
				theApp.RTTAsString(htmr.maxRTT).GetString(), sHost.GetString(), sIPAddress.GetString());
			TRACE(_T("%s\n"), g_lpszOutputString);
			m_pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
			// pNetVoyagerView->m_pWebBrowser->Reload();
		}
		else
		{
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t%s\t%s\t%s\t%s"), nHostNum, theApp.RTTAsString(htmr.minRTT).GetString(), theApp.RTTAsString(htmr.avgRTT).GetString(),
				theApp.RTTAsString(htmr.maxRTT).GetString(), sIPAddress.GetString());
			TRACE(_T("%s\n"), g_lpszOutputString);
			m_pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
			// pNetVoyagerView->m_pWebBrowser->Reload();
		}
	}
	else
	{
		if (htmr.dwError == ERROR_TIMEOUT)
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t*\t*\t*\tRequest timed out."), nHostNum);
		else
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t*\t*\t*\tError:%s"), nHostNum, theApp.GetErrorMessage(htmr.dwError).GetString());
		TRACE(_T("%s\n"), g_lpszOutputString);
		m_pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
		// pNetVoyagerView->m_pWebBrowser->Reload();
	}
	return true;
}

bool CMyTraceRoute::OnSingleHostResult(int nHostNum, const CHostTraceMultiReplyv6& htmr)
{
	if (htmr.dwError == 0)
	{
		CString sHost;
		if (theApp.m_bResolveAddressesToHostnames)
#pragma warning(suppress: 26490)
			sHost = theApp.AddressToString(reinterpret_cast<const SOCKADDR*>(&htmr.Address), sizeof(htmr.Address), NI_NAMEREQD, nullptr);
#pragma warning(suppress: 26490)
		CString sIPAddress{ theApp.AddressToString(reinterpret_cast<const SOCKADDR*>(&htmr.Address), sizeof(htmr.Address), NI_NUMERICHOST, nullptr) };

		if (sHost.GetLength())
		{
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t%s\t%s\t%s\t%s [%s]"), nHostNum, theApp.RTTAsString(htmr.minRTT).GetString(), theApp.RTTAsString(htmr.avgRTT).GetString(),
				theApp.RTTAsString(htmr.maxRTT).GetString(), sHost.GetString(), sIPAddress.GetString());
			TRACE(_T("%s\n"), g_lpszOutputString);
			m_pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
			// pNetVoyagerView->m_pWebBrowser->Reload();
		}
		else
		{
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t%s\t%s\t%s\t%s"), nHostNum, theApp.RTTAsString(htmr.minRTT).GetString(), theApp.RTTAsString(htmr.avgRTT).GetString(),
				theApp.RTTAsString(htmr.maxRTT).GetString(), sIPAddress.GetString());
			TRACE(_T("%s\n"), g_lpszOutputString);
			m_pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
			// pNetVoyagerView->m_pWebBrowser->Reload();
		}
	}
	else
	{
		if (htmr.dwError == ERROR_TIMEOUT)
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t*\t*\t*\tRequest timed out."), nHostNum);
		else
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t*\t*\t*\tError:%s"), nHostNum, theApp.GetErrorMessage(htmr.dwError).GetString());
		TRACE(_T("%s\n"), g_lpszOutputString);
		m_pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
		// pNetVoyagerView->m_pWebBrowser->Reload();
	}
	return true;
}

DWORD WINAPI TRACE_ThreadProc(LPVOID lpParam)
{
	CNetVoyagerView* pNetVoyagerView = reinterpret_cast<CNetVoyagerView*>(lpParam);
	ASSERT(pNetVoyagerView != nullptr);

	// Print the intro comment
#pragma warning(suppress: 26472)
	_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Tracing route to %s over a maximum of %d hops:"), theApp.m_sHostToResolve.GetString(), static_cast<int>(theApp.m_nHopCount));
	TRACE(_T("%s\n"), g_lpszOutputString);
	pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
	// pNetVoyagerView->m_pWebBrowser->Reload();

	// Do the actual trace route
	CTraceRoute::CReplyv4 trrv4;
	CTraceRoute::CReplyv6 trrv6;
	CMyTraceRoute tr;
	tr.m_pNetVoyagerView = pNetVoyagerView;
	if (theApp.m_bIPv6)
	{
		if (tr.Tracev6(theApp.m_sHostToResolve, trrv6, theApp.m_nHopCount, theApp.m_dwTimeout, theApp.m_nPings, 32, 0, false, false, theApp.m_sLocalBoundAddress.GetLength() ? theApp.m_sLocalBoundAddress : nullptr))
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Trace complete."));
		else
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("%s"), theApp.GetErrorMessage(GetLastError()).GetString());
	}
	else
	{
		if (tr.Tracev4(theApp.m_sHostToResolve, trrv4, theApp.m_nHopCount, theApp.m_dwTimeout, theApp.m_nPings, 32, 0, false, false, theApp.m_sLocalBoundAddress.GetLength() ? theApp.m_sLocalBoundAddress : nullptr))
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Trace complete."));
		else
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("%s"), theApp.GetErrorMessage(GetLastError()).GetString());
	}
	TRACE(_T("%s\n"), g_lpszOutputString);
	pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
	// pNetVoyagerView->m_pWebBrowser->Reload();

	return 0;
}

bool WaitWithMessageLoop(HANDLE hEvent, DWORD dwTimeout)
{
	DWORD dwRet;
	MSG msg;
	hEvent = hEvent ? hEvent : CreateEvent(NULL, FALSE, FALSE, NULL);

	while (true)
	{
		dwRet = MsgWaitForMultipleObjects(1, &hEvent, FALSE, dwTimeout, QS_ALLINPUT);
		if (dwRet == WAIT_OBJECT_0)
			return true;
		if (dwRet != WAIT_OBJECT_0 + 1)
			break;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (hEvent && (WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0))
				return true;
		}
	}
	return false;
}

void CNetVoyagerView::OnPing()
{
	if (!g_bThreadRunning)
	{
		CInputBox pInputBox(this);
		if (pInputBox.DoModal() == IDOK)
		{
			g_bThreadRunning = true;
			theApp.m_sHostToResolve = pInputBox.m_strHostname;

			if (m_arrDocumentText.size() > 0)
				m_arrDocumentText.erase(m_arrDocumentText.begin(), m_arrDocumentText.end());
			SetDocumentPath(NewDocumentPath());
			ExportDocument();

			HANDLE hThread = ::CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)PING_ThreadProc,
				this,
				0,
				&m_nThreadID);

			VERIFY(WaitWithMessageLoop(hThread, INFINITE));

			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("file:///%s"), GetDocumentPath().c_str());
			CString strURL(g_lpszOutputString);
			strURL.Replace(_T("\\"), _T("/"));
			m_pWebBrowser->Navigate(g_lpszOutputString, nullptr);

			g_bThreadRunning = false;
		}
	}
}

void CNetVoyagerView::OnUpdatePing(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!g_bThreadRunning);
}

void CNetVoyagerView::OnTraceRoute()
{
	if (!g_bThreadRunning)
	{
		CInputBox pInputBox(this);
		if (pInputBox.DoModal() == IDOK)
		{
			g_bThreadRunning = true;
			theApp.m_sHostToResolve = pInputBox.m_strHostname;

			if (m_arrDocumentText.size() > 0)
				m_arrDocumentText.erase(m_arrDocumentText.begin(), m_arrDocumentText.end());
			SetDocumentPath(NewDocumentPath());
			ExportDocument();

			HANDLE hThread = ::CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)TRACE_ThreadProc,
				this,
				0,
				&m_nThreadID);

			VERIFY(WaitWithMessageLoop(hThread, INFINITE));

			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("file:///%s"), GetDocumentPath().c_str());
			CString strURL(g_lpszOutputString);
			strURL.Replace(_T("\\"), _T("/"));
			m_pWebBrowser->Navigate(g_lpszOutputString, nullptr);

			g_bThreadRunning = false;
		}
	}
}

void CNetVoyagerView::OnUpdateTraceRoute(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!g_bThreadRunning);
}

const std::wstring CNetVoyagerView::NewDocumentPath()
{
	TCHAR lpszTempPath[_MAX_PATH + 1] = { 0, };
	DWORD nLength = GetTempPath(_MAX_PATH, lpszTempPath);
	if (nLength > 0)
	{
		TCHAR lpszFilePath[_MAX_PATH + 1] = { 0, };
		nLength = GetTempFileName(lpszTempPath, L"NET", 0, lpszFilePath);
		if (nLength > 0)
		{
			CString strFileName = lpszFilePath;
			strFileName.Replace(_T(".tmp"), _T(".html"));
			return strFileName.GetString();
		}
	}
	return _T("");
}

void CNetVoyagerView::ExportDocument()
{
	// TRACE(_T("%s\n"), GetDocumentPath().c_str());
	std::ofstream pHtmlFile(GetDocumentPath().c_str(), std::ofstream::out);
	if (pHtmlFile.is_open())
	{
		pHtmlFile << "<!DOCTYPE html>\n";
		pHtmlFile << "<html lang=\"en\">\n";
		pHtmlFile << "<head>\n";
		pHtmlFile << "<meta charset=\"UTF-8\">\n";
		pHtmlFile << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
		pHtmlFile << "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.7/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-LN+7fdVzj6u52u30Kp6M/trliBMCMKTyK833zpbD+pXdCLuTusPj697FH4R/5mcr\" crossorigin=\"anonymous\">\n";
		pHtmlFile << "</head>\n";
		pHtmlFile << "<body>\n";
		pHtmlFile << "<div class=\"container\">\n";
		pHtmlFile << "<div id=\"row\">\n";
		// for (int i = static_cast<int>(m_arrDocumentText.size()) - 1; i >= 0; i--)
		for (int i = 0; i < static_cast<int>(m_arrDocumentText.size()); i++)
			pHtmlFile << m_arrDocumentText[i] << "<br>\n";
		pHtmlFile << "</div>\n";
		pHtmlFile << "</div>\n";
		pHtmlFile << "<script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.7/dist/js/bootstrap.bundle.min.js\" integrity=\"sha384-ndDqU0Gzau9qJ1lfW4pNLlhNTkCfHzAVBReH9diLvGRem5+R9g2FzA8ZGN954O5Q\" crossorigin=\"anonymous\"></script>\n";
		pHtmlFile << "</body>\n";
		pHtmlFile << "</html>\n";
		pHtmlFile.close();
	}
}
