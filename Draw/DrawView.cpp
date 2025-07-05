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
#include "Pen.h"
#include "Eraser.h"

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
	ON_COMMAND(ID_BACKCOLOR, &CDrawView::OnBackcolor)
	ON_COMMAND(ID_GALLERY_SHAPES, &CDrawView::OnGalleryShapes)
	ON_COMMAND(ID_GALLERY_COLORS, &CDrawView::OnGalleryColors)
	ON_COMMAND(ID_OUTLINE, &CDrawView::OnOutline)
	ON_COMMAND(ID_FILL, &CDrawView::OnFill)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
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

	pDoc->DrawAll(pDC);
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

void CDrawView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// As soon as the left mouse button is pressed down, a new
	// Drawable object is created.

	CDrawDoc* pDoc = GetDocument();
	CClientDC dc(this);  // Calls GetDC at construction time and ReleaseDC at destruction time

	switch (pDoc->GetDrawingTool())
	{
	case pen:
	{
		// For pen sizes >= 5, MoveTo/LineTo produce good results for drawing
		// a dot, but for smaller sizes I do it manually for nicer results.

		// Create a new Drawable Pen object
		auto pen = std::make_shared<Pen>(pDoc->GetSizePen(), pDoc->foreColor);

		pDoc->AddObject(pen);
		pDoc->AddPoint(point);
		pDoc->SetPrevPoint(point);  // Mark added point as previous for future reference

		// Select a pen with the current size and color
		CPen cpen(PS_SOLID, pDoc->GetSizePen(), pDoc->foreColor);
		CPen* pOldPen = (CPen*)dc.SelectObject(&cpen);

		if (pDoc->GetSizePen() == 1)
		{
			dc.MoveTo(point.x, point.y);
			dc.SetPixel(point.x, point.y, pDoc->foreColor);
		}
		else if (pDoc->GetSizePen() == 2)  // If pen size is 2, draw a 2 x 2 square
		{
			dc.MoveTo(point.x, point.y);
			dc.SetPixel(point.x, point.y, pDoc->foreColor);
			dc.SetPixel(point.x - 1, point.y, pDoc->foreColor);
			dc.SetPixel(point.x, point.y - 1, pDoc->foreColor);
			dc.SetPixel(point.x - 1, point.y - 1, pDoc->foreColor);
		}
		else if (pDoc->GetSizePen() == 4)  // If pen size is 4, draw a 2 pixel wide cross
		{
			dc.MoveTo(point.x, point.y);
			dc.SetPixel(point.x, point.y, pDoc->foreColor);
			dc.SetPixel(point.x, point.y - 1, pDoc->foreColor);
			dc.SetPixel(point.x, point.y - 2, pDoc->foreColor);
			dc.SetPixel(point.x, point.y + 1, pDoc->foreColor);
			dc.SetPixel(point.x - 1, point.y, pDoc->foreColor);
			dc.SetPixel(point.x - 1, point.y - 1, pDoc->foreColor);
			dc.SetPixel(point.x - 1, point.y - 2, pDoc->foreColor);
			dc.SetPixel(point.x - 1, point.y + 1, pDoc->foreColor);
			dc.SetPixel(point.x - 2, point.y, pDoc->foreColor);
			dc.SetPixel(point.x - 2, point.y - 1, pDoc->foreColor);
			dc.SetPixel(point.x + 1, point.y, pDoc->foreColor);
			dc.SetPixel(point.x + 1, point.y - 1, pDoc->foreColor);
		}
		else  // If pen size is > 4, use MoveTo/LineTo for drawing a dot at first point
		{
			dc.MoveTo(point.x, point.y);
			dc.SetDCPenColor(pDoc->foreColor);
			dc.LineTo(point.x, point.y);
		}

		// Cleanup
		dc.SelectObject(pOldPen);

		break;
	}
	case eraser:
	{
		// Create a new Drawable Eraser object
		auto eraser = std::make_shared<Eraser>(pDoc->GetSizeEraser(), pDoc->backColor);

		pDoc->AddObject(eraser);
		pDoc->AddPoint(point);

		// Select a pen and brush with the current size and color
		CPen cpen(PS_SOLID, 1, pDoc->backColor);
		CPen* pOldPen = (CPen*)dc.SelectObject(&cpen);
		CBrush cbrush(pDoc->backColor);
		CBrush* pOldBrush = (CBrush*)dc.SelectObject(&cbrush);

		// Draw a rectangle at the current point with the size of the eraser
		Rectangle(dc.m_hDC, point.x - pDoc->GetSizeEraser() / 2, point.y - pDoc->GetSizeEraser() / 2, point.x + pDoc->GetSizeEraser() / 2, point.y + pDoc->GetSizeEraser() / 2);

		// Cleanup
		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);

		break;
	}
	}
}

void CDrawView::OnMouseMove(UINT nFlags, CPoint point)
{

	CDrawDoc* pDoc = GetDocument();

	// myTODO: if drawableArr is empty, create a new Drawable object inside switch statement! +++++++++++++++++ASSERT @ DrawDoc.cpp Line 72+++++++++++++++++++++++++++++
	// OR
	// Do it like in microsoft Paint: when it is clicked on the ribbon and dragged into the canvas, nothing happens and cursor remains arrow.

	CClientDC dc(this);  // Calls GetDC at construction time and ReleaseDC at destruction time

	if (GetKeyState(VK_LBUTTON) & 0x8000)  // If the high order bit of return value is set, the left mouse button is down
	{
		switch (pDoc->GetDrawingTool())
		{
		case pen:
		{
			// Add the current point to the last Drawable object (Pen)
			pDoc->AddPoint(point);

			// Select a pen with the current size and color
			CPen cpen(PS_SOLID, pDoc->GetSizePen(), pDoc->foreColor);
			CPen* pOldPen = (CPen*)dc.SelectObject(&cpen);
			
			// Draw a line from previous point to the current point
			dc.MoveTo(pDoc->GetPrevPoint());
			dc.LineTo(point);
			
			// Set the current point as previous
			pDoc->SetPrevPoint(point);

			// Cleanup
			dc.SelectObject(pOldPen);
			
			break;
		}
		case eraser:
		{
			// Add the current point to the last Drawable object (Eraser)
			pDoc->AddPoint(point);

			// Select a pen and brush with the background color
			CPen cpen(PS_SOLID, 1, pDoc->backColor);
			CPen* pOldPen = (CPen*)dc.SelectObject(&cpen);
			CBrush cbrush(pDoc->backColor);
			CBrush* pOldBrush = (CBrush*)dc.SelectObject(&cbrush);

			// Draw a rectangle at the current point with the size of the eraser
			Rectangle(dc.m_hDC, point.x - pDoc->GetSizeEraser() / 2, point.y - pDoc->GetSizeEraser() / 2, point.x + pDoc->GetSizeEraser() / 2, point.y + pDoc->GetSizeEraser() / 2);

			// Cleanup
			dc.SelectObject(pOldPen);

			break;
		}
		}
	}
}

BOOL CDrawView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// Each time the cursor moves and mouse input is not captured, the
	// system sends a WM_SETCURSOR message to the window in which the
	// cursor is moving.

	HINSTANCE hInstance = AfxGetInstanceHandle();
	
	switch (GetDocument()->GetDrawingTool())
	{
	case pen:
	{
		HCURSOR curPen;
		curPen = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_PEN_CURSOR));
		SetCursor(curPen);
		break;
	}
	case eraser:
	{
		HCURSOR curEraser;
		curEraser = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_ERASER_CURSOR));
		SetCursor(curEraser);
		break;
	}
	}

	return TRUE;
}