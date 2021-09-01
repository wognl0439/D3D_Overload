#pragma once

#include "Component.h"
#include "../Client.h"

SSS_USING

typedef struct _tagBossHealthBar
{
	Vector3 vColor;
	float	fProgress;
	int		iLineCount;
	Vector3 vEmpty;

}BOSSHEALTHBAR, *PBOSSHEALTHBAR;

class CBossHealthBar :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CBossHealthBar();
	~CBossHealthBar();

private:
	BOSSHEALTHBAR	m_tCBuffer;
	int				m_iMaxLineCount;

public:
	bool UpdateHealthPoint(float fHealthPoint, float fMaxHealthPoint);
	void SetLineCount(int iCount);

public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;
	int Prerender(class CMeshRenderer* pRenderer) override;
};

