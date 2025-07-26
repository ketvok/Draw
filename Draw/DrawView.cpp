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
#include <cmath>

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
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CDrawView construction/destruction

CDrawView::CDrawView() noexcept : drawingMode{ TRUE }, resizingMode{ FALSE }, strokeInProgress{ FALSE }, paddingHorizontal { 0 }, paddingVertical { 0 }, bitmapInitialized{ FALSE }
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


////////////////////////////////////////////////////////////////////////
//                        CDrawView drawing                           //
////////////////////////////////////////////////////////////////////////

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
		CRect clientRect; GetClientRect(&clientRect);

		if (memDC.m_hDC != NULL)
		{
			// Get the current scroll position
			CPoint scrollPos = GetScrollPosition();
			// Offset the client rectangle by the scroll position
			clientRect.OffsetRect(scrollPos);
			
			// Copy the memory DC to the device context
			VERIFY(pDC->BitBlt(0, 0, clientRect.Width(), clientRect.Height(),
				&memDC, scrollPos.x, scrollPos.y, SRCCOPY));
		}
	}


	////////////
	// PRINTING
	else  // If printing, copy only anvas area to the device context
	{
		// Get the printable area dimensions
		int printWidth = pDC->GetDeviceCaps(HORZRES);
		int printHeight = pDC->GetDeviceCaps(VERTRES);

		// Calculate the scaling factor to fit the canvas on the page
		// while maintaining the aspect ratio.

		// E.g. if the paper has n times larger horizontal resolution than the canvas, and >n
		// times larger vertical resolution, then the viewport is set to be the same horizontal
		// size, and vertical size n times larger than the canvas. That way the canvas is drawn
		// over the entire width of the paper and in the correct aspect ratio.

		double scaleX = static_cast<double>(printWidth) / canvasSize.cx;  // e.g 4.1333 // [ 2480 / 600 ]
		double scaleY = static_cast<double>(printHeight) / canvasSize.cy;  // e.g 8.769 // [ 3508 / 400 ]
		double scaleFactor = min(scaleX, scaleY);

		// Calculate the new viewport extent that preserves aspect ratio
		int viewportWidth = static_cast<int>(std::round(canvasSize.cx * scaleFactor));
		int viewportHeight = static_cast<int>(std::round(canvasSize.cy * scaleFactor));


		// Copy the memory DC to the device context
		pDC->StretchBlt(0, 0, viewportWidth, viewportHeight,
			&memDC, paddingHorizontal, paddingVertical, canvasSize.cx, canvasSize.cy, SRCCOPY);
	}
}

void CDrawView::OnInitialUpdate()
{
	CClientDC dc(this);
	CRect clientRect; GetClientRect(&clientRect);

	// Initialize the memory DC and bitmap for drawing
    VERIFY(drawingBitmap.CreateCompatibleBitmap(&dc, GetDeviceCaps(dc, HORZRES), GetDeviceCaps(dc, VERTRES)));
	memDC.CreateCompatibleDC(&dc);
	memDC.SelectObject(&drawingBitmap);
	bitmapInitialized = TRUE;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// CALCULATE CANVAS SIZE AND SCROLLING AREA

	// Calculate the size of padding for the canvas based on the device's pixel density
	int xLogPixelsPerInch = GetDeviceCaps(memDC, LOGPIXELSX);
	int yLogPixelsPerInch = GetDeviceCaps(memDC, LOGPIXELSY);
	paddingHorizontal = xLogPixelsPerInch / 16;
	paddingVertical = yLogPixelsPerInch / 16;

	// Calculate the size of the canvas based on the client area size, making sure it does not get
	// obstructed by the scroll bars
	int vScrollBarWidth = GetSystemMetrics(SM_CXVSCROLL);
	int hScrollBarHeight = GetSystemMetrics(SM_CYHSCROLL);
	canvasSize.cx = clientRect.Width()  - paddingHorizontal * 2 - vScrollBarWidth;
	canvasSize.cy = clientRect.Height() - paddingVertical   * 2 - hScrollBarHeight;
	
	// Set the canvas rectangle to the calculated size and position
	canvasRect.SetRect(
		paddingHorizontal,                   // left
		paddingVertical,                     // top
		paddingHorizontal + canvasSize.cx,   // right
		paddingVertical   + canvasSize.cy);  // bottom
	
	trackRect = canvasRect;

	// Calculate the scrollable area accounting for padding and width of scroll bars
	CSize scrollSize;
	scrollSize.cx = canvasSize.cx + paddingHorizontal * 2 + vScrollBarWidth;
	scrollSize.cy = canvasSize.cy + paddingVertical * 2 + hScrollBarHeight;
	SetScrollSizes(MM_TEXT, scrollSize);

	CScrollView::OnInitialUpdate();
}


