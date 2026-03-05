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
#include "PleaseWait.h"
#include "ping.h"
#include "tracer.h"
#include <filesystem>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Converts a wide character string to UTF-8 encoded string
 * @param pszText Pointer to the wide character string to convert
 * @param nLength Length of the input string, or -1 for null-terminated strings
 * @return CStringA containing the UTF-8 encoded string
 */
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
	// Get a writable buffer from CStringA with extra space for null terminator
	char* const pszUTF8Text{ sUTF.GetBuffer(nUTF8Length + 1) };
	// Perform the actual conversion from wide char to UTF-8
	int nCharsWritten{ WideCharToMultiByte(CP_UTF8, 0, pszText, nLength, pszUTF8Text, nUTF8Length, nullptr, nullptr) };

	// Ensure we null terminate the text if WideCharToMultiByte doesn't do it for us
	if (nLength != -1)
	{
#pragma warning(suppress: 26477 26496)
		ATLASSUME(nCharsWritten <= nUTF8Length);
#pragma warning(suppress: 26481)
		// Manually add null terminator for counted strings
		pszUTF8Text[nCharsWritten] = '\0';
	}
	// Release the buffer and return the result
	sUTF.ReleaseBuffer();

	return sUTF;
}

/**
 * @brief Converts a UTF-8 encoded string to wide character string
 * @param pszText Pointer to the UTF-8 encoded string to convert
 * @param nLength Length of the input string, or -1 for null-terminated strings
 * @return CStringW containing the wide character string
 */
CStringW UTF82W(_In_NLS_string_(nLength) const char* pszText, _In_ int nLength)
{
	// First call the function to determine how much space we need to allocate
	int nWideLength{ MultiByteToWideChar(CP_UTF8, 0, pszText, nLength, nullptr, 0) };

	// If the calculated length is zero, then ensure we have at least room for a null terminator
	if (nWideLength == 0)
		nWideLength = 1;

	// Now recall with the buffer to get the converted text
	CStringW sWideString;
#pragma warning(suppress: 26429)
	// Get a writable buffer from CStringW with extra space for null terminator
	wchar_t* pszWText{ sWideString.GetBuffer(nWideLength + 1) };
	// Perform the actual conversion from UTF-8 to wide char
	int nCharsWritten{ MultiByteToWideChar(CP_UTF8, 0, pszText, nLength, pszWText, nWideLength) };

	// Ensure we null terminate the text if MultiByteToWideChar doesn't do it for us
	if (nLength != -1)
	{
#pragma warning(suppress: 26477 26496)
		ATLASSUME(nCharsWritten <= nWideLength);
#pragma warning(suppress: 26481)
		// Manually add null terminator for counted strings
		pszWText[nCharsWritten] = '\0';
	}
	// Release the buffer and return the result
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

/**
 * @brief Default constructor for CNetVoyagerView
 */
CNetVoyagerView::CNetVoyagerView() noexcept
{
	// Initialize thread ID to zero (no thread running)
	m_nThreadID = 0;
}

/**
 * @brief Destructor for CNetVoyagerView
 */
CNetVoyagerView::~CNetVoyagerView()
{
}

/**
 * @brief Modifies the window class or styles before window creation
 * @param cs Reference to CREATESTRUCT that can be modified
 * @return TRUE if successful, FALSE otherwise
 */
BOOL CNetVoyagerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CNetVoyagerView drawing

/**
 * @brief Draws the view
 * @param pDC Pointer to the device context (unused in this implementation)
 */
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

/**
 * @brief Called during the initial update of the view
 * Initializes the web browser control and sets up callbacks
 */
void CNetVoyagerView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// Remove window frame styles to create a borderless embedded view
	this->ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE, 0, 0);
	this->ModifyStyle(WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME | WS_BORDER, 0, 0);

	// Create the Edge WebView2 browser control
	m_pWebBrowser = std::make_unique<CWebBrowser>();

	if (m_pWebBrowser != nullptr)
	{
		CRect rectClient;
		GetClientRect(rectClient);

		// Asynchronously create the browser control
		m_pWebBrowser->CreateAsync(
			WS_VISIBLE | WS_CHILD,
			rectClient,
			this,
			1,
			[this]() {
				// Browser initialization callback
				m_pWebBrowser->SetParentView(this);
				// Disable popup windows for security
				m_pWebBrowser->DisablePopups();
				// Navigate to default status page
				m_pWebBrowser->Navigate(L"https://www.ip-address.ro/status.html", nullptr);

				// Register callback to update window title when page title changes
				m_pWebBrowser->RegisterCallback(CWebBrowser::CallbackType::TitleChanged, [this]() {
					CString title = m_pWebBrowser->GetTitle();

					// Update document title if available
					if (GetDocument() != nullptr)
					{
						GetDocument()->SetTitle(title);
					}

					// Update main window title
					AfxGetMainWnd()->SetWindowText(title);
				});
			});
	}
}

