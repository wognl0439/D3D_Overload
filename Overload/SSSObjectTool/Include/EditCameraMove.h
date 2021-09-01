#pragma once
#include "Component.h"

SSS_USING

class CEditCameraMove :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CEditCameraMove();
	~CEditCameraMove();

private:
	float m_fScale;
	bool m_bPressed;
	bool m_bImageEdit;
	vector<class CGameObject*>	m_vEditRect;
	vector<Vector3>	m_vPressedRectPosition;
	Vector2 m_vCursorPressedPosition;
	Vector3 m_vPressedPosition;

	vector<class CGameObject*> m_vecTarget;
	vector<Vector3>	m_vTargetOriginScale;

public:
	void AddTarget(class CGameObject* pTarget);
	void EraseTarget(size_t index);

public:
	bool Initialize() override;
	int Update(float fTime) override;
};

