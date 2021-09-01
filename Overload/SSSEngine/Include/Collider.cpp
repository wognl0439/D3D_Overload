#include "Collider.h"
#include "Shader.h"
#include "Mesh.h"
#include "Device.h"
#include "GameObject.h"
#include "Component\SphereCollider.h"
#include "Scene.h"
#include "Layer.h"
#include "TimerManager.h"
#include "Timer.h"
#include "Component\BoxCollider.h"
#include "CollisionManager.h"
#include "Transform.h"
#include "../Core.h"
#include "Component/Rigidbody.h"

SSS_USING

#include "RenderState.h"
#include "RenderManager.h"
#include "Material.h"
#include "ResourcesManager.h"
#include "Sampler.h"


CCollider::CCollider()
{
	m_pActor = NULL;
	m_pPhysicsMaterial = NULL;
	
	m_iBaseTypeHash = typeid(CCollider).hash_code();
	
	memset(m_pRenderState, 0, sizeof(CRenderState*) * RS_END);
	SetSampler();
	m_vColliderColor = Vector4::green;
	m_pPhysicsMaterial = m_pScene->GetPhysics()->createMaterial(0.5f, 0.5f, 0.5f);
}

CCollider::CCollider(const CCollider & component) :
	CComponent(component)
{
	m_vLocalPosition = component.m_vLocalPosition;
	
	m_pShader = component.m_pShader;
	m_pMesh = component.m_pMesh;
	m_pLayout = component.m_pLayout;

	if (m_pShader)
		m_pShader->AddRef();

	if (m_pMesh)
		m_pMesh->AddRef();
}

CCollider::~CCollider()
{
	if (m_pActor)
	{
		GET_SINGLE(CCollisionManager)->RemoveCollider(this);
		m_pScene->GetPhysicsScene()->removeActor(*m_pActor, true);
		if (m_pActor->isReleasable())
		{
			m_pActor->release();
		}
	}

	//내부에서 PhysicsMesh 해제하기 때문에 Actor가 해제되고 난 뒤에 해제해야함.
	m_tGeometry.Reset();

	//PhysicsMaterial도 마찬가지로 Actor가 해제되고 난 뒤에 해제해야함.
	if (m_pPhysicsMaterial)
	{
		size_t iRefCount = m_pPhysicsMaterial->getReferenceCount();
		for (size_t i = 0; i < iRefCount; ++i)
		{
			if (m_pPhysicsMaterial->isReleasable())
			{
				m_pPhysicsMaterial->release();
			}
		}
	}
	
	if (this == GET_SINGLE(CCollisionManager)->GetCoveredCollider())
	{
		GET_SINGLE(CCollisionManager)->ClearCoveredCollider();
	}
		
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pSampler);

	for (int i = 0; i < RS_END; ++i)
	{
		SAFE_RELEASE(m_pRenderState[i]);
	}
}

void CCollider::CreateActor(bool bStatic)
{
}

void CCollider::SetPhysicsTransform()
{
}

void CCollider::SetPhysicsMaterial(float fStaticFriction, float fDynamicFriction, float fRestitution)
{
	m_pPhysicsMaterial->setStaticFriction(fStaticFriction);
	m_pPhysicsMaterial->setDynamicFriction(fDynamicFriction);
	m_pPhysicsMaterial->setRestitution(fRestitution);
}

PxMaterial * CCollider::GetPhysicsMaterial() const
{
	return m_pPhysicsMaterial;
}

void CCollider::SetStaticFriction(float fStaticFriction)
{
	m_pPhysicsMaterial->setStaticFriction(fStaticFriction);
}

void CCollider::SetDynamicFriction(float fDynamicFriction)
{
	m_pPhysicsMaterial->setDynamicFriction(fDynamicFriction);
}

void CCollider::SetRestitution(float fRestitution)
{
	m_pPhysicsMaterial->setRestitution(fRestitution);
}

float CCollider::GetStaticFriction() const
{
	return static_cast<float>(m_pPhysicsMaterial->getStaticFriction());
}

float CCollider::GetDynamicFriction() const
{
	return static_cast<float>(m_pPhysicsMaterial->getDynamicFriction());
}

float CCollider::GetRestitution() const
{
	return static_cast<float>(m_pPhysicsMaterial->getRestitution());
}

void CCollider::SetRenderState(const string & strKey)
{	
	CRenderState*	pRenderState = GET_SINGLE(CRenderManager)->FindRenderState(strKey);

	if (!pRenderState)
		return;

	SAFE_RELEASE(m_pRenderState[pRenderState->GetRenderStateType()]);

	m_pRenderState[pRenderState->GetRenderStateType()] = pRenderState;
}

void CCollider::SetSampler()
{
	m_pSampler = GET_SINGLE(CResourcesManager)->FindSampler(SAMPLER_LINEAR);
}


void CCollider::SetDebugColor(const Vector4 & vColor)
{
	m_vColliderColor = vColor;
}

void CCollider::RemoveShapes()
{
	if (m_pActor)
	{
		if (m_vecShapes.size() <= 0)
		{
			size_t iShapeCount = m_vecShapes.size();
			for (size_t i = 0; i < iShapeCount; ++i)
			{
				m_pActor->detachShape(*m_vecShapes[i], false);
			}
		}
	}

	m_vecShapes.clear();
}

Vector3 CCollider::GetCenter() const
{
	return m_pTransform->GetWorldPosition() + m_vLocalPosition;
}

Vector3 CCollider::GetLocalPosition() const
{
	return m_vLocalPosition;
}

void CCollider::SetLocalPosition(const Vector3 & vLocalPosition)
{
	m_vLocalPosition = vLocalPosition;
}

void CCollider::SetLocalPosition(float x, float y, float z)
{
	m_vLocalPosition.x = x;
	m_vLocalPosition.y = y;
	m_vLocalPosition.z = z;
}

bool CCollider::Initialize()
{
	m_pGameObject->m_ColliderList.push_back(this);
	return true;
}

int CCollider::Update(float fTime)
{
	return 0;
}

int CCollider::LateUpdate(float fTime)
{
	return 0;
}

int CCollider::OnCollisionEnter(CCollider* pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

int CCollider::OnCollisionStay(CCollider* pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

int CCollider::OnCollisionExit(CCollider* pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

void CCollider::RenderDebug(float fTime)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		DEVICE_CONTEXT->IASetInputLayout(m_pLayout);

		m_pShader->SetShader();
		if (m_pSampler)
		{
			//Diffuse Sampler Register Numger = 0;
			m_pSampler->SetSampler(0);
		}

		if (m_pMesh)
		{
			m_pMesh->Render();
		}
	}
}