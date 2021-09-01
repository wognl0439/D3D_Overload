#pragma once
#include "Light.h"

SSS_BEGIN

class SSS_DLL CDirectionalLight :
	public CLight
{
	friend class CGameObject;

protected:
	CDirectionalLight();
	CDirectionalLight(const CDirectionalLight& light);
	virtual ~CDirectionalLight();

public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;
	int LateUpdate(float fTime) override;
	void Render(float fTime) override;
	CDirectionalLight* Clone()	const override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END
