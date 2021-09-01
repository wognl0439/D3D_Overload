#pragma once
#include "Component.h"

SSS_USING

class CTestCameraMove :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CTestCameraMove();
	~CTestCameraMove();

public:
	bool Initialize() override;
	int Update(float fTime) override;
};

