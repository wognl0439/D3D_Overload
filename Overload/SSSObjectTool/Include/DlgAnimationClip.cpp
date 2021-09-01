// DlgAnimationClip.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgAnimationClip.h"
#include "afxdialogex.h"
#include "Tool.h"

#include "AnimationState.h"
#include "Component/Animator.h"
#include "AnimationClip.h"
#include "EditorArchive.h"
#include "Component/MeshRenderer.h"
#include "AnimationStateController.h"
#include "Mesh.h"
#include "DataManager.h"

#include "TransitionNode.h"
#include "ResourcesManager.h"

// CDlgAnimationClip 대화 상자입니다.
IMPLEMENT_DYNAMIC(CDlgAnimationClip, CDialogEx)

CDlgAnimationClip::CDlgAnimationClip(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_ANIMATIONCLIP, pParent)
	, m_strParameterName(_T(""))
	, m_iParameterType(0)
	, m_fParameterValue(0)
	, m_iParameterValueBool(0)
	, m_bOverridePSA(FALSE)
	, m_iClipStartFrame(0)
	, m_iClipEndFrame(0)
	, m_strClipName(_T(""))
{

}

CDlgAnimationClip::~CDlgAnimationClip()
{
}

void CDlgAnimationClip::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESH_CLIP_LISTBOX, m_clMeshClipList);
	DDX_Control(pDX, IDC_LIST_CONTROLLER_PARAMETER, m_clControllerParameterList);
	DDX_Text(pDX, IDC_EDIT_CONTROLLER_PARAMETER_NAME, m_strParameterName);
	DDX_CBIndex(pDX, IDC_COMBO_CONTROLLER_PARAMETER_TYPE, m_iParameterType);
	DDV_MinMaxInt(pDX, m_iParameterType, 0, 3);
	DDX_Control(pDX, IDC_EDIT_CONTROLLER_PARAMETER_VALUE, m_ceParameterValue);
	DDX_Text(pDX, IDC_EDIT_CONTROLLER_PARAMETER_VALUE, m_fParameterValue);
	DDX_Control(pDX, IDC_COMBO_CONTROLLER_PARAMETER_VALUE_BOOL, m_ccParameterBool);
	DDX_CBIndex(pDX, IDC_COMBO_CONTROLLER_PARAMETER_VALUE_BOOL, m_iParameterValueBool);
	DDV_MinMaxInt(pDX, m_iParameterValueBool, 0, 1);
	DDX_Control(pDX, IDC_COMBO_CONTROLLER_PARAMETER_TYPE, m_ccParameterType);
	DDX_Check(pDX, IDC_CHECK_OVERRIDE_PSA, m_bOverridePSA);
	DDX_Text(pDX, IDC_EDIT_START_FRAME, m_iClipStartFrame);
	DDX_Text(pDX, IDC_EDIT_END_FRAME, m_iClipEndFrame);
	DDX_Text(pDX, IDC_EDIT_CLIPNAME, m_strClipName);
}


BEGIN_MESSAGE_MAP(CDlgAnimationClip, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_CLIP_EDIT_BUTTON, &CDlgAnimationClip::OnBnClickedClipEditButton)
	//ON_BN_CLICKED(IDC_CLIP_MODIFIED_BUTTON2, &CDlgAnimationClip::OnBnClickedClipModifiedButton2)
	ON_EN_KILLFOCUS(IDC_EDIT_ANIMATIONSTATE_NAME, &CDlgAnimationClip::OnEnKillfocusEditAnimationstateName)
	ON_EN_KILLFOCUS(IDC_EDIT_CLIPNAME, &CDlgAnimationClip::OnEnKillfocusEditClipname)
	ON_EN_KILLFOCUS(IDC_EDIT_START_FRAME, &CDlgAnimationClip::OnEnKillfocusEditStartFrame)
	ON_EN_KILLFOCUS(IDC_EDIT_END_FRAME, &CDlgAnimationClip::OnEnKillfocusEditEndFrame)
	ON_LBN_SELCHANGE(IDC_MESH_CLIP_LISTBOX, &CDlgAnimationClip::OnLbnSelchangeMeshClipList)
	//ON_LBN_SELCHANGE(IDC_EDIT_CLIP_LIST, &CDlgAnimationClip::OnLbnSelchangeEditClipList)
	ON_EN_KILLFOCUS(IDC_EDIT_ENTRYSTATE_NAME, &CDlgAnimationClip::OnEnKillfocusEditEntrystateName)
	ON_BN_CLICKED(IDC_BUTTON_CONTROLLER_PARAMETER_CREATE, &CDlgAnimationClip::OnBnClickedButtonControllerParameterCreate)
