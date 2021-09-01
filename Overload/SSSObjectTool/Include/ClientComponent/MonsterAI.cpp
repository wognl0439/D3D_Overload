#include "MonsterAI.h"
#include "Collider.h"
#include "AnimationStateController.h"
#include "Component/Animator.h"
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

#include "EffectController.h"
#include "Component/Billboard.h"
#include "Component/Particle.h"
#include "Component/HealthBar.h"
#include "Mathf.h"
#include "Component/BoxCollider.h"

#include "Soundmanager.h"
#include "Component/Rigidbody.h"


CMonsterAI::CMonsterAI()
{
	m_bAggressive = false;
	m_eMonsterBehaviourState = CS_NONE;
	m_pPlayer = NULL;
	m_pHealthBar = NULL;
	m_eState = NPC_NONE;
	m_eMonsterType = MT_NONE;

	m_fActiveTime = 0.f;
	m_fActiveMaxTime = 1.f;
	m_fMoveSpeed = 5.f;
	m_fHPMax = 100.0f;
	m_fHP = m_fHPMax;

	m_fCoolDownTime = 0.f;
	m_fCurrentCoolDownTime = 0.f;
	m_bIsAttack = false;
	m_bSoundPlay = false;
}

CMonsterAI::~CMonsterAI()
{
	if (m_pHealthBar)
	{
		CHealthBar* pBar = m_pHealthBar->GetComponent<CHealthBar>();
		if (pBar)
		{
			pBar->SetBaseObject(NULL);
		}
		SAFE_RELEASE(pBar);
	}
}

