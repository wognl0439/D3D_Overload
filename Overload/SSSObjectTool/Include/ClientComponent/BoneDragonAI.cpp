#include "BoneDragonAI.h"
#include "Collider.h"
#include "Component/Animator.h"
#include "AnimationState.h"
#include "AnimationClip.h"
#include "Transform.h"
#include "Input.h"
#include "Component/Distortion.h"
#include "Component/MeshRenderer.h"
#include "Material.h"
#include "Scene.h"
#include "Layer.h"
#include "Component/Effect.h"
#include "CollisionManager.h"
#include "Component/TerrainCollider.h"
#include "Attack.h"
#include "Component/SphereCollider.h"
#include "Component/SpriteRenderer.h"
#include "Component/Fire.h"

#include "EffectController.h"
#include "Component/Billboard.h"
#include "Component/Particle.h"
#include "Component/PostProcessing.h"
#include "Component/HealthBar.h"
#include "BossHealthBar.h"
#include "Component/BoxCollider.h"
#include "Component/RigidBody.h"
#include "Component/MeshCollider.h"
#include "SoundManager.h"

#include "PlayerCamera.h"
#include "DataManager.h"

CBoneDragonAI::CBoneDragonAI()
{
	m_pStateController = NULL;
	m_pPlayer = NULL;

	m_fActiveTime = 0.f;
	m_fActiveMaxTime = 1.f;
	m_fMoveSpeed = 15.f;
	
	m_fHPMax = 600.f;
	m_fHP = m_fHPMax;

	m_bSpawn = true;
	m_bSpawnStart = false;
	m_bAttack = false;
	m_bSoundPlay = false;
	m_pHealthBar = NULL;
	m_bRage = false;
	m_bRageOn = false;
	m_bEffect = false;
}


CBoneDragonAI::~CBoneDragonAI()
{
	size_t iSize = m_vecFireEffects.size();
	for (int i = 0; i < iSize; ++i)
	{
		m_vecFireEffects[i]->Destroy(true);
	}
	m_vecFireEffects.clear();

	if (m_pHealthBar)
		m_pHealthBar->Destroy(true);
}

