#pragma once

#include "../Client.h"
#include "Component.h"
#include "AnimationStateController.h"

SSS_USING

class CBoneDragonAI :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CBoneDragonAI();
	~CBoneDragonAI();

private:
	CAnimationStateController*	m_pStateController;
	class CGameObject* m_pPlayer;
	float			m_fActiveTime;
	float			m_fActiveMaxTime;
	float			m_fMoveSpeed;

	float			m_fHP;
	float			m_fHPMax;

	bool			m_bSpawn;
	bool			m_bSpawnStart;
	bool			m_bAttack;
	bool			m_bSoundPlay;
	bool			m_bEffect;

	vector<class CGameObject*>	m_vecFireEffects;
	vector<string>	m_vecFireEffectString;

	class CGameObject* m_pHealthBar;

	bool		m_bRage;
	bool		m_bRageOn;

private:
	void TransState(float fTime);
	void SetRage(float fTime);

public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;
	int LateUpdate(float fTime)	override;

	int OnCollisionEnter(class CCollider* pThis, class CCollider* pTarget, float fTime) override;
	int OnCollisionStay(class CCollider* pThis, class CCollider* pTarget, float fTime) override;
	int OnCollisionExit(class CCollider * pThis, class CCollider * pTarget, float fTime) override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

