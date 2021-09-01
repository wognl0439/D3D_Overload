#pragma once
#include "Client.h"

LMNT_USING

class COption
{
	DECLARE_SINGLE(COption)
	

private:
	float m_fBGMVolume;
	float m_fFXVolume;
	float m_fENVVolume;


public:
	class CGameObject* m_pDebugText;
};

