#pragma once
#include "../Client.h"
#include "Component.h"
#include "GameSystem.h"

SSS_USING

class CPortal :
	public CComponent
{
private:
	friend class CGameObject;

private:
	POTAL_LOAD_SCENE	m_eSceneState;

public:
	void SetSceneType(POTAL_LOAD_SCENE PLS);
	POTAL_LOAD_SCENE GetScene();

public:
	CPortal();
	~CPortal();

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

