#pragma once

// Drawable command target

class Drawable : public CObject
{
public:
	virtual ~Drawable() = default;
	virtual void DrawYourself(CDC* pDC) = 0;
};
