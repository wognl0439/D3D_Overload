#include "PlayerCamera.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Input.h"
#include "Mathf.h"
#include "SceneManager.h"
#include "PlayerController.h"
#include "CollisionManager.h"
#include "Collider.h"
#include "Component/TerrainCollider.h"
#include "Component/Effect.h"
#include "Component/MeshCollider.h"


CPlayerCamera::CPlayerCamera():
	m_vOffset(0.0f, 4.0f, 0.0f),
	m_pPlayer(NULL),
	m_fScrollSpeed(2.0f),
	m_vDistanceRange(5.0f, 20.0f),
	m_vVerticalAngleRange(-80.0f, 30.0f), // Degree
	m_fAngularSpeed(10.0f),
	m_fScrollDistance(2.0f),
	m_fHeightOffset(0.0f),
	m_fHorizontalAngleTarget(0.0f),
	m_fVerticalAngleTarget(0.0f)
{
}


CPlayerCamera::~CPlayerCamera()
{
}

Vector3 CPlayerCamera::UpdateShake(float fTime)
{
	if (m_bShake)
	{
		m_fShakeTimeLapse += fTime;
		float fProgress = CMathf::Clamp(m_fShakeTimeLapse / m_fShakeTime, 0.0f, 1.0f);	
		float fMagnitude = (sinf(m_fShakeTimeLapse * m_fShakeFrequency) * m_fShakeAmplitude) * (1.0f - fProgress);
		
		if (fProgress == 1.0f)
		{
			StopShake();
		}
		return Vector3::RandomDirection() * fMagnitude;
	}
	else
	{
		return Vector3::Zero;
	}
}

void CPlayerCamera::StartShake(float fAmplitude, float fFrequency, float fTime)
{
	//fTime�� ���� �� ���� ���� ���ҵǰ�, 0�� �� ���� �����.
	m_bShake = true;
	m_fShakeAmplitude = fAmplitude;
	m_fShakeFrequency = fFrequency;
	m_fShakeTime = fTime;	
	m_fShakeTimeLapse = 0.0f;
}

void CPlayerCamera::StopShake()
{
	m_fShakeAmplitude = 0.0f;
	m_fShakeFrequency = 0.0f;
	m_fShakeTimeLapse = 0.0f;
	m_fShakeTime = 0.0f;
	m_bShake = false;
}

void CPlayerCamera::SetDistanceRange(float fMin, float fMax)
{
	m_vDistanceRange.x = fMin;
	m_vDistanceRange.y = fMax;
}

void CPlayerCamera::SetDistanceRange(const Vector2 & vMinMax)
{
	m_vDistanceRange = vMinMax;
}

Vector2 CPlayerCamera::GetDistanceRange() const
{
	return m_vDistanceRange;
}

void CPlayerCamera::SetAngleRange(float fMin, float fMax)
{
	m_vVerticalAngleRange.x = fMin;
	m_vVerticalAngleRange.y = fMax;
}

void CPlayerCamera::SetAngleRange(const Vector2 & vMinMax)
{
	m_vVerticalAngleRange = vMinMax;
}

Vector2 CPlayerCamera::GetAngleRange() const
{
	return m_vVerticalAngleRange;
}

void CPlayerCamera::SetPlayer(CGameObject * pTarget)
{
	m_pPlayer = pTarget;
}

float CPlayerCamera::GetScrollTargetDistance() const
{
	return m_fScrollTargetDistance;
}

void CPlayerCamera::SetScrollTargetDistance(float fDistance)
{
	m_fScrollTargetDistance = fDistance;
}

float CPlayerCamera::GetCameraPostionZ() const
{
	CTransform* pCameraTransform = m_pTransform->GetChild(0)->GetChild(0);
	return pCameraTransform->GetLocalPosition().z;
}

void CPlayerCamera::SetCameraPostionZ(float fZ)
{
	CTransform* pCameraTransform = m_pTransform->GetChild(0)->GetChild(0);
	pCameraTransform->SetLocalPosition(0.0f, 0.0f, fZ);
}

bool CPlayerCamera::Initialize()
{
	return true;
}

