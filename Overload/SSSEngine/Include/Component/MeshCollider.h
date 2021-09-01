#pragma once
#include "../Collider.h"

SSS_BEGIN	

class SSS_DLL CMeshCollider :
	public CCollider
{
public:
	CMeshCollider();
	~CMeshCollider();
	
private:
	Vector3 m_vVolume;

	void CreateActor(bool bStatic) override;
	void SetPhysicsTransform() override;
public:
	
	void SetVolume(const Vector3 vScale);
	//반드시 0번 컨테이너의 0번 서브셋만을 생성한다.
	bool BakeTriangleMesh(class CMesh* pMesh = NULL);
	bool BakeConvexMesh(class CMesh* pMesh = NULL);
	bool Initialize() override;
	void Start() override;
	void RenderDebug(float fDeltaTime) override;

	void OnMouseEnter(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;
	void OnMouseExit(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;

	bool Save(FILE* pFile);
	bool Load(FILE* pFile);
};

SSS_END