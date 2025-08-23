#pragma once
#include "afxdialogex.h"


// ResizeDialog dialog

class ResizeDialog : public CDialog
{
	DECLARE_DYNAMIC(ResizeDialog)

public:
// Public data members
	int newWidth;
	int newHeight;

// Public function members
	ResizeDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ResizeDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESIZE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
