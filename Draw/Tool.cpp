#include "pch.h"
#include "Tool.h"
#include "PenTool.h"
#include "EraserTool.h"
#include <stdexcept>
#include <vector>

// Factory
std::unique_ptr<DrawingTool> CreateTool(int toolType, int size, COLORREF color)
{
	switch (toolType)
	{
	case PEN_TOOL:
		return std::make_unique<PenTool>(size, color);
	case ERASER_TOOL:
		return std::make_unique<EraserTool>(size, color);
	default:
		throw std::invalid_argument("Invalid tool type");
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// PenTool class implementation

PenTool::PenTool(int size, COLORREF color) : strokeInProgress{ FALSE }, penColor{ color }
{
	SetSizeByIndex(size);
}

void PenTool::OnLButtonDown(CDC* pDC, const CPoint& point)
{
	strokeInProgress = TRUE;

	CPen pen{ PS_SOLID, penSize, penColor };
	CPen* pOldPen = pDC->SelectObject(&pen);

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

	// Set the current point as previous
	prevPoint = point;

	// Cleanup
	pDC->SelectObject(pOldPen);
}

void PenTool::OnMouseMove(CDC* pDC, const CPoint& point)
{
	if (strokeInProgress == FALSE)
	{
		return;  // If stroke is not in progress or current stroke is null, do nothing
	}

	CPen pen{ PS_SOLID, penSize, penColor };
	CPen* pOldPen = pDC->SelectObject(&pen);

	// Draw a line from previous point to the current point
	pDC->MoveTo(prevPoint);
	pDC->LineTo(point);

	// Set the current point as previous
	prevPoint = point;

	// Cleanup
	pDC->SelectObject(pOldPen);
}

void PenTool::OnLButtonUp()
{
	strokeInProgress = FALSE;  // End the stroke
}

void PenTool::SetSizeByIndex(int index)
{
	// Convert the index to the pen size
	std::vector<int> penSizes{ 1, 2, 3, 4 };
	ASSERT(index >= 0 && index < penSizes.size());
	penSize = penSizes[index];
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// EraserTool class implementation

EraserTool::EraserTool(int size, COLORREF color) : strokeInProgress{ FALSE }, eraserColor{ color }
{
	SetSizeByIndex(size);
}

void EraserTool::OnLButtonDown(CDC* pDC, const CPoint& point)
{
	strokeInProgress = TRUE;

	// Select a pen and brush with the current size and color
	CPen pen{ PS_SOLID, eraserSize, eraserColor };
	CPen* pOldPen = pDC->SelectObject(&pen);
	CBrush brush{ eraserColor };
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	
	// Draw a rectangle at the current point with the size of the eraser
	Rectangle(
		pDC->m_hDC,
		point.x - eraserSize / 2,   // left
		point.y - eraserSize / 2,   // top
		point.x + eraserSize / 2,   // right
		point.y + eraserSize / 2);  // bottom

	// Set the current point as previous
	prevPoint = point;

	// Cleanup
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

void EraserTool::OnMouseMove(CDC* pDC, const CPoint& point)
{
	if (strokeInProgress == FALSE)
	{
		return;  // If stroke is not in progress or current stroke is null, do nothing
	}
	
	// Select a pen and brush with the current size and color
	CPen pen{ PS_SOLID, eraserSize, eraserColor };
	CPen* pOldPen = pDC->SelectObject(&pen);
	CBrush brush{ eraserColor };
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	
	// Calculate the points in between the previous point and the current point
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
	}
	
	// Set the current point as previous
	prevPoint = point;
	
	// Cleanup
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

void EraserTool::OnLButtonUp()
{
	strokeInProgress = FALSE;  // End the stroke
}

void EraserTool::SetSizeByIndex(int index)
{
	// Convert the index to the eraser size
	std::vector<int> eraserSizes{ 4, 6, 8, 10 };
	ASSERT(index >= 0 && index < eraserSizes.size());
	eraserSize =  eraserSizes[index];
}