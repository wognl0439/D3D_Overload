#include "CameraMove.h"
#include "Device.h"
#include "Transform.h"
#include "GameObject.h"
#include "Input.h"
#include "Scene.h"
#include "Layer.h"
#include "SceneManager.h"

CCameraMove::CCameraMove():
	m_pPlayer(NULL),
	m_fSpeed(5.0f)
{
	SetTag("CameraMove");
}
CCameraMove::~CCameraMove()
{
}
CCameraMove::CCameraMove(const CCameraMove & component)
{
	*this = component;
}

void CCameraMove::SetPlayer(CGameObject * pPlayer)
{
	m_pPlayer = pPlayer;
}

CGameObject* CCameraMove::GetPlayer()
{
	if(m_pPlayer)
		m_pPlayer->AddRef();
	return m_pPlayer;
}

void CCameraMove::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;
}

float CCameraMove::GetSpeed()
{
	return m_fSpeed;
}


bool CCameraMove::Initialize()
{
	m_fDistance = 50.0f;
	return true;
}

CCameraMove* CCameraMove::Clone()
{
	return new CCameraMove(*this);
}

int CCameraMove::Update(float fTime)
{
	SCENE_STATE eState = GET_SINGLE(CSceneManager)->GetSceneState();
	if (eState == SS_PLAY)
		return 0;


	if (m_pPlayer)
	{
		if (GETKEYDOWN(MOUSE_WHEEL_UP))
		{
			m_fDistance -= 1.0f;
		}

		if (GETKEYDOWN(MOUSE_WHEEL_DOWN))
		{
			m_fDistance += 1.0f;
		}

		if (!GETKEY(KEY_LEFT_SHIFT))
		{
			CTransform* pPlayerTransform = m_pPlayer->GetTransform();
			Vector3 vPlayerPosition = pPlayerTransform->GetWorldPosition();
			Vector3 vPlayerRotation = pPlayerTransform->GetWorldRotation();
			Vector3 vPlayerBackward = pPlayerTransform->back();
			SAFE_RELEASE(pPlayerTransform);

			Vector3 vTargetPosition = vPlayerPosition + vPlayerBackward * m_fDistance;
			Vector3 vCurrentPosition = XMVectorLerp(m_pTransform->GetWorldPosition(), vTargetPosition, fTime * 5.0f);
			vCurrentPosition.y = m_pTransform->GetWorldPosition().y;

			m_pTransform->SetWorldPosition(vCurrentPosition);
			m_pTransform->SetWorldRotation(vPlayerRotation);
		}
	}
	else
	{
		if (GETKEY(KEY_W))
		{
			if (GETKEY(KEY_LEFT_SHIFT))
			{
				CTransform* pCameraTransform = m_pScene->GetMainCameraTransform();
				pCameraTransform->SetWorldPosition(pCameraTransform->GetWorldPosition() + pCameraTransform->forward() * (m_fSpeed / 3.0f) * fTime);
				SAFE_RELEASE(pCameraTransform);
			}
			else
			{
				CTransform* pCameraTransform = m_pScene->GetMainCameraTransform();
				pCameraTransform->SetWorldPosition(pCameraTransform->GetWorldPosition() + pCameraTransform->forward() * m_fSpeed * fTime);
				SAFE_RELEASE(pCameraTransform);
			}
		}
		
		if (GETKEY(KEY_S))
		{
			if (GETKEY(KEY_LEFT_SHIFT))
			{
				CTransform* pCameraTransform = m_pScene->GetMainCameraTransform();
				pCameraTransform->SetWorldPosition(pCameraTransform->GetWorldPosition() + pCameraTransform->back() * (m_fSpeed / 3.0f) * fTime);
				SAFE_RELEASE(pCameraTransform);
			}
			else
			{
				CTransform* pCameraTransform = m_pScene->GetMainCameraTransform();
				pCameraTransform->SetWorldPosition(pCameraTransform->GetWorldPosition() + pCameraTransform->back() * m_fSpeed * fTime);
				SAFE_RELEASE(pCameraTransform);
			}
		}
		
		if (GETKEY(KEY_A))
		{
			if (GETKEY(KEY_LEFT_SHIFT))
			{
				CTransform* pCameraTransform = m_pScene->GetMainCameraTransform();
				pCameraTransform->SetWorldPosition(pCameraTransform->GetWorldPosition() + pCameraTransform->left() * (m_fSpeed / 3.0f) * fTime);
				SAFE_RELEASE(pCameraTransform);
			}
			else
			{
				CTransform* pCameraTransform = m_pScene->GetMainCameraTransform();
				pCameraTransform->SetWorldPosition(pCameraTransform->GetWorldPosition() + pCameraTransform->left() * m_fSpeed * fTime);
				SAFE_RELEASE(pCameraTransform);
			}
		}
		
		if (GETKEY(KEY_D))
		{
			if (GETKEY(KEY_LEFT_SHIFT))
			{
				CTransform* pCameraTransform = m_pScene->GetMainCameraTransform();
				pCameraTransform->SetWorldPosition(pCameraTransform->GetWorldPosition() + pCameraTransform->right() * (m_fSpeed / 3.0f) * fTime);
				SAFE_RELEASE(pCameraTransform);
			}
			else
			{
				CTransform* pCameraTransform = m_pScene->GetMainCameraTransform();
				pCameraTransform->SetWorldPosition(pCameraTransform->GetWorldPosition() + pCameraTransform->right() * m_fSpeed * fTime);
				SAFE_RELEASE(pCameraTransform);
			}
		}

		if (GETKEY(MOUSE_RIGHT))
		{
			if (GETKEY(KEY_LEFT_SHIFT))
			{
				Vector2 vDeltaPosition = GET_SINGLE(CInput)->GetCursorDeltaPosition(SPACE_DEVICE);
				CTransform* pTransform = m_pScene->GetMainCameraTransform();
				pTransform->SetWorldRotation(pTransform->GetWorldRotation() + Vector3(vDeltaPosition.y * (m_fSpeed  / 3.0f)* D2R * fTime, vDeltaPosition.x * (m_fSpeed / 3.0f) * D2R * fTime, 0.0f));
				SAFE_RELEASE(pTransform);
			}
			else
			{
				Vector2 vDeltaPosition = GET_SINGLE(CInput)->GetCursorDeltaPosition(SPACE_DEVICE);
				CTransform* pTransform = m_pScene->GetMainCameraTransform();
				pTransform->SetWorldRotation(pTransform->GetWorldRotation() + Vector3(vDeltaPosition.y * m_fSpeed * D2R * fTime, vDeltaPosition.x * m_fSpeed * D2R * fTime, 0.0f));
				SAFE_RELEASE(pTransform);
			}
		}		
	}

	

	return 0;
}
