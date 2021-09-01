#include "Core.h"
#include "Client.h"
#include "resource.h"
#include "Scene\HowToUse.h"
#include "Scene\SampleScene.h"
#include "Scene\Testbed.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Scene\AnimationTestbed.h"
#include "Input.h"

SSS_USING

#include "GameObject.h"
#include "Component.h"
#include "Component/CameraMove.h"
#include "Component/PlayerController.h"
#include "Component/Splash.h"
#include "Component/Player.h"
#include "Component/PlayerCamera.h"
#include "Component\EffectController.h"
#include "Component/MonsterAI.h"
#include "Component/NPC_IDLE_AI.h"
#include "Component/GameSystem.h"
#include "Component/BoneDragonAI.h"
#include "Component/Portal.h"

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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{	

	CGameObject::SetClientComponentLoadCallbackFunction(LoadComponentFromExternalProject);

	if (!GET_SINGLE(CCore)->Init(hInstance, RESOLUTION(1280, 720),
		L"SSS", L"SSS", IDI_ICON2, IDI_ICON2, true))
	{
		DESTROY_SINGLE(CCore);
		return 0;
	}

	GET_SINGLE(CCore)->SetRenderMode(RM_3D);
	GET_SINGLE(CCore)->SetEngineState(false);

	GET_SINGLE(CInput)->SetCursorVisible(false);
	//CScene* pScene = GET_SINGLE(CSceneManager)->CreateScene<CHowToUse>("Template Scene");
	//CScene* pScene = GET_SINGLE(CSceneManager)->CreateScene<CSampleScene>("Scene");
	//CScene* pScene = GET_SINGLE(CSceneManager)->CreateScene<CTestbed>("Scene");

	//CScene* pScene = GET_SINGLE(CSceneManager)->CreateScene<CAnimationTestbed>("Scene");


	CScene* pScene = GET_SINGLE(CSceneManager)->CreateScene(L"Release\\Scene\\Splash.SSSScene", DATA_PATH);

	if (pScene)
	{
		GET_SINGLE(CSceneManager)->SetCurrentScene(pScene);
	}
	SAFE_RELEASE(pScene);

	
	int	iRet = GET_SINGLE(CCore)->Run();	
	DESTROY_SINGLE(CCore);
	
	return iRet;
}