void CBoneDragonAI::TransState(float fTime)
{
	if (m_pPlayer == NULL)
		return;

	//if (m_bRage == true
	//	&& m_bRageOn == false)
	//	return;

	CTransform* pPlayerTr = m_pPlayer->GetTransform();
	Vector3 vPlayerPosition = pPlayerTr->GetWorldPosition();
	SAFE_RELEASE(pPlayerTr);

	CAnimationState* pAnimationState = m_pStateController->GetCurrentState();
	CAnimationClip* pClip = pAnimationState->GetClip();

	if (pClip->GetClipName() == "Idle_C")
	{
		m_fActiveMaxTime = 0.75f;
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;

		m_bAttack = false;

		if (fProgress >= 1.f)
		{
			Vector3 vMyPosition = m_pTransform->GetWorldPosition();
			Vector3 vDir = vPlayerPosition - vMyPosition;
			float	fDistance = vDir.Length();
			vDir.normalize();
			vPlayerPosition.y = vMyPosition.y;
			m_pTransform->LookAt(vPlayerPosition);
			// 공격 범위 안에 있으면 공격합니당
			if (fDistance < 20.f)
			{
				m_fActiveTime = 0.f;

				int iRandom = rand() % 80;
				if (iRandom < 20)
				{
					// ATK_01 로 간다
					m_pStateController->SetInteger("State", 2);
				}
				else if (iRandom < 40)
				{
					// ATK_02 로 간다
					m_pStateController->SetInteger("State", 3);
				}
				else if (iRandom < 60)
				{
					// SK_Firing_01 로 간다 // 머리로 올려치는 공격
					m_pStateController->SetInteger("State", 4);
				}
				else if (iRandom < 80)
				{
					// SK_Firing_02 로 간다 // 브레스 공격
					m_pStateController->SetInteger("State", 5);
				}
				else if (iRandom < 100)
				{
					// SK_Firing_03 로 간다 // 발작 공격
					// Rage 상태로 바꿨땅..
					//m_pStateController->SetInteger("State", 6);
				}
			}
			else
			{
				m_fActiveTime = 0.f;
				// Run_F 로 간다
				m_pStateController->SetInteger("State", 1);
			}
		}
	}
	else if (pClip->GetClipName() == "Run_F")
	{
		m_fActiveMaxTime = 5.f;
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;

		if (fProgress >= 1.f)
		{
			m_fActiveTime = 0.f;
			// Idle 로 간다
			m_pStateController->SetInteger("State", 0);
		}

		Vector3 vMyPosition = m_pTransform->GetWorldPosition();
		Vector3 vDir = vPlayerPosition - vMyPosition;
		float	fDistance = vDir.Length();
		vDir.normalize();
		Vector3 vMovePosition = vMyPosition + (vDir * m_fMoveSpeed * fTime);
		m_pTransform->SetWorldPosition(vMovePosition);
		vPlayerPosition.y = vMyPosition.y;
		m_pTransform->LookAt(vPlayerPosition);

		// 공격 범위 안에 있으면 공격합니당
		if (fDistance < 20.f)
		{
			m_fActiveTime = 0.f;

			int iRandom = rand() % 80;
			if (iRandom < 20)
			{
				// ATK_01 로 간다
				m_pStateController->SetInteger("State", 2);
			}
			else if (iRandom < 40)
			{
				// ATK_02 로 간다
				m_pStateController->SetInteger("State", 3);
			}
			else if (iRandom < 60)
			{
				// SK_Firing_01 로 간다 // 머리로 올려치는 공격
				m_pStateController->SetInteger("State", 4);
			}
			else if (iRandom < 80)
			{
				// SK_Firing_02 로 간다 // 브레스 공격
				m_pStateController->SetInteger("State", 5);
			}
			else if (iRandom < 100)
			{
				// SK_Firing_03 로 간다 // 발작 공격
				//m_pStateController->SetInteger("State", 6);
			}
		}
	}
	else if (pClip->GetClipName() == "ATK_01")
	{
		CAnimationState* pAnimationState = m_pStateController->GetCurrentState();
		if (pAnimationState)
		{
			CAnimationClip* pClip = pAnimationState->GetClip();
			if (pAnimationState->End())
			{
				// Idle_C 로 간다
				m_pStateController->SetInteger("State", 0);
			}
			// 공격 판정
			else if (pClip->GetProgress() >= 0.3f
				&& m_bAttack == false)
			{
				CGameObject* pAttackObj = CGameObject::CreateObject("Attack", m_pLayer);
				CSphereCollider* pCollider = pAttackObj->AddComponent<CSphereCollider>();
				pCollider->SetRadius(10.f);
				SAFE_RELEASE(pCollider);
				CAttack* pAttack = pAttackObj->AddComponent<CAttack>();
				pAttack->SetValue(-1);
				pAttack->SetAttribute(ATTACK_DOWN);
				pAttack->SetDamagePoint(50.f);
				pAttack->SetActiveMaxTime(0.5f);
				SAFE_RELEASE(pAttack);
				CTransform* pTr = pAttackObj->GetTransform();
				pTr->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * 10.f);
				SAFE_RELEASE(pTr);
				SAFE_RELEASE(pAttackObj);
				m_bAttack = true;

				GET_SINGLE(CSoundManager)->Play("Dragon_Attack");
			}
		}
	}
	else if (pClip->GetClipName() == "ATK_02")
	{
		CAnimationState* pAnimationState = m_pStateController->GetCurrentState();
		if (pAnimationState)
		{
			CAnimationClip* pClip = pAnimationState->GetClip();
			if (pClip->GetProgress() >= 0.f && pClip->GetProgress() <= 0.1f &&
				!m_bSoundPlay)
			{
				m_bSoundPlay = true;
				GET_SINGLE(CSoundManager)->Play("Dragon_Bite");
			}

			if (pAnimationState->End())
			{
				m_bSoundPlay = false;
				// Idle_C 로 간다
				m_pStateController->SetInteger("State", 0);
			}
			// 공격 판정
			else if (pClip->GetProgress() >= 0.3f
				&& m_bAttack == false)
			{
				CGameObject* pAttackObj = CGameObject::CreateObject("Attack", m_pLayer);
				CSphereCollider* pCollider = pAttackObj->AddComponent<CSphereCollider>();
				pCollider->SetRadius(10.f);
				SAFE_RELEASE(pCollider);
				CAttack* pAttack = pAttackObj->AddComponent<CAttack>();
				pAttack->SetValue(-1);
				pAttack->SetAttribute(ATTACK_NORMAL);
				pAttack->SetDamagePoint(50.f);
				pAttack->SetActiveMaxTime(0.5f);
				SAFE_RELEASE(pAttack);
				CTransform* pTr = pAttackObj->GetTransform();
				pTr->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * 10.f);
				SAFE_RELEASE(pTr);
				SAFE_RELEASE(pAttackObj);
				m_bAttack = true;
			}
		}
	}
	else if (pClip->GetClipName() == "SK_Firing_01")
	{
		CAnimationState* pAnimationState = m_pStateController->GetCurrentState();
		if (pAnimationState)
		{
			CAnimationClip* pClip = pAnimationState->GetClip();
			if (pClip->GetProgress() >= 0.f && pClip->GetProgress() <= 0.1f &&
				!m_bSoundPlay)
			{
				m_bSoundPlay = true;
				GET_SINGLE(CSoundManager)->Play("Dragon_HeadAttack");
			}

			if (pAnimationState->End())
			{
				m_bSoundPlay = false;
				// Idle_C 로 간다
				m_pStateController->SetInteger("State", 0);
			}
			// 공격 판정
			else if (pClip->GetProgress() >= 0.3f && pClip->GetProgress() <= 0.4f
				&& m_bAttack == false)
			{
				CGameObject* pAttackObj = CGameObject::CreateObject("Attack", m_pLayer);
				CSphereCollider* pCollider = pAttackObj->AddComponent<CSphereCollider>();
				pCollider->SetRadius(15.f);
				SAFE_RELEASE(pCollider);
				CAttack* pAttack = pAttackObj->AddComponent<CAttack>();
				pAttack->SetValue(-1);
				pAttack->SetAttribute(ATTACK_DOWN);
				pAttack->SetDamagePoint(50.f);
				pAttack->SetActiveMaxTime(0.5f);
				SAFE_RELEASE(pAttack);
				CTransform* pTr = pAttackObj->GetTransform();
				pTr->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * 10.f);
				SAFE_RELEASE(pTr);
				SAFE_RELEASE(pAttackObj);
				m_bAttack = true;
			}

			else if (pClip->GetProgress() >= 0.4f && pClip->GetProgress() <= 0.5f && m_bAttack)
			{
				m_bAttack = false;
			}
			else if (pClip->GetProgress() >= 0.75f && pClip->GetProgress() <= 0.8f && !m_bAttack)
			{
				CGameObject* pAttackObj = CGameObject::CreateObject("Attack", m_pLayer);
				CSphereCollider* pCollider = pAttackObj->AddComponent<CSphereCollider>();
				pCollider->SetRadius(5.f);
				SAFE_RELEASE(pCollider);
				CAttack* pAttack = pAttackObj->AddComponent<CAttack>();
				pAttack->SetValue(-1);
				pAttack->SetAttribute(ATTACK_DOWN);
				pAttack->SetDamagePoint(50.f);
				pAttack->SetActiveMaxTime(0.5f);
				SAFE_RELEASE(pAttack);
				CTransform* pTr = pAttackObj->GetTransform();
				pTr->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * 5.f);
				SAFE_RELEASE(pTr);
				SAFE_RELEASE(pAttackObj);
				m_bAttack = true;
				GET_SINGLE(CSoundManager)->Play("Dragon_Attack");
			}
		}
	}
	else if (pClip->GetClipName() == "SK_Firing_02")
	{
		CAnimationState* pAnimationState = m_pStateController->GetCurrentState();
		if (pAnimationState)
		{

			CAnimationClip* pClip = pAnimationState->GetClip();
			if (pClip->GetProgress() >= 0.1f && pClip->GetProgress() <= 0.2f && !m_bSoundPlay)
			{
				m_bSoundPlay = true;
				GET_SINGLE(CSoundManager)->Play("Dragon_Breath");
			}

			if (pAnimationState->End())
			{
				// Idle_C 로 간다
				m_pStateController->SetInteger("State", 0);

				// 모션블러 끔
				CGameObject* pMainCameraObject = m_pScene->GetMainCameraObject();
				CPostProcessing* pPostProcessing = pMainCameraObject->GetComponent<CPostProcessing>();
				if (pPostProcessing)
				{
					uint32_t iFlags = pPostProcessing->GetPostProcessingFlags();
					if (iFlags & PT_MOTION_BLUR)
						pPostProcessing->ToggleFlags(PT_MOTION_BLUR);
					SAFE_RELEASE(pPostProcessing);
				}
				SAFE_RELEASE(pMainCameraObject);

				m_bSoundPlay = false;
				m_bAttack = false;
				m_bEffect = false;
			}
			// 공격 판정
			else if (pClip->GetProgress() >= 0.5f
				&& m_bAttack == false)
			{
				CGameObject* pAttackObj = CGameObject::CreateObject("Attack", m_pLayer);
				CSphereCollider* pCollider = pAttackObj->AddComponent<CSphereCollider>();
				pCollider->SetRadius(30.f);
				SAFE_RELEASE(pCollider);
				CAttack* pAttack = pAttackObj->AddComponent<CAttack>();
				pAttack->SetValue(-1);
				pAttack->SetAttribute(ATTACK_DOWN);
				pAttack->SetDamagePoint(50.f);
				pAttack->SetActiveMaxTime(0.5f);
				SAFE_RELEASE(pAttack);
				CTransform* pTr = pAttackObj->GetTransform();
				pTr->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * 5.f);
				SAFE_RELEASE(pTr);
				SAFE_RELEASE(pAttackObj);
				m_bAttack = true;

				// 모션블러킴
				CGameObject* pMainCameraObject = m_pScene->GetMainCameraObject();
				CPostProcessing* pPostProcessing = pMainCameraObject->GetComponent<CPostProcessing>();
				if (pPostProcessing)
				{
					uint32_t iFlags = pPostProcessing->GetPostProcessingFlags();
					if ((iFlags & PT_MOTION_BLUR) == false)
						pPostProcessing->ToggleFlags(PT_MOTION_BLUR);
					SAFE_RELEASE(pPostProcessing);
				}
				SAFE_RELEASE(pMainCameraObject);
			}

			// Effect
			else if (pClip->GetProgress() >= 0.5f
				&& m_bAttack && !m_bEffect)
			{
				m_bEffect = true;

				//	Shake Camera
				{
					CGameObject*	pPlayerCameraGimbal = GET_SINGLE(CDataManager)->GetCameraGimbal();
					if (pPlayerCameraGimbal)
					{
						CPlayerCamera*	pPlayerCamera = pPlayerCameraGimbal->GetComponent<CPlayerCamera>();
						if (pPlayerCamera)
						{
							pPlayerCamera->StartShake(0.5f, 20.f, 2.0f);
							SAFE_RELEASE(pPlayerCamera);
						}
					}
				}
			}
		}
	}
	else if (pClip->GetClipName() == "SK_Firing_03")
	{
		CAnimationState* pAnimationState = m_pStateController->GetCurrentState();
		if (pAnimationState)
		{
			CAnimationClip* pClip = pAnimationState->GetClip();
			if (pClip->GetProgress() >= 0.1f && pClip->GetProgress() <= 0.2f && !m_bSoundPlay)
			{
				m_bSoundPlay = true;
				GET_SINGLE(CSoundManager)->Play("Dragon_Breath");				
			}

			if (pAnimationState->End())
			{
				m_bSoundPlay = false;
				// Idle_C 로 간다
				m_pStateController->SetInteger("State", 0);
			}
			
			// 공격 판정
			else if (pClip->GetProgress() >= 0.5f
				&& m_bAttack == false)
			{
				size_t iSize = m_vecFireEffects.size();
				for (int i = 0; i < iSize; ++i)
				{
					m_vecFireEffects[i]->Enable(true);
				}

				m_bAttack = true;

				
			}
		}
	}
	else if (pClip->GetClipName() == "DMG_B")
	{
		m_fActiveMaxTime = 0.5f;
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;
		if (pClip->GetProgress() >= 0.1f && pClip->GetProgress() <= 0.2f && !m_bSoundPlay)
		{
			m_bSoundPlay = true;
			GET_SINGLE(CSoundManager)->Play("Dragon_Damage");
		}

		if (fProgress >= 1.f)
		{
			m_bSoundPlay = false;
			m_fActiveTime = 0.f;
			// Idle_C 로 간다
			m_pStateController->SetInteger("State", 0);
		}
	}
	else if (pClip->GetClipName() == "DMG_F")
	{
		m_fActiveMaxTime = 0.5f;
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;
		if (pClip->GetProgress() >= 0.1f && pClip->GetProgress() <= 0.2f && !m_bSoundPlay)
		{
			m_bSoundPlay = true;
			GET_SINGLE(CSoundManager)->Play("Dragon_Damage");
		}

		if (fProgress >= 1.f)
		{
			m_bSoundPlay = false;
			m_fActiveTime = 0.f;
			// Idle_C 로 간다
			m_pStateController->SetInteger("State", 0);
		}
	}
	else if (pClip->GetClipName() == "Passout")
	{
		CAnimationState* pAnimationState = m_pStateController->GetCurrentState();
		if (pAnimationState)
		{
			CAnimationClip* pClip = pAnimationState->GetClip();
			if (pAnimationState->End())
			{
				// 죽는모션 
				m_pStateController->SetInteger("State", 10);
				m_fActiveTime = 0.f;
			}
		}
	}
	else if (pClip->GetClipName() == "Die")
	{
		m_fActiveMaxTime = 3.f;
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;
		if (fProgress >= 0.9f)
		{
			m_pGameObject->Destroy(true);
		}
		else if (fProgress >= 0.33f)
		{
			CGameObject* pMeshCore = m_pTransform->GetChild(0)->GetGameObject();
			CEffect* pEffect = pMeshCore->GetComponent<CEffect>();

			if (pEffect)
			{
				pEffect->SetFlags(VF_BURN | VF_LIMLIGHT);
				pEffect->SetLimitTime(2.f);
			}
			else
			{
				pEffect = pMeshCore->AddComponent<CEffect>();
				pEffect->SetFlags(VF_BURN | VF_LIMLIGHT);
				pEffect->SetLimitTime(2.f);
			}
			SAFE_RELEASE(pEffect);
			SAFE_RELEASE(pMeshCore);
		}

		size_t iSize = m_vecFireEffects.size();
		for (int i = 0; i < iSize; ++i)
		{
			CGameObject* pMeshCore = m_pTransform->GetChild(0)->GetGameObject();
			CAnimator* pAnimator = pMeshCore->GetComponent<CAnimator>();
			if (pAnimator)
			{
				CTransform* pFireTransform = m_vecFireEffects[i]->GetTransform();
				Vector3 vLocalScale = pFireTransform->GetLocalScale();
				vLocalScale = vLocalScale - vLocalScale * fProgress;
				pFireTransform->SetLocalScale(vLocalScale);
				SAFE_RELEASE(pFireTransform);
				SAFE_RELEASE(pAnimator);
			}
		}
	}
}

