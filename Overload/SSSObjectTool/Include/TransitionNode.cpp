// TransitionNode.cpp : 구현 파일입니다.
//

#include "TransitionNode.h"
#include "afxdialogex.h"
#include "EditorArchive.h"
#include "AnimationControllerInspector.h"
#include "Hierarchy.h"
#include "DlgAnimationClip.h"
#include "Mathf.h"

#define EMPTY_TARGET L"Empty Target"


// CTransitionNode 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTransitionNode, CDialogEx)

CTransitionNode::CTransitionNode(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_TRANSITION, pParent)
	, m_iStateCount(0)
	, m_bClick(false)
	, m_fSpeedScale(0)
	, m_bLoop(FALSE)
	, m_strClipName(_T(""))
	, m_strTargetStateName(_T(""))
	, m_iOperator(0)
	, m_fValue(0)
	, m_bHasExitTime(FALSE)
	, m_fExitTime(0)
	, m_fExitDuration(0)
	, m_strStateName(_T(""))
	, m_pSelectedState(NULL)
	, m_pSelectedTransition(NULL)
	, m_iSelectedStateCursel(-1)
	, m_strSummary(_T(""))
	, m_strParameterType(_T(""))
	, m_iParameterValueBool(0)
	, m_strParameterTargetName(_T(""))
	, m_strParameterTarget(_T(""))
{
}

CTransitionNode::~CTransitionNode()
{
}

void CTransitionNode::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STATE_NAME, m_clStateList);
	DDX_Control(pDX, IDC_EDIT_STATE_NAME_TRANSITION, m_ceStateName);
	DDX_Text(pDX, IDC_EDIT_SET_SPEED, m_fSpeedScale);
	DDX_Check(pDX, IDC_CHECK_LOOP, m_bLoop);
	DDX_Text(pDX, IDC_EDIT_CLIP_NAME_TRANSITION, m_strClipName);
	DDX_Control(pDX, IDC_LIST_TRANSITION, m_clTransitionList);
	DDX_Text(pDX, IDC_EDIT_TARGET_PARAMETER_NAME, m_strTargetStateName);
	DDX_CBIndex(pDX, IDC_COMBO_OPERATOR, m_iOperator);
	DDV_MinMaxInt(pDX, m_iOperator, -1, 6);
	DDX_Text(pDX, IDC_EDIT_VALUE_TRANSITION, m_fValue);
	DDX_Check(pDX, IDC_CHECK_HAS_EXIT_TILE, m_bHasExitTime);
	DDX_Text(pDX, IDC_EDIT_EXIT_TIME, m_fExitTime);
	DDV_MinMaxFloat(pDX, m_fExitTime, 0, 1);
	DDX_Text(pDX, IDC_EDIT_EXIT_DURATION, m_fExitDuration);
	DDX_Text(pDX, IDC_EDIT_STATE_NAME_TRANSITION, m_strStateName);
	DDX_Control(pDX, IDC_STATIC_PARAMETER_TYPE, m_csParameterType);
	DDX_Control(pDX, IDC_STATIC_PARAMETER_TARGET, m_csParameterTarget);
	DDX_Control(pDX, IDC_STATIC_PARAMETER_OPERATOR, m_csParameterOperator);
	DDX_Control(pDX, IDC_STATIC_PARAMETER_VALUE, m_csParameterValue);
	DDX_Control(pDX, IDC_STATIC_EXIT_DURATION_MEASURE, m_csExitDurationMeasure);
	DDX_Control(pDX, IDC_STATIC_EXIT_DURATION, m_csExitDuration);
	DDX_Control(pDX, IDC_EDIT_EXIT_DURATION, m_ceExitDuration);
	DDX_Control(pDX, IDC_STATIC_STATE_NAME, m_csStateName);
	DDX_Control(pDX, IDC_CHECK_LOOP, m_cbLoop);
	DDX_Control(pDX, IDC_EDIT_SET_SPEED, m_ceStateSpeed);
	DDX_Control(pDX, IDC_STATIC_CLIP_NAME, m_csClipName);
	DDX_Control(pDX, IDC_EDIT_CLIP_NAME, m_ceClipName);
	DDX_Control(pDX, IDC_STATIC_TRANSITION_LIST, m_csTransitionList);
	DDX_Control(pDX, IDC_BUTTON_ADD_TRANSITION, m_cbAddTransition);
	DDX_Control(pDX, IDC_STATIC_STATE_SPEED, m_csStateSpeed);
	DDX_Control(pDX, IDC_STATIC_TRANSITION_TARGET_NAME, m_csTransitionTargetName);
	DDX_Control(pDX, IDC_EDIT_TRANSITION_TARGET_NAME, m_ceTransitionTargetName);
	DDX_Control(pDX, IDC_CHECK_HAS_EXIT_TIME, m_cbHasExitTime);
	DDX_Control(pDX, IDC_STATIC_EXIT_TIME_MEASURE, m_csExitTimeMeasure);
	DDX_Control(pDX, IDC_STATIC_EXIT_TIME, m_csExitTime);
	DDX_Control(pDX, IDC_EDIT_EXIT_TIME, m_ceExitTime);
	DDX_Control(pDX, IDC_STATIC_PARAMETER_LIST, m_csParameterList);
	DDX_Control(pDX, IDC_BUTTON_ADD_PARAMETER, m_cbAddParameter);
	DDX_Control(pDX, IDC_LIST_PARAMETER, m_clParameterList);
	DDX_Control(pDX, IDC_STATIC_PARTING_LINE, m_csPartingLine);
	DDX_Control(pDX, IDC_EDIT_TARGET_PARAMETER_NAME, m_ceParameterTarget);
	DDX_Control(pDX, IDC_COMBO_OPERATOR, m_ccParameterOperator);
	DDX_Control(pDX, IDC_EDIT_VALUE_TRANSITION, m_ceParameterValue);
	DDX_Control(pDX, IDC_STATIC_SUMMARY, m_csSummary);
	DDX_Text(pDX, IDC_STATIC_SUMMARY, m_strSummary);
	DDX_Text(pDX, IDC_STATIC_PARAMETER_TYPE_SPEC, m_strParameterType);
	DDX_Control(pDX, IDC_STATIC_PARAMETER_TYPE_SPEC, m_csParameterTypeSpec);
	DDX_Control(pDX, IDC_COMBO_VALUE_TRANSITION_BOOL, m_ccParameterValueBool);
	DDX_CBIndex(pDX, IDC_COMBO_VALUE_TRANSITION_BOOL, m_iParameterValueBool);
	DDV_MinMaxInt(pDX, m_iParameterValueBool, 0, 1);
	DDX_Text(pDX, IDC_EDIT_TRANSITION_TARGET_NAME, m_strTargetStateName);
	DDX_Text(pDX, IDC_EDIT_TARGET_PARAMETER_NAME, m_strParameterTarget);
	DDX_Control(pDX, IDC_BUTTON_REMOVE_TRANSITION, m_cbRemoveTransition);
}

void CTransitionNode::HideParameterSettings(bool bHide)
{
	if (bHide)
	{
		m_csParameterType.ShowWindow(SW_HIDE);
		m_csParameterTarget.ShowWindow(SW_HIDE);
		m_csParameterOperator.ShowWindow(SW_HIDE);
		m_csParameterValue.ShowWindow(SW_HIDE);

		m_csParameterType.EnableWindow(FALSE);
		m_csParameterTarget.EnableWindow(FALSE);
		m_csParameterOperator.EnableWindow(FALSE);
		m_csParameterValue.EnableWindow(FALSE);
	}
	else
	{
		m_csParameterType.ShowWindow(SW_SHOW);
		m_csParameterTarget.ShowWindow(SW_SHOW);
		m_csParameterOperator.ShowWindow(SW_SHOW);
		m_csParameterValue.ShowWindow(SW_SHOW);

		m_csParameterType.EnableWindow(TRUE);
		m_csParameterTarget.EnableWindow(TRUE);
		m_csParameterOperator.EnableWindow(TRUE);
		m_csParameterValue.EnableWindow(TRUE);
	}
}

