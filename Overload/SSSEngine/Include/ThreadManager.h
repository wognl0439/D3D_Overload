#pragma once

#include "Engine.h"

SSS_BEGIN

class SSS_DLL CThreadManager
{
	DECLARE_SINGLE(CThreadManager)

private:
	unordered_map<string, class CThread*>	m_mapThread;

public:
	bool Initialize();
	class CThread* FindThread(const string& strKey);
	void RemoveThread(const string& strKey);
	void ClearThread();
	void AddThread(const string& strKey, class CThread* pThread);
	void AddThread(class CThread* pThread);
public:
	template <typename T>
	T* CreateThread(const string& strKey)
	{
		if (FindThread(strKey))
			return NULL;

		T* pThread = new T;
		
		pThread->Create(strKey);
		
		m_mapThread.insert(make_pair(strKey, pThread));

		return pThread;
	}

};

SSS_END
