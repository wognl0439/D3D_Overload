#include "Gizmo.h"

#include "../Mesh.h"
#include "../ResourcesManager.h"
#include "../ShaderManager.h"
#include "../RenderManager.h"
#include "MeshRenderer.h"
#include "../Transform.h"
#include "BoxCollider.h"
#include "../RenderState.h"
#include "../Shader.h"
#include "../Input.h"
#include "../Scene.h"
#include "Camera.h"

SSS_USING

CGizmo::CGizmo()
{
	m_bLocalMode = false;
	m_pTarget = NULL;
	m_fLocalScaleFactor = 0.05f;
	m_bClicked = false;
	m_iControlAxis = 0;
	m_vGizmoColor[3];
	m_fSensitivity = 0.2f;

	m_pRendererX = NULL;
	m_pRendererY = NULL;
	m_pRendererZ = NULL;
	m_pRendererXY = NULL;
	m_pRendererXZ = NULL;
	m_pRendererYZ = NULL;
	m_pColliderX = NULL;
	m_pColliderY = NULL;
	m_pColliderZ = NULL;
	m_pColliderXY = NULL;
	m_pColliderXZ = NULL;
	m_pColliderYZ = NULL;
}


CGizmo::~CGizmo()
{
	SAFE_RELEASE(m_pRendererX);
	SAFE_RELEASE(m_pRendererY);
	SAFE_RELEASE(m_pRendererZ);
	SAFE_RELEASE(m_pRendererXY);
	SAFE_RELEASE(m_pRendererXZ);
	SAFE_RELEASE(m_pRendererYZ);
	SAFE_RELEASE(m_pColliderX);
	SAFE_RELEASE(m_pColliderY);
	SAFE_RELEASE(m_pColliderZ);
	SAFE_RELEASE(m_pColliderXY);
	SAFE_RELEASE(m_pColliderXZ);
	SAFE_RELEASE(m_pColliderYZ);
}

void CGizmo::SetLocalMode(bool bForLocal)
{
	m_bLocalMode = bForLocal;
}

void CGizmo::UpdateGizmoTransform()
{
	if (m_pTarget)
	{
		CTransform* pTargetTransform = m_pTarget->GetTransform();
		m_pTransform->SetWorldPosition(pTargetTransform->GetWorldPosition());
		//m_pTransform->SetWorldRotation(pTargetTransform->GetWorldRotation());
		//pTargetTransform->SetWorldScale(m_pTransform->GetWorldScale());
		SAFE_RELEASE(pTargetTransform);
	}
}

void CGizmo::UpdateGizmoLocalTransform()
{
	if (m_pTarget)
	{
		CTransform* pTargetTransform = m_pTarget->GetTransform();
		m_pTransform->SetLocalPosition(pTargetTransform->GetLocalPosition());
		m_pTransform->SetLocalRotation(pTargetTransform->GetLocalRotation());
		//pTargetTransform->SetWorldScale(m_pTransform->GetWorldScale());
		SAFE_RELEASE(pTargetTransform);
	}
}

void CGizmo::UpdateGizmo()
{
	switch (m_eState)
	{
	case SSS::GS_DISABLE:
		break;
	case SSS::GS_TRANSFORMATION:
	{
		m_pRendererX->SetMesh(GIZMO_MESH_ARROW_X, true);
		m_pRendererY->SetMesh(GIZMO_MESH_ARROW_Y, true);
		m_pRendererZ->SetMesh(GIZMO_MESH_ARROW_Z, true);
		//m_pMeshXY = GET_SINGLE(CResourcesManager)->FindMesh(GIZMO_MESH_PLANE);
		//m_pMeshXZ = GET_SINGLE(CResourcesManager)->FindMesh(GIZMO_MESH_PLANE);
		//m_pMeshYZ = GET_SINGLE(CResourcesManager)->FindMesh(GIZMO_MESH_PLANE);
	}
		break;
	case SSS::GS_ROTATION:
		break;
	case SSS::GS_SCALE:
		break;
	case SSS::GS_END:
		break;
	default:
		break;
	}
}