void CTransitionNode::HideInfoPanel(bool bHide)
{
	if (bHide)
	{
		//State Name
		m_csStateName.ShowWindow(SW_HIDE);
		m_ceStateName.ShowWindow(SW_HIDE);
		//Loop
		m_cbLoop.ShowWindow(SW_HIDE);
		//Speed
		m_csStateSpeed.ShowWindow(SW_HIDE);
		m_ceStateSpeed.ShowWindow(SW_HIDE);
		//Clip Name
		m_csClipName.ShowWindow(SW_HIDE);
		m_ceClipName.ShowWindow(SW_HIDE);
		//Transition List
		m_clTransitionList.ShowWindow(SW_HIDE);
		m_csTransitionList.ShowWindow(SW_HIDE);
		m_cbAddTransition.ShowWindow(SW_HIDE);
		m_cbRemoveTransition.ShowWindow(SW_HIDE);
		//Target Name
		m_csTransitionTargetName.ShowWindow(SW_HIDE);
		m_ceTransitionTargetName.ShowWindow(SW_HIDE);
		//Has Exit Time
		m_cbHasExitTime.ShowWindow(SW_HIDE);
		//Exit Time
		m_csExitTime.ShowWindow(SW_HIDE);
		m_ceExitTime.ShowWindow(SW_HIDE);
		m_csExitTimeMeasure.ShowWindow(SW_HIDE);
		//Exit Duration
		m_csExitDuration.ShowWindow(SW_HIDE);
		m_ceExitDuration.ShowWindow(SW_HIDE);
		m_csExitDurationMeasure.ShowWindow(SW_HIDE);
		//Parameter List
		m_csParameterList.ShowWindow(SW_HIDE);
		m_cbAddParameter.ShowWindow(SW_HIDE);
		m_clParameterList.ShowWindow(SW_HIDE);
		//Parting Line
		m_csPartingLine.ShowWindow(SW_HIDE);
		//Parameter
		m_csParameterType.ShowWindow(SW_HIDE);					
		m_csParameterTarget.ShowWindow(SW_HIDE);
		m_csParameterOperator.ShowWindow(SW_HIDE);
		m_csParameterValue.ShowWindow(SW_HIDE);
		m_csParameterTypeSpec.ShowWindow(SW_HIDE);
		m_ceParameterTarget.ShowWindow(SW_HIDE);
		m_ccParameterOperator.ShowWindow(SW_HIDE);
		m_ceParameterValue.ShowWindow(SW_HIDE);
		m_ccParameterValueBool.ShowWindow(SW_HIDE);

	}
	else
	{
		//State Name
		m_csStateName.ShowWindow(SW_SHOW);
		m_ceStateName.ShowWindow(SW_SHOW);
		//Loop
		m_cbLoop.ShowWindow(SW_SHOW);
		//Speed
		m_csStateSpeed.ShowWindow(SW_SHOW);
		m_ceStateSpeed.ShowWindow(SW_SHOW);
		//Clip Name
		m_csClipName.ShowWindow(SW_SHOW);
		m_ceClipName.ShowWindow(SW_SHOW);
		//Transition List
		m_clTransitionList.ShowWindow(SW_SHOW);
		m_csTransitionList.ShowWindow(SW_SHOW);
		m_cbAddTransition.ShowWindow(SW_SHOW);
		m_cbRemoveTransition.ShowWindow(SW_SHOW);
		//Target Name
		m_csTransitionTargetName.ShowWindow(SW_SHOW);
		m_ceTransitionTargetName.ShowWindow(SW_SHOW);
		//Has Exit Time
		m_cbHasExitTime.ShowWindow(SW_SHOW);
		//Exit Time
		m_csExitTime.ShowWindow(SW_SHOW);
		m_ceExitTime.ShowWindow(SW_SHOW);
		m_csExitTimeMeasure.ShowWindow(SW_SHOW);
		//Exit Duration
		m_csExitDuration.ShowWindow(SW_SHOW);
		m_ceExitDuration.ShowWindow(SW_SHOW);
		m_csExitDurationMeasure.ShowWindow(SW_SHOW);
		//Parameter List
		m_csParameterList.ShowWindow(SW_SHOW);
		m_cbAddParameter.ShowWindow(SW_SHOW);
		m_clParameterList.ShowWindow(SW_SHOW);
		//Parting Line
		m_csPartingLine.ShowWindow(SW_SHOW);
		//Parameter
		m_csParameterType.ShowWindow(SW_SHOW);
		m_csParameterTarget.ShowWindow(SW_SHOW);
		m_csParameterOperator.ShowWindow(SW_SHOW);
		m_csParameterValue.ShowWindow(SW_SHOW);
		m_ccParameterValueBool.ShowWindow(SW_HIDE);
		m_csParameterTypeSpec.ShowWindow(SW_HIDE);
		m_ceParameterTarget.ShowWindow(SW_SHOW);
		m_ccParameterOperator.ShowWindow(SW_SHOW);
		m_ceParameterValue.ShowWindow(SW_SHOW);
	}
}

void CTransitionNode::DisabledExitTimeSettings(bool bDisabled)
{
	if (bDisabled)
	{
		m_csExitTimeMeasure.EnableWindow(FALSE);
		m_csExitTime.EnableWindow(FALSE);
		m_ceExitTime.EnableWindow(FALSE);
	}
	else
	{
		m_csExitTimeMeasure.EnableWindow(TRUE);
		m_csExitTime.EnableWindow(TRUE);
		m_ceExitTime.EnableWindow(TRUE);
	}
}

void CTransitionNode::HideTransitionInfo(bool bHide)
{
	if (bHide)
	{
		//Target Name
		m_csTransitionTargetName.ShowWindow(SW_HIDE);
		m_ceTransitionTargetName.ShowWindow(SW_HIDE);
		//Has Exit Time
		m_cbHasExitTime.ShowWindow(SW_HIDE);
		//Exit Time
		m_csExitTime.ShowWindow(SW_HIDE);
		m_ceExitTime.ShowWindow(SW_HIDE);
		m_csExitTimeMeasure.ShowWindow(SW_HIDE);
		//Exit Duration
		m_csExitDuration.ShowWindow(SW_HIDE);
		m_ceExitDuration.ShowWindow(SW_HIDE);
		m_csExitDurationMeasure.ShowWindow(SW_HIDE);
		//Parameter List
		m_csParameterList.ShowWindow(SW_HIDE);
		m_cbAddParameter.ShowWindow(SW_HIDE);
		m_clParameterList.ShowWindow(SW_HIDE);
		//Parting Line
		m_csPartingLine.ShowWindow(SW_HIDE);
		//Parameter
		m_csParameterType.ShowWindow(SW_HIDE);
		m_csParameterTarget.ShowWindow(SW_HIDE);
		m_csParameterOperator.ShowWindow(SW_HIDE);
		m_csParameterValue.ShowWindow(SW_HIDE);
		m_csParameterTypeSpec.ShowWindow(SW_HIDE);
		m_ceParameterTarget.ShowWindow(SW_HIDE);
		m_ccParameterOperator.ShowWindow(SW_HIDE);
		m_ceParameterValue.ShowWindow(SW_HIDE);
	}
	else
	{

		//Target Name
		m_csTransitionTargetName.ShowWindow(SW_SHOW);
		m_ceTransitionTargetName.ShowWindow(SW_SHOW);
		//Has Exit Time
		m_cbHasExitTime.ShowWindow(SW_SHOW);
		//Exit Time
		m_csExitTime.ShowWindow(SW_SHOW);
		m_ceExitTime.ShowWindow(SW_SHOW);
		m_csExitTimeMeasure.ShowWindow(SW_SHOW);
		//Exit Duration
		m_csExitDuration.ShowWindow(SW_SHOW);
		m_ceExitDuration.ShowWindow(SW_SHOW);
		m_csExitDurationMeasure.ShowWindow(SW_SHOW);
		//Parameter List
		m_csParameterList.ShowWindow(SW_SHOW);
		m_cbAddParameter.ShowWindow(SW_SHOW);
		m_clParameterList.ShowWindow(SW_SHOW);
		//Parting Line
		m_csPartingLine.ShowWindow(SW_SHOW);
		//Parameter
		m_csParameterType.ShowWindow(SW_SHOW);
		m_csParameterTarget.ShowWindow(SW_SHOW);
		m_csParameterOperator.ShowWindow(SW_SHOW);
		m_csParameterValue.ShowWindow(SW_SHOW);
		m_csParameterTypeSpec.ShowWindow(SW_SHOW);
		m_ceParameterTarget.ShowWindow(SW_SHOW);
		m_ccParameterOperator.ShowWindow(SW_SHOW);
		m_ceParameterValue.ShowWindow(SW_SHOW);
	}
}

void CTransitionNode::HideParameterInfo(bool bHide, bool bExceptTarget)
{
	if (bHide)
	{
		//Parameter
		m_csParameterTarget.ShowWindow(SW_HIDE);
		m_ceParameterTarget.ShowWindow(SW_HIDE);
		if (!bExceptTarget)
		{
			m_csParameterType.ShowWindow(SW_HIDE);
			m_csParameterTypeSpec.ShowWindow(SW_HIDE);

			m_csParameterOperator.ShowWindow(SW_HIDE);
			m_ccParameterOperator.ShowWindow(SW_HIDE);

			m_csParameterValue.ShowWindow(SW_HIDE);
			m_ceParameterValue.ShowWindow(SW_HIDE);
		}
	}
	else
	{
		//Parameter
		m_csParameterTarget.ShowWindow(SW_SHOW);
		m_ceParameterTarget.ShowWindow(SW_SHOW);
		if (!bExceptTarget)
		{
			m_csParameterType.ShowWindow(SW_SHOW);
			m_csParameterTypeSpec.ShowWindow(SW_SHOW);

			m_csParameterOperator.ShowWindow(SW_SHOW);
			m_ccParameterOperator.ShowWindow(SW_SHOW);

			m_csParameterValue.ShowWindow(SW_SHOW);
			m_ceParameterValue.ShowWindow(SW_SHOW);
		}
	}
}

