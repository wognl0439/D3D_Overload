#include "BoxCollider.h"
#include "../Transform.h"
#include "../GameObject.h"
#include "../CollisionManager.h"
#include "SphereCollider.h"
#include "../ShaderManager.h"
#include "../ResourcesManager.h"
#include "Camera.h"
#include "../Scene.h"
#include "../Mesh.h"
#include "../RenderState.h"
#include "../Core.h"
#include "Rigidbody.h"

SSS_USING

CBoxCollider::CBoxCollider():
	m_vVolume(Vector3::One)
{
	SetTag("Box Collider");

	m_pShader = GET_SINGLE(CShaderManager)->FindShader(COLLIDER_SHADER);
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(PRIMITIVE_MESH_CUBE);
	m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_COLOR_LAYOUT);
	SetRenderState(WIREFRAME);
}
CBoxCollider::~CBoxCollider()
{
	SAFE_RELEASE(m_pMesh);
}

Vector3 CBoxCollider::GetVolume() const
{
	return m_vVolume;
}

void CBoxCollider::CreateActor(bool bStatic)
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

	//Primitive 의 경우 충돌체의 포인터를 가지고 있지 않도록 한다.
	RemoveShapes();
	m_tGeometry.Reset();
	m_tGeometry.eType = CGT_BOX;
	//Exclusive : Actor가 오브젝트를 독점할것인가. 독점할 경우 Shape의 멤버로 Actor가 등록된다.
	//독점하지 않고, 돌려 쓸 경우 메모리는 아끼겠지만 해당 Actor의 고유한 크기를 가질 수 없다.
	PxBoxGeometry cGeometry(0.5f, 0.5f, 0.5f);
	PxShape* pShape = m_pScene->GetPhysics()->createShape(cGeometry, *m_pPhysicsMaterial, true);
	pShape->setFlag(PxShapeFlag::ePARTICLE_DRAIN, false);
	pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true); // 캐스팅 하려면 이거 있어야함.
	pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);	//얘랑
	pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);		//얘랑은 중복되면 안됨. 둘 중 하나만 켜져있어야함.
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

void CBoxCollider::SetPhysicsTransform()
{
	if (m_pActor)
	{
		Vector3 vPosition = m_pTransform->GetWorldPosition();
		Vector4 vQuaternion = m_pTransform->GetWorldRotation().ToQuaternion();
		PxTransform pxTransform(vPosition.x, vPosition.y, vPosition.z, PxQuat(vQuaternion.x, vQuaternion.y, vQuaternion.z, vQuaternion.w));
		m_pActor->setGlobalPose(pxTransform, true);
	}
}

void CBoxCollider::SetVolume(const Vector3 & vVolume)
{
	Vector3 vGeneralScale = m_pTransform->GetWorldScale();
		
	m_vVolume = vVolume;
	PxShape* pShape = NULL;
	m_pActor->getShapes(&pShape, 1);
	if (pShape->getGeometryType() == PxGeometryType::eBOX)
	{
		pShape->setGeometry(PxBoxGeometry(vVolume.x * vGeneralScale.x * 0.5f, vVolume.y * vGeneralScale.y * 0.5f, vVolume.z * vGeneralScale.z * 0.5f));
	}
}


void CBoxCollider::Start()
{
	CreateActor(m_pGameObject->IsStatic());

	if (m_pActor)
	{
		SetVolume(m_vVolume);
		SetPhysicsTransform();
	}
}

bool CBoxCollider::Initialize()
{
	if (CCollider::Initialize())
	{
		CreateActor(m_pGameObject->IsStatic());
	}
	return true;
}

int CBoxCollider::LateUpdate(float fTime)
{
	return 0;
}

void CBoxCollider::RenderDebug(float fDeltaTime)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		TRANSFORMCBUFFER	tBuffer = {};

		Vector3 vCenter = GetCenter();

		//영벡터
		Matrix	matScale, matPos, matWorld;
		matScale = Matrix::Scaling(m_vVolume * m_pTransform->GetWorldScale());
		matPos = Matrix::Translation(vCenter);
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

		// Transform 정보를 Shader 상수버퍼로 넘겨준다.
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

int CBoxCollider::OnCollisionEnter(CCollider * pThis, CCollider * pTarget, float fTime)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		m_vColliderColor = Vector4::red;
	}

	return 0;
}

int CBoxCollider::OnCollisionStay(CCollider * pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

int CBoxCollider::OnCollisionExit(CCollider * pThis, CCollider * pTarget, float fTime)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		m_vColliderColor = Vector4::green;
	}

	return 0;
}

void CBoxCollider::OnMouseEnter(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		m_vColliderColor = Vector4::red;
	}
}

void CBoxCollider::OnMouseExit(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		m_vColliderColor = Vector4::green;
	}
}

bool CBoxCollider::Save(FILE * pFile)
{
	float fFriction = m_pPhysicsMaterial->getStaticFriction();
	float fDynamicFriction = m_pPhysicsMaterial->getDynamicFriction();
	float fRestitution = m_pPhysicsMaterial->getRestitution();
	
	fwrite(&fFriction, sizeof(float), 1, pFile);
	fwrite(&fDynamicFriction, sizeof(float), 1, pFile);
	fwrite(&fRestitution, sizeof(float), 1, pFile);


	fwrite(&m_vLocalPosition, sizeof(Vector3), 1, pFile);
	fwrite(&m_vVolume, sizeof(Vector3), 1, pFile);
	
	bool bIsStatic = m_pGameObject->IsStatic();
	fwrite(&bIsStatic, sizeof(bool), 1, pFile);

	return true;
}

bool CBoxCollider::Load(FILE * pFile)
{
	float fFriction = 0;
	float fDynamicFriction = 0;
	float fRestitution = 0;
	

	fread(&fFriction, sizeof(float), 1, pFile);
	fread(&fDynamicFriction, sizeof(float), 1, pFile);
	fread(&fRestitution, sizeof(float), 1, pFile);

	fread(&m_vLocalPosition, sizeof(Vector3), 1, pFile);
	fread(&m_vVolume, sizeof(Vector3), 1, pFile);

	bool bIsStatic;
	fread(&bIsStatic, sizeof(bool), 1, pFile);
	
	m_pGameObject->SetStatic(bIsStatic);

	SetPhysicsMaterial(fFriction, fDynamicFriction, fRestitution);

	return true;
}
