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
#include <cmath>
#include "MainFrm.h"
#include "ResizeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <stdexcept>


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
	ON_COMMAND(ID_BUTTON_BRUSH, &CDrawView::OnButtonBrush)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_BRUSH, &CDrawView::OnUpdateButtonBrush)
	ON_COMMAND(ID_BUTTON_FILL, &CDrawView::OnButtonFill)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_FILL, &CDrawView::OnUpdateButtonFill)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_BUTTON_COLOR_PICKER, &CDrawView::OnButtonColorPicker)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_COLOR_PICKER, &CDrawView::OnUpdateButtonColorPicker)
	ON_COMMAND(ID_BUTTON_RESIZE, &CDrawView::OnButtonResize)
	ON_COMMAND(ID_GALLERY_SHAPES, &CDrawView::OnGalleryShapes)
	ON_COMMAND(ID_BUTTON_SHAPE_OUTLINE, &CDrawView::OnButtonShapeOutline)
	ON_COMMAND(ID_BUTTON_SHAPE_FILL, &CDrawView::OnButtonShapeFill)
	ON_COMMAND(ID_BUTTON_OUTLINE_NO_OUTLINE, &CDrawView::OnButtonShapeOutlineNoOutline)
	ON_COMMAND(ID_BUTTON_OUTLINE_SOLID_COLOR, &CDrawView::OnButtonShapeOutlineSolidColor)
	ON_COMMAND(ID_BUTTON_FILL_NO_FILL, &CDrawView::OnButtonShapeFillNoFill)
	ON_COMMAND(ID_BUTTON_FILL_SOLID_COLOR, &CDrawView::OnButtonShapeFillSolidColor)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SHAPE_OUTLINE, &CDrawView::OnUpdateButtonShapeOutline)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SHAPE_FILL, &CDrawView::OnUpdateButtonShapeFill)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_OUTLINE_NO_OUTLINE, &CDrawView::OnUpdateButtonOutlineNoOutline)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_OUTLINE_SOLID_COLOR, &CDrawView::OnUpdateButtonOutlineSolidColor)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_FILL_NO_FILL, &CDrawView::OnUpdateButtonFillNoFill)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_FILL_SOLID_COLOR, &CDrawView::OnUpdateButtonFillSolidColor)
END_MESSAGE_MAP()

// CDrawView construction/destruction

CDrawView::CDrawView() noexcept :
	canvasRect{ 0, 0, 0, 0 },
	resizeHandleRect{ 0, 0, 0, 0 },
	paddingHorizontal { 0 },
	paddingVertical { 0 },
	bitmapInitialized{ FALSE },
	trackRect{ 0, 0, 0, 0 },
	foreColor{ RGB(0, 0, 0) },  // black
	backColor{ RGB(255, 255, 255) },  // white
	sizeIndex{ 0 },
	currentTool{ CreatePenTool(sizeIndex, foreColor, backColor)},  // Default tool is Pen
	activeControlCommandID{ ID_BUTTON_PEN },
	curCoordinates{ -1, -1 },
	selectedShape{ 0 },
	shapeOutline{ SOLID_COLOR_OUTLINE },
	shapeFill{ NO_FILL }
{
}

CDrawView::~CDrawView()
{
}

BOOL CDrawView::PreCreateWindow(CREATESTRUCT& cs)
{
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

		if (bkgDC.m_hDC != NULL && canvasDC.m_hDC != NULL)
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
		CMFCRibbonColorButton* pForeColorButton = (CMFCRibbonColorButton*)arr[0];
		pForeColorButton->SetColor(RGB(0, 0, 0));

		pMainFrame->m_wndRibbonBar.GetElementsByID(ID_BACKCOLOR, arr);
		CMFCRibbonColorButton* pBackColorButton = (CMFCRibbonColorButton*)arr[0];
		pBackColorButton->SetColor(RGB(255, 255, 255));

		foreColor = RGB(0, 0, 0);
		backColor = RGB(255, 255, 255);

		currentTool->SetPrimaryColor(foreColor);
		currentTool->SetSecondaryColor(backColor);
		//*********************************************************************************************
	}

	CClientDC dc(this);

	//*************************************************************************************************
	// Initialize the background DC and bitmap
	if (!bkgBitmap.m_hObject)
	{
		VERIFY(bkgBitmap.CreateCompatibleBitmap(&dc, GetDeviceCaps(dc, HORZRES), GetDeviceCaps(dc, VERTRES)));
		bkgDC.CreateCompatibleDC(&dc);
		bkgDC.SelectObject(&bkgBitmap);
		bitmapInitialized = TRUE;
	}
	//*************************************************************************************************
	
	CDC& canvasDC = *pDoc->GetCanvasDC();
	CBitmap& canvasBitmap = pDoc->GetCanvasBitmap();
	const CSize& canvasSize = pDoc->GetCanvasSize();

	// ************************************************************************************************
    // Initialize the canvas DC and bitmap
	if (!canvasDC.m_hDC)
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

	pMainFrame->UpdateStatusCanvasSize(canvasSize);
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
	// Extra initialization before printing
}

