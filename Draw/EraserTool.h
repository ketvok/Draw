#pragma once
#include "Tool.h"

class EraserTool : public DrawingTool
{
	BOOL strokeInProgress;
	CPoint prevPoint;
	int eraserSize;
	COLORREF eraserColor;
public:
	EraserTool(int size, COLORREF color);
	void OnLButtonDown(CDC* pDC, const CPoint& point) override;
	void OnMouseMove(CDC* pDC, const CPoint& point) override;
	void OnLButtonUp() override;
	void SetSizeByIndex(int size) override;
	void SetPrimaryColor(COLORREF color) override { /* No foreground color for eraser */ }
	void SetSecondaryColor(COLORREF color) override { eraserColor = color; }
};
