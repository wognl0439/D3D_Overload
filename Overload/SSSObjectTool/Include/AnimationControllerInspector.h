#pragma once
#include "afxwin.h"


// CAnimationControllerInspector ��ȭ �����Դϴ�.

class CAnimationControllerInspector : public CDialogEx
{
	DECLARE_DYNAMIC(CAnimationControllerInspector)

public:
	CAnimationControllerInspector(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CAnimationControllerInspector();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ANIMATION_CONTROLLER_INSPECTOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