//	ON_EN_CHANGE(IDC_EDIT_CONTROLLER_PARAMETER_NAME, &CDlgAnimationClip::OnEnChangeEditControllerParameterName)
ON_EN_KILLFOCUS(IDC_EDIT_CONTROLLER_PARAMETER_NAME, &CDlgAnimationClip::OnEnKillfocusEditControllerParameterName)
ON_CBN_SELCHANGE(IDC_COMBO_CONTROLLER_PARAMETER_TYPE, &CDlgAnimationClip::OnCbnSelchangeComboControllerParameterType)
ON_EN_CHANGE(IDC_EDIT_CONTROLLER_PARAMETER_VALUE, &CDlgAnimationClip::OnEnChangeEditControllerParameterValue)
ON_CBN_SELCHANGE(IDC_COMBO_CONTROLLER_PARAMETER_VALUE_BOOL, &CDlgAnimationClip::OnCbnSelchangeComboControllerParameterValueBool)
ON_LBN_SELCHANGE(IDC_LIST_CONTROLLER_PARAMETER, &CDlgAnimationClip::OnLbnSelchangeListControllerParameter)
ON_WM_LBUTTONDOWN()
ON_BN_CLICKED(IDC_BUTTON_CONRTOLLER_PARAMETER_REMOVE, &CDlgAnimationClip::OnBnClickedButtonConrtollerParameterRemove)
ON_BN_CLICKED(IDC_BUTTON_LOAD_PSA, &CDlgAnimationClip::OnBnClickedButtonLoadPsa)
ON_BN_CLICKED(IDC_CHECK_OVERRIDE_PSA, &CDlgAnimationClip::OnBnClickedCheck1)
ON_EN_CHANGE(IDC_EDIT_CLIPNAME, &CDlgAnimationClip::OnEnChangeEditClipname)
ON_EN_CHANGE(IDC_EDIT_START_FRAME, &CDlgAnimationClip::OnEnChangeEditClipname)
ON_EN_CHANGE(IDC_EDIT_END_FRAME, &CDlgAnimationClip::OnEnChangeEditClipname)
ON_WM_KEYDOWN()

ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER1, &CDlgAnimationClip::OnTRBNThumbPosChangingSlider1)
END_MESSAGE_MAP()


// CDlgAnimationClip 메시지 처리기입니다.

void CDlgAnimationClip::UpdateContainedAnimationClipInfo(int iCursel)
{
	int iScrollPosition = m_clMeshClipList.GetScrollPos(SB_VERT);	
	m_clMeshClipList.ResetContent(); 

	
	CGameObject* pSelectedObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
	if (pSelectedObject)
	{	
		CMeshRenderer* pRenderer = pSelectedObject->GetComponent<CMeshRenderer>();

		if (pRenderer)
		{
			CMesh* pMesh = pRenderer->GetMesh();

			if (pMesh)
			{
				vector<PANIMATIONCLIP>* vecAnimationClipInfo = pMesh->GetAnimationClipInfo();

				size_t iClipCount = vecAnimationClipInfo->size();
				for (size_t i = 0; i < iClipCount; ++i)
				{
					wstring strDescription = L"[";
					strDescription += to_wstring(vecAnimationClipInfo->at(i)->iStartFrame);
					strDescription += L" ~ ";
					strDescription += to_wstring(vecAnimationClipInfo->at(i)->iEndFrame);
					strDescription += L"]  ";
					strDescription += CA2CT(vecAnimationClipInfo->at(i)->strName.c_str());
					m_clMeshClipList.AddString(strDescription.c_str());
				}
			}
		}
		SAFE_RELEASE(pRenderer);
	}
	UpdateData(FALSE);
	m_clMeshClipList.SetCurSel(iCursel);
	m_clMeshClipList.SetTopIndex(iScrollPosition);
}

