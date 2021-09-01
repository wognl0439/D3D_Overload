#pragma once

#include "GameObject.h"
#include "afxwin.h"
#include "afxcmn.h"
// CDlgAnimationClip 대화 상자입니다.

class CDlgAnimationClip : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAnimationClip)

public:
	CDlgAnimationClip(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgAnimationClip();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ANIMATIONCLIP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	string m_strStateName;
	string m_strEntryStateName;
	float m_fStartFrame;
	float m_fEndFrame;
	CListBox m_clMeshClipList;
	CListBox m_clEditClipList;

public:
	void UpdateContainedAnimationClipInfo(int iCursel = -1);
	
public:
	virtual BOOL OnInitDialog();
	string GetStateName();
	string GetClipName();
	string GetEntryStateName();
	float GetStartFrame();
	float GetEndFrame();

	void ClearParameterControls();
	void UpdateParameterList();

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedClipEditButton();
	//afx_msg void OnBnClickedClipModifiedButton2();
	afx_msg void OnEnKillfocusEditAnimationstateName();
	afx_msg void OnEnKillfocusEditClipname();
	afx_msg void OnEnKillfocusEditStartFrame();
	afx_msg void OnEnKillfocusEditEndFrame();
	afx_msg void OnLbnSelchangeMeshClipList();
	//afx_msg void OnLbnSelchangeEditClipList();
	afx_msg void OnEnKillfocusEditEntrystateName();
	CListBox m_clControllerParameterList;
	CString m_strParameterName;
	CEdit m_ceParameterValue;
	CComboBox m_ccParameterBool;
	CComboBox m_ccParameterType;

	int m_iParameterType;
	float m_fParameterValue;
	afx_msg void OnBnClickedButtonControllerParameterCreate();
	int m_iParameterValueBool;
	BOOL m_bOverridePSA;



//	afx_msg void OnEnChangeEditControllerParameterName();
	afx_msg void OnEnKillfocusEditControllerParameterName();
	afx_msg void OnCbnSelchangeComboControllerParameterType();
	afx_msg void OnEnChangeEditControllerParameterValue();
	afx_msg void OnCbnSelchangeComboControllerParameterValueBool();
	afx_msg void OnLbnSelchangeListControllerParameter();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonConrtollerParameterRemove();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonLoadPsa();
	afx_msg void OnBnClickedCheck1();
	UINT m_iClipStartFrame;
	UINT m_iClipEndFrame;
	afx_msg void OnEnChangeEditClipname();
	CString m_strClipName;
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTRBNThumbPosChangingSlider1(NMHDR *pNMHDR, LRESULT *pResult);
};
