#pragma once
#include "../Component.h"

SSS_BEGIN

class SSS_DLL	CWater :
	public CComponent
{
private:
	friend	class CGameObejct;

public:
	CWater();
	~CWater();

private:
	WATERCBUFFER	m_tCBuffer;
	
	float	m_fCurrentSpeed;
	float	m_fFlowingSpeed;

public:
	bool Initialize() override;
	int LateUpdate(float fTime)		override;
	int Prerender(class CMeshRenderer*	pRenderer) override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END