////////////////////////////////////////////////////////////////////////
//                        CDrawView printing                          //
////////////////////////////////////////////////////////////////////////

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


////////////////////////////////////////////////////////////////////////
//                       CDrawView diagnostics                        //
////////////////////////////////////////////////////////////////////////

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


////////////////////////////////////////////////////////////////////////
//                   CDrawView message handlers                       //
////////////////////////////////////////////////////////////////////////

void CDrawView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// As soon as the left mouse button is pressed down, a new
	// Drawable object is created.

	// Capture the mouse input to this window so that it receives all
	// mouse messages. This is necessary to receive mouse-move messages
	// past the edge of the window.

	CPoint scrollPos = GetScrollPosition();  // Get current scroll position
		
	CRect adjustedResizeHandleRect = resizeHandleRect;
	adjustedResizeHandleRect.OffsetRect(-scrollPos.x, -scrollPos.y);  // Adjust the rectangle by the scroll position
	
	// Clicked inside the resize handle
	if (adjustedResizeHandleRect.PtInRect(point))
	{
		resizingMode = TRUE;
		drawingMode = FALSE;
		Invalidate();  // Prepare DC for resizing.

		CRectTracker rectTracker;

		// startPoint is in client coordinates, so it needs to be adjusted by the scroll position
		CPoint startPoint(canvasRect.left - scrollPos.x, canvasRect.top - scrollPos.y);

		if (rectTracker.TrackRubberBand(this, startPoint, FALSE))
		{
			// Get the tracker rectangle in client coordinates
			trackRect = rectTracker.m_rect;

			resizingMode = FALSE;
			drawingMode = TRUE;

			// Update the scroll sizes and redraw the view
			GetDocument()->UpdateAllViews(NULL, 1, NULL);
		}
	}
	// Clicked inside the canvas
	else
	{
		CPoint scrollPos = GetScrollPosition();
		point.Offset(scrollPos.x, scrollPos.y);  // Offset the point by the scroll position

		if (!canvasRect.PtInRect(point))  // If clicked outside canvas,
		{
			return;  // do nothing.
		}

		SetCapture();

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
			strokeInProgress = TRUE;

			// Select a pen with the current size and color
			CPen cpen(PS_SOLID, pDoc->GetSizePen(), pDoc->foreColor);
			CPen* pOldPen = memDC.SelectObject(&cpen);

			if (pDoc->GetSizePen() == 1)
			{
				memDC.MoveTo(point.x, point.y);
				memDC.SetPixel(point.x, point.y, pDoc->foreColor);
			}
			else if (pDoc->GetSizePen() == 2)  // If pen size is 2, draw a 2 x 2 square
			{
				memDC.MoveTo(point.x, point.y);
				memDC.SetPixel(point.x, point.y, pDoc->foreColor);
				memDC.SetPixel(point.x - 1, point.y, pDoc->foreColor);
				memDC.SetPixel(point.x, point.y - 1, pDoc->foreColor);
				memDC.SetPixel(point.x - 1, point.y - 1, pDoc->foreColor);
			}
			else if (pDoc->GetSizePen() == 4)  // If pen size is 4, draw a 2 pixel wide cross
			{
				memDC.MoveTo(point.x, point.y);
				memDC.SetPixel(point.x, point.y, pDoc->foreColor);
				memDC.SetPixel(point.x, point.y - 1, pDoc->foreColor);
				memDC.SetPixel(point.x, point.y - 2, pDoc->foreColor);
				memDC.SetPixel(point.x, point.y + 1, pDoc->foreColor);
				memDC.SetPixel(point.x - 1, point.y, pDoc->foreColor);
				memDC.SetPixel(point.x - 1, point.y - 1, pDoc->foreColor);
				memDC.SetPixel(point.x - 1, point.y - 2, pDoc->foreColor);
				memDC.SetPixel(point.x - 1, point.y + 1, pDoc->foreColor);
				memDC.SetPixel(point.x - 2, point.y, pDoc->foreColor);
				memDC.SetPixel(point.x - 2, point.y - 1, pDoc->foreColor);
				memDC.SetPixel(point.x + 1, point.y, pDoc->foreColor);
				memDC.SetPixel(point.x + 1, point.y - 1, pDoc->foreColor);
			}
			else  // If pen size is > 4, use MoveTo/LineTo for drawing a dot
			{
				memDC.MoveTo(point.x, point.y);
				memDC.SetDCPenColor(pDoc->foreColor);
				memDC.LineTo(point.x, point.y);
			}

			// Cleanup
			memDC.SelectObject(pOldPen);

			break;
		}
		case eraser:
		{
			// Create a new Drawable Eraser object
			auto eraser = std::make_shared<Eraser>(pDoc->GetSizeEraser(), pDoc->backColor);

			pDoc->AddObject(eraser);
			pDoc->AddPoint(point);
			pDoc->SetPrevPoint(point);  // Mark added point as previous for future reference
			strokeInProgress = TRUE;

			// Select a pen and brush with the current size and color
			CPen cpen(PS_SOLID, 1, pDoc->backColor);
			CPen* pOldPen = memDC.SelectObject(&cpen);
			CBrush cbrush(pDoc->backColor);
			CBrush* pOldBrush = memDC.SelectObject(&cbrush);

			// Draw a rectangle at the current point with the size of the eraser
			Rectangle(
				memDC.m_hDC,
				point.x - pDoc->GetSizeEraser() / 2,   // left
				point.y - pDoc->GetSizeEraser() / 2,   // top
				point.x + pDoc->GetSizeEraser() / 2,   // right
				point.y + pDoc->GetSizeEraser() / 2);  // bottom

			// Cleanup
			memDC.SelectObject(pOldPen);
			memDC.SelectObject(pOldBrush);

			break;
		}
		}  // End switch
		
		Invalidate();
	}  // End if-else
}

