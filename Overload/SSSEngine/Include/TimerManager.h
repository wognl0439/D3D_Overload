#pragma once

#include "Engine.h"

SSS_BEGIN

class SSS_DLL CTimerManager
{
	DECLARE_SINGLE(CTimerManager)

private:
	unordered_map<string, class CTimer*> m_mapTimer;

public:
	bool Initialize();
	class CTimer* CreateTimer(const string& strTag);
	class CTimer* FindTimer(const string& strTag);
};

SSS_END