void CTransitionNode::SetParameterControls()
{
	int iTransitionCursel = m_clTransitionList.GetCurSel();
	int iParameterCursel = m_clParameterList.GetCurSel();

	if (iTransitionCursel < 0 || iParameterCursel < 0)
	{
		//전체 다 날림
		HideParameterInfo(true, false);
		return;
	}
	else
	{
		if (m_pSelectedState)
		{
			vector<CTransition*> vecTransition = m_pSelectedState->GetTransition();
			CTransition* pTransition = vecTransition[iTransitionCursel];
			if (pTransition)
			{
				vector<PPARAMETER>* vecTargetParameter = pTransition->GetTargetParameter();
				vector<PARAMETER_CONDITION>* vecOperator = pTransition->GetOperator();
				vector<PARAMETER>* vecValue = pTransition->GetValue();

				if (vecTargetParameter->at(iParameterCursel))
				{
					//전체 다 켬
					HideParameterInfo(false, false);
					m_strParameterTargetName = vecTargetParameter->at(iParameterCursel)->strKey.c_str();
					m_strParameterTarget = m_strParameterTargetName;
					switch (vecTargetParameter->at(iParameterCursel)->eType)
					{
					case PT_BOOL:
					{
						m_strParameterType = L"(bool)";
						m_csParameterOperator.ShowWindow(SW_SHOW);
						m_csParameterValue.ShowWindow(SW_SHOW);
						m_ceParameterValue.ShowWindow(SW_HIDE);
						m_ccParameterValueBool.ShowWindow(SW_SHOW);
						m_ccParameterOperator.ResetContent();
						m_ccParameterOperator.AddString(L"==");
						m_ccParameterOperator.AddString(L"!=");
						
						//Operator
						if (vecOperator->at(iParameterCursel) == PC_EQUALS)
						{
							m_iOperator = 0;
						}
						else if (vecOperator->at(iParameterCursel) == PC_NOT_EQUAL)
						{
							m_iOperator = 1;
						}
						else
						{
							m_iOperator = 0;
						}

						//Value
						vecValue->at(iParameterCursel).eType = PT_BOOL;
						if (!vecValue->at(iParameterCursel).bValue)
						{
							m_iParameterValueBool = 0;
						}
						else
						{
							m_iParameterValueBool = 1;
						}
					}break;
					case PT_INT:
					{
						m_strParameterType = L"(int)";
						m_csParameterOperator.ShowWindow(SW_SHOW);
						m_csParameterValue.ShowWindow(SW_SHOW);
						m_ceParameterValue.ShowWindow(SW_SHOW);
						m_ccParameterValueBool.ShowWindow(SW_HIDE);
						m_ccParameterOperator.ResetContent();
						m_ccParameterOperator.AddString(L">");
						m_ccParameterOperator.AddString(L">=");
						m_ccParameterOperator.AddString(L"<");
						m_ccParameterOperator.AddString(L"<=");
						m_ccParameterOperator.AddString(L"==");
						m_ccParameterOperator.AddString(L"!=");

						if (vecOperator->at(iParameterCursel) == PC_GRATER)
						{
							m_iOperator = 0;
						}
						else if (vecOperator->at(iParameterCursel) == PC_GRATER_EQUAL)
						{
							m_iOperator = 1;
						}
						else if (vecOperator->at(iParameterCursel) == PC_LESS)
						{
							m_iOperator = 2;
						}
						else if (vecOperator->at(iParameterCursel) == PC_LESS_EQUAL)
						{
							m_iOperator = 3;
						}
						else if (vecOperator->at(iParameterCursel) == PC_EQUALS)
						{
							m_iOperator = 4;
						}
						else if (vecOperator->at(iParameterCursel) == PC_NOT_EQUAL)
						{
							m_iOperator = 5;
						}
						else
						{
							m_iOperator = 0;
						}
						
						//Value
						vecValue->at(iParameterCursel).eType = PT_INT;
						m_fValue = (float)vecValue->at(iParameterCursel).iValue;

					}break;
					case PT_FLOAT:
					{
						m_strParameterType = L"(float)";
						m_csParameterOperator.ShowWindow(SW_SHOW);
						m_csParameterValue.ShowWindow(SW_SHOW);
						m_ceParameterValue.ShowWindow(SW_SHOW);
						m_ccParameterValueBool.ShowWindow(SW_HIDE);
						m_ccParameterOperator.ResetContent();
						m_ccParameterOperator.AddString(L">");
						m_ccParameterOperator.AddString(L">=");
						m_ccParameterOperator.AddString(L"<");
						m_ccParameterOperator.AddString(L"<=");

						if (vecOperator->at(iParameterCursel) == PC_GRATER)
						{
							m_iOperator = 0;
						}
						else if (vecOperator->at(iParameterCursel) == PC_GRATER_EQUAL)
						{
							m_iOperator = 1;
						}
						else if (vecOperator->at(iParameterCursel) == PC_LESS)
						{
							m_iOperator = 2;
						}
						else if (vecOperator->at(iParameterCursel) == PC_LESS_EQUAL)
						{
							m_iOperator = 3;
						}
						else
						{
							m_iOperator = 0;
						}

						//Value
						vecValue->at(iParameterCursel).eType = PT_FLOAT;
						m_fValue = vecValue->at(iParameterCursel).fValue;
					}break;
					case PT_TRIGGER:
					{
						m_strParameterType = L"(trigger)";
						m_csParameterOperator.ShowWindow(SW_HIDE);
						m_csParameterValue.ShowWindow(SW_HIDE);
						m_ceParameterValue.ShowWindow(SW_HIDE);
						m_ccParameterValueBool.ShowWindow(SW_HIDE);
						m_ccParameterOperator.ResetContent();
						m_ccParameterOperator.ShowWindow(SW_HIDE);
						
						//Value
						vecValue->at(iParameterCursel).eType = PT_TRIGGER;

					}break;
					default:
					{
						m_strParameterType = L"";
						m_csParameterValue.ShowWindow(SW_SHOW);
						m_ccParameterValueBool.ShowWindow(SW_HIDE);
					}break;
					}
				}
				else
				{
					//Parameter가 비어있을 떄
					m_strParameterType = L"";
					m_strParameterTarget = L"Empty Parameter";
					m_csParameterValue.ShowWindow(SW_SHOW);
					m_ccParameterValueBool.ShowWindow(SW_HIDE);
					HideParameterInfo(false, true);
				}
			}
		}
	}
}

void CTransitionNode::HideFromState(int iStateCursel)
{
	//Entry
	if (iStateCursel == 0)
	{
		m_ceStateName.EnableWindow(FALSE);

		//State Name
		m_csStateName.ShowWindow(SW_SHOW);
		m_ceStateName.ShowWindow(SW_SHOW);
		//Loop
		m_cbLoop.ShowWindow(SW_HIDE);
		//Speed
		m_csStateSpeed.ShowWindow(SW_HIDE);
		m_ceStateSpeed.ShowWindow(SW_HIDE);
		//Clip Name
		m_csClipName.ShowWindow(SW_HIDE);
		m_ceClipName.ShowWindow(SW_HIDE);
		//Transition List
		m_clTransitionList.ShowWindow(SW_SHOW);
		m_csTransitionList.ShowWindow(SW_SHOW);
		m_cbAddTransition.ShowWindow(SW_SHOW);
		m_cbRemoveTransition.ShowWindow(SW_SHOW);
		HideTransitionInfo(true);
	}
	//Any
	else if (iStateCursel == 1)
	{
		m_ceStateName.EnableWindow(FALSE);

		//State Name
		m_csStateName.ShowWindow(SW_SHOW);
		m_ceStateName.ShowWindow(SW_SHOW);
		//Loop
		m_cbLoop.ShowWindow(SW_HIDE);
		//Speed
		m_csStateSpeed.ShowWindow(SW_HIDE);
		m_ceStateSpeed.ShowWindow(SW_HIDE);
		//Clip Name
		m_csClipName.ShowWindow(SW_HIDE);
		m_ceClipName.ShowWindow(SW_HIDE);
		//Transition List
		m_clTransitionList.ShowWindow(SW_SHOW);
		m_csTransitionList.ShowWindow(SW_SHOW);
		m_cbAddTransition.ShowWindow(SW_SHOW);
		m_cbRemoveTransition.ShowWindow(SW_SHOW);
		HideTransitionInfo(true);
	}
	//Exit
	else if (iStateCursel == 2)
	{
		m_ceStateName.EnableWindow(FALSE);

		//State Name
		m_csStateName.ShowWindow(SW_SHOW);
		m_ceStateName.ShowWindow(SW_SHOW);

		//Loop
		m_cbLoop.ShowWindow(SW_HIDE);

		//Speed
		m_csStateSpeed.ShowWindow(SW_HIDE);
		m_ceStateSpeed.ShowWindow(SW_HIDE);

		//Clip Name
		m_csClipName.ShowWindow(SW_HIDE);
		m_ceClipName.ShowWindow(SW_HIDE);

		//Transition List
		m_clTransitionList.ShowWindow(SW_HIDE);
		m_csTransitionList.ShowWindow(SW_HIDE);
		m_cbAddTransition.ShowWindow(SW_HIDE);
		m_cbRemoveTransition.ShowWindow(SW_HIDE);
		HideTransitionInfo(true);
	}
	else
	{
		m_ceStateName.EnableWindow(TRUE);
		HideInfoPanel(false);
	}
}

