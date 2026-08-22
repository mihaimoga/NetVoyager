# AGENTS.md

## Overview

**NetVoyager** is a native Windows desktop network diagnostics utility developed in C++ using MFC (Microsoft Foundation Classes) and the Microsoft Edge WebView2 control. It provides real-time network connectivity assessment, latency measurement, and route tracing through built-in ICMP ping and traceroute tools presented in a modern, HTML/CSS-styled view.

- **Language & Standard:** C++ (compiled with `/std:c++latest`, Unicode character set)
- **Framework:** MFC (statically linked, `UseOfMfc=Static`)
- **Target Platform:** Windows 10+ (Win32 and x64 architectures)
- **Toolset:** MSVC `v145` (Visual Studio 2022) / Windows SDK 10.0
- **License:** GNU General Public License v3.0 or later (GPL-3.0-or-later)

---

## Architecture & Component Design

NetVoyager follows the classic MFC Document-View architecture enhanced with modern Windows WebView2 rendering and multi-threaded background network probes.

```
┌────────────────────────────────────────────────────────┐
│                   CNetVoyagerApp                       │
│  (Application Lifecycle, CLI Options, Configurations)  │
└──────────────────────────┬─────────────────────────────┘
						   │
			 ┌─────────────┴─────────────┐
			 ▼                           ▼
┌─────────────────────────┐ ┌─────────────────────────┐
│       CMainFrame        │ │     CNetVoyagerDoc      │
│ (Ribbon UI, Status Bar, │ │ (Document Data, State)  │
│    Progress Indicators) │ └────────────┬────────────┘
└────────────┬────────────┘              │
			 │                           │
			 └─────────────┬─────────────┘
						   ▼
			   ┌───────────────────────┐
			   │    CNetVoyagerView    │
			   │ (Hosts WebView2, UI   │
			   │   Dispatch & Actions) │
			   └───────────┬───────────┘
						   │
	   ┌───────────────────┼───────────────────┐
	   ▼                   ▼                   ▼
┌──────────────┐   ┌───────────────┐   ┌───────────────┐
│  CWebBrowser │   │     CPing     │   │  CTraceRoute  │
│  (WebView2)  │   │  (ICMPv4/v6)  │   │  (ICMPv4/v6)  │
└──────────────┘   └───────────────┘   └───────────────┘
```

### Key Modules & Source Files

| File | Role & Description |
|---|---|
| `NetVoyager.h` / `NetVoyager.cpp` | Main application class `CNetVoyagerApp`; handles app initialization, command-line arguments parsing, configuration settings. |
| `MainFrame.h` / `MainFrame.cpp` | Main window frame `CMainFrame`; manages Ribbon UI controls, status bar, and progress bar feedback. |
| `NetVoyagerDoc.h` / `NetVoyagerDoc.cpp` | Document class `CNetVoyagerDoc` representing diagnostic document state. |
| `NetVoyagerView.h` / `NetVoyagerView.cpp` | View class `CNetVoyagerView`; embeds the WebView2 browser control and coordinates asynchronous diagnostic operations. |
| `EdgeWebBrowser.h` / `EdgeWebBrowser.cpp` | Wrapper around Microsoft Edge WebView2 control (`ICoreWebView2`) for HTML output rendering. |
| `ping.h` / `ping.cpp` | Low-level ICMP ping implementation (`CPing`) supporting IPv4 and IPv6 (`IcmpSendEcho`, `Icmp6SendEcho2`). |
| `tracer.h` / `tracer.cpp` | Multi-hop traceroute engine (`CTraceRoute`) supporting IPv4 and IPv6 with per-hop latency measurement. |
| `InputBox.h` / `InputBox.cpp` | Custom dialog `CInputBox` for prompting user input (hostnames, IP addresses, configuration values). |
| `HLinkCtrl.h` / `HLinkCtrl.cpp` | Hyperlink GUI control for clickable links in dialogs and UI. |
| `PleaseWait.h` / `PleaseWait.cpp` | Wait indicator dialog `CPleaseWait` shown during blocking background operations. |
| `VersionInfo.h` / `VersionInfo.cpp` | Utility class `CVersionInfo` for querying file version and build metadata. |
| `Messages.h` | Custom Windows messages (`WM_USER + ...`) used for inter-thread UI notifications. |
| `Resource.h` / `NetVoyager.rc` | Resource symbols, menus, dialogs, icons, ribbons, and string tables. |
| `pch.h` / `framework.h` / `pch.cpp` | Precompiled header configuration and core system headers (MFC, Win32, WinSock2, IPHlpApi, STL). |

