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
#include "MainFrm.h"

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
	ON_COMMAND(ID_BUTTON_PEN, &CDrawView::OnButtonPen)
	ON_COMMAND(ID_BUTTON_ERASER, &CDrawView::OnButtonEraser)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_PEN, &CDrawView::OnUpdateButtonPen)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ERASER, &CDrawView::OnUpdateButtonEraser)
	ON_COMMAND(ID_GALLERY_SIZE, &CDrawView::OnGallerySize)
	ON_COMMAND(ID_FORECOLOR, &CDrawView::OnForecolor)
	ON_COMMAND(ID_BACKCOLOR, &CDrawView::OnBackcolor)
END_MESSAGE_MAP()

// CDrawView construction/destruction

CDrawView::CDrawView() noexcept :
	drawingMode{ TRUE },
	resizingMode{ FALSE },
	canvasRect{ 0, 0, 0, 0 },
	resizeHandleRect{ 0, 0, 0, 0 },
	paddingHorizontal { 0 },
	paddingVertical { 0 },
	bitmapInitialized{ FALSE },
	trackRect{ 0, 0, 0, 0 },
	foreColor{ RGB(0, 0, 0) },  // black
	backColor{ RGB(255, 255, 255) },  // white
	sizePen{ 1 },
	sizeEraser{ 4 },
	sizeShape{ 1 },
	currentTool{ std::make_unique<PenTool>() }  // Default tool is Pen
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

	CDC& canvasDC = *pDoc->GetCanvasDC();
	const CSize& canvasSize = pDoc->GetCanvasSize();

	////////////////
	// NOT PRINTING
	if (!pDC->IsPrinting())
	{	
		CRect clientRect; GetClientRect(&clientRect);

		if (bkgDC.m_hDC != NULL)
		{
			// Get the current scroll position
			CPoint scrollPos = GetScrollPosition();
			// Offset the client rectangle by the scroll position
			clientRect.OffsetRect(scrollPos);

			// Copy the canvas DC to the background DC
			VERIFY(bkgDC.BitBlt(paddingHorizontal, paddingVertical, canvasSize.cx, canvasSize.cy,
				&canvasDC, 0, 0, SRCCOPY));
			
			// Copy the background DC to the screen DC
			VERIFY(pDC->BitBlt(0, 0, clientRect.Width(), clientRect.Height(),
				&bkgDC, scrollPos.x, scrollPos.y, SRCCOPY));
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

		// Copy the canvas DC to the printing DC
		pDC->SetStretchBltMode(STRETCH_DELETESCANS);  // FIX for the scaling artifacts caused by StretchDIBits
		VERIFY(pDC->StretchBlt(0, 0, viewportWidth, viewportHeight,
			&canvasDC, 0, 0, canvasSize.cx, canvasSize.cy, SRCCOPY));
	}
}

