#pragma once

#include "../Client.h"
#include "Component.h"

SSS_USING

enum MONSTER_TYPE
{
	MT_NONE,
	MT_RABBIT,
	MT_HAMSTER,
	MT_YETI,
	MT_SUCCUBUS,
	MT_ZOMBIE,
	MT_DRYAD,
	MT_GHOSTWARRIOR,
	MT_DARKKNIGHT,
	MT_HARPYQ
};


class CMonsterAI :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CMonsterAI();
	~CMonsterAI();

private:
	NPC_STATE	m_eState;
	MONSTER_TYPE	m_eMonsterType;
	MONSTER_STATE m_eMonsterBehaviourState;
	bool		m_bAggressive;

	class CGameObject* m_pPlayer;
	class CGameObject* m_pHealthBar;


	float			m_fActiveTime;
	float			m_fActiveMaxTime;
	float			m_fMoveSpeed;

	float		m_fHP;
	float		m_fHPMax;

	float		m_fCoolDownTime;
	float		m_fCurrentCoolDownTime;

	bool		m_bIsAttack;
	bool		m_bIsHited;
	bool		m_bSoundPlay;

private:
	void TransState(float fTime);
	void UpdateHPBar(float fDeltaTime);
	void SetMonsterType();

public:
	void SetIsHited(bool bHit);
	bool GetIsHited() const;
	void SetMonsterType(MONSTER_TYPE eMonsterType);
	MONSTER_TYPE GetMonsterType()	const;

public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;

	int OnCollisionEnter(class CCollider* pThis, class CCollider* pTarget, float fTime) override;
	int OnCollisionStay(class CCollider* pThis, class CCollider* pTarget, float fTime) override;
	int OnCollisionExit(class CCollider * pThis, class CCollider * pTarget, float fTime) override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

