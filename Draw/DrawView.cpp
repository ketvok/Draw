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
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CDrawView construction/destruction

CDrawView::CDrawView() noexcept : isDrawing{ FALSE }
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

	////////////////
	// NOT PRINTING
	if (!pDC->IsPrinting())
	{	
		// Solve the problem of image not being drawn correctly while scrolling.
		// (artifacting, white background, etc.)
		CRect clipBoxRect; pDC->GetClipBox(&clipBoxRect);
		pDC->FillSolidRect(clipBoxRect, RGB(220, 230, 240));  // Same color as area around canvas

		// Draw area around canvas
		CRect clientRect; GetClientRect(&clientRect);
		pDC->FillSolidRect(clientRect, RGB(220, 230, 240));

		// Draw canvas shadow
		CRect canvasShadowRect(paddingHorizontal * 2.5,
			                   paddingVertical * 2.5,
			                   paddingHorizontal * 2.5 + canvasSize.cx,
			                   paddingVertical * 2.5 + canvasSize.cy);
		pDC->FillSolidRect(canvasShadowRect, RGB(210, 220, 230));

		// Draw canvas
		canvasRect.left = paddingHorizontal;
		canvasRect.top = paddingVertical;
		canvasRect.right = paddingHorizontal + canvasSize.cx;
		canvasRect.bottom = paddingVertical + canvasSize.cy;
		pDC->FillSolidRect(canvasRect, RGB(255, 255, 255));

		// Draw canvas resize handle
		CRect resizeHandleRect(paddingHorizontal + canvasSize.cx,
			                   paddingVertical + canvasSize.cy,
			                   paddingHorizontal * 2 + canvasSize.cx,
			                   paddingVertical * 2 + canvasSize.cy);
		pDC->FillSolidRect(resizeHandleRect, RGB(255, 255, 255));

		// Draw a frame for canvas resize handle
		CPen handlePen(PS_SOLID, 1, RGB(64, 64, 64));
		CPen* pOldPen = pDC->SelectObject(&handlePen);

		pDC->Rectangle(resizeHandleRect.left,
			           resizeHandleRect.top,
			           resizeHandleRect.right,
			           resizeHandleRect.bottom);
		pDC->SelectObject(pOldPen);


		//*****************************************************************************************
		// CANVAS CLIPPING REGION
		// 
		// For some reason this does not work if it's in OnPrepareDC instead of here.
		// Set up clipping region to the canvas
		CRect clipRect = canvasRect;
		CRgn clipRgn; clipRgn.CreateRectRgnIndirect(&clipRect);
		
		// Offset the clipping region by the scroll position.
		CPoint scrollPos = GetScrollPosition();
		clipRgn.OffsetRgn(-scrollPos.x, -scrollPos.y);
		CRect clipRgnRect;
		GetRgnBox(clipRgn, &clipRgnRect);
		
		// Select clipping region into DC
		int savedDC = pDC->SaveDC();																			
		pDC->SelectClipRgn(&clipRgn);
		//*****************************************************************************************

		pDoc->DrawAll(pDC);


		// Cleanup
		pDC->RestoreDC(savedDC);
	}

	////////////
	// PRINTING
	else  // If printing, draw the document content without any background
	{
		// Get the printable area dimensions
		int printWidth = pDC->GetDeviceCaps(HORZRES);  // e.g. 2480  // [ A4 width(8.27 in) * 300 dpi ]
		int printHeight = pDC->GetDeviceCaps(VERTRES);  // e.g. 3508  // [ A4 height(11.7 in) * 300 dpi ]

		// Calculate the scaling factor to fit the canvas on the page
		// while maintaining the aspect ratio

		// E.g. If the paper has n times larger horizontal resolution than the canvas, and >n
		// times larger vertical resolution, then the viewport is set to be the same horizontal
		// size, and vertical size n times larger than the canvas. That way the canvas is drawn
		// over the entire width of the paper and in the correct aspect ratio.

		double scaleX = static_cast<double>(printWidth) / canvasSize.cx;  // e.g 4.1333 // [ 2480 / 600 ]
		double scaleY = static_cast<double>(printHeight) / canvasSize.cy;  // e.g 8.769 // [ 3508 / 400 ]
		double scaleFactor = min(scaleX, scaleY);

		// Calculate the new viewport extent that preserves aspect ratio
		int viewportWidth = static_cast<int>(canvasSize.cx * scaleFactor);
		int viewportHeight = static_cast<int>(canvasSize.cy * scaleFactor);

		int savedDC;

		/////////////////
		// PRINT PREVIEW
		if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
		{
		//*****************************************************************************************
		// PRINT PREVIEW CLIPPING REGION
		// 
		// Copied from: https://www.codeproject.com/Articles/5377/Using-CRgn-with-print-preview
		// For some reason this works better if it's here instead of OnPrepareDC.
		// If it's there then the clipping region is applied only after window resize, and if it's
		// here then it is applied immediately after the print preview window is opened.
		CRect clipRect(0, 0, viewportWidth, viewportHeight);
		CPreviewDC* pPrevDC = static_cast<CPreviewDC*>(pDC);

		// Preview uses 2 different DC's: printer DC and output DC
		// Translate the clip region in use from the printer context to the output context
		pPrevDC->PrinterDPtoScreenDP(&clipRect.TopLeft());
		pPrevDC->PrinterDPtoScreenDP(&clipRect.BottomRight());
		// Now offset the result by the viewport origin of
		// the print preview window...
		CPoint ptOrg; ::GetViewportOrgEx(pDC->m_hDC, &ptOrg);
		clipRect.OffsetRect(ptOrg.x, ptOrg.y);  // Offset the clipping rect. by the viewport origin
		
		CRgn rgn;
		VERIFY(rgn.CreateRectRgnIndirect(clipRect));
		savedDC = pDC->SaveDC();  // Save the current state of the device context
		pDC->SelectClipRgn(&rgn);  // Set the clipping region to the calculated rectangle
		//*****************************************************************************************
		}
		else
		{
			//*************************************************************************************
			// PRINTING CLIPPING REGION
			// 
			// This works the same weather it's in OnPrepareDC or here, but I prefer it here for
			// consistency of keeping all clipping region code in one place.
			CRect clipRect(0, 0, viewportWidth, viewportHeight);
			CRgn clipRgn; VERIFY(clipRgn.CreateRectRgnIndirect(&clipRect));
			savedDC = pDC->SaveDC();
			pDC->SelectClipRgn(&clipRgn);
			//*************************************************************************************
		}

		pDoc->DrawAll(pDC);

		pDC->RestoreDC(savedDC);
	}
}

