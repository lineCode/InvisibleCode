
// InvisibleCodeDoc.cpp : CInvisibleCodeDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "InvisibleCode.h"
#endif

#include "InvisibleCodeDoc.h"
#include "InvisibleCodeView.h"
#include <propkey.h>
#include "DecodeFuncs.h"
#include "..\3DCode\CodeTransform.h"
#include "..\3DCode\AfxGlobal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CInvisibleCodeDoc

IMPLEMENT_DYNCREATE(CInvisibleCodeDoc, CDocument)

BEGIN_MESSAGE_MAP(CInvisibleCodeDoc, CDocument)
END_MESSAGE_MAP()


// CInvisibleCodeDoc ����/����

CInvisibleCodeDoc::CInvisibleCodeDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CInvisibleCodeDoc::~CInvisibleCodeDoc()
{
}

BOOL CInvisibleCodeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CInvisibleCodeDoc ���л�

void CInvisibleCodeDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CInvisibleCodeDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
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

// ������������֧��
void CInvisibleCodeDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:  strSearchContent = _T("point;rectangle;circle;ole object;")��
	SetSearchContent(strSearchContent);
}

void CInvisibleCodeDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CInvisibleCodeDoc ���

#ifdef _DEBUG
void CInvisibleCodeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CInvisibleCodeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CInvisibleCodeDoc ����


BOOL CInvisibleCodeDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (m_Image.IsNull())
		return CDocument::OnSaveDocument(lpszPathName);
	return 	SUCCEEDED(m_Image.Save(lpszPathName, Gdiplus::ImageFormatBMP));
}


BOOL CInvisibleCodeDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	HRESULT hr = m_Image.Load(lpszPathName);
	BOOL success = SUCCEEDED(hr);
	if (success)
	{
		CInvisibleCodeView *pView = GetView();
		pView->UpdateScrollSize();
	}
	return success;
}


/// ������ά��
void CInvisibleCodeDoc::Decode()
{
	if (m_Image.IsNull())
		return;

	BYTE* pHead = m_Image.GetHeadAddress();
	int nWidth = m_Image.GetWidth();
	int nHeight = m_Image.GetHeight();
	int nRowlen = m_Image.GetRowlen();
	int nChannel = m_Image.GetChannel();
	RoiRect roi = RoiRect(0, 0, 0, 0);
	COLORREF cPositionElem = RGB(255, 0, 0);
	COLORREF cColorElem = RGB(0, 255, 0);
	COLORREF cBackgroundElem = RGB(255, 255, 255);

	// �����ά��ͼ��
	int nVersion, nDataFormat, nDataLength, nEcLevel, nMaskingNo;
	char * result = DecodeImage(pHead, nWidth, nHeight, nRowlen, nChannel, roi, 
		cPositionElem, cColorElem, cBackgroundElem, 
		m_Centers, nVersion, nDataFormat, nDataLength, nEcLevel, nMaskingNo);
	if (result)
	{
		CString str = UTF8Convert2Unicode(result, nDataLength);
		AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
		SAFE_DELETE(result);
	}
}


/// ��ȡ�ĵ���Ӧ��CQrEncodeView���ͼ
CInvisibleCodeView* CInvisibleCodeDoc::GetView()
{
	CView* pView = NULL;
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CInvisibleCodeView)))
			return (CInvisibleCodeView*)pView;
		else
			continue;
	}
	if (pView->IsKindOf(RUNTIME_CLASS(CInvisibleCodeView)))
		return (CInvisibleCodeView*)pView;
	else
	{
		TRACE(" * ���ܶ�λ��ͼ��\n");
		return NULL;
	}
}


/// ����CyImage��ַ
CyImage* CInvisibleCodeDoc::GetImage()
{
	return &m_Image;
}


/// ����CSize = (nWidth, nHeight)
CSize CInvisibleCodeDoc::GetImageSize() const
{
	if (m_Image.IsNull())
		return CSize(0, 0);
	return CSize(m_Image.GetWidth(), m_Image.GetHeight());
}