void CDrawView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// Cleanup after printing
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
	CPoint scrollPos = GetScrollPosition();
	point.Offset(scrollPos.x, scrollPos.y); // Offset the point by the scroll position

	CDrawDoc* pDoc = GetDocument();
	
	// Clicked inside the resize handle
	if (resizeHandleRect.PtInRect(point))
	{
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

			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			CSize canvasSize = pDoc->GetCanvasSize();
			pMainFrame->UpdateStatusCanvasSize(canvasSize);
		}
	}
	// Clicked outside resize handle
	else
	{
		if (canvasRect.PtInRect(point))  // Clicked inside the canvas
		{
			// Get the DC on which to draw
			CDC& canvasDC = *pDoc->GetCanvasDC();

			if (activeControlCommandID != ID_GALLERY_SHAPES)  // CRectTracker::TrackRubberBand used in shapes
			{                                                 // does not work if SetCapture is used.
				SetCapture();  // For receiving mouse messages outside the window

				//Convert from client to canvas coordinates
				point.Offset(-paddingHorizontal, -paddingVertical);

				//Let the tool handle the mouse event
				currentTool->OnLButtonDown(&canvasDC, point);
			}
			else  // Drawing shape
			{
				CRectTracker rectTracker;
				rectTracker.m_sizeMin = CSize(1, 1);

				// Undo the point offset by the scroll position because TrackRubberBand expects
				// client coordinates.
				CPoint scrollPos = GetScrollPosition();
				point.Offset(-scrollPos.x, -scrollPos.y);

				if (rectTracker.TrackRubberBand(this, point, TRUE)) // The mouse has moved and
				{                                                   // the rectangle is not empty.
					// Get the tracker rectangle in client coordinates
					CRect rc = rectTracker.m_rect;

					rc.OffsetRect(scrollPos.x, scrollPos.y); // Offset the rect by the scroll position
					rc.OffsetRect(-paddingHorizontal, -paddingVertical); // Convert from client to canvas coordinates

					currentTool->DrawShapeOnLButtonDown(&canvasDC, rc);
				}
			}

			if (activeControlCommandID != ID_BUTTON_COLOR_PICKER)  // Color picker doesn't change
			{                                                      // the document.
				pDoc->SetModifiedFlag(TRUE);
			}
		}
		Invalidate();
	}  // End if-else
}

void CDrawView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CPoint scrollPos = GetScrollPosition();
	point.Offset(scrollPos.x, scrollPos.y); // Offset the point by the scroll position

	if (canvasRect.PtInRect(point))  // Clicked inside the canvas
	{
		CDrawDoc* pDoc = GetDocument();

		// Get the DC on which to draw
		CDC& canvasDC = *pDoc->GetCanvasDC();

		if (activeControlCommandID != ID_GALLERY_SHAPES)  // CRectTracker::TrackRubberBand used in shapes
		{                                                 // does not work if SetCapture is used.
			SetCapture();  // For receiving mouse messages outside the window

			//Convert from client to canvas coordinates
			point.Offset(-paddingHorizontal, -paddingVertical);

			//Let the tool handle the mouse event
			currentTool->OnRButtonDown(&canvasDC, point);
		}
		else  // Drawing shape
		{
			CRectTracker rectTracker;
			rectTracker.m_sizeMin = CSize(1, 1);

			// Undo the point offset by the scroll position because TrackRubberBand expects
			// client coordinates.
			CPoint scrollPos = GetScrollPosition();
			point.Offset(-scrollPos.x, -scrollPos.y);

			if (rectTracker.TrackRubberBand(this, point, TRUE)) // The mouse has moved and
			{                                                   // the rectangle is not empty.
				// Get the tracker rectangle in client coordinates
				CRect rc = rectTracker.m_rect;

				rc.OffsetRect(scrollPos.x, scrollPos.y); // Offset the rect by the scroll position
				rc.OffsetRect(-paddingHorizontal, -paddingVertical); // Convert from client to canvas coordinates

				currentTool->DrawShapeOnRButtonDown(&canvasDC, rc);
			}
		}

		if (activeControlCommandID != ID_BUTTON_COLOR_PICKER)  // Color picker doesn't change doc
		{
			pDoc->SetModifiedFlag(TRUE);
		}
	}
		Invalidate();
}


