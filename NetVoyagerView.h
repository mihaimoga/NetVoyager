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

class CNetVoyagerView : public CView
{
protected: // create from serialization only
	CNetVoyagerView() noexcept;
	DECLARE_DYNCREATE(CNetVoyagerView)

// Attributes
public:
	CNetVoyagerDoc* GetDocument() const;

public:
	std::unique_ptr<CWebBrowser> m_pWebBrowser{};

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CNetVoyagerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DWORD m_nThreadID;
	std::wstring m_strDocumentPath;
	std::vector<std::string> m_arrDocumentText;

// Generated message map functions
protected:
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	afx_msg void OnPing();
	afx_msg void OnUpdatePing(CCmdUI *pCmdUI);
	afx_msg void OnTraceRoute();
	afx_msg void OnUpdateTraceRoute(CCmdUI *pCmdUI);
	// Custom functions
	const std::wstring NewDocumentPath();
	const std::wstring GetDocumentPath() { return m_strDocumentPath; }
	void SetDocumentPath(const std::wstring strNewDocPath) { m_strDocumentPath = strNewDocPath; }
	void AddDocumentText(const std::string strNewDocText) { m_arrDocumentText.push_back(strNewDocText); ExportDocument(); }
	void ExportDocument();

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in NetVoyagerView.cpp
inline CNetVoyagerDoc* CNetVoyagerView::GetDocument() const
   { return reinterpret_cast<CNetVoyagerDoc*>(m_pDocument); }
#endif