/**
 * @brief Called when the view is being destroyed
 * Cleans up the web browser control
 */
void CNetVoyagerView::OnDestroy()
{
	// Release the web browser control before the view is destroyed
	m_pWebBrowser.reset();

	CView::OnDestroy();
}

/**
 * @brief Handles window resizing
 * @param nType Type of resizing requested
 * @param cx New width of the client area
 * @param cy New height of the client area
 */
void CNetVoyagerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CRect rectClient;
	GetClientRect(rectClient);

	// Resize the browser control to match the view's client area
	if (m_pWebBrowser != nullptr)
		m_pWebBrowser->Resize(cx, cy);
}

// Global flag to track if a network operation thread is currently running
bool g_bThreadRunning = false;
// Global buffer for formatting output strings from thread procedures
TCHAR g_lpszOutputString[0x1000] = { 0, };

/**
 * @brief Thread procedure for executing ping operations
 * @param lpParam Pointer to CNetVoyagerView instance
 * @return Thread exit code (always 0)
 */
DWORD WINAPI PING_ThreadProc(LPVOID lpParam)
{
	bool bSuccess{ false };
	const CPing p;
	CPingReplyv4 prv4;
	CPingReplyv6 prv6;
	int nRequestsSent{ 0 };

	CNetVoyagerView* pNetVoyagerView = reinterpret_cast<CNetVoyagerView*>(lpParam);
	ASSERT(pNetVoyagerView != nullptr);

	// Display initial ping header message
	_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Pinging <strong>%s</strong> with %u bytes of data"), theApp.m_sHostToResolve.GetString(), theApp.m_wDataRequestSize);
	TRACE(_T("%s\n"), g_lpszOutputString);
	pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());

	// Main ping loop - continues until stopped by user or request count reached
	while (g_bThreadRunning)
	{
		// Choose IPv4 or IPv6 ping based on configuration
		if (theApp.m_bIPv6)
		{
			if (theApp.m_sLocalBoundAddress.IsEmpty())
#pragma warning(suppress: 26486)
				// Ping using IPv6 with default local address
				bSuccess = p.PingUsingICMPv6(theApp.m_sHostToResolve, prv6, theApp.m_nTTL, theApp.m_dwTimeout,
					theApp.m_wDataRequestSize, theApp.m_nTOS, theApp.m_bDontFragment);
			else
#pragma warning(suppress: 26486)
				// Ping using IPv6 with specific local bound address
				bSuccess = p.PingUsingICMPv6(theApp.m_sHostToResolve, prv6, theApp.m_nTTL, theApp.m_dwTimeout,
					theApp.m_wDataRequestSize, theApp.m_nTOS, theApp.m_bDontFragment, false, theApp.m_sLocalBoundAddress);
		}
		else
		{
			if (theApp.m_sLocalBoundAddress.IsEmpty())
#pragma warning(suppress: 26486)
				// Ping using IPv4 with default local address
				bSuccess = p.PingUsingICMPv4(theApp.m_sHostToResolve, prv4, theApp.m_nTTL, theApp.m_dwTimeout,
					theApp.m_wDataRequestSize, theApp.m_nTOS, theApp.m_bDontFragment);
			else
#pragma warning(suppress: 26486)
				// Ping using IPv4 with specific local bound address
				bSuccess = p.PingUsingICMPv4(theApp.m_sHostToResolve, prv4, theApp.m_nTTL, theApp.m_dwTimeout,
					theApp.m_wDataRequestSize, theApp.m_nTOS, theApp.m_bDontFragment, false, theApp.m_sLocalBoundAddress);
		}

		++nRequestsSent;
		if (bSuccess)
		{
#pragma warning(suppress: 26490)
			// Extract reply information based on IP version
			const SOCKADDR* pAddress{ theApp.m_bIPv6 ? reinterpret_cast<SOCKADDR*>(&prv6) : reinterpret_cast<SOCKADDR*>(&prv4) };
			const int nAddressLen{ theApp.m_bIPv6 ? static_cast<int>(sizeof(prv6)) : static_cast<int>(sizeof(prv4)) };
			const unsigned long& nRTT{ theApp.m_bIPv6 ? prv6.RTT : prv4.RTT };
			const unsigned long& nEchoReplyStatus{ theApp.m_bIPv6 ? prv6.EchoReplyStatus : prv4.EchoReplyStatus };

			CString sHost;
			// Attempt to resolve IP address to hostname if enabled
			if (theApp.m_bResolveAddressesToHostnames)
				sHost = theApp.AddressToString(pAddress, nAddressLen, 0, nullptr);
			if (sHost.GetLength())
			{
				// Format output with hostname and IP address
				if (nEchoReplyStatus == IP_SUCCESS)
#pragma warning(suppress: 26472)
					_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Reply from %s [%s], bytes=%d, time=%s TTL=%d"), theApp.AddressToString(pAddress, nAddressLen, NI_NUMERICHOST, nullptr).GetString(), sHost.GetString(), static_cast<int>(theApp.m_wDataRequestSize), theApp.RTTAsString(nRTT).GetString(), static_cast<int>(theApp.m_nTTL));
				else
					_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Reply from %s [%s]: %s"), theApp.AddressToString(pAddress, nAddressLen, NI_NUMERICHOST, nullptr).GetString(), sHost.GetString(), theApp.GetIpErrorString(nEchoReplyStatus).GetString());
				TRACE(_T("%s\n"), g_lpszOutputString);
				pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
			}
			else
			{
				// Format output with IP address only
				if (nEchoReplyStatus == IP_SUCCESS)
#pragma warning(suppress: 26472)
					_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Reply from %s, bytes=%d, time=%s TTL=%d"), theApp.AddressToString(pAddress, nAddressLen, NI_NUMERICHOST, nullptr).GetString(), static_cast<int>(theApp.m_wDataRequestSize), theApp.RTTAsString(nRTT).GetString(), static_cast<int>(theApp.m_nTTL));
				else
					_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Reply from %s: %s"), theApp.AddressToString(pAddress, nAddressLen, NI_NUMERICHOST, nullptr).GetString(), theApp.GetIpErrorString(nEchoReplyStatus).GetString());
				TRACE(_T("%s\n"), g_lpszOutputString);
				pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
			}
		}
		else
		{
			// Ping failed - display error message
			const DWORD dwError{ GetLastError() };
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("%s"), theApp.GetErrorMessage(dwError).GetString());
			TRACE(_T("%s\n"), g_lpszOutputString);
			pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
		}

		// Check if we should stop pinging
		if (!theApp.m_bPingTillStopped)
		{
			if (nRequestsSent == theApp.m_nRequestsToSend)
				break;
		}
	}

	return 0;
}

