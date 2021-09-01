#pragma once

#include "../Component.h"

SSS_BEGIN

class SSS_DLL CDistortion :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CDistortion();
	~CDistortion();

public:
	bool Initialize() override;
	int Prerender(class CMeshRenderer*	pRenderer) override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END
