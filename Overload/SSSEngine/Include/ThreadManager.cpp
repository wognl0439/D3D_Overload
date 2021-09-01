#include "ThreadManager.h"
#include "Thread.h"

SSS_USING

DEFINITION_SINGLE(CThreadManager)

CThreadManager::CThreadManager()
{
}

CThreadManager::~CThreadManager()
{
	Safe_Delete_Map(m_mapThread);
}

bool CThreadManager::Initialize()
{
	return true;
}

CThread * CThreadManager::FindThread(const string & strKey)
{
	unordered_map<string, class CThread*>::iterator	iter = m_mapThread.find(strKey);

	if (iter == m_mapThread.end())
		return NULL;

	return iter->second;
}

void CThreadManager::RemoveThread(const string & strKey)
{
	unordered_map<string, class CThread*>::iterator iter = m_mapThread.find(strKey);
	
	if (iter != m_mapThread.end())
	{
		SAFE_DELETE(iter->second);
		m_mapThread.erase(iter);
	}
}

void CThreadManager::ClearThread()
{
	unordered_map<string, CThread*>::iterator iter ;
	unordered_map<string, CThread*>::iterator iterEnd = m_mapThread.end();
	
	for (iter = m_mapThread.begin(); iter != iterEnd;)
	{
		delete iter->second;
		iter = m_mapThread.erase(iter);
	}
}

void CThreadManager::AddThread(const string & strKey, CThread * pThread)
{
	if (!strKey.empty())
	{
		pThread->m_strKey = strKey;
		m_mapThread.insert(make_pair(strKey, pThread));
	}
	else
	{
		//이름이 비어있음.
		assert(false);
	}
}

void CThreadManager::AddThread(CThread * pThread)
{
	if (!pThread->m_strKey.empty())
	{
		m_mapThread.insert(make_pair(pThread->m_strKey, pThread));
	}
	else
	{
		//Thread의 이름이 비어있음.
		assert(false);
	}
}