/**
 * @brief Class derived to implement Trace Route with custom result handling
 */
class CMyTraceRoute : public CTraceRoute
{
	bool OnSingleHostResult(int nHostNum, const CHostTraceMultiReplyv4& htmr) override;
	bool OnSingleHostResult(int nHostNum, const CHostTraceMultiReplyv6& htmr) override;
public:
	CNetVoyagerView* m_pNetVoyagerView{ nullptr };
};

/**
 * @brief Handles the result of a single host trace for IPv4
 * @param nHostNum The hop number
 * @param htmr The trace reply structure containing host information
 * @return true to continue tracing, false to stop
 */
bool CMyTraceRoute::OnSingleHostResult(int nHostNum, const CHostTraceMultiReplyv4& htmr)
{
	if (htmr.dwError == 0)
	{
		CString sHost;
		// Attempt to resolve IP address to hostname if enabled
		if (theApp.m_bResolveAddressesToHostnames)
#pragma warning(suppress: 26490)
			sHost = theApp.AddressToString(reinterpret_cast<const SOCKADDR*>(&htmr.Address), sizeof(htmr.Address), NI_NAMEREQD, nullptr);
#pragma warning(suppress: 26490)
		// Get numeric IP address string
		CString sIPAddress{ theApp.AddressToString(reinterpret_cast<const SOCKADDR*>(&htmr.Address), sizeof(htmr.Address), NI_NUMERICHOST, nullptr) };

		if (sHost.GetLength())
		{
			// Display hop with hostname and IP address
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t%s\t%s\t%s\t%s [%s]"), nHostNum, theApp.RTTAsString(htmr.minRTT).GetString(), theApp.RTTAsString(htmr.avgRTT).GetString(),
				theApp.RTTAsString(htmr.maxRTT).GetString(), sHost.GetString(), sIPAddress.GetString());
			TRACE(_T("%s\n"), g_lpszOutputString);
			m_pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
		}
		else
		{
			// Display hop with IP address only
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t%s\t%s\t%s\t%s"), nHostNum, theApp.RTTAsString(htmr.minRTT).GetString(), theApp.RTTAsString(htmr.avgRTT).GetString(),
				theApp.RTTAsString(htmr.maxRTT).GetString(), sIPAddress.GetString());
			TRACE(_T("%s\n"), g_lpszOutputString);
			m_pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
		}
	}
	else
	{
		// Display error or timeout for this hop
		if (htmr.dwError == ERROR_TIMEOUT)
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t*\t*\t*\tRequest timed out."), nHostNum);
		else
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t*\t*\t*\tError:%s"), nHostNum, theApp.GetErrorMessage(htmr.dwError).GetString());
		TRACE(_T("%s\n"), g_lpszOutputString);
		m_pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
	}
	// Return true to continue tracing
	return true;
}

