// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface
// (the "Fluent UI") and is provided only as referential material to supplement the
// Microsoft Foundation Classes Reference and related electronic documentation
// included with the MFC C++ library software.
// License terms to copy, use or distribute the Fluent UI are available separately.
// To learn more about our Fluent UI licensing program, please visit
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// DrawDoc.cpp : implementation of the CDrawDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Draw.h"
#endif

#include "DrawDoc.h"
#include "MainFrm.h"
#include "Pen.h"
#include "Eraser.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDrawDoc

IMPLEMENT_DYNCREATE(CDrawDoc, CDocument)

BEGIN_MESSAGE_MAP(CDrawDoc, CDocument)
	ON_COMMAND(ID_BUTTON_PEN, &CDrawDoc::OnButtonPen)
	ON_COMMAND(ID_BUTTON_ERASER, &CDrawDoc::OnButtonEraser)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_PEN, &CDrawDoc::OnUpdateButtonPen)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ERASER, &CDrawDoc::OnUpdateButtonEraser)
	ON_COMMAND(ID_GALLERY_SIZE, &CDrawDoc::OnGallerySize)
END_MESSAGE_MAP()


// CDrawDoc construction/destruction

CDrawDoc::CDrawDoc() noexcept : selectedDrawingTool{ pen }, sizePen{ 1 }, sizeEraser{ 4 }, sizeShape{ 1 }
{
	// TODO: add one-time construction code here

}

CDrawDoc::~CDrawDoc()
{
}

BOOL CDrawDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

void CDrawDoc::AddPoint(const CPoint& point)
{
	ASSERT(drawableArr.size() > 0);

	switch (selectedDrawingTool)
	{
	case pen:
	{
		Pen* penObj = dynamic_cast<Pen*>(drawableArr[drawableArr.size() - 1].get());
		penObj->AddPoint(point);
		break;
	}
	case eraser:
	{
		Eraser* eraserObj = dynamic_cast<Eraser*>(drawableArr[drawableArr.size() - 1].get());
		eraserObj->AddPoint(point);
		break;
	}
	}
}

void CDrawDoc::AddObject(std::shared_ptr<Drawable> pObject)
{
	drawableArr.push_back(pObject);
}

const POINT& CDrawDoc::GetPrevPoint()
{
	ASSERT(drawableArr.size() > 0);

	switch (selectedDrawingTool)
	{
	case pen:
	{
		Pen* penObj = dynamic_cast<Pen*>(drawableArr[drawableArr.size() - 1].get());
		return penObj->GetPrevPoint();
		break;
	}
	case eraser:
	{
		Eraser* eraserObj = dynamic_cast<Eraser*>(drawableArr[drawableArr.size() - 1].get());
		return eraserObj->GetPrevPoint();
		break;
	}
	}
}

void CDrawDoc::SetPrevPoint(const CPoint& point)
{
	ASSERT(drawableArr.size() > 0);

	switch (selectedDrawingTool)
	{
	case pen:
	{
		dynamic_cast<Pen*>(drawableArr[drawableArr.size() - 1].get())->SetPrevPoint(point);
		break;
	}
	case eraser:
	{
		dynamic_cast<Eraser*>(drawableArr[drawableArr.size() - 1].get())->SetPrevPoint(point);
		break;
	}
	}
}

void CDrawDoc::DrawAll(CDC* pDC) const
{
	for (int i = 0; i < drawableArr.size(); ++i)
	{
		drawableArr[i]->DrawYourself(pDC);
	}
}




// CDrawDoc serialization

void CDrawDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CDrawDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CDrawDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CDrawDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CDrawDoc diagnostics

#ifdef _DEBUG
void CDrawDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDrawDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CDrawDoc commands

void CDrawDoc::OnButtonPen()
{
	selectedDrawingTool = pen;
	
	// Get the size selection gallery
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_GALLERY_SIZE, arr);
	CMFCRibbonGallery* pGallery = DYNAMIC_DOWNCAST(CMFCRibbonGallery, arr[0]);

	// Set icons for pen tool sizes
	pGallery->SetPalette(IDB_SIZES1234, 104);
}

void CDrawDoc::OnButtonEraser()
{
	selectedDrawingTool = eraser;

	// Get the size selection gallery
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_GALLERY_SIZE, arr);
	CMFCRibbonGallery* pGallery = DYNAMIC_DOWNCAST(CMFCRibbonGallery, arr[0]);

	// Set icons for eraser tool sizes
	pGallery->SetPalette(IDB_SIZES46810, 104);
}

void CDrawDoc::OnUpdateButtonPen(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(selectedDrawingTool == pen ? 1 : 0);
}

void CDrawDoc::OnUpdateButtonEraser(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(selectedDrawingTool == eraser ? 1 : 0);
}

void CDrawDoc::OnGallerySize()
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_GALLERY_SIZE, arr);
	CMFCRibbonGallery* pGallery = DYNAMIC_DOWNCAST(CMFCRibbonGallery, arr[0]);

	int index = pGallery->GetSelectedItem();

	switch (index)
	{
	case 0:
		sizePen = 1;
		sizeEraser = 4;
		sizeShape = 1;
		break;
	case 1:
		sizePen = 2;
		sizeEraser = 6;
		sizeShape = 3;
		break;
	case 2:
		sizePen = 3;
		sizeEraser = 8;
		sizeShape = 5;
		break;
	case 3:
		sizePen = 4;
		sizeEraser = 10;
		sizeShape = 8;
		break;
	}
}
