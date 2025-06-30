// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface
// (the "Fluent UI") and is provided only as referential material to supplement the
// Microsoft Foundation Classes Reference and related electronic documentation
// included with the MFC C++ library software.
// License terms to copy, use or distribute the Fluent UI are available separately.
// To learn more about our Fluent UI licensing program, please visit
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// DrawDoc.h : interface of the CDrawDoc class
//


#pragma once
#include "Drawable.h"

enum drawingTool {
	pen,
	eraser,
	ellipse,
	rectangle,
	roundRectangle
};

class CDrawDoc : public CDocument
{
protected: // create from serialization only
	CDrawDoc() noexcept;
	DECLARE_DYNCREATE(CDrawDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CDrawDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	drawingTool drawingTool;
	BOOL penStrokeInProgress;
	CArray<Drawable*> drawableArr;
	int size = 10;
	COLORREF foreColor = RGB(0, 0, 0); // Default color is black
	COLORREF backColor = RGB(255, 255, 255); // Default background color is white

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnButtonPen();
	afx_msg void OnButtonEraser();
};
