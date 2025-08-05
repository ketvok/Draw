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