void CBoneDragonAI::SetRage(float fTime)
{
	if (m_bRage)
		return;

	CSphereCollider* pBoneDragonCollider = m_pGameObject->GetComponent<CSphereCollider>();
	pBoneDragonCollider->Enable(false);
	SAFE_RELEASE(pBoneDragonCollider);

	m_pStateController->SetInteger("State", 6);

	CAnimationState* pAnimationState = m_pStateController->GetCurrentState();
	CAnimationClip* pClip = pAnimationState->GetClip();

	if (pClip->GetClipName() == "SK_Firing_03")
	{
		if (pClip->GetProgress() >= 0.1f && pClip->GetProgress() <= 0.2f && !m_bSoundPlay)
		{
			m_bSoundPlay = true;
			GET_SINGLE(CSoundManager)->Play("Dragon_Breath");
		}

		if (pAnimationState->End())
		{
			m_bSoundPlay = false;
			// Idle_C 로 간다
			m_pStateController->SetInteger("State", 0);

			m_bRage = true;
			m_bRageOn = false;
			m_bEffect = false;

			CSphereCollider* pBoneDragonCollider = m_pGameObject->GetComponent<CSphereCollider>();
			pBoneDragonCollider->Enable(true);
			SAFE_RELEASE(pBoneDragonCollider);
		}

		// 불 켜짐
		else if (pClip->GetProgress() >= 0.5f)
		{
			size_t iSize = m_vecFireEffects.size();
			for (int i = 0; i < iSize; ++i)
			{
				m_vecFireEffects[i]->Enable(true);
			}

			if (!m_bEffect)
			{
				m_bEffect = true;
				//	Shake Camera
				{
					CGameObject*	pPlayerCameraGimbal = GET_SINGLE(CDataManager)->GetCameraGimbal();
					if (pPlayerCameraGimbal)
					{
						CPlayerCamera*	pPlayerCamera = pPlayerCameraGimbal->GetComponent<CPlayerCamera>();
						if (pPlayerCamera)
						{
							pPlayerCamera->StartShake(1.5f, 20.f, 2.0f);
							SAFE_RELEASE(pPlayerCamera);
						}
					}
				}
			}
		} 
	}
}

