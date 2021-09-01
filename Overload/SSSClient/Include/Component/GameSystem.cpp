#include "GameSystem.h"
#include "Scene.h"
#include "Texture.h"
#include "ResourcesManager.h"
#include "Component/SpawningPool.h"
#include "Component/MeshRenderer.h"
#include "Material.h"
#include "SoundManager.h"
#include "Splash.h"
#include "DataManager.h"
#include "Component/Camera.h"
#include "Component/PostProcessing.h"
#include "Component/DirectionalLight.h"
#include "Layer.h"
#include "Input.h"
#include "Transform.h"

#include "NPC_IDLE_AI.h"

#define FILE_VERSION 0

CGameSystem::CGameSystem() :
	m_iVersion(FILE_VERSION),
	m_iSceneID(0)
{
}


CGameSystem::~CGameSystem()
{
}

void CGameSystem::SetSceneID(size_t iID)
{
	m_iSceneID = iID;
}

uint32_t CGameSystem::GetSceneID() const
{
	return m_iSceneID;
}

bool CGameSystem::Initialize()
{
	// Player Fx
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Attack001_SFX", false, "UseSound/PlayerAttackSound/Attack001.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Attack002_SFX", false, "UseSound/PlayerAttackSound/Attack002.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Attack003_SFX", false, "UseSound/PlayerAttackSound/Attack003.ogg");

	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "BladeDance", false, "UseSound/PlayerAttackSound/BladeDance.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "BladeDance_Ready", false, "UseSound/PlayerAttackSound/BladeDance_Ready.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "BladeDance_LastHit", false, "UseSound/PlayerAttackSound/BladeDance_LastHit.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Smash_SFX", false, "UseSound/PlayerAttackSound/Smash.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Kick_SFX", false, "UseSound/PlayerAttackSound/Kick.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Skill001_SFX", false, "UseSound/PlayerAttackSound/Skill001.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "TimeStop_SFX", false, "UseSound/PlayerAttackSound/TimeTraveler.ogg");

	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Skill_Ready", false, "UseSound/Skill_Ready.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Skill_Cancle", false, "UseSound/Skill_Cancle.ogg");

	// 보이스
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_DIALOGUE, "Attack001_Voice", false, "UseSound/PlayerVoice/Attack001.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_DIALOGUE, "Attack002_Voice", false, "UseSound/PlayerVoice/Attack002.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_DIALOGUE, "Attack003_Voice", false, "UseSound/PlayerVoice/Attack003.ogg");

	GET_SINGLE(CSoundManager)->LoadSound(SOUND_DIALOGUE, "Smash_Voice", false, "UseSound/PlayerVoice/Smash.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_DIALOGUE, "Kick_Voice", false, "UseSound/PlayerVoice/Kick.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_DIALOGUE, "Skill001_Voice", false, "UseSound/PlayerVoice/Skill001.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_DIALOGUE, "Skill002_Voice", false, "UseSound/PlayerVoice/Skill002.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_DIALOGUE, "Damage_Voice", false, "UseSound/PlayerVoice/Damage.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_DIALOGUE, "Down_Voice", false, "UseSound/PlayerVoice/Damage_Down.ogg");

	// BGM
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_BGM, "BGM_TernyDay", true, "UseSound/BGM/Terny.ogg", SOUND_PATH);
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_BGM, "BGM_TernyNight", true, "UseSound/BGM/TernyNight.ogg", SOUND_PATH);
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_BGM, "BGM_KingsRoadDay", true, "UseSound/BGM/KingsRoadDay.ogg", SOUND_PATH);
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_BGM, "BGM_KingsRoadNight", true, "UseSound/BGM/KingsRoadNight.ogg", SOUND_PATH);
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_BGM, "BGM_BattleField", true, "UseSound/BGM/BattleField.ogg", SOUND_PATH);

	// Ambient
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_ENV, "Amb_TernyDay", true, "UseSound/Ambient/Amb_Terny.ogg", SOUND_PATH);
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_ENV, "Amb_TernyNight", true, "UseSound/Ambient/Amb_TernyNight.ogg", SOUND_PATH);
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_ENV, "Amb_KingsRoadDay", true, "UseSound/Ambient/Amb_KingsRoadDay.ogg", SOUND_PATH);
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_ENV, "Amb_KingsRoadNight", true, "UseSound/Ambient/Amb_KingsRoadNight.ogg", SOUND_PATH);
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_ENV, "Amb_BattleField", true, "UseSound/Ambient/Amb_BattleField.ogg", SOUND_PATH);

	// Effect
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "MeteoStone", false, "UseSound/PlayerAttackSound/MeteoStone.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "MeteoBomb", false, "UseSound/PlayerAttackSound/MeteoBomb.ogg");

	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Damage", false, "UseSound/Damage.ogg");

	// Logo
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Logo_AR", false, "UseSound/Logo/AR.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Logo_SSS", false, "UseSound/Logo/SSS.ogg");

	// Bone Dragon
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Dragon_Attack", false, "UseSound/BoneDragon/Attack.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Dragon_Spawn", false, "UseSound/BoneDragon/Appear.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Dragon_Bite", false, "UseSound/BoneDragon/Bite.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Dragon_Breath", false, "UseSound/BoneDragon/Breath.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Dragon_HeadAttack", false, "UseSound/BoneDragon/HeadAttack.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Dragon_Scream", false, "UseSound/BoneDragon/Scream.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Dragon_Damage", false, "UseSound/BoneDragon/Damage.ogg");

	// Monster Sound
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Yeti_Attack1", false, "UseSound/Monster/Yeti/Yeti_Attack1.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Yeti_Attack2", false, "UseSound/Monster/Yeti/Yeti_Attack2.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Yeti_Damage", false, "UseSound/Monster/Yeti/Yeti_Damage.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Yeti_Death", false, "UseSound/Monster/Yeti/Yeti_Death.ogg");

	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Zombie_Attack", false, "UseSound/Monster/Zombie/Zombie_Attack.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Zombie_Damage", false, "UseSound/Monster/Zombie/Zombie_Damage.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Zombie_Death", false, "UseSound/Monster/Zombie/Zombie_Death.ogg");

	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Succubus_Attack1", false, "UseSound/Monster/Succubus/Succubus_Attack1.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Succubus_Attack2", false, "UseSound/Monster/Succubus/Succubus_Attack2.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Succubus_Damage", false, "UseSound/Monster/Succubus/Succubus_Damage.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Succubus_Death", false, "UseSound/Monster/Succubus/Succubus_Death.wav");

	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Hamster_Attack1", false, "UseSound/Monster/Hamster/Hamster_Attack1.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Hamster_Attack2", false, "UseSound/Monster/Hamster/Hamster_Attack2.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Hamster_Damage", false, "UseSound/Monster/Hamster/Hamster_Damage.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Hamster_Death", false, "UseSound/Monster/Hamster/Hamster_Death.ogg");

	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Rabbit_Attack", false, "UseSound/Monster/Rabbit/Rabbit_Attack.wav");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Rabbit_Death", false, "UseSound/Monster/Rabbit/Rabbit_Death.wav");

	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Dryad_Attack1", false, "UseSound/Monster/Dryad/Dryad_Attack1.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Dryad_Attack2", false, "UseSound/Monster/Dryad/Dryad_Attack2.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Dryad_Damage", false, "UseSound/Monster/Dryad/Dryad_Damage.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "Dryad_Death", false, "UseSound/Monster/Dryad/Dryad_Death.wav");

	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "DarkKnight_Attack1", false, "UseSound/Monster/DarkKnight/DarkKnight_Attack1.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "DarkKnight_Attack2", false, "UseSound/Monster/DarkKnight/DarkKnight_Attack2.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "DarkKnight_Damage", false, "UseSound/Monster/DarkKnight/DarkKnight_Damage.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "DarkKnight_Death", false, "UseSound/Monster/DarkKnight/DarkKnight_Death.ogg");

	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "GhostWarrior_Attack1", false, "UseSound/Monster/GhostWarrior/GhostWarrior_Attack1.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "GhostWarrior_Attack2", false, "UseSound/Monster/GhostWarrior/GhostWarrior_Attack2.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "GhostWarrior_Damage", false, "UseSound/Monster/GhostWarrior/GhostWarrior_Damage.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "GhostWarrior_Death", false, "UseSound/Monster/GhostWarrior/GhostWarrior_Death.ogg");

	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "HarpyQ_Attack1", false, "UseSound/Monster/HarpyQ/HarpyQ_Attack1.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "HarpyQ_Attack2", false, "UseSound/Monster/HarpyQ/HarpyQ_Attack2.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "HarpyQ_Damage", false, "UseSound/Monster/HarpyQ/HarpyQ_Damage.ogg");
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_SFX, "HarpyQ_Death", false, "UseSound/Monster/HarpyQ/HarpyQ_Death.ogg");

	return true;
}