CGameObject * CGizmo::GetGizmoTarget() const
{
	if(m_pTarget)
		m_pTarget->AddRef();

	return m_pTarget;
}

void CGizmo::SetGizmoState(GIZMO_STATE eState)
{
	if (m_eState != eState)
	{
		m_eState = eState;
		UpdateGizmo();
	}
}

void CGizmo::SetGizmoTarget(CGameObject * pTarget)
{
	m_pTarget = pTarget;
	if (pTarget)
	{
		CTransform* pTargetTransform = pTarget->GetTransform();

		m_pTransform->SetWorldPosition(pTargetTransform->GetWorldPosition());
		m_pTransform->SetWorldRotation(pTargetTransform->GetWorldRotation());
		//Scale 은 Internal Scale 가지고 있어야 하기 때문에 적용 하지 않음. Gizmo 스케일은 오브젝트의 Scale의 적용을 받으면 안됨.
		SAFE_RELEASE(pTargetTransform);
	}
}

bool CGizmo::Initialize()
{
	m_pRendererX = m_pGameObject->AddComponent<CMeshRenderer>();
	m_pRendererX->SetMesh(GIZMO_MESH_ARROW_X, true);
	m_pRendererX->SetShader(STANDARD_COLOR_NORMAL_SHADER);
	m_pRendererX->SetRenderState(CULLNONE);
	m_pRendererX->SetRenderState(ALPHA_BLEND);
	//m_pRendererX->SetRenderState(DEPTH_DISABLE);

	m_pRendererY = m_pGameObject->AddComponent<CMeshRenderer>();
	m_pRendererY->SetMesh(GIZMO_MESH_ARROW_Y, true);
	m_pRendererY->SetShader(STANDARD_COLOR_NORMAL_SHADER);
	m_pRendererY->SetRenderState(CULLNONE);
	m_pRendererY->SetRenderState(ALPHA_BLEND);
	//m_pRendererY->SetRenderState(DEPTH_DISABLE);

	m_pRendererZ = m_pGameObject->AddComponent<CMeshRenderer>();
	m_pRendererZ->SetMesh(GIZMO_MESH_ARROW_Z, true);
	m_pRendererZ->SetShader(STANDARD_COLOR_NORMAL_SHADER);
	m_pRendererZ->SetRenderState(CULLNONE);
	m_pRendererZ->SetRenderState(ALPHA_BLEND);
	//m_pRendererZ->SetRenderState(DEPTH_DISABLE);

	//m_pRendererXY = m_pGameObject->AddComponent<CMeshRenderer>();
	//m_pRendererXY->SetMesh(GIZMO_MESH_ARROW);
	//m_pRendererXY->SetRenderState(CULLNONE);
	//m_pRendererXY->SetRenderState(ALPHA_BLEND);
	//
	//m_pRendererXZ = m_pGameObject->AddComponent<CMeshRenderer>();
	//m_pRendererXZ->SetMesh(GIZMO_MESH_ARROW);
	//m_pRendererXZ->SetRenderState(CULLNONE);
	//m_pRendererXZ->SetRenderState(ALPHA_BLEND);
	//
	//m_pRendererYZ = m_pGameObject->AddComponent<CMeshRenderer>();
	//m_pRendererYZ->SetMesh(GIZMO_MESH_ARROW
	//m_pRendererYZ->SetRenderState(CULLNONE);
	//m_pRendererYZ->SetRenderState(ALPHA_BLEND);


	m_pColliderX = m_pGameObject->AddComponent<CBoxCollider>();
	m_pColliderX->SetVolume(Vector3(5.0f, 1.0f, 1.0f));
	m_pColliderX->SetLocalPosition(Vector3(0.0f, 0.0f, 0.0f));

	m_pColliderY = m_pGameObject->AddComponent<CBoxCollider>();
	m_pColliderY->SetVolume(Vector3(1.0f, 5.0f, 1.0f));
	m_pColliderY->SetLocalPosition(Vector3(0.0f, 0.0f, 0.0f));

	m_pColliderZ = m_pGameObject->AddComponent<CBoxCollider>();
	m_pColliderZ->SetVolume(Vector3(1.0f, 1.0f, 5.0f));
	m_pColliderZ->SetLocalPosition(Vector3(0.0f, 0.0f, 0.0f));

	//m_pColliderXY = m_pGameObject->AddComponent<CBoxCollider>();
	//m_pColliderXY->SetVolume(Vector3(1.0f, 1.0f, 1.0f));
	//
	//m_pColliderXZ = m_pGameObject->AddComponent<CBoxCollider>();
	//m_pColliderXZ->SetVolume(Vector3(1.0f, 1.0f, 1.0f));
	//
	//m_pColliderYZ = m_pGameObject->AddComponent<CBoxCollider>();
	//m_pColliderYZ->SetVolume(Vector3(1.0f, 1.0f, 1.0f));

	return true;
}