void CDrawView::OnInitialUpdate()
{
	CDrawDoc* pDoc = GetDocument();
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	if (!pDoc->GetNewImageLoaded())
	{
		//*********************************************************************************************
	    // Reset colors to default values

		CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
		pMainFrame->m_wndRibbonBar.GetElementsByID(ID_FORECOLOR, arr);
		CMFCRibbonColorButton* pForeColorButton = DYNAMIC_DOWNCAST(CMFCRibbonColorButton, arr[0]);
		pForeColorButton->SetColor(RGB(0, 0, 0));

		pMainFrame->m_wndRibbonBar.GetElementsByID(ID_BACKCOLOR, arr);
		CMFCRibbonColorButton* pBackColorButton = DYNAMIC_DOWNCAST(CMFCRibbonColorButton, arr[0]);
		pBackColorButton->SetColor(RGB(255, 255, 255));

		foreColor = RGB(0, 0, 0);
		backColor = RGB(255, 255, 255);

		switch (currentTool->GetToolType())
		{
		case pen:
			currentTool->SetColor(foreColor);
			break;
		case eraser:
			currentTool->SetColor(backColor);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		//*********************************************************************************************
	}

	//*************************************************************************************************
	// Initialize background DC and bitmap
	CClientDC dc(this);
	if (!bkgBitmap.m_hObject)
	{
		VERIFY(bkgBitmap.CreateCompatibleBitmap(&dc, GetDeviceCaps(dc, HORZRES), GetDeviceCaps(dc, VERTRES)));
		bkgDC.CreateCompatibleDC(&dc);
		bkgDC.SelectObject(&bkgBitmap);
		bitmapInitialized = TRUE;
	}
	//*************************************************************************************************
	
	// ************************************************************************************************
	// Initialize the canvas DC and bitmap
	CDC& canvasDC = *pDoc->GetCanvasDC();
	CBitmap& canvasBitmap = pDoc->GetCanvasBitmap();

	const CSize& canvasSize = pDoc->GetCanvasSize();
	if (!canvasBitmap.m_hObject && !canvasDC.m_hDC)
	{
		VERIFY(canvasBitmap.CreateCompatibleBitmap(&dc, canvasSize.cx, canvasSize.cy));
		canvasDC.CreateCompatibleDC(&dc);
		canvasDC.SelectObject(&canvasBitmap);
		canvasDC.FillSolidRect(0, 0, canvasSize.cx, canvasSize.cy, RGB(255, 255, 255)); // White
	}
	//*************************************************************************************************

	// Calculate the size of canvas padding based on the device's pixel density
	int xLogPixelsPerInch = GetDeviceCaps(dc, LOGPIXELSX);
	int yLogPixelsPerInch = GetDeviceCaps(dc, LOGPIXELSY);
	paddingHorizontal = xLogPixelsPerInch / 16;
	paddingVertical = yLogPixelsPerInch / 16;

	// Set the canvas rectangle coordinates, accounting for padding
	canvasRect.SetRect(
		paddingHorizontal,                   // left
		paddingVertical,                     // top
		paddingHorizontal + canvasSize.cx,   // right
		paddingVertical   + canvasSize.cy);  // bottom
	
	trackRect = canvasRect;

	// Set the scrollable area, accounting for padding and width of scroll bars
	int vScrollBarWidth = GetSystemMetrics(SM_CXVSCROLL);
	int hScrollBarHeight = GetSystemMetrics(SM_CYHSCROLL);
	CSize scrollSize;
	scrollSize.cx = canvasSize.cx + paddingHorizontal * 2 + vScrollBarWidth;
	scrollSize.cy = canvasSize.cy + paddingVertical * 2 + hScrollBarHeight;
	SetScrollSizes(MM_TEXT, scrollSize);

	CScrollView::OnInitialUpdate();  // Calls OnUpdate

	// Adjusts the layout of all items in the ribbon bar and parent window and redraws the whole
	// window. This is needed to instantly update the ribbon on File>New to reflect changes in
	// color buttons. Must be here at the bottom, otherwise - error.
	pMainFrame->m_wndRibbonBar.ForceRecalcLayout();
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
	
	// Offset the reize handle rectagle by the scroll position
	CRect adjustedResizeHandleRect = resizeHandleRect;
	adjustedResizeHandleRect.OffsetRect(-scrollPos.x, -scrollPos.y);

	CDrawDoc* pDoc = GetDocument();
	
	// Clicked inside the resize handle
	if (adjustedResizeHandleRect.PtInRect(point))
	{
		resizingMode = TRUE;
		drawingMode = FALSE;
		Invalidate();  // Prepare DC for resizing.

		CRectTracker rectTracker;

		// Define startPoint for the rubber band tracker and adjust it by the scroll position
		CPoint startPoint(canvasRect.left, canvasRect.top);
		startPoint.Offset(-scrollPos.x, -scrollPos.y);

		if (rectTracker.TrackRubberBand(this, startPoint, FALSE)) // The mouse has moved and the rectangle is not empty
		{
			// Get the tracker rectangle in client coordinates
			trackRect = rectTracker.m_rect;

			pDoc->SetModifiedFlag(TRUE);

			// Update the scroll sizes and redraw the view
			pDoc->UpdateAllViews(NULL, 1, NULL);
		}

		resizingMode = FALSE;
		drawingMode = TRUE;
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

		// Get the DC on which to draw
		CDC& canvasDC = *pDoc->GetCanvasDC();

		// Adjust point to canvas top-left (0, 0) coordinates
		point.Offset(-paddingHorizontal, -paddingVertical);

		// Let the tool handle the mouse event
		currentTool->OnLButtonDown(&canvasDC, point, pDoc);

		Invalidate();
	}  // End if-else
}

void CDrawView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!drawingMode)  // If drawing is not in progress, do nothing.
		return;

	if (GetKeyState(VK_LBUTTON) & 0x8000)
		// If the high order bit of return value is set,
		//  the left mouse button is down.
	{
		CDrawDoc* pDoc = GetDocument();
		// Get the DC on which to draw
		CDC& canvasDC = *pDoc->GetCanvasDC();

		CPoint scrollPos = GetScrollPosition();
		point.Offset(scrollPos.x, scrollPos.y);  // Offset the point by the scroll position

		// Adjust point to canvas top-left (0, 0) coordinates
		point.Offset(-paddingHorizontal, -paddingVertical);

		// Let the tool handle the mouse event
		currentTool->OnMouseMove(&canvasDC, point, pDoc);

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
			switch (currentTool->GetToolType())
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

	currentTool->OnLButtonUp();
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
			bkgDC.SelectClipRgn(NULL);
		}
		else if (drawingMode == TRUE)
		{
			const CSize& canvasSize = GetDocument()->GetCanvasSize();

			// If drawing, set the clipping region to the canvas rectangle
			CRect clipRect(
				paddingHorizontal,                   // left
				paddingVertical,                     // top
				paddingHorizontal + canvasSize.cx,   // right
				paddingVertical   + canvasSize.cy);  // bottom

			CRgn clipRgn;
			clipRgn.CreateRectRgnIndirect(&clipRect);
			bkgDC.SelectClipRgn(&clipRgn);
		}
	}  // End if-else
}

