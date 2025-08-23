// MainFrm.h : interface of the CMainFrame class
//

#pragma once

class CMainFrame : public CFrameWndEx
{
private:
	CPoint curCoordinates;
	CSize canvasSize;
	
protected: // create from serialization only
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
	CMFCRibbonBar m_wndRibbonBar;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	/// <summary>
	/// Updates the current cursor coordinates displayed in the status bar. </summary>
	/// <param name="point">The current cursor coordinates to display.</param>
	void UpdateStatusCurCoordinates(CPoint point) { curCoordinates = point; }
	/// <summary>
	/// Updates the canvas size displayed in the status bar. </summary>
	/// <param name="size">The new canvas size to display.</param>
	void UpdateStatusCanvasSize(CSize size) { canvasSize = size; }

protected:  // control bar embedded members
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	/// <summary>
	/// Updates the state of the first pane in the status bar. </summary>
	/// <param name="pCmdUI">Pointer to a CCmdUI object that manages the user interface state for the status bar pane.</param>
	afx_msg void OnUpdateStatusBarPane1(CCmdUI* pCmdUI);
	/// <summary>
	/// Updates the state of the second pane in the status bar. </summary>
	/// <param name="pCmdUI">Pointer to a CCmdUI object that manages the user interface state for the status bar pane.</param>
	afx_msg void OnUpdateStatusBarPane2(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

public:
};