void CMonsterAI::TransState(float fTime)
{
	if (m_pPlayer == NULL)
		return;

	CTransform* pPlayerTr = m_pPlayer->GetTransform();
	Vector3 vPlayerPosition = pPlayerTr->GetWorldPosition();
	Vector3 vPosition = m_pTransform->GetWorldPosition();
	Vector3 vDirectionToPlayer = vPlayerPosition - vPosition;
	float fDistance = vDirectionToPlayer.Length();
	SAFE_RELEASE(pPlayerTr);

	switch (m_eState)
	{
	case NPC_IDLE1:
	{
		m_eMonsterBehaviourState = CS_ON_IDLE;
		if (fDistance < 20.f) // 임시
		{
			// Agro
			m_bAggressive = true;
			m_eState = NPC_RUN_FRONT;
			m_fActiveTime = 0.f;
			return;
		}

		m_fActiveMaxTime = 2.f;
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;

		if (fProgress < 1.f)
		{
			return;
		}

		int iRandom = rand() % 10;
		if (iRandom > 5)
		{
			m_pTransform->SetWorldRotationY(RandomRange(-PI, PI));
			m_eState = NPC_WALK_FRONT;
		}
		else
		{
			m_eState = NPC_IDLE2;
		}

		m_fActiveTime = 0.f;
	}
	break;

	case NPC_IDLE2:
	{
		m_eMonsterBehaviourState = CS_ON_IDLE;
		if (fDistance < 20.f) // 임시
		{
			// Agro
			m_bAggressive = true;
			m_eState = NPC_RUN_FRONT;
			m_fActiveTime = 0.f;
			return;
		}

		m_fActiveMaxTime = 2.f;
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;

		if (fProgress < 1.f)
		{
			return;
		}

		int iRandom = rand() % 10;
		if (iRandom > 5)
		{
			m_pTransform->SetWorldRotationY(RandomRange(-PI, PI));
			m_eState = NPC_WALK_FRONT;
			m_fActiveTime = 0.f;
		}
		else
		{
			m_eState = NPC_IDLE1;
			m_fActiveTime = 0.f;
		}
	}
	break;

	case NPC_WALK_FRONT:
	{
		m_fActiveMaxTime = 2.f;
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;

		if (fProgress >= 1.f)
		{
			m_fActiveTime = 0.f;
			m_eState = NPC_IDLE1;
		}
		Vector3 vPos = vPosition + (m_pTransform->forward() * m_fMoveSpeed * fTime);

		if (vPos.x > 254.f)
			vPos.x = 254.f;
		if (vPos.x < -254.f)
			vPos.x = 254.f;
		if (vPos.z > 254.f)
			vPos.z = 254.f;
		if (vPos.z < -254.f)
			vPos.z = -254.f;

		m_pTransform->SetWorldPosition(vPos);
	}
	break;

	case NPC_RUN_FRONT:
	{
		m_eMonsterBehaviourState = CS_ON_CHASE;
		m_fActiveMaxTime = 3.f;
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;

		if (fProgress >= 1.f)
		{
			m_bAggressive = false;
			m_fActiveTime = 0.f;
			m_eState = NPC_IDLE1;
		}
		vDirectionToPlayer.y = 0.f;
		float fDistanceExceptY = vDirectionToPlayer.Length();

		vDirectionToPlayer.normalize();

		Vector3 vPos = vPosition + (vDirectionToPlayer * m_fMoveSpeed * 1.5f * fTime);

		if (vPos.x > 254.f)
			vPos.x = 254.f;
		if (vPos.x < -254.f)
			vPos.x = 254.f;
		if (vPos.z > 254.f)
			vPos.z = 254.f;
		if (vPos.z < -254.f)
			vPos.z = -254.f;

		m_pTransform->SetWorldPosition(vPos);

		//Vector3 vMovePoint = vPosition + (vDirectionToPlayer * m_fMoveSpeed * 1.5f * fTime);
		//m_pTransform->SetWorldPosition(vMovePoint);
		vPlayerPosition.y = vPosition.y;
		m_pTransform->LookAt(vPlayerPosition);

		// 너무 멀리 있으면 Idle로 돌아갑니다
		if (fDistance > 50.f)
		{
			m_fActiveTime = 0.f;
			m_eState = NPC_IDLE1;
		}
		// 공격 범위 안에 있으면 공격합니당
		else if (fDistanceExceptY < 5.f)
		{
			m_fActiveTime = 0.f;

			int iRandom = rand() % 10;
			if (iRandom > 5)
				m_eState = NPC_ATTACK2;
			else
				m_eState = NPC_ATTACK1;
		}
	}
	break;

	case NPC_ATTACK1:
	{
		m_eMonsterBehaviourState = CS_ON_ATTACK;

		m_fActiveMaxTime = 1.f;
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;
		// 다시 추적 합니당
		if (fProgress >= 1.f)
		{
			m_fActiveTime = 0.f;
			m_eState = NPC_RUN_FRONT;
			m_bSoundPlay = false;
		}
		// 공격 판정
		else if (fProgress >= 0.5f && !m_bIsAttack)
		{
			CGameObject* pAttackObj = CGameObject::CreateObject("Attack", m_pLayer);
			CSphereCollider* pCollider = pAttackObj->AddComponent<CSphereCollider>();
			pCollider->SetRadius(3.f);
			SAFE_RELEASE(pCollider);
			CAttack* pAttack = pAttackObj->AddComponent<CAttack>();
			pAttack->SetValue(-1);
			pAttack->SetAttribute(ATTACK_NORMAL);
			pAttack->SetDamagePoint(50.f);
			pAttack->SetActiveMaxTime(0.5f);
			SAFE_RELEASE(pAttack);
			CTransform* pTr = pAttackObj->GetTransform();
			pTr->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * 2.f);
			SAFE_RELEASE(pTr);
			SAFE_RELEASE(pAttackObj);
			m_bIsAttack = true;
		}

		else if (!m_bSoundPlay && fProgress <= 0.1f)
		{
			m_bSoundPlay = true;
			switch (m_eMonsterType)
			{
			case MT_RABBIT:
				GET_SINGLE(CSoundManager)->Play("Rabbit_Attack");
				break;
			case MT_HAMSTER:
				GET_SINGLE(CSoundManager)->Play("Hamster_Attack1");
				break;
			case MT_YETI:
				GET_SINGLE(CSoundManager)->Play("Yeti_Attack1");
				break;
			case MT_ZOMBIE:
				GET_SINGLE(CSoundManager)->Play("Zombie_Attack");
				break;
			case MT_SUCCUBUS:
				GET_SINGLE(CSoundManager)->Play("Succubus_Attack1");
				break;
			case MT_DRYAD:
				GET_SINGLE(CSoundManager)->Play("Dryad_Attack1");
				break;
			case MT_HARPYQ:
				GET_SINGLE(CSoundManager)->Play("HarpyQ_Attack1");
				break;
			case MT_GHOSTWARRIOR:
				GET_SINGLE(CSoundManager)->Play("GhostWarrior_Attack1");
				break;
			case MT_DARKKNIGHT:
				GET_SINGLE(CSoundManager)->Play("DarkKnight_Attack1");
				break;
			}
		}
	}
	break;

	case NPC_ATTACK2:
	{
		m_eMonsterBehaviourState = CS_ON_ATTACK;

		m_fActiveMaxTime = 1.f;
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;
		// 다시 추적 합니당
		if (fProgress >= 1.f)
		{
			m_fActiveTime = 0.f;
			m_eState = NPC_RUN_FRONT;
			m_bSoundPlay = false;
		}
		// 공격 판정
		else if (fProgress >= 0.5f && !m_bIsAttack)
		{
			CGameObject* pAttackObj = CGameObject::CreateObject("Attack", m_pLayer);
			CSphereCollider* pCollider = pAttackObj->AddComponent<CSphereCollider>();
			pCollider->SetRadius(3.f);
			SAFE_RELEASE(pCollider);
			CAttack* pAttack = pAttackObj->AddComponent<CAttack>();
			pAttack->SetValue(-1);
			pAttack->SetAttribute(ATTACK_DOWN);
			pAttack->SetDamagePoint(500.f);
			pAttack->SetActiveMaxTime(0.5f);
			SAFE_RELEASE(pAttack);
			CTransform* pTr = pAttackObj->GetTransform();
			pTr->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * 2.f);
			SAFE_RELEASE(pTr);
			SAFE_RELEASE(pAttackObj);
			m_bIsAttack = true;
		}

		else if (!m_bSoundPlay && fProgress <= 0.1f)
		{
			m_bSoundPlay = true;
			switch (m_eMonsterType)
			{
			case MT_RABBIT:
				GET_SINGLE(CSoundManager)->Play("Rabbit_Attack");
				break;
			case MT_HAMSTER:
				GET_SINGLE(CSoundManager)->Play("Hamster_Attack2");
				break;
			case MT_YETI:
				GET_SINGLE(CSoundManager)->Play("Yeti_Attack2");
				break;
			case MT_ZOMBIE:
				GET_SINGLE(CSoundManager)->Play("Zombie_Attack");
				break;
			case MT_SUCCUBUS:
				GET_SINGLE(CSoundManager)->Play("Succubus_Attack2");
				break;
			case MT_DRYAD:
				GET_SINGLE(CSoundManager)->Play("Dryad_Attack2");
				break;
			case MT_HARPYQ:
				GET_SINGLE(CSoundManager)->Play("HarpyQ_Attack2");
				break;
			case MT_GHOSTWARRIOR:
				GET_SINGLE(CSoundManager)->Play("GhostWarrior_Attack2");
				break;
			case MT_DARKKNIGHT:
				GET_SINGLE(CSoundManager)->Play("DarkKnight_Attack2");
				break;
			}
		}
	}
	break;

	case NPC_DAMAGED_BACK:
	{
		m_eMonsterBehaviourState = CS_ON_DAMAGED;
		m_fActiveMaxTime = 0.5f; // 경직시간
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;

		if (fProgress >= 1.f)
		{
			m_fActiveTime = 0.f;
			m_eState = NPC_IDLE1;
			m_bIsHited = false;
			m_bSoundPlay = false;
		}

		else if (!m_bSoundPlay)
		{
			m_bSoundPlay = true;
			switch (m_eMonsterType)
			{
			case MT_RABBIT:
				GET_SINGLE(CSoundManager)->Play("Rabbit_Damage");
				break;
			case MT_HAMSTER:
				GET_SINGLE(CSoundManager)->Play("Hamster_Damage");
				break;
			case MT_YETI:
				GET_SINGLE(CSoundManager)->Play("Yeti_Damage");
				break;
			case MT_ZOMBIE:
				GET_SINGLE(CSoundManager)->Play("Zombie_Damage");
				break;
			case MT_SUCCUBUS:
				GET_SINGLE(CSoundManager)->Play("Succubus_Damage");
				break;
			case MT_DRYAD:
				GET_SINGLE(CSoundManager)->Play("Dryad_Damage");
				break;
			case MT_HARPYQ:
				GET_SINGLE(CSoundManager)->Play("HarpyQ_Damage");
				break;
			case MT_GHOSTWARRIOR:
				GET_SINGLE(CSoundManager)->Play("GhostWarrior_Damage");
				break;
			case MT_DARKKNIGHT:
				GET_SINGLE(CSoundManager)->Play("DarkKnight_Damage");
				break;
			}
		}
	}
	break;

	case NPC_DAMAGED_FRONT:
	{
		m_eMonsterBehaviourState = CS_ON_DAMAGED;
		m_fActiveMaxTime = 0.5f; // 경직시간
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;

		if (fProgress >= 1.f)
		{
			m_fActiveTime = 0.f;
			m_eState = NPC_IDLE1;
			m_bIsHited = false;
			m_bSoundPlay = false;
		}

		else if (!m_bSoundPlay)
		{
			m_bSoundPlay = true;
			switch (m_eMonsterType)
			{
			case MT_RABBIT:
				GET_SINGLE(CSoundManager)->Play("Rabbit_Damage");
				break;
			case MT_HAMSTER:
				GET_SINGLE(CSoundManager)->Play("Hamster_Damage");
				break;
			case MT_YETI:
				GET_SINGLE(CSoundManager)->Play("Yeti_Damage");
				break;
			case MT_ZOMBIE:
				GET_SINGLE(CSoundManager)->Play("Zombie_Damage");
				break;
			case MT_SUCCUBUS:
				GET_SINGLE(CSoundManager)->Play("Succubus_Damage");
				break;
			case MT_DRYAD:
				GET_SINGLE(CSoundManager)->Play("Dryad_Damage");
				break;
			case MT_HARPYQ:
				GET_SINGLE(CSoundManager)->Play("HarpyQ_Damage");
				break;
			case MT_GHOSTWARRIOR:
				GET_SINGLE(CSoundManager)->Play("GhostWarrior_Damage");
				break;
			case MT_DARKKNIGHT:
				GET_SINGLE(CSoundManager)->Play("DarkKnight_Damage");
				break;
			}
		}
	}
	break;

	case NPC_DOWN:
	{
		m_eMonsterBehaviourState = CS_ON_DOWN;
		m_fActiveMaxTime = 1.f; // 경직시간
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;

		if (fProgress >= 1.f)
		{
			m_fActiveTime = 0.f;
			m_eState = NPC_DOWNLOOP;
			m_bSoundPlay = false;
		}

		else if (!m_bSoundPlay)
		{
			m_bSoundPlay = true;
			switch (m_eMonsterType)
			{
			case MT_RABBIT:
				GET_SINGLE(CSoundManager)->Play("Rabbit_Damage");
				break;
			case MT_HAMSTER:
				GET_SINGLE(CSoundManager)->Play("Hamster_Damage");
				break;
			case MT_YETI:
				GET_SINGLE(CSoundManager)->Play("Yeti_Damage");
				break;
			case MT_ZOMBIE:
				GET_SINGLE(CSoundManager)->Play("Zombie_Damage");
				break;
			case MT_SUCCUBUS:
				GET_SINGLE(CSoundManager)->Play("Succubus_Damage");
				break;
			case MT_DRYAD:
				GET_SINGLE(CSoundManager)->Play("Dryad_Damage");
				break;
			case MT_HARPYQ:
				GET_SINGLE(CSoundManager)->Play("HarpyQ_Damage");
				break;
			case MT_GHOSTWARRIOR:
				GET_SINGLE(CSoundManager)->Play("GhostWarrior_Damage");
				break;
			case MT_DARKKNIGHT:
				GET_SINGLE(CSoundManager)->Play("DarkKnight_Damage");
				break;
			}
		}
	}
	break;

	case NPC_DOWNLOOP:
	{
		m_eMonsterBehaviourState = CS_ON_DOWN;
		m_fActiveMaxTime = 0.5f;
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;

		if (fProgress >= 1.f)
		{
			m_fActiveTime = 0.f;
			m_eState = NPC_RECOVERY;
		}
	}
	break;

	case NPC_RECOVERY:
	{
		m_eMonsterBehaviourState = CS_ON_DOWN;
		m_fActiveMaxTime = 0.5f;
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;

		if (fProgress >= 1.f)
		{
			m_fActiveTime = 0.f;
			m_eState = NPC_IDLE1;
		}
	}
	break;

	case NPC_DIE:
	{
		m_eMonsterBehaviourState = CS_ON_DEATH;
		m_fActiveMaxTime = 2.f;
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;

		if (fProgress >= 1.f)
		{
			m_bAggressive = false;
			m_fActiveTime = 0.f;
			m_eState = NPC_DOWN_DEAD_BODY;
		}
	}
	break;

	case NPC_DOWN_DEAD_BODY:
	{
		m_eMonsterBehaviourState = CS_ON_DEATH;
		CGameObject* pMeshCore = m_pTransform->GetChild(0)->GetGameObject();

		CEffect* pEffect = pMeshCore->GetComponent<CEffect>();

		if (pEffect)
		{
			pEffect->SetFlags(VF_BURN);
			pEffect->SetLimitTime(3.f);
		}
		else
		{
			pEffect = pMeshCore->AddComponent<CEffect>();
			pEffect->SetFlags(VF_BURN);
			pEffect->SetLimitTime(3.f);
		}

		SAFE_RELEASE(pEffect);
		SAFE_RELEASE(pMeshCore);

		m_fActiveMaxTime = 3.f; // 경직시간
		m_fActiveTime += fTime;
		float fProgress = m_fActiveTime / m_fActiveMaxTime;

		if (fProgress >= 1.f)
		{
			m_fActiveTime = 0.f;
			m_pGameObject->Enable(false);
		}
	}
	break;

	}
}

