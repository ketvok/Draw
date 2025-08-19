#pragma once
#include "Drawable.h"
#include <vector>

class Pen :
    public Drawable
{
    std::vector<CPoint> points;
	CPen pen;
	int size;
	COLORREF color1;
	CPoint prevPoint;
public:
	Pen(int size, COLORREF color);
    void DrawYourself(CDC* pDC) override;
	void AddPoint(const CPoint& point);
	void SetPrevPoint(const CPoint& point);
	const CPoint& GetPrevPoint() const;
	CPen& GetPen() { return pen; }
};