void CGameSystem::Start()
{
	//미리 로드 되기 때문에 Start에서 넣어주도록 함.
	GET_SINGLE(CDataManager)->SetGameSystem(m_pGameObject);

	if (m_iSceneID == (uint32_t)PLS_TERNY_DAY)//terny Scene
	{
		// Volume Setting
		GET_SINGLE(CSoundManager)->Stop(SOUND_BGM);
		GET_SINGLE(CSoundManager)->Stop(SOUND_ENV);

		GET_SINGLE(CSoundManager)->Play("BGM_TernyDay");
		GET_SINGLE(CSoundManager)->Play("Amb_TernyDay");

		GET_SINGLE(CSoundManager)->Volume(SOUND_BGM, 0.5f);
		GET_SINGLE(CSoundManager)->Volume(SOUND_ENV, 0.6f);

		/*PostProcessing & Directional Light*/
		CCamera* pCamera = m_pScene->GetMainCamera();
		CPostProcessing* pPostProcessing = pCamera->GetComponent<CPostProcessing>();
		SAFE_RELEASE(pCamera);

		pPostProcessing->SetFogColor(Vector4(0.7f, 0.8f, 0.9f, 1.0f));
		pPostProcessing->SetFogDistance(50.0f, 250.0f);
		pPostProcessing->SetDepthOfField(70.0f, 170.0f, 30.0f);
		pPostProcessing->SetHDRExposure(1.f);
		pPostProcessing->SetHDRWhiteThreshold(100.f);
		SAFE_RELEASE(pPostProcessing);

		CGameObject* pLight = m_pLayer->FindGameObjectWithTag("Light");
		CDirectionalLight* pDirectionalLight = pLight->GetComponent<CDirectionalLight>();
		pDirectionalLight->SetLightColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(1.05f, 1.05f, 1.05f, 1.0f), Vector4(0.375f, 0.375f, 0.375f, 1.0f));
		SAFE_RELEASE(pDirectionalLight);
		SAFE_RELEASE(pLight);
		/*****************************/

		CSplash* pSplash = m_pGameObject->AddComponent<CSplash>();
		pSplash->SetSplashState(SPLASH_ENTER);
		SAFE_RELEASE(pSplash);

		//Change Skybox
		m_pScene->SetSkybox("ClearSky", L"Environments\\SkyboxClearHigh01.dds", TEXTURE_PATH);

		//Load Terrain
		CGameObject* pTerrain = CGameObject::Load(m_pScene, PREFAB_TERRAIN_TERNY_DAY_RELATIVE_PATH, DATA_PATH);
		SAFE_RELEASE(pTerrain);

		//Prefab의 상대경로
		CSpawningPool* pPool = m_pGameObject->AddComponent<CSpawningPool>();

		/*---Rabbit---*/
		string strPoolName = "Rabbit";
		string strFilePath = "Prefab\\Monster\\Rabbit.SSSPrefab";
		string strPathKey = DATA_PATH;
		pPool->AddPool(strPoolName);
		pPool->AddRange(strPoolName, Vector2(130.0f, 0.0f), 60.0f);
		pPool->SetMaxSpawnCount(strPoolName, 30);
		pPool->SetInterval(strPoolName, 20.0f);
		pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		/*-Hamster-*/
		strPoolName = "Hamster";
		strFilePath = "Prefab\\Monster\\Hamster.SSSPrefab";
		pPool->AddPool(strPoolName);
		pPool->AddRange(strPoolName, Vector2(-140.0f, -80.0f), 60.0f);
		pPool->SetMaxSpawnCount(strPoolName, 25);
		pPool->SetInterval(strPoolName, 18.0f);
		pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		/*-Yeti-*/
		strPoolName = "Yeti";
		strFilePath = "Prefab\\Monster\\Yeti.SSSPrefab";
		pPool->AddPool(strPoolName);
		pPool->AddRange(strPoolName, Vector2(200.0f, -200.0f), 30.0f);
		pPool->SetMaxSpawnCount(strPoolName, 3);
		pPool->SetInterval(strPoolName, 16.0f);
		pPool->SetPrefab(strPoolName, strFilePath, strPathKey);
		SAFE_RELEASE(pPool);

#pragma region NPC
		{
			wstring strPath = L"Prefab\\NPC\\Abelardo.SSSPrefab";
			CGameObject* pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			CTransform* pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(-3.f, 58.f, 155.f);
			pNPCTransform->SetWorldRotationY(-90.f * D2R);
			SAFE_RELEASE(pNPCTransform);
			
			CNPC_IDLE_AI* pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(-2.f, 56.f, 157.f), 1.f);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);

			strPath = L"Prefab\\NPC\\Balian.SSSPrefab";
			pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(-5.f, 31.f, 100.f);
			pNPCTransform->SetWorldRotationY(18.f * D2R);
			SAFE_RELEASE(pNPCTransform);

			pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(0.f, 27.8f, 60.f), 40.f);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);

			strPath = L"Prefab\\NPC\\Camilo.SSSPrefab";
			pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(20.f, 31.f, 100.f);
			pNPCTransform->SetWorldRotationY(90.f * D2R);
			SAFE_RELEASE(pNPCTransform);

			pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(0.f, 27.8f, 60.f), 40.f);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);

			strPath = L"Prefab\\NPC\\Cat.SSSPrefab";
			pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(20.f, 31.f, 80.f);
			SAFE_RELEASE(pNPCTransform);

			pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(0.f, 27.8f, 60.f), 40.f);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);

			strPath = L"Prefab\\NPC\\Chinuwa.SSSPrefab";
			pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(-25.f, 31.f, 62.f);
			pNPCTransform->SetWorldRotationY(90.f * D2R);
			SAFE_RELEASE(pNPCTransform);

			pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(0.f, 27.8f, 60.f), 40.f);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);

			strPath = L"Prefab\\NPC\\Chitata.SSSPrefab";
			pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(-15.f, 28.f, 60.f);
			SAFE_RELEASE(pNPCTransform);

			pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(0.f, 27.8f, 60.f), 40.f);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);

			strPath = L"Prefab\\NPC\\Erlione.SSSPrefab";
			pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(10.f, 28.f, 70.f);
			SAFE_RELEASE(pNPCTransform);

			pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(0.f, 27.8f, 60.f), 40.f);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);

			strPath = L"Prefab\\NPC\\Gagato.SSSPrefab";
			pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(20.f, 31.f, 90.f);
			pNPCTransform->SetWorldRotationY(-90.f * D2R);
			SAFE_RELEASE(pNPCTransform);

			pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(0.f, 27.8f, 60.f), 40.f);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);

			strPath = L"Prefab\\NPC\\LeoniAdler.SSSPrefab";
			pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(0.f, 31.f, 40.f);
			SAFE_RELEASE(pNPCTransform);

			pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(0.f, 27.8f, 60.f), 40.f);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);
		}