/**
 * @brief Handles the result of a single host trace for IPv6
 * @param nHostNum The hop number
 * @param htmr The trace reply structure containing host information
 * @return true to continue tracing, false to stop
 */
bool CMyTraceRoute::OnSingleHostResult(int nHostNum, const CHostTraceMultiReplyv6& htmr)
{
	if (htmr.dwError == 0)
	{
		CString sHost;
		// Attempt to resolve IP address to hostname if enabled
		if (theApp.m_bResolveAddressesToHostnames)
#pragma warning(suppress: 26490)
			sHost = theApp.AddressToString(reinterpret_cast<const SOCKADDR*>(&htmr.Address), sizeof(htmr.Address), NI_NAMEREQD, nullptr);
#pragma warning(suppress: 26490)
		// Get numeric IPv6 address string
		CString sIPAddress{ theApp.AddressToString(reinterpret_cast<const SOCKADDR*>(&htmr.Address), sizeof(htmr.Address), NI_NUMERICHOST, nullptr) };

		if (sHost.GetLength())
		{
			// Display hop with hostname and IPv6 address
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t%s\t%s\t%s\t%s [%s]"), nHostNum, theApp.RTTAsString(htmr.minRTT).GetString(), theApp.RTTAsString(htmr.avgRTT).GetString(),
				theApp.RTTAsString(htmr.maxRTT).GetString(), sHost.GetString(), sIPAddress.GetString());
			TRACE(_T("%s\n"), g_lpszOutputString);
			m_pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
		}
		else
		{
			// Display hop with IPv6 address only
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t%s\t%s\t%s\t%s"), nHostNum, theApp.RTTAsString(htmr.minRTT).GetString(), theApp.RTTAsString(htmr.avgRTT).GetString(),
				theApp.RTTAsString(htmr.maxRTT).GetString(), sIPAddress.GetString());
			TRACE(_T("%s\n"), g_lpszOutputString);
			m_pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
		}
	}
	else
	{
		// Display error or timeout for this hop
		if (htmr.dwError == ERROR_TIMEOUT)
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t*\t*\t*\tRequest timed out."), nHostNum);
		else
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("  %d\t*\t*\t*\tError:%s"), nHostNum, theApp.GetErrorMessage(htmr.dwError).GetString());
		TRACE(_T("%s\n"), g_lpszOutputString);
		m_pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());
	}
	// Return true to continue tracing
	return true;
}

