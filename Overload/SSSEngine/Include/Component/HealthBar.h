#pragma once
#include "../Component.h"


SSS_BEGIN

enum HEALTH_BAR_FADING_PHASE
{
	HB_FADE_NONE,
	HB_FADE_IN,
	HB_FADE_OUT
};

enum HEALTH_BAR_STATE
{
	HBS_STABLE,
	HBS_START_VISIBLE,
	HBS_STAY_VISIBLE,
	HBS_EXIT_VISIBLE
};

class SSS_DLL CHealthBar :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CHealthBar();
	~CHealthBar();

private:
	HEALTHBARCBUFFER m_tConstantBuffer;
	Vector3 m_vOffset;
	float m_fAlphaTimer;			// Fading Progress 산출 용 타이머
	float m_fDamagedTimer;		// 피격 직후 경과 타이머
	float m_fVisibleDistanceThreshold;
	float m_fFadingTimeThreshold;
	class CGameObject* m_pPlayerObject;
	class CGameObject* m_pBaseObject;
	
	bool m_bCurrentAggressive;
	bool m_bPreviousAggressive;
	bool m_bCurrentInsideScreen;
	bool m_bPreviousInsideScreen;
	bool m_bCurrentInsideRange;
	bool m_bPreviousInsideRange;
	HEALTH_BAR_FADING_PHASE m_eFadingPhase;
	HEALTH_BAR_STATE m_eState;
	
private:
	void ComputeProgressColor();

public:
	void SetBackgroundTexture(const string& strKey, const wchar_t* pFileName, const string& strPathKey = TEXTURE_PATH);
	void SetForegroundTexture(const string& strKey, const wchar_t* pFileName, const string& strPathKey = TEXTURE_PATH);
	//체력바를 표기할 최소 상대위치를 설정합니다.
	//-반환 : 없음
	//-비고
	//> 오버로딩 함수 있음
	void SetOffset(const Vector3 vOffset);
	void SetOffset(float fX, float fY, float fZ);
	
	//체력바를 표기할 최소 거리를 설정합니다.
	//-반환 : 없음
	//-비고
	//> fDistance는 절댓값으로 설정됩니다.
	void SetVisibleDistanceThreshold(float fDistance);

	//플레이어 오브젝트를 설정합니다.
	void SetPlayerObject(class CGameObject* pGameObject);

	//기준이 될 오브젝트를 설정합니다.
	void SetBaseObject(class CGameObject* pGameObject);

	//체력바의 정보를 업데이트 합니다.
	//-반환 : 없음
	//-비고
	//> 
	void UpdateHealthBarInfo(bool bIsAggressive, float fCurrentHP, float fMaxHP, float fDeltaTime);

public:
	bool Initialize() override;
	void Start() override;
	int LateUpdate(float fTime) override;
	int Prerender(class CMeshRenderer* pRenderer) override;
	

};

SSS_END

