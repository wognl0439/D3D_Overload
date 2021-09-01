// AnimationInspector.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AnimationInspector.h"
#include "afxdialogex.h"
#include "Tool.h"
#include "EditorArchive.h"
#include "DlgTabControl.h"

// CAnimationInspector 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAnimationInspector, CDialogEx)

CAnimationInspector::CAnimationInspector(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_ANIMATION_INSPECTOR, pParent),
	m_pBtnClose(NULL)
{

}

CAnimationInspector::~CAnimationInspector()
{
	if (m_pBtnClose)
		delete(m_pBtnClose);
}

void CAnimationInspector::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

CRect CAnimationInspector::GetDlgRC()
{
	return m_DlgRC;
}

BOOL CAnimationInspector::OnInitDialog()
{
	CDialogEx::OnInitDialog();
		
	GetWindowRect(&m_DlgRC);

	//GET_SINGLE(CEditorArchive)->SetAnimationInspector(this);
	//GET_SINGLE(CEditorArchive)->SetAnimationHandle(this);

	//GET_SINGLE(CEditorArchive)->CreateAnimationInspector();
	//GET_SINGLE(CEditorArchive)->GetFormAniInspector()->SetHWND(m_hWnd);
	//GET_SINGLE(CEditorArchive)->GetFormAniInspector()->SetDlgRC(m_DlgRC);
	//GET_SINGLE(CEditorArchive)->GetFormAniInspector()->OnInitialUpdate();
	//CScrollView::SetScrollSizes(MM_TEXT, m_DlgRC);
/*
	m_rComponentRect.top = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 100;
	m_rComponentRect.left = 100;
	m_rComponentRect.right = m_rComponentRect.left + 100;*/
	
	//CGameObject* pSelObj = GET_SINGLE(CEditorArchive)->GetSelectObject();
	//GET_SINGLE(CEditorArchive)->CreateButton(pSelObj, L"Close", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	//	m_rComponentRect, IDC_BUTTON_DIALGOUE_EXIT, this);



	return TRUE;
}

void CAnimationInspector::OnBnClickedButtonDlgExit()
{
	CDlgTabControl* pTabControl = GET_SINGLE(CEditorArchive)->GetDlgTabControl();
	pTabControl->OnClose();
}


BEGIN_MESSAGE_MAP(CAnimationInspector, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_DIALGOUE_EXIT, &CAnimationInspector::OnBnClickedButtonDlgExit)
END_MESSAGE_MAP()


// CAnimationInspector 메시지 처리기입니다.
