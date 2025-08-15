#pragma once
#include "Tool.h"

class BrushTool : public DrawingTool
{
	BOOL strokeInProgress;
	CPoint prevPoint;
	int brushSize;
	COLORREF brushColor;
	CPoint prevCurPoint;
public:
	BrushTool(int size, COLORREF color);
	void OnLButtonDown(CDC* pDC, const CPoint& point) override;
	void OnMouseMove(CDC* pDC, const CPoint& point) override;
	void OnLButtonUp() override;
	void SetSizeByIndex(int size) override;
	void SetPrimaryColor(COLORREF color) override { brushColor = color; }
	void SetSecondaryColor(COLORREF color) override { /* No background color */ }
};