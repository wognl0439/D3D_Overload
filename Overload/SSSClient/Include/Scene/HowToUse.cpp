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
	

	//�̰� �Ȱ���
	CLayer* pUILayer = FindLayer("UI");
	CLayer* pLayer = FindLayer("Default");


	//�̰� ���� �ϴ���
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
//		//Prototype �� Clone ���� ����� ���� ����
//		CGameObject* pSampleObject = CGameObject::CreateObject("Object Name", pLayer);
//
//		//Transform ���� Move�Լ� ����.
//		//��ǥ�� ��ȯ���� ������ �Ϻη� �ȸ��������
//		//�� ������ �ȴٸ� �ǰ� �����.	
//		CTransform* pTransform = pSampleObject->GetTransform();
//		pTransform->SetWorldPosition(640.0f, 320.0f, 0.0f);
//		pTransform->SetWorldRotation(0.0f, 0.0f, 0.0f);
//		pTransform->SetWorldScale(91.5f, 160.0f, 1.0f);
//		pTransform->SetPivot(0.5f, 0.5f, 0.0f);
//		SAFE_RELEASE(pTransform);
//		
//		//������Ʈ �߰� 
//		//�̸� �� Ÿ�� �ߺ� ����. 
//		//GetComponent<T>()�� ã�ƿ� ������ AddComponent<T>() ������ ù ��° ������Ʈ�� ��ȯ�Ѵ�.
//		//GetComponents<T>()�� ã�ƿ� ������ AddComponent<T>() ������� vector�� �����Ͽ� ��ȯ�ȴ�.
//		CSpriteRenderer* pRenderer = pSampleObject->AddComponent<CSpriteRenderer>("Renderer Name");
//		pRenderer->SetMesh(ENGINE_MESH_ORTHORECT_TEX);
//		pRenderer->SetShader(STANDARD_TEXTURE_SHADER);
//		pRenderer->SetInputLayout(POS_UV_LAYOUT);
//		pRenderer->SetRenderState(ALPHA_BLEND);
//		pRenderer->SetRenderMode(RM_2D);
//
//		//�̹����� LT, RB ��ġ�� ���(���� �ؽ����� �ȼ�����) �ش� ������ ����ϰ� ���� �� ����Ϸ��� ������. UI���� �ַ� ���� ����
//		//string���� ���ڷ� �޴� �����ε� �Լ��� meta���Ͽ� ���� ������ �����ϰ�, �Ľ��ؼ� key�� ���� �� ����� ��.
//		//meta ������ ���ٸ� �ϳ��ϳ� �����. �װ� ������ �ϳ�
//		//ImageInfo�� ���������� ������ �ڵ����� �ؽ��� ��ü�� �����.
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
//		//Animator �� Animation Controller������ �־ ����� �� �ֵ��� �� ����. ���� Load / Save �ȸ���
//		//������ �ſ�ſ� ���� �ʿ��� �༮�̶� ����� ������ �ڵ尡 ������ �������Ƿ�, ���� ���� �ϵ��� ��.
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
