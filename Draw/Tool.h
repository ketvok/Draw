#pragma once
#include <afxwin.h>
#include <memory>  // std::unique_ptr

class DrawingTool
{
public:
	/// <summary>
	/// Observer interface for tools that need to communicate back to the view.
	/// Currently used by ColorPickerTool to notify color changes.
	/// </summary>
	class Observer
	{
	public:
		virtual void OnPrimaryColorPicked(COLORREF color) = 0;
		virtual void OnSecondaryColorPicked(COLORREF color) = 0;
	};

protected:
	Observer* observer = nullptr;

public:
	virtual ~DrawingTool() = default;

	virtual void SetObserver(Observer* observer) { this->observer = observer; }

	// Core functionality
	virtual void OnLButtonDown(CDC* pDC, const CPoint& point) = 0;

	// Optional
	virtual void OnLButtonUp() {}
	virtual void OnMouseMove(CDC* pDC, const CPoint& point) {}
	virtual void OnRButtonDown(CDC* pDC, const CPoint& point) {}
	virtual void OnRButtonUp() {}
	virtual void SetSizeByIndex(int index) {}
	virtual void SetPrimaryColor(COLORREF color) {}
	virtual void SetSecondaryColor(COLORREF color) {}
};

std::unique_ptr<DrawingTool> CreatePenTool(int size, COLORREF color1, COLORREF color2);
std::unique_ptr<DrawingTool> CreateEraserTool(int size, COLORREF color);
std::unique_ptr<DrawingTool> CreateBrushTool(int size, COLORREF color1, COLORREF color2);
std::unique_ptr<DrawingTool> CreateFillTool(COLORREF color1, COLORREF color2);
std::unique_ptr<DrawingTool> CreateColorPickerTool();