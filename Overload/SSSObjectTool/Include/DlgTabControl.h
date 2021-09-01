#pragma once
#include "afxcmn.h"


// CDlgTabControl 대화 상자입니다.

class CDlgTabControl : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTabControl)

public:
	CDlgTabControl(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgTabControl();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CWnd*	m_wndTabCtr;
	class CDlgAnimationClip*	m_pAnimation;
	class CTransitionNode* m_pTransition;
	class CAnimationControllerInspector* m_pAnimationCtrInspector;
	CTabCtrl m_TabCtr;
	CSplitterWnd	  m_SplitWnd;

public:
	virtual BOOL OnInitDialog();
	void	SetWndTabCtr(CWnd*	pWnd);
	afx_msg void OnTcnSelchangeTabControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnEnable(BOOL bEnable);
//	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