/**
 * @brief Thread procedure for executing traceroute operations
 * @param lpParam Pointer to CNetVoyagerView instance
 * @return Thread exit code (always 0)
 */
DWORD WINAPI TRACE_ThreadProc(LPVOID lpParam)
{
	CNetVoyagerView* pNetVoyagerView = reinterpret_cast<CNetVoyagerView*>(lpParam);
	ASSERT(pNetVoyagerView != nullptr);

	// Display initial traceroute header message
#pragma warning(suppress: 26472)
	_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Tracing route to <strong>%s</strong> over a maximum of %d hops:"), theApp.m_sHostToResolve.GetString(), static_cast<int>(theApp.m_nHopCount));
	TRACE(_T("%s\n"), g_lpszOutputString);
	pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());

	// Perform the actual trace route operation
	CTraceRoute::CReplyv4 trrv4;
	CTraceRoute::CReplyv6 trrv6;
	CMyTraceRoute tr;
	tr.m_pNetVoyagerView = pNetVoyagerView;
	if (theApp.m_bIPv6)
	{
		// Execute IPv6 traceroute
		if (tr.Tracev6(theApp.m_sHostToResolve, trrv6, theApp.m_nHopCount, theApp.m_dwTimeout, theApp.m_nPings, 32, 0, false, false, theApp.m_sLocalBoundAddress.GetLength() ? theApp.m_sLocalBoundAddress : nullptr))
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Trace complete."));
		else
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("%s"), theApp.GetErrorMessage(GetLastError()).GetString());
	}
	else
	{
		// Execute IPv4 traceroute
		if (tr.Tracev4(theApp.m_sHostToResolve, trrv4, theApp.m_nHopCount, theApp.m_dwTimeout, theApp.m_nPings, 32, 0, false, false, theApp.m_sLocalBoundAddress.GetLength() ? theApp.m_sLocalBoundAddress : nullptr))
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("Trace complete."));
		else
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("%s"), theApp.GetErrorMessage(GetLastError()).GetString());
	}
	// Display completion or error message
	TRACE(_T("%s\n"), g_lpszOutputString);
	pNetVoyagerView->AddDocumentText(W2UTF8(g_lpszOutputString, static_cast<int>(_tcslen(g_lpszOutputString))).GetString());

	return 0;
}

/**
 * @brief Waits for an event while processing Windows messages
 * @param hEvent Handle to the event to wait for (created if NULL)
 * @param dwTimeout Timeout in milliseconds
 * @return true if the event was signaled, false on timeout or error
 */
bool WaitWithMessageLoop(HANDLE hEvent, DWORD dwTimeout)
{
	DWORD dwRet;
	MSG msg;
	// Create event if not provided
	hEvent = hEvent ? hEvent : CreateEvent(NULL, FALSE, FALSE, NULL);

	while (true)
	{
		// Wait for event or message
		dwRet = MsgWaitForMultipleObjects(1, &hEvent, FALSE, dwTimeout, QS_ALLINPUT);
		if (dwRet == WAIT_OBJECT_0)
			return true; // Event signaled
		if (dwRet != WAIT_OBJECT_0 + 1)
			break; // Timeout or error
		// Process all pending messages to keep UI responsive
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			// Check if event was signaled while processing messages
			if (hEvent && (WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0))
				return true;
		}
	}
	return false;
}

