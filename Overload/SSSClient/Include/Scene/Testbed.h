#pragma once
#include "Scene.h"

SSS_USING

class CTestbed :
	public CScene
{
public:
	CTestbed();
	~CTestbed();


public:
	bool Initialize() override;
};

