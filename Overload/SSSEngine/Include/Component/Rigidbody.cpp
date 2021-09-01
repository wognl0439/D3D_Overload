#include "Rigidbody.h"
#include "../CollisionManager.h"
#include "../Collider.h"
#include "BoxCollider.h"
#include "MeshCollider.h"
#include "SphereCollider.h"
#include "TerrainCollider.h"

#include "../Transform.h"
#include "../Debug.h"

SSS_USING

CRigidbody::CRigidbody():
	m_bIsKinematic(false)
	, m_bUseGravity(true)
	, m_fMass(1.0f)
	, m_fDrag(0.0f)
	, m_fAngularDrag(0.0f)
{
}

CRigidbody::~CRigidbody()
{
}

void CRigidbody::SetKinematic(bool bIsKinematic)
{
	m_bIsKinematic = bIsKinematic;
	PxRigidStatic* pStatic = m_pRigidbodyActor->is<PxRigidStatic>();
	if (pStatic)
	{
		return;
	}

	if (m_pRigidbodyActor)
	{
		PxRigidDynamic* pActor = m_pRigidbodyActor->is<PxRigidDynamic>();
		CCollider* pCollider = GET_SINGLE(CCollisionManager)->GetCollider(pActor);
		
		COLLIDER_GEOMETRY_TYPE eType = pCollider->m_tGeometry.eType;

			//항상 Static 이어야 하는 모양들이다.
		if (eType == CGT_TRIANGLE_MESH || eType == CGT_TERRAIN)
		{
			return;
		}

		if (!m_pGameObject->IsStatic() && pActor)
		{
			pActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, m_bIsKinematic);

			PxShape* pShape = NULL;
			uint32_t iShapeCount = pActor->getNbShapes();
			pActor->getShapes(&pShape, iShapeCount, 0);

			if (eType == CGT_CONVEX_MESH)
			{
				for (uint32_t i = 0; i < iShapeCount; ++i)
				{
					pShape[i].setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
					pShape[i].setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
				}
			}
			else
			{
				for (uint32_t i = 0; i < iShapeCount; ++i)
				{
					pShape[i].setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
					pShape[i].setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);

					pShape[i].setFlag(PxShapeFlag::eSIMULATION_SHAPE, !m_bIsKinematic);
					pShape[i].setFlag(PxShapeFlag::eTRIGGER_SHAPE, m_bIsKinematic);
				}
			}
		}

		if (!bIsKinematic)
		{
			//ClearForce();
			pActor->wakeUp();
		}
	}
}

void CRigidbody::SetUseGravity(bool bUse)
{
	m_bUseGravity = bUse;
	if (m_pRigidbodyActor)
	{
		PxRigidStatic* pStatic = m_pRigidbodyActor->is<PxRigidStatic>();
		PxRigidDynamic* pDynamic = m_pRigidbodyActor->is<PxRigidDynamic>();
		if (pStatic)
		{
			return;
		}
		if (pDynamic)
		{
			m_pRigidbodyActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !bUse);

			if(!m_bIsKinematic)
				ClearForce();
				pDynamic->wakeUp();
		}
	}
}

void CRigidbody::SetMass(float fMass)
{
	m_fMass = fMass;	
	if (m_pRigidbodyActor)
	{
		PxRigidDynamic* pActor = m_pRigidbodyActor->is<PxRigidDynamic>();
		pActor->setMass(fMass);
	}
}

void CRigidbody::SetDrag(float fDrag)
{
	m_fDrag = fDrag; 
	if (m_pRigidbodyActor)
	{
		PxRigidDynamic* pActor = m_pRigidbodyActor->is<PxRigidDynamic>();
		pActor->setLinearDamping(m_fDrag);
	}
}

void CRigidbody::SetAngularDrag(float fAngularDrag)
{
	m_fAngularDrag = fAngularDrag;
	if (m_pRigidbodyActor)
	{
		PxRigidDynamic* pActor = m_pRigidbodyActor->is<PxRigidDynamic>();
		pActor->setAngularDamping(m_fAngularDrag);
	}
}

void CRigidbody::ClearForce()
{
	if (m_pRigidbodyActor)
	{
		PxRigidDynamic* pDynamic = m_pRigidbodyActor->is<PxRigidDynamic>();
		if (pDynamic)
		{
			PxRigidBodyFlags eFlag = pDynamic->getRigidBodyFlags();
			if (!(eFlag & PxRigidBodyFlag::eKINEMATIC))
			{
				pDynamic->clearForce(PxForceMode::eACCELERATION);
				pDynamic->clearForce(PxForceMode::eFORCE);
				pDynamic->clearForce(PxForceMode::eIMPULSE);
				pDynamic->clearForce(PxForceMode::eIMPULSE);
				pDynamic->clearForce(PxForceMode::eVELOCITY_CHANGE);
			}
		}
	}
}

bool CRigidbody::IsKinematic() const
{
	return m_bIsKinematic;
}