BOOL CDlgAnimationClip::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	return TRUE;
}

string CDlgAnimationClip::GetStateName()
{
	return m_strStateName;
}

string CDlgAnimationClip::GetClipName()
{	
	return string(CT2CA(m_strClipName));
}

string CDlgAnimationClip::GetEntryStateName()
{
	return m_strEntryStateName;
}

float CDlgAnimationClip::GetStartFrame()
{
	return m_fStartFrame;
}

float CDlgAnimationClip::GetEndFrame()
{
	return m_fEndFrame;
}

void CDlgAnimationClip::ClearParameterControls()
{
	UpdateData(TRUE);
	
	m_strParameterName = L"";
	m_fParameterValue = 0;
	m_iParameterType = 0;
	m_iParameterValueBool = 0;

	m_ccParameterBool.SetCurSel(-1);
	m_ccParameterType.SetCurSel(-1);
	m_ccParameterBool.ShowWindow(SW_SHOW);
	m_ceParameterValue.ShowWindow(SW_HIDE);
	
	UpdateData(FALSE);
}

void CDlgAnimationClip::UpdateParameterList()
{
	m_clControllerParameterList.ResetContent();


	CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

	if (pGameObject)
	{
		CAnimator* pAnimator = pGameObject->GetComponent<CAnimator>();

		if (pAnimator)
		{
			CAnimationStateController* pController = pAnimator->GetAnimationController();
			if (pController)
			{
				unordered_map<string, PPARAMETER> mapParam = pController->GetParameter();

				unordered_map<string, PPARAMETER>::iterator iter;
				unordered_map<string, PPARAMETER>::iterator iterEnd = mapParam.end();
				
				for (iter = mapParam.begin(); iter != iterEnd; ++iter)
				{
					m_clControllerParameterList.AddString(CA2CT(iter->first.c_str()));
				}
			}
		}
		SAFE_RELEASE(pAnimator);
	}

	UpdateData(FALSE);
}

BOOL CDlgAnimationClip::OnEraseBkgnd(CDC* pDC)
{
	CRect	rc;
	GetClientRect(rc);
	pDC->FillSolidRect(rc, RGB(255, 255, 255));

	return TRUE;
}


// 클립 편집 버튼
void CDlgAnimationClip::OnBnClickedClipEditButton()
{
	// 스테이트 이름, 클립 이름, 시작 프레임, 끝 프레임 정보를 읽어와서
	// 현재 오브젝트의 애니메이터에 만들어 넣는다.
	CGameObject* pSelObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
	if (pSelObject)
	{
		CAnimator*	pAnimator = pSelObject->GetComponent<CAnimator>();		
		CMesh* pMesh = pAnimator->GetMeshRenderer()->GetMesh();
		if (pAnimator)
		{
			int iStartFrame = m_fStartFrame;
			int iEndFrame = m_fEndFrame;

			pMesh->AddAnimationClip(string(CT2CA(m_strClipName)), iStartFrame, iEndFrame);

			SAFE_RELEASE(pAnimator);
		}
	}

	UpdateContainedAnimationClipInfo();
}

// 애니메이션 스테이트 이름 킬 포커스
void CDlgAnimationClip::OnEnKillfocusEditAnimationstateName()
{
	CString	StateName;
	GetDlgItem(IDC_EDIT_ANIMATIONSTATE_NAME)->GetWindowTextW(StateName);
	m_strStateName = CT2CA(StateName);
}

// 클립 이름 킬 포커스
void CDlgAnimationClip::OnEnKillfocusEditClipname()
{
	CString	ClipName;
	GetDlgItem(IDC_EDIT_CLIPNAME)->GetWindowTextW(ClipName);
	m_strClipName = CT2CA(ClipName);
}


