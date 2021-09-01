#pragma once
#include "Light.h"

SSS_BEGIN

class SSS_DLL CSpotLight :
	public CLight
{
	friend class CGameObject;

protected:
	CSpotLight();
	CSpotLight(const CSpotLight& light);
	virtual ~CSpotLight();

public:
	void SetRange(float fRange);
	void SetInAngle(float fAngle);
	void SetOutAngle(float fAngle);
	float GetRange();
	float GetInAngle();
	float GetOutAngle();

public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;
	int LateUpdate(float fTime) override;
	void Render(float fTime) override;
	CSpotLight* Clone()	const override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END
