#pragma once

#include "../Component.h"

SSS_BEGIN

class SSS_DLL CDecal :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CDecal();
	~CDecal();

public:
	bool Initialize() override;
};

SSS_END
