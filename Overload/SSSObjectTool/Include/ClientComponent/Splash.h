#pragma once
#include "Component.h"

SSS_USING

enum FADING_STATE
{
	FS_NONE,
	FS_FADE_IN,
	FS_FADE_STAY,
	FS_FADE_OUT
};

enum SPLASH_STATE
{
	SPLASH_NONE			= 0x0,
	SPLASH_ENTER			= 0x1 << 0,
	SPLASH_EXIT				= 0x1 << 1,
	SPLASH_TITLE			= 0x1 << 2,
	SPLASH_DESTROY		= 0x1 << 3
};

enum SPLASH_INNER_PHASE
{
	SIP_NIL,
	SIP_000,
	SIP_001,
	SIP_002,
	SIP_003,
	SIP_004,
	SIP_005,
	SIP_006,
	SIP_007,
	SIP_008,
	SIP_009,
	SIP_010,
	SIP_011,
	SIP_012,
	SIP_013,
	SIP_014,
};

class CSplash :
	public CComponent
{
	friend class CGameObject;

public:
	CSplash();
	~CSplash();

private:
	class CGameObject* m_pBackgroundPanel;
	class CGameObject* m_pForegroundPanel;
	
	SPLASH_INNER_PHASE m_ePhase;
	FADING_STATE m_eFadingState;
	SPLASH_STATE m_eSplashState;
	CScene* m_pLoadedScene;
	wstring m_strPath;
	string m_strPathKey;
	float m_fFadingTime;
	float m_fTimer;
	bool	m_bSoundPlay;

public:
	//SPLASH_STATE а╤гу
	void SetSplashState(SPLASH_STATE eState);
	void SetPath(const wstring& strPath);
	void SetPathKey(const string& strPathKey);
	void SetFadingTime(float fTime);
	const wstring& GetPath() const;
	const string& GetPathKey() const;
	float GetFadingTime() const;

public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;

public:
	bool Save(FILE* pFile);
	bool Load(FILE* pFile);
};

