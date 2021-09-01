#pragma once

#include "../Client.h"
#include "Component.h"
#include "Component/Effect.h"

SSS_USING

class CNPC_IDLE_AI :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CNPC_IDLE_AI();
	~CNPC_IDLE_AI();

private:
	NPC_STATE	m_eState;
	float			m_fActiveTime;
	float			m_fActiveMaxTime;
	float			m_fMoveSpeed;

	class CGameObject*	m_pPlayer;
	CEffect*	m_pEffect;
	bool		m_bAgro;

	float		m_fMoveRange;
	Vector3		m_vStandardPosition;

	bool		m_bDeadBody;

private:
	void RunningState(float fTime);

public:
	bool GetAgroState() const;
	void SetMoveDistance(Vector3 vPos, float fRange);
	void SetDeadBody(bool bSwitch);

public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;

	void OnMouseEnter(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;
	void OnMouseStay(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime) override;
	void OnMouseExit(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