/**
 * @brief Handles the Ping command
 * Prompts user for hostname and executes ping operation in a separate thread
 */
void CNetVoyagerView::OnPing()
{
	// Only allow one network operation at a time
	if (!g_bThreadRunning)
	{
		CInputBox pInputBox(this);
		if (pInputBox.DoModal() == IDOK)
		{
			// Set thread running flag
			g_bThreadRunning = true;
			theApp.m_sHostToResolve = pInputBox.m_strHostname;

			// Clear previous results
			if (m_arrDocumentText.size() > 0)
				m_arrDocumentText.erase(m_arrDocumentText.begin(), m_arrDocumentText.end());
			// Create new temporary HTML file for results
			SetDocumentPath(NewDocumentPath());
			ExportDocument();

			// Create and show progress dialog
			CPleaseWait dlgPleaseWait(this);
			VERIFY(dlgPleaseWait.Create(IDD_PLEASEWAIT, this));
			dlgPleaseWait.m_ctrlProgress.SetMarquee(TRUE, 40);
			dlgPleaseWait.CenterWindow();
			dlgPleaseWait.ShowWindow(SW_SHOW);

			// Create ping worker thread
			HANDLE hThread = ::CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)PING_ThreadProc,
				this,
				0,
				&m_nThreadID);

			// Wait for thread completion while keeping UI responsive
			VERIFY(WaitWithMessageLoop(hThread, INFINITE));

			// Navigate to the results HTML file
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("file:///%s"), GetDocumentPath().c_str());
			CString strURL(g_lpszOutputString);
			// Convert backslashes to forward slashes for file URL
			strURL.Replace(_T("\\"), _T("/"));
			m_pWebBrowser->Navigate(g_lpszOutputString, nullptr);

			// Clean up progress dialog
			VERIFY(dlgPleaseWait.DestroyWindow());

			// Clear thread running flag
			g_bThreadRunning = false;
		}
	}
}

/**
 * @brief Updates the UI state for the Ping command
 * @param pCmdUI Pointer to the command UI object
 */
void CNetVoyagerView::OnUpdatePing(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!g_bThreadRunning);
}

/**
 * @brief Handles the Trace Route command
 * Prompts user for hostname and executes traceroute operation in a separate thread
 */
void CNetVoyagerView::OnTraceRoute()
{
	// Only allow one network operation at a time
	if (!g_bThreadRunning)
	{
		CInputBox pInputBox(this);
		if (pInputBox.DoModal() == IDOK)
		{
			// Set thread running flag
			g_bThreadRunning = true;
			theApp.m_sHostToResolve = pInputBox.m_strHostname;

			// Clear previous results
			if (m_arrDocumentText.size() > 0)
				m_arrDocumentText.erase(m_arrDocumentText.begin(), m_arrDocumentText.end());
			// Create new temporary HTML file for results
			SetDocumentPath(NewDocumentPath());
			ExportDocument();

			// Create and show progress dialog
			CPleaseWait dlgPleaseWait(this);
			VERIFY(dlgPleaseWait.Create(IDD_PLEASEWAIT, this));
			dlgPleaseWait.m_ctrlProgress.SetMarquee(TRUE, 40);
			dlgPleaseWait.CenterWindow();
			dlgPleaseWait.ShowWindow(SW_SHOW);

			// Create traceroute worker thread
			HANDLE hThread = ::CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)TRACE_ThreadProc,
				this,
				0,
				&m_nThreadID);

			// Wait for thread completion while keeping UI responsive
			VERIFY(WaitWithMessageLoop(hThread, INFINITE));

			// Navigate to the results HTML file
			_stprintf_s(g_lpszOutputString, _countof(g_lpszOutputString) - 1, _T("file:///%s"), GetDocumentPath().c_str());
			CString strURL(g_lpszOutputString);
			// Convert backslashes to forward slashes for file URL
			strURL.Replace(_T("\\"), _T("/"));
			m_pWebBrowser->Navigate(g_lpszOutputString, nullptr);

			// Clean up progress dialog
			VERIFY(dlgPleaseWait.DestroyWindow());

			// Clear thread running flag
			g_bThreadRunning = false;
		}
	}
}

