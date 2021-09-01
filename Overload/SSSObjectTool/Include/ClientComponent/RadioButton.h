#pragma once
#include "Component.h"

SSS_USING

class CRadioButton :
	public CComponent
{
private:
	friend class CGameObject;
public:
	CRadioButton();
	~CRadioButton();

private:
	bool	m_bCheck;

public:
	bool Initialize() override;
	void Start() override;

	void OnMouseStay(CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime) override;

public:
	bool GetCheck() const;
	void SetCheck(const bool check);
};

