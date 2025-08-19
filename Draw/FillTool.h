#pragma once
#include "Tool.h"

class FillTool : public DrawingTool
{
	COLORREF color1;
	COLORREF color2;
public:
	FillTool(COLORREF color1, COLORREF color2);
	void OnLButtonDown(CDC* pDC, const CPoint& point) override;
	void OnRButtonDown(CDC* pDC, const CPoint& point) override;
	void SetPrimaryColor(COLORREF color) override { color1 = color; }
	void SetSecondaryColor(COLORREF color) override { color2 = color; }
};
