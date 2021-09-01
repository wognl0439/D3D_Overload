#include "stdafx.h"
#include "EditCameraMove.h"
#include "Device.h"
#include "Transform.h"
#include "GameObject.h"
#include "Input.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Layer.h"
#include "Flags.h"
#include "Component/UICollider.h"


CEditCameraMove::CEditCameraMove():
	m_bPressed(false),
	m_bImageEdit(false),
	m_fScale(1.0f)
{
	SetTag("CameraMove");
}


CEditCameraMove::~CEditCameraMove()
{
}

void CEditCameraMove::AddTarget(CGameObject * pTarget)
{
	m_vecTarget.push_back(pTarget);
	CTransform* pTransform = pTarget->GetTransform();
	m_vTargetOriginScale.push_back(pTransform->GetWorldScale());
	SAFE_RELEASE(pTransform);
}

void CEditCameraMove::EraseTarget(size_t index)
{
	m_vecTarget.erase(m_vEditRect.begin() + index);
	m_vTargetOriginScale.erase(m_vTargetOriginScale.begin() + index);
}

bool CEditCameraMove::Initialize()
{
	return true;
}

int CEditCameraMove::Update(float fTime)
{
	if (m_bPressed)
	{
		Vector2 vCursorDistance = m_vCursorPressedPosition - GET_SINGLE(CInput)->GetCursorPosition(SPACE_DEVICE);
		Vector3 vFixedPosition = Vector3(m_vPressedPosition.x + vCursorDistance.x, m_vPressedPosition.y + vCursorDistance.y, m_pTransform->GetWorldPosition().z);
		
		m_pTransform->SetWorldPosition(vFixedPosition);

		size_t iSize = m_vEditRect.size();

		for (size_t i = 0; i < iSize; ++i)
		{
			CTransform* pTransform = m_vEditRect[i]->GetTransform();
			pTransform->SetWorldPosition(m_vPressedRectPosition[i] - vFixedPosition);
			SAFE_RELEASE(pTransform);
		}
	}
	
	if (m_bImageEdit)
	{

		Vector2 vCursorDistance = m_vCursorPressedPosition - GET_SINGLE(CInput)->GetCursorPosition(SPACE_DEVICE);
		Vector3 vFixedPosition = Vector3(m_vPressedPosition.x - vCursorDistance.x, m_vPressedPosition.y - vCursorDistance.y, m_pTransform->GetWorldPosition().z);


		CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
		CLayer*	pLayer = pScene->FindLayer("UI");
		CGameObject*	pEditRectObj = m_vEditRect[m_vEditRect.size() - 1];
		CTransform*		pEditTr = pEditRectObj->GetTransform();
		CUICollider*	pEditCollider = pEditRectObj->GetComponent<CUICollider>();

		pEditCollider->SetScale(vFixedPosition.x, vFixedPosition.y);

		SAFE_RELEASE(pEditCollider);
		SAFE_RELEASE(pEditTr);
/*
		if (GETKEYDOWN(VK_ESCAPE) || GETKEYDOWN(VK_RBUTTON))
		{
			m_vEditRect.erase(m_vEditRect.begin() + m_vEditRect.size() - 1);
			pEditRectObj->Destroy(true);
			m_vCursorPressedPosition = Vector3();
			m_vPressedPosition = Vector3();
			m_bImageEdit = false;
		}
*/
		SAFE_RELEASE(pScene);
		SAFE_RELEASE(pLayer);

	}

	if (GETKEYDOWN(VK_RBUTTON))
	{
		m_vCursorPressedPosition = GET_SINGLE(CInput)->GetCursorPosition(SPACE_DEVICE);
		m_vPressedPosition = m_pTransform->GetWorldPosition();
		m_bPressed = true;


		size_t iSize = m_vEditRect.size();

		for (size_t i = 0; i < iSize; ++i)
		{
			CTransform* pTransform = m_vEditRect[i]->GetTransform();
			Vector3 vPos = pTransform->GetWorldPosition();
			SAFE_RELEASE(pTransform);

			m_vPressedRectPosition.push_back(vPos);
		}

		
	}

	if (GETKEYUP(VK_RBUTTON))
	{
		m_vCursorPressedPosition = Vector3();
		m_vPressedPosition = Vector3();
		m_bPressed = false;
		m_vPressedRectPosition.clear();
	}


	if (GETKEYDOWN(VK_LBUTTON))
	{
		m_vCursorPressedPosition = GET_SINGLE(CInput)->GetCursorPosition(SPACE_DEVICE);
		m_vPressedPosition = m_pTransform->GetWorldPosition();
		m_bImageEdit = true;

		CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
		CLayer*	pLayer = pScene->FindLayer("UI");

		CGameObject*	pEditRectObj = CGameObject::CreateObject("EditRectObj", pLayer);
		CTransform*		pRectTr = pEditRectObj->GetTransform();

		pRectTr->SetWorldScale(1.f, 1.f, 1.f);
		pRectTr->SetWorldPosition(m_vCursorPressedPosition.x, m_vCursorPressedPosition.y, 1.0f);

		CUICollider*	pEditCollider = pEditRectObj->AddComponent<CUICollider>("EditRectCollider");

		SAFE_RELEASE(pEditCollider);
		SAFE_RELEASE(pRectTr);

		m_vEditRect.push_back(pEditRectObj);

		SAFE_RELEASE(pEditRectObj);
		SAFE_RELEASE(pLayer);
		SAFE_RELEASE(pScene);
	}

	if (GETKEYUP(VK_LBUTTON))
	{
		m_vCursorPressedPosition = Vector3();
		m_vPressedPosition = Vector3();
		m_bImageEdit = false;

		CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
		CLayer*	pLayer = pScene->FindLayer("UI");
		CGameObject*	pEditRectObj = m_vEditRect[m_vEditRect.size() - 1];
		CTransform*		pEditTr = pEditRectObj->GetTransform();
		CUICollider*	pEditCollider = pEditRectObj->GetComponent<CUICollider>();

		m_vEditRect.push_back(pEditRectObj);

		SAFE_RELEASE(pEditCollider);
		SAFE_RELEASE(pEditTr);
		SAFE_RELEASE(pScene);
		SAFE_RELEASE(pLayer);
	}


	if (GETKEYDOWN(MOUSE_WHEEL_UP))
	{
		m_fScale += 0.1f;

		size_t iSize = m_vecTarget.size();

		for (size_t i = 0; i < iSize; ++i)
		{
			CTransform* pTransform = m_vecTarget[i]->GetTransform();
			Vector3 vScale = pTransform->GetWorldScale();
			pTransform->SetWorldScale(m_vTargetOriginScale[i] * m_fScale);
			SAFE_RELEASE(pTransform);
		}

		iSize = m_vEditRect.size();

		for (size_t i = 0; i < iSize; ++i)
		{
			CTransform* pTransform = m_vEditRect[i]->GetTransform();
			Vector3 vScale = pTransform->GetWorldScale();
			pTransform->SetWorldScale(Vector3(m_fScale, m_fScale, 1.0f));
			SAFE_RELEASE(pTransform);
		}
	}

	else if (GETKEYDOWN(MOUSE_WHEEL_DOWN))
	{
		m_fScale -= 0.1f;

		size_t iSize = m_vecTarget.size();
		for (size_t i = 0; i < iSize; ++i)
		{
			CTransform* pTransform = m_vecTarget[i]->GetTransform();
			Vector3 vScale = pTransform->GetWorldScale();
			pTransform->SetWorldScale(m_vTargetOriginScale[i] * m_fScale);
			SAFE_RELEASE(pTransform);
		}
		
		iSize = m_vEditRect.size();
		for (size_t i = 0; i < iSize; ++i)
		{
			CTransform* pTransform = m_vEditRect[i]->GetTransform();
			Vector3 vScale = pTransform->GetWorldScale();
			pTransform->SetWorldScale(Vector3(m_fScale, m_fScale, 1.0f));
			SAFE_RELEASE(pTransform);
		}
	}

	return 0;
}
