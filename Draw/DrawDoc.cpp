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
	ON_COMMAND(ID_FILE_OPEN, &CDrawDoc::OnFileOpen)
END_MESSAGE_MAP()


// CDrawDoc construction/destruction

CDrawDoc::CDrawDoc() noexcept : canvasSize{ 800, 600 }
{
	// TODO: add one-time construction code here

}

CDrawDoc::~CDrawDoc()
{
}

BOOL CDrawDoc::OnNewDocument()
{
	//The default implementation of this function calls the DeleteContents member function
	// to ensure that the document is empty and then marks the new document as clean.
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

void CDrawDoc::AddObject(std::shared_ptr<Drawable> pObject)
{
	drawableArr.push_back(pObject);

	SetModifiedFlag(TRUE);
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
	canvasDC.DeleteDC();  // Delete DC first, then delete bitmap, because
	canvasBitmap.DeleteObject();  // deleting bitmap while selected into DC will fail.
	canvasImage.Destroy();  // Destroy the image object

	CDocument::DeleteContents();
}


///////////////////////////////////////////////////////////////////////////////
//                                Saving file                                //
///////////////////////////////////////////////////////////////////////////////

// https://www.codeproject.com/Articles/2908/CDocument-DoSave-revealed
// Call order:
// File>Save   -> OnFileSave   -> DoFileSave -> DoSave -> OnSaveDocument
// File>SaveAs -> OnFileSaveAs ->               DoSave -> OnSaveDocument
// File>New    ->                               DoSave -> OnSaveDocument

BOOL CDrawDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
{
	CString newName = lpszPathName;

	// If no path name is provided or we're doing a "Save As" operation
	if (newName.IsEmpty() || !bReplace)
	{
		// Define file filters for different image formats
		CString strFilter =
			_T("PNG (*.png)|*.png|")
			_T("JPEG (*.jpg;*.jpeg)|*.jpg|")
			_T("BMP (*.bmp)|*.bmp|")
			_T("GIF (*.gif)|*.gif|")
			_T("TIFF (*.tiff)|*.tiff||");

		// Create file dialog for saving
		CFileDialog dlg(FALSE,	// FALSE = Save As dialog
			_T(".png"),			// Default extension
			_T("Untitled"),		// Default filename // myTODO: add to string table
			OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
			strFilter,			// File type filters
			AfxGetMainWnd());	// Parent window

		// Set default filter to the first one (PNG)
		dlg.m_ofn.nFilterIndex = 1;

		// Set dialog title
		dlg.m_ofn.lpstrTitle = _T("Save As");  // myTODO: add to string table

		// Open the dialog and check if the user clicked OK
		if (dlg.DoModal() != IDOK)
			return FALSE;  // User cancelled

		newName = dlg.GetPathName();
	}

	// Save the document using the existing OnSaveDocument method
	if (!OnSaveDocument(newName))
		return FALSE;

	// If successful, update the document's pathname and clear modified flag
	if (bReplace)
	{
		SetPathName(newName);  // This is so that the dialog will not prompt for a filename on "Save"
		SetModifiedFlag(FALSE);
	}

	return TRUE;
}

BOOL CDrawDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// Get information about the bitmap
	BITMAP bmpInfo;
	//canvasBitmap.GetBitmap(&bmpInfo);
	CDC& canvasDC = *GetCanvasDC();
	CBitmap* bm = canvasDC.GetCurrentBitmap();
	bm->GetBitmap(&bmpInfo);

	// Create an image from the bitmap in memory
	CImage image;
	image.Create(bmpInfo.bmWidth, bmpInfo.bmHeight, bmpInfo.bmBitsPixel);

	// Create a temporary DC
	CDC tempDC;
	tempDC.CreateCompatibleDC(&canvasDC);

	// Select the image into the DC
	HBITMAP hOldBitmap = (HBITMAP)tempDC.SelectObject(image);

	// Copy canvas area to the image
	tempDC.BitBlt(0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight,
		&canvasDC, 0, 0, SRCCOPY);

	CString strPath = lpszPathName;
	CString strExt = strPath.Right(5).MakeLower();

	// Save the image based on file extension
	HRESULT hr;
	if (strExt.Right(4) == ".png")
		hr = image.Save(lpszPathName, Gdiplus::ImageFormatPNG);
	else if (strExt.Right(4) == ".jpg" ||
		     strExt.Right(5) == ".jpeg")
		hr = image.Save(lpszPathName, Gdiplus::ImageFormatJPEG);
	else if (strExt.Right(4) == ".gif")
		hr = image.Save(lpszPathName, Gdiplus::ImageFormatGIF);
	else if (strExt.Right(4) == ".bmp")
		hr = image.Save(lpszPathName, Gdiplus::ImageFormatBMP);
	else  // TIFF
		hr = image.Save(lpszPathName, Gdiplus::ImageFormatTIFF);

	// Clean up
	tempDC.SelectObject(hOldBitmap);

	return SUCCEEDED(hr);
}


///////////////////////////////////////////////////////////////////////////////
//                               Opening file                                //
///////////////////////////////////////////////////////////////////////////////

void CDrawDoc::OnFileOpen() // ON COMMAND ID_FILE_OPEN
{
	CString strFilter =
		_T("PNG (*.png)|*.png|")
		_T("JPEG (*.jpg;*.jpeg)|*.jpg|")
		_T("BMP (*.bmp)|*.bmp|")
		_T("GIF (*.gif)|*.gif|")
		_T("TIFF (*.tiff)|*.tiff|")
		_T("All Picture Files|*.png;*.jpg;*.jpeg;*.bmp;*.gif;*.tiff|")
		_T("All Files (*.*)|*.*||");

	// Create file dialog for opening
	CFileDialog dlg(TRUE,	// TRUE = Open dialog
		_T(".png"),			// Default extension
		NULL,				// No default filename
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		strFilter,			// File type filters
		AfxGetMainWnd());	// Parent window

	// Set default filter
	dlg.m_ofn.nFilterIndex = 6;

	// Set dialog title
	dlg.m_ofn.lpstrTitle = _T("Open");  // myTODO: add to string table

	// Open the dialog and check if the user clicked OK
	if (dlg.DoModal() == IDOK)
	{
		CString lpszPathName = dlg.GetPathName();
		if (lpszPathName.IsEmpty())
			return;
		AfxGetApp()->OpenDocumentFile(lpszPathName);  // MFC TN022
	}
}

BOOL CDrawDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CString message;
	message.Append(lpszPathName);
	message.Append(_T(" cannot be opened. It is not an image file, or its format is not \
		supported."));  // myTODO: add to string table.
	
	// Load the image from the specified path
	if (FAILED(canvasImage.Load(lpszPathName)))
	{
		AfxMessageBox(message, MB_ICONERROR | MB_OK);
		return FALSE;
	}
	newImageLoaded = TRUE;
	
	// Update canvas size
	canvasSize = CSize(canvasImage.GetWidth(), canvasImage.GetHeight());
	
	return TRUE;
}
