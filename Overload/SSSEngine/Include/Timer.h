#pragma once

#include "Engine.h"

SSS_BEGIN

class SSS_DLL CTimer
{
private:
	friend class CTimerManager;

private:
	CTimer();
	~CTimer();

private:
	string m_strTag;
	LARGE_INTEGER m_tSecond;
	LARGE_INTEGER m_tPrev;
	float	m_fDeltaTime;
	float m_fTimeScale;

	float	m_fFPS;
	float	m_fFPSTime;
	int		m_iTick;

public:
	float GetFixedDeltaTime() const;
	float GetTimeScale() const;
	float GetDeltaTime() const;
	float GetFPS() const;

	void SetTimeScale(float fTimeScale);

public:

	bool Initialize();
	void Update();


};

SSS_END
