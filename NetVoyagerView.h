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

// NetVoyagerView.h : interface of the CNetVoyagerView class
//

#pragma once
#include "EdgeWebBrowser.h"

// CNetVoyagerView: MFC view class that hosts the Edge WebView2 browser control
// and orchestrates ping and traceroute network operations.
class CNetVoyagerView : public CView
{
protected: // create from serialization only
	CNetVoyagerView() noexcept;          // Protected constructor; instances are created via MFC dynamic creation
	DECLARE_DYNCREATE(CNetVoyagerView)   // Enables runtime dynamic creation of this class

// Attributes
public:
	CNetVoyagerDoc* GetDocument() const; // Returns a typed pointer to the associated document

public:
	std::unique_ptr<CWebBrowser> m_pWebBrowser{}; // Embedded Edge WebView2 browser control that renders HTML results

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);             // Overridden to draw this view (unused; content is rendered in the browser)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs); // Allows modification of window class/styles before creation

// Implementation
public:
	virtual ~CNetVoyagerView(); // Destructor; releases browser control and any other view resources
#ifdef _DEBUG
	virtual void AssertValid() const;          // Validates internal state of the view object (debug only)
	virtual void Dump(CDumpContext& dc) const; // Dumps diagnostic information to the debug output (debug only)
#endif

protected:
	DWORD m_nThreadID;                          // Thread ID of the active ping/traceroute worker thread
	std::wstring m_strDocumentPath;             // Full path to the temporary HTML file that holds operation results
	std::vector<std::string> m_arrDocumentText; // Accumulated UTF-8 output lines written into the HTML results file

// Generated message map functions
protected:
	virtual void OnInitialUpdate();                    // Called on first update; creates and initializes the browser control
	afx_msg void OnDestroy();                          // Releases the browser control when the view is destroyed
	afx_msg void OnSize(UINT nType, int cx, int cy);   // Resizes the browser control to fill the view's client area
public:
	afx_msg void OnPing();                             // Handles the Ping menu command; prompts for host and runs ping thread
	afx_msg void OnUpdatePing(CCmdUI *pCmdUI);         // Disables the Ping command while a network thread is running
	afx_msg void OnTraceRoute();                       // Handles the Trace Route command; prompts for host and runs trace thread
	afx_msg void OnUpdateTraceRoute(CCmdUI *pCmdUI);   // Disables the Trace Route command while a network thread is running
	// Custom functions
	const std::wstring NewDocumentPath();              // Generates a unique temporary .html file path for storing results
	const std::wstring GetDocumentPath() { return m_strDocumentPath; }                                              // Returns the current HTML output file path
	void SetDocumentPath(const std::wstring strNewDocPath) { m_strDocumentPath = strNewDocPath; }                   // Sets the HTML output file path
	void AddDocumentText(const std::string strNewDocText) { m_arrDocumentText.push_back(strNewDocText); ExportDocument(); } // Appends a result line and re-exports the HTML file
	void ExportDocument(); // Writes all accumulated result lines to the HTML output file

private:
	void WriteHtmlHeader(std::ofstream& file); // Writes the HTML5 doctype, <head>, and opening <body> tags with Bootstrap CSS
	void WriteHtmlBody(std::ofstream& file);   // Writes each result line as a <br>-terminated paragraph in the HTML body
	void WriteHtmlFooter(std::ofstream& file); // Writes the closing </body> and </html> tags with Bootstrap JS bundle

	DECLARE_MESSAGE_MAP() // Declares the MFC message map for this class
};

#ifndef _DEBUG  // debug version in NetVoyagerView.cpp
inline CNetVoyagerDoc* CNetVoyagerView::GetDocument() const
   { return reinterpret_cast<CNetVoyagerDoc*>(m_pDocument); }
#endif
