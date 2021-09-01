#include "SoundManager.h"
#include "PathManager.h"

SSS_USING

DEFINITION_SINGLE(CSoundManager)

CSoundManager::CSoundManager() :
	m_pSystem(NULL)
{
	for (int i = 0; i < SOUND_END; ++i)
	{
		m_pChannelGroup[i] = NULL;
	}
}


CSoundManager::~CSoundManager()
{
	unordered_map<string, PSOUNDINFO>::iterator	iter;
	for (iter = m_mapSound.begin(); iter != m_mapSound.end(); ++iter)
	{
		iter->second->pSound->release();
		SAFE_DELETE(iter->second);
	}

	m_pSystem->close();
	m_pSystem->release();
}

ChannelGroup * CSoundManager::FindChannelGroup(SOUND_TYPE eType)
{
	return m_pChannelGroup[eType];
}

bool CSoundManager::CreateChannelGroup(SOUND_TYPE eGroupType, const string& strGroupName)
{
	if (!m_pChannelGroup[eGroupType])
	{
		if (m_pSystem->createChannelGroup(strGroupName.c_str(), &m_pChannelGroup[eGroupType]) != FMOD_OK)
			return false;		
	}
	else
	{
		return false;
	}

	return true;
}

bool CSoundManager::Initialize()
{
	System_Create(&m_pSystem);

	m_pSystem->init(32, FMOD_INIT_NORMAL, NULL);

	//CreateChannelGroup(SOUND_BGM, "BGM");
	//CreateChannelGroup(SOUND_SFX, "SFX");

	return true;
}

bool CSoundManager::LoadSound(SOUND_TYPE eType, const string & strKey, bool bLoop, const char * pFileName, const string & strPathKey)
{
	if (FindSound(strKey))
		return true;

	const char*	pPath = GET_SINGLE(CPathManager)->FindPathMultiByte(strPathKey);
	string	strFullPath;

	if (pPath)
		strFullPath = pPath;

	strFullPath += pFileName;

	FMOD_MODE	eMode = FMOD_LOOP_NORMAL;

	if (!bLoop)
		eMode = FMOD_DEFAULT;

	PSOUNDINFO	pSoundInfo = new SOUNDINFO;

	pSoundInfo->eType = eType;
	pSoundInfo->bLoop = bLoop;
	m_pSystem->createSound(strFullPath.c_str(), eMode, NULL, &pSoundInfo->pSound);

	m_mapSound.insert(make_pair(strKey, pSoundInfo));

	return true;
}

Channel * CSoundManager::Play(const string & strKey)
{
	PSOUNDINFO	pSound = FindSound(strKey);

	if (!pSound)
		return NULL;

	m_pSystem->update();

	SOUND_TYPE	eType = pSound->eType;

	m_pSystem->playSound(pSound->pSound, NULL, false, &m_pChannel[eType]);

	return m_pChannel[eType];
}

void CSoundManager::Pause()
{
	ChannelGroup*	pGroup;
	m_pSystem->getMasterChannelGroup(&pGroup);

	pGroup->setPaused(true);
}

void CSoundManager::Resume()
{
	ChannelGroup*	pGroup;
	m_pSystem->getMasterChannelGroup(&pGroup);

	pGroup->setPaused(false);
}

void CSoundManager::Stop(SOUND_TYPE eType)
{
	m_pChannel[eType]->stop();
}

void CSoundManager::Volume(SOUND_TYPE eType, float fVolume)
{
	m_pChannel[eType]->setVolume(fVolume);
}

void CSoundManager::Volume(float fVolume)
{
	ChannelGroup*	pGroup;
	m_pSystem->getMasterChannelGroup(&pGroup);

	pGroup->setVolume(fVolume);
}

PSOUNDINFO CSoundManager::FindSound(const string & strKey)
{
	unordered_map<string, PSOUNDINFO>::iterator	iter = m_mapSound.find(strKey);

	if (iter == m_mapSound.end())
		return NULL;

	return iter->second;
}
