#include "Core.h"
#include "Device.h"
#include "SceneManager.h"
#include "ResourcesManager.h"
#include "PathManager.h"
#include "RenderManager.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "Shader.h"
#include "TimerManager.h"
#include "Timer.h"
#include "Input.h"
#include "CollisionManager.h"
#include "FontManager.h"
#include "SoundManager.h"
#include "ThreadManager.h"
#include "DataManager.h"
#include "Mathf.h"
#include "NavigationManager.h"
#include "Component\Camera.h"

//Debug
#include "Scene.h"
#include "Debug.h"

SSS_BEGIN

DEFINITION_SINGLE(CCore)
bool CCore::m_bLoop = true;
bool CCore::m_bDebug = false;

wchar_t cWord[10] ;

CCore::CCore()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//_CrtSetBreakAlloc(1118);
	//_CrtSetBreakAlloc(530);

	memset(m_fClearColor, 0, sizeof(float) * 4);
	srand(time(0));

}

CCore::~CCore()
{
	DESTROY_SINGLE(CDebug);
	DESTROY_SINGLE(CSoundManager);
	DESTROY_SINGLE(CInput);
	DESTROY_SINGLE(CSceneManager);
	DESTROY_SINGLE(CTimerManager);
	DESTROY_SINGLE(CRenderManager);
	DESTROY_SINGLE(CNavigationManager);
	DESTROY_SINGLE(CPathManager);
	DESTROY_SINGLE(CResourcesManager);
	DESTROY_SINGLE(CDevice);
	DESTROY_SINGLE(CCollisionManager);
	DESTROY_SINGLE(CFontManager);
	DESTROY_SINGLE(CThreadManager);
	DESTROY_SINGLE(CDataManager);
	DESTROY_SINGLE(CMathf);





}

HINSTANCE CCore::GetWindowInstance() const
{
	return m_hInst;
}

HWND CCore::GetWindowHandle() const
{
	return m_hWnd;
}

RESOLUTION CCore::GetWindowResolution() const
{
	return m_tResolution;
}

void CCore::SetClearColor(float fColor[4])
{
	memcpy(m_fClearColor, fColor, sizeof(float) * 4);
}

void CCore::SetClearColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{
	m_fClearColor[0] = r / 255.0f;
	m_fClearColor[1] = g / 255.0f;
	m_fClearColor[2] = b / 255.0f;
	m_fClearColor[3] = a / 255.0f;
}

void CCore::SetRenderMode(RENDER_MODE eMode)
{
	GET_SINGLE(CRenderManager)->SetRenderMode(eMode);
}


void CCore::SetEngineState(bool bDebug)
{
	m_bDebug = bDebug;
}

bool CCore::GetEngineState() const
{
	return m_bDebug;
}

void CCore::ExitProcess()
{
	GET_SINGLE(CCore)->m_bLoop = false;
}

bool CCore::Init(HINSTANCE hInst, const RESOLUTION & tResolution, const wchar_t * pTitle, const wchar_t * pClass, int iIconID, int iSmallIconID, bool bWindowMode)
{
	m_hInst = hInst;
	m_tResolution = tResolution;

	MyRegisterClass(pClass, iIconID, iSmallIconID);
	Create(pTitle, pClass);

	return Init(m_hInst, m_hWnd, tResolution, bWindowMode);

}

bool CCore::Init(HINSTANCE hInst, HWND hWnd, const RESOLUTION& tResolution, bool bWindowMode)
{

	m_tResolution = tResolution;
	m_hInst = hInst;
	m_hWnd = hWnd;

	if (!GET_SINGLE(CDevice)->Initialize(m_hWnd, m_tResolution,	bWindowMode))
		return false;

	if (!GET_SINGLE(CThreadManager)->Initialize())
		return false;

	if (!GET_SINGLE(CSoundManager)->Initialize())
		return false;

	if (!GET_SINGLE(CFontManager)->Initialize())
		return false;

	if (!GET_SINGLE(CCollisionManager)->Initialize())
		return false;

	// 경로 관리자 초기화
	if (!GET_SINGLE(CPathManager)->Initialize(m_hWnd))
		return false;

	//타이머 관리자 초기화
	if (!GET_SINGLE(CTimerManager)->Initialize())
		return false;

	// 리소스 관리자 초기화
	if (!GET_SINGLE(CResourcesManager)->Initialize())
		return false;

	if (!GET_SINGLE(CDataManager)->Initialize())
		return false;

	// 렌더링 관리자 초기화
	if (!GET_SINGLE(CRenderManager)->Initialize())
		return false;

	if (!GET_SINGLE(CNavigationManager)->Init())
		return false;

	// 장면관리자 초기화
	if (!GET_SINGLE(CSceneManager)->Initialize())
		return false;

	if (!GET_SINGLE(CInput)->Initialize())
		return false;

	if (!GET_SINGLE(CDebug)->Initialize())
		return false;

	SetClearColor(255, 120, 120, 255);
	//SetClearColor(0xff, 0xC8, 0, 255); // SSS_YELLOW

	return true;
}

int CCore::Run()
{
	MSG msg;

	while (m_bLoop)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Logic();
		}
	}
	return 0;
}