void CDrawView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!drawingMode)  // If drawing is not in progress, do nothing.
		return;

	CDrawDoc* pDoc = GetDocument();

	CClientDC dc(this);

	CPoint scrollPos = GetScrollPosition();
	point.Offset(scrollPos.x, scrollPos.y);  // Offset the point by the scroll position

	if (GetKeyState(VK_LBUTTON) & 0x8000 && strokeInProgress == TRUE)
		// If the high order bit of return value is set,
		//  the left mouse button is down.
	{
		switch (pDoc->GetDrawingTool())
		{
		case pen:
		{
			// Add the current point to the last Drawable object (Pen)
			pDoc->AddPoint(point);

			// Select a pen with the current size and color
			CPen cpen(PS_SOLID, pDoc->GetSizePen(), pDoc->foreColor);
			CPen* pOldPen = memDC.SelectObject(&cpen);
			
			// Draw a line from previous point to the current point
			memDC.MoveTo(pDoc->GetPrevPoint());
			memDC.LineTo(point);
			
			// Set the current point as previous
			pDoc->SetPrevPoint(point);

			// Cleanup
			memDC.SelectObject(pOldPen);
			
			break;
		}
		case eraser:
		{
			// Add the current point to the last Drawable object (Eraser)
			pDoc->AddPoint(point);

			// Select a pen and brush with the background color
			CPen cpen(PS_SOLID, 1, pDoc->backColor);
			CPen* pOldPen = memDC.SelectObject(&cpen);
			CBrush cbrush(pDoc->backColor);
			CBrush* pOldBrush = memDC.SelectObject(&cbrush);

			// Calculate the points in between the previous point and the current point
			CPoint prevPoint = pDoc->GetPrevPoint();
			int dx = point.x - prevPoint.x;     // Difference in x coordinates
			int dy = point.y - prevPoint.y;     // Difference in y coordinates
			int steps = max(abs(dx), abs(dy));  // Number of steps needed for the line

			// Draw a "line" from the previous point to the current point
			for (int i = 1; i <= steps; ++i)
			{
				// Calculate the interpolated point based on the step
				int x = prevPoint.x + dx * i / steps;
				int y = prevPoint.y + dy * i / steps;
					
				// Draw a rectangle at the interpolated point
				int eraserSize = pDoc->GetSizeEraser();
				Rectangle(
					memDC.m_hDC,
					x - eraserSize / 2,   // left
					y - eraserSize / 2,   // top
					x + eraserSize / 2,   // right
					y + eraserSize / 2);  // bottom

				// Add the interpolated point to the last Drawable object (Eraser)
				pDoc->AddPoint(point);
			}

			// Set the current point as previous
			pDoc->SetPrevPoint(point);

			// Cleanup
			memDC.SelectObject(pOldPen);
			memDC.SelectObject(pOldBrush);

			break;
		}
		}  // End switch
		Invalidate();
	}
}

