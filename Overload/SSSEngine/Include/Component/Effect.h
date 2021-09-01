#pragma once

#include "../Component.h"

SSS_BEGIN

class SSS_DLL CEffect :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CEffect();
	~CEffect();

private:
	EFFECT m_tEffectCBuffer;
	float m_fTimer;
	float m_fTimeLimit;
	float m_fProgress;
	bool m_bBackward;

public:
	void ResetTimer();
	void SetForward();
	void SetBackward();
	void SetFlags(unsigned int iFlags);
	uint32_t GetFlags() const;

	// Burn Effect
	void SetLimitTime(float fTime);
	void SetBorderColor(const Vector4& vColor);
	void SetBorderRangePercentage(float fPercentage);
	void SetTexture(const string & strKey, const wchar_t * pFileName);
	float GetProgress() const;
	float GetLimitTime() const;
	Vector4 GetBorderColor() const;
	float GetBorderRangePercentage() const;

	// LimLight Effect
	void SetLimColor(Vector3& vColor);
	void SetLimPower(float fPower);
	Vector3 GetLimColor() const;
	float GetLimPower() const;

public:
	bool Initialize() override;
	int Update(float fTime) override;
	int Prerender(class CMeshRenderer*	pRenderer) override;
	int Postrender(class CMeshRenderer* pRenderer) override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END