BOOL CTransitionNode::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_rComponentRect.top = 100;
	m_rComponentRect.left = 10;
	m_rComponentRect.bottom = m_rComponentRect.top + 50;
	m_rComponentRect.right = m_rComponentRect.left + 100;

	return 0;
}

void CTransitionNode::CreateDefaultState()
{
	CGameObject* pObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

	if (pObject)
	{
		CAnimator* pAnimator = pObject->GetComponent<CAnimator>();

		if (pAnimator)
		{
			CAnimationStateController* pController = pAnimator->GetAnimationController();

			if (pController)
			{
				m_clStateList.ResetContent();
				m_clStateList.AddString(CA2CT("Entry State"));
				m_clStateList.AddString(CA2CT("Any State"));
				m_clStateList.AddString(CA2CT("Exit State"));
			}
		}
		SAFE_RELEASE(pAnimator);
	}
}

void CTransitionNode::UpdateContainedItems()
{
	CGameObject* pObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

	if (pObject)
	{
		CAnimator* pAnimator = pObject->GetComponent<CAnimator>();

		if (pAnimator)
		{
			CAnimationStateController* pController = pAnimator->GetAnimationController();
		
			if (pController)
			{
				CreateDefaultState();
				unordered_map<string, CAnimationState*>::const_iterator iter;
				unordered_map<string, CAnimationState*>::const_iterator iterEnd = pController->GetAnimationState().end();
				
				for (iter = pController->GetAnimationState().begin(); iter != iterEnd; ++iter)
				{
					m_clStateList.AddString(CA2CT((*iter).first.c_str()));
				}
			}
		}
		SAFE_RELEASE(pAnimator);
	}
}

void CTransitionNode::SetCurSel(CAnimationState * pState)
{
	CString strStateName = CA2CT(pState->GetStateName().c_str());
	
	int iCount = m_clStateList.GetCount();
	for (int i = 0; i < iCount; ++i)
	{
		CString strCursel;
		m_clStateList.GetText(i, strCursel);
		if (strCursel == strStateName)
		{
			m_clStateList.SetCurSel(i);
			return;
		}
	}

	m_clStateList.SetCurSel(-1);
}

//Get String From Current Cursel
string CTransitionNode::GetStateName()
{
	string strResult;
	int iCursel = m_clStateList.GetCurSel();
	if (iCursel >= 0 && iCursel < m_clStateList.GetCount())
	{
		CString strStateName;
		m_clStateList.GetText(iCursel, strStateName);
		strResult = CT2CA(strStateName.GetString());
	}

	return strResult;
}

//Get String From Cursel
string CTransitionNode::GetStateName( size_t iCursel)
{
	string strResult;

	if (iCursel >= 0 && iCursel < m_clStateList.GetCount())
	{
		CString strStateName;
		m_clStateList.GetText(iCursel, strStateName);

		strResult = CT2CA(strStateName.GetString());
	}

	return strResult;
}

CAnimationState * CTransitionNode::GetState()
{
	string strStateName = GetStateName();
	if (strStateName != "")
	{
		CGameObject* pObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

		if (pObject)
		{
			CAnimator* pAnimator = pObject->GetComponent<CAnimator>();

			if (pAnimator)
			{
				CAnimationStateController* pController = pAnimator->GetAnimationController();

				if (pController)
				{
					SAFE_RELEASE(pAnimator);
					return pController->GetAnimationState(strStateName);
				}
			}
			SAFE_RELEASE(pAnimator);
		}
	}
	return NULL;
}

CAnimationState * CTransitionNode::GetState(size_t iCursel)
{
	string strStateName = GetStateName(iCursel);
	if (strStateName != "")
	{
		CGameObject* pObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

		if (pObject)
		{
			CAnimator* pAnimator = pObject->GetComponent<CAnimator>();

			if (pAnimator)
			{
				CAnimationStateController* pController = pAnimator->GetAnimationController();

				if (pController)
				{
					SAFE_RELEASE(pAnimator);
					return pController->GetAnimationState(strStateName);
				}
			}
			SAFE_RELEASE(pAnimator);
		}
	}
	return NULL;
}

void CTransitionNode::SelectState(CAnimationState * pState)
{
	if (pState)
	{
		HideInfoPanel(false);

		if (m_iSelectedStateCursel == 0 || m_iSelectedStateCursel == 1 || m_iSelectedStateCursel == 2)
		{
			m_ceStateName.EnableWindow(FALSE);
		}
		else
		{
			m_ceStateName.EnableWindow(TRUE);
		}

		//State Info
		m_strStateName = pState->GetStateName().c_str();
		m_bLoop = pState->IsLoop();
		m_fSpeedScale = pState->GetSpeed();

		if (pState->GetClip())
		{
			m_strClipName = pState->GetClip()->GetClipName().c_str();
		}
		else
		{
			m_strClipName = "";
		}

		//Transition Info
		vector<CTransition*> vecTransition = pState->GetTransition();
		size_t iTransitionCount = vecTransition.size();
		for (size_t i = 0; i < iTransitionCount; ++i)
		{
			if (vecTransition[i]->GetTargetState())
			{
				string strTargetName = vecTransition[i]->GetTargetState()->GetStateName();
				m_clTransitionList.AddString(CA2CT(strTargetName.c_str()));
			}
			else
			{
				m_clTransitionList.AddString(EMPTY_TARGET);
			}
		}

		m_clTransitionList.SetCurSel(-1);
		
		HideParameterInfo(true);
	}
	else
	{
		HideInfoPanel(true);
	}
}


void CTransitionNode::RefreshStateInfo(CAnimationState * pState)
{
}

void CTransitionNode::RefreshTransitionList(CAnimationState * pState)
{
	m_clTransitionList.ResetContent();
	
	if (pState)
	{
		vector<CTransition*> vecTransition = pState->GetTransition();
	
		size_t iCount = vecTransition.size();
		for (size_t i = 0; i < iCount; ++i)
		{
			CAnimationState* pTargetState = vecTransition[i]->GetTargetState();
			if (pTargetState)
			{
				string strTargetStateName = pTargetState->GetStateName();
				m_clTransitionList.AddString(CA2CT(strTargetStateName.c_str()));
			}
			else
			{
				m_clTransitionList.AddString(EMPTY_TARGET);
			}
		}
	}
}

void CTransitionNode::RefreshTransitionInfo(CTransition * pTransition)
{
	m_fExitDuration = pTransition->GetExitDuration();
	m_fExitTime = pTransition->GetExitTime();
	m_bHasExitTime = pTransition->HasExitTime();

	if (pTransition->GetTargetState())
	{
		m_strTargetStateName = pTransition->GetTargetState()->GetStateName().c_str();
	}
	else
	{
		m_strTargetStateName = L"";
	}
}

void CTransitionNode::RefreshParameterInfo(CTransition * pTransition, int iCursel)
{
	if (iCursel > 0)
	{
		m_clTransitionList.SetCurSel(iCursel);
		m_bHasExitTime = pTransition->HasExitTime();
		m_fExitTime = pTransition->GetExitTime();
		DisabledExitTimeSettings(!m_bHasExitTime);
		if (m_bHasExitTime)
		{
			m_fExitDuration = pTransition->GetExitDuration();
		}
		else
		{
			m_fExitDuration = 0.0f;
		}
	}
	else
	{
		m_clTransitionList.SetCurSel(-1);
		m_bHasExitTime = false;
		m_fExitTime = 0.0f;
		m_fExitDuration = 0.0f;
		DisabledExitTimeSettings(true);
	}
}