/**
 * @brief Updates the UI state for the Trace Route command
 * @param pCmdUI Pointer to the command UI object
 */
void CNetVoyagerView::OnUpdateTraceRoute(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!g_bThreadRunning);
}

/**
 * @brief Generates a new temporary HTML file path
 * @return Wide string containing the full path to a temporary HTML file
 */
const std::wstring CNetVoyagerView::NewDocumentPath()
{
	TCHAR lpszTempPath[_MAX_PATH + 1] = { 0, };
	// Get the system temporary directory path
	DWORD nLength = GetTempPath(_MAX_PATH, lpszTempPath);
	if (nLength > 0)
	{
		TCHAR lpszFilePath[_MAX_PATH + 1] = { 0, };
		// Generate a unique temporary file name with "NET" prefix
		nLength = GetTempFileName(lpszTempPath, L"NET", 0, lpszFilePath);
		if (nLength > 0)
		{
			CString strFileName = lpszFilePath;
			// Change extension from .tmp to .html
			strFileName.Replace(_T(".tmp"), _T(".html"));
			return strFileName.GetString();
		}
	}
	// Return empty string on failure
	return _T("");
}

/**
 * @brief Exports the current document text to an HTML file
 * Creates a complete HTML document with Bootstrap styling
 */
void CNetVoyagerView::ExportDocument()
{
	std::ofstream htmlFile(GetDocumentPath().c_str(), std::ofstream::out);
	if (!htmlFile.is_open())
		return; // Failed to open file for writing

	// Write the complete HTML document structure
	WriteHtmlHeader(htmlFile);
	WriteHtmlBody(htmlFile);
	WriteHtmlFooter(htmlFile);
	// File is automatically closed by ofstream destructor
}

/**
 * @brief Writes the HTML header section to the output file
 * @param file Reference to the output file stream
 */
void CNetVoyagerView::WriteHtmlHeader(std::ofstream& file)
{
	// Write HTML5 doctype and opening tags
	file << "<!DOCTYPE html>\n"
		<< "<html lang=\"en\">\n"
		<< "<head>\n"
		<< "<meta charset=\"UTF-8\">\n"
		<< "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
		// Include Bootstrap CSS from CDN for styling
		<< "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.7/dist/css/bootstrap.min.css\" "
		<< "rel=\"stylesheet\" integrity=\"sha384-LN+7fdVzj6u52u30Kp6M/trliBMCMKTyK833zpbD+pXdCLuTusPj697FH4R/5mcr\" "
		<< "crossorigin=\"anonymous\">\n"
		<< "</head>\n"
		<< "<body>\n"
		<< "<div class=\"container\">\n"
		<< "<div id=\"row\">\n";
}

/**
 * @brief Writes the HTML body content to the output file
 * @param file Reference to the output file stream
 */
void CNetVoyagerView::WriteHtmlBody(std::ofstream& file)
{
	// Iterate through all document text lines and write them to HTML
	for (const auto& text : m_arrDocumentText)
	{
		file << text << "<br>\n";
	}
}

/**
 * @brief Writes the HTML footer section to the output file
 * @param file Reference to the output file stream
 */
void CNetVoyagerView::WriteHtmlFooter(std::ofstream& file)
{
	// Close HTML tags and include Bootstrap JavaScript from CDN
	file << "</div>\n"
		<< "</div>\n"
		// Include Bootstrap JS bundle for interactive components
		<< "<script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.7/dist/js/bootstrap.bundle.min.js\" "
		<< "integrity=\"sha384-ndDqU0Gzau9qJ1lfW4pNLlhNTkCfHzAVBReH9diLvGRem5+R9g2FzA8ZGN954O5Q\" "
		<< "crossorigin=\"anonymous\"></script>\n"
		<< "</body>\n"
		<< "</html>\n";
}
