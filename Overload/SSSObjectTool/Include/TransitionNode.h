#pragma once

#include "stdafx.h"
#include "afxwin.h"
#include "Tool.h"

#include "GameObject.h"
#include "Component.h"
#include "Component/Animator.h"
#include "AnimationState.h"
#include "AnimationStateController.h"
#include "AnimationClip.h"
#include "Transition.h"

// CTransitionNode 대화 상자입니다.

SSS_USING

class CTransitionNode : public CDialogEx
{
	DECLARE_DYNAMIC(CTransitionNode)

public:
	CTransitionNode(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTransitionNode();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TRANSITION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	int		m_iStateCount;
	CRect	m_rComponentRect;
	vector<class CStatic*> m_vecStaticBit;
	vector<class CStatic*> m_vecStaticName;
	bool	m_bClick;


public:
	CAnimationState* m_pSelectedState;
	CTransition* m_pSelectedTransition;
	int					m_iSelectedStateCursel;
	
	virtual BOOL OnInitDialog();
	void CreateDefaultState();
	void UpdateContainedItems();

	void SetCurSel(class CAnimationState* pState);
	string GetStateName();
	string GetStateName(size_t iCursel);
	CAnimationState* GetState();
	CAnimationState* GetState(size_t iCursel);
	void SelectState(class CAnimationState* pState);

	
	//Refresh : ENGINE ->MFC
	//~ UpdateData(FALSE) 하도록 함. 
	void RefreshStateInfo(class CAnimationState* pState);
	void RefreshTransitionList(class CAnimationState* pState);
	void RefreshTransitionInfo(class CTransition* pTransition);
	void RefreshParameterInfo(class CTransition* pTransition, int iCursel);
	void HideParameterSettings(bool bHide);
	void HideInfoPanel(bool bHide);
	void DisabledExitTimeSettings(bool bDisabled);
	void HideTransitionInfo(bool bHide);
	void HideParameterInfo(bool bHide, bool bExceptTarget = false);
	void SetParameterControls();
	void HideFromState(int iStateCursel);

	//Update : MFC -> ENGINE
	//UpdateData(TRUE) ~ UpdateData(FALSE) 하도록 함. 
	void UpdateTransitionList(class CAnimationState* pState);
	void UpdateStateInfo(class CAnimationState* pState);
	void UpdateTransitionInfo(class CTransition* pTransition);

	//Clear : MFC  
	void ClearInfoPanel();

public:
	//void AddStateNode();

	BOOL		m_bLoop;
	BOOL		m_bHasExitTime;
	int				m_iOperator;
	float			m_fSpeedScale;
	float			m_fValue;
	float			m_fExitTime;
	float			m_fExitDuration;
	CString		m_strStateName;
	CString		m_strClipName;
	CString		m_strParameterTargetName;
	afx_msg void OnBnClickedButtonCreateState();
	afx_msg void OnBnClickedButtonAddTransition();
	afx_msg void OnLbnSelchangeListStateName();
	afx_msg void OnBnClickedCheckHasExitTime();
	afx_msg void OnCbnSelchangeComboTypeTransition();
	afx_msg void OnBnClickedButtonAddParameter();
	afx_msg void OnEnChangeEditValueTransition();
	afx_msg void OnStnClickedStaticParameterType();
	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnChangeEditStateNameTransition();

	afx_msg void OnEnKillfocusEditStateNameTransition();
	afx_msg void OnBnClickedCheckLoop();
	afx_msg void OnEnChangeEditSetSpeed();
	afx_msg void OnEnChangeEditClipName();
	afx_msg void OnEnKillfocusEditClipName();
	afx_msg void OnStnClickedStaticSummary();
	//InfoPanel Controls
	CStatic m_csStateName;
	CButton m_cbLoop;
	CEdit m_ceStateSpeed;
	CStatic m_csClipName;
	CEdit m_ceClipName;
	CStatic m_csTransitionList;
	CButton m_cbAddTransition;
	CStatic m_csStateSpeed;
	CStatic m_csTransitionTargetName;
	CButton m_cbHasExitTime;
	CStatic m_csExitTimeMeasure;
	CStatic m_csExitTime;
	CStatic m_csParameterList;
	CButton m_cbAddParameter;
	CListBox m_clParameterList;
	CListBox	m_clStateList;
	CListBox	m_clTransitionList;
	CStatic		m_csParameterType;
	CStatic		m_csParameterTarget;
	CStatic		m_csParameterOperator;
	CStatic		m_csParameterValue;
	CStatic		m_csExitDurationMeasure;
	CStatic		m_csExitDuration;
	CComboBox m_ccParameterOperator;
	CStatic		m_csPartingLine;
	CEdit			m_ceTransitionTargetName;
	CEdit			m_ceExitTime;
	CEdit			m_ceStateName;
	CEdit			m_ceExitDuration;
	CEdit			m_ceParameterTarget;
	CEdit			m_ceParameterValue;
	CStatic m_csSummary;
	CString m_strSummary;
	CString m_strParameterType;
	CStatic m_csParameterTypeSpec;
	CComboBox m_ccParameterValueBool;
	int m_iParameterValueBool;
	CString m_strTargetStateName;
	CString m_strParameterTarget;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLbnSelchangeListTransition();
	//	afx_msg void OnEnChangeEditTransitionTargetName();
	afx_msg void OnEnKillfocusEditTransitionTargetName();
	afx_msg void OnEnChangeEditExitTime();
	afx_msg void OnEnChangeEditExitDuration();
	afx_msg void OnLbnSelchangeListParameter();
	afx_msg void OnEnKillfocusEditTargetParameterName();
	afx_msg void OnEnChangeEditTargetParameterName();
	afx_msg void OnLbnSetfocusListParameter();
	afx_msg void OnEnChangeEditTransitionTargetName();
	afx_msg void OnCbnSelchangeComboValueTransitionBool();
	afx_msg void OnCbnSelchangeComboOperator();
	CButton m_cbRemoveTransition;
	afx_msg void OnStnClickedStaticTransitionList();
	afx_msg void OnBnClickedButtonRemoveTransition();
	afx_msg void OnEnKillfocusEditValueTransition();
	afx_msg void OnEnKillfocusEditSetSpeed();
};
