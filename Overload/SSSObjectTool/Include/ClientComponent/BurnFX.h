#pragma once
#include "Component.h"
#include "Texture.h"

SSS_USING

class CBurnFX :
	public CComponent
{
private:
	friend class CGameObject;
public:
	CBurnFX();
	~CBurnFX();

private:
	FXBURN m_tRendererCBuffer;
	float m_fTimer;
	float m_fTimeLimit;
	float m_fProgress;
	

public:
	void SetBorderColor(const Vector4& vColor);
	void SetBorderRangePercentage(float fPercentage);
	void SetTexture(const string & strKey, const wchar_t * pFileName);
	bool Initialize() override;
	int Update(float fTime) override;
	void Render(float fTime) override;

	int Prerender(class CMeshRenderer* pRenderer) override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

