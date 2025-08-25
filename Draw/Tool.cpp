#include "pch.h"
#include "Tool.h"
#include "PenTool.h"
#include "EraserTool.h"
#include "BrushTool.h"
#include "FillTool.h"
#include "ColorPickerTool.h"
#include "RectTool.h"
#include <stdexcept>
#include <vector>
#include <gdiplus.h>

// Tool creation

std::unique_ptr<DrawingTool> CreatePenTool(int size, COLORREF color1, COLORREF color2)
{
	return std::make_unique<PenTool>(size, color1, color2);
}

std::unique_ptr<DrawingTool> CreateEraserTool(int size, COLORREF color)
{
	return std::make_unique<EraserTool>(size, color);
}

std::unique_ptr<DrawingTool> CreateBrushTool(int size, COLORREF color1, COLORREF color2)
{
	return std::make_unique<BrushTool>(size, color1, color2);
}

std::unique_ptr<DrawingTool> CreateFillTool(COLORREF color1, COLORREF color2)
{ 
	return std::make_unique<FillTool>(color1, color2);
}

std::unique_ptr<DrawingTool> CreateColorPickerTool()
{
	return std::make_unique<ColorPickerTool>();
}

std::unique_ptr<DrawingTool> CreateRectTool(int selectedShape, shapeOutline outline, shapeFill fill, int size, COLORREF color1, COLORREF color2)
{
	return std::make_unique<RectTool>(selectedShape, outline, fill, size, color1, color2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// PenTool class implementation

PenTool::PenTool(int size, COLORREF color1, COLORREF color2) : strokeInProgress{ FALSE }, color1{ color1 }, color2{ color2 }
{
	SetSizeByIndex(size);
}

void PenTool::OnLButtonDown(CDC* pDC, const CPoint& point)
{
	strokeInProgress = TRUE;

	CPen pen{ PS_SOLID, size, color1 };
	CPen* pOldPen = pDC->SelectObject(&pen);

	// For pen sizes >= 5, MoveTo/LineTo produce good results for drawing
	// a dot, but for smaller sizes I do it manually for nicer results.

	if (size == 1)
	{
		pDC->MoveTo(point.x, point.y);
		pDC->SetPixel(point.x, point.y, color1);
	}
	else if (size == 2)  // If pen size is 2, draw a 2 x 2 square
	{
		pDC->MoveTo(point.x, point.y);
		pDC->SetPixel(point.x, point.y, color1);
		pDC->SetPixel(point.x - 1, point.y, color1);
		pDC->SetPixel(point.x, point.y - 1, color1);
		pDC->SetPixel(point.x - 1, point.y - 1, color1);
	}
	else if (size == 4)  // If pen size is 4, draw a 2 pixel wide cross
	{
		pDC->MoveTo(point.x, point.y);
		pDC->SetPixel(point.x, point.y, color1);
		pDC->SetPixel(point.x, point.y - 1, color1);
		pDC->SetPixel(point.x, point.y - 2, color1);
		pDC->SetPixel(point.x, point.y + 1, color1);
		pDC->SetPixel(point.x - 1, point.y, color1);
		pDC->SetPixel(point.x - 1, point.y - 1, color1);
		pDC->SetPixel(point.x - 1, point.y - 2, color1);
		pDC->SetPixel(point.x - 1, point.y + 1, color1);
		pDC->SetPixel(point.x - 2, point.y, color1);
		pDC->SetPixel(point.x - 2, point.y - 1, color1);
		pDC->SetPixel(point.x + 1, point.y, color1);
		pDC->SetPixel(point.x + 1, point.y - 1, color1);
	}
	else  // If pen size is > 4, use MoveTo/LineTo for drawing a dot
	{
		pDC->MoveTo(point.x, point.y);
		pDC->SetDCPenColor(color1);
		pDC->LineTo(point.x, point.y);
	}

	// Set the current point as previous
	prevPoint = point;

	// Cleanup
	pDC->SelectObject(pOldPen);
}

void PenTool::OnRButtonDown(CDC* pDC, const CPoint& point)
{
	strokeInProgress = TRUE;

	CPen pen{ PS_SOLID, size, color2 };
	CPen* pOldPen = pDC->SelectObject(&pen);

	// For pen sizes >= 5, MoveTo/LineTo produce good results for drawing
	// a dot, but for smaller sizes I do it manually for nicer results.

	if (size == 1)
	{
		pDC->MoveTo(point.x, point.y);
		pDC->SetPixel(point.x, point.y, color2);
	}
	else if (size == 2)  // If pen size is 2, draw a 2 x 2 square
	{
		pDC->MoveTo(point.x, point.y);
		pDC->SetPixel(point.x, point.y, color2);
		pDC->SetPixel(point.x - 1, point.y, color2);
		pDC->SetPixel(point.x, point.y - 1, color2);
		pDC->SetPixel(point.x - 1, point.y - 1, color2);
	}
	else if (size == 4)  // If pen size is 4, draw a 2 pixel wide cross
	{
		pDC->MoveTo(point.x, point.y);
		pDC->SetPixel(point.x, point.y, color2);
		pDC->SetPixel(point.x, point.y - 1, color2);
		pDC->SetPixel(point.x, point.y - 2, color2);
		pDC->SetPixel(point.x, point.y + 1, color2);
		pDC->SetPixel(point.x - 1, point.y, color2);
		pDC->SetPixel(point.x - 1, point.y - 1, color2);
		pDC->SetPixel(point.x - 1, point.y - 2, color2);
		pDC->SetPixel(point.x - 1, point.y + 1, color2);
		pDC->SetPixel(point.x - 2, point.y, color2);
		pDC->SetPixel(point.x - 2, point.y - 1, color2);
		pDC->SetPixel(point.x + 1, point.y, color2);
		pDC->SetPixel(point.x + 1, point.y - 1, color2);
	}
	else  // If pen size is > 4, use MoveTo/LineTo for drawing a dot
	{
		pDC->MoveTo(point.x, point.y);
		pDC->SetDCPenColor(color2);
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
	if (GetKeyState(VK_LBUTTON) & 0x8000)
		// If the high order bit of return value is set,
		//  the left mouse button is down.
	{
		CPen pen{ PS_SOLID, size, color1 };
		CPen* pOldPen = pDC->SelectObject(&pen);

		// Draw a line from previous point to the current point
		pDC->MoveTo(prevPoint);
		pDC->LineTo(point);

		// Set the current point as previous
		prevPoint = point;

		// Cleanup
		pDC->SelectObject(pOldPen);
	}
	if (GetKeyState(VK_RBUTTON) & 0x8000)
		// If the high order bit of return value is set,
		//  the right mouse button is down.
	{
		CPen pen{ PS_SOLID, size, color2 };
		CPen* pOldPen = pDC->SelectObject(&pen);

		// Draw a line from previous point to the current point
		pDC->MoveTo(prevPoint);
		pDC->LineTo(point);

		// Set the current point as previous
		prevPoint = point;

		// Cleanup
		pDC->SelectObject(pOldPen);
	}
}

void PenTool::OnLButtonUp()
{
	strokeInProgress = FALSE;  // End the stroke
}

void PenTool::OnRButtonUp()
{
	strokeInProgress = FALSE;  // End the stroke
}

void PenTool::SetSizeByIndex(int index)
{
	// Convert the index to the pen size
	std::vector<int> penSizes{ 1, 2, 3, 4 };
	ASSERT(index >= 0 && index < penSizes.size());
	size = penSizes[index];
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// EraserTool class implementation

EraserTool::EraserTool(int size, COLORREF color) : strokeInProgress{ FALSE }, color{ color }
{
	SetSizeByIndex(size);
}

void EraserTool::OnLButtonDown(CDC* pDC, const CPoint& point)
{
	strokeInProgress = TRUE;

	// Select a pen and brush with the current size and color
	CPen pen{ PS_SOLID, size, color };
	CPen* pOldPen = pDC->SelectObject(&pen);
	CBrush brush{ color };
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	
	// Draw a rectangle at the current point with the size of the eraser
	Rectangle(
		pDC->m_hDC,
		point.x - size / 2,   // left
		point.y - size / 2,   // top
		point.x + size / 2,   // right
		point.y + size / 2);  // bottom

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
	if (GetKeyState(VK_LBUTTON) & 0x8000)
		// If the high order bit of return value is set,
		//  the left mouse button is down.
	{
		// Select a pen and brush with the current size and color
		CPen pen{ PS_SOLID, size, color };
		CPen* pOldPen = pDC->SelectObject(&pen);
		CBrush brush{ color };
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
				x - size / 2,   // left
				y - size / 2,   // top
				x + size / 2,   // right
				y + size / 2);  // bottom
		}

		// Set the current point as previous
		prevPoint = point;

		// Cleanup
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
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
	size =  eraserSizes[index];
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// BrushTool class implementation

BrushTool::BrushTool(int size, COLORREF color1, COLORREF color2) : strokeInProgress{ FALSE }, color1 { color1 }, color2 {color2}
{
	SetSizeByIndex(size);
}

void BrushTool::OnLButtonDown(CDC* pDC, const CPoint& point)
{
	strokeInProgress = TRUE;
	
	if (size == 1)
	{
		pDC->SetPixel(point.x, point.y, color1);
	}
	else
	{
		Gdiplus::Graphics graphics(pDC->m_hDC);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		Gdiplus::SolidBrush brush(Gdiplus::Color(GetRValue(color1), GetGValue(color1), GetBValue(color1)));

		graphics.FillEllipse(&brush, point.x - size / 2, point.y - size / 2, size, size);
	}
	
	// Set the current point as previous
	prevPoint = point;
}

void BrushTool::OnRButtonDown(CDC* pDC, const CPoint& point)
{
	strokeInProgress = TRUE;

	if (size == 1)
	{
		pDC->SetPixel(point.x, point.y, color2);
	}
	else
	{
		Gdiplus::Graphics graphics(pDC->m_hDC);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		Gdiplus::SolidBrush brush(Gdiplus::Color(GetRValue(color2), GetGValue(color2), GetBValue(color2)));

		graphics.FillEllipse(&brush, point.x - size / 2, point.y - size / 2, size, size);
	}

	// Set the current point as previous
	prevPoint = point;
}

void BrushTool::OnMouseMove(CDC* pDC, const CPoint& point)
{
	if (strokeInProgress == FALSE)
	{
		return;  // If stroke is not in progress or current stroke is null, do nothing
	}
	if (GetKeyState(VK_LBUTTON) & 0x8000)
		// If the high order bit of return value is set,
		//  the left mouse button is down.
	{
		Gdiplus::Graphics graphics(pDC->m_hDC);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		if (size == 1)
		{
			Gdiplus::Pen pen(Gdiplus::Color(GetRValue(color1), GetGValue(color1), GetBValue(color1)), 1);
			graphics.DrawLine(&pen, prevPoint.x, prevPoint.y, point.x, point.y);
		}
		else
		{
			Gdiplus::SolidBrush brush(Gdiplus::Color(GetRValue(color1), GetGValue(color1), GetBValue(color1)));

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
				// Draw an ellipse at the interpolated point
				graphics.FillEllipse(&brush, x - size / 2, y - size / 2, size, size);
			}
		}

		// Set the current point as previous
		prevPoint = point;
	}
	if (GetKeyState(VK_RBUTTON) & 0x8000)
		// If the high order bit of return value is set,
		//  the right mouse button is down.
	{
		Gdiplus::Graphics graphics(pDC->m_hDC);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		if (size == 1)
		{
			Gdiplus::Pen pen(Gdiplus::Color(GetRValue(color2), GetGValue(color2), GetBValue(color2)), 1);
			graphics.DrawLine(&pen, prevPoint.x, prevPoint.y, point.x, point.y);
		}
		else
		{
			Gdiplus::SolidBrush brush(Gdiplus::Color(GetRValue(color2), GetGValue(color2), GetBValue(color2)));

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
				// Draw an ellipse at the interpolated point
				graphics.FillEllipse(&brush, x - size / 2, y - size / 2, size, size);
			}
		}

		// Set the current point as previous
		prevPoint = point;
	}
}

void BrushTool::OnLButtonUp()
{
	strokeInProgress = FALSE;  // End the stroke
}

void BrushTool::OnRButtonUp()
{
	strokeInProgress = FALSE;  // End the stroke
}

void BrushTool::SetSizeByIndex(int index)
{
	// Convert the index to the eraser size
	std::vector<int> brushSizes{ 1, 3, 5, 8 };
	ASSERT(index >= 0 && index < brushSizes.size());
	size = brushSizes[index];
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// FillTool class implementation

FillTool::FillTool(COLORREF color1, COLORREF color2) : color1{ color1 }, color2{ color2 } {}

void FillTool::OnLButtonDown(CDC* pDC, const CPoint& point)
{
	CBrush fillBrush;
	fillBrush.CreateSolidBrush(color1);
	CBrush* pOldBrush = pDC->SelectObject(&fillBrush);
	COLORREF pointColor;
	pointColor = pDC->GetPixel(point.x, point.y);

	ExtFloodFill(
		pDC->m_hDC,
		point.x, point.y, pointColor, FLOODFILLSURFACE);

	// Cleanup
	pDC->SelectObject(pOldBrush);
}

void FillTool::OnRButtonDown(CDC* pDC, const CPoint& point)
{
	CBrush fillBrush;
	fillBrush.CreateSolidBrush(color2);
	CBrush* pOldBrush = pDC->SelectObject(&fillBrush);
	COLORREF pointColor;
	pointColor = pDC->GetPixel(point.x, point.y);

	ExtFloodFill(
		pDC->m_hDC,
		point.x, point.y, pointColor, FLOODFILLSURFACE);

	// Cleanup
	pDC->SelectObject(pOldBrush);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// ColorPickerTool class implementation

ColorPickerTool::ColorPickerTool() {}

void ColorPickerTool::OnLButtonDown(CDC* pDC, const CPoint& point)
{
	COLORREF color = pDC->GetPixel(point.x, point.y);
	if (observer) observer->OnPrimaryColorPicked(color);
}

void ColorPickerTool::OnRButtonDown(CDC* pDC, const CPoint& point)
{
	COLORREF color = pDC->GetPixel(point.x, point.y);
	if (observer) observer->OnSecondaryColorPicked(color);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// RectTool class implementation

RectTool::RectTool(
	int selectedShape, shapeOutline outline, shapeFill fill, int size, COLORREF color1, COLORREF color2)
	    : selectedShape{ selectedShape }, outline{ outline }, fill{ fill }, size{ size }, color1{ color1 }, color2{ color2 }
{
	SetSizeByIndex(size);
}

void RectTool::DrawShapeOnLButtonDown(CDC* pDC, const CRect& rc)
{
	CPen pen;
	CBrush brush;
	CPen* pOldPen = nullptr;
	CBrush* pOldBrush = nullptr;
	COLORREF outlineColor = color1;
	COLORREF fillColor = color2;

	if (outline != NO_OUTLINE) // If shape has outline
	{
		pen.CreatePen(PS_SOLID, size, outlineColor);
		pOldPen = pDC->SelectObject(&pen);
	}
	else
	{
		pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
	}

	if (fill != NO_FILL)  // If shape has fill
	{
		brush.CreateSolidBrush(fillColor);
		pOldBrush = pDC->SelectObject(&brush);
	}
	else
	{
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	}

	switch (selectedShape)
	{
	case ELLIPSE:
		pDC->Ellipse(rc);
		break;
	case RECTANGLE:
		pDC->Rectangle(rc);
		break;
	case ROUNDED_RECTANGLE:
		pDC->RoundRect(rc, CPoint(20, 20));
		break;
	}

	// Cleanup
	if (pOldPen) pDC->SelectObject(pOldPen);
	if (pOldBrush) pDC->SelectObject(pOldBrush);
}

void RectTool::DrawShapeOnRButtonDown(CDC* pDC, const CRect& rc)
{
	CPen pen;
	CBrush brush;
	CPen* pOldPen = nullptr;
	CBrush* pOldBrush = nullptr;
	COLORREF outlineColor = color2; // Swap colors for right button
	COLORREF fillColor = color1;

	if (outline != NO_OUTLINE) // If shape has outline
	{
		pen.CreatePen(PS_SOLID, size, outlineColor);
		pOldPen = pDC->SelectObject(&pen);
	}
	else
	{
		pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
	}

	if (fill != NO_FILL)  // If shape has fill
	{
		brush.CreateSolidBrush(fillColor);
		pOldBrush = pDC->SelectObject(&brush);
	}
	else
	{
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	}

	switch (selectedShape)
	{
	case ELLIPSE:
		pDC->Ellipse(rc);
		break;
	case RECTANGLE:
		pDC->Rectangle(rc);
		break;
	case ROUNDED_RECTANGLE:
		pDC->RoundRect(rc, CPoint(20, 20));
		break;
	}

	// Cleanup
	if (pOldPen) pDC->SelectObject(pOldPen);
	if (pOldBrush) pDC->SelectObject(pOldBrush);
}

void RectTool::SetSizeByIndex(int index)
{
	// Convert the index to the outline size
	std::vector<int> shapeSizes{ 1, 3, 5, 8 };
	ASSERT(index >= 0 && index < shapeSizes.size());
	size = shapeSizes[index];
}