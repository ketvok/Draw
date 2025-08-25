#pragma once
#include "Tool.h"

class RectTool : public DrawingTool
{
	int selectedShape;
	shapeOutline outline;
	shapeFill fill;
	int size;
	COLORREF color1;
	COLORREF color2;
public:
	RectTool(int selectedShape, shapeOutline outline, shapeFill fill, int size, COLORREF color1, COLORREF color2);
	void SetSizeByIndex(int size) override;
	void SetPrimaryColor(COLORREF color) override { color1 = color; }
	void SetSecondaryColor(COLORREF color) override { color2 = color; }
	void SetOutline(shapeOutline outline) override { this->outline = outline; }
	void SetFill(shapeFill fill) override { this->fill = fill; }
	void DrawShapeOnLButtonDown(CDC* pDC, const CRect& rc) override;
	void DrawShapeOnRButtonDown(CDC* pDC, const CRect& rc) override;
};