bool CBoneDragonAI::Initialize()
{
	return true;
}

void CBoneDragonAI::Start()
{
	CLayer* pPlayerContainedLayer = m_pScene->FindLayer("Default");
	if (pPlayerContainedLayer)
	{
		CGameObject* pPlayer = pPlayerContainedLayer->FindGameObjectWithTag("Player"); // MUST BE UNIQUE
		m_pPlayer = pPlayer;
		SAFE_RELEASE(pPlayer);
	}
	SAFE_RELEASE(pPlayerContainedLayer);


	CLayer* pUILayer = m_pScene->FindLayer("UI");
	CGameObject* pHealthBar = CGameObject::CreateObject("HealthBar", pUILayer);
	CBossHealthBar* pBossHPBar = pHealthBar->AddComponent<CBossHealthBar>();
	pBossHPBar->SetLineCount(5);
	SAFE_RELEASE(pBossHPBar);
	m_pHealthBar = pHealthBar;
	SAFE_RELEASE(pHealthBar);
	SAFE_RELEASE(pUILayer);
	m_pHealthBar->Enable(false);

	CSphereCollider* pColl = m_pGameObject->AddComponent<CSphereCollider>();
	pColl->SetLocalPosition(0.f, 2.f, 0.f);
	pColl->SetRadius(8.f);
	pColl->Enable(false);
	SAFE_RELEASE(pColl);

	// 불 이펙트 생성
	wstring strPath = L"Prefab\\Effects\\PurpleFire_BoneDragon_Wing.SSSPrefab";
	for (int i = 0; i < 1; ++i)
	{
		CGameObject* pFireObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
		m_vecFireEffects.push_back(pFireObject);
	}

	strPath = L"Prefab\\Effects\\PurpleFire_BoneDragon.SSSPrefab";
	for (int i = 1; i < 29; ++i)
	{
		CGameObject* pFireObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
		m_vecFireEffects.push_back(pFireObject);
	}

	strPath = L"Prefab\\Effects\\PurpleFire_BoneDragon_Wing.SSSPrefab";
	for (int i = 0; i < 31; ++i)
	{
		CGameObject* pFireObject = CGameObject::Load(m_pScene, strPath.c_str(), DATA_PATH);
		m_vecFireEffects.push_back(pFireObject);
	}

	CGameObject* pMeshCore = m_pTransform->GetChild(0)->GetGameObject();
	CAnimator* pAnimator = pMeshCore->GetComponent<CAnimator>();
	if (pAnimator)
	{
		CAnimationStateController* pController = pAnimator->GetAnimationController();
		if (pController)
		{
			// 많이 쓸꺼라 멤버변수로 받아놓..
			m_pStateController = pController;
		}

		CTransform* pFireTransform = m_vecFireEffects[0]->GetTransform();
		pFireTransform->SetLocalScale(1.f, 1.3f, 1.f);
		string strBoneName = "Bip01-Head_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[1]->GetTransform();
		pFireTransform->SetLocalScale(1.f, 1.5f, 1.f);
		strBoneName = "Bip01-Spine1";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[2]->GetTransform();
		pFireTransform->SetLocalScale(1.f, 1.25f, 1.f);
		strBoneName = "Bip01-Spine2";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[3]->GetTransform();
		strBoneName = "Bip01-Spine3";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[4]->GetTransform();
		pFireTransform->SetLocalScale(1.f, 1.3f, 1.f);
		strBoneName = "Bip01-Spine";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[5]->GetTransform();
		strBoneName = "Bip01-Pelvis";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		Vector3 vFireFinger = Vector3(0.3f, 0.3f, 1.f);

		pFireTransform = m_vecFireEffects[6]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-L-Finger01_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[7]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-L-Finger11_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[8]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-L-Finger21_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[9]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-L-Finger31_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[10]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-L-Finger41_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[11]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-R-Finger01_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[12]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-R-Finger11_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[13]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-R-Finger21_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[14]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-R-Finger31_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[15]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-R-Finger41_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[16]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-R-Toe01_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[17]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-R-Toe11_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[18]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-R-Toe21_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[19]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-R-Toe31_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[20]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-R-Toe41_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[21]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-L-Toe01_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[22]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-L-Toe11_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[23]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-L-Toe21_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[24]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-L-Toe31_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[25]->GetTransform();
		pFireTransform->SetLocalScale(vFireFinger);
		strBoneName = "Bip01-L-Toe41_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[26]->GetTransform();
		pFireTransform->SetLocalScale(0.7f, 1.f, 1.f);
		strBoneName = "Bip01-L-Thigh";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[27]->GetTransform();
		pFireTransform->SetLocalScale(0.7f, 1.f, 1.f);
		strBoneName = "Bip01-R-Thigh";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[28]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "Bip01-Neck_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		// 날개
		Vector3 vWingSclae = Vector3(0.5f, 0.75f, 1.f);
		pFireTransform = m_vecFireEffects[29]->GetTransform();
		pFireTransform->SetLocalScale(vWingSclae);
		strBoneName = "BN_Wing_L001";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[30]->GetTransform();
		pFireTransform->SetLocalScale(vWingSclae);
		strBoneName = "BN_Wing_L002";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[31]->GetTransform();
		pFireTransform->SetLocalScale(vWingSclae);
		strBoneName = "BN_Wing_L003";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[32]->GetTransform();
		pFireTransform->SetLocalScale(vWingSclae);
		strBoneName = "BN_Wing_L009";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[33]->GetTransform();
		pFireTransform->SetLocalScale(vWingSclae);
		strBoneName = "BN_Wing_L014";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[34]->GetTransform();
		pFireTransform->SetLocalScale(vWingSclae);
		strBoneName = "BN_Wing_L019";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[35]->GetTransform();
		pFireTransform->SetLocalScale(vWingSclae);
		strBoneName = "BN_Wing_L024";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[36]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_L010";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[37]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_L015";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[38]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_L020";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[39]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_L025";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[40]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_L008";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[41]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_L013";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[42]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_L018";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[43]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_L023";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[44]->GetTransform();
		pFireTransform->SetLocalScale(vWingSclae);
		strBoneName = "BN_Wing_R001";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[45]->GetTransform();
		pFireTransform->SetLocalScale(vWingSclae);
		strBoneName = "BN_Wing_R002";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[46]->GetTransform();
		pFireTransform->SetLocalScale(vWingSclae);
		strBoneName = "BN_Wing_R003";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[47]->GetTransform();
		pFireTransform->SetLocalScale(vWingSclae);
		strBoneName = "BN_Wing_R009";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[48]->GetTransform();
		pFireTransform->SetLocalScale(vWingSclae);
		strBoneName = "BN_Wing_R014";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[49]->GetTransform();
		pFireTransform->SetLocalScale(vWingSclae);
		strBoneName = "BN_Wing_R019";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[50]->GetTransform();
		pFireTransform->SetLocalScale(vWingSclae);
		strBoneName = "BN_Wing_R024";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[51]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_R010";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[52]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_R015";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[53]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_R020";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[54]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_R025";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[55]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_R008";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[56]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_R013";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[57]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_R018";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
		pFireTransform = m_vecFireEffects[58]->GetTransform();
		pFireTransform->SetLocalScale(0.5f, 1.f, 1.f);
		strBoneName = "BN_Wing_R023";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);

		pFireTransform = m_vecFireEffects[59]->GetTransform();
		pFireTransform->SetLocalScale(1.f, 1.f, 1.f);
		strBoneName = "Bip01-Neck7_Bone";
		pAnimator->RegisterSocket(strBoneName, pFireTransform);
		m_vecFireEffectString.push_back(strBoneName);
		SAFE_RELEASE(pFireTransform);
	}

	SAFE_RELEASE(pAnimator);
	SAFE_RELEASE(pMeshCore);



	size_t iSize = m_vecFireEffects.size();
	for (int i = 0; i < iSize; ++i)
	{
		m_vecFireEffects[i]->Enable(false);
	}
}

