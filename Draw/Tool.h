#pragma once
#include <afxwin.h>
#include <memory>
#include "DrawDoc.h"
#include "Pen.h"
#include "Eraser.h"

enum ToolType {
	pen,
	eraser
};

class DrawingTool
{
public:
	virtual ~DrawingTool() = default;
	virtual void OnLButtonDown(CDC* pDC, const CPoint& point, CDrawDoc* pDoc) = 0;
	virtual void OnMouseMove(CDC* pDC, const CPoint& point, CDrawDoc* pDoc) = 0;
	virtual void OnLButtonUp() = 0;
	virtual void SetSize(int size) = 0;
	virtual void SetColor(COLORREF color) = 0;
	virtual ToolType GetToolType() const = 0;
};

class PenTool : public DrawingTool
{
	std::shared_ptr<Pen> currentStroke;
	int penSize;
	COLORREF penColor;
public:
	PenTool(int size = 1, COLORREF color = RGB(0, 0, 0));
	void OnLButtonDown(CDC* pDC, const CPoint& point, CDrawDoc* pDoc) override;
	void OnMouseMove(CDC* pDC, const CPoint& point, CDrawDoc* pDoc) override;
	void OnLButtonUp() override;
	void SetSize(int size) override { penSize = size; }
	void SetColor(COLORREF color) override { penColor = color; }
	ToolType GetToolType() const override { return ToolType::pen; }
};

class EraserTool : public DrawingTool
{
	std::shared_ptr<Eraser> currentStroke;
	int eraserSize;
	COLORREF eraserColor;
public:
	EraserTool(int size = 1, COLORREF color = RGB(255, 255,255));
	void OnLButtonDown(CDC* pDC, const CPoint& point, CDrawDoc* pDoc) override;
	void OnMouseMove(CDC* pDC, const CPoint& point, CDrawDoc* pDoc) override;
	void OnLButtonUp() override;
	void SetSize(int size) override { eraserSize = size; }
	void SetColor(COLORREF color) override { eraserColor = color; }
	ToolType GetToolType() const override { return ToolType::eraser; }
};
