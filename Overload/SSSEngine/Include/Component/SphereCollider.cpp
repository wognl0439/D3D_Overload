#include "SphereCollider.h"
#include "../ResourcesManager.h"
#include "../ShaderManager.h"
#include "../Transform.h"
#include "BoxCollider.h"
#include "Rigidbody.h"
#include "../CollisionManager.h"
#include "../Core.h"
#include "../SceneManager.h"

#include "Camera.h"
#include "../RenderManager.h"
#include "../RenderState.h"
#include "../Scene.h"
#include "../Layer.h"
#include "../Mesh.h"
#include "../Sampler.h"

using namespace physx;

SSS_USING

CSphereCollider::CSphereCollider()
{
	m_fRadius = 1.0f;
	SetTag("Sphere Collider");
		
	m_pShader = GET_SINGLE(CShaderManager)->FindShader(COLLIDER_SHADER);
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(PRIMITIVE_MESH_SPHERE);
	m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_COLOR_LAYOUT);

	SetRenderState(WIREFRAME);
}

CSphereCollider::CSphereCollider(const CSphereCollider & component):
	CCollider(component)
{
	*this = component;
}

CSphereCollider::~CSphereCollider()
{
	SAFE_RELEASE(m_pMesh);
}

void CSphereCollider::CreateActor(bool bStatic)
{
	if (m_pActor)
	{
		bool bRecentlyStatic = m_pActor->is<PxRigidStatic>() != NULL;
		if (bStatic == bRecentlyStatic)
		{
			return;
		}

		bool bRecentlyDynamic = m_pActor->is<PxRigidDynamic>() != NULL;
		if (!bStatic == bRecentlyDynamic)
		{
			return;
		}

		GET_SINGLE(CCollisionManager)->RemoveCollider(this);
		m_pScene->GetPhysicsScene()->removeActor(*m_pActor, true);
		if (m_pActor->isReleasable())
		{
			m_pActor->release();
			m_pActor = NULL;
		}
	}


	//Primitive �� ��� �浹ü�� �����͸� ������ ���� �ʵ��� �Ѵ�.
	RemoveShapes();
	m_tGeometry.Reset();
	m_tGeometry.eType = CGT_SPHERE;
	//Exclusive : Actor�� ������Ʈ�� �����Ұ��ΰ�. ������ ��� Shape�� ����� Actor�� ��ϵȴ�.
	//�������� �ʰ�, ���� �� ��� �޸𸮴� �Ƴ������� �ش� Actor�� ������ ũ�⸦ ���� �� ����.
	PxSphereGeometry cGeometry(0.5f);
	PxShape* pShape = m_pScene->GetPhysics()->createShape(cGeometry, *m_pPhysicsMaterial, true);
	pShape->setFlag(PxShapeFlag::ePARTICLE_DRAIN, false);
	pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true); // ĳ���� �Ϸ��� �̰� �־����.
	pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);	//���
	pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);		//����� �ߺ��Ǹ� �ȵ�. �� �� �ϳ��� �����־����.
	pShape->setFlag(PxShapeFlag::eVISUALIZATION, true);
	m_vecShapes.push_back(pShape);

	PxVec3 vT(0.0f, 0.0f, 0.0f);
	PxQuat vQ(0.0f, 0.0f, 0.0f, 1.0f);
	PxTransform pTransform(vT, vQ);

	if (!bStatic)
	{
		//Dynamic
		PxRigidDynamic* pActor = m_pScene->GetPhysics()->createRigidDynamic(pTransform);
		m_pActor = pActor;
		pActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		pActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		pActor->setName(m_pGameObject->GetTag().c_str());
		pActor->attachShape(*pShape);
		pShape->release();
	}
	else
	{
		//Static
		PxRigidStatic* pActor = m_pScene->GetPhysics()->createRigidStatic(pTransform);
		m_pActor = pActor;
		pActor->setName(m_pGameObject->GetTag().c_str());
		pActor->attachShape(*pShape);
		pShape->release();
	}

	SetPhysicsTransform();
	GET_SINGLE(CCollisionManager)->AddCollider(this);
}

void CSphereCollider::SetPhysicsTransform()
{
	if (m_pActor)
	{
		Vector3 vPosition = m_pTransform->GetWorldPosition();
		Vector4 vQuaternion = m_pTransform->GetWorldRotation().ToQuaternion();
		PxTransform pxTransform(vPosition.x, vPosition.y, vPosition.z, PxQuat(vQuaternion.x, vQuaternion.y, vQuaternion.z, vQuaternion.w));
		m_pActor->setGlobalPose(pxTransform, true);
	}
}

float CSphereCollider::GetRadius() const
{
	return m_fRadius;
}

void CSphereCollider::SetRadius(float fRadius)
{
	float fScaleMax = m_pTransform->GetWorldScale().Max();
	m_fRadius = fRadius;
	PxShape* pShape = NULL;
	m_pActor->getShapes(&pShape, 1);
	pShape->setGeometry(PxSphereGeometry(m_fRadius * fScaleMax));
}


void CSphereCollider::Start()
{
	CreateActor(m_pGameObject->IsStatic());

	if (m_pActor)
	{
		SetRadius(m_fRadius);
		SetPhysicsTransform();
	}
}

