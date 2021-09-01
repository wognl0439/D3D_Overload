#include "MeshCollider.h"
#include "../ShaderManager.h"
#include "../ResourcesManager.h"
#include "../CollisionManager.h"
#include "../Transform.h"
#include "../Mesh.h"
#include "MeshRenderer.h"
#include "../Debug.h"
#include "../Scene.h"
#include "Rigidbody.h"
#include "../RenderState.h"
#include "Camera.h"
#include "../Core.h"

SSS_USING

CMeshCollider::CMeshCollider()
{
	m_vVolume = Vector3::One;

	SetTag("Mesh Collider");

	m_pShader = GET_SINGLE(CShaderManager)->FindShader(COLLIDER_SHADER);
	m_pMesh = NULL;
	m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_COLOR_LAYOUT);

	SetRenderState(WIREFRAME);
}


CMeshCollider::~CMeshCollider()
{
}

void CMeshCollider::CreateActor(bool bStatic)
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

		m_tGeometry.Reset();
	}
	
	CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();
	if (bStatic)
		BakeTriangleMesh(pRenderer->GetMesh());
	else
		BakeConvexMesh(pRenderer->GetMesh());
	SAFE_RELEASE(pRenderer);
}

void CMeshCollider::SetPhysicsTransform()
{
	if (m_pActor)
	{
		Matrix matWorld = m_pTransform->GetWorldMatrix();
		Vector4 vQuaternion;
		Vector3 vPosition;
		Vector3 vScale;
		Matrix::Decompose(matWorld, &vPosition, &vQuaternion, &vScale);
		PxTransform pxTransform(vPosition.x, vPosition.y, vPosition.z, PxQuat(vQuaternion.x, vQuaternion.y, vQuaternion.z, vQuaternion.w));
		m_pActor->setGlobalPose(pxTransform, true);
	}
}

void CMeshCollider::SetVolume(const Vector3 vScale)
{
	if (m_pActor)
	{
		m_vVolume = vScale;
		if (m_tGeometry.vecResourceData.size() > 0)
		{
			CCollider* pCollider = GET_SINGLE(CCollisionManager)->GetCollider(m_pActor);
			if (m_tGeometry.eType == CGT_CONVEX_MESH)
			{
				size_t iShapeCount = m_vecShapes.size();				
				for (size_t i = 0; i < iShapeCount; ++i)
				{
					m_pActor->detachShape(*m_vecShapes[i], false); 
				}

				Vector3 vGeneralScale = m_pTransform->GetWorldScale();
				PxMeshScale tScale(PxVec3(vGeneralScale.x * vScale.x, vGeneralScale.y *  vScale.y, vGeneralScale.z *  vScale.z));
				
				size_t iGeometryCount = m_tGeometry.vecResourceData.size();
				RemoveShapes();
				for (size_t i = 0; i < iGeometryCount; ++i)
				{
					PxConvexMeshGeometry pGeometry((PxConvexMesh*)m_tGeometry.vecResourceData[i], tScale);
					PxShape* pShape = GET_SINGLE(CCollisionManager)->GetPhysics()->createShape(pGeometry, *m_pPhysicsMaterial, true);
					m_pActor->attachShape(*pShape);
					m_vecShapes.push_back(pShape);
					pShape->release();
				}
			}
			else if (m_tGeometry.eType == CGT_TRIANGLE_MESH)
			{
				size_t iShapeCount = m_vecShapes.size();
				for (size_t i = 0; i < iShapeCount; ++i)
				{
					m_pActor->detachShape(*m_vecShapes[i], false);
				}

				Vector3 vGeneralScale = m_pTransform->GetWorldScale();
				PxMeshScale tScale(PxVec3(vGeneralScale.x * vScale.x, vGeneralScale.y *  vScale.y, vGeneralScale.z *  vScale.z));

				size_t iGeometryCount = m_tGeometry.vecResourceData.size();
				RemoveShapes();

				for (size_t i = 0; i < iGeometryCount; ++i)
				{
					PxTriangleMeshGeometry pGeometry((PxTriangleMesh*)m_tGeometry.vecResourceData[i], tScale);
					PxShape* pShape = GET_SINGLE(CCollisionManager)->GetPhysics()->createShape(pGeometry, *m_pPhysicsMaterial, true);
					
					m_pActor->attachShape(*pShape);
					
					m_vecShapes.push_back(pShape);
					pShape->release();
				}			
			}	
		}
	}
}

