#pragma once
#include "Scene.h"

SSS_USING

class CSampleScene :
	public CScene
{
private:
	friend class CSceneManager;
	friend class CScene;

public:
	CSampleScene();
	~CSampleScene();

public:
	bool Initialize() override;
};

