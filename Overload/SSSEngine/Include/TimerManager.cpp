#include "TimerManager.h"
#include "Timer.h"

SSS_USING

DEFINITION_SINGLE(CTimerManager)

CTimerManager::CTimerManager()
{
}


CTimerManager::~CTimerManager()
{
	unordered_map<string, CTimer*>::iterator iter;
	unordered_map<string, CTimer*>::iterator iterEnd = m_mapTimer.end();
	for (iter = m_mapTimer.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}

	m_mapTimer.clear();
}

bool CTimerManager::Initialize()
{
	CreateTimer("MainTimer");

	return true;
}

CTimer * CTimerManager::CreateTimer(const string & strTag)
{
	CTimer* pTimer = FindTimer(strTag);

	if (pTimer)
		return pTimer;

	pTimer = new CTimer;
	pTimer->m_strTag = strTag;

	if (!pTimer->Initialize())
	{
		SAFE_DELETE(pTimer);
		return NULL;
	}

	m_mapTimer.insert(make_pair(strTag, pTimer));

	return pTimer;
}

CTimer * CTimerManager::FindTimer(const string & strTag)
{
	unordered_map<string, CTimer*>::iterator iter = m_mapTimer.find(strTag);
	if (iter == m_mapTimer.end())
	{
		return NULL;
	}

	return iter->second;
}
