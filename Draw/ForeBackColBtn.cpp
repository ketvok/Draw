// ForeBackColBtn.cpp : implementation file
//

#include "pch.h"
#include "Draw.h"
#include "ForeBackColBtn.h"
#include "MainFrm.h"

ForeBackColBtn::ForeBackColBtn(UINT nID, LPCTSTR lpszText)
	: CMFCRibbonButton(nID, lpszText)
{
	m_nID = nID;
}

void ForeBackColBtn::OnDraw(CDC* pDC)
{
	ASSERT (m_nID == ID_FORECOLOR || m_nID == ID_BACKCOLOR);
	
	// Call the base class method to handle the default drawing
	CMFCRibbonButton::OnDraw(pDC);

	// TODO: Tmp values, replace with actual color management logic
	COLORREF foregroundColor = RGB(0, 0, 0); // Black
	COLORREF backgroundColor = RGB(255, 255, 255); // White

	CRect rc = GetRect();

	int squareSideLength = rc.Width() < rc.Height() ? rc.Width() : rc.Height();

	// Position the square at the top of the button rectangle
	int squareBottom = rc.top + rc.Width();
	
	CRect squareRect{ rc.left, rc.top, rc.right, squareBottom };
	
	switch (m_nID)
	{
	case ID_FORECOLOR:
		// Decrease the rectangle size
		squareRect.left += 6;
		squareRect.right -= 6;
		squareRect.top += 3;
		squareRect.bottom -= 9;
		break;
	case ID_BACKCOLOR:
		// Decrease the rectangle size
		squareRect.left += 9;
		squareRect.right -= 9;
		squareRect.top += 6;
		squareRect.bottom -= 12;
		break;
	}

	// Add a border around the square
	pDC->FillSolidRect(squareRect, RGB(128, 128, 128));
	squareRect.DeflateRect(1, 1);
	pDC->FillSolidRect(squareRect, RGB(255, 255, 255));
	squareRect.DeflateRect(1, 1);

	CRect textRect = rc;
	CString text;
	switch (m_nID)
	{
	case ID_FORECOLOR:
		pDC->FillSolidRect(squareRect, foregroundColor);
		// Set up the text rectangle below the square
		textRect.top = squareRect.bottom += 7;  // Gap between square and text
		// Prepare the text;	TODO: put in string table in resource.h
		text.LoadString(IDS_FORECOLOR_LABEL);
		break;
	case ID_BACKCOLOR:
		pDC->FillSolidRect(squareRect, backgroundColor);
		// Set up the text rectangle below the square
		textRect.top = squareRect.bottom += 10;  // Gap between square and text
		// Prepare the text;	TODO: put in string table in resource.h
		text.LoadString(IDS_BACKCOLOR_LABEL);
		break;
	}

	// Draw the text centered in the text rectangle
	pDC->DrawText(text, textRect, DT_CENTER | DT_TOP | DT_WORDBREAK);
}

CSize ForeBackColBtn::GetRegularSize(CDC* pDC)
{
	return m_sizeCustomRegular;
}

void ForeBackColBtn::SetCustomRegularSize(CSize size)
{
	m_sizeCustomRegular = size;
}