int CBoneDragonAI::Update(float fTime)
{
	if (m_pPlayer == NULL)
		return 0;

	// 일어날때
	if (m_bSpawn)
	{
		if (m_bSpawnStart == false)
		{
			CTransform* pPlayerTr = m_pPlayer->GetTransform();
			Vector3 vPlayerPosition = pPlayerTr->GetWorldPosition();
			SAFE_RELEASE(pPlayerTr);
			Vector3 vMyPosition = m_pTransform->GetWorldPosition();
			Vector3 vDir = vPlayerPosition - vMyPosition;
			float	fDistance = vDir.Length();
			if (fDistance <= 40.f)
			{
				m_pStateController->SetInteger("State", -10);

				CAnimationState* pAnimationState = m_pStateController->GetCurrentState();
				CAnimationClip* pClip = pAnimationState->GetClip();
				if (pClip->GetClipName() == "Spawn")
					m_bSpawnStart = true;
				return 0;
			}
			else
				return 0;
		}

		CAnimationState* pAnimationState = m_pStateController->GetCurrentState();
		if (pAnimationState)
		{
			if (pAnimationState->End())
			{
				m_pStateController->SetInteger("State", 0);
				m_bSpawn = false;
				m_bSoundPlay = false;

				m_pHealthBar->Enable(true);

				CSphereCollider* pMyCollider = m_pGameObject->GetComponent<CSphereCollider>();
				pMyCollider->Enable(true);
				SAFE_RELEASE(pMyCollider);
			}

			else if (!m_bSoundPlay)
			{
				m_bSoundPlay = true;
				GET_SINGLE(CSoundManager)->Play("Dragon_Spawn");
			}
		}
		return 0;
	}

	if (m_pPlayer == NULL)
	{
		CLayer* pPlayerContainedLayer = m_pScene->FindLayer("Default");
		if (pPlayerContainedLayer)
		{
			CGameObject* pPlayer = pPlayerContainedLayer->FindGameObjectWithTag("Player"); // MUST BE UNIQUE
			m_pPlayer = pPlayer;
			SAFE_RELEASE(pPlayer);
		}
		SAFE_RELEASE(pPlayerContainedLayer);
	}


	Vector3 position = m_pTransform->GetWorldPosition();
	RAY tRay;
	tRay.vOrigin = position + Vector3(0.0f, 4.0f, 0.0f);
	tRay.vDirection = Vector3(0.0f, -1.0f, 0.0f);
	tRay.fDistance = 50.0f;

	LINECASTHIT tHit = GET_SINGLE(CCollisionManager)->LineCast(tRay);

	float fResult = -10000.f;
	for (int i = 0; i < tHit.iCount; ++i)
	{
		if (tHit.vecHits[i].bResult)
		{
			if (tHit.vecHits[i].pCollider->IsComponent<CTerrainCollider>())
				if (fResult < tHit.vecHits[i].vPoint.y)
					fResult = tHit.vecHits[i].vPoint.y;

			if (tHit.vecHits[i].pCollider->IsComponent<CMeshCollider>())
			{
				CRigidbody* pRigidbody = tHit.vecHits[i].pCollider->GetComponent<CRigidbody>();
				if (pRigidbody == NULL)
				{
					if (fResult < tHit.vecHits[i].vPoint.y)
						fResult = tHit.vecHits[i].vPoint.y;
				}
				SAFE_RELEASE(pRigidbody);
			}

			if (tHit.vecHits[i].pCollider->IsComponent<CBoxCollider>())
				if (fResult < tHit.vecHits[i].vPoint.y)
					fResult = tHit.vecHits[i].vPoint.y;
		}
	}

	if (fResult != -10000.f)
		m_pTransform->SetWorldPosition(position.x, fResult, position.z);

	position.y += 0.5f;
	tRay.vOrigin = position;
	tRay.vDirection = m_pTransform->forward();
	tRay.fDistance = 2.0f;
	tHit = GET_SINGLE(CCollisionManager)->LineCast(tRay);

	for (int i = 0; i < tHit.iCount; ++i)
	{
		if (tHit.vecHits[i].bResult)
		{
			if (tHit.vecHits[i].pCollider->IsComponent<CMeshCollider>())
			{
				CRigidbody* pRigidbody = tHit.vecHits[i].pCollider->GetComponent<CRigidbody>();
				if (pRigidbody == NULL)
				{

					Vector3 vFinalPosition = m_pTransform->GetWorldPosition();
					vFinalPosition.x = tHit.vecHits[i].vPoint.x;
					vFinalPosition.z = tHit.vecHits[i].vPoint.z;
					vFinalPosition -= tRay.vDirection * tHit.vecHits[i].fDistamce;
					m_pTransform->SetWorldPosition(vFinalPosition);
				}
				SAFE_RELEASE(pRigidbody);
			}

			if (tHit.vecHits[i].pCollider->IsComponent<CBoxCollider>())
			{
				Vector3 vFinalPosition = m_pTransform->GetWorldPosition();
				vFinalPosition.x = tHit.vecHits[i].vPoint.x;
				vFinalPosition.z = tHit.vecHits[i].vPoint.z;
				vFinalPosition -= tRay.vDirection * tHit.vecHits[i].fDistamce;
				m_pTransform->SetWorldPosition(vFinalPosition);
			}
		}
	}

	if (m_bRageOn == false
		&& m_bRage == false)
	{
		float fProgress = m_fHP / m_fHPMax;
		if (fProgress <= 0.5f)
		{
			m_bRageOn = true;
			m_bEffect = false;
		}
	}

	if (m_bRageOn)
		SetRage(fTime);
	else
		TransState(fTime);
	
	if (m_pHealthBar)
	{
		CBossHealthBar* pHPBar = m_pHealthBar->GetComponent<CBossHealthBar>();
		if (pHPBar)
		{
			pHPBar->UpdateHealthPoint(m_fHP, m_fHPMax);
		}
		SAFE_RELEASE(pHPBar);
	}

	return 0;
}