bool CMeshCollider::BakeTriangleMesh(CMesh * pMesh)
{
	CMesh* pBaseMesh = pMesh;
	if (!pBaseMesh)
	{
		CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();
		if (!pRenderer)
		{
			GET_SINGLE(CDebug)->Log(L"오브젝트가 Mesh Renderer컴포넌트를 가지고 있지 않은 상태에서 Bake하려 했습니다.");
			return false;
		}

		pBaseMesh = pRenderer->GetMesh();
		SAFE_RELEASE(pRenderer);
		if (!pBaseMesh)
		{
			GET_SINGLE(CDebug)->Log(L"Mesh Renderer 에서 Mesh를 가지고 있지 않습니다.");
			return false;
		}
	}

	//Refcount 올리지 않음.
	m_pMesh = pBaseMesh;

	/*원본에서 PhysicsMesh의 VertexData 뽑아냄*/
	vector<Vector3> vecPoints;
	size_t iCount = pBaseMesh->m_vecMeshContainer[0]->tVB.iCount;
	size_t iStride = pBaseMesh->m_vecMeshContainer[0]->tVB.iSize;
	char* pOffset = (char*)pBaseMesh->m_vecMeshContainer[0]->tVB.pData;
	for (size_t i = 0; i < iCount; ++i)
	{
		char* pCurrentVertex = pOffset + (iStride * i);
		Vector3 vPoint;
		memcpy(&vPoint, pCurrentVertex, sizeof(Vector3));
		vecPoints.push_back(vPoint);
	}

	m_tGeometry.Reset();
	m_tGeometry.eType = CGT_TRIANGLE_MESH;

	PxTriangleMeshDesc tDesc;
	size_t iSubsetCount = pBaseMesh->m_vecMeshContainer[0]->vecIB.size();
	tDesc.points.count = pBaseMesh->m_vecMeshContainer[0]->tVB.iCount;

	/*원본에서 PhysicsMesh를 뽑아냄*/
	vector<PxTriangleMesh*> vecTriangleMeshes;
	for (size_t i = 0; i < iSubsetCount; ++i)
	{
		tDesc.triangles.count = pBaseMesh->m_vecMeshContainer[0]->vecIB[i].iCount / 3;
		tDesc.points.stride = sizeof(Vector3);
		tDesc.triangles.stride = pBaseMesh->m_vecMeshContainer[0]->vecIB[i].iSize * 3;
		tDesc.points.data = &vecPoints[0];
		tDesc.triangles.data = pBaseMesh->m_vecMeshContainer[0]->vecIB[i].pData;
		tDesc.flags = PxMeshFlags(0);

		PxDefaultMemoryOutputStream writeBuffer;
		PxTriangleMeshCookingResult::Enum result;
		bool bResult = GET_SINGLE(CCollisionManager)->GetCooking()->cookTriangleMesh(tDesc, writeBuffer, &result);

		if (!bResult)
			return false;

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		PxTriangleMesh* pPhysicsMesh = GET_SINGLE(CCollisionManager)->GetPhysics()->createTriangleMesh(readBuffer);
		size_t iRefCount = pPhysicsMesh->getReferenceCount();

		bool bIsStatic = m_pGameObject->IsStatic();
		bool bIsKinematic = true;
		CRigidbody* pRigidbody = GetComponent<CRigidbody>();
		if (pRigidbody)
		{
			bIsKinematic = pRigidbody->IsKinematic();
			SAFE_RELEASE(pRigidbody);
		}

		m_tGeometry.vecResourceData.push_back(pPhysicsMesh);
		vecTriangleMeshes.push_back(pPhysicsMesh);
	}
	/*************************/

	Vector3 vPosition = m_pTransform->GetWorldPosition();
	Vector4 vQuaternion = m_pTransform->GetWorldRotation().ToQuaternion();
	PxTransform cTransform(vPosition.x, vPosition.y, vPosition.z, PxQuat(vQuaternion.x, vQuaternion.y, vQuaternion.z, vQuaternion.w));
	
	//Mesh Collider는 항상 Static 이다.
	m_pGameObject->SetStatic(true);
	m_pActor = dynamic_cast<PxRigidActor*>(m_pScene->GetPhysics()->createRigidStatic(cTransform));
	((PxRigidStatic*)m_pActor)->setName(m_pGameObject->GetTag().c_str());

	RemoveShapes();	
	size_t iTriangleMeshCount = vecTriangleMeshes.size();
	for (size_t i = 0; i < iTriangleMeshCount; ++i)
	{				
		size_t iRefCount = vecTriangleMeshes[i]->getReferenceCount();
		PxShape* pShape = GET_SINGLE(CCollisionManager)->GetPhysics()->createShape(PxTriangleMeshGeometry(vecTriangleMeshes[i]), *m_pPhysicsMaterial, true);
		iRefCount = vecTriangleMeshes[i]->getReferenceCount();

		pShape->setFlag(PxShapeFlag::ePARTICLE_DRAIN, false);
		pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
		pShape->setFlag(PxShapeFlag::eVISUALIZATION, true);

		m_pActor->attachShape(*pShape);
		iRefCount = vecTriangleMeshes[i]->getReferenceCount();

		m_vecShapes.push_back(pShape);				
		pShape->release();	
	}

	if (m_vecShapes.size() <= 0)
	{
		GET_SINGLE(CDebug)->Log(L"Terrain Collider Shape 생성에 실패하였습니다.");
	}

	GET_SINGLE(CCollisionManager)->AddCollider(this);

	return true;
}

