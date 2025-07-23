#pragma once
#include "Drawable.h"

class Eraser :
    public Drawable
{
    CArray<CPoint> points;
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