BOOL CDrawView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// Each time the cursor moves and mouse input is not captured, the
	// system sends a WM_SETCURSOR message to the window in which the
	// cursor is moving.

	// Get current cursor position (in screen coordinates)
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	// Add scroll offset
	CPoint scrollPos = GetScrollPosition();

	CRect adjustedResizeHandleRect = resizeHandleRect;
	adjustedResizeHandleRect.OffsetRect(-scrollPos.x, -scrollPos.y);

	CRect adjustedCanvasRect = canvasRect;
	adjustedCanvasRect.OffsetRect(-scrollPos.x, -scrollPos.y);
	
	// It is important that checks are done in a particular order,
	// otherwise the cursor may not change correctly:

	// 1. If cursor is over a scroll bar,
	if (nHitTest == HTHSCROLL || nHitTest == HTVSCROLL)
	{
		// set the default arrow cursor.
		HCURSOR curArrow;
		curArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		SetCursor(curArrow);
	}
	// 2. If cursor is over a client area, but not over a scroll bar
	else if (nHitTest == HTCLIENT)
	{
		// If cursor is over canvas,
		if (adjustedCanvasRect.PtInRect(point))
		{
			// set the custom cursor based on the drawing tool.
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
			default:
				break;
			}  // End switch
		}
		// If cursor is over resize handle,
		else if (adjustedResizeHandleRect.PtInRect(point))
		{
			// set the default resize cursor.
			HCURSOR curResize;
			curResize = AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE);
			SetCursor(curResize);
		}
		else
		{
			// Set the default arrow cursor
			HCURSOR curArrow;
			curArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
			SetCursor(curArrow);
		}
	}

	return TRUE;
}

void CDrawView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// Releases the mouse capture from OnLButtonDown.
	ReleaseCapture();

	strokeInProgress = FALSE;
}