void CTransitionNode::ClearInfoPanel()
{
	m_pSelectedState = NULL;
	m_pSelectedTransition = NULL;

	m_strStateName = "";
	m_bLoop = false;
	m_fSpeedScale = 0.0f;
	m_strClipName = "";

	m_clTransitionList.ResetContent();
	m_clTransitionList.SetCurSel(-1);
	m_strTargetStateName = "";
	m_bHasExitTime = false;
	m_fExitTime = 0.0f;
	m_fExitDuration = 0.0f;
	DisabledExitTimeSettings(true);

	m_clParameterList.ResetContent();
	m_clParameterList.SetCurSel(-1);
	HideParameterInfo(true);
}

void CTransitionNode::UpdateTransitionList(CAnimationState * pState)
{
	m_clTransitionList.ResetContent();

	vector<CTransition*> vecTransition = pState->GetTransition();
	size_t iTransitionCount = vecTransition.size();

	for (size_t i = 0; i < iTransitionCount; ++i)
	{
		CAnimationState* pTargetState = vecTransition[i]->GetTargetState();
		if (pTargetState)
		{
			m_clTransitionList.AddString(CA2CT(pTargetState->GetStateName().c_str()));
		}
		else
		{
			m_clTransitionList.AddString(EMPTY_TARGET);
		}
	}

	HideParameterInfo(true);
}

void CTransitionNode::UpdateStateInfo(CAnimationState *)
{
}

void CTransitionNode::UpdateTransitionInfo(CTransition * pTransition)
{	
	m_fExitDuration = pTransition->GetExitDuration();
	m_fExitTime = pTransition->GetExitTime();
	m_bHasExitTime = pTransition->HasExitTime();

	if (pTransition->GetTargetState())
	{
		m_strTargetStateName = pTransition->GetTargetState()->GetStateName().c_str();
	}
	else
	{
		m_strTargetStateName = L"";
	}
}

BEGIN_MESSAGE_MAP(CTransitionNode, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_STATE, &CTransitionNode::OnBnClickedButtonCreateState)
	ON_BN_CLICKED(IDC_BUTTON_ADD_TRANSITION, &CTransitionNode::OnBnClickedButtonAddTransition)
	ON_LBN_SELCHANGE(IDC_LIST_STATE_NAME, &CTransitionNode::OnLbnSelchangeListStateName)
	ON_BN_CLICKED(IDC_CHECK_HAS_EXIT_TILE, &CTransitionNode::OnBnClickedCheckHasExitTime)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE_TRANSITION, &CTransitionNode::OnCbnSelchangeComboTypeTransition)
	ON_BN_CLICKED(IDC_BUTTON_ADD_PARAMETER, &CTransitionNode::OnBnClickedButtonAddParameter)
	ON_EN_CHANGE(IDC_EDIT_VALUE_TRANSITION, &CTransitionNode::OnEnChangeEditValueTransition)
	ON_STN_CLICKED(IDC_STATIC_PARAMETER_TYPE, &CTransitionNode::OnStnClickedStaticParameterType)
	ON_WM_LBUTTONDOWN()
	ON_EN_CHANGE(IDC_EDIT_STATE_NAME_TRANSITION, &CTransitionNode::OnEnChangeEditStateNameTransition)
	ON_EN_KILLFOCUS(IDC_EDIT_STATE_NAME_TRANSITION, &CTransitionNode::OnEnKillfocusEditStateNameTransition)
	ON_BN_CLICKED(IDC_CHECK_LOOP, &CTransitionNode::OnBnClickedCheckLoop)
	ON_EN_CHANGE(IDC_EDIT_SET_SPEED, &CTransitionNode::OnEnChangeEditSetSpeed)
	ON_EN_CHANGE(IDC_EDIT_CLIP_NAME, &CTransitionNode::OnEnChangeEditClipName)
	ON_EN_KILLFOCUS(IDC_EDIT_CLIP_NAME, &CTransitionNode::OnEnKillfocusEditClipName)
	ON_STN_CLICKED(IDC_STATIC_SUMMARY, &CTransitionNode::OnStnClickedStaticSummary)
	ON_LBN_SELCHANGE(IDC_LIST_TRANSITION, &CTransitionNode::OnLbnSelchangeListTransition)
//	ON_EN_CHANGE(IDC_EDIT_TRANSITION_TARGET_NAME, &CTransitionNode::OnEnChangeEditTransitionTargetName)
ON_EN_KILLFOCUS(IDC_EDIT_TRANSITION_TARGET_NAME, &CTransitionNode::OnEnKillfocusEditTransitionTargetName)
ON_EN_CHANGE(IDC_EDIT_EXIT_TIME, &CTransitionNode::OnEnChangeEditExitTime)
ON_EN_CHANGE(IDC_EDIT_EXIT_DURATION, &CTransitionNode::OnEnChangeEditExitDuration)
ON_LBN_SELCHANGE(IDC_LIST_PARAMETER, &CTransitionNode::OnLbnSelchangeListParameter)
ON_EN_KILLFOCUS(IDC_EDIT_TARGET_PARAMETER_NAME, &CTransitionNode::OnEnKillfocusEditTargetParameterName)
ON_EN_CHANGE(IDC_EDIT_TARGET_PARAMETER_NAME, &CTransitionNode::OnEnChangeEditTargetParameterName)
ON_LBN_SETFOCUS(IDC_LIST_PARAMETER, &CTransitionNode::OnLbnSetfocusListParameter)
ON_EN_CHANGE(IDC_EDIT_TRANSITION_TARGET_NAME, &CTransitionNode::OnEnChangeEditTransitionTargetName)
ON_CBN_SELCHANGE(IDC_COMBO_VALUE_TRANSITION_BOOL, &CTransitionNode::OnCbnSelchangeComboValueTransitionBool)
ON_CBN_SELCHANGE(IDC_COMBO_OPERATOR, &CTransitionNode::OnCbnSelchangeComboOperator)
ON_STN_CLICKED(IDC_STATIC_TRANSITION_LIST, &CTransitionNode::OnStnClickedStaticTransitionList)
ON_BN_CLICKED(IDC_BUTTON_REMOVE_TRANSITION, &CTransitionNode::OnBnClickedButtonRemoveTransition)
ON_EN_KILLFOCUS(IDC_EDIT_VALUE_TRANSITION, &CTransitionNode::OnEnKillfocusEditValueTransition)
ON_EN_KILLFOCUS(IDC_EDIT_SET_SPEED, &CTransitionNode::OnEnKillfocusEditSetSpeed)
END_MESSAGE_MAP()


// CTransitionNode 메시지 처리기입니다.

// Create State Button Click Event
void CTransitionNode::OnBnClickedButtonCreateState()
{
	CGameObject* pObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

	if (pObject)
	{
		CAnimator* pAnimator = pObject->GetComponent<CAnimator>();

		if (pAnimator)
		{
			CAnimationStateController* pController = pAnimator->GetAnimationController();

			if (pController)
			{
				int iOverlapCount = 0;
				while (true)
				{
					string strStateName("New State");
					if (iOverlapCount != 0)
					{
						strStateName += " ";
						strStateName += to_string(iOverlapCount);
					}

					CAnimationState* pOverlapState = pController->GetAnimationState(strStateName);

					if (pOverlapState)
					{
						++iOverlapCount;
						continue;
					}
					else
					{
						CAnimationState* pState = pController->CreateAnimationState(strStateName);
						m_clStateList.AddString(CA2CT(strStateName.c_str()));
						m_pSelectedState = pState;
						int iCurSel = m_clStateList.GetCount() - 1;
						m_clStateList.SetCurSel(iCurSel);
						m_iSelectedStateCursel = iCurSel;
						SelectState(pState);
						OnLbnSelchangeListStateName();
						break;
					}
				}
			}
		}
		SAFE_RELEASE(pAnimator);
	}
	UpdateData(FALSE);
}


// Add Transition Button Click Event
void CTransitionNode::OnBnClickedButtonAddTransition()
{

	CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

	if (pGameObject)
	{
		CAnimator* pAnimator = pGameObject->GetComponent<CAnimator>();
		if (pAnimator)
		{
			CAnimationStateController* pController = pAnimator->GetAnimationController();
			if (pController)
			{
				CAnimationState* pEntryState = pController->GetAnimationState("Entry State");
				CAnimationState* pAnyState = pController->GetAnimationState("Any State");
				CAnimationState* pExitState = pController->GetAnimationState("Exit State");

				if (m_pSelectedState == pEntryState)
				{
					if (m_pSelectedState->GetTransitionCount() < 1)
					{
						CTransition* pTransition = m_pSelectedState->AddTransition();
						m_clTransitionList.AddString(EMPTY_TARGET);
					}
				}
				else if (m_pSelectedState == pAnyState)
				{
					CTransition* pTransition = m_pSelectedState->AddTransition();
					m_clTransitionList.AddString(EMPTY_TARGET);
				}
				else if (m_pSelectedState == pExitState)
				{
					//Do Nothing
				}
				else
				{
					CTransition* pTransition = m_pSelectedState->AddTransition();
					m_clTransitionList.AddString(EMPTY_TARGET);
				}

			}
		}
		SAFE_RELEASE(pAnimator);
	}
	UpdateData(FALSE);

}

