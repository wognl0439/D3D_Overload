#pragma once


// CAnimationInspector 대화 상자입니다.

class CAnimationInspector : public CDialogEx
{
	DECLARE_DYNAMIC(CAnimationInspector)

public:
	CAnimationInspector(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAnimationInspector();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TRANSITION_INSPECTOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

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
};
