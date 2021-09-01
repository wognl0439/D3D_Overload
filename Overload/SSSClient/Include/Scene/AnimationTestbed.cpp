#include "AnimationTestbed.h"
#include "GameObject.h"
#include "Component.h"
#include "ResourcesManager.h"
#include "Layer.h"
#include "Component/Animator.h"
#include "AnimationClip.h"
#include "AnimationState.h"
#include "Transition.h"
#include "Component/MeshRenderer.h"
#include "Transform.h"
#include "Component/DirectionalLight.h"
#include "Component/Particle.h"

#include "DataManager.h"
#include "../Component/HierarchyTest.h"
#include "Component/Animator.h"
#include "AnimationState.h"
#include "AnimationClip.h"
#include "AnimationStateController.h"
#include "Component/Billboard.h"
#include "../Component/TestComponent.h"
#include "Material.h"

CAnimationTestbed::CAnimationTestbed()
{
}


CAnimationTestbed::~CAnimationTestbed()
{
}

bool CAnimationTestbed::Initialize()
{
	//GET_SINGLE(CDataManager)->LoadAtlasData(L"Test");

	CLayer* pLayer = FindLayer("Default");
	
	CTransform* pTransfer = NULL;
	//Billboard
	//{
	//	CGameObject* pGameObject = CGameObject::CreateObject("Body", pLayer);
	//	
	//	CBillboard* pBillboard = pGameObject->AddComponent<CBillboard>();
	//	SAFE_RELEASE(pBillboard);
	//
	//	CAnimator* pAnimator = pGameObject->AddComponent<CAnimator>();
	//	CAnimationController* pController = pAnimator->CreateAnimationController();
	//	CAnimationState* pState = pController->CreateAnimationState("DefaultState");
	//	CAnimationClip* pClip = pState->CreateEmptyClip("TestClip");
	//	pClip->SetKeyFrame(0, "Test00", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(1, "Test00", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(2, "Test00", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(3, "Test01", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(4, "Test01", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(5, "Test01", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(6, "Test02", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(7, "Test02", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(8, "Test02", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(9, "Test03", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(10, "Test03", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(11, "Test03", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(12, "Test04", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(13, "Test04", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(14, "Test04", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(15, "Test05", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(16, "Test05", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(17, "Test05", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(18, "Test06", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(19, "Test06", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(20, "Test06", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(21, "Test07", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(22, "Test07", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(23, "Test07", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(24, "Test08", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(25, "Test08", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(26, "Test08", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(27, "Test09", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(28, "Test09", "Atlas", L"SampleAtlas.png");
	//	pClip->SetKeyFrame(29, "Test09", "Atlas", L"SampleAtlas.png");
	//
	//	pClip->SetFrameRate(30);
	//	pController->SetEntryState("DefaultState");
	//
	//	SAFE_RELEASE(pAnimator);
	//	SAFE_RELEASE(pGameObject);
	//}
	
	{
		CGameObject* pTestMonster = CGameObject::CreateObject("Test", pLayer);
	
		CTransform* pTransform = pTestMonster->GetTransform();
		pTransform->SetWorldScale(0.05f, 0.05f, 0.05f);
		pTransform->SetWorldRotation(0, -PI / 2, 0);
		SAFE_RELEASE(pTransform);
	
		CMeshRenderer* pRenderer = pTestMonster->AddComponent<CMeshRenderer>();
		pRenderer->SetMesh("Monster4");
		pRenderer->SetShader(STANDARD_BUMP_SHADER);
		pRenderer->SetInputLayout(BUMP_ANIM_LAYOUT);
		
		CMaterial* pMaterial = pRenderer->GetMaterial();
		
		pMaterial->SetDiffuseTexture("DIF", L"Monster/CH_NPC_MOB_Behemoth_A01_D_KVN.tga");
		pMaterial->SetNormalTexture("NOR", L"Monster/CH_NPC_MOB_Behemoth_A01_N_KVN.tga");
		pMaterial->SetSpecularTexture("SPC", L"Monster/CH_NPC_MOB_Behemoth_A01_SP_KVN.tga");
		
		SAFE_RELEASE(pMaterial);
		
		//CMesh* pMesh = pRenderer->GetMesh();
		//pMesh->AddAnimationClip("Dead Body", 0, 1);
		//pMesh->AddAnimationClip("Run Front", 2, 41);
		//pMesh->AddAnimationClip("Attack01", 42, 131);
		//pMesh->AddAnimationClip("Attack02", 132, 208);
		//pMesh->AddAnimationClip("Damaged Bottom", 209, 278);
		//pMesh->AddAnimationClip("Damaged Front", 279, 348);
		//pMesh->AddAnimationClip("Idle01", 349, 448);
		//pMesh->AddAnimationClip("Down DeadBody", 449, 450);
		//pMesh->AddAnimationClip("Down Loop", 451, 510);
		//pMesh->AddAnimationClip("Get Up", 511, 570);
		//pMesh->AddAnimationClip("Hit Up", 571, 589);
		//pMesh->AddAnimationClip("Hit Loop", 590, 709);
		//pMesh->AddAnimationClip("Hit Down", 710, 721);
		//pMesh->AddAnimationClip("Hit Bounce", 722, 751);
		//pMesh->AddAnimationClip("Idle02", 752, 1171);
		//pMesh->AddAnimationClip("Passout", 1172, 1291);
		//pMesh->AddAnimationClip("Die", 1292, 1447);
		//pMesh->AddAnimationClip("Idle03", 1448, 1727);
		//pMesh->AddAnimationClip("Walk Front", 1728, 1827);
		//pMesh->AddAnimationClip("Down", 1828, 1858);
		//pMesh->AddAnimationClip("Fire", 1859, 1998);
		//pMesh->AddAnimationClip("Add Damage Bottom", 1999, 2068);
		//pMesh->AddAnimationClip("Add Damage Front", 2069, 2138);
		//
		//pMesh->Save(L"MonsterAnimationTest.SSSMesh", MESH_PATH);
		
		SAFE_RELEASE(pRenderer);
	
		CAnimator* pAnimator = pTestMonster->AddComponent<CAnimator>();
		pAnimator->CreateAnimationController();
		CAnimationStateController* pController = pAnimator->GetAnimationController();
		pController->AddParameter("Run", PT_BOOL);
		//pController->AddParameter("Idle", PT_INT);
		//pController->AddParameter("Attack1", PT_FLOAT);
		//pController->AddParameter("Attack2", PT_BOOL);
		//pController->AddParameter("Damaged", PT_BOOL);
		
		CAnimationState* pIdleState = pController->CreateAnimationState("Idle02", "Idle02", 752, 1171);
		//CAnimationState* pAttack1State = pController->CreateAnimationState("Attack1", "Attack1", 42, 131);
		//CAnimationState* pAttack2State = pController->CreateAnimationState("Attack2", "Attack2", 132, 209);
		//CAnimationState* pIdleState = pController->CreateAnimationState("Idle", "Idle", 210, 278);
		CAnimationState* pDamagedState = pController->CreateAnimationState("Damaged Front", "Damaged Front", 279, 348);
		pIdleState->Loop(false);
		pDamagedState->Loop(false);

		CTransition* pTransition = pIdleState->AddTransition(pDamagedState);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Run", PC_EQUALS, (bool)true);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);
			
		pTransition = pDamagedState->AddTransition(pIdleState);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Run", PC_EQUALS,(bool)false);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);
		
		pController->SetEntryState("Idle02");
		
		SAFE_RELEASE(pAnimator);
		CTestComponent* pTest = pTestMonster->AddComponent<CTestComponent>();
		SAFE_RELEASE(pTest);
	
		SAFE_RELEASE(pTestMonster);
	}

	//Parent
	//{
	//	CGameObject* pGameObject = CGameObject::CreateObject("Parent", pLayer);
	//	CTransform* pTransform = pGameObject->GetTransform();
	//	pTransfer = pTransform;
	//	SAFE_RELEASE(pTransform);
	//
	//	CMeshRenderer* pRenderer = pGameObject->AddComponent<CMeshRenderer>();
	//	pRenderer->SetMesh(PRIMITIVE_MESH_CUBE);
	//	//pRenderer->SetShader(STANDARD_COLOR_NORMAL_SHADER);
	//	//pRenderer->SetRenderState(ALPHA_BLEND);
	//	SAFE_RELEASE(pRenderer);
	//
	//	CHierarchyTest* pTestComponent = pGameObject->AddComponent<CHierarchyTest>();
	//	SAFE_RELEASE(pTestComponent);
	//
	//	SAFE_RELEASE(pGameObject);
	//}
  
	////Child
	//{
	//	CGameObject* pGameObject = CGameObject::CreateObject("Child", pLayer);
	//	CTransform* pTransform = pGameObject->GetTransform();
	//	pTransform->SetParent(pTransfer);
	//	pTransform->SetLocalRotation(1.0f, 1.25f, 0.5f);
	//	pTransform->SetLocalPosition(3.0f, 0.0f, 0.0f);
	//	pTransform->SetLocalScale(2.0f, 3.0f, 4.0f);
	//	SAFE_RELEASE(pTransform);
	//
	//	CMeshRenderer* pRenderer = pGameObject->AddComponent<CMeshRenderer>();
	//	pRenderer->SetMesh(PRIMITIVE_MESH_PYRAMID);
	//	//pRenderer->SetRenderState(ALPHA_BLEND);
	//	SAFE_RELEASE(pRenderer);
	//	SAFE_RELEASE(pGameObject);
	//}
   //
	
	

	//{
	//
	//	CGameObject* pParticleEmitter = CGameObject::CreateObject("Particle", pLayer);
	//	
	//	CParticle* pParticle = pParticleEmitter->AddComponent<CParticle>();		
	//	pParticle->SetTexture("particle", L"Particle_Black.png");
	//	pParticle->SetEmissionShape(PES_SPHERE);
	//	pParticle->SetRadius(5);
	//	pParticle->SetAlpha(0.0f, 1.0f);
	//	pParticle->SetLifetime(2.0f, 2.0f);
	//	pParticle->SetLoop(true);
	//	pParticle->SetMaxCount(100);
	//	pParticle->SetRateOverTime(10.0f);
	//	pParticle->SetSize(Vector2::Half, Vector2::One);
	//	pParticle->SetSpeed(1.0f, 2.0f);
	//	pParticle->SetRigidbody(false);
	//	pParticle->SetStartDelay(0);
	//	pParticle->SetTimeScale(1.0f);
	//
	//	SAFE_RELEASE(pParticle);
	//	SAFE_RELEASE(pParticleEmitter);
	//}

	
	
	{
		CGameObject* pLightObject = CGameObject::CreateObject("Light", pLayer);
	
		CTransform* pTransform = pLightObject->GetTransform();
		pTransform->SetWorldRotation(PI / 2.0f, 0.0f, 0.0f);
		SAFE_RELEASE(pTransform);
	
		CDirectionalLight* pLight = pLightObject->AddComponent<CDirectionalLight>("Light");
		SAFE_RELEASE(pLight);
		SAFE_RELEASE(pLightObject);
	}

	
	SAFE_RELEASE(pLayer);

	return true;
}