void CPlayerCamera::Start()
{
	CLayer* pPlayerContainedLayer = m_pScene->FindLayer("Default");
	CGameObject* pPlayer = pPlayerContainedLayer->FindGameObjectWithTag("Player"); // MUST BE UNIQUE
	m_pPlayer = pPlayer;
	
	CTransform* pPlayerTransform = pPlayer->GetTransform();
	Vector3 vPlayerPosition = pPlayerTransform->GetWorldPosition();
	SAFE_RELEASE(pPlayerTransform);
	
	SAFE_RELEASE(pPlayer);
	SAFE_RELEASE(pPlayerContainedLayer);

	m_fScrollTargetDistance = m_vDistanceRange.y;

	//�ڽ��� �ڽ��� ī�޶��̴�. �� ������ �ݵ�� ��ȿ�ؾ� �ϹǷ� ����ó�� ���� �ʴ´�.	
	//�⺻���δ� �ִ밪�� �����Ѵ�.
	//���������� 180�� ������ �θ� ���ϰ� �����.
	m_pTransform->SetWorldPosition(vPlayerPosition + m_vOffset);
	m_pTransform->GetChild(0)->GetChild(0)->SetLocalPosition(0.0f, 0.0f, m_vDistanceRange.y);
	m_pTransform->GetChild(0)->GetChild(0)->SetLocalRotation(0.0f, PI, 0.0f); 
	m_pTransform->GetChild(0)->SetLocalPosition(0.0f, 0.0f, 0.0f);
	m_pTransform->GetChild(0)->SetLocalRotation(m_vVerticalAngleRange.x * D2R, 0.0f, 0.0f);
}

int CPlayerCamera::Update(float fTime)
{
	if (GET_SINGLE(CSceneManager)->GetSceneState() == SCENE_STATE::SS_PLAY)
	{
		if (m_pPlayer)
		{
			CTransform* pHorizontalGimbal = m_pTransform;
			CTransform* pVerticalGimbal = pHorizontalGimbal->GetChild(0);
			CTransform* pDistanceGimbal = pVerticalGimbal->GetChild(0);

			if (GETKEY(MOUSE_RIGHT))
			{
				Vector2 vDeltaPosition = GET_SINGLE(CInput)->GetCursorDeltaPosition(SPACE_DEVICE);	
				m_fHorizontalAngleTarget += vDeltaPosition.x * m_fAngularSpeed * D2R *  fTime;										
				m_fVerticalAngleTarget -= vDeltaPosition.y * m_fAngularSpeed * D2R * fTime;

				//���� ȸ���� �����̹Ƿ� ������ �ֵ��� �Ѵ�.
				m_fVerticalAngleTarget = CMathf::Clamp(m_fVerticalAngleTarget, m_vVerticalAngleRange.x * D2R, m_vVerticalAngleRange.y * D2R);
			}

			//���Ÿ��� ������ �� ���� �۾����Ƿ� ���ش�.
			if (GETKEYDOWN(MOUSE_WHEEL_UP))
			{
				m_fScrollTargetDistance = CMathf::Clamp(m_fScrollTargetDistance - m_fScrollDistance, m_vDistanceRange.x, m_vDistanceRange.y);
			}
			if (GETKEYDOWN(MOUSE_WHEEL_DOWN))
			{
				m_fScrollTargetDistance = CMathf::Clamp(m_fScrollTargetDistance + m_fScrollDistance, m_vDistanceRange.x, m_vDistanceRange.y);
			}
			
			RAY tRay;
			LINECASTHIT tHit;
			tRay.vOrigin = m_pTransform->GetWorldPosition();
			tRay.vDirection = (pDistanceGimbal->GetWorldPosition() - m_pTransform->GetWorldPosition()).Normalize();
			tRay.fDistance = m_vDistanceRange.y;			

			tHit = GET_SINGLE(CCollisionManager)->LineCast(tRay);
			
			for (size_t i = 0; i < tHit.iCount; ++i)
			{
				if (tHit.vecHits[i].bResult)
				{
					CGameObject* pTarget = tHit.vecHits[i].pCollider->GetGameObject();
					if (pTarget->IsStatic())
					{
						//Near ����� �Ÿ���ŭ ���ʱ��� ���� 
						m_fScrollTargetDistance = tHit.vecHits[i].fDistamce - 5.0f;
					}
					SAFE_RELEASE(pTarget);
				}				
			}


			float fHorizontalAngle = pHorizontalGimbal->GetWorldRotation().y;
			fHorizontalAngle = CMathf::Lerp(fHorizontalAngle, m_fHorizontalAngleTarget, 0.5f);
			pHorizontalGimbal->SetWorldRotation(0.0f, fHorizontalAngle, 0.0f);

			float fVerticalAngle = pVerticalGimbal->GetLocalRotation().x;
			fVerticalAngle = CMathf::Lerp(fVerticalAngle, m_fVerticalAngleTarget, 0.5f);
			pVerticalGimbal->SetLocalRotation(fVerticalAngle, 0.0f, 0.0f);

			float fCurrentDistance = CMathf::Clamp(pDistanceGimbal->GetLocalPosition().z, m_vDistanceRange.x, m_vDistanceRange.y);
			float fDistance = CMathf::Lerp(fCurrentDistance, m_fScrollTargetDistance, fTime * m_fScrollSpeed);
			pDistanceGimbal->SetLocalPosition(0.0f, 0.0f, fDistance);
		}
	}
	return 0;
}

