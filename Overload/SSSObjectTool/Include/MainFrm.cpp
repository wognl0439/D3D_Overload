
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "Tool.h"

#include "MainFrm.h"
#include "Core.h"
#include "EditorArchive.h"
#include "DlgTabControl.h"

SSS_USING

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() :
	m_pDlgTabCtr(NULL)
{
	//GET_SINGLE(CAtlasEditorArchive)->Initialize();
}

CMainFrame::~CMainFrame()
{
	//GET_SINGLE(CAtlasEditorArchive)->DestroyInstance();
	SAFE_DELETE(m_pDlgTabCtr);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext * pContext)
{
	GET_SINGLE(CEditorArchive)->SetContext(pContext);
	m_MainFrameWindow.CreateStatic(this, 1, 3);

	// 뷰 클래스를 왼쪽, 인스펙터 클래스를 오른쪽에 배치한다.
	m_MainFrameWindow.CreateView(0, 1, RUNTIME_CLASS(CToolView),
		CSize(1080, 650), pContext);

	m_MainFrameWindow.CreateView(0, 2, RUNTIME_CLASS(CInspector),
		CSize(435, 550), pContext);

	m_MainFrameWindow.CreateView(0, 0, RUNTIME_CLASS(CHierarchy),
		CSize(200, 650), pContext);
	

	m_pView = (CToolView*)m_MainFrameWindow.GetPane(0, 1);
	m_pInspector = (CInspector*)m_MainFrameWindow.GetPane(0, 2);
	m_pInspector->SetWndInspector(m_MainFrameWindow.GetPane(0, 2));
		
	m_pHierarchy = (CHierarchy*)m_MainFrameWindow.GetPane(0, 0);
	m_pHierarchy->SetWndHierarchy(m_MainFrameWindow.GetPane(0, 0));

	m_pInspector->SetHierarchy(m_pHierarchy);
	m_pHierarchy->SetInspector(m_pInspector);

	GET_SINGLE(CEditorArchive)->SetToolViewHandle(m_MainFrameWindow.GetPane(0, 1));
	GET_SINGLE(CEditorArchive)->SetInspectorHandle(m_MainFrameWindow.GetPane(0,2));
	GET_SINGLE(CEditorArchive)->SetHierarchyHandle(m_MainFrameWindow.GetPane(0, 0));

	GET_SINGLE(CEditorArchive)->SetInspector(m_pInspector);
	GET_SINGLE(CEditorArchive)->SetHierarchy(m_pHierarchy);


	return TRUE;

}


void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();

}

CWnd * CMainFrame::GetWndInspector()
{
	return m_wndInspector;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	cs.cx = 1715;
	cs.cy = 800;

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

