#pragma once
#include "Tool.h"

class ColorPickerTool : public DrawingTool
{
public:
    ColorPickerTool();
    void OnLButtonDown(CDC* pDC, const CPoint& point) override;
    void OnRButtonDown(CDC* pDC, const CPoint& point) override;
};