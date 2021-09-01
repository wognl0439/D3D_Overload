#pragma once
#include "../Component.h"

SSS_BEGIN

class SSS_DLL CBillboard :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CBillboard();
	~CBillboard();


private:
	BILLBOARDCBUFFER m_tCBuffer;
	bool	m_bRotate;

public:
	bool Initialize() override;
	void SetRotate(bool bRotate);
	int Prerender(class CMeshRenderer* pRenderer) override;
	
	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END
