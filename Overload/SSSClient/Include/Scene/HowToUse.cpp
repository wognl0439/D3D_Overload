#include "HowToUse.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Core.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Device.h"
#include "Material.h"
#include "Input.h"
#include "Component/SpriteRenderer.h"
#include "Component/Camera.h"
#include "Component/RectCollider2D.h"
#include "Component/Animator.h"
#include "Component/Text.h"
#include "AnimationState.h"
#include "AnimationStateController.h"
#include "Transition.h"
#include "ThreadManager.h"


#include "../Component/CameraMove.h"

#include "DataManager.h"
#include "../Component/Chat.h"
#include "Component/UICollider.h"
#include "Component/MeshRenderer.h"

#include "Component/DirectionalLight.h"
#include "Component/SpotLight.h"
#include "Component/PointLight.h"

#include "../Component/PlayerController.h"
#include "../Component/TestCameraMove.h"

CHowToUse::CHowToUse()
{
}


CHowToUse::~CHowToUse()
{
}

bool CHowToUse::Initialize()
{
	GET_SINGLE(CDataManager)->LoadAtlasData(L"Test");
	//Metadata Test
	//GET_SINGLE(CDataManager)->InsertAtlasData("Slice00", 0, 0, 10, 10, 128, 128); // LT
	//GET_SINGLE(CDataManager)->InsertAtlasData("Slice01", 10, 0, 42, 10, 128, 128); // T
	//GET_SINGLE(CDataManager)->InsertAtlasData("Slice02", 52, 0, 10, 10, 128, 128); // RT
	//GET_SINGLE(CDataManager)->InsertAtlasData("Slice03", 0, 10, 10, 42, 128, 128); // L
	//GET_SINGLE(CDataManager)->InsertAtlasData("Slice04", 10, 10, 42, 42, 128, 128); // C
	//GET_SINGLE(CDataManager)->InsertAtlasData("Slice05", 52, 10, 10, 42, 128, 128); // R
	//GET_SINGLE(CDataManager)->InsertAtlasData("Slice06", 0, 52, 10, 10, 128, 128); // LB
	//GET_SINGLE(CDataManager)->InsertAtlasData("Slice07", 10, 52, 42, 10, 128, 128); // B
	//GET_SINGLE(CDataManager)->InsertAtlasData("Slice08", 52, 52, 10, 10, 128, 128); // RB
	//GET_SINGLE(CDataManager)->SaveAtlasData(L"Test");
	

	//이건 똑같음
	CLayer* pUILayer = FindLayer("UI");
	CLayer* pLayer = FindLayer("Default");


	//이건 지금 하던거
	CGameObject* pText = CGameObject::CreateObject("text", pUILayer);
	CText* pTextComponent = pText->AddComponent<CText>("Text");
	pTextComponent->SetColor(Vector4::black);
	pTextComponent->SetRenderArea(0, 0, 600, 200);
	pTextComponent->SetText(L"");

	//GET_SINGLE(CInput)->SetEnableToInputText(pTextComponent);

	SAFE_RELEASE(pTextComponent);
	SAFE_RELEASE(pText);
	

	CGameObject* pChat = CGameObject::CreateObject("Chat Pannel", pUILayer);
	CTransform* pChatTransform = pChat->GetTransform();
	pChatTransform->SetWorldPosition(100.0f, 100.0f, 0.0f);
	SAFE_RELEASE(pChatTransform);
	
	CChat* pChatComponent = pChat->AddComponent<CChat>("Chat");
	SAFE_RELEASE(pChatComponent);
	
	CUICollider* pCollider = pChat->AddComponent<CUICollider>("Collider");
	//pCollider->SetScale(500.0f, 450.0f);
	//pCollider->SetPivot(0.0f, 0.0f);
	SAFE_RELEASE(pCollider);
	SAFE_RELEASE(pChat);



	{
		CGameObject* pObject = CGameObject::CreateObject("Player", pLayer);
		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldPosition(0.0f, 1.5f, 0.0f);
		pTransform->SetWorldScale(2.0f, 1.f, 2.0f);
		SAFE_RELEASE(pTransform);

		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		//pRenderer->SetMesh("Mountain");
		pRenderer->SetMesh(PRIMITIVE_MESH_PYRAMID);
		SAFE_RELEASE(pRenderer);

		//CMaterial* pMaterial = pObject->AddComponent<CMaterial>("Material");		
		//pMaterial->SetDiffuseTexture("Mountain", L"mountain_2.png");
		//pMaterial->SetSpecularColor(Vector4(1.0f, 1.0f, 1.0f, 3.0f));
		//SAFE_RELEASE(pMaterial);

		//CPlayerController* pController = pObject->AddComponent<CPlayerController>("Controller");
		//SAFE_RELEASE(pController);

		SAFE_RELEASE(pObject);
	}

	{
		CGameObject* pObject = CGameObject::CreateObject("Buildings", pLayer);
		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldPosition(0.0f, 0.0f, 0.0f);
		pTransform->SetWorldScale(1.0f, 1.0f, 1.0f);
		SAFE_RELEASE(pTransform);

		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		pRenderer->SetMesh(PRIMITIVE_MESH_PYRAMID);
		//pRenderer->SetMesh("Building");
		SAFE_RELEASE(pRenderer);

		//CMaterial* pMaterial = pObject->AddComponent<CMaterial>("Material");
		//pMaterial->SetDiffuseTexture("Mountain", L"mountain_2.png");
		//pMaterial->SetSpecularColor(Vector4(1.0f, 1.0f, 1.0f, 3.0f));
		//SAFE_RELEASE(pMaterial);


		//CPlayerController* pController = pObject->AddComponent<CPlayerController>("Controller");
		//SAFE_RELEASE(pController);

		SAFE_RELEASE(pObject);
	}





	{
		CGameObject* pObject = CGameObject::CreateObject("Light", pLayer);

		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldScale(0.1f, 0.1f, 0.1f);
		pTransform->SetWorldPosition(0.0f, 4.0f, 0.0f);
		pTransform->SetWorldRotation(PI / 2.0f, 0.0f, 0.0f);
		SAFE_RELEASE(pTransform);
		

		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		pRenderer->SetMesh(PRIMITIVE_MESH_PYRAMID);
		SAFE_RELEASE(pRenderer);

		//CMaterial* pMaterial = pObject->AddComponent<CMaterial>("Material");
		//SAFE_RELEASE(pMaterial);


		CTestCameraMove* pTemp = pObject->AddComponent<CTestCameraMove>("Mover");
		SAFE_RELEASE(pTemp);

		//CDirectionalLight* pLight = pObject->AddComponent<CDirectionalLight>("Light");
		//SAFE_RELEASE(pLight);
		CSpotLight* pLight = pObject->AddComponent<CSpotLight>("Light");
		pLight->SetRange(5);
		pLight->SetInAngle(PI / 18.0f);
		pLight->SetOutAngle(PI / 16.0f);
		SAFE_RELEASE(pLight);
		//CDirectionalLight* pLight = pObject->AddComponent<CDirectionalLight>("Light");
		//SAFE_RELEASE(pLight);

		SAFE_RELEASE(pObject);
	}

//#pragma region Object Sample
//	{
//		//Prototype 및 Clone 관련 기능은 추후 수정
//		CGameObject* pSampleObject = CGameObject::CreateObject("Object Name", pLayer);
//
//		//Transform 에는 Move함수 없음.
//		//좌표계 변환등의 이유로 일부러 안만들었으니
//		//꼭 만들어야 된다면 의견 듣겠음.	
//		CTransform* pTransform = pSampleObject->GetTransform();
//		pTransform->SetWorldPosition(640.0f, 320.0f, 0.0f);
//		pTransform->SetWorldRotation(0.0f, 0.0f, 0.0f);
//		pTransform->SetWorldScale(91.5f, 160.0f, 1.0f);
//		pTransform->SetPivot(0.5f, 0.5f, 0.0f);
//		SAFE_RELEASE(pTransform);
//		
//		//컴포넌트 추가 
//		//이름 및 타입 중복 가능. 
//		//GetComponent<T>()로 찾아올 때에는 AddComponent<T>() 순서상 첫 번째 컴포넌트를 반환한다.
//		//GetComponents<T>()로 찾아올 때에는 AddComponent<T>() 순서대로 vector에 삽입하여 반환된다.
//		CSpriteRenderer* pRenderer = pSampleObject->AddComponent<CSpriteRenderer>("Renderer Name");
//		pRenderer->SetMesh(ENGINE_MESH_ORTHORECT_TEX);
//		pRenderer->SetShader(STANDARD_TEXTURE_SHADER);
//		pRenderer->SetInputLayout(POS_UV_LAYOUT);
//		pRenderer->SetRenderState(ALPHA_BLEND);
//		pRenderer->SetRenderMode(RM_2D);
//
//		//이미지의 LT, RB 위치를 잡고(원본 텍스쳐의 픽셀단위) 해당 영역만 출력하고 싶을 때 사용하려고 만들어둠. UI에서 주로 쓰일 예정
//		//string만을 인자로 받는 오버로딩 함수는 meta파일에 위의 정보를 정의하고, 파싱해서 key값 생성 후 사용할 것.
//		//meta 파일이 없다면 하나하나 써야함. 그게 나머지 하나
//		//ImageInfo를 설정해주지 않으면 자동으로 텍스쳐 전체를 사용함.
//
//		//pRenderer->SetImageInfo("AtlasKey\\Key\\Key");
//		//pRenderer->SetImageInfo(0, 0, 100, 100, 4096, 4096, AT_CUSTOM);
//		CMaterial* pMaterial = pRenderer->CreateMaterial();
//		pMaterial->SetDiffuseTexture("SampleTexture", L"Sample01.png");
//		pMaterial->SetSampler(SAMPLER_LINEAR);
//		SAFE_RELEASE(pMaterial);
//		SAFE_RELEASE(pRenderer);
//		
//
//		SAFE_RELEASE(pSampleObject);
//	}
//#pragma endregion



//#pragma region Animation Sample
//	{
//		CGameObject* pSampleObject = CGameObject::CreateObject("Object Name", pLayer);
//
//		CTransform* pTransform = pSampleObject->GetTransform();
//		pTransform->SetWorldPosition(240.0f, 320.0f, 0.0f);
//		pTransform->SetWorldRotation(0.0f, 0.0f, 0.0f);
//		pTransform->SetWorldScale(100.0f, 100.0f, 1.0f);
//		pTransform->SetPivot(0.5f, 0.5f, 0.0f);
//		SAFE_RELEASE(pTransform);
//
//		CSpriteRenderer* pRenderer = pSampleObject->AddComponent<CSpriteRenderer>("Renderer Name");
//		pRenderer->SetMesh(ENGINE_MESH_ORTHORECT_TEX);
//
//		CMaterial* pMaterial = pSampleObject->AddComponent<CMaterial>("Material");
//		pMaterial->SetDiffuseTexture("SampleAtlas", L"SampleAtlas.png");
//		pMaterial->SetSampler(SAMPLER_LINEAR);
//		SAFE_RELEASE(pMaterial);
//		SAFE_RELEASE(pRenderer);
//
//		//Animator 에 Animation Controller파일을 넣어서 사용할 수 있도록 할 예정. 아직 Load / Save 안만듬
//		//설명이 매우매우 많이 필요한 녀석이라 여기다 적으면 코드가 저세상 가버리므로, 구두 전달 하도록 함.
//
//		CAnimator* pAnimator = pSampleObject->AddComponent<CAnimator>("Animator");
//		pAnimator->CreateAnimationController();
//		CAnimationState* pState = pAnimator->CreateAnimationStateInCurrentController("Running");
//		pState->AddClip("Running", AO_LOOP, 1.0f, "SampleAtlas", L"SampleAtlas.png");
//		pAnimator->SetEntryState("Running"); 
//
//		pState->GetClip()->AddCustomInfo("Test00");
//		pState->GetClip()->AddCustomInfo("Test01");
//		pState->GetClip()->AddCustomInfo("Test02");
//		pState->GetClip()->AddCustomInfo("Test03");
//		pState->GetClip()->AddCustomInfo("Test04");
//		pState->GetClip()->AddCustomInfo("Test05");
//		pState->GetClip()->AddCustomInfo("Test06");
//		pState->GetClip()->AddCustomInfo("Test07");
//		pState->GetClip()->AddCustomInfo("Test08");
//		pState->GetClip()->AddCustomInfo("Test09");
//
//
//		SAFE_RELEASE(pAnimator);
//
//
//
//		CRectCollider2D* pCollider = pSampleObject->AddComponent<CRectCollider2D>("Collider");
//		SAFE_RELEASE(pCollider);
//
//
//
//		SAFE_RELEASE(pSampleObject);
//	}
#pragma endregion




	CGameObject* pCamera = GetMainCameraObject();
	pCamera->SetScene(this);
	
	CTransform* pCameraTransform = pCamera->GetTransform();
	pCameraTransform->SetWorldPosition(0.0f, 0.0f, -5.0f);
	pCameraTransform->SetWorldRotation(-PI / 8.0f, 0.0f, 0.0f);
	SAFE_RELEASE(pCameraTransform);

	CCameraMove* pCameraMove = pCamera->AddComponent<CCameraMove>("Move");
	SAFE_RELEASE(pCameraMove);
	SAFE_RELEASE(pCamera);

	SAFE_RELEASE(pUILayer);
	SAFE_RELEASE(pLayer);
	return true;
}