void CCore::Logic()
{
	string strDebugScope = "Core::Logic";
	GET_SINGLE(CDebug)->StartReport(strDebugScope);

	CTimer* pTimer = GET_SINGLE(CTimerManager)->FindTimer("MainTimer");
	float fFixedTimeStep = GET_SINGLE(CCollisionManager)->GetFixedTimeStep();
	static float fCollisionUpdateTimer = 0;

	pTimer->Update();	
	float fDeltaTime = pTimer->GetDeltaTime();
	
	fCollisionUpdateTimer += fDeltaTime;

	GET_SINGLE(CInput)->Update();

	GET_SINGLE(CDebug)->CheckReport(strDebugScope, "UpdateInput");

	if (Update(fDeltaTime) == 1)
		return;

	GET_SINGLE(CDebug)->CheckReport(strDebugScope, "Update");

	if (LateUpdate(fDeltaTime) == 1)
		return;

	GET_SINGLE(CDebug)->CheckReport(strDebugScope, "LateUpdate");

	if (GET_SINGLE(CCollisionManager)->UpdateCollision(fDeltaTime) == 1)
		return;

	GET_SINGLE(CDebug)->CheckReport(strDebugScope, "CollisionUpdate");

	CScene* pCurrentScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
	CCamera* pCamera = pCurrentScene->GetMainCamera();
	pCamera->ComputeViewMatrix();
	Matrix tVP = pCamera->GetViewMatrix() * pCamera->GetPerspectiveProjectionMatrix();
	pCamera->CreateFrustum(tVP.Inverse());
	SAFE_RELEASE(pCamera);
	SAFE_RELEASE(pCurrentScene);


	Render(fDeltaTime);

	GET_SINGLE(CDebug)->CheckReport(strDebugScope, "Render");

	static float t = 0;
	t += pTimer->GetFixedDeltaTime();
	if (t >= 1)
	{
		t -= 1.0f;
		int iCount = 0;
		CScene* pCurrentScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
		iCount = pCurrentScene->GetColliderCount();
		SAFE_RELEASE(pCurrentScene);

		//char	strFPS[64] = {};
		//sprintf_s(strFPS, "FPS : %.2f \t\t Collider Count : %d \n", 1.0f / pTimer->GetFixedDeltaTime(), iCount);
		//_cprintf(strFPS);
		//SetWindowTextA(GET_SINGLE(CCore)->GetWindowHandle(), strFPS);
	}


#pragma region Change Scene Phase

	CScene* pNextScene = GET_SINGLE(CSceneManager)->GetNextScene();

	if (pNextScene)
	{
		SAFE_RELEASE(pNextScene);
		GET_SINGLE(CSceneManager)->ChangeScene();
		CScene* pCurrentScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
		SAFE_RELEASE(pCurrentScene);
	}

#pragma endregion

#pragma region Update Wheel State

	GET_SINGLE(CInput)->m_bReturn = false; // 엔터키 상태 확인


	GET_SINGLE(CDebug)->EndReport(strDebugScope);
#pragma endregion	
}


int CCore::Update(float fTime)
{
	return GET_SINGLE(CSceneManager)->Update(fTime);
}

int CCore::LateUpdate(float fTime)
{
	return GET_SINGLE(CSceneManager)->LateUpdate(fTime);
}

void CCore::Render(float fTime)
{
	GET_SINGLE(CDevice)->Clear(m_fClearColor, 1.f, 0);	
	GET_SINGLE(CSceneManager)->Render(fTime);
	GET_SINGLE(CRenderManager)->Render(fTime);
	GET_SINGLE(CRenderManager)->ClearLight();
	GET_SINGLE(CDevice)->Present();	
}

ATOM CCore::MyRegisterClass(const wchar_t * pClass, int iIconID, int iSmallIconID)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CCore::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInst;
	wcex.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(iIconID));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCEW(IDC_MY180103_1);
	wcex.lpszClassName = pClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(iSmallIconID));

	return RegisterClassExW(&wcex);
}

BOOL CCore::Create(const wchar_t * pTitle, const wchar_t * pClass)
{
	m_hWnd = CreateWindowW(pClass, pTitle,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, m_hInst, nullptr);

	if (!m_hWnd)
	{
		return FALSE;
	}

	// 실제 윈도우 타이틀바나 메뉴를 포함한 윈도우의 크기를
	// 구해준다.
	RECT	rc = { 0, 0, m_tResolution.iWidth, m_tResolution.iHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	// 위에서 구해준 크기로 윈도우 클라이언트 영역의 크기를 
	// 원하는 크기로 맞춰줘야 한다.
	SetWindowPos(m_hWnd, HWND_TOPMOST, 100, 100, rc.right - rc.left,
		rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return TRUE;
}

LRESULT CCore::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//GET_SINGLE(CInput)->ImeTextUpdate(hWnd, message, wParam, lParam);
	if (GET_SINGLE(CInput)->OnTextInput())
	{
		if (GET_SINGLE(CInput)->ImeTextUpdate(hWnd, message, wParam, lParam) == 0)
		{
			return 0;
		}
	}

	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;

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

	}
	
	break;
	case WM_DESTROY:
		m_bLoop = false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

SSS_END