void CDrawView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// Called by the framework before OnDraw and OnPrint

	CScrollView::OnPrepareDC(pDC, pInfo);

	// PRINTING & PRINT PREVIEW
	if (pDC->IsPrinting())
	{
		return;
	}
	// NOT PRINTING
	else
	{
		CRect clientRect; GetClientRect(&clientRect);
		
		if (bitmapInitialized == FALSE)
		{
			return;
		}

		pDC->SetWindowOrg(0, 0);
		pDC->SetWindowExt(clientRect.Width(), clientRect.Height());

		pDC->SetViewportOrg(0, 0);
		pDC->SetViewportExt(clientRect.Width(), clientRect.Height());

		if (resizingMode == TRUE)
		{
			// If not drawing, remove the clipping region that is set to canvas only
			memDC.SelectClipRgn(NULL);
		}
		else if (drawingMode == TRUE)
		{
			// If drawing, set the clipping region to the canvas rectangle
			CRect clipRect(
				paddingHorizontal,                   // left
				paddingVertical,                     // top
				paddingHorizontal + canvasSize.cx,   // right
				paddingVertical   + canvasSize.cy);  // bottom

			CRgn clipRgn;
			clipRgn.CreateRectRgnIndirect(&clipRect);
			memDC.SelectClipRgn(&clipRgn);
		}
	}  // End if-else
}

void CDrawView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
{
	CClientDC dc(this);
	CRect clientRect; GetClientRect(&clientRect);

	int vScrollBarWidth = GetSystemMetrics(SM_CXVSCROLL);
	int hScrollBarHeight = GetSystemMetrics(SM_CYHSCROLL);

	if (lHint == 1)  // Resize handle was moved
	{
		// Get current bitmap and its size
		BITMAP drawingBitmapInfo;
		drawingBitmap.GetBitmap(&drawingBitmapInfo);
		LONG bmWidth = drawingBitmapInfo.bmWidth;
		LONG bmHeight = drawingBitmapInfo.bmHeight;

		// Calculate the size of the bitmap needed to hold the new canvas
		CSize neededBmSize{ trackRect.Width()  + paddingHorizontal * 2 + vScrollBarWidth,
						    trackRect.Height() + paddingVertical   * 2 + hScrollBarHeight };

		// Save the canvas portion of the current bitmap
		CBitmap curCanvasBm;
		VERIFY(curCanvasBm.CreateCompatibleBitmap(&dc, canvasSize.cx, canvasSize.cy));
		CDC tmpDC;
		tmpDC.CreateCompatibleDC(&dc);
		CBitmap* pOldCanvasBm = tmpDC.SelectObject(&curCanvasBm);
		tmpDC.BitBlt(0, 0, canvasSize.cx, canvasSize.cy,
			&memDC, canvasRect.left, canvasRect.top, SRCCOPY);

		CBitmap newBm;

		// If both width and height of the needed bitmap are larger than the current bitmap size,
		if (neededBmSize.cx > bmWidth && neededBmSize.cy > bmHeight)
		{
			// create a new bitmap that is both wider and taller.
			VERIFY(newBm.CreateCompatibleBitmap(&dc, neededBmSize.cx, neededBmSize.cy));
		}
		// If only the width of the needed bitmap is larger than the current bitmap width,
		else if (neededBmSize.cx > bmWidth)
		{
			// create a new to bitmap that is wider.
			VERIFY(newBm.CreateCompatibleBitmap(&dc, neededBmSize.cx, bmHeight));
		}
		// If only height of the needed bitmap is larger than the current bitmap height,
		else if (neededBmSize.cy > bmHeight)
		{
			// create a new bitmap that is taller.
			VERIFY(newBm.CreateCompatibleBitmap(&dc, bmWidth, neededBmSize.cy));
		}
		// If we don't need bigger bitmap, but the canvas size has changed,
		else if (trackRect.Width() != canvasSize.cx || trackRect.Height() != canvasSize.cy)
		{
			// create a new bitmap that is the same size as current one.
			VERIFY(newBm.CreateCompatibleBitmap(&dc, bmWidth, bmHeight));
		}

		// Select the new bitmap to the memory DC
		memDC.SelectObject(&newBm);

		// Update the canvas size based on the tracker rectangle
		canvasSize.cx = trackRect.Width();
		canvasSize.cy = trackRect.Height();

		// Fill the new bitmap with background and canvas
		UpdateClientArea();

		// Copy the previously saved canvas content to the new canvas.
		memDC.BitBlt(paddingHorizontal, paddingVertical, canvasSize.cx, canvasSize.cy,
			&tmpDC, 0, 0, SRCCOPY);
	}

	else  // lHint == 0 -> no resize, just redraw the view
	{
		UpdateClientArea();
	}

	// Call OnDraw to copy the memory DC to the screen DC
	Invalidate();
}

