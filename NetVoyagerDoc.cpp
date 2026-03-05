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

// NetVoyagerDoc.cpp : implementation of the CNetVoyagerDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "NetVoyager.h"
#endif

#include "NetVoyagerDoc.h"

// Include Windows property key definitions for search integration
#include <propkey.h>

#ifdef _DEBUG
// Enable memory leak detection in debug builds
#define new DEBUG_NEW
#endif

// CNetVoyagerDoc

// Implement dynamic creation support for MFC document/view architecture
IMPLEMENT_DYNCREATE(CNetVoyagerDoc, CDocument)

// Message map for handling Windows messages and commands
BEGIN_MESSAGE_MAP(CNetVoyagerDoc, CDocument)
END_MESSAGE_MAP()

// CNetVoyagerDoc construction/destruction

/**
 * @brief Default constructor for CNetVoyagerDoc
 * Initializes a new document instance
 */
CNetVoyagerDoc::CNetVoyagerDoc() noexcept
{
	// TODO: add one-time construction code here
}

/**
 * @brief Destructor for CNetVoyagerDoc
 * Cleans up document resources
 */
CNetVoyagerDoc::~CNetVoyagerDoc()
{
}

/**
 * @brief Called when a new document is created
 * @return TRUE if successful, FALSE otherwise
 * @note SDI documents will reuse this document instance
 */
BOOL CNetVoyagerDoc::OnNewDocument()
{
	// Call base class implementation first
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	// Note: In SDI (Single Document Interface) applications, this method
	// is called for both new documents and when resetting the existing document

	return TRUE;
}

// CNetVoyagerDoc serialization

/**
 * @brief Serializes or deserializes the document
 * @param ar Archive object for reading or writing
 * @details Called to save or load the document's data to/from storage
 */
void CNetVoyagerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		// Called when saving the document to a file
		// Use ar << operator to write data to the archive
	}
	else
	{
		// TODO: add loading code here
		// Called when loading the document from a file
		// Use ar >> operator to read data from the archive
	}
}

#ifdef SHARED_HANDLERS

/**
 * @brief Draws a thumbnail representation of the document
 * @param dc Device context to draw on
 * @param lprcBounds Rectangle bounds for the thumbnail
 * @details Used by Windows Explorer to display document thumbnails
 */
void CNetVoyagerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	// Fill background with white color for now
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	// Get the default GUI font and modify its size for thumbnail
	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	// Set larger font size for visibility in thumbnail
	lf.lfHeight = 36;

	// Create and select the custom font
	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	// Draw centered, word-wrapped text
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	// Restore the original font
	dc.SelectObject(pOldFont);
}

/**
 * @brief Initializes search content for Windows Search integration
 * @details Sets up searchable content from the document's data
 * @note Content parts should be separated by ";"
 */
void CNetVoyagerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	// TODO: Build search content string from actual document data
	// This allows Windows Search to index and find this document

	// Update the search content in the Windows Search index
	SetSearchContent(strSearchContent);
}

/**
 * @brief Sets the search content for Windows Search indexing
 * @param value Search content string to be indexed
 * @details If value is empty, removes the search content chunk
 */
void CNetVoyagerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		// Remove search content if value is empty
		// This removes the document from the Windows Search index
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		// Create and set search content chunk
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		// Use ATL exception-safe allocation
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			// Set the text value for search indexing
			// PKEY_Search_Contents is the Windows property key for searchable content
			// CHUNK_TEXT indicates this is plain text content
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			// Add the chunk to the document's search metadata
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CNetVoyagerDoc diagnostics

#ifdef _DEBUG
/**
 * @brief Validates the document object in debug builds
 * @details Performs diagnostic checks on the document's state
 */
void CNetVoyagerDoc::AssertValid() const
{
	// Call base class validation
	// This checks the internal consistency of the document object
	CDocument::AssertValid();
}

/**
 * @brief Dumps the document's state to a dump context for debugging
 * @param dc Dump context to write diagnostic information to
 */
void CNetVoyagerDoc::Dump(CDumpContext& dc) const
{
	// Call base class dump function
	// This outputs diagnostic information about the document
	CDocument::Dump(dc);
}
#endif //_DEBUG

// CNetVoyagerDoc commands
