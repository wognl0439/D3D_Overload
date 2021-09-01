#include "Testbed.h"
#include "GameObject.h"
#include "Component.h"
#include "Layer.h"
#include "../Component/TestComponent.h"
#include "ResourcesManager.h"
#include "Transform.h"
#include "Component/MeshRenderer.h"
#include "Component/DirectionalLight.h"
#include "Mesh.h"
#include "Component/SpriteRenderer.h"
#include "DataManager.h"
#include "Device.h"
#include "ResourcesManager.h"
#include "Component/Animator.h"
#include "AnimationStateController.h"
#include "AnimationState.h"
#include "AnimationClip.h"
#include "Transition.h"

#include "Component/UICollider.h"



CTestbed::CTestbed()
{
}


CTestbed::~CTestbed()
{
}

bool CTestbed::Initialize()
{
	//GET_SINGLE(CDataManager)->InsertAtlasData("HUD_BottomPanel_Foreground", 0, 0, 3072, 320, 4096, 4096);
	//GET_SINGLE(CDataManager)->InsertAtlasData("HUD_BottomPanel_Background", 0, 320, 3072, 320, 4096, 4096);
	//GET_SINGLE(CDataManager)->SaveAtlasData(L"UI\\UI", TEXTURE_PATH);

	for (size_t y = 0; y < 8; ++y)
	{
		for (size_t x = 0; x < 8; ++x)
		{
			GET_SINGLE(CDataManager)->InsertAtlasData("Cell" + to_string(y * 8 + x), 256 * x, 256 * y, 256 , 256 , 2048, 2048);
		}
	}

	GET_SINGLE(CDataManager)->InsertAtlasData("Card", 0, 0, 450.0f, 671.0f, 450.0f, 671.0f);

	CLayer* pLayer = FindLayer("UI");

	//HUD
	{
		CGameObject* pHUD = CGameObject::CreateObject("HeadUpDisplay", pLayer);
		CTransform* pTransform = pHUD->GetTransform();
		pTransform->SetPivot(0.5f, 1.0f, 0.5f);
		pTransform->SetWorldScale(256, 256, 1.0f);
		pTransform->SetWorldPosition(DEVICE_RESOLUTION.iWidth / 2.0f, DEVICE_RESOLUTION.iHeight / 2.0f, 0.0f);
		SAFE_RELEASE(pTransform);
	
		CSpriteRenderer* pRenderer = pHUD->AddComponent<CSpriteRenderer>();
		pRenderer->SetShader(STANDARD_TEXTURE_SHADER);
		pRenderer->SetDiffuseTexture("Cell", L"Effects\\Cell_01.png");
		pRenderer->SetImageInfo("Cell2");

		CUICollider* pCollider = pHUD->AddComponent<CUICollider>();
		pCollider->SetLocalScale(Vector2(1.0f, 1.0f));
		SAFE_RELEASE(pCollider);
		

		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pHUD);
	}




	SAFE_RELEASE(pLayer);

	return true;
}