void CMonsterAI::UpdateHPBar(float fDeltaTime)
{
	if (m_pHealthBar)
	{
		CHealthBar* pHPBar = m_pHealthBar->GetComponent<CHealthBar>();
		if (pHPBar)
		{
			pHPBar->UpdateHealthBarInfo(m_bAggressive, m_fHP, m_fHPMax, fDeltaTime);
		}
		SAFE_RELEASE(pHPBar);
	}
}

void CMonsterAI::SetMonsterType()
{
	if (m_pGameObject->GetTag() == "Rabbit")
		m_eMonsterType = MT_RABBIT;
	else if (m_pGameObject->GetTag() == "Hamster")
		m_eMonsterType = MT_HAMSTER;
	else if (m_pGameObject->GetTag() == "Yeti")
		m_eMonsterType = MT_YETI;
	else if (m_pGameObject->GetTag() == "SuccA" || m_pGameObject->GetTag() == "SucuuC")
		m_eMonsterType = MT_SUCCUBUS;
	else if (m_pGameObject->GetTag() == "ZombieA" || m_pGameObject->GetTag() == "Zombie_C")
		m_eMonsterType = MT_ZOMBIE;
	else if (m_pGameObject->GetTag() == "Dryad")
		m_eMonsterType = MT_DRYAD;
	else if (m_pGameObject->GetTag() == "HarpyQ")
		m_eMonsterType = MT_HARPYQ;
	else if (m_pGameObject->GetTag() == "GhostWarrior")
		m_eMonsterType = MT_GHOSTWARRIOR;
	else if (m_pGameObject->GetTag() == "DarkKnight")
		m_eMonsterType = MT_DARKKNIGHT;
}