BOOL CDrawView::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}


////////////////////////////////////////////////////////////////////////
//                       CDrawView helper methods                     //
////////////////////////////////////////////////////////////////////////

void CDrawView::UpdateClientArea()
{
	////////////////////////////////////////////////////////////////////
	// CALCULATIG AN SETTING SIZES AND SCROLLING

	int vScrollBarWidth = GetSystemMetrics(SM_CXVSCROLL);
	int hScrollBarHeight = GetSystemMetrics(SM_CYHSCROLL);

	// Update the canvas rectangle position
	canvasRect.right  = paddingHorizontal + canvasSize.cx;
	canvasRect.bottom = paddingVertical   + canvasSize.cy;

	// Update the resize handle rectangle position
	int handleWidth = paddingHorizontal;
	int handleHeight = paddingVertical;
	
	resizeHandleRect.SetRect(
		canvasRect.right,                  // left
		canvasRect.bottom,                 // top
		canvasRect.right + handleWidth,    // right
		canvasRect.bottom + handleHeight); // bottom

	// Calculate the scrollable area accounting for padding and width of scroll bars
	CSize scrollSize;
	scrollSize.cx = canvasSize.cx + paddingHorizontal * 2 + vScrollBarWidth;
	scrollSize.cy = canvasSize.cy + paddingVertical   * 2 + hScrollBarHeight;
	SetScrollSizes(MM_TEXT, scrollSize);

	////////////////////////////////////////////////////////////////////
	// DRAWING CANVAS AND BACKGROUND

	CRect clientRect; GetClientRect(&clientRect);

	// Solve the problem of image not being drawn correctly while scrolling (artifacting).
	CRect clipBoxRect; memDC.GetClipBox(&clipBoxRect);
	memDC.FillSolidRect(clipBoxRect, RGB(220, 230, 240));  // Same color as area around canvas

	// Draw area around canvas
	GetClientRect(&clientRect);
	memDC.FillSolidRect(clientRect, RGB(220, 230, 240));

	// Draw canvas shadow
	CRect canvasShadowRect(
		paddingHorizontal * 2,
		paddingVertical   * 2,
		paddingHorizontal * 2 + canvasSize.cx,
		paddingVertical   * 2 + canvasSize.cy);
	memDC.FillSolidRect(canvasShadowRect, RGB(210, 220, 230));

	// Draw canvas
	canvasRect.left   = paddingHorizontal;
	canvasRect.top    = paddingVertical;
	canvasRect.right  = paddingHorizontal + canvasSize.cx;
	canvasRect.bottom = paddingVertical   + canvasSize.cy;
	memDC.FillSolidRect(canvasRect, RGB(255, 255, 255));

	// Draw canvas resize handle
	CPen handlePen(PS_SOLID, 1, RGB(64, 64, 64));
	CPen* pOldPen = memDC.SelectObject(&handlePen);

	memDC.Rectangle(
		resizeHandleRect.left,
		resizeHandleRect.top,
		resizeHandleRect.right,
		resizeHandleRect.bottom);

	// Cleanup
	memDC.SelectObject(pOldPen);
}