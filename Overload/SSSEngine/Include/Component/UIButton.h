#pragma once
#include "UI.h"

SSS_BEGIN

class SSS_DLL CUIButton :
	public CUI
{
private:
	friend class CGameObject;
	friend class CUI;

private:
	CUIButton();
	CUIButton(const CUIButton& component);
	~CUIButton();

private:
	Vector4 m_vDiffuse;

private:
	bool m_bCheck;

private:
	void* m_pCallBack;

public:
	bool Initialize() override;

	void OnMouseEnter(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition);
	void OnMouseStay(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime);
	void OnMouseExit(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition);
};

SSS_END	
