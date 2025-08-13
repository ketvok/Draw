#pragma once
#include "Tool.h"

class PenTool : public DrawingTool
{
	BOOL strokeInProgress;
	CPoint prevPoint;
	int penSize;
	COLORREF penColor;
public:
	PenTool(int size, COLORREF color);
	void OnLButtonDown(CDC* pDC, const CPoint& point) override;
	void OnMouseMove(CDC* pDC, const CPoint& point) override;
	void OnLButtonUp() override;
	void SetSizeByIndex(int size) override;
	void SetPrimaryColor(COLORREF color) override { penColor = color; }
	void SetSecondaryColor(COLORREF color) override { /* No background color for pen */ }
};