// 시작 프레임 킬 포커스
void CDlgAnimationClip::OnEnKillfocusEditStartFrame()
{
	CString	StartFrame;
	GetDlgItem(IDC_EDIT_START_FRAME)->GetWindowTextW(StartFrame);
	m_fStartFrame = atof((CStringA)StartFrame);
	
}


// 끝 프레임 킬 포커스
void CDlgAnimationClip::OnEnKillfocusEditEndFrame()
{
	CString	EndFrame;
	GetDlgItem(IDC_EDIT_END_FRAME)->GetWindowTextW(EndFrame);
	m_fEndFrame = atof((CStringA)EndFrame);
}

// 메쉬 클립 셀 체인지
void CDlgAnimationClip::OnLbnSelchangeMeshClipList()
{
 	UpdateData(TRUE);

	int iCursel = m_clMeshClipList.GetCurSel();

	if (iCursel >= 0)
	{
		CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
		if (pGameObject)
		{
			CMeshRenderer* pRenderer = pGameObject->GetComponent<CMeshRenderer>();
			if (pRenderer)
			{
				CMesh* pMesh = pRenderer->GetMesh();
				if (pMesh)
				{
					vector<PANIMATIONCLIP>* vecClip = pMesh->GetAnimationClipInfo();
					m_strClipName = vecClip->at(iCursel)->strName.c_str();
					m_iClipStartFrame = vecClip->at(iCursel)->iStartFrame;
					m_iClipEndFrame = vecClip->at(iCursel)->iEndFrame;					
				}
			}
			SAFE_RELEASE(pRenderer);
		}
	}
	UpdateData(FALSE);
}

//// 편집 클립 셀 체인지
//void CDlgAnimationClip::OnLbnSelchangeEditClipList()
//{
//	CGameObject*	pSelObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
//	if (pSelObject)
//	{
//		CAnimator*	pAnimator = pSelObject->GetComponent<CAnimator>();
//		if (pAnimator)
//		{
//			CAnimationState* pState = pAnimator->GetAnimationController()->CreateAnimationState(m_strStateName);			
//			pState->SetClip(m_strClipName);			
//			pAnimator->GetAnimationController()->SetEntryState(pState->GetStateName());
//			SAFE_RELEASE(pAnimator);
//		}
//	}
//}

// Entry State Name Kill Focus
void CDlgAnimationClip::OnEnKillfocusEditEntrystateName()
{
	CString	EntryStateName;
	GetDlgItem(IDC_EDIT_ENTRYSTATE_NAME)->GetWindowTextW(EntryStateName);
	m_strEntryStateName = CT2CA(EntryStateName);

	CGameObject* pSelObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

	if (pSelObject)
	{
		CAnimator*	pAnimator = pSelObject->GetComponent<CAnimator>();
		if (pAnimator)
		{
			SAFE_RELEASE(pAnimator);
		}
	}
}


//void CDlgAnimationClip::OnEnChangeEditControllerParameterName()
//{
//	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
//	// CDialogEx::OnInitDialog() 함수를 재지정 
//	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
//	// 이 알림 메시지를 보내지 않습니다.
//
//	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
//}


void CDlgAnimationClip::OnEnKillfocusEditControllerParameterName()
{
}


void CDlgAnimationClip::OnCbnSelchangeComboControllerParameterType()
{
	UpdateData(TRUE);

	//bool
	if (m_iParameterType == 0)
	{
		m_ccParameterBool.ShowWindow(SW_SHOW);
		m_ceParameterValue.ShowWindow(SW_HIDE);
	}
	else if (m_iParameterType == 3)
	{
		//Trigger
		m_ccParameterBool.ShowWindow(SW_HIDE);
		m_ceParameterValue.ShowWindow(SW_HIDE);		
	}
	else
	{
		m_ccParameterBool.ShowWindow(SW_HIDE);
		m_ceParameterValue.ShowWindow(SW_SHOW);
	}

	UpdateData(FALSE);
}

