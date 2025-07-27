#include "pch.h"
#include "Eraser.h"

Eraser::Eraser(int size, COLORREF color) : pen{ PS_SOLID, 1, color }, brush { color }, eraserSize { size }, eraserColor{ color } {};

void Eraser::DrawYourself(CDC* pDC)
{
	pDC->SelectObject(&pen);  // Nothing is being drawn if the pen is not selected into pDC
	pDC->SelectObject(&brush);

	if (points.size() == 0)
	{
		return;  // No points to draw
	}
	for (int i = 0; i < points.size(); ++i)
	{
		pDC->Rectangle(points[i].x - eraserSize / 2, points[i].y - eraserSize / 2,
			points[i].x + eraserSize / 2, points[i].y + eraserSize / 2);
	}
}

void Eraser::AddPoint(const CPoint& point)
{
	points.push_back(point);
}

void Eraser::SetPrevPoint(const CPoint& point)
{
	prevPoint = point;
}

const CPoint& Eraser::GetPrevPoint() const
{
	return prevPoint;
}

void Eraser::OnLButtonDown(CDC* pDC, const CPoint& point)
{
	// Add current point to container of points and
	AddPoint(point);
	SetPrevPoint(point);  // mark it as previous.

	// Select a pen and brush with the current size and color
	CPen* pOldPen = pDC->SelectObject(&pen);
	CBrush* pOldBrush = pDC->SelectObject(&brush);

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

void Eraser::OnMouseMove(CDC* pDC, const CPoint& point)
{
	AddPoint(point);  // Add current point to container of points

	// Select a pen and brush with the background color
	CPen* pOldPen = pDC->SelectObject(&pen);
	CBrush* pOldBrush = pDC->SelectObject(&brush);

	// Calculate the points in between the previous point and the current point
	CPoint prevPoint = GetPrevPoint();
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
		AddPoint(point);
	}

	// Set the current point as previous
	SetPrevPoint(point);

	// Cleanup
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}