void CDrawView::OnMouseMove(UINT nFlags, CPoint point)
{
	CDrawDoc* pDoc = GetDocument();

	// Get the DC on which to draw
	CDC& canvasDC = *pDoc->GetCanvasDC();

	CPoint scrollPos = GetScrollPosition();
	point.Offset(scrollPos.x, scrollPos.y);  // Offset the point by the scroll position

	// Convert from client to canvas coordinates
	point.Offset(-paddingHorizontal, -paddingVertical);

	// Let the tool handle the mouse event
	currentTool->OnMouseMove(&canvasDC, point);

	// Notify main frame
	CRect adjustedCanvasRect = canvasRect;
	adjustedCanvasRect.OffsetRect(-paddingHorizontal, -paddingVertical);

	if (adjustedCanvasRect.PtInRect(point))
	{
		curCoordinates = point;
	}
	else
	{
		curCoordinates = CPoint(-1, -1);  // Ignore coordinates if outside canvas
	}
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->UpdateStatusCurCoordinates(curCoordinates);

	Invalidate();
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
			switch (activeControlCommandID)
			{
			case ID_BUTTON_PEN:
			{
				HCURSOR curPen;
				curPen = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_PEN_CURSOR));
				SetCursor(curPen);
				break;
			}
			case ID_BUTTON_ERASER:
			{
				HCURSOR curEraser;
				curEraser = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_ERASER_CURSOR));
				SetCursor(curEraser);
				break;
			}
			case ID_BUTTON_BRUSH:
			{
				HCURSOR curBrush;
				curBrush = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_BRUSH_CURSOR));
				SetCursor(curBrush);
				break;
			}
			case ID_BUTTON_FILL:
			{
				HCURSOR curFill;
				curFill = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_FILL_CURSOR));
				SetCursor(curFill);
				break;
			}
			case ID_BUTTON_COLOR_PICKER:
			{
				HCURSOR curColorPicker;
				curColorPicker = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_COLOR_PICKER_CURSOR));
				SetCursor(curColorPicker);
				break;
			}
			case ID_GALLERY_SHAPES:
			{
				HCURSOR curShapes;
				curShapes = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_SHAPES_CURSOR));
				SetCursor(curShapes);
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

void CDrawView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	// Releases the mouse capture from OnRButtonDown.
	ReleaseCapture();

	currentTool->OnRButtonUp();
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
		if (bitmapInitialized == FALSE)
		{
			return;
		}

		CRect clientRect; GetClientRect(&clientRect);

		pDC->SetWindowOrg(0, 0);
		pDC->SetWindowExt(clientRect.Width(), clientRect.Height());

		pDC->SetViewportOrg(0, 0);
		pDC->SetViewportExt(clientRect.Width(), clientRect.Height());
	}  // End if-else
}

void CDrawView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
{
	CClientDC dc(this);
	CDrawDoc* pDoc = GetDocument();
	CDC& canvasDC = *pDoc->GetCanvasDC();
	CBitmap& canvasBitmap = pDoc->GetCanvasBitmap();
	const CSize& canvasSize = pDoc->GetCanvasSize();

	// Initialize the canvas DC and bitmap
	if (!canvasDC.m_hDC)
	{
		VERIFY(canvasBitmap.CreateCompatibleBitmap(&dc, canvasSize.cx, canvasSize.cy));
		canvasDC.CreateCompatibleDC(&dc);
		canvasDC.SelectObject(&canvasBitmap);
		canvasDC.FillSolidRect(0, 0, canvasSize.cx, canvasSize.cy, RGB(255, 255, 255)); // White
	}

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

	else  // Just redraw the view
	{
		UpdateClientArea();

		bkgDC.BitBlt(paddingHorizontal, paddingVertical, canvasSize.cx, canvasSize.cy,
			&canvasDC, 0, 0, SRCCOPY);
	}

	// Call OnDraw to copy the memory DC to the screen DC
	Invalidate();
}

