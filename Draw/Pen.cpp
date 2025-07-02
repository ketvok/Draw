#include "pch.h"
#include "Pen.h"

Pen::Pen(int size, COLORREF color) : pen{ PS_SOLID, size, color }, penSize{ size }, penColor{ color } {}

void Pen::DrawYourself(CDC* pDC)
{
	pDC->SelectObject(pen);  // Nothing is being drawn if a pen is not selected into pDC
	
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
			else if (penSize == 2)
			{ 
				pDC->MoveTo(point);
				pDC->SetPixel(point, penColor);
				pDC->SetPixel(point.x - 1, point.y, penColor);
				pDC->SetPixel(point.x, point.y - 1, penColor);
				pDC->SetPixel(point.x - 1, point.y - 1, penColor);
			}
			else if (penSize == 4)
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
			else
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

void Pen::addPoint(const POINT& point)
{
	points.Add(point);
}

void Pen::setPrevPoint(const POINT& point)
{
	prevPoint = point;
}

const POINT& Pen::getPrevPoint()
{
	return prevPoint;
}
