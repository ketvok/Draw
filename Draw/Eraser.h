#pragma once
#include "Drawable.h"

class Eraser :
    public Drawable
{
    CArray<CPoint> points;
    CPen pen;
	CBrush brush;
    int eraserSize;
    COLORREF eraserColor;
public:
    Eraser(int size, COLORREF color);
    void DrawYourself(CDC* pDC) override;
    void AddPoint(const CPoint& point);
};
