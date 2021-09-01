#pragma once
#include "Component.h"
#include "Engine.h"
#include "GameObject.h"
#include "../Client.h"

SSS_USING

class CEffectController :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CEffectController();
	~CEffectController();

private:
	float	m_fLifeTime;
	bool	m_bUseScaling;
	float	m_fScalingTimeThreshold;
	bool	m_bRotate;
	float	m_fRotatePerSecond;
	Vector3	m_vMinSize;
	Vector3 m_vMaxSize;
	float   m_fProgress;
	float	m_fTimer;
	bool	m_bUseAlpha;
	float	m_fAlphaTime;
	float	m_fAlphaBlink;
	float	m_fMinAlpha;
	float	m_fMaxAlpha;
	bool	m_bBlink;
	bool	m_bMove;
	float	m_fAcceleration;
	Vector3 m_vMoveDir;
	Vector3 m_vArrivePosition;
	bool	m_bUseArrive;
	bool	m_bMeteo;
	bool	m_bReScale;
	bool	m_bUseLimLight;
	float	m_fLimLightTime;
	Vector3 m_vLimLightColor;
	bool	m_bUseDestroy;
	bool	m_bUseBurnEffect;
	bool	m_bDestroy;
	float	m_fBurnTime;
	bool	m_bUseLight;
	bool	m_bUseSkill;
	bool	m_bUseMeteoFire;
	bool	m_bUseNoramlScale;
	float	m_fMinNormalScale;
	float	m_fMaxNormalScale;
	float	m_fNormalScalingTimeThreshold;

public:
	bool Initialize() override;
	int Update(float fTime) override;
	void SetScaleInfo(float fScalingTime, Vector3 vMinSize, Vector3 vMaxSize, bool bStay = false, bool bReScale = false);
	void SetAlphaInfo(float fAlphaTime, float fMinAlpha, float fMaxAlpha, bool bBlink = false);
	void SetRotateInfo(float fRotatePerSecond);
	void SetMoveInfo(Vector3 vStartPosition, Vector3 vEndPosition, float fDefaultSpeed, bool m_bUseArrive = false);
	void CheckArrive();
	void SetUseMeteo();
	void SetLimLightInfo(float fLimLightTime, Vector3 vLimLightColor, float fLimLightPower, bool bUseDestroy = true);
	//반드시 설정 해야 함.
	void SetLifeTime(float fLifeTime, bool bDestroy = true);
	void UseBurnEffect(float fBurnTime);
	void SetSkillCircle(bool bUseSkill = false);
	void SetLightRedution();
	void UseDamageEffect();
	void UseMeteoFire(Vector3 vArrivePosition, bool bUseArrive = true);
	void SetNormalScaleInfo(float fScalingTime, float fMinScale, float fMaxScale);
	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

