#pragma once
#include "Component.h"

SSS_USING

class CTestComponent :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CTestComponent();
	~CTestComponent();

	bool m_bClicked;


public:
	bool Initialize() override;
	int Update(float fTime) override;
};

