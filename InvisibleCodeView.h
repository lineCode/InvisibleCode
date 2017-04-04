/**
* @file InvisibleCodeView.h 
* @brief CInvisibleCodeView ��Ľӿ�
*/

#pragma once

class CInvisibleCodeDoc;

/**
* @class CInvisibleCodeView
* @brief InvisibleCode��ͼ
*/
class CInvisibleCodeView : public CScrollView
{
protected: // �������л�����
	CInvisibleCodeView();
	DECLARE_DYNCREATE(CInvisibleCodeView)

// ����
public:
	CInvisibleCodeDoc* GetDocument() const;

	/// ���¹�����ͼ��С
	void UpdateScrollSize();

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CInvisibleCodeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDecode();
	afx_msg void OnUpdateDecode(CCmdUI *pCmdUI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // InvisibleCodeView.cpp �еĵ��԰汾
inline CInvisibleCodeDoc* CInvisibleCodeView::GetDocument() const
   { return reinterpret_cast<CInvisibleCodeDoc*>(m_pDocument); }
#endif

// ���������·�ת
void FlipPosition(float2 position[], int K, int nHeight);