#pragma once
#include "../Client.h"
#include "Component.h"
#include "AnimationStateController.h"

SSS_USING

typedef struct _tagPlayerEquipList
{
	int				iHairCount;
	vector<string>	vecHair;
	int				iUPPERCount;
	vector<string>	vecUPPER;
	int				iSHOULDERCount;
	vector<string>	vecSHOULDER;
	int				iLOWERCount;
	vector<string>	vecLOWER;
	int				iHELMETCount;
	vector<string>	vecHELMET;
	int				iGLOVESCount;
	vector<string>	vecGLOVES;
	int				iBOOTSCount;
	vector<string>	vecBOOTS;
	int				iBELTCount;
	vector<string>	vecBELT;

	_tagPlayerEquipList() :
		iHairCount(0),
		iUPPERCount(0),
		iSHOULDERCount(0),
		iLOWERCount(0),
		iHELMETCount(0),
		iGLOVESCount(0),
		iBOOTSCount(0),
		iBELTCount(0)
	{

	}
}PLAYEREQUIPLIST;

class CPlayerController :
	public CComponent
{
private:
	friend class CGameObject;


private:
	CPlayerController();
	~CPlayerController();
	
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

	bool m_bPressed;
	bool	m_bSoundPlay;
	bool	m_bSkillEffect;
	bool	m_bUseMeteo;
	bool	m_bClickGround;
	Vector3 m_vCursorClickPosition;
	Vector2 m_vPrevCursorPosition;

	CAnimationStateController*	m_pStateController;
	PLAYER_STANDARD_STATE	m_eStandardState;
	PLAYER_BATTLE_STATE		m_eBattleState;
	PLAYER_DAMAGED_STATE	m_eDMGState;
	float	m_fActiveTime;
	float	m_fActiveMaxTime;
	// Trail 객체 0, 2 는 Trail 1, 3 는 TrailDistortion
	class CGameObject*		m_pAttackTrail[4];
	// Attack 공격 단계 설정용	
	int		m_iAttackPhase;
	// 일부 모션 카메라위치 잡기용
	class CGameObject*		m_pDummyForCameraWork;
	// Point Light
	class CGameObject*		m_pPlayerPointLight;
	class CGameObject*		m_pSkillCircle;

	// Idle 상태일때 true
	bool		m_bOnIdle;
	// 장비 갈아입히기용
	PLAYEREQUIPLIST			m_eEquipList;

private:
	void TransState(float fTime);
	void PlayerActiveAttackSphere(float fRadius, ATTACK_ATTRIBUTE eAttribute, float fDamage, float fMaxActiveTime, Vector3 vPosition);

public:
	bool IsIdleState() const;
	int GetItemIndex(const string& strItemName);
	void ChangeEquipment(const string& strOnEquipItemName);
	ITEMINFO GetEquipItemInfo(const string& strItemName);

public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;

	int OnCollisionEnter(class CCollider* pThis, class CCollider* pTarget, float fTime) override;
	int OnCollisionExit(class CCollider * pThis, class CCollider * pTarget, float fTime) override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};
