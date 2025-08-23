// Draw.h : main header file for the Draw application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CDrawApp:
// See Draw.cpp for the implementation of this class
//

class CDrawApp : public CWinAppEx
{
public:
	CDrawApp() noexcept;

// Helper methods
	/// <summary>
	/// Loads a string from the resource file using the specified resource ID. </summary>
	/// <param name="nID">Resource ID of the string to load.</param>
	/// <returns>
	/// Loaded string from the resource file.</returns>
	static CString LoadStringFromResource(UINT nID);


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CDrawApp theApp;
