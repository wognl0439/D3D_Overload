#include "Timer.h"
#include "Core.h"
#include "Input.h"

SSS_USING

CTimer::CTimer():
	m_fTimeScale(1.0f)
{
}


CTimer::~CTimer()
{
}

float CTimer::GetFixedDeltaTime() const
{
	return m_fDeltaTime;
}

float CTimer::GetTimeScale() const
{
	return m_fTimeScale;
}

float CTimer::GetDeltaTime() const
{
	return m_fDeltaTime * m_fTimeScale;
}

float CTimer::GetFPS() const
{
	return m_fFPS;
}

void CTimer::SetTimeScale(float fTimeScale)
{
	m_fTimeScale = fTimeScale;
}

bool CTimer::Initialize()
{
	QueryPerformanceFrequency(&m_tSecond);
	QueryPerformanceCounter(&m_tPrev);

	m_fDeltaTime = 0.f;
	m_fFPS = 0.f;
	m_fFPSTime = 0.f;
	m_iTick = 0;

	return true;
}

void CTimer::Update()
{
	LARGE_INTEGER	tTime;
	QueryPerformanceCounter(&tTime);

	m_fDeltaTime = (tTime.QuadPart - m_tPrev.QuadPart) / (float)m_tSecond.QuadPart;

	m_tPrev = tTime;

	++m_iTick;
	m_fFPSTime += m_fDeltaTime;

	if (m_iTick == 60)
	{
		m_fFPS = m_iTick / m_fFPSTime;
		m_fFPSTime = 0.f;
		m_iTick = 0;
	}

}
