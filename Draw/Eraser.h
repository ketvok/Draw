#pragma once
#include "Drawable.h"
#include <vector>

class Eraser :
    public Drawable
{
    std::vector<CPoint> points;
    CPen pen;
    int eraserSize;
    COLORREF eraserColor;
	CPoint prevPoint;
    CBrush brush;
public:
    Eraser(int size, COLORREF color);
    void DrawYourself(CDC* pDC) override;
    void AddPoint(const CPoint& point);
    void SetPrevPoint(const CPoint& point);
    const CPoint& GetPrevPoint() const;
};
