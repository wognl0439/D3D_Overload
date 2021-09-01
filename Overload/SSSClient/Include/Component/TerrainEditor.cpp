#include "TerrainEditor.h"
#include "Component/MeshRenderer.h"
#include "Transform.h"
#include "Input.h"

CTerrainEditor::CTerrainEditor() :
	m_pTerrainComponent(NULL),
	m_bBrushOn(false),
	m_bNone(true),
	m_fBrushDir(1),
	m_fCurrentSplate(0),
	m_fBrushRadius(50.f),
	m_fBrushPower(10.f),
	m_bClick(false)
{
}


CTerrainEditor::~CTerrainEditor()
{
	if (m_pTerrainComponent)
		SAFE_RELEASE(m_pTerrainComponent);
}

bool CTerrainEditor::Initialize()
{
	m_pTerrainComponent = GetComponent<CTerrain>();
	
	return true;
}

void CTerrainEditor::Start()
{
	if (!m_pTerrainComponent)
	{
		m_pTerrainComponent = GetComponent<CTerrain>();
	}
}


void CTerrainEditor::OnMouseStay(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
	if (GETKEYDOWN(KEY_LEFT_SHIFT))
	{
		m_fBrushDir = -1;
	}

	if (GETKEYUP(KEY_LEFT_SHIFT))
	{
		m_fBrushDir = 1;
	}

	if (GETKEYDOWN(MOUSE_LEFT))
	{
		m_bClick = true;
	}
	if (GETKEYUP(MOUSE_LEFT))
	{
		m_bClick = false;
	}

	Vector2 vUVPosition = m_pTerrainComponent->GetUVFromWorldPosition(vIntersectPosition);
	m_pTerrainComponent->SetMouseUV(vUVPosition, m_fBrushRadius);
}

void CTerrainEditor::OnMouseDrag(CCollider * pTerget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
	if (m_pTerrainComponent && m_bClick && !m_bNone)
	{
		Vector2 vUVPosition = m_pTerrainComponent->GetUVFromWorldPosition(vIntersectPosition);
		Vector2 vCenterPosition;
		vCenterPosition.x = vIntersectPosition.x;
		vCenterPosition.y = vIntersectPosition.z;

		if (m_bBrushOn)
		{
			m_pTerrainComponent->BrushEditHeight(m_fBrushPower, m_fBrushDir, m_fBrushRadius, vUVPosition, vCenterPosition);
			m_pTerrainComponent->GetClickPosition(vUVPosition);
		}
		else
			m_pTerrainComponent->BrushEditSplat(m_fBrushPower, m_fBrushDir, m_fBrushRadius, vUVPosition, vCenterPosition, m_fCurrentSplate);
	}
}

void CTerrainEditor::SetBrushOn(bool Brush ,bool None)
{
	m_bBrushOn = Brush;
	m_bNone = None;
}

void CTerrainEditor::SetCurrentSplate(int Count)
{
	m_fCurrentSplate = Count;
}

int CTerrainEditor::GetCurrentSplate() const
{
	return m_fCurrentSplate;
}

float CTerrainEditor::GetBrushRadius() const
{
	return m_fBrushRadius;
}

void CTerrainEditor::SetBrushRadius(float Radius)
{
	m_fBrushRadius = Radius;
}

float CTerrainEditor::GetBrushPower() const
{
	return m_fBrushPower;
}

void CTerrainEditor::SetBrushPower(float Brush)
{
	m_fBrushPower = Brush;
}