bool CSphereCollider::Initialize()
{
	if (CCollider::Initialize())
	{
		CreateActor(m_pGameObject->IsStatic());
	}
	return true;
}

int CSphereCollider::LateUpdate(float fTime)
{
	return 0;
}

void CSphereCollider::RenderDebug(float fDeltaTime)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		TRANSFORMCBUFFER	tBuffer = {};

		//������
		//�޽��� ���ؽ��� ���̰� 1�̹Ƿ�, scale�� max�� �ȴ�.

		Matrix	matScale, matPos, matWorld;
		matScale = Matrix::Scaling(Vector3::One * m_fRadius * 2.0f);
		matPos = Matrix::Translation(GetCenter());
		matWorld = matScale * m_pTransform->GetWorldRotationMatrix() * matPos;

		CCamera*	pCamera = m_pScene->GetMainCamera();

		tBuffer.matWorld = matWorld;
		tBuffer.matView = pCamera->GetViewMatrix();
		tBuffer.matProjection = pCamera->GetPerspectiveProjectionMatrix();
		tBuffer.matWV = matWorld * tBuffer.matView;
		tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
		tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
		tBuffer.matWP = tBuffer.matWorld * tBuffer.matProjection;
		tBuffer.vPivot = Vector3::Zero;
		tBuffer.vLength = m_pMesh->GetLength();

		SAFE_RELEASE(pCamera);

		tBuffer.matWorld = tBuffer.matWorld.Transpose();
		tBuffer.matView = tBuffer.matView.Transpose();
		tBuffer.matProjection = tBuffer.matProjection.Transpose();
		tBuffer.matWV = tBuffer.matWV.Transpose();
		tBuffer.matWVP = tBuffer.matWVP.Transpose();
		tBuffer.matVP = tBuffer.matVP.Transpose();
		tBuffer.matWP = tBuffer.matWP.Transpose();

		// Transform ������ Shader ������۷� �Ѱ��ش�.
		GET_SINGLE(CShaderManager)->UpdateConstantBuffer(TRANSFORM_CBUFFER, &tBuffer, SSS::CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);


		for (int i = 0; i < RS_END; ++i)
		{
			if (m_pRenderState[i])
				m_pRenderState[i]->SetState();
		}

		COLLIDERCBUFFER tColliderBuffer = {};
		tColliderBuffer.bUI = 0;
		tColliderBuffer.vColor = m_vColliderColor;
		GET_SINGLE(CShaderManager)->UpdateConstantBuffer(COLLIDER_CBUFFER, &tColliderBuffer, SSS::CBT_VERTEX);
		CCollider::RenderDebug(fDeltaTime);


		for (int i = 0; i < RS_END; ++i)
		{
			if (m_pRenderState[i])
				m_pRenderState[i]->ResetState();
		}
	}
	
}

int CSphereCollider::OnCollisionEnter(CCollider * pThis, CCollider * pTarget, float fTime)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		m_vColliderColor = Vector4::red;
	}
	return 0;
}

int CSphereCollider::OnCollisionStay(CCollider * pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

int CSphereCollider::OnCollisionExit(CCollider * pThis, CCollider * pTarget, float fTime)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		m_vColliderColor = Vector4::green;
	}
	return 0;
}

void CSphereCollider::OnMouseEnter(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		m_vColliderColor = Vector4::red;
	}
}

void CSphereCollider::OnMouseExit(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		m_vColliderColor = Vector4::green;
	}
}

CSphereCollider * CSphereCollider::Clone() const
{
	return new CSphereCollider(*this);
}

bool CSphereCollider::Save(FILE * pFile)
{
	float fFriction = m_pPhysicsMaterial->getStaticFriction();
	float fDynamicFriction = m_pPhysicsMaterial->getDynamicFriction();
	float fRestitution = m_pPhysicsMaterial->getRestitution();
	bool bIsStatic = m_pGameObject->IsStatic();

	fwrite(&fFriction, sizeof(float), 1, pFile);
	fwrite(&fDynamicFriction, sizeof(float), 1, pFile);
	fwrite(&fRestitution, sizeof(float), 1, pFile);

	fwrite(&m_vLocalPosition, sizeof(Vector3), 1, pFile);
	fwrite(&m_fRadius, sizeof(float), 1, pFile);
	fwrite(&bIsStatic, sizeof(bool), 1, pFile);

	return true;
}

bool CSphereCollider::Load(FILE * pFile)
{
	float fFriction = 0;
	float fDynamicFriction = 0;
	float fRestitution = 0;
	bool bIsStatic = false;

	fread(&fFriction, sizeof(float), 1, pFile);
	fread(&fDynamicFriction, sizeof(float), 1, pFile);
	fread(&fRestitution, sizeof(float), 1, pFile);

	fread(&m_vLocalPosition, sizeof(Vector3), 1, pFile);
	fread(&m_fRadius, sizeof(float), 1, pFile);
	fread(&bIsStatic, sizeof(bool), 1, pFile);
	SetRadius(m_fRadius);

	SetPhysicsMaterial(fFriction, fDynamicFriction, fRestitution);
	return true;
}
