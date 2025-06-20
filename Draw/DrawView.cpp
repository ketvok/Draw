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

// DrawView.cpp : implementation of the CDrawView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Draw.h"
#endif

#include "DrawDoc.h"
#include "DrawView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDrawView

IMPLEMENT_DYNCREATE(CDrawView, CScrollView)

BEGIN_MESSAGE_MAP(CDrawView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CDrawView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_FORECOLOR, &CDrawView::OnForecolor)
	ON_COMMAND(ID_GALLERY_SIZE, &CDrawView::OnGallerySize)
	ON_COMMAND(ID_BACKCOLOR, &CDrawView::OnBackcolor)
	ON_COMMAND(ID_GALLERY_SHAPES, &CDrawView::OnGalleryShapes)
	ON_COMMAND(ID_GALLERY_COLORS, &CDrawView::OnGalleryColors)
	ON_COMMAND(ID_BUTTON_PEN, &CDrawView::OnButtonPen)
	ON_COMMAND(ID_BUTTON_ERASER, &CDrawView::OnButtonEraser)
	ON_COMMAND(ID_OUTLINE, &CDrawView::OnOutline)
	ON_COMMAND(ID_FILL, &CDrawView::OnFill)
END_MESSAGE_MAP()

// CDrawView construction/destruction

CDrawView::CDrawView() noexcept
{
	// TODO: add construction code here
}

CDrawView::~CDrawView()
{
}

BOOL CDrawView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

// CDrawView drawing

void CDrawView::OnDraw(CDC* pDC)
{
    CDrawDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;
}

void CDrawView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


// CDrawView printing


void CDrawView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CDrawView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDrawView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDrawView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CDrawView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CDrawView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CDrawView diagnostics

#ifdef _DEBUG
void CDrawView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CDrawView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CDrawDoc* CDrawView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDrawDoc)));
	return (CDrawDoc*)m_pDocument;
}
#endif //_DEBUG


// CDrawView message handlers

void CDrawView::OnGallerySize()
{
	// TODO: Add your command handler code here
	MessageBox(_T("Size selected!"), _T("Size Selection"), MB_OK | MB_ICONINFORMATION);
}

void CDrawView::OnForecolor()
{
	// TODO: Add your command handler code here
	MessageBox(_T("Color 1 selected!"), _T("Color 1 Selection"), MB_OK | MB_ICONINFORMATION);
}

void CDrawView::OnBackcolor()
{
	// TODO: Add your command handler code here
	MessageBox(_T("Color 2 selected!"), _T("Color 2 Selection"), MB_OK | MB_ICONINFORMATION);
}

void CDrawView::OnGalleryShapes()
{
	// TODO: Add your command handler code here
	MessageBox(_T("Shape selected!"), _T("Shape Selection"), MB_OK | MB_ICONINFORMATION);
}

void CDrawView::OnGalleryColors()
{
	// TODO: Add your command handler code here
	MessageBox(_T("Color selected!"), _T("Color Selection"), MB_OK | MB_ICONINFORMATION);
}

void CDrawView::OnButtonPen()
{
	// TODO: Add your command handler code here
	MessageBox(_T("Pen button clicked!"), _T("Pen Selection"), MB_OK | MB_ICONINFORMATION);
}

void CDrawView::OnButtonEraser()
{
	// TODO: Add your command handler code here
	MessageBox(_T("Eraser button clicked!"), _T("Eraser Selection"), MB_OK | MB_ICONINFORMATION);
}

void CDrawView::OnOutline()
{
	// TODO: Add your command handler code here
	MessageBox(_T("Outline button clicked!"), _T("Outline Selection"), MB_OK | MB_ICONINFORMATION);
}

void CDrawView::OnFill()
{
	// TODO: Add your command handler code here
	MessageBox(_T("Fill button clicked!"), _T("Fill Selection"), MB_OK | MB_ICONINFORMATION);
}
