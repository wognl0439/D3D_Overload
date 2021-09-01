#pragma once
#include "Scene.h"

SSS_USING

class CEditScene :
	public CScene
{
	friend class CSceneManager;
	friend class CScene;

public:
	CEditScene();
	~CEditScene();

public:
	bool Initialize() override;

};