void CDrawView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	//CSize sizeTotal;
	//// TODO: calculate the total size of this view
	//sizeTotal.cx = sizeTotal.cy = 100;
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
	// Framework calls OnPreparePrinting passing a pointer to CPrintInfo structure. 

	// default preparation								
	return DoPreparePrinting(pInfo);

	// DoPreparePrinting displays the Print dialog box and creates a printer device context.
	// When it returns, the CPrintInfo structure contains the values specified by the user.

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

	// Capture the mouse input to this window so that it receives all
	// mouse messages. This is necessary to receive mouse-move messages
	// past the edge of the window.
	SetCapture();
	
	isDrawing = TRUE;  // Mark that drawing has started

	CDrawDoc* pDoc = GetDocument();
	CClientDC dc(this);  // Calls GetDC at construction time and ReleaseDC at destruction time

	OnPrepareDC(&dc);
	dc.DPtoLP(&point);  // Convert the point to logical coordinates

	CPoint scrollPos = GetScrollPosition();  // Get the current scroll position
	CRect clipRect(paddingHorizontal - scrollPos.x,
		           paddingVertical - scrollPos.y,
		           paddingHorizontal + canvasSize.cx - scrollPos.x,
		           paddingVertical + canvasSize.cy - scrollPos.y);
	//dc.DPtoLP(&clipRect);  // DO NOT DO THIS.
	CRgn clipRgn;
	clipRgn.CreateRectRgnIndirect(&clipRect);
	int savedDC = dc.SaveDC();  // Save the current state of the device context
	dc.SelectClipRgn(&clipRgn);

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
		dc.RestoreDC(savedDC);  // Restore the device context to its previous state

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
		Rectangle(dc.m_hDC,
			      point.x - pDoc->GetSizeEraser() / 2,
			      point.y - pDoc->GetSizeEraser() / 2,
			      point.x + pDoc->GetSizeEraser() / 2,
			      point.y + pDoc->GetSizeEraser() / 2);

		// Cleanup
		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);

		break;
	}
	}  // End switch
}