bool CRigidbody::UseGravity() const
{
	return m_bUseGravity;
}

float CRigidbody::GetMass() const
{
	return m_fMass;
}

float CRigidbody::GetDrag() const
{
	return m_fDrag;
}

float CRigidbody::GetAngularDrag() const
{
	return m_fAngularDrag;
}

void CRigidbody::AddForce(const Vector3 & vForce)
{
	if (m_pRigidbodyActor)
	{
		if (!m_bIsKinematic)
		{
			m_pRigidbodyActor->is<PxRigidDynamic>()->addForce(PxVec3(vForce.x, vForce.y, vForce.z));
		}
	}
}

bool CRigidbody::Initialize()
{
	/*
	* Types
	* 0 :: None
	* 1 :: Box Collider
	* 2 :: Sphere Collider
	* 3 :: Mesh Collider
	* 4 :: Terrain Collider
	*/
	uint32_t iType = 0;
	CCollider* pBaseCollider = NULL;

	iType = m_pGameObject->HasComponent<CBoxCollider>() ? 1 : 0;
	
	if(iType == 0)
		iType = m_pGameObject->HasComponent<CSphereCollider>() ? 2 : 0;

	if (iType == 0)
		iType = m_pGameObject->HasComponent<CMeshCollider>() ? 3 : 0;

	if (iType == 0)
		iType = m_pGameObject->HasComponent<CTerrainCollider>() ? 4 : 0;
	
	if (iType == 0)
	{
		GET_SINGLE(CDebug)->Log(L"Rigidbody는 반드시 Collider가 존재해야 합니다.");
		return false;
	}
	else
	{
		switch (iType)
		{
		case 1:
		{
			CBoxCollider* pCollider = GetComponent<CBoxCollider>();
			pBaseCollider = pCollider;
			m_pRigidbodyActor = pCollider->m_pActor;
			SAFE_RELEASE(pCollider);
		}
			break;
		case 2:
		{
			CSphereCollider* pCollider = GetComponent<CSphereCollider>();
			pBaseCollider = pCollider;
			m_pRigidbodyActor = pCollider->m_pActor;
			SAFE_RELEASE(pCollider);
		}
		break;
		case 3:
		{
			CMeshCollider* pCollider = GetComponent<CMeshCollider>();
			pBaseCollider = pCollider;
			m_pRigidbodyActor = pCollider->m_pActor;
			SAFE_RELEASE(pCollider);
		}
		break;
		case 4:
		{
			GET_SINGLE(CDebug)->Log(L"Terrain Collider는 Rigidbody의 영향을 받을 수 없습니다.");		
		}
		break;

		default:
			break;
		}
	}
	

	if (m_pRigidbodyActor)
	{
		PxRigidDynamic* pActor = m_pRigidbodyActor->is<PxRigidDynamic>();
		if (!m_pGameObject->IsStatic() && pActor)
		{
			SetUseGravity(m_bUseGravity);
			SetKinematic(m_bIsKinematic);
		}
		else
		{
			GET_SINGLE(CDebug)->Log(L"Static 오브젝트 또는 Static 충돌체는 Rigidbody의 영향을 받지 않습니다.");
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

void CRigidbody::Start()
{
	if (m_pRigidbodyActor)
	{
		PxRigidDynamic* pRigidbody = m_pRigidbodyActor->is<PxRigidDynamic>();
		if (pRigidbody)
		{
			SetKinematic(m_bIsKinematic);
			pRigidbody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !m_bUseGravity);
			pRigidbody->setMass(m_fMass);
			pRigidbody->setLinearDamping(m_fDrag);
			pRigidbody->setAngularDamping(m_fAngularDrag);

			if (!m_bIsKinematic)
				pRigidbody->wakeUp();
		}		
	}
}

int CRigidbody::Update(float fTime)
{
	return 0;
}

bool CRigidbody::Save(FILE * pFile)
{
	fwrite(&m_bIsKinematic, sizeof(bool), 1, pFile);
	fwrite(&m_bUseGravity, sizeof(bool), 1, pFile);
	fwrite(&m_fMass, sizeof(float), 1, pFile);
	fwrite(&m_fDrag, sizeof(float), 1, pFile);
	fwrite(&m_fAngularDrag, sizeof(float), 1, pFile);
	return true;
}

bool CRigidbody::Load(FILE * pFile)
{
	fread(&m_bIsKinematic, sizeof(bool), 1, pFile);
	fread(&m_bUseGravity, sizeof(bool), 1, pFile);
	fread(&m_fMass, sizeof(float), 1, pFile);
	fread(&m_fDrag, sizeof(float), 1, pFile);
	fread(&m_fAngularDrag, sizeof(float), 1, pFile);

	SetKinematic(m_bIsKinematic);
	SetUseGravity(m_bUseGravity);
	SetMass(m_fMass);
	SetDrag(m_fDrag);
	SetAngularDrag(m_fAngularDrag);

	return true;
}