#pragma endregion
	}
	else if (m_iSceneID == (uint32_t)PLS_KINGSROAD_DAY) // KingsLoad
	{
		// Volume Setting
		GET_SINGLE(CSoundManager)->Stop(SOUND_BGM);
		GET_SINGLE(CSoundManager)->Stop(SOUND_ENV);

		GET_SINGLE(CSoundManager)->Play("BGM_KingsRoadDay");
		GET_SINGLE(CSoundManager)->Play("Amb_KingsRoadDay");

		GET_SINGLE(CSoundManager)->Volume(SOUND_BGM, 0.5f);
		GET_SINGLE(CSoundManager)->Volume(SOUND_ENV, 0.6f);


		/*PostProcessing & Directional Light*/
		CCamera* pCamera = m_pScene->GetMainCamera();
		CPostProcessing* pPostProcessing = pCamera->GetComponent<CPostProcessing>();
		SAFE_RELEASE(pCamera);

		pPostProcessing->SetFogColor(Vector4(0.9f, 0.8f, 0.7f, 1.0f));
		pPostProcessing->SetFogDistance(50.0f, 450.0f);
		pPostProcessing->SetDepthOfField(70.0f, 170.0f, 30.0f);
		pPostProcessing->SetHDRExposure(1.f);
		pPostProcessing->SetHDRWhiteThreshold(15.f);
		SAFE_RELEASE(pPostProcessing);

		CGameObject* pLight = m_pLayer->FindGameObjectWithTag("Light");
		CDirectionalLight* pDirectionalLight = pLight->GetComponent<CDirectionalLight>();
		pDirectionalLight->SetLightColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.8f, 0.8f, 0.8f, 1.0f), Vector4(0.375f, 0.375f, 0.375f, 1.0f));
		SAFE_RELEASE(pDirectionalLight);
		SAFE_RELEASE(pLight);
		/*****************************/
		CSplash* pSplash = m_pGameObject->AddComponent<CSplash>();
		pSplash->SetSplashState(SPLASH_ENTER);
		SAFE_RELEASE(pSplash);

		//Change Skybox
		m_pScene->SetSkybox("KingsRoad_Day_Sky", L"Environments\\Skybox_KingsRoad_Day.dds", TEXTURE_PATH);

		//Load Terrain
		CGameObject* pTerrain = CGameObject::Load(m_pScene, PREFAB_TERRAIN_KINGSROAD_DAY_RELATIVE_PATH, DATA_PATH);
		SAFE_RELEASE(pTerrain);

		//Prefab의 상대경로
		CSpawningPool* pPool = m_pGameObject->AddComponent<CSpawningPool>();

		/*---서큐버스---*/
		string strPoolName = "SuccA";
		string strFilePath = "Prefab\\Monster\\SuccA.SSSPrefab";
		string strPathKey = DATA_PATH;
		pPool->AddPool(strPoolName);
		pPool->AddRange(strPoolName, Vector2(80.0f, 40.0f), 35.0f);
		pPool->SetMaxSpawnCount(strPoolName, 10);
		pPool->SetInterval(strPoolName, 10.0f);
		pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		/*-서큐버스-*/
		strPoolName = "SuccuC";
		strFilePath = "Prefab\\Monster\\SuccuC.SSSPrefab";
		pPool->AddPool(strPoolName);
		pPool->AddRange(strPoolName, Vector2(-100.0f, -60.0f), 30.0f);
		pPool->SetMaxSpawnCount(strPoolName, 10);
		pPool->SetInterval(strPoolName, 12.0f);
		pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		/*-하피-*/
		strPoolName = "HarpyQ";
		strFilePath = "Prefab\\Monster\\HarpyQ.SSSPrefab";
		pPool->AddPool(strPoolName);
		pPool->AddRange(strPoolName, Vector2(160, 160), 70.0f);
		pPool->AddRange(strPoolName, Vector2(-160, -160), 60.0f);
		pPool->AddRange(strPoolName, Vector2(-180, 170), 45.0f);
		pPool->AddRange(strPoolName, Vector2(160, -160), 60.0f);
		pPool->SetMaxSpawnCount(strPoolName, 25);
		pPool->SetInterval(strPoolName, 15.0f);
		pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		SAFE_RELEASE(pPool);
	}

	else if (m_iSceneID == (uint32_t)PLS_BATTLFIELD_DAY) //Bandith
	{
		// Volume Setting
		GET_SINGLE(CSoundManager)->Stop(SOUND_BGM);
		GET_SINGLE(CSoundManager)->Stop(SOUND_ENV);

		GET_SINGLE(CSoundManager)->Play("BGM_BattleField");
		GET_SINGLE(CSoundManager)->Play("Amb_BattleField");

		GET_SINGLE(CSoundManager)->Volume(SOUND_BGM, 0.5f);
		GET_SINGLE(CSoundManager)->Volume(SOUND_ENV, 0.6f);

		/*PostProcessing & Directional Light*/
		CCamera* pCamera = m_pScene->GetMainCamera();
		CPostProcessing* pPostProcessing = pCamera->GetComponent<CPostProcessing>();
		SAFE_RELEASE(pCamera);

		pPostProcessing->SetFogColor(Vector4(0.74f, 0.64f, 0.57f, 1.0f));
		pPostProcessing->SetFogDistance(10.0f, 380.0f);
		pPostProcessing->SetDepthOfField(70.0f, 170, 30.0f);
		pPostProcessing->SetHDRExposure(1.f);
		pPostProcessing->SetHDRWhiteThreshold(70.f);
		SAFE_RELEASE(pPostProcessing);

		CGameObject* pLight = m_pLayer->FindGameObjectWithTag("Light");
		CDirectionalLight* pDirectionalLight = pLight->GetComponent<CDirectionalLight>();
		pDirectionalLight->SetLightColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.375f, 0.375f, 0.375f, 1.0f));
		SAFE_RELEASE(pDirectionalLight);
		SAFE_RELEASE(pLight);
		/*****************************/


		CSplash* pSplash = m_pGameObject->AddComponent<CSplash>();
		pSplash->SetSplashState(SPLASH_ENTER);
		SAFE_RELEASE(pSplash);

		//Change Skybox
		m_pScene->SetSkybox("BattleField_Day_Sky", L"Environments\\DefaultSkybox.dds", TEXTURE_PATH);

		//Load Terrain
		CGameObject* pTerrain = CGameObject::Load(m_pScene, PREFAB_TERRAIN_BATTLEFIELD_DAY_RELATIVE_PATH, DATA_PATH);
		SAFE_RELEASE(pTerrain);


		//Prefab의 상대경로
		CSpawningPool* pPool = m_pGameObject->AddComponent<CSpawningPool>();
		
		/*---Rabbit---*/
		string strPoolName = "Rabbit";
		string strFilePath = "Prefab\\Monster\\Rabbit.SSSPrefab";
		string strPathKey = DATA_PATH;
		pPool->AddPool(strPoolName);
		pPool->AddRange(strPoolName, Vector2(-80.0f, -70.0f), 55.0f);
		pPool->AddRange(strPoolName, Vector2(170.0f, -80.0f), 55.0f);
		pPool->AddRange(strPoolName, Vector2(-200.0f, 0.f), 35.0f);
		pPool->AddRange(strPoolName, Vector2(-200.0f, -200.0f), 55.0f);
		pPool->AddRange(strPoolName, Vector2(0, -210.0f), 35.0f);
		pPool->SetMaxSpawnCount(strPoolName, 50);
		pPool->SetInterval(strPoolName, 15.0f);
		pPool->SetPrefab(strPoolName, strFilePath, strPathKey);
		/*-Hamster-*/
		strPoolName = "Hamster";
		strFilePath = "Prefab\\Monster\\Hamster.SSSPrefab";
		pPool->AddPool(strPoolName);
		pPool->AddRange(strPoolName, Vector2(10.0f, -70.0f), 55.0f);
		pPool->AddRange(strPoolName, Vector2(170.0f, 60.0f), 55.0f);
		pPool->AddRange(strPoolName, Vector2(-200.0f, -90.f), 35.0f);
		pPool->AddRange(strPoolName, Vector2(-150.0f, -200.0f), 55.0f);
		pPool->AddRange(strPoolName, Vector2(60, -210.0f), 40.0f);
		pPool->SetMaxSpawnCount(strPoolName, 30);
		pPool->SetInterval(strPoolName, 18.0f);
		pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		strPoolName = "Dryad";
		strFilePath = "Prefab\\Monster\\Dryad.SSSPrefab";
		pPool->AddPool(strPoolName);
		pPool->AddRange(strPoolName, Vector2(-100.f, 150.0f), 80.0f);
		pPool->SetMaxSpawnCount(strPoolName, 15);
		pPool->SetInterval(strPoolName, 22.0f);
		pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		strPoolName = "ZombieC";
		strFilePath = "Prefab\\Monster\\ZombieC.SSSPrefab";
		pPool->AddPool(strPoolName);
		pPool->AddRange(strPoolName, Vector2(0.f, 150.0f), 80.0f);
		pPool->SetMaxSpawnCount(strPoolName, 15);
		pPool->SetInterval(strPoolName, 20.0f);
		pPool->SetPrefab(strPoolName, strFilePath, strPathKey);
		SAFE_RELEASE(pPool);
	}
	else if (m_iSceneID == (uint32_t)PLS_KINGSROAD_NIGHT)
	{
		// Volume Setting
		GET_SINGLE(CSoundManager)->Stop(SOUND_BGM);
		GET_SINGLE(CSoundManager)->Stop(SOUND_ENV);

		GET_SINGLE(CSoundManager)->Play("BGM_KingsRoadNight");
		GET_SINGLE(CSoundManager)->Play("Amb_KingsRoadNight");

		GET_SINGLE(CSoundManager)->Volume(SOUND_BGM, 0.5f);
		GET_SINGLE(CSoundManager)->Volume(SOUND_ENV, 0.6f);

		/*PostProcessing & Directional Light*/
		CCamera* pCamera = m_pScene->GetMainCamera();
		CPostProcessing* pPostProcessing = pCamera->GetComponent<CPostProcessing>();
		SAFE_RELEASE(pCamera);

		pPostProcessing->SetFogColor(Vector4(0.3f, 0.1f, 0.1f, 1.0f));
		pPostProcessing->SetFogDistance(50.0f, 350.0f);
		pPostProcessing->SetDepthOfField(75.0f, 150, 50.0f);
		pPostProcessing->SetHDRExposure(1.5f);
		pPostProcessing->SetHDRWhiteThreshold(9.70112610f);
		SAFE_RELEASE(pPostProcessing);

		CGameObject* pLight = m_pLayer->FindGameObjectWithTag("Light");
		CDirectionalLight* pDirectionalLight = pLight->GetComponent<CDirectionalLight>();
		pDirectionalLight->SetLightColor(Vector4(0.7f, 0.7f, 0.7f, 1.0f), Vector4(0.7f, 0.7f, 0.7f, 1.0f), Vector4(0.375f, 0.375f, 0.375f, 1.0f));
		SAFE_RELEASE(pDirectionalLight);
		SAFE_RELEASE(pLight);
		/*****************************/

		CSplash* pSplash = m_pGameObject->AddComponent<CSplash>();
		pSplash->SetSplashState(SPLASH_ENTER);
		SAFE_RELEASE(pSplash);

		//Change Skybox
		m_pScene->SetSkybox("KingsRoad_Night_Sky", L"Environments\\Skybox_KingsRoad_Night.dds", TEXTURE_PATH);

		//Load Terrain
		CGameObject* pTerrain = CGameObject::Load(m_pScene, PREFAB_TERRAIN_KINGSROAD_NIGHT_RELATIVE_PATH, DATA_PATH);
		SAFE_RELEASE(pTerrain);

		//Prefab의 상대경로
		CSpawningPool* pPool = m_pGameObject->AddComponent<CSpawningPool>();

		/*---다크나이트---*/
		string strPoolName = "DarkKnight";
		string strFilePath = "Prefab\\Monster\\DarkKnight.SSSPrefab";
		string strPathKey = DATA_PATH;
		pPool->AddPool(strPoolName);
		pPool->AddRange(strPoolName, Vector2(80.0f, 40.0f), 35.0f);
		pPool->SetMaxSpawnCount(strPoolName, 10);
		pPool->SetInterval(strPoolName, 10.0f);
		pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		/*-고스트워리어-*/
		strPoolName = "GhostWarrior";
		strFilePath = "Prefab\\Monster\\GhostWarrior.SSSPrefab";
		pPool->AddPool(strPoolName);
		pPool->AddRange(strPoolName, Vector2(-100.0f, -60.0f), 30.0f);
		pPool->SetMaxSpawnCount(strPoolName, 10);
		pPool->SetInterval(strPoolName, 12.0f);
		pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		/*-좀비A-*/
		strPoolName = "ZombieA";
		strFilePath = "Prefab\\Monster\\ZombieA.SSSPrefab";
		pPool->AddPool(strPoolName);
		pPool->AddRange(strPoolName, Vector2(5.f, 5.0f), 30.0f);
		pPool->SetMaxSpawnCount(strPoolName, 8);
		pPool->SetInterval(strPoolName, 30.0f);
		pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		/*-하피-*/
		strPoolName = "HarpyQ";
		strFilePath = "Prefab\\Monster\\HarpyQ.SSSPrefab";
		pPool->AddPool(strPoolName);
		pPool->AddRange(strPoolName, Vector2(160, 160), 70.0f);
		pPool->AddRange(strPoolName, Vector2(-160, -160), 60.0f);
		pPool->AddRange(strPoolName, Vector2(-180, 170), 45.0f);
		pPool->AddRange(strPoolName, Vector2(160, -160), 60.0f);
		pPool->SetMaxSpawnCount(strPoolName, 26);
		pPool->SetInterval(strPoolName, 15.0f);
		pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		SAFE_RELEASE(pPool);
	}
	else if (m_iSceneID == (uint32_t)PLS_TERNY_NIGHT)
	{
		// Volume Setting
		GET_SINGLE(CSoundManager)->Stop(SOUND_BGM);
		GET_SINGLE(CSoundManager)->Stop(SOUND_ENV);

		GET_SINGLE(CSoundManager)->Play("BGM_TernyNight");
		GET_SINGLE(CSoundManager)->Play("Amb_TernyNight");

		GET_SINGLE(CSoundManager)->Volume(SOUND_BGM, 0.5f);
		GET_SINGLE(CSoundManager)->Volume(SOUND_ENV, 0.6f);

		/*PostProcessing & Directional Light*/
		CCamera* pCamera = m_pScene->GetMainCamera();
		CPostProcessing* pPostProcessing = pCamera->GetComponent<CPostProcessing>();
		SAFE_RELEASE(pCamera);

		pPostProcessing->SetFogColor(Vector4(0.1f, 0.1f, 0.45f, 1.0f));
		pPostProcessing->SetFogDistance(20.0f, 180.0f);
		pPostProcessing->SetDepthOfField(75.0f, 150, 50.0f);
		pPostProcessing->SetHDRExposure(1.5f);
		pPostProcessing->SetHDRWhiteThreshold(9.70112610f);
		SAFE_RELEASE(pPostProcessing);

		CGameObject* pLight = m_pLayer->FindGameObjectWithTag("Light");
		CDirectionalLight* pDirectionalLight = pLight->GetComponent<CDirectionalLight>();
		pDirectionalLight->SetLightColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.375f, 0.375f, 0.375f, 1.0f));
		SAFE_RELEASE(pDirectionalLight);
		SAFE_RELEASE(pLight);
		/*****************************/


		CSplash* pSplash = m_pGameObject->AddComponent<CSplash>();
		pSplash->SetSplashState(SPLASH_ENTER);
		SAFE_RELEASE(pSplash);

		//Change Skybox
		m_pScene->SetSkybox("Terny_Night_Sky", L"Environments\\Skybox_Terny_Night.dds", TEXTURE_PATH);

		//Load Terrain
		CGameObject* pTerrain = CGameObject::Load(m_pScene, PREFAB_TERRAIN_TERNY_NIGHT_RELATIVE_PATH, DATA_PATH);
		SAFE_RELEASE(pTerrain);


		//Prefab의 상대경로
		//CSpawningPool* pPool = m_pGameObject->AddComponent<CSpawningPool>();

		///*---Rabbit---*/
		//string strPoolName = "GhostWarrior";
		//string strFilePath = "Prefab\\Monster\\GhostWarrior.SSSPrefab";
		//string strPathKey = DATA_PATH;
		//pPool->AddPool(strPoolName);
		//pPool->AddRange(strPoolName, Vector2(-120.0f, -40.0f), 55.0f);
		//pPool->SetMaxSpawnCount(strPoolName, 15);
		//pPool->SetInterval(strPoolName, 15.0f);
		//pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		///*-Hamster-*/
		//strPoolName = "DarkKnight";
		//strFilePath = "Prefab\\Monster\\DarkKnight.SSSPrefab";
		//pPool->AddPool(strPoolName);
		//pPool->AddRange(strPoolName, Vector2(150.0f, -110.0f), 50.0f);
		//pPool->SetMaxSpawnCount(strPoolName, 16);
		//pPool->SetInterval(strPoolName, 30.0f);
		//pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		//strPoolName = "ZombieA";
		//strFilePath = "Prefab\\Monster\\ZombieA.SSSPrefab";
		//pPool->AddPool(strPoolName);
		//pPool->AddRange(strPoolName, Vector2(0.f, 100.0f), 40.0f);
		//pPool->SetMaxSpawnCount(strPoolName, 15);
		//pPool->SetInterval(strPoolName, 23.0f);
		//pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		//strPoolName = "ZombieC";
		//strFilePath = "Prefab\\Monster\\ZombieC.SSSPrefab";
		//pPool->AddPool(strPoolName);
		//pPool->AddRange(strPoolName, Vector2(0.f, -100.0f), 40.0f);
		//pPool->SetMaxSpawnCount(strPoolName, 15);
		//pPool->SetInterval(strPoolName, 18.0f);
		//pPool->SetPrefab(strPoolName, strFilePath, strPathKey);

		//SAFE_RELEASE(pPool);

		// BoneDragon
		{
			wstring strPath = L"Prefab\\Monster\\BoneDragon.SSSPrefab";
			CGameObject* pBoneDragonObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			CTransform* pBoneDragonTransform = pBoneDragonObject->GetTransform();
			pBoneDragonTransform->SetWorldPosition(-5.f, 32.f, 84.f);
			SAFE_RELEASE(pBoneDragonTransform);
			SAFE_RELEASE(pBoneDragonObject);
		}


