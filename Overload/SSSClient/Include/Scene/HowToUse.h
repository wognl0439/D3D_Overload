#pragma once
#include "Scene.h"

SSS_USING

class CHowToUse :
	public CScene
{
	friend class CSceneManager;
	friend class CScene;

public:
	CHowToUse();
	~CHowToUse();

public:
	bool Initialize() override;
};