//int & float Value Update
void CDlgAnimationClip::OnEnChangeEditControllerParameterValue()
{
	UpdateData(TRUE);
	int iCursel = m_clControllerParameterList.GetCurSel();

	if (iCursel >= 0)
	{
		CString strParameterName;
		m_clControllerParameterList.GetText(iCursel, strParameterName);

		CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

		if (pGameObject)
		{
			CAnimator* pAnimator = pGameObject->GetComponent<CAnimator>();

			if (pAnimator)
			{
				CAnimationStateController* pController = pAnimator->GetAnimationController();
				if (pController)
				{
					string strParameter = CT2CA(strParameterName);
					PPARAMETER pParam = pController->FindParameter(strParameter);
					
					if (pParam->eType == PT_INT)
					{
						pParam->iValue = (int)m_fParameterValue;
					}
					else if (pParam->eType == PT_FLOAT)
					{
						pParam->fValue = m_fParameterValue;
					}
				}
			}
			SAFE_RELEASE(pAnimator);
		}
	}
	UpdateData(FALSE);
}

//bool Value Update
void CDlgAnimationClip::OnCbnSelchangeComboControllerParameterValueBool()
{
	UpdateData(TRUE);
	int iCursel = m_clControllerParameterList.GetCurSel();

	if (iCursel >= 0)
	{
		CString strParameterName;
		m_clControllerParameterList.GetText(iCursel, strParameterName);

		CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

		if (pGameObject)
		{
			CAnimator* pAnimator = pGameObject->GetComponent<CAnimator>();

			if (pAnimator)
			{
				CAnimationStateController* pController = pAnimator->GetAnimationController();
				if (pController)
				{
					string strParameter = CT2CA(strParameterName);
					PPARAMETER pParam = pController->FindParameter(strParameter);

					if (pParam->eType == PT_BOOL)
					{
						pParam->bValue = (bool)m_iParameterValueBool;
					}
				}
			}
			SAFE_RELEASE(pAnimator);
		}
	}
	UpdateData(FALSE);
}

//Create Parameter
void CDlgAnimationClip::OnBnClickedButtonControllerParameterCreate()
{
	UpdateData(TRUE);
	CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

	if (pGameObject)
	{
		CAnimator* pAnimator = pGameObject->GetComponent<CAnimator>();

		if (pAnimator)
		{
			CAnimationStateController* pController = pAnimator->GetAnimationController();
			if (pController)
			{
				string strName = CT2CA(m_strParameterName);
				PARAMETER_TYPE eType;
				if (m_iParameterType == 0)
				{
					eType = PT_BOOL;
				}
				else if (m_iParameterType == 1)
				{
					eType = PT_INT;
				}
				else if (m_iParameterType == 2)
				{
					eType = PT_FLOAT;
				}
				else if (m_iParameterType == 3)
				{
					eType = PT_TRIGGER;
				}
				else
				{
					MessageBox(L"Invalid Type", L"Exception", MB_OK);
					SAFE_RELEASE(pAnimator);
					return;
				}

				PPARAMETER pParam = pController->AddParameter(strName, eType);
				
				switch (eType)
				{
				case SSS::PT_BOOL:
					pParam->bValue = (bool)m_iParameterValueBool;
					break;
				case SSS::PT_INT:
					pParam->iValue = (int)m_fParameterValue;
					break;
				case SSS::PT_FLOAT:
					pParam->fValue = m_fParameterValue;
					break;
				case SSS::PT_TRIGGER:
					pParam->bValue = false;
					break;
				default:
					break;
				}
			}
		}
		SAFE_RELEASE(pAnimator);
	}
	UpdateParameterList(); 
	ClearParameterControls();
	UpdateData(FALSE);
}

