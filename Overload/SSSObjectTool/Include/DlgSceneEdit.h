#pragma once
#include "afxwin.h"


// CDlgSceneEdit ��ȭ �����Դϴ�.

class CDlgSceneEdit : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSceneEdit)

public:
	CDlgSceneEdit(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSceneEdit();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SCENE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_CBoxLayer;
	CButton m_btnDeleteLayer;
	CEdit m_editAddLayerName;
	CButton m_btnAddLayer;
	CListBox m_listCollideLayer;
	CListBox m_listNoneCollideLayer;
	CButton m_btnCollideLayer;
	CButton m_btnNoneCollideLayer;
	afx_msg void OnBnClickedButtonDeleteLayer();
	afx_msg void OnBnClickedButtonAddLayer();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnLbnSelchangeList2();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonCollideList();
	afx_msg void OnBnClickedButtonNoneCollideList();
	afx_msg void OnCbnSelchangeComboLayer();
	virtual void PostNcDestroy();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	CEdit m_editLayerZOrder;
};
