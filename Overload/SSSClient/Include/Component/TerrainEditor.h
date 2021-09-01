#pragma once
#include "Component.h"
#include "Component/Terrain.h"

SSS_USING

class CTerrainEditor :
	public CComponent
{
private:
	friend class CGameObject;
public:
	CTerrainEditor();
	~CTerrainEditor();

private:
	CTerrain* m_pTerrainComponent;
	bool			m_bBrushOn;
	bool			m_bNone;
	bool			m_bClick;
	float			m_fBrushDir;
	float			m_fBrushRadius;
	float			m_fBrushPower;
	float			m_fCurrentSplate;
public:
	bool Initialize() override;
	void Start() override;

	void OnMouseStay(CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime) override;
	void OnMouseDrag(CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime) override;

	void SetBrushOn(bool Brush,bool None = false);
	void SetCurrentSplate(int Count);
	int GetCurrentSplate() const;
	float GetBrushRadius() const;
	void SetBrushRadius(float Radius);
	float GetBrushPower() const;
	void SetBrushPower(float Brush);
};

