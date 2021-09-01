#pragma once
#include "afxwin.h"


// CAnimationControllerInspector 대화 상자입니다.

class CAnimationControllerInspector : public CDialogEx
{
	DECLARE_DYNAMIC(CAnimationControllerInspector)

public:
	CAnimationControllerInspector(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAnimationControllerInspector();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ANIMATION_CONTROLLER_INSPECTOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CButton m_btnAddTarget;
	afx_msg void OnBnClickedAddTargetButton();

private:
	CRect	m_DlgRC;
	CRect	m_rComponentRect;
	CButton*	m_pBtnClose;

public:
	CRect GetDlgRC();

public:
	virtual BOOL OnInitDialog();

private:
	void OnBnClickedButtonDlgExit();
public:
	afx_msg void OnEnKillfocusInsClipSpeedEdit();
	afx_msg void OnEnKillfocusInsCompareValueEdit();
	CComboBox m_TypeComboBox;
	afx_msg void OnCbnSelchangeInsCompareTypeCombo();
};