void CDrawView::OnButtonResize()
{
	ResizeDialog dlg;
	CDrawDoc* pDoc = GetDocument();
	dlg.newWidth = pDoc->GetCanvasSize().cx;
	dlg.newHeight = pDoc->GetCanvasSize().cy;

	if (dlg.DoModal() == IDOK)
	{
		CRect newCanvasRectSize;
		newCanvasRectSize.SetRect(
			0,
			0,
			dlg.newWidth,
			dlg.newHeight
		);

		trackRect = newCanvasRectSize;

		pDoc->SetModifiedFlag(TRUE);
		pDoc->UpdateAllViews(NULL, 1, NULL);
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		CSize canvasSize = pDoc->GetCanvasSize();
		pMainFrame->UpdateStatusCanvasSize(canvasSize);
	}
}

void CDrawView::OnButtonPen()
{
	currentTool = CreatePenTool(sizeIndex, foreColor, backColor);
	activeControlCommandID = ID_BUTTON_PEN;

	//	Get the size selection gallery
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_GALLERY_SIZE, arr);
	CMFCRibbonGallery* pGallery = (CMFCRibbonGallery*)arr[0];

	//	Set icons for pen tool sizes
	pGallery->SetPalette(IDB_SIZES1234, 104);
}

void CDrawView::OnButtonEraser()
{
	currentTool = CreateEraserTool(sizeIndex, backColor);
	activeControlCommandID = ID_BUTTON_ERASER;

	//	Get the size selection gallery
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_GALLERY_SIZE, arr);
	CMFCRibbonGallery* pGallery = (CMFCRibbonGallery*)arr[0];

	//	Set icons for eraser tool sizes
	pGallery->SetPalette(IDB_SIZES46810, 104);
}

void CDrawView::OnButtonBrush()
{
	currentTool = CreateBrushTool(sizeIndex, foreColor, backColor);
	activeControlCommandID = ID_BUTTON_BRUSH;

	//	Get the size selection gallery
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_GALLERY_SIZE, arr);
	CMFCRibbonGallery* pGallery = (CMFCRibbonGallery*)arr[0];

	//	Set icons for brush tool sizes
	pGallery->SetPalette(IDB_SIZES1358, 104);
}

void CDrawView::OnButtonFill()
{
	currentTool = CreateFillTool(foreColor, backColor);
	activeControlCommandID = ID_BUTTON_FILL;
}

void CDrawView::OnButtonColorPicker()
{
	currentTool = CreateColorPickerTool();
	currentTool->SetObserver(this);  // Set the observer to receive color updates
	activeControlCommandID = ID_BUTTON_COLOR_PICKER;
}

void CDrawView::OnGalleryShapes()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	
	// Get the shape selection gallery
	pMainFrame->m_wndRibbonBar.GetElementsByID(ID_GALLERY_SHAPES, arr);
	CMFCRibbonGallery* pGallery = (CMFCRibbonGallery*)arr[0];
	
	// Get the selected shape
	selectedShape = pGallery->GetSelectedItem();
	
	currentTool = CreateRectTool(selectedShape, shapeOutline, shapeFill, sizeIndex, foreColor, backColor);
	activeControlCommandID = ID_GALLERY_SHAPES;
	
	//	Get the size selection gallery
	pMainFrame->m_wndRibbonBar.GetElementsByID(ID_GALLERY_SIZE, arr);
	CMFCRibbonGallery* psGallery = (CMFCRibbonGallery*)arr[0];

	//	Set icons for shape tool sizes
	psGallery->SetPalette(IDB_SIZES1358, 104);
}

void CDrawView::OnButtonShapeOutline()
{
}

void CDrawView::OnButtonShapeFill()
{
}

void CDrawView::OnButtonShapeOutlineNoOutline()
{
	shapeOutline = NO_OUTLINE;
	currentTool->SetOutline(shapeOutline);
}

void CDrawView::OnButtonShapeOutlineSolidColor()
{
	shapeOutline = SOLID_COLOR_OUTLINE;
	currentTool->SetOutline(shapeOutline);
}

void CDrawView::OnButtonShapeFillNoFill()
{
	shapeFill = NO_FILL;
	currentTool->SetFill(shapeFill);
}

void CDrawView::OnButtonShapeFillSolidColor()
{
	shapeFill = SOLID_COLOR_FILL;
	currentTool->SetFill(shapeFill);
}

void CDrawView::OnGallerySize()
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_GALLERY_SIZE, arr);
	CMFCRibbonGallery* pGallery = (CMFCRibbonGallery*)arr[0];

	sizeIndex = pGallery->GetSelectedItem();
	currentTool->SetSizeByIndex(sizeIndex);
}

