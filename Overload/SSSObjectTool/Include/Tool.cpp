
// Tool.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Tool.h"
#include "MainFrm.h"

#include "ToolDoc.h"
#include "ToolView.h"
#include "Core.h"

#include "EditorArchive.h"
#include "SceneManager.h"
#include "Transform.h"
#include "Layer.h"
#include "CollisionManager.h"
#include "PathManager.h"

SSS_USING

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CToolApp

BEGIN_MESSAGE_MAP(CToolApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CToolApp::OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// ǥ�� �μ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_CREATE_OBJECT, &CToolApp::OnCreateObject)
	ON_COMMAND(ID_CREATE_UI, &CToolApp::OnCreateUI)
	
	//ON_COMMAND(ID_TEXTURELOAD_BASE, &CToolApp::LoadBaseTexture)
	//ON_COMMAND(ID_TEXTURELOAD_SUB, &CToolApp::LoadSubTexture)
	ON_COMMAND(ID_32779, &CToolApp::OnAnimationDlg)
	ON_COMMAND(ID_32780, &CToolApp::SceneSave)
	ON_COMMAND(ID_32781, &CToolApp::SceneLoad)
	ON_COMMAND(ID_32786, &CToolApp::SavePrefab)
	ON_COMMAND(ID_32787, &CToolApp::LoadPrefab)
	//ON_COMMAND(ID_SCENE_GAMEMODE, &CToolApp::OnSceneGamemode)
	ON_COMMAND(ID_32788, &CToolApp::EditLayer)
END_MESSAGE_MAP()


// CToolApp ����

CToolApp::CToolApp()
{
	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���� ���α׷��� ���� ��� ��Ÿ�� ������ ����Ͽ� ������ ���(/clr):
	//     1) �� �߰� ������ �ٽ� ���� ������ ������ ����� �۵��ϴ� �� �ʿ��մϴ�.
	//     2) ������Ʈ���� �����Ϸ��� System.Windows.Forms�� ���� ������ �߰��ؾ� �մϴ�.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
	// ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Tool.AppID.NoVersion"));

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

// ������ CToolApp ��ü�Դϴ�.

CToolApp theApp;


// CToolApp �ʱ�ȭ

BOOL CToolApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.	
	// AfxInitRichEdit2();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.


	// ���� ���α׷��� ���� ���ø��� ����մϴ�.  ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CToolDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ������ â�Դϴ�.
		RUNTIME_CLASS(CToolView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// â �ϳ��� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();


	return TRUE;
}

int CToolApp::ExitInstance()
{
	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CToolApp �޽��� ó����


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CToolApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CToolApp::OnIdle(LONG lCount)
{	
	if (m_pMainWnd->IsIconic())
	{		
		return FALSE;
	}

	else
	{		
		//CWnd *pWnd = AfxGetMainWnd();
		//HWND CurrenthWnd = pWnd->m_hWnd;

		//HINSTANCE	CurInstance = AfxGetInstanceHandle();//m_hInstance;
		
		//if (CurrenthWnd == m_pMainWnd->m_hWnd)
		//if(CurInstance == m_hInstance)
		if(!GET_SINGLE(CEditorArchive)->GetAnimationDlgState())
		{
			GET_SINGLE(CCore)->Logic();
			if(GET_SINGLE(CEditorArchive)->GetTransformState())
				GET_SINGLE(CEditorArchive)->GetInspector()->CheckTransform();
			if (GET_SINGLE(CSceneManager)->GetSceneState() == SS_PLAY)
			{
				CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
				if (pScene->IsInitialized())
				{
					GET_SINGLE(CEditorArchive)->GetHierarchy()->SetSceneStartObject();
				}
				SAFE_RELEASE(pScene)
			}
		}
	}

	return TRUE;
}

int CToolApp::Run()
{
	return CWinApp::Run();
}



// CToolApp �޽��� ó����


// CreateObject
void CToolApp::OnCreateObject()
{
	GET_SINGLE(CEditorArchive)->GetHierarchy()->OnCreateObject();
}

void CToolApp::OnCreateUI()
{
	GET_SINGLE(CEditorArchive)->GetHierarchy()->OnCreateUI();
}



void CToolApp::OnAnimationDlg()
{
	CGameObject* pSelectedGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
	if (pSelectedGameObject)
	{
		CAnimator* pAnimator = pSelectedGameObject->GetComponent<CAnimator>();
		CMeshRenderer* pRenderer = pSelectedGameObject->GetComponent<CMeshRenderer>();
		if (pAnimator && pRenderer)
		{
			CMesh* pMesh = pRenderer->GetMesh();
			if (pMesh)
			{
				GET_SINGLE(CEditorArchive)->CreateAnimationDlg();
			}
		}
		SAFE_RELEASE(pAnimator);
		SAFE_RELEASE(pRenderer);
	}
}


void CToolApp::SceneSave()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CFileDialog	dlg(FALSE, L"SSSScene", L"Default", OFN_HIDEREADONLY,
		L"SSSScene(*.*)|*.*|AllFiles(*.*)|*.*||");

	if (dlg.DoModal() == IDOK)
	{
		CString	strFileName = dlg.GetPathName();

		char* pRelativePath = NULL;
		char* pPathKey = NULL;
		GET_SINGLE(CPathManager)->ExtractRelativeDirectoryFromFullPath(CT2CA(strFileName), &pRelativePath, &pPathKey);
		string temp(pRelativePath);
		wstring wstrRelativePath(temp.begin(), temp.end());

		CScene*	pCurrentScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
		GET_SINGLE(CSceneManager)->SaveScene(pCurrentScene, wstrRelativePath.c_str(), pPathKey);
		SAFE_RELEASE(pCurrentScene);
	}
}


void CToolApp::SceneLoad()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CFileDialog	dlg(TRUE, L"SSSScene", L"Default", OFN_HIDEREADONLY,
		L"SSSScene(*.*)|*.*|AllFiles(*.*)|*.*||");

	if (dlg.DoModal() == IDOK)
	{
		CString	strFileName = dlg.GetPathName();

		char* pRelativePath = NULL;
		char* pPathKey = NULL;
		GET_SINGLE(CPathManager)->ExtractRelativeDirectoryFromFullPath(CT2CA(strFileName), &pRelativePath, &pPathKey);
		string temp(pRelativePath);
		wstring wstrRelativePath(temp.begin(), temp.end());

		GET_SINGLE(CEditorArchive)->GetHierarchy()->ClearTreeItem();

		CScene*	pLoadScene = GET_SINGLE(CSceneManager)->CreateScene(wstrRelativePath.c_str(), pPathKey);
		if (pLoadScene)
		{
			GET_SINGLE(CEditorArchive)->RemoveGizmoControl();
			GET_SINGLE(CSceneManager)->ReleaseAllScene();
			GET_SINGLE(CSceneManager)->SetCurrentScene(pLoadScene);
			

			vector<CLayer*>*	pLoadLayers = pLoadScene->GetLayers();

			vector<CLayer*>::iterator	iter;
			vector<CLayer*>::iterator	iterEnd = pLoadLayers->end();

			for (iter = pLoadLayers->begin(); iter != iterEnd; ++iter)
			{
				list<CGameObject*>::const_iterator	Objiter;
				list<CGameObject*>::const_iterator	ObjiterEnd = (*iter)->GetAllGameObjects()->end();

				for (Objiter = (*iter)->GetAllGameObjects()->begin(); Objiter != ObjiterEnd; ++Objiter)
				{
					CTransform*	pObjTr = (*Objiter)->GetTransform();
					CTransform* pParentTr = pObjTr->GetParent();

					if (!pParentTr)
					{
						GET_SINGLE(CEditorArchive)->GetHierarchy()->LoadChildTreeItem(*Objiter);
					}

					SAFE_RELEASE(pObjTr);
				}
			}			
		}
		GET_SINGLE(CEditorArchive)->GetHierarchy()->OnInitialUpdate();
		SAFE_RELEASE(pLoadScene);
	}
}


BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.



	return CDialogEx::PreTranslateMessage(pMsg);
}


void CToolApp::SavePrefab()
{
	GET_SINGLE(CEditorArchive)->SavePrefab();
}


void CToolApp::LoadPrefab()
{
	GET_SINGLE(CEditorArchive)->LoadPrefab();
}

void CToolApp::EditLayer()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	GET_SINGLE(CEditorArchive)->CreateDlgSceneEdit();
}
