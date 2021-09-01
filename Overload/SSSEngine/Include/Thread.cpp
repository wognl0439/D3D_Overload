#include "Thread.h"
#include "ThreadManager.h"
SSS_USING


CThread::CThread():
	m_pThread(NULL),
	m_pFunction(NULL),
	m_bJoin(false)
{
}


CThread::~CThread()
{
	SAFE_DELETE(m_pThread);
}

void CThread::SetFunction(void(*pFunc)())
{
	m_pFunction = pFunc;
}

void CThread::Create(const string& strKey, void(*pFunc)(), bool bWaitForEnd)
{
	m_strKey = strKey;
	m_bJoin = bWaitForEnd;
	m_pFunction = pFunc;

	GET_SINGLE(CThreadManager)->AddThread(this);
}

void CThread::Start()
{
	if (m_pFunction)
	{
		//기본적으로 메인 스레드가 기다리지 않도록 한다.
		m_pThread = new thread(m_pFunction);
		if (m_bJoin)
		{
			m_pThread->join();
		}
		else
		{
			m_pThread->detach();		
		}
	}
}

bool CThread::WaitForEndOfThisThread()
{
	if (m_pThread)
	{
		if (m_pThread->joinable())
		{
			m_pThread->join();
			return true;
		}
	}
	return false;
}

