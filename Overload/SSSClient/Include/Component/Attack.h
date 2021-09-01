#pragma once

#include "Component.h"
#include "../Client.h"

SSS_USING

class CAttack :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CAttack();
	~CAttack();

private:
	int		m_iValue;
	ATTACK_ATTRIBUTE	m_eAttribute;
	bool	m_bIsHited;
	float	m_fActiveTime;
	float	m_fActiveMaxTime;

	float	m_fDamagePoint;
	bool	m_bDamageSound;

public:
	void SetValue(int iValue);
	int	GetValue() const;
	void SetAttribute(ATTACK_ATTRIBUTE eAttribute);
	ATTACK_ATTRIBUTE GetAttribute() const;
	void SetActiveMaxTime(float fTime);
	void SetDamagePoint(float fDMG);
	float GetDamagePoint() const;

public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;

	int OnCollisionEnter(class CCollider* pThis, class CCollider* pTarget, float fTime) override;
	int OnCollisionStay(class CCollider* pThis, class CCollider* pTarget, float fTime) override;
	int OnCollisionExit(class CCollider * pThis, class CCollider * pTarget, float fTime) override;
};