//Transition List Click Event
void CTransitionNode::OnLbnSelchangeListTransition()
{
	UpdateData(TRUE);
	int iCursel = m_clTransitionList.GetCurSel();
	if (iCursel >= 0)
	{
		m_strSummary = L"Has Exit Time이 체크되어 있다면\n현재 State에서 Exit Time 까지 대기 후 Exit Duration 동안 Transition됩니다.\n체크되어 있지 않다면 조건 만족 시 즉시 Transition됩니다.\nExit Duration은 Has Exit Time과 상관없이 사용 가능합니다.";

		vector<CTransition*> vecTransition = m_pSelectedState->GetTransition();
		CTransition* pTransition = vecTransition[iCursel];
		m_pSelectedTransition = pTransition;

		HideTransitionInfo(false);

		if (pTransition->GetTargetState())
		{
			m_strTargetStateName = pTransition->GetTargetState()->GetStateName().c_str();
		}
		else
		{
			m_strTargetStateName = EMPTY_TARGET;
		}


		CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

		if (pGameObject)
		{
			CAnimator* pAnimator = pGameObject->GetComponent<CAnimator>();
			if (pAnimator)
			{
				CAnimationStateController* pController = pAnimator->GetAnimationController();
				if (pController)
				{
					CAnimationState* pEntryState = pController->GetAnimationState("Entry State");
					CAnimationState* pAnyState = pController->GetAnimationState("Any State");
					CAnimationState* pExitState = pController->GetAnimationState("Exit State");

					if (m_pSelectedState == pEntryState)
					{
						m_csTransitionTargetName.ShowWindow(SW_SHOW);
						m_ceTransitionTargetName.ShowWindow(SW_SHOW);
						m_cbHasExitTime.ShowWindow(SW_HIDE);
						m_csExitTime.ShowWindow(SW_HIDE);
						m_csExitTimeMeasure.ShowWindow(SW_HIDE);
						m_ceExitTime.ShowWindow(SW_HIDE);
						m_csExitDuration.ShowWindow(SW_HIDE);
						m_csExitDurationMeasure.ShowWindow(SW_HIDE);
						m_ceExitDuration.ShowWindow(SW_HIDE);
						m_csPartingLine.ShowWindow(SW_HIDE);
						m_clParameterList.ShowWindow(SW_HIDE);
						m_csParameterList.ShowWindow(SW_HIDE);
						m_cbAddParameter.ShowWindow(SW_HIDE);
					}
					else if (m_pSelectedState == pAnyState)
					{
						m_bHasExitTime = pTransition->HasExitTime();
						m_fExitDuration = pTransition->GetExitDuration();
						m_fExitTime = pTransition->GetExitTime();

						DisabledExitTimeSettings(!m_bHasExitTime);
					}
					else if (m_pSelectedState == pExitState)
					{
						m_csTransitionTargetName.ShowWindow(SW_HIDE);
						m_ceTransitionTargetName.ShowWindow(SW_HIDE);
						m_cbHasExitTime.ShowWindow(SW_HIDE);
						m_csExitTime.ShowWindow(SW_HIDE);
						m_csExitTimeMeasure.ShowWindow(SW_HIDE);
						m_ceExitTime.ShowWindow(SW_HIDE);
						m_csExitDuration.ShowWindow(SW_HIDE);
						m_csExitDurationMeasure.ShowWindow(SW_HIDE);
						m_ceExitDuration.ShowWindow(SW_HIDE);
						m_csPartingLine.ShowWindow(SW_HIDE);
						m_clParameterList.ShowWindow(SW_HIDE);
						m_csParameterList.ShowWindow(SW_HIDE);
						m_cbAddParameter.ShowWindow(SW_HIDE);
						DisabledExitTimeSettings(true);
					}
					else
					{
						m_bHasExitTime = pTransition->HasExitTime();
						m_fExitDuration = pTransition->GetExitDuration();
						m_fExitTime = pTransition->GetExitTime();
						DisabledExitTimeSettings(!m_bHasExitTime);
					}
				}
			}
		}
		
		//Parameters
		m_clParameterList.ResetContent();
		vector<PPARAMETER>* vecTargetParameter =  pTransition->GetTargetParameter();
		vector<PARAMETER_CONDITION>* vecOperator = pTransition->GetOperator();
		vector<PARAMETER>* vecValue = pTransition->GetValue();

		size_t iCount = pTransition->GetParameterCount();

		for (size_t i = 0; i < iCount; ++i)
		{
			if (vecTargetParameter->at(i))
			{
				string strParameterName = vecTargetParameter->at(i)->strKey;
				m_clParameterList.AddString(CA2CT(strParameterName.c_str()));
			}
			else
			{
				m_clParameterList.AddString(L"Empty Parameter");
			}
		}

		HideParameterInfo(true);
	}

	UpdateData(FALSE);
}

//State List Contents Clicked
void CTransitionNode::OnLbnSelchangeListStateName()
{
	UpdateData(TRUE);

	HideInfoPanel(false);
	int iCursel = m_clStateList.GetCurSel();

	if (iCursel >= 0 && iCursel < m_clStateList.GetCount())
	{
		CAnimationState* pState = GetState((size_t)iCursel);
		if (pState)
		{
			m_pSelectedState = pState;
			m_iSelectedStateCursel = iCursel;

			string strStateName = pState->GetStateName();
			m_strStateName = CA2CT(strStateName.c_str());
			m_fSpeedScale = pState->GetSpeed();
			m_bLoop = pState->IsLoop();
			
			if (pState->GetClip())
			{
				m_strClipName = pState->GetClip()->GetClipName().c_str();
			}
			else
			{
				m_strClipName = "";
			}

			if (m_iSelectedStateCursel == 0 || m_iSelectedStateCursel == 1 || m_iSelectedStateCursel == 2)
			{
				if (m_iSelectedStateCursel == 0)
				{
					m_strSummary = L"Entry State는 반드시 하나의 State로만 Transition될 수 있습니다.\nEntry State는 Transition의 대상이 될 수 없습니다.";

				}
				else if (m_iSelectedStateCursel == 1)
				{
					m_strSummary = L"Any State는 여러 State로 Transition될 수 있습니다.\nAny State는 Transition의 대상이 될 수 없습니다.";
				}
				else
				{
					m_strSummary = L"Exit State는 어떤 State로도 Transition될 수 없습니다.\nExit State는 Transition의 대상이 될 수 있습니다.";
				}
			}
			else
			{
				m_strSummary = L"";
			}

			HideFromState(m_iSelectedStateCursel);
			
			m_clTransitionList.ResetContent();
			
			if (pState->GetTransitionCount() > 0)
			{
				vector<CTransition*> vecTransition = pState->GetTransition();
				size_t iSize = vecTransition.size();
				for (size_t i = 0; i < iSize; ++i)
				{
					CAnimationState* pTargetState = vecTransition[i]->GetTargetState();
					if (pTargetState)
					{
						m_clTransitionList.AddString(CA2CT(vecTransition[i]->GetTargetState()->GetStateName().c_str()));
					}
					else
					{
						m_clTransitionList.AddString(EMPTY_TARGET);
					}
				}
			}

			HideTransitionInfo(true);
		}
	}
	else
	{
		UpdateData(TRUE);
		m_clStateList.SetCurSel(-1);
		m_iSelectedStateCursel = -1;
		m_pSelectedState = NULL;
		m_pSelectedTransition = NULL;
		m_strSummary = L"";

		ClearInfoPanel();
		HideInfoPanel(true);
		UpdateData(FALSE);
	}
	UpdateData(FALSE);
}

//Has Exit Time Toggled
void CTransitionNode::OnBnClickedCheckHasExitTime()
{
	UpdateData(TRUE); 
	int iCursel = m_clTransitionList.GetCurSel();
	if (iCursel >= 0)
	{
		vector<CTransition*> vecTransition = m_pSelectedState->GetTransition();
		CTransition* pTransition = vecTransition[iCursel];
		pTransition->UseExitTime(m_bHasExitTime);
		DisabledExitTimeSettings(!m_bHasExitTime);
	}

	UpdateData(FALSE);
}



//Add Parameter Button Clicked
void CTransitionNode::OnBnClickedButtonAddParameter()
{
	UpdateData(TRUE);

	if (m_pSelectedState)
	{
		int iTransitionCursel = m_clTransitionList.GetCurSel();

		if (iTransitionCursel >= 0)
		{
			vector<CTransition*> vecTransition = m_pSelectedState->GetTransition();
			CTransition* pTransition = vecTransition[iTransitionCursel];
			pTransition->AddEmptyParameter();
			m_clParameterList.AddString(L"Empty Parameter");
		}
	}

	UpdateData(FALSE);
}

//Parameter - Type Drop Down Clicked
void CTransitionNode::OnCbnSelchangeComboTypeTransition()
{
	UpdateData(TRUE);
	UpdateData(FALSE);
}

