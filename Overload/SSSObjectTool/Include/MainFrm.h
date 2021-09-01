
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once

#include "ToolView.h"
#include "Inspector.h"
#include "Hierarchy.h"

class CMainFrame : public CFrameWnd
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	CSplitterWnd	  m_MainFrameWindow;
	CSplitterWnd	  m_AddsOn;
	CWnd*			  m_wndInspector;
public:
	CToolView*		m_pView;
	class CInspector*		m_pInspector;
	class CFormTabControl*	m_pFormTabCtr;
	class CHierarchy*		m_pHierarchy;
	class CDlgTabControl*	m_pDlgCtr;
	class CDlgTabControl*	m_pDlgTabCtr;

	// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	
public:
	afx_msg void OnDestroy();
	CWnd*	GetWndInspector();

};


