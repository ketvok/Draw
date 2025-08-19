#pragma once
#include "Tool.h"

class EraserTool : public DrawingTool
{
	BOOL strokeInProgress;
	CPoint prevPoint;
	int size;
	COLORREF color;
public:
	EraserTool(int size, COLORREF color);
	void OnLButtonDown(CDC* pDC, const CPoint& point) override;
	void OnMouseMove(CDC* pDC, const CPoint& point) override;
	void OnLButtonUp() override;
	void SetSizeByIndex(int size) override;
	void SetSecondaryColor(COLORREF color) override { this->color = color; }
};
