#pragma once
#include "../Collider.h"

SSS_BEGIN

class SSS_DLL CSphereCollider :
	public CCollider
{
private:
	friend class CGameObject;
private:
	CSphereCollider();
	CSphereCollider(const CSphereCollider& component);
	~CSphereCollider();


private:	
	float m_fRadius;

	void CreateActor(bool bStatic) override;
	void SetPhysicsTransform() override;

public:
	float GetRadius() const;
	void SetRadius(float fRadius);
		
public:
	void Start() override;
	bool Initialize() override;
	int LateUpdate(float fTime) override;
	void RenderDebug(float fDeltaTime) override;
	int OnCollisionEnter(class CCollider* pThis, class CCollider* pTarget, float fTime)override;
	int OnCollisionStay(class CCollider* pThis, class CCollider* pTarget, float fTime)override;
	int OnCollisionExit(class CCollider* pThis, class CCollider* pTarget, float fTime)override;

	void OnMouseEnter(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;
	void OnMouseExit(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;
	CSphereCollider* Clone() const override;
	
	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};
SSS_END