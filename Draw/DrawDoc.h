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
#include <memory>
#include <vector>

enum drawingTool {
	pen,
	eraser,
	ellipse,
	rectangle,
	roundRectangle
};

class CDrawDoc : public CDocument
{
// Private data members
	drawingTool selectedDrawingTool;
	std::vector<std::shared_ptr<Drawable>> drawableArr;
	int sizePen;
	int sizeEraser;
	int sizeShape;

protected: // create from serialization only
	CDrawDoc() noexcept;
	DECLARE_DYNCREATE(CDrawDoc)

// Attributes
public:
	drawingTool GetDrawingTool() const { return selectedDrawingTool; };
	void AddPoint(const CPoint& point);
	void AddObject(std::shared_ptr<Drawable> pObject);
	const POINT& GetPrevPoint();
	void SetPrevPoint(const CPoint& point);
	void DrawAll(CDC* pDC) const;
	const int GetSizePen() const { return sizePen; };
	const int GetSizeEraser() const { return sizeEraser; };
	const int GetSizeShape() const { return sizeShape; };

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
	COLORREF foreColor = RGB(0, 0, 0); // TMP: Default color is black
	COLORREF backColor = RGB(255, 255, 255); // TMP: Default background color is white

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
	afx_msg void OnUpdateButtonPen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonEraser(CCmdUI* pCmdUI);
	afx_msg void OnGallerySize();
};