#pragma region NPC
		{
			wstring strPath = L"Prefab\\NPC\\Abelardo.SSSPrefab";
			CGameObject* pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			CTransform* pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(-3.f, 58.f, 155.f);
			pNPCTransform->SetWorldRotationY(-90.f * D2R);
			SAFE_RELEASE(pNPCTransform);

			CNPC_IDLE_AI* pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(-2.f, 56.f, 157.f), 1.f);
			pNpc->SetDeadBody(true);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);

			strPath = L"Prefab\\NPC\\Balian.SSSPrefab";
			pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(-5.f, 31.f, 100.f);
			pNPCTransform->SetWorldRotationY(18.f * D2R);
			SAFE_RELEASE(pNPCTransform);

			pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(0.f, 27.8f, 60.f), 40.f);
			pNpc->SetDeadBody(true);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);

			strPath = L"Prefab\\NPC\\Camilo.SSSPrefab";
			pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(20.f, 31.f, 100.f);
			pNPCTransform->SetWorldRotationY(90.f * D2R);
			SAFE_RELEASE(pNPCTransform);

			pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(0.f, 27.8f, 60.f), 40.f);
			pNpc->SetDeadBody(true);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);

			strPath = L"Prefab\\NPC\\Chitata.SSSPrefab";
			pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(-15.f, 28.f, 60.f);
			SAFE_RELEASE(pNPCTransform);

			pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(0.f, 27.8f, 60.f), 40.f);
			pNpc->SetDeadBody(true);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);

			strPath = L"Prefab\\NPC\\Erlione.SSSPrefab";
			pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(10.f, 28.f, 70.f);
			SAFE_RELEASE(pNPCTransform);

			pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(0.f, 27.8f, 60.f), 40.f);
			pNpc->SetDeadBody(true);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);

			strPath = L"Prefab\\NPC\\Gagato.SSSPrefab";
			pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(20.f, 31.f, 90.f);
			pNPCTransform->SetWorldRotationY(-90.f * D2R);
			SAFE_RELEASE(pNPCTransform);

			pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(0.f, 27.8f, 60.f), 40.f);
			pNpc->SetDeadBody(true);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);

			strPath = L"Prefab\\NPC\\LeoniAdler.SSSPrefab";
			pNPCObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
			pNPCTransform = pNPCObject->GetTransform();
			pNPCTransform->SetWorldPosition(0.f, 31.f, 35.f);
			SAFE_RELEASE(pNPCTransform);

			pNpc = pNPCObject->GetComponent<CNPC_IDLE_AI>();
			pNpc->SetMoveDistance(Vector3(0.f, 27.8f, 60.f), 40.f);
			pNpc->SetDeadBody(true);
			SAFE_RELEASE(pNpc);

			SAFE_RELEASE(pNPCObject);
		}