int CGizmo::Update(float fTime)
{
	Vector2 vCursorDeltaPosition = GET_SINGLE(CInput)->GetCursorDeltaPosition(SPACE_DEVICE);

	vCursorDeltaPosition *= 0.5f;
	if (m_pTarget)
	{
		if (m_bClicked)
		{
			if (m_bLocalMode)
			{
				CTransform* pTargetTransform = m_pTarget->GetTransform();
				Vector3 vTargetLocal = pTargetTransform->GetLocalPosition();
				Vector3 vTargetWorld = pTargetTransform->GetWorldPosition();
				Vector3 vPosition = m_pTransform->GetWorldPosition();
				Vector3 vDistance;

				switch (m_eState)
				{
				case SSS::GS_DISABLE:
					break;
				case SSS::GS_TRANSFORMATION:
				{
					//X
					if (m_iControlAxis &= 1)
					{
						vDistance = m_pTransform->right() * Vector3(vCursorDeltaPosition.x, 0.0f, 0.0f) * m_fSensitivity;
						m_pTransform->SetWorldPosition(vPosition + vDistance);
					}
					//Y
					if (m_iControlAxis &= 2)
					{
						vDistance = m_pTransform->up() * Vector3(0.0f, -vCursorDeltaPosition.y, 0.0f) * m_fSensitivity;
						m_pTransform->SetWorldPosition(vPosition + vDistance);
					}
					//Z
					if (m_iControlAxis &= 4)
					{
						vDistance = m_pTransform->forward() * Vector3(0.0f, 0.0f, vCursorDeltaPosition.x) * m_fSensitivity;
						m_pTransform->SetWorldPosition(vPosition + vDistance);
					}
				}
				break;
				case SSS::GS_ROTATION:
					break;
				case SSS::GS_SCALE:
					break;
				case SSS::GS_END:
					break;
				default:
					break;
				}
				pTargetTransform->SetLocalPosition(vTargetLocal + vDistance);
				m_pTransform->SetWorldRotation(pTargetTransform->GetWorldRotation());				
				SAFE_RELEASE(pTargetTransform);
			}
			else
			{
				switch (m_eState)
				{
				case SSS::GS_DISABLE:
					break;
				case SSS::GS_TRANSFORMATION:
				{
					if (m_iControlAxis == 1)
					{
						Vector3 vPosition = m_pTransform->GetWorldPosition();
						m_pTransform->SetWorldPosition(vPosition + Vector3(vCursorDeltaPosition.x, 0.0f, 0.0f) * m_fSensitivity);
					}
					else if (m_iControlAxis == 2)
					{
						Vector3 vPosition = m_pTransform->GetWorldPosition();
						m_pTransform->SetWorldPosition(vPosition + Vector3(0.0f, -vCursorDeltaPosition.y, 0.0f) * m_fSensitivity);
					}
					else if (m_iControlAxis == 4)
					{
						Vector3 vPosition = m_pTransform->GetWorldPosition();
						m_pTransform->SetWorldPosition(vPosition + Vector3(0.0f, 0.0f, vCursorDeltaPosition.x) * m_fSensitivity);
					}
					//X | Y
					else if (m_iControlAxis == 3)
					{
						Vector3 vPosition = m_pTransform->GetWorldPosition();
						m_pTransform->SetWorldPosition(vPosition + Vector3(vCursorDeltaPosition.x, -vCursorDeltaPosition.y, 0.0f) * m_fSensitivity);
					}
					//X | Z
					else if (m_iControlAxis == 5)
					{
						Vector3 vPosition = m_pTransform->GetWorldPosition();
						m_pTransform->SetWorldPosition(vPosition + Vector3(vCursorDeltaPosition.x, 0.0f, -vCursorDeltaPosition.y) * m_fSensitivity);
					}
					//Y | Z
					else if (m_iControlAxis == 6)
					{
						Vector3 vPosition = m_pTransform->GetWorldPosition();
						m_pTransform->SetWorldPosition(vPosition + Vector3(0.0f, -vCursorDeltaPosition.y, vCursorDeltaPosition.x) * m_fSensitivity);
					}
				}
				break;
				case SSS::GS_ROTATION:
					break;
				case SSS::GS_SCALE:
					break;
				case SSS::GS_END:
					break;
				default:
					break;
				}

				CTransform* pTargetTransform = m_pTarget->GetTransform();
				pTargetTransform->SetWorldPosition(m_pTransform->GetWorldPosition());
				SAFE_RELEASE(pTargetTransform);
			}
		}
	}



	if (!GETKEY(MOUSE_LEFT))
	{
		m_bClicked = false;
		m_iControlAxis = 0;
	}


	CTransform* pCameraTransform = m_pScene->GetMainCameraTransform();
	Vector3 vCameraPosition = pCameraTransform->GetWorldPosition();
	SAFE_RELEASE(pCameraTransform);

	Vector3 vPosition = m_pTransform->GetWorldPosition();

	Vector3 vDirection = vPosition - vCameraPosition;
	float fDistance = vDirection.Length() * m_fLocalScaleFactor;

	m_pTransform->SetWorldScale(fDistance, fDistance, fDistance);
	//m_pColliderX->SetVolume(Vector3(fDistance * 5, fDistance, fDistance));
	//m_pColliderZ->SetVolume(Vector3(fDistance, fDistance, fDistance * 5));
	//m_pColliderY->SetVolume(Vector3(fDistance, fDistance * 5, fDistance));
	return 0;
}


void CGizmo::OnMouseEnter(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
}

void CGizmo::OnMouseStay(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
}

void CGizmo::OnMouseExit(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
}

void CGizmo::OnMouseDown(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	if (GETKEY(MOUSE_LEFT))
	{
		m_bClicked = true;
		m_iControlAxis = 0;

		if (pTarget == m_pColliderX)
		{
			m_iControlAxis = GCA_X;
		}
		else if (pTarget == m_pColliderY)
		{
			m_iControlAxis = GCA_Y;
		}
		else if (pTarget == m_pColliderZ)
		{
			m_iControlAxis = GCA_Z;
		}
		else if (pTarget == m_pColliderXY)
		{
			m_iControlAxis = GCA_X | GCA_Y;
		}
		else if (pTarget == m_pColliderXZ)
		{
			m_iControlAxis = GCA_X | GCA_Z;
		}
		else if (pTarget == m_pColliderYZ)
		{
			m_iControlAxis = GCA_Y | GCA_Z;
		}
	}
}

void CGizmo::OnMouseDrag(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
}

void CGizmo::OnMouseUp(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	m_bClicked = false;
	m_iControlAxis = 0;
}


