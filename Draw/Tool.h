#pragma once
#include <afxwin.h>
#include <memory>  // std::unique_ptr

enum shape {
	ELLIPSE = 0,
	RECTANGLE = 1,
	ROUNDED_RECTANGLE = 2
};

enum shapeOutline {
	NO_OUTLINE = 0,
	SOLID_COLOR_OUTLINE = 1
};

enum shapeFill {
	NO_FILL = 0,
	SOLID_COLOR_FILL = 1
};

class DrawingTool
{
public:
	/// <summary>
	/// Observer interface for tools that need to communicate back to the view.
	/// Currently used by ColorPickerTool to notify color changes.
	/// </summary>
	class ToolObserver
	{
	public:
		virtual void OnPrimaryColorPicked(COLORREF color) = 0;
		virtual void OnSecondaryColorPicked(COLORREF color) = 0;
	};

protected:
	ToolObserver* observer = nullptr;

public:
	virtual ~DrawingTool() = default;

	virtual void SetObserver(ToolObserver* observer) { this->observer = observer; }

	// Optional
	virtual void OnLButtonDown(CDC* pDC, const CPoint& point) {}
	virtual void OnLButtonUp() {}
	virtual void OnMouseMove(CDC* pDC, const CPoint& point) {}
	virtual void OnRButtonDown(CDC* pDC, const CPoint& point) {}
	virtual void OnRButtonUp() {}
	virtual void SetSizeByIndex(int index) {}
	virtual void SetPrimaryColor(COLORREF color) {}
	virtual void SetSecondaryColor(COLORREF color) {}
	virtual void SetOutline(shapeOutline outline) {}
	virtual void SetFill(shapeFill fill) {}
	virtual void DrawShapeOnLButtonDown(CDC* pDC, const CRect& rc) {}
	virtual void DrawShapeOnRButtonDown(CDC* pDC, const CRect& rc) {}
};

std::unique_ptr<DrawingTool> CreatePenTool(int size, COLORREF color1, COLORREF color2);
std::unique_ptr<DrawingTool> CreateEraserTool(int size, COLORREF color);
std::unique_ptr<DrawingTool> CreateBrushTool(int size, COLORREF color1, COLORREF color2);
std::unique_ptr<DrawingTool> CreateFillTool(COLORREF color1, COLORREF color2);
std::unique_ptr<DrawingTool> CreateColorPickerTool();
std::unique_ptr<DrawingTool> CreateRectTool(int selectedShape, shapeOutline outline, shapeFill fill, int size, COLORREF color1, COLORREF color2);