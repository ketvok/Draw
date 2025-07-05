#include "pch.h"
#include "Pen.h"

Pen::Pen(int size, COLORREF color) : pen{ PS_SOLID, size, color }, penSize{ size }, penColor{ color } {}

void Pen::DrawYourself(CDC* pDC)
{
	// For pen sizes >= 5, MoveTo/LineTo produce good results for drawing
	// a dot, but for smaller sizes I do it manually for nicer results.

	pDC->SelectObject(&pen);  // Nothing is being drawn if a pen is not selected into pDC
	
	if (points.GetSize() == 0)
	{
		return; // No points to draw
	}
	for (int i = 0; i < points.GetCount(); ++i)
	{
		CPoint point = points[i];
		if (i == 0)  // First and possibly only point
		{
			if (penSize == 1)
			{
				pDC->MoveTo(point);
				pDC->SetPixel(point, penColor);  // LineTo cannot draw a single pixel
			}
			else if (penSize == 2)  // If pen size is 2, draw a 2 x 2 square
			{ 
				pDC->MoveTo(point);
				pDC->SetPixel(point, penColor);
				pDC->SetPixel(point.x - 1, point.y, penColor);
				pDC->SetPixel(point.x, point.y - 1, penColor);
				pDC->SetPixel(point.x - 1, point.y - 1, penColor);
			}
			else if (penSize == 4)  // If pen size is 4, draw a 2 pixel wide cross
			{
				pDC->MoveTo(point);
				pDC->SetPixel(point, penColor);
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
			else  // If pen size is > 4, use MoveTo/LineTo for drawing a dot at first point
			{
				pDC->MoveTo(point);
				pDC->LineTo(point);
			}
		}
		else  // Subsequent points after the first
		{
			pDC->LineTo(point);
		}
	}
}

void Pen::AddPoint(const CPoint& point)
{
	points.Add(point);
}

void Pen::SetPrevPoint(const CPoint& point)
{
	prevPoint = point;
}

const CPoint& Pen::GetPrevPoint() const
{
	return prevPoint;
}