void CDrawView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
{
	CClientDC dc(this);
	CDrawDoc* pDoc = GetDocument();

	const CSize& canvasSize = pDoc->GetCanvasSize();

	CDC& canvasDC = *pDoc->GetCanvasDC();
	CBitmap& canvasBitmap = pDoc->GetCanvasBitmap();

	int vScrollBarWidth = GetSystemMetrics(SM_CXVSCROLL);
	int hScrollBarHeight = GetSystemMetrics(SM_CYHSCROLL);

	if (lHint == 1)  // Resize handle was moved
	{
		// Calculate the size of the background bitmap needed to hold the new canvas
		CSize neededBmSize{ trackRect.Width()  + paddingHorizontal * 2 + vScrollBarWidth,
						    trackRect.Height() + paddingVertical   * 2 + hScrollBarHeight };

		// First, create a temporary bitmap to hold the old canvas content
		CBitmap tmpCanvasBm;
		VERIFY(tmpCanvasBm.CreateCompatibleBitmap(&dc, canvasSize.cx, canvasSize.cy));
		CDC tmpCanvasDC;
		VERIFY(tmpCanvasDC.CreateCompatibleDC(&dc));
		tmpCanvasDC.SelectObject(&tmpCanvasBm);

		// Copy the current canvas bitmap to the temporary bitmap
		VERIFY(tmpCanvasDC.BitBlt(0, 0, canvasSize.cx, canvasSize.cy,
			&canvasDC, 0, 0, SRCCOPY));

		// Get the current background bitmap information
		BITMAP bkgBmInfo;
		CBitmap* curBkgBm = bkgDC.GetCurrentBitmap();
		curBkgBm->GetBitmap(&bkgBmInfo);
		LONG bkgBmWidth = bkgBmInfo.bmWidth;
		LONG bkgBmHeight = bkgBmInfo.bmHeight;

		CBitmap newBkgBm;

		// If both width and height of the needed bitmap are larger than the current bitmap size,
		if (neededBmSize.cx > bkgBmWidth && neededBmSize.cy > bkgBmHeight)
		{
			// create a new bitmap that is both wider and taller.
			VERIFY(newBkgBm.CreateCompatibleBitmap(&dc, neededBmSize.cx, neededBmSize.cy));
		}
		// If only the width of the needed bitmap is larger than the current bitmap width,
		else if (neededBmSize.cx > bkgBmWidth)
		{
			// create a new to bitmap that is wider.
			VERIFY(newBkgBm.CreateCompatibleBitmap(&dc, neededBmSize.cx, bkgBmHeight));
		}
		// If only height of the needed bitmap is larger than the current bitmap height,
		else if (neededBmSize.cy > bkgBmHeight)
		{
			// create a new bitmap that is taller.
			VERIFY(newBkgBm.CreateCompatibleBitmap(&dc, bkgBmWidth, neededBmSize.cy));
		}
		// If we don't need bigger bitmap, but the canvas size has changed,
		else if (trackRect.Width() != canvasSize.cx || trackRect.Height() != canvasSize.cy)
		{
			// create a new bitmap that is the same size as current one.
			VERIFY(newBkgBm.CreateCompatibleBitmap(&dc, bkgBmWidth, bkgBmHeight));
		}

		// Update the canvas size info based on the tracker rectangle
		pDoc->SetCanvasSize(CSize(
			trackRect.Width(), trackRect.Height()));

		bkgDC.DeleteDC();
		bkgBitmap.DeleteObject();

		bkgDC.CreateCompatibleDC(&dc);
		bkgBitmap.Attach(newBkgBm.Detach());  // Transfer ownership to member variable

		bkgDC.SelectObject(&bkgBitmap);
		UpdateClientArea();  // Draw background elements (shadow, handle, etc.)

		canvasDC.DeleteDC();
		canvasBitmap.DeleteObject();

		canvasDC.CreateCompatibleDC(&dc);

		// Create a new blank canvas bitmap with the new calculated size
        CBitmap newCanvasBitmap;
        VERIFY(newCanvasBitmap.CreateCompatibleBitmap(&dc, canvasSize.cx, canvasSize.cy));

		// Select the new canvas bitmap to the canvas DC and paint it
		canvasDC.SelectObject(&newCanvasBitmap);
		canvasDC.FillSolidRect(0, 0, canvasSize.cx, canvasSize.cy, RGB(255, 255, 255)); // White

		// Copy the current canvas bitmap to the new canvas bitmap
		VERIFY(canvasDC.BitBlt(0, 0, canvasSize.cx, canvasSize.cy,
			&tmpCanvasDC, 0, 0, SRCCOPY));

		canvasBitmap.Attach(newCanvasBitmap.Detach());  // Transfer ownership to member variable
		canvasDC.SelectObject(&canvasBitmap);  // Select the new bitmap into the DC

		tmpCanvasDC.DeleteDC();
		tmpCanvasBm.DeleteObject();
	}

 	else if (pDoc->GetNewImageLoaded() == TRUE)  // New image was loaded
	{
		pDoc->SetNewImageLoaded(FALSE);  // Reset the flag

		const CImage& canvasImage = pDoc->GetCanvasImage();
		if (canvasImage)
		{
			// If the canvas image is loaded, copy it to the canvas DC
			canvasImage.BitBlt(canvasDC, 0, 0, canvasSize.cx, canvasSize.cy, 0, 0, SRCCOPY);
		}

		// Calculate the size of the background bitmap needed to hold the new canvas
		CSize neededBmSize{ canvasSize.cx + paddingHorizontal * 2 + vScrollBarWidth,
							canvasSize.cy + paddingVertical * 2 + hScrollBarHeight };

		// Get the current background bitmap information
		BITMAP bkgBmInfo;
		CBitmap* curBkgBm = bkgDC.GetCurrentBitmap();
		curBkgBm->GetBitmap(&bkgBmInfo);
		LONG bkgBmWidth = bkgBmInfo.bmWidth;
		LONG bkgBmHeight = bkgBmInfo.bmHeight;

		CBitmap newBkgBm;

		// If both width and height of the needed bitmap are larger than the current bitmap size,
		if (neededBmSize.cx > bkgBmWidth && neededBmSize.cy > bkgBmHeight)
		{
			// create a new bitmap that is both wider and taller.
			VERIFY(newBkgBm.CreateCompatibleBitmap(&dc, neededBmSize.cx, neededBmSize.cy));
		}
		// If only the width of the needed bitmap is larger than the current bitmap width,
		else if (neededBmSize.cx > bkgBmWidth)
		{
			// create a new to bitmap that is wider.
			VERIFY(newBkgBm.CreateCompatibleBitmap(&dc, neededBmSize.cx, bkgBmHeight));
		}
		// If only height of the needed bitmap is larger than the current bitmap height,
		else if (neededBmSize.cy > bkgBmHeight)
		{
			// create a new bitmap that is taller.
			VERIFY(newBkgBm.CreateCompatibleBitmap(&dc, bkgBmWidth, neededBmSize.cy));
		}
		// If we don't need bigger bitmap
		else
		{
			// create a new background bitmap that is the same size as current one.
			VERIFY(newBkgBm.CreateCompatibleBitmap(&dc, bkgBmWidth, bkgBmHeight));
		}

		bkgDC.DeleteDC();
		bkgBitmap.DeleteObject();

		bkgDC.CreateCompatibleDC(&dc);
		bkgBitmap.Attach(newBkgBm.Detach());  // Transfer ownership to member variable

		bkgDC.SelectObject(&bkgBitmap);
		UpdateClientArea();  // Draw background elements (shadow, handle, etc.)
	}

	else  // lHint == 0 -> just redraw the view
	{
		UpdateClientArea();

		bkgDC.BitBlt(paddingHorizontal, paddingVertical, canvasSize.cx, canvasSize.cy,
			&canvasDC, 0, 0, SRCCOPY);
	}

	// Call OnDraw to copy the memory DC to the screen DC
	Invalidate();
}

