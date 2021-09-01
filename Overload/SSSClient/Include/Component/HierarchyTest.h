#pragma once
#include "Component.h"

SSS_USING

class CHierarchyTest :
	public CComponent
{
	friend class CGameObject;

public:
	CHierarchyTest();
	~CHierarchyTest();

public:
	bool Initialize() override;
	int Update(float fTime) override;
};