---

## Dependencies & Package Management

Dependencies are managed via NuGet (`packages.config`):

1. **Microsoft.Web.WebView2** (`native` package):
   - Provides `ICoreWebView2` COM interfaces and WebView2 loader library for embedding Chromium-based Edge rendering engine.
2. **Microsoft.Windows.ImplementationLibrary (WIL)** (`native` package):
   - Header-only library with RAII wrappers, smart pointers, error handling helpers, and COM safety utilities.

Windows Native APIs used:
- **IP Helper API (`iphlpapi.lib`, `icmpapi.h`)**: ICMP echo requests and routing queries.
- **Windows Sockets 2 (`ws2_32.lib`, `ws2tcpip.h`)**: Socket operations, name resolution (`getaddrinfo`, `getnameinfo`).
- **Setup API (`setupapi.lib`)** & **WMI (`WBemCli.h`)**: System network interface querying.

---

## Build & Configuration Guidelines

### Project Configurations
The project `NetVoyager.vcxproj` supports the following build matrices:
- `Debug|x64`
- `Release|x64`
- `Debug|Win32`
- `Release|Win32`

### Command-Line Build (MSBuild)
To build via developer command prompt / PowerShell:
```powershell
# Restore NuGet packages
nuget restore NetVoyager.vcxproj

# Build x64 Release
msbuild NetVoyager.vcxproj /p:Configuration=Release /p:Platform=x64

# Build x64 Debug
msbuild NetVoyager.vcxproj /p:Configuration=Debug /p:Platform=x64
```

---

## Code Conventions & Implementation Guidelines

When working on this codebase, agents and developers must adhere to these standards:

### 1. Precompiled Headers
- Every `.cpp` source file must include `"pch.h"` as its very first non-comment line.
- Commonly used external / standard library headers should be included inside `framework.h` (referenced by `pch.h`) rather than repeated across individual headers.

### 2. C++ & MFC Conventions
- **Language Level:** C++20 / latest (`/std:c++latest`). Utilize modern C++ features (structured bindings, `std::string_view`, concepts, smart pointers) while preserving MFC compatibility.
- **MFC Class Naming:** Prefix MFC derived classes with `C` (e.g., `CNetVoyagerApp`, `CMainFrame`, `CWebBrowser`).
- **Member Variables:** Prefix class member variables with `m_` (e.g., `m_bIPv6`, `m_sHostToResolve`, `m_pWebBrowser`).
- **Strings & Characters:** Use `CString` / `CStringW` or `std::wstring` with `_T()` / `L` string literals for UI and Windows API compatibility (`_UNICODE` is enabled).
- **Resource IDs:** Match naming conventions in `Resource.h` (e.g., `IDR_MAINFRAME`, `ID_NETWORK_PING`, `IDC_...`).

### 3. Concurrency & UI Thread Safety
- **Network Probes:** Network operations (ICMP ping loops, DNS lookups, route tracing) must execute on worker threads to keep the UI responsive.
- **UI Updates:** Worker threads must **never** call UI functions or touch CWnd controls directly. Instead, post custom messages defined in `Messages.h` or marshal through synchronization primitives to the main UI thread.
- **Cancellation & Timeouts:** Long-running loops must periodically verify cancellation flags to support graceful user cancellation.

### 4. Memory & Resource Management
- Prefer RAII types (`std::unique_ptr`, `std::shared_ptr`, `wil::unique_handle`, `wil::com_ptr`) over raw pointer ownership (`new` / `delete`).
- Ensure Windows handles (ICMP handles via `IcmpCreateFile`, sockets, events) are closed via RAII wrappers or safe cleanup logic.

### 5. Modifying Resources & Ribbon UI
- When modifying UI elements, update `Resource.h`, `NetVoyager.rc`, and `res/ribbon.mfcribbon-ms` concurrently.
- Verify that numeric resource IDs in `Resource.h` do not conflict and `_APS_NEXT_..._VALUE` symbols at the bottom of `Resource.h` are incremented accordingly.