void CDrawView::OnButtonPen()
{
	currentTool = std::make_unique<PenTool>(sizePen, foreColor);

	//	Get the size selection gallery
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_GALLERY_SIZE, arr);
	CMFCRibbonGallery* pGallery = DYNAMIC_DOWNCAST(CMFCRibbonGallery, arr[0]);

	//	Set icons for pen tool sizes
	pGallery->SetPalette(IDB_SIZES1234, 104);
}

void CDrawView::OnButtonEraser()
{
	currentTool = std::make_unique<EraserTool>(sizeEraser, backColor);

	//	Get the size selection gallery
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_GALLERY_SIZE, arr);
	CMFCRibbonGallery* pGallery = DYNAMIC_DOWNCAST(CMFCRibbonGallery, arr[0]);

	//	Set icons for eraser tool sizes
	pGallery->SetPalette(IDB_SIZES46810, 104);
}

void CDrawView::OnUpdateButtonPen(CCmdUI* pCmdUI)
{
	if (currentTool->GetToolType() == pen)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

void CDrawView::OnUpdateButtonEraser(CCmdUI* pCmdUI)
{
	if (currentTool->GetToolType() == eraser)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

void CDrawView::OnGallerySize()
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_GALLERY_SIZE, arr);
	CMFCRibbonGallery* pGallery = DYNAMIC_DOWNCAST(CMFCRibbonGallery, arr[0]);

	int index = pGallery->GetSelectedItem();

	const std::vector<int> penSizes = { 1, 2, 3, 4 };
	const std::vector<int> eraserSizes = { 4, 6, 8, 10 };

	ASSERT(index >= 0 && index < penSizes.size() && index < eraserSizes.size());

	sizePen = penSizes[index];
	sizeEraser = eraserSizes[index];

	switch (currentTool->GetToolType())
	{
	case pen:
		currentTool->SetSize(sizePen);
		break;
	case eraser:
		currentTool->SetSize(sizeEraser);
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}

void CDrawView::OnForecolor()
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_FORECOLOR, arr);
	CMFCRibbonColorButton* pColorButton = DYNAMIC_DOWNCAST(CMFCRibbonColorButton, arr[0]);

	COLORREF selectedColor = pColorButton->GetColor();

	if (selectedColor == CLR_INVALID)  // CLR_INVALID == 0xFFFFFFFF == 4294967295
	{
		foreColor = pColorButton->GetAutomaticColor();
	}
	else
	{
		foreColor = selectedColor;
	}

	if (currentTool->GetToolType() == pen)
	{
		currentTool->SetColor(foreColor);
	}
}

