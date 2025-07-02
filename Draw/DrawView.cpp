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
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
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

	if (pDoc->drawableArr.GetSize() == 0) {  // If no drawable objects yet exist, draw nothing
		return;
	}

	else
	{
		for (int i = 0; i < pDoc->drawableArr.GetSize(); i++)  // Loop through all drawable objects
		{
			pDoc->drawableArr[i]->DrawYourself(pDC);  // Call the draw method of each drawable object
		}

	}
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
	CDrawDoc* doc = GetDocument();

	switch (doc->drawingTool)
	{
	case pen:
	{
		Pen* pen = new Pen(doc->sizePen, doc->foreColor);  // Create a new Pen object with the current size and color
		CClientDC dc(this);  // Calls GetDC at construction time and ReleaseDC at destruction time

		CPen cpen(PS_SOLID, doc->sizePen, doc->foreColor);
		CPen* pOldPen = (CPen*)dc.SelectObject(&cpen);  // Select a red solid pen into the device context

		if (doc->sizePen == 1)
		{
			dc.MoveTo(point.x, point.y);
			dc.SetPixel(point.x, point.y, doc->foreColor);
		}
		else if (doc->sizePen == 2)  // If the pen size is 2, draw a 2x2 square
		{
			dc.MoveTo(point.x, point.y);
			dc.SetPixel(point.x, point.y, doc->foreColor);
			dc.SetPixel(point.x - 1, point.y, doc->foreColor);
			dc.SetPixel(point.x, point.y - 1, doc->foreColor);
			dc.SetPixel(point.x - 1, point.y - 1, doc->foreColor);
		}
		else if (doc->sizePen == 4)  // If pen size is 4, draw a 2 pixel wide cross
		{
			dc.MoveTo(point.x, point.y);
			dc.SetPixel(point.x, point.y, doc->foreColor);
			dc.SetPixel(point.x, point.y - 1, doc->foreColor);
			dc.SetPixel(point.x, point.y - 2, doc->foreColor);
			dc.SetPixel(point.x, point.y + 1, doc->foreColor);
			dc.SetPixel(point.x - 1, point.y, doc->foreColor);
			dc.SetPixel(point.x - 1, point.y - 1, doc->foreColor);
			dc.SetPixel(point.x - 1, point.y - 2, doc->foreColor);
			dc.SetPixel(point.x - 1, point.y + 1, doc->foreColor);
			dc.SetPixel(point.x - 2, point.y, doc->foreColor);
			dc.SetPixel(point.x - 2, point.y - 1, doc->foreColor);
			dc.SetPixel(point.x + 1, point.y, doc->foreColor);
			dc.SetPixel(point.x + 1, point.y - 1, doc->foreColor);
		}
		else
		{
			dc.MoveTo(point.x, point.y);  // Move the pen to the clicked point
			dc.SetDCPenColor(doc->foreColor);  // Set the pen color
			dc.LineTo(point.x, point.y);  // Draw a line to the clicked point
		}

		dc.SelectObject(pOldPen);  // Restore the old pen

		pen->addPoint(point);  // Add the point to the pen's container of points
		pen->setPrevPoint(point);  // Mark added point as previous for future reference
		doc->drawableArr.Add(pen);  // Add pen object to array of objects to be drawn
		break;
	}
	case eraser:
	{
		break;
	}
	}

	//CScrollView::OnLButtonDown(nFlags, point);
}

void CDrawView::OnLButtonUp(UINT nFlags, CPoint point)
{
	switch (GetDocument()->drawingTool)
	{
	case pen:
	{
		if (GetDocument()->penStrokeInProgress)
		{
			GetDocument()->penStrokeInProgress = FALSE;
		}
	}
	case eraser:
	{
		break;
	}
	}

	//CScrollView::OnLButtonUp(nFlags, point);
}

void CDrawView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetKeyState(VK_LBUTTON) & 0x8000)  // High order bit is set if left mouse button is pressed
	{
		switch (GetDocument()->drawingTool)
		{
		case pen:
		{
			if (GetDocument()->penStrokeInProgress)
			{
				CClientDC dc(this);  // Get a device context for the client area of the view
				CPen cpen(PS_SOLID, GetDocument()->sizePen, GetDocument()->foreColor);  // Create a solid pen with the specified size and color
				CPen* pOldPen = (CPen*)dc.SelectObject(&cpen);  // Select the created pen into the device context
				
				size_t indexLast = GetDocument()->drawableArr.GetSize() - 1;  // Get the index of the last drawable object
				Pen* pen = (Pen*)GetDocument()->drawableArr[indexLast];  // Get the current pen object
				pen->addPoint(point);  // Add the new point to the pen's container of points
				
				dc.MoveTo(pen->getPrevPoint());  // Move to the previous point
				dc.LineTo(point);  // Draw a line to the current point
				pen->setPrevPoint(point);  // Update the previous point to the current point

				dc.SelectObject(pOldPen);  // Restore the old pen
			}
			else
			{
				GetDocument()->penStrokeInProgress = TRUE;  // Mark that a pen stroke is in progress
			}
			break;
		}
		case eraser:
		{
			break;
		}
		default:
			break;
		}
	}

	//CScrollView::OnMouseMove(nFlags, point);
}
