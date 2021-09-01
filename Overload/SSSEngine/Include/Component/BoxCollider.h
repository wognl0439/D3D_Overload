#pragma once
#include "../Collider.h"

SSS_BEGIN

//기본으로 OBB 충돌을 수행하도록 함
class SSS_DLL CBoxCollider :
	public CCollider
{
private:
	friend class CGameObject;
	
public:
	CBoxCollider();
	~CBoxCollider();
		
private:
	Vector3 m_vVolume;

	void CreateActor(bool bStatic) override;
	void SetPhysicsTransform() override;

public:
	void SetVolume(const Vector3& vScale);
	Vector3 GetVolume() const;

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
	
	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END