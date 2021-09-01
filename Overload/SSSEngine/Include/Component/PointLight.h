#pragma once
#include "Light.h"

SSS_BEGIN

class SSS_DLL CPointLight :
	public CLight
{
	friend class CGameObject;

protected:
	CPointLight();
	CPointLight(const CPointLight& light);
	virtual ~CPointLight();

public:
	void SetRange(float fRange);
	float GetRange();

public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;
	int LateUpdate(float fTime) override;
	void Render(float fTime) override;
	CPointLight* Clone()	const  override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END
