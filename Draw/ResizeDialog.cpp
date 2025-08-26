// ResizeDialog.cpp : implementation file
//

#include "pch.h"
#include "Draw.h"
#include "afxdialogex.h"
#include "ResizeDialog.h"


// ResizeDialog dialog

IMPLEMENT_DYNAMIC(ResizeDialog, CDialog)

ResizeDialog::ResizeDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_RESIZE, pParent)
{
	newWidth = 0;
	newHeight = 0;
}

ResizeDialog::~ResizeDialog()
{
}

void ResizeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ResizeDialog, CDialog)
END_MESSAGE_MAP()


// ResizeDialog message handlers

BOOL ResizeDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemInt(IDC_EDIT_WIDTH, newWidth);
	SetDlgItemInt(IDC_EDIT_HEIGHT, newHeight);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void ResizeDialog::OnOK()
{
	CDrawApp* pApp = (CDrawApp*)AfxGetApp();

	BOOL b;
	newWidth = GetDlgItemInt(IDC_EDIT_WIDTH, &b, FALSE);
	ASSERT(b);
	newHeight = GetDlgItemInt(IDC_EDIT_HEIGHT, &b, FALSE);
	ASSERT(b);

	if (newWidth < 8 || newWidth > 10000 || newHeight < 8 || newHeight > 10000)  // 8 = default min RectTracker size
	{
		CString error = pApp->LoadStringFromResource(IDS_DIALOG_SIZE_ERROR);
		AfxMessageBox(error);
		return;
	}

	CDialog::OnOK();
}
