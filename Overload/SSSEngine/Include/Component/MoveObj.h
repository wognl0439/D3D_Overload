#pragma once
#include "../Component.h"

SSS_BEGIN

class SSS_DLL CMoveObj : public CComponent
{
public:
	CMoveObj();
	~CMoveObj();

private:
	bool m_bMove;

public:
	bool Initialize();
	int Update(float fTime);
	void OnMouseStay(class CCollider* pTarget, const Vector2& mousePosition, const Vector3& vIntersectedPosition, float fTime);
	void OnMouseExit(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition);
	void OnMouseDrag(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime);
};

SSS_END