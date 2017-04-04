
// InvisibleCodeView.cpp : CInvisibleCodeView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "InvisibleCode.h"
#endif

#include "InvisibleCodeDoc.h"
#include "InvisibleCodeView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CInvisibleCodeView

IMPLEMENT_DYNCREATE(CInvisibleCodeView, CScrollView)

BEGIN_MESSAGE_MAP(CInvisibleCodeView, CScrollView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_COMMAND(ID_DECODE, &CInvisibleCodeView::OnDecode)
	ON_UPDATE_COMMAND_UI(ID_DECODE, &CInvisibleCodeView::OnUpdateDecode)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/// ���������·�ת
void FlipPosition(float2 position[], int K, int nHeight)
{
	for (int i = 0; i < K; ++i)
	{
		position[i].y = nHeight - position[i].y;
	}
}


// CInvisibleCodeView ����/����

CInvisibleCodeView::CInvisibleCodeView()
{
	// TODO: �ڴ˴���ӹ������

}

CInvisibleCodeView::~CInvisibleCodeView()
{
}

BOOL CInvisibleCodeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CScrollView::PreCreateWindow(cs);
}

// CInvisibleCodeView ����

void CInvisibleCodeView::OnDraw(CDC* pDC)
{
	CInvisibleCodeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (!pDoc->m_Image.IsNull())
	{
		int nWidth = pDoc->m_Image.GetWidth();
		int nHeight = pDoc->m_Image.GetHeight();
		pDoc->m_Image.Draw(pDC->GetSafeHdc(), 0, 0, nWidth, nHeight);
		float2 m_Centers[4];
		memcpy(m_Centers, pDoc->m_Centers, 4 * sizeof(float2));
		FlipPosition(m_Centers, 4, nHeight);
		pDC->Ellipse((int)m_Centers[0].x - 4, (int)m_Centers[0].y - 4, (int)m_Centers[0].x + 4, (int)m_Centers[0].y + 4);
		pDC->Ellipse((int)m_Centers[1].x - 4, (int)m_Centers[1].y - 4, (int)m_Centers[1].x + 4, (int)m_Centers[1].y + 4);
		pDC->Ellipse((int)m_Centers[2].x - 4, (int)m_Centers[2].y - 4, (int)m_Centers[2].x + 4, (int)m_Centers[2].y + 4);
		pDC->Ellipse((int)m_Centers[3].x - 4, (int)m_Centers[3].y - 4, (int)m_Centers[3].x + 4, (int)m_Centers[3].y + 4);
	}
}

void CInvisibleCodeView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: �������ͼ�ĺϼƴ�С
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


// CInvisibleCodeView ��ӡ

BOOL CInvisibleCodeView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CInvisibleCodeView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CInvisibleCodeView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}


// CInvisibleCodeView ���

#ifdef _DEBUG
void CInvisibleCodeView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CInvisibleCodeView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CInvisibleCodeDoc* CInvisibleCodeView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CInvisibleCodeDoc)));
	return (CInvisibleCodeDoc*)m_pDocument;
}
#endif //_DEBUG


// CInvisibleCodeView ��Ϣ�������


// "����"������
void CInvisibleCodeView::OnDecode()
{
	CInvisibleCodeDoc *pDoc = GetDocument();
	pDoc->Decode();
	Invalidate(TRUE);
}


// ���¹�����ͼ��״̬
void CInvisibleCodeView::OnUpdateDecode(CCmdUI *pCmdUI)
{
	CInvisibleCodeDoc *pDoc = GetDocument();
	CyImage *pImage = pDoc->GetImage();
	pCmdUI->Enable(!pImage->IsNull());
}


// ���¹�����ͼ��С
void CInvisibleCodeView::UpdateScrollSize()
{
	CInvisibleCodeDoc *pDoc = GetDocument();
	SetScrollSizes(MM_TEXT, pDoc->GetImageSize());
}


// ��ʾ�����������ص�RGBֵ
void CInvisibleCodeView::OnMouseMove(UINT nFlags, CPoint point)
{
	CString str;
	CDC* pDC = GetDC();
	COLORREF ref = GetPixel(pDC->GetSafeHdc(), point.x, point.y);
	int R = GetRValue(ref), G = GetGValue(ref), B = GetBValue(ref);
	float Gray = RgbColorRef2Gray(ref);
	str.Format(_T("x = %4d, y = %4d, g = %.2f, RGB(%3d, %3d, %3d)"), 
		point.x, point.y, Gray, R, G, B);
	CMainFrame* pMainFrm = (CMainFrame*) AfxGetApp()->GetMainWnd();
	pMainFrm->GetStatusBar().SetWindowText(str);

	CScrollView::OnMouseMove(nFlags, point);
}