bool CMeshCollider::BakeConvexMesh(CMesh * pMesh)
{
	//추후에 인덱스 버퍼에 해당하는 버텍스만 뽑아서 Bake 할 수 있도록 수정
	CMesh* pBaseMesh = pMesh;
	if (!pBaseMesh)
	{
		CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();
		if (!pRenderer)
		{
			GET_SINGLE(CDebug)->Log(L"오브젝트가 Mesh Renderer컴포넌트를 가지고 있지 않은 상태에서 Bake하려 했습니다.");
			return false;
		}

		pBaseMesh = pRenderer->GetMesh();
		SAFE_RELEASE(pRenderer);
		if (!pBaseMesh)
		{
			GET_SINGLE(CDebug)->Log(L"Mesh Renderer 에서 Mesh를 가지고 있지 않습니다.");
			return false;
		}
	}

	//Refcount 올리지 않음.
	//SAFE_RELEASE(m_pMesh);
	m_pMesh = pBaseMesh;

	/*원본에서 PhysicsMesh를 뽑아냄*/
	vector<Vector3> vecPoints;
	size_t iCount = pBaseMesh->m_vecMeshContainer[0]->tVB.iCount;
	size_t iStride = pBaseMesh->m_vecMeshContainer[0]->tVB.iSize;
	char* pOffset = (char*)pBaseMesh->m_vecMeshContainer[0]->tVB.pData;
	for (size_t i = 0; i < iCount; ++i)
	{
		char* pCurrentVertex = pOffset + (iStride * i);
		Vector3 vPoint;
		memcpy(&vPoint, pCurrentVertex, sizeof(Vector3));
		vecPoints.push_back(vPoint);
	}
	/*************************/

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = iCount;
	convexDesc.points.stride = sizeof(Vector3);
	convexDesc.points.data = &vecPoints[0];
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
	convexDesc.vertexLimit = 128;

	PxDefaultMemoryOutputStream buffer;
	if (!GET_SINGLE(CCollisionManager)->GetCooking()->cookConvexMesh(convexDesc, buffer))
		return false;


	PxDefaultMemoryInputData readBuffer(buffer.getData(), buffer.getSize());

	PxConvexMesh* pPxMesh = GET_SINGLE(CCollisionManager)->GetPhysics()->createConvexMesh(readBuffer);
	
	if (!pPxMesh)
		return false;


	RemoveShapes();
	m_tGeometry.Reset();
	m_tGeometry.eType = CGT_CONVEX_MESH;	
	m_tGeometry.vecResourceData.push_back(pPxMesh);

	PxShape* pTempShape = GET_SINGLE(CCollisionManager)->GetPhysics()->createShape(PxConvexMeshGeometry(pPxMesh), *m_pPhysicsMaterial, true);
	pTempShape->setFlag(PxShapeFlag::ePARTICLE_DRAIN, false);
	pTempShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	pTempShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	pTempShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	pTempShape->setFlag(PxShapeFlag::eVISUALIZATION, true);

	m_vecShapes.push_back(pTempShape);

	bool bIsStatic = m_pGameObject->IsStatic();
	bool bIsKinematic = true;
	CRigidbody* pRigidbody = GetComponent<CRigidbody>();
	if (pRigidbody)
	{
		bIsKinematic = pRigidbody->IsKinematic();
		SAFE_RELEASE(pRigidbody);
	}

	if (bIsKinematic)
	{
		if (bIsStatic)
		{
			PxTransform pxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat(0.0f, 0.0f, 0.0f, 1.0f));

			//Static
			m_pActor = m_pScene->GetPhysics()->createRigidStatic(pxTransform);
			m_pActor->setName(m_pGameObject->GetTag().c_str());

			size_t iShapeCount = m_vecShapes.size();
			for (size_t i = 0; i < iShapeCount; ++i)
			{
				PxShape* pShape = m_vecShapes[i];
				m_pActor->attachShape(*pShape); 
				pShape->release();
			}
		}
		else
		{
			//Dynamic Collider Using Kinematic
			PxTransform pxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat(0.0f, 0.0f, 0.0f, 1.0f));
			PxRigidDynamic* pActor = m_pScene->GetPhysics()->createRigidDynamic(pxTransform);
			m_pActor = pActor;
			pActor->setName(m_pGameObject->GetTag().c_str());
			pActor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
			pActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
			pActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			
			size_t iShapeCount = m_vecShapes.size();
			for (size_t i = 0; i < iShapeCount; ++i)
			{
				PxShape* pShape = m_vecShapes[i];
				m_pActor->attachShape(*pShape);
				pShape->release();
			}
		}
	}
	else
	{
		PxTransform pxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat(0.0f, 0.0f, 0.0f, 1.0f));
		//Dynamic Collider No Using Kinematic
		PxRigidDynamic* pActor = m_pScene->GetPhysics()->createRigidDynamic(pxTransform);
		m_pActor = pActor;
		pActor->setName(m_pGameObject->GetTag().c_str());
		pActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
		pActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		size_t iShapeCount = m_vecShapes.size();
		for (size_t i = 0; i < iShapeCount; ++i)
		{
			PxShape* pShape = m_vecShapes[i];
			m_pActor->attachShape(*pShape);
			pShape->release();
		}
	}

	GET_SINGLE(CCollisionManager)->AddCollider(this);
	SetPhysicsTransform(); 
	return true;
}

