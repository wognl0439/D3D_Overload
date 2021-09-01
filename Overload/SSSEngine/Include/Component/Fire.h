#pragma once

#include "../Component.h"

SSS_BEGIN

class SSS_DLL	CFire :
	public CComponent
{
private:
	friend	class CGameObject;

public:
	CFire();
	~CFire();

private:
	FIRECBUFFER		m_tCBuffer;
	Vector3		m_vScrollSpeed;
	float	m_fActiveTime;
	float	m_fActiveMaxTime;
	float	m_fFireSpeed;
	bool	m_bOnceDestroy;

public:
	void SetScale(const Vector3& vScale);
	void SetSpeed(const Vector3& vSpeed);

	void SetDistortion1(const Vector2& vDistord);
	void SetDistortion2(const Vector2& vDistord);
	void SetDistortion3(const Vector2& vDistord);

	void SetDistortionScale(float fScale);
	void SetBias(float fBias);
	void SetOffsetY(float fOffsetY);

	float GetDistortionScale() const;

	bool GetDestroyOption() const;
	void SetDestroyOption(bool bOption);
	float GetActiveMaxTime() const;
	void SetActiveMaxTime(float fActiveMaxTime);

public:
	bool Initialize() override;
	int LateUpdate(float fTime)		override;
	int Prerender(class CMeshRenderer*	pRenderer) override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END