//Click Parameter List Event
void CDlgAnimationClip::OnLbnSelchangeListControllerParameter()
{
	UpdateData(TRUE);

	int iCursel = m_clControllerParameterList.GetCurSel();
	CString strParameterName;

	if (iCursel >= 0)
	{
		m_clControllerParameterList.GetText(iCursel, strParameterName);

		CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
		if (pGameObject)
		{
			CAnimator* pAnimator = pGameObject->GetComponent<CAnimator>();

			if (pAnimator)
			{
				CAnimationStateController* pController = pAnimator->GetAnimationController();
				if (pController)
				{
					unordered_map<string, PPARAMETER> mapParam = pController->GetParameter();

					string strParam = CT2CA(strParameterName);
					unordered_map<string, PPARAMETER>::iterator iter = mapParam.find(strParam);
					if (iter != mapParam.end())
					{
						m_strParameterName = iter->second->strKey.c_str();
						PARAMETER_TYPE eType = iter->second->eType;
					
						switch (eType)
						{
						case SSS::PT_BOOL:
						{
							m_iParameterType = 0;
							m_ccParameterType.SetCurSel(m_iParameterType);
							m_ccParameterBool.ShowWindow(SW_SHOW);
							m_ceParameterValue.ShowWindow(SW_HIDE);			
							if (!iter->second->bValue)
							{
								m_iParameterValueBool = 0;
								//m_ccParameterBool.SetCurSel(0);
							}
							else
							{
								m_iParameterValueBool = 1;
								//m_ccParameterBool.SetCurSel(1);
							}
							
						}
							break;
						case SSS::PT_INT:
						{
							m_iParameterType = 1;
							m_ccParameterType.SetCurSel(m_iParameterType);
							m_ccParameterBool.ShowWindow(SW_HIDE);
							m_ceParameterValue.ShowWindow(SW_SHOW);

							m_fParameterValue = (float)iter->second->iValue;
						}
							break;
						case SSS::PT_FLOAT:
						{
							m_iParameterType = 2;
							m_ccParameterType.SetCurSel(m_iParameterType);
							m_ccParameterBool.ShowWindow(SW_HIDE);
							m_ceParameterValue.ShowWindow(SW_SHOW);
							m_fParameterValue = iter->second->fValue;
						}
							break;
						case SSS::PT_TRIGGER:
						{
							m_iParameterType = 3;
							m_ccParameterType.SetCurSel(m_iParameterType);
							m_ccParameterBool.ShowWindow(SW_HIDE);
							m_ceParameterValue.ShowWindow(SW_HIDE);
							m_fParameterValue = iter->second->bValue;
						}
							break;

						default:
							break;
						}
					}
					else
					{
						wstring wstrParam(strParam.begin(), strParam.end());
						wstring wstrLog(L"Cannot find \"");
						wstrLog += wstrParam;
						wstrLog += L"\" in this animation controller.";
						MessageBox(wstrLog.c_str(), L"Exception", MB_OK);
					}
				}
			}
			SAFE_RELEASE(pAnimator);
		}
	}

	UpdateData(FALSE);
}

//Background Click Event
void CDlgAnimationClip::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_clControllerParameterList.SetCurSel(-1);
	ClearParameterControls();

	CDialogEx::OnLButtonDown(nFlags, point);
}

//Remove Parameter
void CDlgAnimationClip::OnBnClickedButtonConrtollerParameterRemove()
{
	int iCursel = m_clControllerParameterList.GetCurSel();

	if (iCursel >= 0)
	{
		CString strParameterName;
		m_clControllerParameterList.GetText(iCursel, strParameterName);
		CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
		if (pGameObject)
		{
			CAnimator* pAnimator = pGameObject->GetComponent<CAnimator>();
			if (pAnimator)
			{
				CAnimationStateController* pController = pAnimator->GetAnimationController();
				if (pController)
				{
					string strParam = CT2CA(strParameterName);
					pController->RemoveParameter(strParam);
					m_clControllerParameterList.DeleteString(iCursel);
					ClearParameterControls();
				}
			}

			SAFE_RELEASE(pAnimator);
		}
	}
}


