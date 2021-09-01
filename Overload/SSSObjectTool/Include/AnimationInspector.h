#pragma once


// CAnimationInspector ��ȭ �����Դϴ�.

class CAnimationInspector : public CDialogEx
{
	DECLARE_DYNAMIC(CAnimationInspector)

public:
	CAnimationInspector(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CAnimationInspector();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TRANSITION_INSPECTOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