void CDrawView::OnBackcolor()
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_BACKCOLOR, arr);
	CMFCRibbonColorButton* pColorButton = DYNAMIC_DOWNCAST(CMFCRibbonColorButton, arr[0]);

	COLORREF selectedColor = pColorButton->GetColor();

	if (selectedColor == CLR_INVALID)  // CLR_INVALID == 0xFFFFFFFF == 4294967295
	{
		backColor = pColorButton->GetAutomaticColor();
	}
	else
	{
		backColor = selectedColor;
	}

	if (currentTool->GetToolType() == eraser)
	{
		currentTool->SetColor(backColor);
	}
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
	// CALCULATIG AND SETTING SIZES AND SCROLLING

	int vScrollBarWidth = GetSystemMetrics(SM_CXVSCROLL);
	int hScrollBarHeight = GetSystemMetrics(SM_CYHSCROLL);

	const CSize& canvasSize = GetDocument()->GetCanvasSize();

	// Update the canvas rectangle position
	canvasRect.SetRect(
		paddingHorizontal,                   // left
		paddingVertical,                     // top
		paddingHorizontal + canvasSize.cx,   // right
		paddingVertical + canvasSize.cy);    // bottom

	// Update the resize handle rectangle position
	int handleWidth = paddingHorizontal;
	int handleHeight = paddingVertical;

	resizeHandleRect.SetRect(
		canvasRect.right,                   // left
		canvasRect.bottom,                  // top
		canvasRect.right  + handleWidth,    // right
		canvasRect.bottom + handleHeight);  // bottom

	// Calculate the scrollable area accounting for padding and width of scroll bars
	CSize scrollSize;
	scrollSize.cx = canvasSize.cx + paddingHorizontal * 2 + vScrollBarWidth;
	scrollSize.cy = canvasSize.cy + paddingVertical   * 2 + hScrollBarHeight;
	SetScrollSizes(MM_TEXT, scrollSize);

	////////////////////////////////////////////////////////////////////
	// DRAWING BACKGROUND

	CRect clientRect; GetClientRect(&clientRect);

	// Solve the problem of image not being drawn correctly while scrolling (artifacting).
	CRect clipBoxRect; bkgDC.GetClipBox(&clipBoxRect);
	bkgDC.FillSolidRect(clipBoxRect, RGB(220, 230, 240));  // Same color as area around canvas

	// Draw area around canvas
	GetClientRect(&clientRect);
	bkgDC.FillSolidRect(clientRect, RGB(220, 230, 240));

	// Draw canvas shadow
	CRect canvasShadowRect(
		paddingHorizontal * 2,
		paddingVertical   * 2,
		paddingHorizontal * 2 + canvasSize.cx,
		paddingVertical   * 2 + canvasSize.cy);
	bkgDC.FillSolidRect(canvasShadowRect, RGB(210, 220, 230));

	// Draw canvas resize handle
	CPen handlePen(PS_SOLID, 1, RGB(64, 64, 64));
	CPen* pOldPen = bkgDC.SelectObject(&handlePen);

	bkgDC.Rectangle(
		resizeHandleRect.left,
		resizeHandleRect.top,
		resizeHandleRect.right,
		resizeHandleRect.bottom);

	// Cleanup
	bkgDC.SelectObject(pOldPen);
}
