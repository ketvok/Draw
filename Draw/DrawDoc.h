// DrawDoc.h : interface of the CDrawDoc class
//

#pragma once
#include <memory>
#include <vector>

class CDrawDoc : public CDocument
{
// Private data members
	CBitmap canvasBitmap;
	CDC canvasDC;
	CSize canvasSize;
	CImage canvasImage;
	BOOL newImageLoaded;

protected: // create from serialization only
	CDrawDoc() noexcept;
	DECLARE_DYNCREATE(CDrawDoc)

// Attributes
public:	
	CBitmap& GetCanvasBitmap() { return canvasBitmap; }
	CDC* GetCanvasDC() { return &canvasDC; }
	const CSize& GetCanvasSize() const { return canvasSize; }
	void SetCanvasSize(const CSize& size) { canvasSize = size; }
	const CImage& GetCanvasImage() { return canvasImage; }
	BOOL GetNewImageLoaded() { return newImageLoaded; }
	void SetNewImageLoaded(BOOL value) { newImageLoaded = value; }

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CDrawDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:


// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual void DeleteContents();
	/// <summary>
	/// Undocumented CDocument implementation helper.</summary>
	/// <param name="lpszPathName">Full path of the file to save. If this is NULL the default
	/// implementation will prompt the user for a filename and path using the File-Save-As common
	/// dialog.</param>
	/// <param name="bReplace">If TRUE it will replace an existing file, if FALSE it won't.</param>
	/// <returns>
	/// TRUE if the document was saved successfully; otherwise FALSE.</returns>
	virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace = TRUE);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	/// <summary>
	/// TN022: The recommended way to customize this is to replace the default implementation with
	/// your own FileOpen dialog, and call CWinApp::OpenDocumentFile with the document's file or
	/// path name.</summary>
	afx_msg void OnFileOpen();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};
