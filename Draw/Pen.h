#pragma once
#include "Drawable.h"
#include <vector>

class Pen :
    public Drawable
{
    std::vector<CPoint> points;
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
