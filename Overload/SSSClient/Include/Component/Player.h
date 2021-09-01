#pragma once
#include "Component.h"
#include "Engine.h"

SSS_USING


class CPlayer :
	public CComponent
{
private:
	friend class CGameObject;
	
public:
	CPlayer();
	~CPlayer();

private:
	unordered_map<string, ITEMINFO> m_mapEquipItemInfo;
	float m_fMoveSpeed;
	float m_fJumpHeight;
	float m_fDamage;
	float m_fDefence;
	float m_fHP;
	float m_fHPMax;
	float m_fMP;
	float m_fMPMax;
	float m_fSP;
	float m_fSPMax;
	float m_fRotateSpeed;

public:
	int GetItemIndex(const string& strItemName);
	void ChangeEquipment(const string& strOnEquipItemName);
	ITEMINFO GetEquipItemInfo(const string& strItemName);


public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime);

	int OnCollisionEnter(class CCollider* pThis, class CCollider* pTarget, float fTime) override;
	int OnCollisionStay(class CCollider* pThis, class CCollider* pTarget, float fTime) override;
	int OnCollisionExit(class CCollider* pThis, class CCollider* pTarget, float fTime) override;


public:
	bool Save(FILE* pFile);
	bool Load(FILE* pFile);
};

