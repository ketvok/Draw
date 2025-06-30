#pragma once
#include "Drawable.h"
class Pen :
    public Drawable
{
    CArray<CPoint> points;
	BOOL isDown{ FALSE };
	int penSize{ 10 };
	CPen pen;
	COLORREF penColor{ RGB(255, 0, 0) }; // Red color for the pen
	CPoint prevPoint;
	BOOL strokeInProgress = FALSE;
public:
	Pen(int size, COLORREF color);
    void DrawYourself(CDC* pDC) override;
	void addPoint(const POINT& point);
	void setPrevPoint(const POINT& point);
	const POINT& getPrevPoint();
};

