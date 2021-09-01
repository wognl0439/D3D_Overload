
// ToolView.cpp : CToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "ToolDoc.h"
#include "ToolView.h"
#include "Core.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"
#include "EditScene.h"
#include "Input.h"
#include "EditorArchive.h"

SSS_USING

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include "GameObject.h"
#include "Component.h"
#include "ClientComponent/CameraMove.h"
#include "ClientComponent/PlayerController.h"
#include "ClientComponent/Splash.h"
#include "ClientComponent/Player.h"
#include "ClientComponent/PlayerCamera.h"
#include "ClientComponent\EffectController.h"
#include "ClientComponent/MonsterAI.h"
#include "ClientComponent/NPC_IDLE_AI.h"
#include "ClientComponent/GameSystem.h"
#include "ClientComponent/BoneDragonAI.h"
#include "ClientComponent/Portal.h"

bool LoadComponentFromExternalProject(CGameObject* pTarget, size_t iHash, FILE* pFile)
{
	if (iHash == typeid(CPlayerCamera).hash_code())
	{
		CPlayerCamera* pComponent = pTarget->AddComponent<CPlayerCamera>();
		if (pComponent)
		{
			pComponent->Load(pFile);
			SAFE_RELEASE(pComponent);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (iHash == typeid(CPlayer).hash_code())
	{
		CPlayer* pComponent = pTarget->AddComponent<CPlayer>();
		if (pComponent)
		{
			pComponent->Load(pFile);
			SAFE_RELEASE(pComponent);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (iHash == typeid(CGameSystem).hash_code())
	{
		CGameSystem* pComponent = pTarget->AddComponent<CGameSystem>();
		if (pComponent)
		{
			pComponent->Load(pFile);
			SAFE_RELEASE(pComponent);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (iHash == typeid(CSplash).hash_code())
	{
		CSplash* pComponent = pTarget->AddComponent<CSplash>();
		if (pComponent)
		{
			pComponent->Load(pFile);
			SAFE_RELEASE(pComponent);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (iHash == typeid(CCameraMove).hash_code())
	{
		CCameraMove* pComponent = pTarget->AddComponent<CCameraMove>();
		if (pComponent)
		{
			pComponent->Load(pFile);
			SAFE_RELEASE(pComponent);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (iHash == typeid(CPlayerController).hash_code())
	{
		CPlayerController* pComponent = pTarget->AddComponent<CPlayerController>();
		if (pComponent)
		{
			pComponent->Load(pFile);
			SAFE_RELEASE(pComponent);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (iHash == typeid(CMonsterAI).hash_code())
	{
		CMonsterAI* pComponent = pTarget->AddComponent<CMonsterAI>();
		if (pComponent)
		{
			pComponent->Load(pFile);
			SAFE_RELEASE(pComponent);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (iHash == typeid(CNPC_IDLE_AI).hash_code())
	{
		CNPC_IDLE_AI* pComponent = pTarget->AddComponent<CNPC_IDLE_AI>();
		if (pComponent)
		{
			pComponent->Load(pFile);
			SAFE_RELEASE(pComponent);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (iHash == typeid(CEffectController).hash_code())
	{
		CEffectController* pComponent = pTarget->AddComponent<CEffectController>();
		if (pComponent)
		{
			pComponent->Load(pFile);
			SAFE_RELEASE(pComponent);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (iHash == typeid(CBoneDragonAI).hash_code())
	{
		CBoneDragonAI* pComponent = pTarget->AddComponent<CBoneDragonAI>();
		if (pComponent)
		{
			pComponent->Load(pFile);
			SAFE_RELEASE(pComponent);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (iHash == typeid(CPortal).hash_code())
	{
		CPortal* pComponent = pTarget->AddComponent<CPortal>();
		if (pComponent)
		{
			pComponent->Load(pFile);
			SAFE_RELEASE(pComponent);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	return false;
}

// CToolView

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CToolView ����/�Ҹ�

CToolView::CToolView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	m_iSelectBGM = 0;
}

CToolView::~CToolView()
{
	DESTROY_SINGLE(CEditorArchive);
	DESTROY_SINGLE(CCore);
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	cs.cx = 1280;
	cs.cy = 720;

	return CView::PreCreateWindow(cs);
}

// CToolView �׸���

void CToolView::OnDraw(CDC* pDC)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CToolView �μ�

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CToolView ����

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView �޽��� ó����


void CToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CGameObject::SetClientComponentLoadCallbackFunction(LoadComponentFromExternalProject);
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	GET_SINGLE(CCore)->Init(AfxGetInstanceHandle(), m_hWnd, RESOLUTION(1280, 720), true);

	GET_SINGLE(CSceneManager)->Stop(true);
	GET_SINGLE(CCore)->SetRenderMode(RM_3D);
	GET_SINGLE(CCore)->SetEngineState(false);

	//CScene* pScene = GET_SINGLE(CSceneManager)->CreateScene(L"Release\\Scene\\Splash.SSSScene", DATA_PATH);
	CScene* pScene = GET_SINGLE(CSceneManager)->CreateScene<CEditScene>("EditScene");

	CLayer* pLayer = pScene->FindLayer("Default");
	pScene->SetCollisionMatrix(pLayer, pLayer, true);
	SAFE_RELEASE(pLayer);


	GET_SINGLE(CSceneManager)->SetCurrentScene(pScene);
	SAFE_RELEASE(pScene);

	//CScene*	pLoadScene = GET_SINGLE(CSceneManager)->CreateScene(L"Default.SSSScene");
	//GET_SINGLE(CSceneManager)->SetCurrentScene(pLoadScene);
	//SAFE_RELEASE(pLoadScene);
}


BOOL CToolView::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_MOVING:
	case WM_SIZE:
	case WM_SIZING:
	case WM_MOVE:
	{
		if (GET_SINGLE(CInput)->IsInitialized())
		{
			GET_SINGLE(CInput)->ResetResolution();
		}
	}
	case WM_KEYDOWN:
	{		
		if (pMsg->wParam == VK_TAB)
		{
			++m_iSelectBGM;
			if (m_iSelectBGM >= 3)
				m_iSelectBGM = 0;

			GET_SINGLE(CEditorArchive)->PlayMusic(SOUND_BGM, m_iSelectBGM);
		}
	}
		break;
	}

	return CView::PreTranslateMessage(pMsg);
}
