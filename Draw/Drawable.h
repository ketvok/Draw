#pragma once

// Drawable command target

class Drawable : public CObject
{
public:
	virtual ~Drawable() = default;
	virtual void DrawYourself(CDC* pDC) = 0;
	virtual void OnLButtonDown(CDC* pDC, const CPoint& point) = 0;
	virtual void OnMouseMove(CDC* pDC, const CPoint& point) = 0;
};
