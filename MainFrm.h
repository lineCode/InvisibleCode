/**
* @file MainFrm.h 
* @brief CMainFrame ��Ľӿ�
*/

#pragma once
#include "..\3DCode\3DCodePane.h"

class CDlgEncode;

/// ����Ի���������ͼ��ID
#define ID_VIEW_ENCODE               10000

/// ����Ի���������ͼ��ID
#define ID_VIEW_DECODE               10001

/**
* @class CMainFrame
* @brief �����
*/
class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CMFCToolBar			m_wndToolBar;		/**< ������ */
	CMFCStatusBar		m_wndStatusBar;		/**< ״̬�� */
	C3DCodePane			m_paneEncode;		/**< ������� */

	/// ����Ի���
	CDlgEncode*			m_pDlgEncode;

	// ����PANE
	BOOL CreateDockingWindows();

public:
	/// ��ȡ����Ի���
	CDlgEncode* GetEncodeDlg(){ return m_pDlgEncode; }

	/// ��ȡ״̬������
	CMFCStatusBar& GetStatusBar() { return m_wndStatusBar; }

	// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEncode();
	afx_msg void OnUpdateEncode(CCmdUI *pCmdUI);
};