#pragma endregion
	}


	//TitleScene
	if (m_iSceneID == (uint32_t)PLS_NONE)
	{
		CSplash* pSplash = m_pGameObject->AddComponent<CSplash>();
		pSplash->SetSplashState(SPLASH_TITLE);
		//로드할 경로
		pSplash->SetPath(SCENE_TERNY_DAY_RELATIVE_PATH);

		pSplash->SetPathKey(DATA_PATH);
		SAFE_RELEASE(pSplash);
	}
}

int CGameSystem::Update(float fTime)
{
	if (GETKEYDOWN(KEY_F1))
	{
		PLAYERDATA tData = GET_SINGLE(CDataManager)->GetPlayerData();
		tData.iQuestProgress = 30U;
		GET_SINGLE(CDataManager)->SetPlayerData(tData);
	}
	return 0;
}

bool CGameSystem::Save(FILE * pFile)
{
	fwrite(&m_iVersion, sizeof(uint32_t), 1, pFile);
	fwrite(&m_iSceneID, sizeof(uint32_t), 1, pFile);
	return true;
}

bool CGameSystem::Load(FILE * pFile)
{
	fread(&m_iVersion, sizeof(uint32_t), 1, pFile);
	fread(&m_iSceneID, sizeof(uint32_t), 1, pFile);
	return true;
}
