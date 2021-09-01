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
	float m_fAlphaTimer;			// Fading Progress ���� �� Ÿ�̸�
	float m_fDamagedTimer;		// �ǰ� ���� ��� Ÿ�̸�
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
	//ü�¹ٸ� ǥ���� �ּ� �����ġ�� �����մϴ�.
	//-��ȯ : ����
	//-���
	//> �����ε� �Լ� ����
	void SetOffset(const Vector3 vOffset);
	void SetOffset(float fX, float fY, float fZ);
	
	//ü�¹ٸ� ǥ���� �ּ� �Ÿ��� �����մϴ�.
	//-��ȯ : ����
	//-���
	//> fDistance�� �������� �����˴ϴ�.
	void SetVisibleDistanceThreshold(float fDistance);

	//�÷��̾� ������Ʈ�� �����մϴ�.
	void SetPlayerObject(class CGameObject* pGameObject);

	//������ �� ������Ʈ�� �����մϴ�.
	void SetBaseObject(class CGameObject* pGameObject);

	//ü�¹��� ������ ������Ʈ �մϴ�.
	//-��ȯ : ����
	//-���
	//> 
	void UpdateHealthBarInfo(bool bIsAggressive, float fCurrentHP, float fMaxHP, float fDeltaTime);

public:
	bool Initialize() override;
	void Start() override;
	int LateUpdate(float fTime) override;
	int Prerender(class CMeshRenderer* pRenderer) override;
	

};

SSS_END

