#pragma once

#include "../Collider.h"

SSS_BEGIN

//AABB�� OBB�� ���Խ�ų �� �ֱ� ������, Rect���� �ϳ��� ����ϵ��� �Ѵ�.

class SSS_DLL CRectCollider2D :
	public CCollider
{
private:
	friend class CGameObject;
	
private:
	CRectCollider2D();
	CRectCollider2D(const CRectCollider2D& component);
	~CRectCollider2D();

private:
	Vector3 m_vScale;
	Vector3 m_vPivot;
public:
	Vector3 GetScale() const;
	Vector3 GetPivot() const;
	void SetScale(const Vector3 vScale);
	void SetPivot(const Vector3 vPivot);

public:
	bool VoxelCollisionCheck(const Vector2& LT, const Vector2& RB);
	bool CollisionCheckWithRectCollider(CCollider* pTarget);
	bool CollisionCheckWithRectCollider(CCollider* pTarget, bool bIsAABB);
	bool CollisionCheckWithCircleCollider(CCollider* pTarget);


public:
	int LateUpdate(float fTime) override;
	void RenderDebug(float fDeltaTime) override;
	CRectCollider2D* Clone() const override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END