int CPlayerCamera::LateUpdate(float fTime)
{
	if (m_pPlayer)
	{
		CTransform* pHorizontalGimbal = m_pTransform;
		CTransform* pVerticalGimbal = pHorizontalGimbal->GetChild(0);
		CTransform* pDistanceGimbal = pVerticalGimbal->GetChild(0);

		float fDistanceProgress = pDistanceGimbal->GetLocalPosition().z / (m_vDistanceRange.y - m_vDistanceRange.x);
		m_fHeightOffset = fDistanceProgress * 3.0f + 3.0f;

		
		CTransform* pPlayerTransform = m_pPlayer->GetTransform() ;
		Vector3 vOffset = Vector3(0.0f, m_fHeightOffset, 0.0f);
		Vector3 vPosition = Vector3::Lerp(m_pTransform->GetWorldPosition(), pPlayerTransform->GetWorldPosition() + vOffset, 0.5f);
		m_pTransform->SetWorldPosition(vPosition);


		//ĳ������ y�� ȸ������ ���� �ϵ��� �Ѵ�.
		//��, �÷��̾ Idle ������ ��쿡�� ī�޶� ȸ���� ���� �޵��� �Ѵ�.
		CPlayerController* pController = m_pPlayer->GetComponent<CPlayerController>();
		if (pController)
		{
			if (!pController->IsIdleState())			
			{
				Vector3 vPlayerRotation = pPlayerTransform->GetWorldRotation();
				float fPlayerRotation = vPlayerRotation.y;
				float fCameraRotation = pHorizontalGimbal->GetWorldRotation().y;
				
				//ª�������� ��ȸ�ϵ� ȸ�� ���� ��ø�Ǿ� ���� ��� �о���� �Ѵ�.
				float fSign = fCameraRotation > 0.0f ? 1.0f : -1.0f;

				fCameraRotation = fabsf(fCameraRotation);
				while (fabsf(fCameraRotation) > 2.0f * PI)
				{
					fCameraRotation -= 2.0f * PI;
				}

				fCameraRotation *= fSign;

				float fTargetRotation = 0.0f;
				if (fabsf(fCameraRotation - fPlayerRotation + PI) < fabsf(fCameraRotation - fPlayerRotation - PI))
					fTargetRotation = fCameraRotation + PI;
				else
					fTargetRotation = fCameraRotation - PI;

				float fRotation = CMathf::Lerp(fPlayerRotation, fTargetRotation, fTime * 2.0f);
				pPlayerTransform->SetWorldRotation(vPlayerRotation.x, fRotation, vPlayerRotation.z);
			}
		}
		SAFE_RELEASE(pController);
		SAFE_RELEASE(pPlayerTransform);

		Vector3 vCameraShakeOffset = UpdateShake(fTime);
		CTransform* pCameraTransform = m_pScene->GetMainCameraTransform();
		Matrix matCameraWorld = pDistanceGimbal->GetWorldMatrix();

		matCameraWorld.fMat[3][0] += vCameraShakeOffset.x;
		matCameraWorld.fMat[3][1] += vCameraShakeOffset.y;
		matCameraWorld.fMat[3][2] += vCameraShakeOffset.z;

		pCameraTransform->SetWorldMatrix(matCameraWorld);
		SAFE_RELEASE(pCameraTransform);
	}
	return 0;
}


bool CPlayerCamera::Save(FILE * pFile)
{
	return true;
}

bool CPlayerCamera::Load(FILE * pFile)
{
	return true;
}
