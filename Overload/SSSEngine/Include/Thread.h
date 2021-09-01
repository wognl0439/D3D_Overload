#pragma once

#include "Engine.h"

SSS_BEGIN

class SSS_DLL CThread
{
	friend class CThreadManager;

protected:
	CThread();

public:
	virtual ~CThread() = 0;

protected:
	string		m_strKey;
	thread*		m_pThread;
	bool			m_bJoin;
	function<void(void)>			m_pFunction;

public:
	void SetFunction(void(*pFunc)());
	void Create(const string& strKey, void(*pFunc)() = NULL, bool bWaitForEnd = false);
	void Start();
	bool WaitForEndOfThisThread();

};

SSS_END