//Parameter Value Changed
void CTransitionNode::OnEnChangeEditValueTransition()
{
	UpdateData(TRUE);
	UpdateData(FALSE);
}

//Parameter Type Changed
void CTransitionNode::OnStnClickedStaticParameterType()
{
	UpdateData(TRUE);
	UpdateData(FALSE);
}


//Clicked Background Event
void CTransitionNode::OnLButtonDown(UINT nFlags, CPoint point)
{
	UpdateData(TRUE);
	m_clStateList.SetCurSel(-1);
	m_iSelectedStateCursel = -1;
	m_pSelectedState = NULL;
	m_pSelectedTransition = NULL;
	m_strSummary = L"";

	ClearInfoPanel();
	HideInfoPanel(true);
	CDialogEx::OnLButtonDown(nFlags, point);
	UpdateData(FALSE);
}


void CTransitionNode::OnEnChangeEditStateNameTransition()
{
	UpdateData(TRUE);
	m_clStateList.DeleteString(m_iSelectedStateCursel);
	string strStateName = CT2CA(m_strStateName);
	m_pSelectedState->SetStateName(strStateName);
	m_clStateList.InsertString(m_iSelectedStateCursel, m_strStateName);
	UpdateData(FALSE);
}


void CTransitionNode::OnEnKillfocusEditStateNameTransition()
{
	UpdateData(TRUE);	
	UpdateData(FALSE);
}

//Loop Button Changed
void CTransitionNode::OnBnClickedCheckLoop()
{
	UpdateData(TRUE);	
	if (m_pSelectedState)
	{
		m_pSelectedState->Loop(m_bLoop);
	}
	UpdateData(FALSE);
}


void CTransitionNode::OnEnChangeEditSetSpeed()
{
	//UpdateData(TRUE);
	//if (m_pSelectedState)
	//{
	//	m_pSelectedState->SetSpeed(m_fSpeedScale);
	//}
	//UpdateData(FALSE);
}


void CTransitionNode::OnEnChangeEditClipName()
{
	UpdateData(TRUE);
	UpdateData(FALSE);
}


void CTransitionNode::OnEnKillfocusEditClipName()
{
	UpdateData(TRUE);

	CGameObject* pObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

	if (pObject)
	{
		CAnimator* pAnimator = pObject->GetComponent<CAnimator>();
		if (pAnimator)
		{
			if (pAnimator->GetMeshRenderer())
			{
				CMesh* pMesh = pAnimator->GetMeshRenderer()->GetMesh();

				string strClipName = CT2CA(m_strClipName.GetString());
				PANIMATIONCLIP pClip = pMesh->GetAnimationClipInfo(strClipName);

				if (pClip)
				{
					if (m_pSelectedState)
					{
						m_pSelectedState->SetClip(strClipName);
					}
				}
				else
				{
					wstring strReport(L"There is no such clip with \"");
					strReport += m_strClipName.GetString();
					strReport += L"\"";

					MessageBox(strReport.c_str(), L"Error", MB_OK);
				}
			}
			else
			{
				MessageBox(L"Cannot find \"CMeshRenderer\" in this game object", L"", MB_OK);
			}
		}
		SAFE_RELEASE(pAnimator);
	}

	UpdateData(FALSE);
}


void CTransitionNode::OnStnClickedStaticSummary()
{
	UpdateData(TRUE);
	UpdateData(FALSE);
}


//Edit Transition Target Name Event
//void CTransitionNode::OnEnChangeEditTransitionTargetName()
//{
//}


void CTransitionNode::OnEnKillfocusEditTransitionTargetName()
{
	UpdateData(TRUE);
	
	int iTransitionCursel = m_clTransitionList.GetCurSel();

	if (iTransitionCursel >= 0 && m_pSelectedState)
	{
		string strTargetName = CT2CA(m_strTargetStateName);
		CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
		if (pGameObject)
		{
			CAnimator* pAnimator = pGameObject->GetComponent<CAnimator>();
			if (pAnimator)
			{
				CAnimationStateController* pController = pAnimator->GetAnimationController();
				if (pController)
				{
					vector<CTransition*> vecTransition = m_pSelectedState->GetTransition();
					CTransition* pTransition = vecTransition[iTransitionCursel];

					if (strTargetName == "Entry State")
					{
						MessageBox(L"\"Entry State\" cannot be target state", L"Exception", MB_OK);
						if (pTransition->GetTargetState())
						{
							string strTargetStateName = pTransition->GetTargetState()->GetStateName();
							m_strTargetStateName = strTargetStateName.c_str();
							m_clTransitionList.DeleteString(iTransitionCursel);
							m_clTransitionList.InsertString(iTransitionCursel, m_strTargetStateName);
						}
						else
						{
							m_strTargetStateName = EMPTY_TARGET;
							m_clTransitionList.DeleteString(iTransitionCursel);
							m_clTransitionList.InsertString(iTransitionCursel, m_strTargetStateName);
						}
					}
					else if (strTargetName == "Any State")
					{
						MessageBox(L"\"Any State\" cannot be target state", L"Exception", MB_OK);
						if (pTransition->GetTargetState())
						{
							string strTargetStateName = pTransition->GetTargetState()->GetStateName();
							m_strTargetStateName = strTargetStateName.c_str();
							m_clTransitionList.DeleteString(iTransitionCursel);
							m_clTransitionList.InsertString(iTransitionCursel, m_strTargetStateName);
						}
						else
						{
							m_strTargetStateName = EMPTY_TARGET;
							m_clTransitionList.DeleteString(iTransitionCursel);
							m_clTransitionList.InsertString(iTransitionCursel, m_strTargetStateName);
						}
					}
					else
					{
						CAnimationState* pTargetState = pController->GetAnimationState(strTargetName);

						if (pTargetState)
						{
							pTransition->SetTargetState(pTargetState);
							m_clTransitionList.DeleteString(iTransitionCursel);
							m_clTransitionList.InsertString(iTransitionCursel, m_strTargetStateName);
						}
						else
						{
							MessageBox(L"Cannot find target state", L"Exception", MB_OK);
							if (pTransition->GetTargetState())
							{
								string strTargetStateName = pTransition->GetTargetState()->GetStateName();
								m_strTargetStateName = strTargetStateName.c_str();
								m_clTransitionList.DeleteString(iTransitionCursel);
								m_clTransitionList.InsertString(iTransitionCursel, m_strTargetStateName);
							}
							else
							{
								m_strTargetStateName = EMPTY_TARGET;
								m_clTransitionList.DeleteString(iTransitionCursel);
								m_clTransitionList.InsertString(iTransitionCursel, m_strTargetStateName);
							}
						}
					}
				}
			}
			SAFE_RELEASE(pAnimator);
		}
	}
	UpdateData(FALSE);
}


//Edit Exit Time Event
void CTransitionNode::OnEnChangeEditExitTime()
{
	UpdateData(TRUE);
	if (m_pSelectedState)
	{
		int iCursel = m_clTransitionList.GetCurSel();
		vector<CTransition*> vecTransition = m_pSelectedState->GetTransition();
		CTransition* pTransition = vecTransition[iCursel];
		if (pTransition)
		{ 
			m_fExitTime = CMathf::Clamp(m_fExitTime, 0.0f, 1.0f);
			pTransition->SetExitTime(m_fExitTime);
		}
	}
	UpdateData(FALSE);
}

//Edit Exit Duration Event
void CTransitionNode::OnEnChangeEditExitDuration()
{
	UpdateData(TRUE);
	if (m_pSelectedState)
	{
		int iCursel = m_clTransitionList.GetCurSel();
		if (iCursel >= 0)
		{
			vector<CTransition*> vecTransition = m_pSelectedState->GetTransition();
			CTransition* pTransition = vecTransition[iCursel];
			if (pTransition)
			{
				m_fExitDuration = CMathf::Clamp(m_fExitDuration, 0.0f, 10000000.0f);
				pTransition->SetExitDuration(m_fExitDuration);
			}
		}
	}
	UpdateData(FALSE);
}


//Parameter List Click Event
void CTransitionNode::OnLbnSelchangeListParameter()
{
	UpdateData(TRUE);
	int iCursel = m_clParameterList.GetCurSel();

	if (iCursel >= 0)
	{
		SetParameterControls();		
	}
	else
	{
		//Hide Parameter
		HideParameterInfo(true, true);
	}
	UpdateData(FALSE);
}

