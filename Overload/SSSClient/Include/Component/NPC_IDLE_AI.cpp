#include "NPC_IDLE_AI.h"
#include "AnimationStateController.h"
#include "Component/Animator.h"
#include "Transform.h"
#include "CollisionManager.h"
#include "Component/TerrainCollider.h"
#include "PlayerController.h"
#include "Scene.h"
#include "Layer.h"
#include "Component/SphereCollider.h"
#include "Component/MeshCollider.h"
#include "Component/BoxCollider.h"
#include "PlayerController.h"
#include "Component/Camera.h"
#include "Input.h"
#include "Component/Effect.h"
#include "Device.h"
#include "Component/SpriteRenderer.h"
#include "Material.h"
#include "Component/Rigidbody.h"

CNPC_IDLE_AI::CNPC_IDLE_AI()
{
	m_eState = NPC_NONE;
	m_vStandardPosition = Vector3::Zero;
	m_fMoveRange = 0.f;
	m_fActiveTime = 0.f;
	m_fActiveMaxTime = 1.f;
	m_fMoveSpeed = 5.f;

	m_pPlayer = NULL;
	m_pEffect = NULL;
	m_bDeadBody = false;
}


CNPC_IDLE_AI::~CNPC_IDLE_AI()
{
}

void CNPC_IDLE_AI::SetMoveDistance(Vector3 vPos, float fRange)
{
	m_vStandardPosition = vPos;
	m_fMoveRange = fRange;
}

void CNPC_IDLE_AI::SetDeadBody(bool bSwitch)
{
	m_bDeadBody = bSwitch;
}

void CNPC_IDLE_AI::RunningState(float fTime)
{
	if (m_pPlayer == NULL)
		return;

	CTransform* pPlayerTransform = m_pPlayer->GetTransform();
	Vector3 vPlayerPosition = pPlayerTransform->GetWorldPosition();
	SAFE_RELEASE(pPlayerTransform);

	Vector3 vMyPosition = m_pTransform->GetWorldPosition();
	Vector3 vDir = vPlayerPosition - vMyPosition;
	float	fDistance = vDir.Length();
	if (fDistance <= 5.f)
	{
		m_eState = NPC_IDLE1;
		vPlayerPosition.y = vMyPosition.y;
		m_pTransform->LookAt(vPlayerPosition);
		m_bAgro = true;
		return;
	}
	else
		m_bAgro = false;

	switch (m_eState)
	{
	case NPC_IDLE1:
	{
		Vector3 vMyPosition = m_pTransform->GetWorldPosition();

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
			iRandom = rand() % 360;
			float fRot = iRandom;
			m_pTransform->SetWorldRotationY(D2R * iRandom);
			m_eState = NPC_WALK_FRONT;
			m_fActiveTime = 0.f;
		}
		else
		{
			m_eState = NPC_IDLE2;
			m_fActiveTime = 0.f;
		}
	}
	break;

	case NPC_IDLE2:
	{
		Vector3 vMyPosition = m_pTransform->GetWorldPosition();

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
			iRandom = rand() % 360;
			float fRot = iRandom;
			m_pTransform->SetWorldRotationY(D2R * iRandom);
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

		Vector3 vMyPosition = m_pTransform->GetWorldPosition();
		Vector3 vRot = m_pTransform->forward();
		Vector3 vMovePosition = vMyPosition + (vRot * m_fMoveSpeed * fTime);

		float Length = (m_vStandardPosition - vMovePosition).Length();

		if(Length < m_fMoveRange)
			m_pTransform->SetWorldPosition(vMovePosition);
	}
	break;
	}
}

bool CNPC_IDLE_AI::GetAgroState() const
{
	return m_bAgro;
}

bool CNPC_IDLE_AI::Initialize()
{
	m_eState = NPC_IDLE1;

	return true;
}

void CNPC_IDLE_AI::Start()
{
	CLayer* pPlayerContainedLayer = m_pScene->FindLayer("Default");
	if (pPlayerContainedLayer)
	{
		CGameObject* pPlayer = pPlayerContainedLayer->FindGameObjectWithTag("Player"); // MUST BE UNIQUE
		m_pPlayer = pPlayer;
		SAFE_RELEASE(pPlayer);
	}
	SAFE_RELEASE(pPlayerContainedLayer);

	CGameObject* pMeshCore = m_pTransform->GetChild(0)->GetGameObject();
	CEffect* pEffect = pMeshCore->AddComponent<CEffect>();
	pEffect->SetFlags(VF_LIMLIGHT);
	m_pEffect = pEffect;
	SAFE_RELEASE(pEffect);
	SAFE_RELEASE(pMeshCore);
}

