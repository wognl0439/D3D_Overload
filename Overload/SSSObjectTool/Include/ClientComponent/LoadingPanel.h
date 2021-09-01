#pragma once
#include "Component.h"

SSS_USING

class CLoadingPanel :
	public CComponent
{
	friend class CGameObject;

private:
	LOADINGCBUFFER m_tLoadingConstantBuffer;

public:
	void SetLoadingProgress(float fProgress);

public:
	CLoadingPanel();
	~CLoadingPanel();

	bool Initialize() override;
	int Prerender(class CMeshRenderer* pRenderer) override;

	bool Save(FILE* pFile);
	bool Load(FILE* pFile);
};