void CDrawView::OnForecolor()
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_FORECOLOR, arr);
	CMFCRibbonColorButton* pColorButton = (CMFCRibbonColorButton*)arr[0];

	COLORREF selectedColor = pColorButton->GetColor();

	if (selectedColor == CLR_INVALID)  // CLR_INVALID == 0xFFFFFFFF == 4294967295
	{
		selectedColor = pColorButton->GetAutomaticColor();
	}

	currentTool->SetPrimaryColor(selectedColor);
	foreColor = selectedColor;
}

void CDrawView::OnBackcolor()
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_BACKCOLOR, arr);
	CMFCRibbonColorButton* pColorButton = (CMFCRibbonColorButton*)arr[0];

	COLORREF selectedColor = pColorButton->GetColor();

	if (selectedColor == CLR_INVALID)  // CLR_INVALID == 0xFFFFFFFF == 4294967295
	{
		selectedColor = pColorButton->GetAutomaticColor();
	}

	currentTool->SetSecondaryColor(selectedColor);
	backColor = selectedColor;
}


void CDrawView::OnUpdateButtonPen(CCmdUI* pCmdUI)
{
	if (activeControlCommandID == ID_BUTTON_PEN)
	{
		pCmdUI->SetCheck(TRUE);
		DeselectShapes();
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

void CDrawView::OnUpdateButtonEraser(CCmdUI* pCmdUI)
{
	if (activeControlCommandID == ID_BUTTON_ERASER)
	{
		pCmdUI->SetCheck(TRUE);
		DeselectShapes();
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

void CDrawView::OnUpdateButtonBrush(CCmdUI* pCmdUI)
{
	if (activeControlCommandID == ID_BUTTON_BRUSH)
	{
		pCmdUI->SetCheck(TRUE);
		DeselectShapes();
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

void CDrawView::OnUpdateButtonFill(CCmdUI* pCmdUI)
{
	if (activeControlCommandID == ID_BUTTON_FILL)
	{
		pCmdUI->SetCheck(TRUE);
		DeselectShapes();
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

void CDrawView::OnUpdateButtonColorPicker(CCmdUI* pCmdUI)
{
	if (activeControlCommandID == ID_BUTTON_COLOR_PICKER)
	{
		pCmdUI->SetCheck(TRUE);
		DeselectShapes();
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

void CDrawView::OnUpdateButtonShapeOutline(CCmdUI* pCmdUI)
{
	if (activeControlCommandID == ID_GALLERY_SHAPES)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CDrawView::OnUpdateButtonShapeFill(CCmdUI* pCmdUI)
{
	if (activeControlCommandID == ID_GALLERY_SHAPES)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CDrawView::OnUpdateButtonOutlineNoOutline(CCmdUI* pCmdUI)
{
	if (shapeOutline == NO_OUTLINE)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

void CDrawView::OnUpdateButtonOutlineSolidColor(CCmdUI* pCmdUI)
{
	if (shapeOutline == SOLID_COLOR_OUTLINE)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

void CDrawView::OnUpdateButtonFillNoFill(CCmdUI* pCmdUI)
{
	if (shapeFill == NO_FILL)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

void CDrawView::OnUpdateButtonFillSolidColor(CCmdUI* pCmdUI)
{
	if (shapeFill == SOLID_COLOR_FILL)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
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

void CDrawView::OnPrimaryColorPicked(COLORREF color)
{
	foreColor = color;

	// Update the foreground color button in the ribbon bar
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	pMainFrame->m_wndRibbonBar.GetElementsByID(ID_FORECOLOR, arr);
	CMFCRibbonColorButton* pForeColorButton = (CMFCRibbonColorButton*)arr[0];
	pForeColorButton->SetColor(foreColor);
	pMainFrame->m_wndRibbonBar.ForceRecalcLayout();
}

void CDrawView::OnSecondaryColorPicked(COLORREF color)
{
	backColor = color;

	// Update the background color button in the ribbon bar
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	pMainFrame->m_wndRibbonBar.GetElementsByID(ID_BACKCOLOR, arr);
	CMFCRibbonColorButton* pBackColorButton = (CMFCRibbonColorButton*)arr[0];
	pBackColorButton->SetColor(backColor);
	pMainFrame->m_wndRibbonBar.ForceRecalcLayout();
}

void CDrawView::DeselectShapes()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CMFCRibbonBar* rbnBar = &pMainFrame->m_wndRibbonBar;
	CMFCRibbonGallery* pGalleryShapes = (CMFCRibbonGallery*)rbnBar->FindByID(ID_GALLERY_SHAPES);
	pGalleryShapes->SelectItem(-1);
}

