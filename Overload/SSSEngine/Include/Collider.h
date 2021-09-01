#pragma once
#include "Component.h"
#include "PxPhysicsAPI.h"
using namespace physx;

SSS_BEGIN

enum COLLIDER_GEOMETRY_TYPE
{
	CGT_NONE,
	CGT_BOX,
	CGT_SPHERE,
	CGT_TRIANGLE_MESH,
	CGT_CONVEX_MESH,
	CGT_TERRAIN,
	CGT_END
};
typedef struct SSS_DLL  _tagGeometryResource
{
	COLLIDER_GEOMETRY_TYPE eType;
	vector<void*> vecResourceData;

	_tagGeometryResource():
		eType(CGT_NONE)
	{};

	void Reset()
	{
		size_t iShapeCount = vecResourceData.size();
		for (size_t i = 0; i < iShapeCount; ++i)
		{
			uint32_t iRefCount = 0;
			PxBase* pMesh = NULL;
			if (eType == CGT_TRIANGLE_MESH)
			{
				pMesh = (PxTriangleMesh*)vecResourceData[i];
				iRefCount = ((PxTriangleMesh*)pMesh)->getReferenceCount();
			}
			else if (eType == CGT_CONVEX_MESH)
			{
				pMesh = (PxConvexMesh*)vecResourceData[i];
				iRefCount = ((PxConvexMesh*)pMesh)->getReferenceCount();
			}
			else if (eType == CGT_TERRAIN)
			{
				pMesh = (PxHeightField*)vecResourceData[i];
				iRefCount = ((PxHeightField*)pMesh)->getReferenceCount();
			}

			//����
			for (size_t i = 0; i < iRefCount; ++i)
			{
				if (pMesh->isReleasable())
					pMesh->release();
			}
		}

		eType = CGT_NONE;		
		vecResourceData.clear();
	}
}GEOMETRYRESOURCE, *PGEOMETRYRESOURCE;


typedef struct SSS_DLL _tagCollisionArea
{
	Vector3 vMin;
	Vector3 vMax;
	Vector3 vSize;
	Vector3 vRenderMin;
	Vector3 vRenderMax;

	_tagCollisionArea() :
		vMin(),
		vMax(),
		vSize(),
		vRenderMin(),
		vRenderMax()
	{
	}
}COLLISIONAREA, *PCOLLISIONAREA;



class SSS_DLL CCollider :
	public CComponent
{
private:
	friend class CGameObject;
	friend class CCollisionManager;
	friend class CRigidbody;

public:
	CCollider();
	CCollider(const CCollider& component);
	virtual ~CCollider();



/***************PhysX***************/
protected:
	class PxRigidActor* m_pActor;
	vector<class PxShape*> m_vecShapes;// �ڽ� Ŭ�������� �����ϰ� �θ� �Ҹ��ڿ��� ����.
	GEOMETRYRESOURCE m_tGeometry; //��� �浹ü�� Exclusive�ϱ� ������ Dynamic �� Static ���� ��ȯ, �� Static�� Kinematic���� ��ȯ�� ���ؼ��� Shape�� ���������� ���� �浹 �޽ð� �ʿ��ϴ�.
	class PxMaterial* m_pPhysicsMaterial;

protected:
	//�⺻�� Trigger�� ������.
	virtual void CreateActor(bool bStatic);
	virtual void SetPhysicsTransform();


public:
	void SetPhysicsMaterial(float fStaticFriction, float fDynamicFriction, float fRestitution);
	class PxMaterial* GetPhysicsMaterial() const;
	void SetStaticFriction(float fStaticFriction);
	void SetDynamicFriction(float fDynamicFriction);
	void SetRestitution(float fRestitution);
	float GetStaticFriction() const;
	float GetDynamicFriction() const;
	float GetRestitution() const;

/***********************************/
protected:
	Vector3 m_vLocalPosition;	
public:
	void RemoveShapes();
	Vector3 GetCenter() const;
	Vector3 GetLocalPosition() const;

public:
	void SetLocalPosition(const Vector3& vLocalPosition);
	void SetLocalPosition(float x, float y, float z);

/***************Debug***************/
protected:
	class CShader*			m_pShader;
	class CMesh*				m_pMesh;
	class CSampler*			m_pSampler;
	ID3D11InputLayout*	m_pLayout;
	class CRenderState*	m_pRenderState[RS_END];
	Vector4						m_vColliderColor;

protected:
	void SetRenderState(const string & strKey);
	void SetSampler();

public:
	void SetDebugColor(const Vector4& vColor);
/***********************************/


public:
	bool Initialize() override;
	int Update(float fTime) override;
	int LateUpdate(float fTime) override;
	int OnCollisionEnter(CCollider* pThis, CCollider* pTarget, float fTime) override;
	int OnCollisionStay(CCollider* pThis, CCollider* pTarget, float fTime) override;
	int OnCollisionExit(CCollider* pThis, CCollider* pTarget, float fTime) override;
	void RenderDebug(float fTime) override;
};

SSS_END