int CBoneDragonAI::LateUpdate(float fTime)
{
	size_t iSize = m_vecFireEffects.size();
	for (int i = 0; i < iSize; ++i)
	{
		CGameObject* pMeshCore = m_pTransform->GetChild(0)->GetGameObject();
		CAnimator* pAnimator = pMeshCore->GetComponent<CAnimator>();
		if (pAnimator)
		{
			CTransform* pFireTransform = m_vecFireEffects[i]->GetTransform();

			Matrix matLocal = pFireTransform->GetLocalMatrix();

			Matrix matBone = pAnimator->GetBoneMatrix(m_vecFireEffectString[i]);

			Matrix matScale;
			matScale = Matrix::Scaling(Vector3(100.0f, 100.0f, 100.0f) * pFireTransform->GetLocalScale());

			matLocal = matScale * matLocal * matBone;

			pFireTransform->SetWorldMatrix(matLocal);

			SAFE_RELEASE(pFireTransform);

			SAFE_RELEASE(pAnimator);
		}
	}

	return 0;
}

int CBoneDragonAI::OnCollisionEnter(CCollider * pThis, CCollider * pTarget, float fTime)
{
	CAttack* pAttack = pTarget->GetComponent<CAttack>();
	if (pAttack)
	{
		if (pAttack->GetValue() == 1)
		{
			if (m_fHP <= 0.f)
			{
				SAFE_RELEASE(pAttack);
				return 0;
			}
			CGameObject* pMonsterCore = m_pTransform->GetChild(0)->GetGameObject();

			Vector3	vMonster = m_pTransform->GetWorldPosition();
			float	fRadius = ((CSphereCollider*)pTarget)->GetRadius();

			// LimLight
			//{
			//	if (!pMonsterCore->HasComponent<CEffectController>())
			//	{
			//		CEffectController*	pEffectController = pMonsterCore->AddComponent<CEffectController>("LimLight_Ctrl");
			//		pEffectController->SetLimLightInfo(0.1f, Vector3(0.6f, 0.5f, 0.5f), 0.25f, false);
			//		SAFE_RELEASE(pEffectController);
			//	}
			//	else
			//	{
			//		CEffectController*	pEffectController = pMonsterCore->GetComponent<CEffectController>();
			//		pEffectController->SetLimLightInfo(0.1f, Vector3(0.6f, 0.5f, 0.5f), 0.25f, false);
			//		SAFE_RELEASE(pEffectController);
			//	}
			//}

			m_fHP -= pAttack->GetDamagePoint();
			if (m_fHP <= 0.f)
			{
				// 죽었을경우 
				m_fActiveTime = 0.f;
				m_pStateController->SetInteger("State", 9);
			}
			else
			{
				// 경직을 주지 않는게 나을듯 하다능..
			}

			SAFE_RELEASE(pMonsterCore);

		}
		SAFE_RELEASE(pAttack);
	}

	return 0;
}

int CBoneDragonAI::OnCollisionStay(CCollider * pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

int CBoneDragonAI::OnCollisionExit(CCollider * pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

bool CBoneDragonAI::Save(FILE * pFile)
{
	return true;
}

bool CBoneDragonAI::Load(FILE * pFile)
{
	return true;
}
