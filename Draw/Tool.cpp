#include "pch.h"
#include "Tool.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// PenTool class implementation

PenTool::PenTool(int size, COLORREF color) : currentStroke{ nullptr }, penSize{ size }, penColor{ color } {}

void PenTool::OnLButtonDown(CDC* pDC, const CPoint& point, CDrawDoc* pDoc)
{
    currentStroke = std::make_shared<Pen>(penSize, penColor);
    currentStroke->AddPoint(point);
    currentStroke->SetPrevPoint(point);
    
    // Add the pen to the document's drawable array
    pDoc->AddObject(currentStroke);

	pDoc->SetModifiedFlag(TRUE);

	// Select a pen with the current size and color
	CPen* pOldPen = pDC->SelectObject(&currentStroke->GetPen());
	pDC->SelectObject(currentStroke.get());


	// For pen sizes >= 5, MoveTo/LineTo produce good results for drawing
	// a dot, but for smaller sizes I do it manually for nicer results.

	if (penSize == 1)
	{
		pDC->MoveTo(point.x, point.y);
		pDC->SetPixel(point.x, point.y, penColor);
	}
	else if (penSize == 2)  // If pen size is 2, draw a 2 x 2 square
	{
		pDC->MoveTo(point.x, point.y);
		pDC->SetPixel(point.x, point.y, penColor);
		pDC->SetPixel(point.x - 1, point.y, penColor);
		pDC->SetPixel(point.x, point.y - 1, penColor);
		pDC->SetPixel(point.x - 1, point.y - 1, penColor);
	}
	else if (penSize == 4)  // If pen size is 4, draw a 2 pixel wide cross
	{
		pDC->MoveTo(point.x, point.y);
		pDC->SetPixel(point.x, point.y, penColor);
		pDC->SetPixel(point.x, point.y - 1, penColor);
		pDC->SetPixel(point.x, point.y - 2, penColor);
		pDC->SetPixel(point.x, point.y + 1, penColor);
		pDC->SetPixel(point.x - 1, point.y, penColor);
		pDC->SetPixel(point.x - 1, point.y - 1, penColor);
		pDC->SetPixel(point.x - 1, point.y - 2, penColor);
		pDC->SetPixel(point.x - 1, point.y + 1, penColor);
		pDC->SetPixel(point.x - 2, point.y, penColor);
		pDC->SetPixel(point.x - 2, point.y - 1, penColor);
		pDC->SetPixel(point.x + 1, point.y, penColor);
		pDC->SetPixel(point.x + 1, point.y - 1, penColor);
	}
	else  // If pen size is > 4, use MoveTo/LineTo for drawing a dot
	{
		pDC->MoveTo(point.x, point.y);
		pDC->SetDCPenColor(penColor);
		pDC->LineTo(point.x, point.y);
	}

	// Cleanup
	pDC->SelectObject(pOldPen);
}

void PenTool::OnMouseMove(CDC* pDC, const CPoint& point, CDrawDoc* pDoc)
{
	if (!currentStroke)
	{
		return;  // If stroke is not in progress or current stroke is null, do nothing
	}

	// Add current point to container of points
	currentStroke->AddPoint(point);

	// Select a pen with the current size and color
	CPen* pOldPen = pDC->SelectObject(&currentStroke->GetPen());

	// Draw a line from previous point to the current point
	pDC->MoveTo(currentStroke->GetPrevPoint());
	pDC->LineTo(point);

	// Set the current point as previous
	currentStroke->SetPrevPoint(point);

	// Cleanup
	pDC->SelectObject(pOldPen);
}

void PenTool::OnLButtonUp()
{
	currentStroke = nullptr;  // End the stroke
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// EraserTool class implementation

EraserTool::EraserTool(int size, COLORREF color) : currentStroke{ nullptr }, eraserSize{ size }, eraserColor{ color } {}

void EraserTool::OnLButtonDown(CDC* pDC, const CPoint& point, CDrawDoc* pDoc)
{
	currentStroke = std::make_shared<Eraser>(eraserSize, eraserColor);
	currentStroke->AddPoint(point);
	currentStroke->SetPrevPoint(point);

	// Add the eraser to the document's drawable array
	pDoc->AddObject(currentStroke);

	pDoc->SetModifiedFlag(TRUE);
	
	// Select a pen and brush with the current size and color
	CPen* pOldPen = pDC->SelectObject(&currentStroke->GetPen());
	CBrush* pOldBrush = pDC->SelectObject(&currentStroke->GetBrush());
	
	// Draw a rectangle at the current point with the size of the eraser
	Rectangle(
		pDC->m_hDC,
		point.x - eraserSize / 2,   // left
		point.y - eraserSize / 2,   // top
		point.x + eraserSize / 2,   // right
		point.y + eraserSize / 2);  // bottom
	
	// Cleanup
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

void EraserTool::OnMouseMove(CDC* pDC, const CPoint& point, CDrawDoc* pDoc)
{
	if (!currentStroke)
	{
		return;  // If stroke is not in progress or current stroke is null, do nothing
	}

	currentStroke->AddPoint(point);  // Add current point to container of points
	
	// Select a pen and brush with the background color
	CPen* pOldPen = pDC->SelectObject(&currentStroke->GetPen());
	CBrush* pOldBrush = pDC->SelectObject(&currentStroke->GetBrush());
	
	// Calculate the points in between the previous point and the current point
	CPoint prevPoint = currentStroke->GetPrevPoint();
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
		Rectangle(
			pDC->m_hDC,
			x - eraserSize / 2,   // left
			y - eraserSize / 2,   // top
			x + eraserSize / 2,   // right
			y + eraserSize / 2);  // bottom
	
		// Add the interpolated point to the last Drawable object (Eraser)
		currentStroke->AddPoint(point);
	}
	
	// Set the current point as previous
	currentStroke->SetPrevPoint(point);
	
	// Cleanup
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

void EraserTool::OnLButtonUp()
{
    currentStroke = nullptr;  // End the stroke
}
