#pragma once
#include <afxwin.h>
#include <memory>  // std::unique_ptr

enum ToolType
{
	PEN_TOOL = 0,
	ERASER_TOOL = 1,
	BRUSH_TOOL = 2
};

class DrawingTool
{
public:
	virtual ~DrawingTool() = default;
	virtual void OnLButtonDown(CDC* pDC, const CPoint& point) = 0;
	virtual void OnMouseMove(CDC* pDC, const CPoint& point) = 0;
	virtual void OnLButtonUp() = 0;
	virtual void SetSizeByIndex(int index) = 0;
	virtual void SetPrimaryColor(COLORREF color) = 0;
	virtual void SetSecondaryColor(COLORREF color) = 0;
};

std::unique_ptr<DrawingTool> CreateTool(int toolType, int size, COLORREF color);