int CNPC_IDLE_AI::Update(float fTime)
{
	if (m_bDeadBody)
	{
		CGameObject* pMeshCore = m_pTransform->GetChild(0)->GetGameObject();
		CAnimator* pAnimator = pMeshCore->GetComponent<CAnimator>();
		SAFE_RELEASE(pMeshCore);
		if (pAnimator)
		{
			CAnimationStateController* pController = pAnimator->GetAnimationController();
			if (pController)
			{
				pController->SetInteger("State", 100);
			}
		}
		SAFE_RELEASE(pAnimator);

		return 0;
	}

	if (m_pPlayer)
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


	CGameObject* pMeshCore = m_pTransform->GetChild(0)->GetGameObject();
	CAnimator* pAnimator = pMeshCore->GetComponent<CAnimator>();
	SAFE_RELEASE(pMeshCore);
	if (pAnimator)
	{
		CAnimationStateController* pController = pAnimator->GetAnimationController();
		if (pController)
		{
			if (m_eState == NPC_IDLE2)
			{
				pController->SetInteger("State", 1);
			}
			else if (m_eState == NPC_WALK_FRONT)
			{
				pController->SetInteger("State", 2);
			}
			else if (m_eState == NPC_RUN_FRONT)
			{
				pController->SetInteger("State", 3);
			}
			else if (m_eState == NPC_ATTACK1)
			{
				pController->SetInteger("State", 4);
			}
			else if (m_eState == NPC_ATTACK2)
			{
				pController->SetInteger("State", 5);
			}
			else if (m_eState == NPC_SKILL1)
			{
				pController->SetInteger("State", 6);
			}
			else if (m_eState == NPC_SKILL2)
			{
				pController->SetInteger("State", 7);
			}
			else if (m_eState == NPC_DAMAGED_BACK)
			{
				pController->SetInteger("State", 8);
			}
			else if (m_eState == NPC_DAMAGED_FRONT)
			{
				pController->SetInteger("State", 9);
			}
			else if (m_eState == NPC_DOWN)
			{
				pController->SetInteger("State", 10);
			}
			else if (m_eState == NPC_RECOVERY)
			{
				pController->SetInteger("State", 11);
			}
			else if (m_eState == NPC_DIE)
			{
				pController->SetInteger("State", 12);
			}
			else if (m_eState == NPC_DOWN_DEAD_BODY)
			{
				pController->SetInteger("State", 13);
			}
			else
			{
				pController->SetInteger("State", 0);
			}
		}
	}
	SAFE_RELEASE(pAnimator);

	RunningState(fTime);

	return 0;
}


void CNPC_IDLE_AI::OnMouseEnter(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	//m_pEffect->SetLimPower(0.1f);

	//if (GETKEYDOWN(MOUSE_LEFT))
	//{
	//	//CLayer* pUILayer = m_pScene->FindLayer("UI");
	//	//CGameObject* pPanelObject = CGameObject::CreateObject("NPCDialog", pUILayer);
	//	//CSpriteRenderer* pRenderer = pPanelObject->AddComponent<CSpriteRenderer>();
	//	//CMaterial* pMaterial = pRenderer->GetMaterial();
	//	////pMaterial->SetDiffuseTexture("Dialog", L"UI\\Dialog");
	//	//SAFE_RELEASE(pMaterial);
	//	//SAFE_RELEASE(pRenderer);
	//	//SAFE_RELEASE(pPanelObject);
	//	//SAFE_RELEASE(pUILayer);
	//}
}

void CNPC_IDLE_AI::OnMouseStay(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
	
}

void CNPC_IDLE_AI::OnMouseExit(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	//m_pEffect->SetLimPower(3.f);
}


bool CNPC_IDLE_AI::Save(FILE * pFile)
{
	return true;
}

bool CNPC_IDLE_AI::Load(FILE * pFile)
{
	return true;
}
