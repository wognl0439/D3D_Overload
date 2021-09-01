#include "SampleScene.h"
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
#include "Component/SphereCollider.h"

#include "../Component/PlayerController.h"
#include "CollisionManager.h"
#include "Component/PostProcessing.h"
#include "../Component/TestComponent.h"
#include "Component/CardinalSpline.h"
#include "Component/Outline.h"
#include "Component/Terrain.h"
#include "Component/BoxCollider.h"

#include "Component/Gizmo.h"
#include "Component/Billboard.h"
#include "Component/Fire.h"
#include "Component/Water.h"
#include "Component/Trail.h"
#include "Component/Distortion.h"
#include "Component/Decal.h"

#include "Component/Animator.h"
#include "AnimationState.h"
#include "AnimationClip.h"
#include "AnimationStateController.h"
#include "../Component/TestComponent.h"
#include "Component/Navigation3D.h"
#include "PathManager.h"

CSampleScene::CSampleScene()
{
}


CSampleScene::~CSampleScene()
{
}

bool CSampleScene::Initialize()
{
	CLayer* pLayer = FindLayer("Default");
	SetCollisionMatrix(pLayer, pLayer, true);

	//Spline Object
	{
		CGameObject* pSpline = CGameObject::CreateObject("Spline", pLayer);
		CCardinalSpline* pComponent = pSpline->AddComponent<CCardinalSpline>();
		pComponent->AddPoint(-100.0f, 10.0f, -100.0f); 
		pComponent->AddPoint(-90.0f, 5.0f, -110.0f);
		pComponent->AddPoint(-50.0f, 10.0f, -120.0f);
		pComponent->AddPoint(-90.0f, 16.0f, -120.0f);
		pComponent->AddPoint(-50.0f, 30.0f, -80.0f);
		pComponent->AddPoint(Vector3::Zero); // Dummy
		pComponent->SetTension(1.0f);
		pComponent->SetSegment(30);
		pComponent->SetColor(Vector4::red);

		SAFE_RELEASE(pComponent);
		SAFE_RELEASE(pSpline);
	}

	{
		CLayer* pUILayer = FindLayer("UI");
		CGameObject* pText = CGameObject::CreateObject("text", pUILayer);
		CTransform* pTransform = pText->GetTransform();
		pTransform->SetWorldPosition(300.0f, 50.0f, 0.0f);
		SAFE_RELEASE(pTransform);

		CText* pTextComponent = pText->AddComponent<CText>("Text");
		pTextComponent->SetColor(Vector4::black);
		pTextComponent->SetRenderArea(0, 0, 600, 200);
		pTextComponent->SetText(L"");

		//GET_SINGLE(CInput)->SetEnableToInputText(pTextComponent);

		//CChat* pChat = pText->AddComponent<CChat>();
		//SAFE_RELEASE(pChat);

		SAFE_RELEASE(pTextComponent);
		SAFE_RELEASE(pText);
		SAFE_RELEASE(pUILayer);
	}


	//{
	//	CGameObject* pObject = CGameObject::CreateObject("Block", pLayer);
	//	CTransform* pTransform = pObject->GetTransform();
	//	pTransform->SetWorldPosition(-100.0f, 10.0f, -100.0f);
	//	pTransform->SetWorldScale(100.0f, 100.0f, 100.0f);
	//	pTransform->SetWorldRotation(0.0f, 0.0f, PI / 2.0f);

	//	SAFE_RELEASE(pTransform);

	//	CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
	//	pRenderer->SetMesh(PRIMITIVE_MESH_PYRAMID);

	//	//변경
	//	//CMaterial* pMaterial = pObject->AddComponent<CMaterial>("Material");
	//	//CMaterial* pMaterial = pRenderer->GetMaterial();		
	//	//pMaterial->SetDiffuseTexture("Mountain", L"mountain_2.png");
	//	//pMaterial->SetDiffuseColor(Vector4(0x0B / 255.0f, 0x0B / 255.0f, 0x3B / 255.0f, 1.0f));
	//	//pMaterial->SetSpecularColor(Vector4(1.0f, 1.0f, 1.0f, 3.0f));
	//	//pMaterial->SetSampler(SAMPLER_LINEAR);
	//	//SAFE_RELEASE(pMaterial);
	//	SAFE_RELEASE(pRenderer);
	//	SAFE_RELEASE(pObject);
	//}


	/*{
		CGameObject* pObject = CGameObject::CreateObject("Mountain", pLayer);
		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldPosition(0.0f, -0.5f, 0.0f);
		pTransform->SetWorldScale(10.0f, 10.0f, 10.0f);
		SAFE_RELEASE(pTransform);
	
		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		pRenderer->SetMesh("Mountain");
		pRenderer->SetShadow(true);

		CMaterial* pMaterial = pRenderer->GetMaterial();
		pMaterial->SetDiffuseTexture("Mountain", L"mountain_2.png");
		pMaterial->SetSpecularColor(Vector4(1.0f, 1.0f, 1.0f, 3.0f));
		SAFE_RELEASE(pMaterial);
		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pObject);
	}
	
	{
		CGameObject* pObject = CGameObject::CreateObject("Buildings", pLayer);
		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldPosition(0.0f, -0.5f, 0.0f);
		pTransform->SetWorldScale(10.0f, 10.0f, 10.0f);
		SAFE_RELEASE(pTransform);
	
		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		pRenderer->SetMesh("Building");
		pRenderer->SetShadow(true);

		CMaterial* pMaterial = pRenderer->GetMaterial();
		pMaterial->SetDiffuseTexture("Mountain", L"mountain_2.png");
		pMaterial->SetSpecularColor(Vector4(1.0f, 1.0f, 1.0f, 3.0f));
		SAFE_RELEASE(pMaterial);
		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pObject);
	}*/

	////Terrain
	//CGameObject* TerrainObj = CGameObject::CreateObject("Terrain", pLayer);
	//CTerrain* pTerrain = TerrainObj->AddComponent<CTerrain>("Terrain");
	//pTerrain->LoadTerrainData(L"Terrain\\test2.Terrain", L"Terrain\\test2.SSSMesh");
	//SAFE_RELEASE(pTerrain);
	//SAFE_RELEASE(TerrainObj);

	// Terrain
	//{
	//	CGameObject* pTerrain = CGameObject::CreateObject("ASDF", pLayer);
	//	CTerrain* pTerrainComponent = pTerrain->AddComponent<CTerrain>("Terrain");
	//	//pTerrainComponent->SetSplatCount(4);

	//	//pTerrainComponent->LoadTerrainData(L"Terrain/test.Terrain", L"Terrain/test.SSSMesh");
	//	pTerrainComponent->LoadHeightMap("Default Height", 30.f, "Terrain/Default Height.bmp");

	//	pTerrainComponent->SetBaseDiffuseTexture("TerrainBaseTex", L"Terrain/BD_Terrain_Cliff05.dds");
	//	pTerrainComponent->SetBaseNormalTexture("TerrainBaseNrmTex", L"Terrain/BD_Terrain_Cliff05_NRM.bmp");
	//	pTerrainComponent->SetBaseSpecularTexture("TerrainBaseTex", L"Terrain/BD_Terrain_Cliff05.dds");

	//	vector<wstring>	vecMultiDif;
	//	vector<wstring>	vecMultiNrm;
	//	vector<wstring>	vecMultiSpc;
	//	vector<wstring>	vecMultiAlpha;

	//	vecMultiDif.push_back(L"Terrain/Terrain_Cliff_15_Large.dds");
	//	vecMultiDif.push_back(L"Terrain/BD_Terrain_Cliff05.dds");
	//	vecMultiDif.push_back(L"Terrain/Terrain_Cliff_11.dds");
	//	vecMultiDif.push_back(L"Terrain/Terrain_Pebbles_01.dds");

	//	vecMultiNrm.push_back(L"Terrain/Terrain_Cliff_15_Large_NRM.bmp");
	//	vecMultiNrm.push_back(L"Terrain/BD_Terrain_Cliff05_NRM.bmp");
	//	vecMultiNrm.push_back(L"Terrain/Terrain_Cliff_11_NRM.bmp");
	//	vecMultiNrm.push_back(L"Terrain/Terrain_Pebbles_01_NRM.bmp");

	//	vecMultiSpc.push_back(L"Terrain/Terrain_Cliff_15_Large_SPEC.bmp");
	//	vecMultiSpc.push_back(L"Terrain/BD_Terrain_Cliff05_SPEC.bmp");
	//	vecMultiSpc.push_back(L"Terrain/Terrain_Cliff_11_SPEC.bmp");
	//	vecMultiSpc.push_back(L"Terrain/Terrain_Pebbles_01_SPEC.bmp");

	//	vecMultiAlpha.push_back(L"Terrain/asd.bmp");
	//	vecMultiAlpha.push_back(L"Terrain/SandBaseAlpha.bmp");
	//	vecMultiAlpha.push_back(L"Terrain/WaterBaseAlpha.bmp");
	//	vecMultiAlpha.push_back(L"Terrain/FlowerAlpha.bmp");

	//	pTerrainComponent->SetSampler();

	//	pTerrainComponent->AddSplatDiffuseTexture("Splat", "SplatDif", vecMultiDif);
	//	pTerrainComponent->AddSplatNormalTexture("Splat", "SplatNrm", vecMultiNrm);
	//	pTerrainComponent->AddSplatSpecularTexture("Splat", "SplatSpc", vecMultiSpc);
	//	pTerrainComponent->AddSplatAlphaTexture("SplatAlpha", "SplatAlpha", vecMultiAlpha);

	//	// SAVE 용
	//	pTerrainComponent->SaveTerrainData(L"Terrain/Test181111.Terrain", L"Terrain/Test181111.Terrain");
	//	/*wstring strFullPathW = GET_SINGLE(CPathManager)->FindPath(DATA_PATH);
	//	strFullPathW += L"MainTower.MeshRenderer";
	//	string strFullPathA(strFullPathW.begin(), strFullPathW.end());
	//	FILE* pFile = NULL;*/
	//	//fopen_s(&pFile, strFullPathA.c_str(), "wb");
	//	//if (pFile)
	//	//{
	//	//	//pTerrainComponent->SaveTerrainData(pFile);
	//	//	fclose(pFile);
	//	//}

	//	SAFE_RELEASE(pTerrainComponent);
	//	SAFE_RELEASE(pTerrain);
	//}

	// Decal
	{
		CGameObject* pDecalObject = CGameObject::CreateObject("Decal", pLayer);

		CTransform*	pDecalTr = pDecalObject->GetTransform();
		pDecalTr->SetWorldPosition(5.f, 0.f, 5.f);
		pDecalTr->SetWorldScale(10.f, 10.f, 10.f);
		SAFE_RELEASE(pDecalTr);

		CDecal*	pDecal = pDecalObject->AddComponent<CDecal>("Decal");

		CMeshRenderer* pMeshRenderer = pDecalObject->GetComponent<CMeshRenderer>();

		CMaterial*	pDecalMtrl = pMeshRenderer->GetMaterial();

		pDecalMtrl->SetDiffuseTexture("Decal", L"Decal/Decal.png");
		pDecalMtrl->SetDiffuseTextureRegister(0);

		pDecalMtrl->SetNormalTexture("DecalNrm", L"Decal/Decal_NRM.png");
		pDecalMtrl->SetNormalTextureRegister(1);

		pDecalMtrl->SetSpecularTexture("DecalSpc", L"Decal/Decal_SPEC.png");
		pDecalMtrl->SetSpecularTextureRegister(2);

		pDecalMtrl->SetSampler(SAMPLER_LINEAR);
		pDecalMtrl->SetSamplerRegister(0);

		SAFE_RELEASE(pDecalMtrl);

		SAFE_RELEASE(pMeshRenderer);
		
		SAFE_RELEASE(pDecal);

		SAFE_RELEASE(pDecalObject);
	}
/*
	{
		CGameObject* pObject = CGameObject::CreateObject("Static_Lake_Ferry", pLayer);
		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldPosition(0.0f, -30.0f, 120.0f);
		pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
		SAFE_RELEASE(pTransform);
		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		pRenderer->SetMesh("Static_Lake_Ferry");
		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pObject);
	}
	{
		CGameObject* pObject = CGameObject::CreateObject("Static_Lake_WoodBridge", pLayer);
		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldPosition(-100.0f, 0.0f, 100.0f);
		pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
		SAFE_RELEASE(pTransform);
		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		pRenderer->SetMesh("Static_Lake_WoodBridge");
		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pObject);
	}
	{
		CGameObject* pObject = CGameObject::CreateObject("Static_LakeStone01", pLayer);
		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldPosition(-100.0f, 0.0f, 80.0f);
		pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
		SAFE_RELEASE(pTransform);
		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		pRenderer->SetMesh("Static_LakeStone01");
		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pObject);
	}
	{
		CGameObject* pObject = CGameObject::CreateObject("Static_LakeStone02", pLayer);
		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldPosition(-100.0f, 0.0f, 60.0f);
		pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
		SAFE_RELEASE(pTransform);
		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		pRenderer->SetMesh("Static_LakeStone02");
		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pObject);
	}
	{
		CGameObject* pObject = CGameObject::CreateObject("Static_LakeStone03", pLayer);
		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldPosition(-100.0f, 0.0f, 40.0f);
		pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
		SAFE_RELEASE(pTransform);
		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		pRenderer->SetMesh("Static_LakeStone03");
		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pObject);
	}
	{
		CGameObject* pObject = CGameObject::CreateObject("Static_LakeStone04", pLayer);
		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldPosition(-100.0f, 0.0f, 20.0f);
		pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
		SAFE_RELEASE(pTransform);
		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		pRenderer->SetMesh("Static_LakeStone04");
		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pObject);
	}
	{
		CGameObject* pObject = CGameObject::CreateObject("Static_LakeStone05", pLayer);
		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldPosition(-100.0f, 0.0f, 0.0f);
		pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
		SAFE_RELEASE(pTransform);
		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		pRenderer->SetMesh("Static_LakeStone05");
		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pObject);
	}
*/
	//{
	//	CGameObject* pObject = CGameObject::CreateObject("Bridge", pLayer);
	//	CTransform* pTransform = pObject->GetTransform();
	//	pTransform->SetWorldPosition(-100.0f, 0.0f, 10.0f);
	//	pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
	//	SAFE_RELEASE(pTransform);
	//	//CBillboard* pBillboard = pObject->AddComponent<CBillboard>();
	//	//SAFE_RELEASE(pBillboard);
	//	CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
	//	pRenderer->SetMesh("Bridge");
	//	SAFE_RELEASE(pRenderer);
	//	SAFE_RELEASE(pObject);
	//}

	////
	//{
	//	CGameObject* pObject = CGameObject::CreateObject("Market_Book", pLayer);
	//	CTransform* pTransform = pObject->GetTransform();
	//	pTransform->SetWorldPosition(-100.0f, 0.0f, 20.0f);
	//	pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
	//	SAFE_RELEASE(pTransform);
	//	CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
	//	pRenderer->SetMesh("Market_Book");
	//	pRenderer->SetShader(STANDARD_BUMP_SHADER);
	//	pRenderer->SetInputLayout(BUMP_LAYOUT);
	//	SAFE_RELEASE(pRenderer);
	//	SAFE_RELEASE(pObject);
	//}
	//{
	//	CGameObject* pObject = CGameObject::CreateObject("Market_Box", pLayer);
	//	CTransform* pTransform = pObject->GetTransform();
	//	pTransform->SetWorldPosition(-100.0f, 0.0f, 30.0f);
	//	pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
	//	SAFE_RELEASE(pTransform);
	//	CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
	//	pRenderer->SetMesh("Market_Box");
	//	pRenderer->SetShader(STANDARD_BUMP_SHADER);
	//	pRenderer->SetInputLayout(BUMP_LAYOUT);
	//	SAFE_RELEASE(pRenderer);
	//	SAFE_RELEASE(pObject);
	//}
	//{
	//	CGameObject* pObject = CGameObject::CreateObject("Market_Bread", pLayer);
	//	CTransform* pTransform = pObject->GetTransform();
	//	pTransform->SetWorldPosition(-100.0f, 0.0f, 40.0f);
	//	pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
	//	SAFE_RELEASE(pTransform);
	//	CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
	//	pRenderer->SetMesh("Market_Bread");
	//	pRenderer->SetShader(STANDARD_BUMP_SHADER);
	//	pRenderer->SetInputLayout(BUMP_LAYOUT);
	//	SAFE_RELEASE(pRenderer);
	//	SAFE_RELEASE(pObject);
	//}
	/*{
		CGameObject* pObject = CGameObject::CreateObject("weagon", pLayer);
		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldPosition(-100.0f, 0.0f, 50.0f);
		pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
		SAFE_RELEASE(pTransform);
		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		pRenderer->SetMesh("weagon");
		pRenderer->SetShader(STANDARD_BUMP_SHADER);
		pRenderer->SetInputLayout(BUMP_LAYOUT);
		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pObject);
	}*/
	//{
	//	CGameObject* pObject = CGameObject::CreateObject("Static_Siraka_House", pLayer);
	//	CTransform* pTransform = pObject->GetTransform();
	//	pTransform->SetWorldPosition(-100.0f, 0.0f, 50.0f);
	//	pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
	//	SAFE_RELEASE(pTransform);
	//	CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
	//	pRenderer->SetMesh("Static_Siraka_House");
	//	pRenderer->SetShader(STANDARD_BUMP_SHADER);
	//	pRenderer->SetInputLayout(BUMP_LAYOUT);
	//	// SAVE 용
	//	wstring strFullPathW = GET_SINGLE(CPathManager)->FindPath(DATA_PATH);
	//	strFullPathW += L"Static_Siraka_House.MeshRenderer";
	//	string strFullPathA(strFullPathW.begin(), strFullPathW.end());
	//	FILE* pFile = NULL;
	//	fopen_s(&pFile, strFullPathA.c_str(), "wb");
	//	if (pFile)
	//	{
	//		pRenderer->Save(pFile);
	//		fclose(pFile);
	//	}
	//	SAFE_RELEASE(pRenderer);
	//	SAFE_RELEASE(pObject);
	//}
	//

	//{
	//	CGameObject* pObject = CGameObject::CreateObject("Whale", pLayer);
	//	CTransform* pTransform = pObject->GetTransform();
	//	pTransform->SetWorldPosition(-100.0f, 0.0f, 50.0f);
	//	pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
	//	SAFE_RELEASE(pTransform);
	//	CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
	//	pRenderer->SetMesh("Whale");
	//	pRenderer->SetShader(STANDARD_BUMP_SHADER);
	//	pRenderer->SetInputLayout(BUMP_LAYOUT);
	//	// SAVE 용
	//	wstring strFullPathW = GET_SINGLE(CPathManager)->FindPath(DATA_PATH);
	//	strFullPathW += L"Whale.MeshRenderer";
	//	string strFullPathA(strFullPathW.begin(), strFullPathW.end());
	//	FILE* pFile = NULL;
	//	fopen_s(&pFile, strFullPathA.c_str(), "wb");
	//	if (pFile)
	//	{
	//		pRenderer->Save(pFile);
	//		fclose(pFile);
	//	}
	//	SAFE_RELEASE(pRenderer);
	//	SAFE_RELEASE(pObject);
	//}
	
	//{
	//	CGameObject* pObject = CGameObject::CreateObject("MainTower", pLayer);
	//	CTransform* pTransform = pObject->GetTransform();
	//	pTransform->SetWorldPosition(-100.0f, 0.0f, 50.0f);
	//	pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
	//	SAFE_RELEASE(pTransform);
	//	CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
	//	pRenderer->SetMesh("MainTower");
	//	pRenderer->SetShader(STANDARD_BUMP_SHADER);
	//	pRenderer->SetInputLayout(BUMP_LAYOUT);
	//	// SAVE 용
	//	wstring strFullPathW = GET_SINGLE(CPathManager)->FindPath(DATA_PATH);
	//	strFullPathW += L"MainTower.MeshRenderer";
	//	string strFullPathA(strFullPathW.begin(), strFullPathW.end());
	//	FILE* pFile = NULL;
	//	fopen_s(&pFile, strFullPathA.c_str(), "wb");
	//	if (pFile)
	//	{
	//		pRenderer->Save(pFile);
	//		fclose(pFile);
	//	}
	//	SAFE_RELEASE(pRenderer);
	//	SAFE_RELEASE(pObject);
	//}

	/*for (int i = 0; i < 1; ++i)
	{
		CGameObject* pObject = CGameObject::CreateObject("Static_Tree", pLayer);
		pObject->SetTransparency(true);
		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldPosition(-100.0f + (i * 5.f), 0.0f, 50.0f);
		pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
		SAFE_RELEASE(pTransform);
		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		pRenderer->SetMesh("Static_Tree");
		pRenderer->SetShader("TreeTextureShader");
		pRenderer->SetInputLayout(POS_UV_NORMAL_LAYOUT);
		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pObject);
	}*//*
for (int i = 0; i < 1; ++i)
{
	CGameObject* pObject = CGameObject::CreateObject("New_Tree", pLayer);
	CTransform* pTransform = pObject->GetTransform();
	pTransform->SetWorldPosition(-100.0f + (i * 5.f), 0.0f, 50.0f);
	pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
	SAFE_RELEASE(pTransform);
	CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
	pRenderer->SetMesh("New_Tree");
	SAFE_RELEASE(pRenderer);
	SAFE_RELEASE(pObject);
}*/
//
//for (int i = 0; i < 1; ++i)
//{
//CGameObject* pObject = CGameObject::CreateObject("New_Tree", pLayer);
//pObject->SetTransparency(true);
//CTransform* pTransform = pObject->GetTransform();
//pTransform->SetWorldPosition(-100.0f + (i * 5.f), 0.0f, 50.0f);
//pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
//SAFE_RELEASE(pTransform);
//CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
//pRenderer->SetMesh("New_Tree");
//pRenderer->SetShader("TreeTextureShader");
//pRenderer->SetInputLayout(POS_UV_NORMAL_LAYOUT);
//SAFE_RELEASE(pRenderer);
//SAFE_RELEASE(pObject);
//}

	/*{
		CGameObject* pObject = CGameObject::CreateObject("TEst_asd", pLayer);
		CTransform* pTransform = pObject->GetTransform();
		pTransform->SetWorldPosition(-100.0f, 0.0f, 50.0f);
		pTransform->SetWorldScale(0.50f, 0.50f, 0.50f);
		SAFE_RELEASE(pTransform);
		CMeshRenderer* pRenderer = pObject->AddComponent<CMeshRenderer>("Renderer");
		pRenderer->SetMesh("TEst_asd");
		pRenderer->SetShader(STANDARD_BUMP_SHADER);
		pRenderer->SetInputLayout(BUMP_LAYOUT);
		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pObject);
	}*/

	CGameObject* pTransmitter = NULL;
	{
		CGameObject* pPlayer = CGameObject::CreateObject("Player", pLayer);
		pTransmitter = pPlayer;

		CTransform* pTransform = pPlayer->GetTransform();
		pTransform->SetWorldPosition(-100.0f, 10.0f, -100.0f);
		pTransform->SetWorldScale(0.3f, 0.3f, 0.3f);
		pTransform->SetWorldRotationY(PI / 2.f);
		SAFE_RELEASE(pTransform);

		CMeshRenderer* pRenderer = pPlayer->AddComponent<CMeshRenderer>("Renderer");
		//pRenderer->SetShadow(true);
		pRenderer->SetMesh("Monster");
		pRenderer->SetShader(STANDARD_BUMP_SHADER);
		pRenderer->SetInputLayout(BUMP_ANIM_LAYOUT);

		// LOAD 용
		wstring strFullPathW = GET_SINGLE(CPathManager)->FindPath(DATA_PATH);
		strFullPathW += L"Chinuwa.MeshRenderer";
		string strFullPathA(strFullPathW.begin(), strFullPathW.end());
		FILE* pFile = NULL;
		fopen_s(&pFile, strFullPathA.c_str(), "rb");
		if (pFile)
		{
			pRenderer->Load(pFile);
			fclose(pFile);
		}

		SAFE_RELEASE(pRenderer);

		CPlayerController* pPlayerController = pPlayer->AddComponent<CPlayerController>("Controller");
		SAFE_RELEASE(pPlayerController);

		CNavigation3D* pNavi3D = pPlayer->AddComponent<CNavigation3D>("Navi3D");
		SAFE_RELEASE(pNavi3D);
		/*CBurnFX* pBurn = pPlayer->AddComponent<CBurnFX>("Burn");
		pBurn->SetBorderColor(Vector4::green);
		pBurn->SetBorderRangePercentage(0.1f);
		SAFE_RELEASE(pBurn);*/

		 
		//CSphereCollider* pCollider = pPlayer->AddComponent<CSphereCollider>();
		//pCollider->SetRadius(10.0f);
		//SAFE_RELEASE(pCollider);

		//CBoxCollider* pCollider = pPlayer->AddComponent<CBoxCollider>();
		//pCollider->SetVolume(Vector3(10.0f, 0.0f, 10.0f));
		//SAFE_RELEASE(pCollider);

		/*COutLine* pOutline = pPlayer->AddComponent<COutLine>();
		pOutline->SetOutlineColor(Vector4::white);
		SAFE_RELEASE(pOutline);*/


		CAnimator* pAnimator = pPlayer->AddComponent<CAnimator>();
		pAnimator->CreateAnimationController();
		CAnimationStateController* pController = pAnimator->GetAnimationController();
		pController->AddParameter("Run", PT_BOOL);
		pController->AddParameter("Attack", PT_BOOL);
		pController->AddParameter("Attack2", PT_BOOL);
		pController->AddParameter("Skill1", PT_BOOL);
		pController->AddParameter("Skill2", PT_BOOL);

		CAnimationState* pSitState = pController->CreateAnimationState("Sitting", "Sitting", 0, 299);
		CAnimationState* pTalk1 = pController->CreateAnimationState("talk1", "talk1", 300, 1105);
		CAnimationState* pTalk2 = pController->CreateAnimationState("talk2", "talk2", 1106, 1911);
		CAnimationState* pIdleState = pController->CreateAnimationState("Idle1", "Idle1", 1912, 2224);
		CAnimationState* pIdleState2 = pController->CreateAnimationState("Idle2", "Idle2", 2225, 2622);
		//CAnimationState* pRunFState = pController->CreateAnimationState("RunFront", "RunFront", 2623, 2675);
		CAnimationState* pRunFState = pController->CreateAnimationState("RunFront", "RunFront", 2623, 2674);
		CAnimationState* pWalkFState = pController->CreateAnimationState("WalkFront", "WalkFront", 2676, 2764);
		CAnimationState* pDamage_Back = pController->CreateAnimationState("Damage_Back", "Damage_Back", 2765, 2785);
		CAnimationState* pDamage_Front = pController->CreateAnimationState("Damage_Front", "Damage_Front", 2786, 2806);
		CAnimationState* pDamage_Back_Upper = pController->CreateAnimationState("Damage_Back_Upper", "Damage_Back_Upper", 2807, 2829);
		CAnimationState* pDamage_Front_Upper = pController->CreateAnimationState("Damage_Front_Upper", "Damage_Front_Upper", 2830, 2850);
		CAnimationState* pADD_Damage_Back = pController->CreateAnimationState("ADD_Damage_Back", "ADD_Damage_Back", 2851, 2873);
		CAnimationState* pADD_Damage_Front = pController->CreateAnimationState("ADD_Damage_Front", "ADD_Damage_Front", 2874, 2896);
		CAnimationState* pGet_Up = pController->CreateAnimationState("Get_Up", "Get_Up", 2897, 2927);
		CAnimationState* pDown_Loop = pController->CreateAnimationState("Down_Loop", "Down_Loop", 2928, 2988);
		CAnimationState* pDown_DeadBody = pController->CreateAnimationState("Down_DeadBody", "Down_DeadBody", 2989, 2996);
		CAnimationState* pDown = pController->CreateAnimationState("Down", "Down", 2997, 3012);
		CAnimationState* pPassOut = pController->CreateAnimationState("PassOut", "PassOut", 3013, 3173);
		CAnimationState* pDie = pController->CreateAnimationState("Die", "Die", 3174, 3273);
		CAnimationState* pHit_Bounce = pController->CreateAnimationState("Hit_Bounce", "Hit_Bounce", 3274, 3289);
		CAnimationState* pHit_Down = pController->CreateAnimationState("Hit_Down", "Hit_Down", 3290, 3296);
		CAnimationState* pHit_Loop = pController->CreateAnimationState("Hit_Loop", "Hit_Loop", 3297, 3347);
		CAnimationState* pDeadBody = pController->CreateAnimationState("DeadBody", "DeadBody", 3348, 3352);
		CAnimationState* pHit_Up = pController->CreateAnimationState("Hit_Up", "Hit_Up", 3353, 3362);
		CAnimationState* pAttack1 = pController->CreateAnimationState("Attack1", "Attack1", 3363, 3408);
		CAnimationState* pAttack2 = pController->CreateAnimationState("Attack2", "Attack2", 3409, 3454);
		CAnimationState* pDamage_B_WP = pController->CreateAnimationState("Damage_B_WP", "Damage_B_WP", 3455, 3477);
		CAnimationState* pDamage_F_WP = pController->CreateAnimationState("Damage_F_WP", "Damage_F_WP", 3478, 3500);
		CAnimationState* pIdle_C = pController->CreateAnimationState("Idle_C", "Idle_C", 3501, 3549);
		CAnimationState* pIdle_P_01_WP = pController->CreateAnimationState("Idle_P_01_WP", "Idle_P_01_WP", 3550, 3625);
		CAnimationState* pRun_F_WP = pController->CreateAnimationState("Run_F_WP", "Run_F_WP", 3626, 3649);
		CAnimationState* pUnWeapon = pController->CreateAnimationState("UnWeapon", "UnWeapon", 3650, 3694);
		CAnimationState* pUnWeapon_Ready = pController->CreateAnimationState("UnWeapon_Ready", "UnWeapon_Ready", 3695, 3731);
		CAnimationState* pWalk_F_WP = pController->CreateAnimationState("Walk_F_WP", "Walk_F_WP", 3732, 3772);
		CAnimationState* pWeapon = pController->CreateAnimationState("Weapon", "Weapon", 3773, 3817);
		CAnimationState* pWeapon_Ready = pController->CreateAnimationState("Weapon_Ready", "Weapon_Ready", 3818, 3854);
		CAnimationState* pUnWeapon_Run = pController->CreateAnimationState("UnWeapon_Run", "UnWeapon_Run", 3855, 3889);
		CAnimationState* pWeapon_Run = pController->CreateAnimationState("Weapon_Run", "Weapon_Run", 3890, 3925);
		CAnimationState* pDMG_B_Upper_WP = pController->CreateAnimationState("DMG_B_Upper_WP", "DMG_B_Upper_WP", 3926, 3948);
		CAnimationState* pDMG_F_Upper_WP = pController->CreateAnimationState("DMG_F_Upper_WP", "DMG_F_Upper_WP", 3949, 3971);
		CAnimationState* pAdd_DMG_B_WP = pController->CreateAnimationState("Add_DMG_B_WP", "Add_DMG_B_WP", 3972, 3994);
		CAnimationState* pAdd_DMG_F_WP = pController->CreateAnimationState("Add_DMG_F_WP", "Add_DMG_F_WP", 3995, 4017);
		CAnimationState* pSkill1 = pController->CreateAnimationState("Skill1", "Skill1", 4018, 4101);
		CAnimationState* pSkill2 = pController->CreateAnimationState("Skill2", "Skill2", 4102, 4170);
		CAnimationState* pDeadBody2 = pController->CreateAnimationState("DeadBody", "DeadBody", 4171, 4193);
		CAnimationState* pDie2 = pController->CreateAnimationState("Die", "Die", 4194, 4293);
		CAnimationState* pTESTState = pController->CreateAnimationState("TEST", "TEST", 0, 4581);


		pAttack1->Loop(true);
		pAttack2->Loop(true);
		pSkill1->Loop(true);
		pSkill2->Loop(true);
		pAttack1->SetSpeed(0.35f);
		pAttack2->SetSpeed(0.35f);
		pSkill1->SetSpeed(0.35f);

		// Run
		CTransition* pTransition = pIdleState->AddTransition(pRunFState);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Run", PC_EQUALS, (bool)true);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);

		pTransition = pRunFState->AddTransition(pIdleState);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Run", PC_EQUALS, (bool)false);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);

		// Attack
		pTransition = pIdleState->AddTransition(pAttack1);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Attack", PC_EQUALS, (bool)true);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);

		pTransition = pAttack1->AddTransition(pIdleState);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Attack", PC_EQUALS, (bool)false);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);

		// Attack2
		pTransition = pIdleState->AddTransition(pAttack2);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Attack2", PC_EQUALS, (bool)true);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.85f);

		pTransition = pAttack2->AddTransition(pIdleState);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Attack2", PC_EQUALS, (bool)false);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);

		// Skill1
		pTransition = pIdleState->AddTransition(pSkill1);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Skill1", PC_EQUALS, (bool)true);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.35f);

		pTransition = pSkill1->AddTransition(pIdleState);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Skill1", PC_EQUALS, (bool)false);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);
		
		// Skill2
		pTransition = pIdleState->AddTransition(pSkill2);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Skill2", PC_EQUALS, (bool)true);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.35f);

		pTransition = pSkill2->AddTransition(pIdleState);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Skill2", PC_EQUALS, (bool)false);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);

		pController->SetEntryState("Idle1");

		SAFE_RELEASE(pAnimator);
		
		SAFE_RELEASE(pPlayer);
	}

	{
		CGameObject* pLight = CGameObject::CreateObject("DirLight", pLayer);

		CDirectionalLight* pLIghtCom = pLight->AddComponent<CDirectionalLight>("DirLightCom");

		SAFE_RELEASE(pLIghtCom);

		CTransform* pTr = pLight->GetTransform();
		pTr->SetWorldPosition(-100.0f, 10.0f, -100.0f);
		SAFE_RELEASE(pTr);

		SAFE_RELEASE(pLight);
	}
	
	//GIZMO
	/*{
		CGameObject* pGizmo = CGameObject::CreateObject("Gizmo", pLayer);
		CGizmo* pGizmoComponent = pGizmo->AddComponent<CGizmo>();
		pGizmoComponent->SetGizmoState(GS_TRANSFORMATION);
		SAFE_RELEASE(pGizmoComponent);

		SAFE_RELEASE(pGizmo);
	}*/

#pragma region Test Monster
	{
		CGameObject* pMonster = CGameObject::CreateObject("Monster_HarpyQ", pLayer);

		CTransform* pTransform = pMonster->GetTransform();
		pTransform->SetWorldPosition(-100.0f, 10.0f, -100.0f);
		pTransform->SetWorldScale(0.3f, 0.3f, 0.3f);
		pTransform->SetWorldRotationY(PI / 2.f);
		SAFE_RELEASE(pTransform);

		CMeshRenderer* pRenderer = pMonster->AddComponent<CMeshRenderer>("HarpyQRenderer");
		pRenderer->SetMesh("HarpyQ");
		pRenderer->SetShader(STANDARD_BUMP_SHADER);
		pRenderer->SetInputLayout(BUMP_ANIM_LAYOUT);
		CMaterial* pMaterial = pRenderer->GetMaterial();

		// HarpyQ Tex
		pMaterial->AddSubset();

		for (int i = 0; i < 1; ++i)
		{
			pMaterial->SetDiffuseTexture("DIF", L"HarpyQ/CH_NPC_MOB_HarpyQ_A01_D_ERC.tga", 0, i);
			pMaterial->SetNormalTexture("NOR", L"HarpyQ/CH_NPC_MOB_HarpyQ_A01_N_ERC.tga", 0, i);
			pMaterial->SetSpecularTexture("SPC", L"HarpyQ/CH_NPC_MOB_HarpyQ_A01_SP_ERC.tga", 0, i);
		}

		SAFE_RELEASE(pMaterial);

		SAFE_RELEASE(pRenderer);

		/*CBurnFX* pBurn = pPlayer->AddComponent<CBurnFX>("Burn");
		pBurn->SetBorderColor(Vector4::green);
		pBurn->SetBorderRangePercentage(0.1f);
		SAFE_RELEASE(pBurn);*/

		CAnimator* pAnimator = pMonster->AddComponent<CAnimator>();
		pAnimator->CreateAnimationController();
		CAnimationStateController* pController = pAnimator->GetAnimationController();
		pController->AddParameter("Run", PT_BOOL);
		pController->AddParameter("Attack1", PT_BOOL);

		// HarpyQ 애니메이션 클립 모음..
		CAnimationState* pAttack1 = pController->CreateAnimationState("Attack1", "Attack1", 0, 79);
		CAnimationState* pAttack2 = pController->CreateAnimationState("Attack2", "Attack2", 80, 154);
		CAnimationState* pDeadBody = pController->CreateAnimationState("DeadBody", "DeadBody", 155, 156);
		CAnimationState* pDie = pController->CreateAnimationState("Die", "Die", 157, 477);
		CAnimationState* pDamage_B = pController->CreateAnimationState("Damage_B", "Damage_B", 478, 532);
		CAnimationState* pDamage_F = pController->CreateAnimationState("Damage_F", "Damage_F", 533, 587);
		CAnimationState* pIdle_C = pController->CreateAnimationState("Idle_C", "Idle_C", 588, 627);
		CAnimationState* pIdle_P1 = pController->CreateAnimationState("Idle_P1", "Idle_P1", 628, 867);
		CAnimationState* pIdle_P2 = pController->CreateAnimationState("Idle_P2", "Idle_P2", 868, 1227);
		CAnimationState* pPassOut = pController->CreateAnimationState("PassOut", "PassOut", 1228, 1327);
		CAnimationState* pRun_F = pController->CreateAnimationState("Run_F", "Run_F", 1328, 1367);
		CAnimationState* pSK_Firing1 = pController->CreateAnimationState("SK_Firing1", "SK_Firing1", 1368, 1492);
		CAnimationState* pSK_Firing2 = pController->CreateAnimationState("SK_Firing2", "SK_Firing2", 1493, 1672);
		CAnimationState* pWalk_F = pController->CreateAnimationState("Walk_F", "Walk_F", 1673, 1752);
		CAnimationState* pAdd_Damage_B = pController->CreateAnimationState("Add_Damage_B", "Add_Damage_B", 1753, 1807);
		CAnimationState* pAdd_Damage_F = pController->CreateAnimationState("Add_Damage_F", "Add_Damage_F", 1808, 1862);
		CAnimationState* pDown = pController->CreateAnimationState("Down", "Down", 1863, 1892);
		CAnimationState* pDown_DeadBody = pController->CreateAnimationState("Down_DeadBody", "Down_DeadBody", 1893, 1894);
		CAnimationState* pDown_Loop = pController->CreateAnimationState("Down_Loop", "Down_Loop", 1895, 1954);
		CAnimationState* pGet_Up = pController->CreateAnimationState("Get_Up", "Get_Up", 1955, 2014);
		CAnimationState* pHit_Up = pController->CreateAnimationState("Hit_Up", "Hit_Up", 2015, 2033);
		CAnimationState* pHit_Loop = pController->CreateAnimationState("Hit_Loop", "Hit_Loop", 2034, 2153);
		CAnimationState* pHit_Down = pController->CreateAnimationState("Hit_Down", "Hit_Down", 2154, 2165);
		CAnimationState* pHit_Bounce = pController->CreateAnimationState("Hit_Bounce", "Hit_Bounce", 2166, 2195);

		//unordered_map<string, CAnimationState*>::iterator	iter = pController->GetAnimationState().find("Exit State");

		//CAnimationState* pExitState = pController->GetAnimationState().find("Exit State")->second;

		CTransition* pTransition = NULL;
		pTransition = pIdle_P1->AddTransition(pRun_F);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Run", PC_EQUALS, (bool)true);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);

		pTransition = pRun_F->AddTransition(pIdle_P1);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Run", PC_EQUALS, (bool)false);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);

		pTransition = pRun_F->AddTransition(pAttack1);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Attack1", PC_EQUALS, (bool)true);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);

		pTransition = pAttack1->AddTransition(pIdle_P1);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Attack1", PC_EQUALS, (bool)false);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);

		pTransition = pRun_F->AddTransition(pDie);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Dead", PC_EQUALS, (bool)true);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);

		/*pTransition = pDie->AddTransition(pDie);
		pTransition->UseExitTime(false);
		pTransition->AddParameterCondition("Dead", PC_EQUALS, (bool)true);
		pTransition->SetExitTime(0.85f);
		pTransition->SetExitDuration(0.15f);*/
		
		pController->SetEntryState("Idle_P1");

		SAFE_RELEASE(pAnimator);


		SAFE_RELEASE(pMonster);
	}

	{
		CGameObject* pMonster = CGameObject::CreateObject("Monster_Yeti", pLayer);

		CTransform* pTransform = pMonster->GetTransform();
		pTransform->SetWorldPosition(-100.0f, 10.0f, -100.0f);
		pTransform->SetWorldScale(0.3f, 0.3f, 0.3f);
		pTransform->SetWorldRotationY(PI / 2.f);
		SAFE_RELEASE(pTransform);

		CMeshRenderer* pRenderer = pMonster->AddComponent<CMeshRenderer>("YetiRenderer");
		pRenderer->SetMesh("Yeti");
		pRenderer->SetShader(STANDARD_BUMP_SHADER);
		pRenderer->SetInputLayout(BUMP_ANIM_LAYOUT);
		SAFE_RELEASE(pRenderer);

		CAnimator* pAnimator = pMonster->AddComponent<CAnimator>();
		pAnimator->CreateAnimationController();
		CAnimationStateController* pController = pAnimator->GetAnimationController();
		pController->AddParameter("Run", PT_BOOL);
		pController->AddParameter("Attack1", PT_BOOL);

		CAnimationState* pAttack1 = pController->CreateAnimationState("Attack1", "Attack1", 0, 2424);

		pController->SetEntryState("Attack1");

		SAFE_RELEASE(pAnimator);
		

		SAFE_RELEASE(pMonster);
	}
#pragma endregion

#pragma region Effect

	// BillBoard Fire
	{
		CGameObject* pFireObject = CGameObject::CreateObject("Fire", pLayer);

		CTransform* pTransform = pFireObject->GetTransform();
		pTransform->SetWorldPosition(20.f, 100.f, 50.f);
		pTransform->SetWorldScale(50.f, 50.f, 1.f);
		SAFE_RELEASE(pTransform);

		CMeshRenderer* pRenderer = pFireObject->AddComponent<CMeshRenderer>("FireRenderer");

		CBillboard* pBillboard = pFireObject->AddComponent<CBillboard>();
		SAFE_RELEASE(pBillboard);

		CFire* pFire = pFireObject->AddComponent<CFire>("FireEffect");

		SAFE_RELEASE(pFire);

		CMaterial* pMaterial = pRenderer->GetMaterial();

		// Sampler WRAP 
		pMaterial->SetSampler("LINEAR_WRAP");
		// Fire Tex (원본 불의 Diffuse Texture 입니다)
		pMaterial->SetDiffuseTexture("FireDiffuse", L"Fire/fire01.dds");
		// Normal Texture에는 왜곡을 적용할 Noise Tex 가 들어갑니다
		pMaterial->SetNormalTexture("FireNoise", L"Fire/noise02.jpg");
		// Specular Texture에는 왜곡된 이미지를 걸러내줄 Alpha Texure 가 들어갑니다
		pMaterial->SetSpecularTexture("FireAlpha", L"Fire/alpha01.dds");

		SAFE_RELEASE(pMaterial);

		pRenderer->SetMesh(ENGINE_MESH_BILLBOARD);
		pRenderer->SetShader("FireBillBoardShader");
		pRenderer->SetInputLayout(POS_UV_LAYOUT);
		pRenderer->SetRenderState(ALPHA_BLEND);
		pRenderer->SetRenderState(CULLNONE);

		SAFE_RELEASE(pRenderer);

		SAFE_RELEASE(pFireObject);
	}

	//// Fire
	//{
	//	CGameObject* pFireObject = CGameObject::CreateObject("Fire", pLayer);

	//	CTransform* pTransform = pFireObject->GetTransform();
	//	pTransform->SetWorldPosition(20.f, 100.f, 50.f);
	//	pTransform->SetWorldScale(50.f, 50.f, 1.f);
	//	SAFE_RELEASE(pTransform);

	//	CMeshRenderer* pRenderer = pFireObject->AddComponent<CMeshRenderer>("FireRenderer");

	//	CFire* pFire = pFireObject->AddComponent<CFire>("FireEffect");

	//	SAFE_RELEASE(pFire);

	//	CMaterial* pMaterial = pRenderer->GetMaterial();
	//	
	//	// Sampler WRAP      
	//	pMaterial->SetSampler("LINEAR_WRAP");
	//	// Fire Tex (원본 불의 Diffuse Texture 입니다)
	//	pMaterial->SetDiffuseTexture("FireDiffuse", L"Fire/fire01.dds");
	//	// Normal Texture에는 왜곡을 적용할 Noise Tex 가 들어갑니다
	//	pMaterial->SetNormalTexture("FireNoise", L"Fire/noise02.jpg");
	//	// Specular Texture에는 왜곡된 이미지를 걸러내줄 Alpha Texure 가 들어갑니다
	//	pMaterial->SetSpecularTexture("FireAlpha", L"Fire/alpha01.dds");

	//	SAFE_RELEASE(pMaterial);
	//	SAFE_RELEASE(pRenderer);

	//	SAFE_RELEASE(pFireObject);
	//}

	// Water
	{
		CGameObject* pWaterObject = CGameObject::CreateObject("Water", pLayer);

		CTransform* pTransform = pWaterObject->GetTransform();
		pTransform->SetWorldPosition(-50.0f, 15.0f, -100.0f);
		pTransform->SetWorldScale(100.f, 100.f, 100.f);
		SAFE_RELEASE(pTransform);

		CWater* pWater = pWaterObject->AddComponent<CWater>("WaterComponent");

		SAFE_RELEASE(pWater);

		CMeshRenderer* pRenderer = pWaterObject->GetComponent<CMeshRenderer>();

		CMaterial* pMaterial = pRenderer->GetMaterial();

		pMaterial->SetSampler("LINEAR_WRAP");
		// Normal Texure 에는 Bump를 적용할 Normal Texture가 들어갑니다
		pMaterial->SetNormalTexture("WaterNormal", L"Water/WaterNormal1.png");
		// Specular Texure 에는 교란을 적용할 Derangement Texure가 들어갑니다
		pMaterial->SetSpecularTexture("WaterDerangement", L"Water/WaterDerangement.jpg");

		SAFE_RELEASE(pMaterial);
		SAFE_RELEASE(pRenderer);

		SAFE_RELEASE(pWaterObject);
	}

	// Distortion
	{
		CGameObject* pDistordObject = CGameObject::CreateObject("Distord", pLayer);

		CTransform* pTransform = pDistordObject->GetTransform();
		pTransform->SetWorldPosition(-50.0f, 17.0f, -100.0f);
		pTransform->SetWorldScale(100.f, 100.f, 100.f);
		SAFE_RELEASE(pTransform);

		CDistortion* pDistortion = pDistordObject->AddComponent<CDistortion>("Distortion");

		SAFE_RELEASE(pDistortion);
		
		SAFE_RELEASE(pDistordObject);
	}

	// Trail
	{
		CGameObject* pTrail = CGameObject::CreateObject("Trail", pLayer);

		//pTrail->SetTransparency(true);

		CTransform* pPlayerTr = pTransmitter->GetTransform();

		CTransform* pTransform = pTrail->GetTransform();
		pTransform->SetWorldScale(50.f, 50.f, 50.f);
		pTransform->SetParent(pPlayerTr);
		SAFE_RELEASE(pTransform);

		SAFE_RELEASE(pPlayerTr);

		CMeshRenderer* pRenderer = pTrail->AddComponent<CMeshRenderer>("Renderer");
		
		CDistortion* pDistortion = pTrail->AddComponent<CDistortion>("");
		SAFE_RELEASE(pDistortion);

		pRenderer->SetMesh(ENGINE_MESH_BILLBOARD);
		//pRenderer->SetShader("TrailShader");
		pRenderer->SetShader("TrailDistortionShader");
		pRenderer->SetRenderState(ALPHA_BLEND);
		pRenderer->SetRenderState(CULLNONE);

		CMaterial* pMaterial = pRenderer->GetMaterial();

		//pMaterial->SetDiffuseTexture("TestTrail", L"Trail/TestTrail.png");
		pMaterial->SetDiffuseTexture("TrailDistortionTex", L"Trail/TrailDist.png");
		SAFE_RELEASE(pMaterial);
		SAFE_RELEASE(pRenderer);

		CTrail* pTrailComponent = pTrail->AddComponent<CTrail>();
		pTrailComponent->SetSize(50.f);
		SAFE_RELEASE(pTrailComponent);

		SAFE_RELEASE(pTrail);
	}
#pragma endregion

#pragma region Camera
	{
		CGameObject* pCamera = GetMainCameraObject();
		pCamera->SetScene(this);

		CTransform* pCameraTransform = pCamera->GetTransform();
		pCameraTransform->SetWorldPosition(0.0f, 20.0f, -60.0f);
		pCameraTransform->SetWorldRotation(PI / 8.0f, 0.0f, 0.0f);
		SAFE_RELEASE(pCameraTransform);

		CCameraMove* pCameraMove = pCamera->AddComponent<CCameraMove>("Move");
		pCameraMove->SetPlayer(pTransmitter);
		SAFE_RELEASE(pCameraMove);

		CPostProcessing* pPostProcessing = pCamera->AddComponent<CPostProcessing>();
		pPostProcessing->SetPostProcessingFlags(PT_NONE);
		pPostProcessing->SetFogColor(Vector4(0.5f, 0.75f, 1.0f, 1.0f));
		pPostProcessing->SetFogDistance(100.0f, 200.0f);
		//pPostProcessing->SetBlurSampleCount(5);
		pPostProcessing->SetDepthOfField(100.0f, 200.0f, 50.0f);
		pPostProcessing->SetHDRExposure(1.0f);
		pPostProcessing->SetAdaptionInterval(2.0f);
		
		SAFE_RELEASE(pPostProcessing);
		SAFE_RELEASE(pCamera);
	}
#pragma endregion
	


	
	SAFE_RELEASE(pLayer);
	return true;
}
