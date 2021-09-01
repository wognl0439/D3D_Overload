#include "MoveObj.h"
#include "../Input.h"
#include "../Transform.h"
#include "SpriteRenderer.h"
#include "../Material.h"

SSS_USING

CMoveObj::CMoveObj() :
	m_bMove(false)
{
}

CMoveObj::~CMoveObj()
{
}

bool CMoveObj::Initialize()
{
	return true;
}

int CMoveObj::Update(float fTime)
{
	if (GETKEYUP(MOUSE_LEFT))
	{
		m_bMove = false;
	}

	return 0;
}

void CMoveObj::OnMouseStay(CCollider * pTarget, const Vector2 & mousePosition, const Vector3 & vIntersectedPosition, float fTime)
{
	if (GETKEY(MOUSE_LEFT))
	{
		Vector2 MousePos = CInput::GetInstance()->GetCursorPosition(SPACE_DEVICE);
		Vector3 vTranspos = m_pTransform->GetWorldPosition();
		vTranspos.x = m_pTransform->GetWorldScale().x * 0.5f;
		vTranspos.y = m_pTransform->GetWorldScale().y * 0.5f;

		MousePos.x -= vTranspos.x;
		MousePos.y -= vTranspos.y;

		vTranspos.z = 0.0f;

		m_pTransform->SetWorldPosition((int)MousePos.x, (int)MousePos.y, 0.0f);
	}
}

void CMoveObj::OnMouseExit(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
}

void CMoveObj::OnMouseDrag(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
}

