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
END_MESSAGE_MAP()


// CDrawDoc construction/destruction

CDrawDoc::CDrawDoc() noexcept
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

void CDrawDoc::AddObject(std::shared_ptr<Drawable> pObject)
{
	drawableArr.push_back(pObject);
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

void CDrawDoc::DeleteContents()
{
	drawableArr.clear();
	canvasDC.DeleteDC();
	canvasBitmap.DeleteObject();

	CDocument::DeleteContents();
}

BOOL CDrawDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// Check if the file extension indicates an image format rather than a document
	CString strPath = lpszPathName;
	CString strExt = strPath.Right(5).MakeLower();

	if (strExt.Right(4) == ".jpg"  ||
		strExt.Right(5) == ".jpeg" ||
		strExt.Right(4) == ".png"  ||
		strExt.Right(4) == ".bmp"  ||
		strExt.Right(4) == ".gif")
	{
		// This is an image file - save directly using CImage
		CImage image;

		// Get information about the bitmap
		BITMAP bmpInfo;
		//canvasBitmap.GetBitmap(&bmpInfo);
		CDC& canvasDC = *GetCanvasDC();
		CBitmap* bm = canvasDC.GetCurrentBitmap();
		bm->GetBitmap(&bmpInfo);

		// Create an image from the bitmap in memory
		image.Create(bmpInfo.bmWidth, bmpInfo.bmHeight, bmpInfo.bmBitsPixel);

		// Create a temporary DC
		CDC tempDC;
		tempDC.CreateCompatibleDC(&canvasDC);

		// Select the image into the DC
		HBITMAP hOldBitmap = (HBITMAP)tempDC.SelectObject(image);

		// Copy canvas area to the image
		tempDC.BitBlt(0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight,
			&canvasDC, 0, 0, SRCCOPY);

		// Clean up
		tempDC.SelectObject(hOldBitmap);

		// Save the image based on file extension
		HRESULT hr;
		if (strExt.Right(4) == ".png")
			hr = image.Save(lpszPathName, Gdiplus::ImageFormatPNG);
		else if (strExt.Right(4) == ".jpg" || strExt.Right(5) == ".jpeg")
			hr = image.Save(lpszPathName, Gdiplus::ImageFormatJPEG);
		else if (strExt.Right(4) == ".gif")
			hr = image.Save(lpszPathName, Gdiplus::ImageFormatGIF);
		else  // strExt == .bmp
			hr = image.Save(lpszPathName, Gdiplus::ImageFormatBMP);

		return SUCCEEDED(hr);
	}


	return CDocument::OnSaveDocument(lpszPathName);
}
