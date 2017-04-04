/**
* @file InvisibleCodeDoc.h 
* @brief CInvisibleCodeDoc ��Ľӿ�
*/


#pragma once
#include "..\3DCode\CyImage.h"

class CInvisibleCodeView;

/**
* @class CInvisibleCodeDoc
* @brief InvisibleCode�ĵ�
*/
class CInvisibleCodeDoc : public CDocument
{
protected: // �������л�����
	CInvisibleCodeDoc();
	DECLARE_DYNCREATE(CInvisibleCodeDoc)

// ����
public:
	/// ͼ�����
	CyImage m_Image;

	/// ��������
	float2 m_Centers[4];

	/// ��ȡͼ��ָ��
	CyImage* GetImage();

	/// ��ȡͼ���С
	CSize GetImageSize() const;

	/// ��ȡ���ͼ
	CInvisibleCodeView* GetView();

	/// ������ά��
	void Decode();

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CInvisibleCodeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};