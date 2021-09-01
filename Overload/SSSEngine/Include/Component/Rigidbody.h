#pragma once
#include "../Component.h"
#include "../CollisionManager.h"

SSS_BEGIN

class SSS_DLL CRigidbody :
	public CComponent
{
	friend class CCollisionManager;
	
public:
	CRigidbody();
	~CRigidbody();

private:
	class PxRigidActor* m_pRigidbodyActor;

private:
	bool m_bIsKinematic;
	bool m_bUseGravity;
	float m_fMass;
	float m_fDrag;
	float m_fAngularDrag;


public:
	void SetKinematic(bool bIsKinematic);
	void SetUseGravity(bool bUse);
	void SetMass(float fMass);
	void SetDrag(float fDrag);
	void SetAngularDrag(float fAngularDrag);
	void ClearForce();

	bool IsKinematic() const;
	bool UseGravity() const;
	float GetMass() const;
	float GetDrag() const;
	float GetAngularDrag() const;

public:
	void AddForce(const Vector3& vForce);

public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;
	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END