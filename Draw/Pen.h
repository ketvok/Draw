#pragma once
#include "Drawable.h"

class Pen :
    public Drawable
{
    CArray<CPoint> points;
	CPen pen;
	int penSize;
	COLORREF penColor;
	CPoint prevPoint;
public:
	Pen(int size, COLORREF color);
    void DrawYourself(CDC* pDC) override;
	void AddPoint(const CPoint& point);
	void SetPrevPoint(const CPoint& point);
	const CPoint& GetPrevPoint() const;
};