BOOL CDlgAnimationClip::PreTranslateMessage(MSG* pMsg)
{	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
			return TRUE;
		else if (pMsg->wParam == VK_DELETE)
		{
			int iClipCursel = m_clMeshClipList.GetCurSel();
			if (iClipCursel >= 0)
			{
				CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
				if (pGameObject)
				{
					CMeshRenderer* pRenderer = pGameObject->GetComponent<CMeshRenderer>();
					if (pRenderer)
					{
						CMesh* pMesh = pRenderer->GetMesh();
						if (pMesh)
						{
							UpdateData(TRUE);
							vector<PANIMATIONCLIP>* vecClip = pMesh->GetAnimationClipInfo();
							SAFE_DELETE(vecClip->at(iClipCursel));
							vecClip->erase(vecClip->begin() + iClipCursel);
							UpdateContainedAnimationClipInfo(-1);
							m_strClipName = L"";
							m_iClipEndFrame = 0;
							m_iClipStartFrame = 0;
							UpdateData(FALSE);
						}
					}
					SAFE_RELEASE(pRenderer);
				}
			}
		}
		else if (pMsg->wParam == VK_RETURN)
			return TRUE;
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}


//Load PSA
void CDlgAnimationClip::OnBnClickedButtonLoadPsa()
{
	UpdateData(TRUE);
	
	CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

	if (pGameObject)
	{
		CMeshRenderer* pRenderer = pGameObject->GetComponent<CMeshRenderer>();
		if (pRenderer)
		{
			CMesh* pMesh = pRenderer->GetMesh();
			if (pMesh)
			{

				CFileDialog	dlg(TRUE, L"psa", L"Default", OFN_HIDEREADONLY, L"PSA(*.*)|*.*|AllFiles(*.*)|*.*||");

				if (dlg.DoModal() == IDOK)
				{
					CString	strFullPath = dlg.GetPathName();
					wstring wstrFullPath = strFullPath;

					vector<PANIMATIONCLIP>* pClip = pMesh->GetAnimationClipInfo();
					bool bResult = GET_SINGLE(CDataManager)->LoadPSAClipInfo(pClip, m_bOverridePSA, wstrFullPath.c_str(), NO_USE_PATH);
				
					if (bResult)
					{
						UpdateContainedAnimationClipInfo();
					}
				}
				else
				{
					MessageBox(L"Cannot open file.", L"", MB_OK);
				}
			}
			else
			{
				MessageBox(L"Mesh Renderer does not have mesh.", L"", MB_OK);
			}
		}
		else
		{
			MessageBox(L"GameObject does not have Mesh Renderer.", L"", MB_OK);
		}
		SAFE_RELEASE(pRenderer);
	}
	else
	{
		MessageBox(L"There is no selected object", L"", MB_OK);
	}

	UpdateData(FALSE);
}

//Toggle Override
void CDlgAnimationClip::OnBnClickedCheck1()
{
	UpdateData(TRUE);	
	UpdateData(FALSE);
}

//Edit Clip Name
void CDlgAnimationClip::OnEnChangeEditClipname()
{
	UpdateData(TRUE);

	int iCursel = m_clMeshClipList.GetCurSel();

	if (iCursel >= 0)
	{
		CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
		if (pGameObject)
		{
			CMeshRenderer* pRenderer = pGameObject->GetComponent<CMeshRenderer>();
			if (pRenderer)
			{
				CMesh* pMesh = pRenderer->GetMesh();
				if (pMesh)
				{
					vector<PANIMATIONCLIP>* vecClip = pMesh->GetAnimationClipInfo();
					vecClip->at(iCursel)->strName = CT2CA(m_strClipName);
					vecClip->at(iCursel)->iStartFrame = m_iClipStartFrame;
					vecClip->at(iCursel)->iEndFrame = m_iClipEndFrame;
					vecClip->at(iCursel)->iLength = vecClip->at(iCursel)->iEndFrame - vecClip->at(iCursel)->iStartFrame + 1;
				
					UpdateContainedAnimationClipInfo(iCursel);
				}
			}
			SAFE_RELEASE(pRenderer);
		}
	}

	UpdateData(FALSE);
}




void CDlgAnimationClip::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CDlgAnimationClip::OnTRBNThumbPosChangingSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 이 기능을 사용하려면 Windows Vista 이상이 있어야 합니다.
	// _WIN32_WINNT 기호는 0x0600보다 크거나 같아야 합니다.
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
