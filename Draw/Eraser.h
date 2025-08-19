#pragma once
#include "Drawable.h"
#include <vector>

class Eraser :
    public Drawable
{
    std::vector<CPoint> points;
    CPen pen;
    CBrush brush;
    int size;
    COLORREF color;
	CPoint prevPoint;
public:
    Eraser(int size, COLORREF color);
    void DrawYourself(CDC* pDC) override;
    void AddPoint(const CPoint& point);
    void SetPrevPoint(const CPoint& point);
    const CPoint& GetPrevPoint() const;
	CPen& GetPen() { return pen; }
    CBrush& GetBrush() { return brush; }
};
