// AnimationControllerInspector.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AnimationControllerInspector.h"
#include "afxdialogex.h"
#include "Tool.h"
#include "DlgTabControl.h"
#include "EditorArchive.h"

#include "AnimationState.h"
#include "Component/Animator.h"
#include "AnimationClip.h"
#include "EditorArchive.h"
#include "Component/MeshRenderer.h"
#include "AnimationStateController.h"

// CAnimationControllerInspector 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAnimationControllerInspector, CDialogEx)

CAnimationControllerInspector::CAnimationControllerInspector(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_ANIMATION_CONTROLLER_INSPECTOR, pParent),
	m_pBtnClose(NULL)
{

}

CAnimationControllerInspector::~CAnimationControllerInspector()
{
	if (m_pBtnClose)
		delete(m_pBtnClose);
}

void CAnimationControllerInspector::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ADD_TARGET_BUTTON, m_btnAddTarget);
	DDX_Control(pDX, IDC_INS_COMPARE_TYPE_COMBO, m_TypeComboBox);
}


BEGIN_MESSAGE_MAP(CAnimationControllerInspector, CDialogEx)
	ON_BN_CLICKED(IDC_ADD_TARGET_BUTTON, &CAnimationControllerInspector::OnBnClickedAddTargetButton)
	ON_EN_KILLFOCUS(IDC_INS_CLIP_SPEED_EDIT, &CAnimationControllerInspector::OnEnKillfocusInsClipSpeedEdit)
	ON_EN_KILLFOCUS(IDC_INS_COMPARE_VALUE_EDIT, &CAnimationControllerInspector::OnEnKillfocusInsCompareValueEdit)
	ON_CBN_SELCHANGE(IDC_INS_COMPARE_TYPE_COMBO, &CAnimationControllerInspector::OnCbnSelchangeInsCompareTypeCombo)
END_MESSAGE_MAP()


// CAnimationControllerInspector 메시지 처리기입니다.

// Add Target Button
void CAnimationControllerInspector::OnBnClickedAddTargetButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CGameObject*	pSelObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;
	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;

	GET_SINGLE(CEditorArchive)->CreateStatic(pSelObject, L"Target State Name", WS_CHILD | WS_VISIBLE | SS_LEFT, m_rComponentRect,
		IDC_STATIC_TARGET_STATE_NAME, this);
	
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 100;
	GET_SINGLE(CEditorArchive)->CreateEdit(pSelObject, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_TARGET_STATE_NAME, this);

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pSelObject, L"Change Value", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect, IDC_STATIC_CHANGE_VALUE, this);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 100;
	GET_SINGLE(CEditorArchive)->CreateEdit(pSelObject, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_CHANGE_VALUE, this);

	//CGameObject* pSelObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

	if (pSelObject)
	{
		CAnimator*	pAnimator = pSelObject->GetComponent<CAnimator>();
		if (pAnimator)
		{		
			CAnimationStateController*	pAniStateController = pAnimator->GetAnimationController();
			if (pAniStateController)
			{
				//pAniStateController->AddParameter("");
				CAnimationState* pAniState = pAniStateController->FindAnimationState("");
				if (pAniState)
				{
					//pAniState->AddTransition()
				}
			}
		}
		SAFE_RELEASE(pAnimator);
	}
}

CRect CAnimationControllerInspector::GetDlgRC()
{
	return m_DlgRC;
}

BOOL CAnimationControllerInspector::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetWindowRect(&m_DlgRC);

	m_rComponentRect.top = 100;
	m_rComponentRect.left = 10;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	m_rComponentRect.right = m_rComponentRect.left + 200;
	


	return TRUE;
}

void CAnimationControllerInspector::OnBnClickedButtonDlgExit()
{
	CDlgTabControl* pTabControl = GET_SINGLE(CEditorArchive)->GetDlgTabControl();
	pTabControl->OnClose();
}


// Clip Set Speed EditBox Kill Focus
void CAnimationControllerInspector::OnEnKillfocusInsClipSpeedEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CGameObject* pSelObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

	if (pSelObject)
	{
		CAnimator*	pAnimator = pSelObject->GetComponent<CAnimator>();
		if (pAnimator)
		{
			CAnimationStateController*	pAniStateController = pAnimator->GetAnimationController();
			if (pAniStateController)
			{
				CAnimationState* pAniState = pAniStateController->FindAnimationState("");
				if (pAniState)
				{
					CString strData;
					GetDlgItem(IDC_INS_CLIP_SPEED_EDIT)->GetWindowTextW(strData);
					float fAniSpeed = atof((CStringA)strData);
					pAniState->SetSpeed(fAniSpeed);
				}
			}
		}
		SAFE_RELEASE(pAnimator);
	}
}

// Compare Value EditBox Kill Focus
void CAnimationControllerInspector::OnEnKillfocusInsCompareValueEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CGameObject* pSelObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

	if (pSelObject)
	{
		CAnimator*	pAnimator = pSelObject->GetComponent<CAnimator>();
		if (pAnimator)
		{
			CAnimationStateController*	pAniStateController = pAnimator->GetAnimationController();
			if (pAniStateController)
			{
				CAnimationState* pAniState = pAniStateController->FindAnimationState("");
				if (pAniState)
				{
					CString strData;
					GetDlgItem(IDC_INS_CLIP_SPEED_EDIT)->GetWindowTextW(strData);
					float fAniSpeed = atof((CStringA)strData);
					CAnimationClip* pAniClip = pAniState->GetClip();
					
				}
			}
		}
		SAFE_RELEASE(pAnimator);
	}
}


// Compare Type ComboBox Sel Change
void CAnimationControllerInspector::OnCbnSelchangeInsCompareTypeCombo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int		iType = m_TypeComboBox.GetCurSel();
	switch (iType)
	{
		//bool
	case 0:
	{
		bool b = false;
	}	break;
	// int
	case 1:
	{	int a = 0;
	}	break;
	// float
	case 2:
	{	float f = 0;
	}	break;
	}
}
