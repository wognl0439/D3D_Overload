#pragma once

#include "Engine.h"
#include "fmod.hpp"


using namespace FMOD;

#ifdef _WIN64
#pragma comment(lib, "fmod64_vc")
#else
#pragma comment(lib, "fmod_vc")
#endif

SSS_BEGIN

enum SSS_DLL SOUND_TYPE
{
	SOUND_BGM,
	SOUND_ENV,
	SOUND_SFX,
	SOUND_DIALOGUE,
	SOUND_END
};

typedef struct SSS_DLL _tagSoundInfo
{
	SOUND_TYPE eType;
	Sound*	pSound;
	bool	bLoop;
}SOUNDINFO, *PSOUNDINFO;

class SSS_DLL CSoundManager
{
private:
	ChannelGroup* m_pChannelGroup[SOUND_END];
	System*		m_pSystem;
	Channel* m_pChannel[32];
	unordered_map<string, PSOUNDINFO>	m_mapSound;

public:
	ChannelGroup* FindChannelGroup(SOUND_TYPE eType);
	bool CreateChannelGroup(SOUND_TYPE eGroupType, const string& strGroupName);
	bool Initialize();
	bool LoadSound(SOUND_TYPE eType, const string& strKey, bool bLoop, const char* pFileName, const string& strPathKey = SOUND_PATH);
	Channel* Play(const string& strKey);
	void Pause();
	void Resume();
	void Stop(SOUND_TYPE eType);
	void Volume(SOUND_TYPE eType, float fVolume);
	void Volume(float fVolume);

private:
	PSOUNDINFO FindSound(const string& strKey);

	DECLARE_SINGLE(CSoundManager)
};

SSS_END
