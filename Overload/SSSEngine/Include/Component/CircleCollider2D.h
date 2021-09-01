#pragma once
#include "../Collider.h"


SSS_BEGIN

class SSS_DLL CCircleCollider2D :
	public CCollider
{
private:
	friend class CGameObject;

private:
	CCircleCollider2D();
	CCircleCollider2D(const CCircleCollider2D& component);
	~CCircleCollider2D();

private:
	float m_fRadius;
	Vector3 m_vScale;
	Vector3 m_vPivot;

public:
	float GetRadius() const;
	void SetRadius(float fRadius);
	
public:
	int LateUpdate(float fTime) override;
	void RenderDebug(float fDeltaTime) override;
	CCircleCollider2D* Clone() const override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END