void CMonsterAI::SetIsHited(bool bHit)
{
	m_bIsHited = bHit;
}

bool CMonsterAI::GetIsHited() const
{
	return m_bIsHited;
}

void CMonsterAI::SetMonsterType(MONSTER_TYPE eMonsterType)
{
	m_eMonsterType = eMonsterType;
}

MONSTER_TYPE CMonsterAI::GetMonsterType() const
{
	return m_eMonsterType;
}

bool CMonsterAI::Initialize()
{
	m_eState = NPC_IDLE1;

	return true;
}

void CMonsterAI::Start()
{
	CLayer* pPlayerContainedLayer = m_pScene->FindLayer("Default");
	if (pPlayerContainedLayer)
	{
		CGameObject* pPlayer = pPlayerContainedLayer->FindGameObjectWithTag("Player"); // MUST BE UNIQUE
		m_pPlayer = pPlayer;
		SAFE_RELEASE(pPlayer);
	}

	if (m_pPlayer)
	{
		CGameObject* pChild = m_pTransform->GetChild(0)->GetGameObject();
		CMeshRenderer* pRenderer = pChild->GetComponent<CMeshRenderer>();
		CMesh* pMesh = pRenderer->GetMesh();
		float fMeshRadius = pMesh->GetRadius();
		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pChild);

		CGameObject* pHealthBar = CGameObject::CreateObject("HealthBar", m_pLayer);
		CHealthBar* pBar = pHealthBar->AddComponent<CHealthBar>();
		pBar->SetBaseObject(m_pGameObject);
		pBar->SetPlayerObject(m_pPlayer);
		pBar->SetVisibleDistanceThreshold(50.0f);
		pBar->SetOffset(0.0f, fMeshRadius * m_pTransform->GetWorldScale().Max() * 0.1f, 0.0f);
		SAFE_RELEASE(pBar);

		m_pHealthBar = pHealthBar;
		SAFE_RELEASE(pHealthBar);
	}

	CSphereCollider* pColl = m_pGameObject->AddComponent<CSphereCollider>();
	pColl->SetLocalPosition(0.f, 2.f, 0.f);
	pColl->SetRadius(3.f);
	SAFE_RELEASE(pColl);

	SAFE_RELEASE(pPlayerContainedLayer);

	SetMonsterType();


	/*CGameObject* pBip01 = m_pTransform->GetChild(0)->GetChild(0)->GetGameObject();
	CSphereCollider* pColl = pBip01->AddComponent<CSphereCollider>();
	pColl->SetRadius(5.f);
	SAFE_RELEASE(pColl);
	CAttack* pAttack = pBip01->AddComponent<CAttack>();
	SAFE_RELEASE(pAttack);
	SAFE_RELEASE(pBip01);*/
}

