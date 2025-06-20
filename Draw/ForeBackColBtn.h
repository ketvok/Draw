#pragma once

// ForeBackColBtn command target

class ForeBackColBtn : public CMFCRibbonButton
{
private:
	UINT m_nID; // Button ID
	CSize m_sizeCustomRegular; // Custom size for the button
public:
	ForeBackColBtn(UINT nID, LPCTSTR lpszText);
	void OnDraw(CDC* pDC) override;
	CSize GetRegularSize(CDC* pDC) override;
	void SetCustomRegularSize(CSize size);
};