//End Edit Parameter Target Name
void CTransitionNode::OnEnKillfocusEditTargetParameterName()
{
	UpdateData(TRUE);

	string strTargetName = CT2CA(m_strParameterTarget);

	if (m_pSelectedState)
	{
		int iCursel = m_clParameterList.GetCurSel();
		int iTransitionCursel = m_clTransitionList.GetCurSel();

		if (iCursel >= 0)
		{
			CAnimationStateController* pController = m_pSelectedState->GetAnimationController();

			PPARAMETER pParameter = pController->FindParameter(strTargetName);
			if (pParameter)
			{
				m_clParameterList.DeleteString(iCursel);
				m_clParameterList.InsertString(iCursel, m_strParameterTarget);


				vector<CTransition*> vecTransition = m_pSelectedState->GetTransition();
				CTransition* pTransition = vecTransition[iTransitionCursel];
				if (pTransition)
				{
					vector<PPARAMETER>* vecTargetParameter = pTransition->GetTargetParameter();
					vector<PARAMETER_CONDITION>* vecOperator = pTransition->GetOperator();
					vector<PARAMETER>* vecValue = pTransition->GetValue();

					vecTargetParameter->at(iCursel) = pParameter;
					vecValue->at(iCursel).eType = pParameter->eType;
				}

				SetParameterControls();
			}
		}
	}

	UpdateData(FALSE);
}


//Parameter 이름 실시간으로 체크함
void CTransitionNode::OnEnChangeEditTargetParameterName()
{	
}


BOOL CTransitionNode::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
			return TRUE;
		else if (pMsg->wParam == VK_RETURN)
		{
			CWnd*	pFocus = GetFocus();
			HWND	hFocus = pFocus->GetSafeHwnd();
			if (hFocus == GetDlgItem(IDC_EDIT_TARGET_PARAMETER_NAME)->GetSafeHwnd())
			{
				OnEnKillfocusEditTargetParameterName();
			}
			if (hFocus == GetDlgItem(IDC_EDIT_TRANSITION_TARGET_NAME)->GetSafeHwnd())
			{
				OnEnKillfocusEditTransitionTargetName();
			}
			if (hFocus == GetDlgItem(IDC_EDIT_CLIP_NAME)->GetSafeHwnd())
			{
				OnEnKillfocusEditClipName();
			}

			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CTransitionNode::OnLbnSetfocusListParameter()
{
}


void CTransitionNode::OnEnChangeEditTransitionTargetName()
{
}

//Target Parameter Type Change
void CTransitionNode::OnCbnSelchangeComboValueTransitionBool()
{
	UpdateData(TRUE);
	int iTransitionCursel = m_clTransitionList.GetCurSel();
	int iParameterCursel = m_clParameterList.GetCurSel();
	
	if (iTransitionCursel >= 0 && iParameterCursel >= 0)
	{
		CTransition* pTransition = m_pSelectedState->GetTransition((size_t)iTransitionCursel);
		if (pTransition)
		{
			vector<PARAMETER>* pvecParameter = pTransition->GetValue();
			//반드시 bool 타입일 경우이므로 예외를 따로 잡지 않는다.
			if (pvecParameter)
			{
				pvecParameter->at(iParameterCursel).bValue = (bool)m_iParameterValueBool;
			}
		}
	}

	UpdateData(FALSE);
}


void CTransitionNode::OnCbnSelchangeComboOperator()
{
	UpdateData(TRUE);
	int iTransitionCursel = m_clTransitionList.GetCurSel();
	int iParameterCursel = m_clParameterList.GetCurSel();

	if (iTransitionCursel >= 0 && iParameterCursel >= 0)
	{
		CTransition* pTransition = m_pSelectedState->GetTransition((size_t)iTransitionCursel);
		if (pTransition)
		{
			vector<PARAMETER_CONDITION>* pvecOperator = pTransition->GetOperator();
			vector<PPARAMETER>* pvecTargetParameter = pTransition->GetTargetParameter();

			if (pvecOperator)
			{
				if (pvecTargetParameter)
				{
					PARAMETER_TYPE eType = pvecTargetParameter->at(iParameterCursel)->eType;

					if (eType == PT_BOOL)
					{
						if (m_iOperator == 0)
						{
							//==
							pvecOperator->at(iParameterCursel) = PC_EQUALS;
						}
						else if (m_iOperator == 1)
						{
							// !=
							pvecOperator->at(iParameterCursel) = PC_NOT_EQUAL;
						}
					}
					else if (eType == PT_INT)
					{
						if (m_iOperator == 0)
						{
							//>
							pvecOperator->at(iParameterCursel) = PC_GRATER;
						}
						else if (m_iOperator == 1)
						{
							// >=
							pvecOperator->at(iParameterCursel) = PC_GRATER_EQUAL;
						}
						else if (m_iOperator == 2)
						{
							// <
							pvecOperator->at(iParameterCursel) = PC_LESS;
						}
						else if (m_iOperator == 3)
						{
							// <=
							pvecOperator->at(iParameterCursel) = PC_LESS_EQUAL;
						}
						else if (m_iOperator == 4)
						{
							// ==
							pvecOperator->at(iParameterCursel) = PC_EQUALS;
						}
						else if (m_iOperator == 5)
						{
							// !=
							pvecOperator->at(iParameterCursel) = PC_NOT_EQUAL;
						}
					}
					else if (eType == PT_FLOAT)
					{
						if (m_iOperator == 0)
						{
							//>
							pvecOperator->at(iParameterCursel) = PC_GRATER;
						}
						else if (m_iOperator == 1)
						{
							// >=
							pvecOperator->at(iParameterCursel) = PC_GRATER_EQUAL;
						}
						else if (m_iOperator == 2)
						{
							// <
							pvecOperator->at(iParameterCursel) = PC_LESS;
						}
						else if (m_iOperator == 3)
						{
							// <=
							pvecOperator->at(iParameterCursel) = PC_LESS_EQUAL;
						}
					}
				}
			}
		}
	}

	UpdateData(FALSE);
}


void CTransitionNode::OnStnClickedStaticTransitionList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CTransitionNode::OnBnClickedButtonRemoveTransition()
{
	int iStateCursel = m_clStateList.GetCurSel();
	int iTransitionCursel = m_clTransitionList.GetCurSel();

	if ((iStateCursel >= 0 && iStateCursel < m_clStateList.GetCount()) &&
		(iTransitionCursel >= 0 && iTransitionCursel < m_clTransitionList.GetCount()))
	{
		CGameObject* pSelectedObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

		if (pSelectedObject)
		{
			CAnimator* pAnimator = pSelectedObject->GetComponent<CAnimator>();
			if (pAnimator)
			{
				CAnimationStateController* pController = pAnimator->GetAnimationController();
				if (pController)
				{
					string strStateName = GetStateName(iStateCursel);
					CAnimationState* pState = pController->GetAnimationState(strStateName);				
					if (pState)
					{
						CTransition* pTransition = pState->GetTransition(iTransitionCursel);						
						if (pTransition)
						{
							pState->RemoveTransition(pTransition);
							m_clTransitionList.DeleteString(iTransitionCursel);
						}					
					}				
				}
			}
			SAFE_RELEASE(pAnimator);
		}
	}
}


void CTransitionNode::OnEnKillfocusEditValueTransition()
{
	UpdateData(TRUE);

	int iTransitionCursel = m_clTransitionList.GetCurSel();
	int iParameterCursel = m_clParameterList.GetCurSel();
	int iStateCursel = m_clStateList.GetCurSel();
	
	if (iStateCursel >= 0 && iStateCursel < m_clStateList.GetCount())
	{
		if (iTransitionCursel >= 0 && iTransitionCursel < m_clTransitionList.GetCount())
		{
			if (iParameterCursel >= 0 && iParameterCursel < m_clParameterList.GetCount())
			{
				CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
				if (pGameObject)
				{
					CAnimator* pAnimator = pGameObject->GetComponent<CAnimator>();
					if (pAnimator)
					{
						CAnimationStateController* pController = pAnimator->GetAnimationController();
						if (pController)
						{
							string strStateName = GetStateName(iStateCursel);
							CAnimationState* pState = pController->GetAnimationState(strStateName);
							if (pState)
							{
								CTransition* pTransition = pState->GetTransition(iTransitionCursel);
								if (pTransition)
								{
									vector<PPARAMETER>* vecTargetParameter = pTransition->GetTargetParameter();
									vector<PARAMETER>* vecParam = pTransition->GetValue();

									PARAMETER_TYPE eType = vecTargetParameter->at(iParameterCursel)->eType;

									switch (eType)
									{
									case SSS::PT_INT:
									{
										vecParam->at(iParameterCursel).iValue = (int)m_fValue;
									}
										break;
									case SSS::PT_FLOAT:
									{
										vecParam->at(iParameterCursel).fValue = m_fValue;
									}
										break;
									default:
										break;
									}
								}
							}
						}
					}
					SAFE_RELEASE(pAnimator);
				}
			}
		}
	}
	UpdateData(FALSE);
}


void CTransitionNode::OnEnKillfocusEditSetSpeed()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	if (m_pSelectedState)
	{
		m_pSelectedState->SetSpeed(m_fSpeedScale);
	}
	UpdateData(FALSE);
}
