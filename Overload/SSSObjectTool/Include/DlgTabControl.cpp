// DlgTabControl.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgTabControl.h"
#include "afxdialogex.h"

#include "Inspector.h"
#include "DlgAnimationClip.h"
#include "TransitionNode.h"
#include "EditorArchive.h"


// CDlgTabControl ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgTabControl, CDialogEx)

CDlgTabControl::CDlgTabControl(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_TAB, pParent),
	m_pAnimation(NULL),
	m_pTransition(NULL)
{
}

CDlgTabControl::~CDlgTabControl()
{
	SAFE_DELETE(m_pAnimation);
	SAFE_DELETE(m_pTransition);
}

void CDlgTabControl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CONTROL, m_TabCtr);
}

BOOL CDlgTabControl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	m_TabCtr.InsertItem(0, _T("Animation Clip"));
	m_TabCtr.InsertItem(1, _T("Transition Node"));
	m_TabCtr.SetCurSel(0);

	//TCITEM*	pTCItem = NULL;
	//int pHandle = m_TabCtr.GetCurFocus();
	//m_TabCtr.GetItem()

	CRect	rt;
	m_TabCtr.GetWindowRect(&rt);
	
	if (!m_pAnimation)
	{
		m_pAnimation = new CDlgAnimationClip;
		m_pAnimation->Create(IDD_DIALOG_ANIMATION, &m_TabCtr);
		m_pAnimation->MoveWindow(5, 25, rt.Width() - 15, rt.Height() - 30);
		m_pAnimation->ShowWindow(SW_SHOW);
		m_pAnimation->UpdateContainedAnimationClipInfo();
		m_pAnimation->UpdateParameterList();
		GET_SINGLE(CEditorArchive)->SetDlgAnimationClip(m_pAnimation);
	}

	if (!m_pTransition)
	{
		m_pTransition = new CTransitionNode;
		m_pTransition->Create(IDD_DIALOG_TRANSITION, &m_TabCtr);
		m_pTransition->MoveWindow(5, 25, rt.Width() - 15, rt.Height() - 30);
		m_pTransition->ShowWindow(SW_HIDE);

		GET_SINGLE(CEditorArchive)->SetTransitionNode(m_pTransition);
	}
	

	return TRUE;
}

void CDlgTabControl::SetWndTabCtr(CWnd * pWnd)
{
}


BEGIN_MESSAGE_MAP(CDlgTabControl, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONTROL, &CDlgTabControl::OnTcnSelchangeTabControl)
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_WM_ENABLE()
//	ON_WM_ACTIVATEAPP()
ON_WM_DESTROY()
END_MESSAGE_MAP()


void CDlgTabControl::OnTcnSelchangeTabControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	int iTabIndex = m_TabCtr.GetCurSel();

	switch (iTabIndex)
	{
	case 0:
		m_pAnimation->ShowWindow(SW_SHOW);
		m_pTransition->ShowWindow(SW_HIDE);
		//m_pAnimationCtrInspector->ShowWindow(SW_SHOW);
		m_pTransition->ClearInfoPanel();
		break;

	case 1:
		m_pAnimation->ShowWindow(SW_HIDE);
		m_pTransition->ShowWindow(SW_SHOW); 
		
		m_pTransition->UpdateContainedItems();

		if (m_pTransition->m_pSelectedState)
		{
			if (m_pTransition->m_iSelectedStateCursel != -1)
			{
				m_pTransition->m_clStateList.SetCurSel(m_pTransition->m_iSelectedStateCursel);
			}
		}

		if (m_pTransition->m_pSelectedState)
		{
			m_pTransition->HideInfoPanel(false);
			m_pTransition->SelectState(m_pTransition->m_pSelectedState);
			m_pTransition->SetCurSel(m_pTransition->m_pSelectedState);
		}
		else
		{
			m_pTransition->HideInfoPanel(true);
		}

		break;
	}
}


void CDlgTabControl::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CDialogEx::OnClose();
	OnCancel();
	GET_SINGLE(CEditorArchive)->SetAnimationDlgState(false);
	SAFE_DELETE(m_pAnimation);
	SAFE_DELETE(m_pTransition);
	//SAFE_DELETE(m_pAnimationCtrInspector);

	GET_SINGLE(CEditorArchive)->DeleteAnimationDlg();

}

// TabControlDlg Ȱ��ȭ
void CDlgTabControl::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (nState == 1 || nState == 2)
	{
		GET_SINGLE(CEditorArchive)->SetAnimationDlgState(true);
	}
	if (nState == 0)
	{
		GET_SINGLE(CEditorArchive)->SetAnimationDlgState(false);
	}
}


void CDlgTabControl::OnEnable(BOOL bEnable)
{
	CDialogEx::OnEnable(bEnable);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	int a = 0;
}


//void CDlgTabControl::OnActivateApp(BOOL bActive, DWORD dwThreadID)
//{
//	CDialogEx::OnActivateApp(bActive, dwThreadID);
//
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
//	int a = 0;
//}


void CDlgTabControl::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	OnCancel();
	GET_SINGLE(CEditorArchive)->SetAnimationDlgState(false);
	SAFE_DELETE(m_pAnimation);
	SAFE_DELETE(m_pTransition);
	//SAFE_DELETE(m_pAnimationCtrInspector);

	GET_SINGLE(CEditorArchive)->DeleteAnimationDlg();
}


BOOL CDlgTabControl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
			return TRUE;
		else if (pMsg->wParam == VK_RETURN)
			return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