bool CMeshCollider::Initialize()
{
	if (CCollider::Initialize())
	{
		//CreateActor(m_pGameObject->IsStatic());
	}
	return true;
}

void CMeshCollider::Start()
{
	CreateActor(m_pGameObject->IsStatic());
	if (m_pActor)
	{
		SetVolume(m_vVolume);
		SetPhysicsTransform();
	}
}

void CMeshCollider::RenderDebug(float fDeltaTime)
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

void CMeshCollider::OnMouseEnter(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		m_vColliderColor = Vector4::red;
	}
}

void CMeshCollider::OnMouseExit(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		m_vColliderColor = Vector4::green;
	}
}

bool CMeshCollider::Save(FILE * pFile)
{
	float fFriction = m_pPhysicsMaterial->getStaticFriction();
	float fDynamicFriction = m_pPhysicsMaterial->getDynamicFriction();
	float fRestitution = m_pPhysicsMaterial->getRestitution();
	bool bIsStatic = m_pGameObject->IsStatic();

	fwrite(&fFriction, sizeof(float), 1, pFile);
	fwrite(&fDynamicFriction, sizeof(float), 1, pFile);
	fwrite(&fRestitution, sizeof(float), 1, pFile);
	fwrite(&bIsStatic, sizeof(bool), 1, pFile);	
	fwrite(&m_tGeometry.eType, sizeof(COLLIDER_GEOMETRY_TYPE), 1, pFile);

	return true;
}

bool CMeshCollider::Load(FILE * pFile)
{
	float fFriction = 0;
	float fDynamicFriction = 0;
	float fRestitution = 0;
	bool bIsStatic = false;

	fread(&fFriction, sizeof(float), 1, pFile);
	fread(&fDynamicFriction, sizeof(float), 1, pFile);
	fread(&fRestitution, sizeof(float), 1, pFile);

	fread(&bIsStatic, sizeof(bool), 1, pFile);
	m_pGameObject->SetStatic(bIsStatic);
	COLLIDER_GEOMETRY_TYPE eType;
	fread(&eType, sizeof(COLLIDER_GEOMETRY_TYPE), 1, pFile);
	//로드 시 GameObject의 Static인지 저장이 되지 않으므로, 항상 Covex로 정보가 저장 되어 있음.
	//따라서 Convex를 Triangle로 저장할 필요가 있음.
	
	GET_SINGLE(CCollisionManager)->RemoveCollider(this);
	if (m_pActor)
	{
		m_pScene->GetPhysicsScene()->removeActor(*m_pActor, true);
		m_pActor->release();
		m_pActor = NULL;
	}

	RemoveShapes();
	m_tGeometry.Reset();
	CreateActor(bIsStatic);
	
	SetPhysicsMaterial(fFriction, fDynamicFriction, fRestitution);
	return true;
}
