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

// NetVoyagerDoc.h : interface of the CNetVoyagerDoc class
//


#pragma once


// CNetVoyagerDoc: MFC document class for the NetVoyager application.
// Manages the application's data model within the document/view architecture.
class CNetVoyagerDoc : public CDocument
{
protected: // create from serialization only
	CNetVoyagerDoc() noexcept; // Protected constructor; instances are created via MFC dynamic creation
	DECLARE_DYNCREATE(CNetVoyagerDoc) // Enables runtime dynamic creation of this class

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();  // Called by the framework when a new document is created
	virtual void Serialize(CArchive& ar); // Saves or loads document data to/from a file archive
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent(); // Populates searchable content for Windows Search indexing
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds); // Renders a thumbnail preview of the document
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CNetVoyagerDoc(); // Destructor; releases any resources held by the document
#ifdef _DEBUG
	virtual void AssertValid() const; // Validates internal state of the document object (debug only)
	virtual void Dump(CDumpContext& dc) const; // Dumps diagnostic information to the debug output (debug only)
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP() // Declares the MFC message map for this class

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value); // Registers document text with Windows Search via a filter chunk
#endif // SHARED_HANDLERS
};
