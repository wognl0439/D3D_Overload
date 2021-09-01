#pragma once
#include "Scene.h"

SSS_USING

class CAnimationTestbed :
	public CScene
{
public:
	CAnimationTestbed();
	~CAnimationTestbed();

public:
	bool Initialize() override;
};

