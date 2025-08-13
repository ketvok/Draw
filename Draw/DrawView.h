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

// DrawView.h : interface of the CDrawView class
//

#pragma once
#include "Tool.h"

class CDrawView : public CScrollView
{
// Private data members
	BOOL drawingMode;
	BOOL resizingMode;
	CRect canvasRect;
	CRect resizeHandleRect;
	int paddingHorizontal;
	int paddingVertical;
	BOOL bitmapInitialized;
	CRect trackRect;
	COLORREF foreColor;
	COLORREF backColor;
	int sizeIndex;
	CDC bkgDC;
	CBitmap bkgBitmap;
	std::shared_ptr<DrawingTool> currentTool;
	int activeControlCommandID;

protected: // create from serialization only
	CDrawView() noexcept;
	DECLARE_DYNCREATE(CDrawView)

// Attributes
public:
	CDrawDoc* GetDocument() const;
	COLORREF GetForeColor() const { return foreColor; }
	COLORREF GetBackColor() const { return backColor; }

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CDrawView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
// Helper methods
	void UpdateClientArea();

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnButtonPen();
	afx_msg void OnButtonEraser();
	afx_msg void OnUpdateButtonPen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonEraser(CCmdUI* pCmdUI);
	afx_msg void OnGallerySize();
	afx_msg void OnForecolor();
	afx_msg void OnBackcolor();
};

#ifndef _DEBUG  // debug version in DrawView.cpp
inline CDrawDoc* CDrawView::GetDocument() const
   { return reinterpret_cast<CDrawDoc*>(m_pDocument); }
#endif