int CMonsterAI::Update(float fTime)
{
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

	if (m_eState == NPC_WALK_FRONT
		|| m_eState == NPC_RUN_FRONT)
	{
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
	}

	if (m_bIsAttack)
	{
		if (m_eState != NPC_ATTACK1
			&& m_eState != NPC_ATTACK2)
			m_bIsAttack = false;
	}

	CGameObject* pMeshCore = m_pTransform->GetChild(0)->GetGameObject();
	CAnimator* pAnimator = pMeshCore->GetComponent<CAnimator>();
	SAFE_RELEASE(pMeshCore);
	if (pAnimator)
	{
		CAnimationStateController* pController = pAnimator->GetAnimationController();
		if (pController)
		{
			switch (m_eState)
			{// switch 문의 여는 괄호입니다
			case NPC_IDLE1:
			{
				pController->SetInteger("State", 0);
			}
			break;
			case NPC_IDLE2:
			{
				pController->SetInteger("State", 1);
			}
			break;
			case NPC_WALK_FRONT:
			{
				pController->SetInteger("State", 2);
			}
			break;
			case NPC_RUN_FRONT:
			{
				pController->SetInteger("State", 3);
			}
			break;
			case NPC_ATTACK1:
			{
				pController->SetInteger("State", 4);
			}
			break;
			case NPC_ATTACK2:
			{
				pController->SetInteger("State", 5);
			}
			break;
			case NPC_SKILL1:
			{
				pController->SetInteger("State", 6);
			}
			break;
			case NPC_SKILL2:
			{
				pController->SetInteger("State", 7);
			}
			break;
			case NPC_DAMAGED_BACK:
			{
				pController->SetInteger("State", 8);
			}
			break;
			case NPC_DAMAGED_FRONT:
			{
				pController->SetInteger("State", 9);
			}
			break;
			case NPC_DOWN:
			{
				pController->SetInteger("State", 10);
			}
			break;
			case NPC_RECOVERY:
			{
				pController->SetInteger("State", 11);
			}
			break;
			case NPC_DIE:
			{
				pController->SetInteger("State", 12);
			}
			break;
			case NPC_DOWN_DEAD_BODY:
			{
				pController->SetInteger("State", 13);
			}
			break;
			case NPC_PASSOUT:
			{
				pController->SetInteger("State", 14);
			}
			break;
			case NPC_DOWNLOOP:
			{
				pController->SetInteger("State", 15);
			}
			break;
			case NPC_FREEZE:
			{
				pController->SetInteger("State", 16);
			}
			break;

			}// switch 문의 닫는 괄호입니다
		}
	}
	SAFE_RELEASE(pAnimator);

	TransState(fTime);
	UpdateHPBar(fTime);
	return 0;
}