void CDrawView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!isDrawing)  // If drawing is not in progress, do nothing.
		return;

	CDrawDoc* pDoc = GetDocument();

	CClientDC dc(this);  // Calls GetDC at construction time and ReleaseDC at destruction time

	OnPrepareDC(&dc);
	dc.DPtoLP(&point);  // Convert the point to logical coordinates

	CPoint scrollPos = GetScrollPosition();  // Get the current scroll position
	CRect clipRect(paddingHorizontal - scrollPos.x,
		           paddingVertical - scrollPos.y,
		           paddingHorizontal + canvasSize.cx - scrollPos.x,
		           paddingVertical + canvasSize.cy - scrollPos.y);
	//dc.DPtLP(&clipRect);  // DO NOT DO THIS.
	CRgn clipRgn;
	clipRgn.CreateRectRgnIndirect(&clipRect);
	int savedDC = dc.SaveDC();
	dc.SelectClipRgn(&clipRgn);

	if (GetKeyState(VK_LBUTTON) & 0x8000)  // If the high order bit of return value is set,
	{                                      //  the left mouse button is down.
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
			Rectangle(dc.m_hDC,
				      point.x - pDoc->GetSizeEraser() / 2,
				      point.y - pDoc->GetSizeEraser() / 2,
				      point.x + pDoc->GetSizeEraser() / 2,
				      point.y + pDoc->GetSizeEraser() / 2);

			// Cleanup
			dc.SelectObject(pOldPen);

			break;
		}
		}  // End switch
	}
	dc.RestoreDC(savedDC);  // Restore the device context to its previous state
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
	}  // End switch

	return TRUE;
}
void CDrawView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// Releases the mouse capture from OnLButtonDown.
	ReleaseCapture();

	isDrawing = FALSE;
}

void CDrawView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// Called by the framework before the OnDraw member function is called for screen display and
	// before the OnPrint member function is called for each page during printing or print preview.

	CScrollView::OnPrepareDC(pDC, pInfo);
	pDC->SetMapMode(MM_ISOTROPIC);

	CPoint scrollPos = GetScrollPosition();

	// PRINTING & PRINT PREVIEW
	if (pDC->IsPrinting())
	{
		// Set window to match the canvas content area (logical coordinates)
		pDC->SetWindowOrg(paddingHorizontal, paddingVertical);  // E.g. (6, 6)  // [ 96(dpi) * 1/16 ]
		pDC->SetWindowExt(canvasSize.cx, canvasSize.cy);  // E.g. (640, 480)  // [ hardcoded ]

		// Get the device capabilities for the printable area
		int printWidth = pDC->GetDeviceCaps(HORZRES);  // Printable width in device units // E.g. 2480  // [ A4 width(8.27 in) * 300 dpi ]
		int printHeight = pDC->GetDeviceCaps(VERTRES); // Printable height in device units // E.g. 3508  // [ A4 height(11.7 in) * 300 dpi ]

		// Calculate the scaling factor to fit the canvas on the page
		// while maintaining the aspect ratio
		double scaleX = static_cast<double>(printWidth) / canvasSize.cx;  // E.g. 4.1333 // [ 2480 / 600 ]
		double scaleY = static_cast<double>(printHeight) / canvasSize.cy;  // E.g. 8.769 // [ 3508 / 400 ]
		double scaleFactor = min(scaleX, scaleY);

		// Calculate the new viewport extent that preserves aspect ratio
		int viewportWidth = static_cast<int>(canvasSize.cx * scaleFactor);
		int viewportHeight = static_cast<int>(canvasSize.cy * scaleFactor);

		pDC->SetViewportOrg(0, 0);
		pDC->SetViewportExt(viewportWidth, viewportHeight);  // Set the viewport extent to the calculated size
	}

	// NOT PRINTING
	else
	{
		pDC->SetWindowOrg(paddingHorizontal + scrollPos.x, paddingVertical + scrollPos.y);
		pDC->SetWindowExt(canvasSize.cx, canvasSize.cy);

		pDC->SetViewportOrg(paddingHorizontal, paddingVertical);
		pDC->SetViewportExt(canvasSize.cx, canvasSize.cy);
	}
}

void CDrawView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// Get pixels per inch for horizontal and vertical directions of the device (screen)
	int xLogPixelsPerInch = GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSX);
	int yLogPixelsPerInch = GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY);

	// Set the padding for the canvas based on the device's pixel density
	paddingHorizontal = xLogPixelsPerInch / 16;
	paddingVertical = yLogPixelsPerInch / 16;

	// Get the width of the vertical and height of the horizontal scroll bar
	int vScrollBarWidth = GetSystemMetrics(SM_CXVSCROLL);
	int hScrollBarHeight = GetSystemMetrics(SM_CYHSCROLL);

	// Set the size of the canvas based on the client area size, making sure it does not get
	// obstructed by the scroll bars
	CRect clientRect; GetClientRect(&clientRect);
	canvasSize.cx = clientRect.Width() - paddingHorizontal * 2 - vScrollBarWidth;
	canvasSize.cy = clientRect.Height() - paddingVertical * 2 - hScrollBarHeight;

	// Set the scrollable area size accounting for padding and scroll bar sizes
	CSize scrollSize;
	scrollSize.cx = canvasSize.cx + paddingHorizontal * 2 + vScrollBarWidth;  // Width of the scrollable area
	scrollSize.cy = canvasSize.cy + paddingVertical * 2 + hScrollBarHeight;  // Height of the scrollable area
	SetScrollSizes(MM_TEXT, scrollSize);
}
