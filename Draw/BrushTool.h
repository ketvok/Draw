#pragma once
#include "Tool.h"

class BrushTool : public DrawingTool
{
	BOOL strokeInProgress;
	CPoint prevPoint;
	int size;
	COLORREF color1;
	COLORREF color2;
public:
	BrushTool(int size, COLORREF color1, COLORREF color2);
	void OnLButtonDown(CDC* pDC, const CPoint& point) override;
	void OnRButtonDown(CDC* pDC, const CPoint& point) override;
	void OnMouseMove(CDC* pDC, const CPoint& point) override;
	void OnLButtonUp() override;
	void OnRButtonUp() override;
	void SetSizeByIndex(int size) override;
	void SetPrimaryColor(COLORREF color) override { color1 = color; }
	void SetSecondaryColor(COLORREF color) override { color2 = color; }
};