int CMonsterAI::OnCollisionEnter(CCollider * pThis, CCollider * pTarget, float fTime)
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
			CLayer* pLayer = m_pGameObject->GetLayer();

			// Effect Distortion
			{
				CGameObject* pSpriteObj = CGameObject::CreateObject("EffectDistortion", pLayer);
				//pSpriteObj->SetTransparency(true);
				CTransform*	pSpriteTr = pSpriteObj->GetTransform();

				pSpriteTr->SetPivot(0.5f, 0.5f, 0.5f);
				pSpriteTr->SetWorldScale(0.f, 0.f, 1.f);

				Vector3	vObjTr = m_pTransform->GetWorldPosition();
				pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 2.5f, vObjTr.z);

				CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("EffectDistortion");
				pMeshRenderer->SetMesh(PRIMITIVE_MESH_DOUGHNUT, true);
				pMeshRenderer->SetRenderState(ALPHA_BLEND);
				pMeshRenderer->SetRenderState(CULLNONE);
				pMeshRenderer->SetShader("DistortionAnimShader");
				pMeshRenderer->SetInputLayout(BUMP_ANIM_LAYOUT);

				float fMaxNormalScale;
				CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
				pMaterial->SetDiffuseTexture("distortion2.png", L"Effects/Distortion/distortion2.png");
				pMaterial->SetAlphaColor(Vector4(1.f, 1.f, 1.f, 1.0f));
				pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
				pMaterial->SetEmissiveColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
				fMaxNormalScale = pMaterial->GetNormalScale();
				SAFE_RELEASE(pMaterial);

				SAFE_RELEASE(pMeshRenderer);

				CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_Distortion_Ctrl");

				pEffectController->SetLifeTime(0.3f);
				pEffectController->SetScaleInfo(0.3f, Vector3(0.f, 0.f, 1.f), Vector3(fRadius * 6, fRadius * 6, 1.f), true);
				pEffectController->SetNormalScaleInfo(0.25f, fMaxNormalScale, 0.f);

				SAFE_RELEASE(pEffectController);

				CBillboard*	pBillboard = pSpriteObj->AddComponent<CBillboard>("Effect_Distortion_Billboard");
				SAFE_RELEASE(pBillboard);

				CDistortion* pDistortion = pSpriteObj->AddComponent<CDistortion>("Effect_Distortion");
				SAFE_RELEASE(pDistortion);

				SAFE_RELEASE(pSpriteTr);
				SAFE_RELEASE(pSpriteObj);
			}


			// Damage Effect
			{
				CGameObject* pSpriteObj = CGameObject::CreateObject("Effect_Damage", pLayer);
				pSpriteObj->SetTransparency(true);
				CTransform*	pSpriteTr = pSpriteObj->GetTransform();

				pSpriteTr->SetWorldPosition(vMonster.x, vMonster.y, 1.0f);
				pSpriteTr->SetWorldScale(0.f, 0.f, 1.f);

				Vector3	vObjTr = m_pTransform->GetWorldPosition();
				pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 2.5f, vObjTr.z);

				CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("Effect_Damage");
				pMeshRenderer->SetMesh(ENGINE_MESH_RECTANGLE_TEX, true);
				pMeshRenderer->SetRenderState(ALPHA_BLEND);
				pMeshRenderer->SetRenderState(CULLNONE);

				CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
				pMaterial->SetDiffuseTexture("FX_Hit_010_TEX_HKB", L"Effects/FX_Hit_010_TEX_HKB.tga");
				pMaterial->SetAlphaTexture("FX_Hit_010_TEX_HKB", L"Effects/FX_Hit_010_TEX_HKB.tga");
				pMaterial->SetAlphaColor(Vector4(0.f, 0.f, 0.f, 1.0f));
				pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
				SAFE_RELEASE(pMaterial);

				SAFE_RELEASE(pMeshRenderer);

				CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_Damage_Ctrl");
				Vector3	vMin = { 0.f, 0.f, 0.f };
				Vector3	vMax = { fRadius * 6, fRadius, 1.0f };
				pEffectController->SetLifeTime(0.3f);
				pEffectController->SetScaleInfo(0.3f, vMin, vMax, true);
				SAFE_RELEASE(pEffectController);

				CBillboard*	pBillboard = pSpriteObj->AddComponent<CBillboard>();
				SAFE_RELEASE(pBillboard);

				SAFE_RELEASE(pSpriteTr);
				SAFE_RELEASE(pSpriteObj);
			}

			SAFE_RELEASE(pLayer);
			// LimLight
			{
				if (!pMonsterCore->HasComponent<CEffectController>())
				{
					CEffectController*	pEffectController = pMonsterCore->AddComponent<CEffectController>("LimLight_Ctrl");
					pEffectController->SetLimLightInfo(0.1f, Vector3(0.7f, 0.7f, 0.7f), 0.25f, false);
					SAFE_RELEASE(pEffectController);
				}
				else
				{
					CEffectController*	pEffectController = pMonsterCore->GetComponent<CEffectController>();
					pEffectController->SetLimLightInfo(0.1f, Vector3(0.7f, 0.7f, 0.7f), 0.25f, false);
					SAFE_RELEASE(pEffectController);
				}
			}

			if (m_eState != NPC_DOWN
				&& m_eState != NPC_DOWNLOOP
				&& m_eState != NPC_RECOVERY)
			{
				CAnimator* pAnimator = pMonsterCore->GetComponent<CAnimator>();
				if (pAnimator)
				{
					CAnimationStateController* pController = pAnimator->GetAnimationController();
					if (pController)
					{
						pController->SetInteger("State", 0);
					}
				}
				SAFE_RELEASE(pAnimator);

				switch (pAttack->GetAttribute())
				{
				case ATTACK_NORMAL:
				{
					m_eState = NPC_DAMAGED_BACK;
				}
				break;
				case ATTACK_DOWN:
				{
					m_eState = NPC_DOWN;
				}
				break;
				case ATTACK_STUN:
				{
					m_eState = NPC_PASSOUT;
				}
				break;
				case ATTACK_FREEZE:
				{
					//m_eState = NPC_DAMAGED_BACK;
				}
				break;
				}
			}

			m_fActiveTime = 0.f;
			m_fHP -= pAttack->GetDamagePoint();
			if (m_fHP <= 0.f)
			{
				switch (m_eMonsterType)
				{
				case MT_RABBIT:
					GET_SINGLE(CSoundManager)->Play("Rabbit_Death");
					break;
				case MT_HAMSTER:
					GET_SINGLE(CSoundManager)->Play("Hamster_Death");
					break;
				case MT_YETI:
					GET_SINGLE(CSoundManager)->Play("Yeti_Death");
					break;
				case MT_ZOMBIE:
					GET_SINGLE(CSoundManager)->Play("Zombie_Death");
					break;
				case MT_SUCCUBUS:
					GET_SINGLE(CSoundManager)->Play("Succubus_Death");
					break;
				case MT_DRYAD:
					GET_SINGLE(CSoundManager)->Play("Dryad_Death");
					break;
				case MT_HARPYQ:
					GET_SINGLE(CSoundManager)->Play("HarpyQ_Death");
					break;
				case MT_GHOSTWARRIOR:
					GET_SINGLE(CSoundManager)->Play("GhostWarrior_Death");
					break;
				case MT_DARKKNIGHT:
					GET_SINGLE(CSoundManager)->Play("DarkKnight_Death");
					break;
				}


				m_eState = NPC_DIE;
				m_fActiveTime = 0.f;
			}

			SAFE_RELEASE(pMonsterCore);

		}
		SAFE_RELEASE(pAttack);
	}

	return 0;
}

int CMonsterAI::OnCollisionStay(CCollider * pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

int CMonsterAI::OnCollisionExit(CCollider * pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

bool CMonsterAI::Save(FILE * pFile)
{
	return true;
}

bool CMonsterAI::Load(FILE * pFile)
{
	return true;
}
