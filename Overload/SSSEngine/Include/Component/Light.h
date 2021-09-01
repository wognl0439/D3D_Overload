#pragma once
#include "../Component.h"

SSS_BEGIN

class SSS_DLL CLight :
	public CComponent
{
protected:
	CLight();
	CLight(const CLight& light);
	virtual ~CLight() = 0;

protected:
	LIGHTCBUFFER	m_tLightInfo;

public:
	LIGHT_TYPE GetLightType() const;
	void SetLightColor(const Vector4& vDif, const Vector4& vAmb, const Vector4& vSpc);

	const Vector4& GetDiffuseColor()	const;
	const Vector4& GetAmbientColor()	const;
	const Vector4& GetSpecularColor()	const;

public:
	bool Initialize() override;
	void Start() = 0;
	int Update(float fTime) = 0;
	int LateUpdate(float fTime) override;
	void Render(float fTime) override;
	CLight* Clone()	const = 0;

	bool Save(FILE* pFile) override = 0;
	bool Load(FILE* pFile) override = 0;

public:
	void SetLight();
};

SSS_END
