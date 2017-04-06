
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "InvisibleCode.h"

#include "MainFrm.h"
#include "DlgEncode.h"
#include <afxmdiframewndex.h>
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_ENCODE, &CMainFrame::OnEncode)
	ON_UPDATE_COMMAND_UI(ID_ENCODE, &CMainFrame::OnUpdateEncode)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	m_pDlgEncode = NULL;
}

CMainFrame::~CMainFrame()
{
	SAFE_DELETE(m_pDlgEncode);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: �������Ҫ��ͣ������������ɾ��������
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndToolBar);

	// ����ͣ������
	if (!CreateDockingWindows())
	{
		TRACE0("δ�ܴ���ͣ������\n");
		return -1;
	}

	m_paneEncode.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_paneEncode);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return TRUE;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������

BOOL CMainFrame::CreateDockingWindows()
{
	CRect ChildRect;

	// ��������Ի���
	m_pDlgEncode = new CDlgEncode;
	m_pDlgEncode->Create(IDD_ENCODE, this);
	m_pDlgEncode->GetWindowRect(&ChildRect);
	// ʹ�öԻ����㹻���Ը�������View
	m_pDlgEncode->MoveWindow(0, 0, 1920, 1080);
	m_pDlgEncode->ShowWindow(SW_SHOW);

	// �����������
	if (!m_paneEncode.Create(_T("����"), this, CRect(0, 0, ChildRect.Width(), ChildRect.Height()), TRUE, ID_VIEW_ENCODE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("δ�ܴ������봰��\n");
		return FALSE; // δ�ܴ���
	}
	m_paneEncode.m_pEncodeView->AddChildWnd(m_pDlgEncode, ChildRect.Width(), ChildRect.Height());
	// 2015��12��11��ע�ͣ������롱�Ի����������Ϊ��Child����������strcore.cpp���ִ���

	return TRUE;
}


void CMainFrame::OnEncode()
{
	if (m_paneEncode.IsWindowVisible())
	{
		m_paneEncode.ShowPane(FALSE, FALSE, FALSE);
	}
	else
	{
		m_paneEncode.ShowPane(TRUE, FALSE, TRUE);
	}
}


void CMainFrame::OnUpdateEncode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_paneEncode.IsPaneVisible());
}
