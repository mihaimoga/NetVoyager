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

// NetVoyager.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "NetVoyager.h"
#include "MainFrame.h"

#include "NetVoyagerDoc.h"
#include "NetVoyagerView.h"

// Include version information reader and hyperlink control
#include "VersionInfo.h"
#include "HLinkCtrl.h"

#ifdef _DEBUG
// Override new operator for memory leak detection in debug builds
#define new DEBUG_NEW
#endif


// CNetVoyagerApp

// Map Windows messages and commands to handler functions
BEGIN_MESSAGE_MAP(CNetVoyagerApp, CWinAppEx)
	// About dialog command
	ON_COMMAND(ID_APP_ABOUT, &CNetVoyagerApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()

// CNetVoyagerApp construction

/**
 * @brief Default constructor for CNetVoyagerApp
 * Initializes all network diagnostic settings with default values
 * @details Sets up ping and traceroute parameters, enables Restart Manager support
 */
CNetVoyagerApp::CNetVoyagerApp() noexcept :
	m_bResolveAddressesToHostnames{ false },  // Don't resolve IPs to hostnames by default
	m_bPingTillStopped{ false },              // Send limited number of pings
	m_nRequestsToSend{ 4 },                   // Default: send 4 ping requests
	m_nTTL{ 128 },                            // Time To Live for packets
	m_nTOS{ 0 },                              // Type Of Service (QoS)
	m_wDataRequestSize{ 32 },                 // Default ping data size: 32 bytes
	m_dwTimeout{ 5000 },                      // Timeout: 5 seconds
	m_bDontFragment{ false },                 // Allow packet fragmentation
	m_bIPv6{ false },                         // Use IPv4 by default
	m_nHopCount{ 30 },                        // Maximum hops for traceroute
	m_nPings{ 3 }                             // Number of pings per hop in traceroute
{
	// Enable Restart Manager support for application recovery
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	// Configure exception handling for managed code
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	// Set application ID for taskbar grouping and jump list support
	SetAppID(_T("NetVoyager.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CNetVoyagerApp object

CNetVoyagerApp theApp;

// CNetVoyagerApp initialization

/**
 * @brief Initializes the application instance
 * @return TRUE if initialization succeeds, FALSE otherwise
 * @details Initializes common controls, sockets, OLE libraries, document templates,
 *          and processes command line arguments
 */
BOOL CNetVoyagerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	// ICC_WIN95_CLASSES includes standard controls like buttons, list boxes, etc.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	// Call base class initialization
	CWinAppEx::InitInstance();

	// Initialize Windows Sockets for network operations
	if (!AfxSocketInit())
	{
		// Display error if socket initialization fails
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Initialize OLE libraries for COM support
	if (!AfxOleInit())
	{
		// Display error if OLE initialization fails
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	// Disable taskbar interaction features
	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	// Store settings under HKEY_CURRENT_USER\Software\Mihai Moga\NetVoyager
	SetRegistryKey(_T("Mihai Moga"));
	// Load standard INI file options (including MRU - Most Recently Used files list)
	// Maximum of 10 items in MRU list
	LoadStdProfileSettings(10);

	// Initialize context menu manager for popup menus
	InitContextMenuManager();

	// Initialize keyboard shortcut manager
	InitKeyboardManager();

	// Initialize tooltip manager for enhanced tooltips
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	// Use Visual Manager theme for tooltips
	ttParams.m_bVislManagerTheme = TRUE;
	// Apply tooltip parameters to all tooltip types
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	// Create SDI (Single Document Interface) template
	// Links CNetVoyagerDoc, CMainFrame, and CNetVoyagerView together
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,                        // Resource ID for menus and icons
		RUNTIME_CLASS(CNetVoyagerDoc),        // Document class
		RUNTIME_CLASS(CMainFrame),            // Main SDI frame window
		RUNTIME_CLASS(CNetVoyagerView));      // View class
	if (!pDocTemplate)
		return FALSE;
	// Register the document template with the application
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	// m_pMainWnd->MoveWindow(CRect(0, 0, 814, 607));
	// m_pMainWnd->CenterWindow();
	// Make the main window visible
	m_pMainWnd->ShowWindow(SW_SHOW);
	// Force window update
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

/**
 * @brief Called when the application is terminating
 * @return Exit code for the application
 * @details Performs cleanup of OLE libraries and other resources
 */
int CNetVoyagerApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	// Uninitialize OLE libraries
	// FALSE parameter means don't check for unreleased objects (faster shutdown)
	AfxOleTerm(FALSE);

	// Call base class cleanup
	return CWinAppEx::ExitInstance();
}

// CNetVoyagerApp message handlers

// CAboutDlg dialog used for App About

/**
 * @brief About dialog class for the application
 * @details Displays application version, copyright, license information,
 *          and contact links in a modal dialog
 */
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

protected:
	CStatic m_ctrlVersion;
	CEdit m_ctrlWarning;
	CVersionInfo m_pVersionInfo;
	CHLinkCtrl m_ctrlWebsite;
	CHLinkCtrl m_ctrlEmail;
	CHLinkCtrl m_ctrlContributors;

	DECLARE_MESSAGE_MAP()
};

/**
 * @brief Default constructor for CAboutDlg
 */
CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

/**
 * @brief Exchanges and validates dialog data
 * @param pDX Data exchange object
 * @details Maps dialog controls to member variables
 */
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERSION, m_ctrlVersion);
	DDX_Control(pDX, IDC_WARNING, m_ctrlWarning);
	DDX_Control(pDX, IDC_WEBSITE, m_ctrlWebsite);
	DDX_Control(pDX, IDC_EMAIL, m_ctrlEmail);
	DDX_Control(pDX, IDC_CONTRIBUTORS, m_ctrlContributors);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/**
 * @brief Gets the full path of the current module (executable)
 * @param pdwLastError Optional pointer to receive the last error code
 * @return Full path to the module, or empty string on failure
 * @details Dynamically allocates buffer and retries if path is too long
 */
CString GetModuleFileName(_Inout_opt_ DWORD* pdwLastError = nullptr)
{
	CString strModuleFileName;
	// Start with standard MAX_PATH size
	DWORD dwSize{ _MAX_PATH };
	while (true)
	{
		// Get buffer to write the path into
		TCHAR* pszModuleFileName{ strModuleFileName.GetBuffer(dwSize) };
		// Try to get the module file name
		const DWORD dwResult{ ::GetModuleFileName(nullptr, pszModuleFileName, dwSize) };
		if (dwResult == 0)
		{
			// Failed - return error code if requested
			if (pdwLastError != nullptr)
				*pdwLastError = GetLastError();
			strModuleFileName.ReleaseBuffer(0);
			return CString{};
		}
		else if (dwResult < dwSize)
		{
			// Success - path fit in the buffer
			if (pdwLastError != nullptr)
				*pdwLastError = ERROR_SUCCESS;
			strModuleFileName.ReleaseBuffer(dwResult);
			return strModuleFileName;
		}
		else if (dwResult == dwSize)
		{
			// Buffer too small - double the size and try again
			strModuleFileName.ReleaseBuffer(0);
			dwSize *= 2;
		}
	}
}

/**
 * @brief Initializes the About dialog
 * @return TRUE to set focus to the first control
 * @details Loads version information, sets GPL license text, and initializes hyperlinks
 */
BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Get the full path to the executable
	CString strFullPath{ GetModuleFileName() };
	if (strFullPath.IsEmpty())
#pragma warning(suppress: 26487)
		return FALSE;

	// Load version information from the executable
	if (m_pVersionInfo.Load(strFullPath.GetString()))
	{
		// Extract product name and version
		CString strName = m_pVersionInfo.GetProductName().c_str();
		CString strVersion = m_pVersionInfo.GetProductVersionAsString().c_str();
		// Clean up version string formatting
		strVersion.Replace(_T(" "), _T(""));  // Remove spaces
		strVersion.Replace(_T(","), _T(".")); // Replace commas with dots
		// Find positions of first two dots to extract major.minor version
		const int nFirst = strVersion.Find(_T('.'));
		const int nSecond = strVersion.Find(_T('.'), nFirst + 1);
		// Truncate to major.minor (e.g., "1.0" from "1.0.0.0")
		strVersion.Truncate(nSecond);
#if _WIN32 || _WIN64
#if _WIN64
		// Display version with 64-bit indicator
		m_ctrlVersion.SetWindowText(strName + _T(" version ") + strVersion + _T(" (64-bit)"));
#else
		// Display version with 32-bit indicator
		m_ctrlVersion.SetWindowText(strName + _T(" version ") + strVersion + _T(" (32-bit)"));
#endif
#endif
	}

	// Set the GPL v3 license text in the warning/license control
	m_ctrlWarning.SetWindowText(_T("This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>."));

	// Set up hyperlinks for contact and contribution information
	m_ctrlWebsite.SetHyperLink(_T("https://www.moga.doctor/"));
	m_ctrlEmail.SetHyperLink(_T("mailto:stefan-mihai@moga.doctor"));
	m_ctrlContributors.SetHyperLink(_T("https://github.com/mihaimoga/NetVoyager/graphs/contributors"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Called when the About dialog is being destroyed
 */
void CAboutDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
}

/**
 * @brief Handles the About menu command
 * @details Creates and displays the modal About dialog
 */
void CNetVoyagerApp::OnAppAbout()
{
	// Create About dialog instance
	CAboutDlg aboutDlg;
	// Display as modal dialog (blocks until closed)
	aboutDlg.DoModal();
}

/**
 * @brief Converts a socket address to a string representation
 * @param pSockAddr Pointer to the socket address structure
 * @param nSockAddrLen Length of the socket address structure
 * @param nFlags Flags to control name resolution (e.g., NI_NUMERICHOST)
 * @param pnSocketPort Optional pointer to receive the port number
 * @return String representation of the address, or empty string on failure
 * @details Uses GetNameInfo to convert the address, supports both IPv4 and IPv6
 */
CString CNetVoyagerApp::AddressToString(const SOCKADDR* pSockAddr, int nSockAddrLen, int nFlags, UINT* pnSocketPort)
{
	// What will be the return value from this function
	CString sSocketAddress;

	int nResult{ 0 };
#ifdef _UNICODE
	// Use wide character version for Unicode builds
	CStringW sName;
	nResult = GetNameInfoW(pSockAddr, nSockAddrLen, sName.GetBuffer(NI_MAXHOST), NI_MAXHOST, nullptr, 0, nFlags);
	sName.ReleaseBuffer();
#else
	// Use ANSI version for non-Unicode builds
	CStringA sName;
	nResult = getnameinfo(pSockAddr, nSockAddrLen, sName.GetBuffer(NI_MAXHOST), NI_MAXHOST, nullptr, 0, nFlags);
	sName.ReleaseBuffer();
#endif
	if (nResult == 0)
	{
		// Conversion successful
		sSocketAddress = sName;
		// Extract port number if requested
		if (pnSocketPort != nullptr)
			*pnSocketPort = ntohs(SS_PORT(&pSockAddr));  // Convert from network to host byte order
	}

	return sSocketAddress;
}

/**
 * @brief Retrieves the system error message for a given error code
 * @param dwError The error code (typically from GetLastError())
 * @return Human-readable error message string
 * @details Uses FormatMessage to convert error code to text
 */
CString CNetVoyagerApp::GetErrorMessage(DWORD dwError)
{
	CString sError;

	// Lookup the error using FormatMessage
	LPTSTR lpBuffer{ nullptr };
#pragma warning(suppress: 26490)
	// Ask Windows to allocate buffer and format the error message
	// FORMAT_MESSAGE_ALLOCATE_BUFFER: System allocates buffer
	// FORMAT_MESSAGE_FROM_SYSTEM: Look up system error messages
	// FORMAT_MESSAGE_IGNORE_INSERTS: Don't process message inserts
	const DWORD dwReturn{ FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
									   nullptr, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT), reinterpret_cast<LPTSTR>(&lpBuffer), 0, nullptr) };

	if (dwReturn)
	{
		// Successfully formatted the message
		sError = lpBuffer;
		// Free the buffer allocated by FormatMessage
		LocalFree(lpBuffer);
	}

	return sError;
}

/**
 * @brief Retrieves the error message for an IP status code
 * @param dwError The IP status error code
 * @return Human-readable IP error message string
 * @details Calls Windows GetIpErrorString API twice: first to get buffer size, then to get text
 */
CString CNetVoyagerApp::GetIpErrorString(IP_STATUS dwError)
{
	DWORD dwBuffer{ 0 };
	// First call: determine required buffer size
	::GetIpErrorString(dwError, nullptr, &dwBuffer);
	CStringW sUnicodeError;
	// Second call: get the actual error string
	::GetIpErrorString(dwError, sUnicodeError.GetBuffer(dwBuffer), &dwBuffer);
	sUnicodeError.ReleaseBuffer();
	// Convert to CString (handles Unicode/ANSI automatically)
	return CString{ sUnicodeError };
}

/**
 * @brief Converts a round-trip time value to a formatted string
 * @param dwRTT Round-trip time in milliseconds
 * @return Formatted string (e.g., "<1ms" or "25ms")
 * @details Returns "<1ms" for RTT of 0, otherwise formats as "XXms"
 */
CString CNetVoyagerApp::RTTAsString(DWORD dwRTT)
{
	CString sMsg;
	if (dwRTT == 0)
		// RTT too fast to measure accurately
		sMsg = _T("<1ms");
	else
		// Format as milliseconds (e.g., "25ms")
		sMsg.Format(_T("%ums"), dwRTT);
	return sMsg;
}

// CNetVoyagerApp customization load/save methods

/**
 * @brief Called before loading application state
 * @details Initializes context menu manager with edit menu
 */
void CNetVoyagerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	// Load the edit menu string from resources
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	// Register the edit menu with the context menu manager
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

/**
 * @brief Loads custom application state from registry or file
 * @details Currently not implemented, can be used to restore user preferences
 */
void CNetVoyagerApp::LoadCustomState()
{
}

/**
 * @brief Saves custom application state to registry or file
 * @details Currently not implemented, can be used to persist user preferences
 */
void CNetVoyagerApp::SaveCustomState()
{
}

// CNetVoyagerApp message handlers
