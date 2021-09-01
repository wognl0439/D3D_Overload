#include "RadioButton.h"
#include "GameObject.h"
#include "Transform.h"
#include "Component/SpriteRenderer.h"
#include "input.h"
#include "Component/UICollider.h"

CRadioButton::CRadioButton()	:
	m_bCheck(false)
{
}


CRadioButton::~CRadioButton()
{
}

bool CRadioButton::Initialize()
{
	CSpriteRenderer* pSpriteRenderer = m_pGameObject->AddComponent<CSpriteRenderer>();
	pSpriteRenderer->SetDiffuseTexture("Check_False", L"UI\\Check_False.png");
	pSpriteRenderer->SetZOrder(15000);
	SAFE_RELEASE(pSpriteRenderer);

	m_pTransform->SetPivot(0.5f, 0.5f, 0.f);
	m_pTransform->SetWorldScale(25, 25.f, 1.f);
	m_pTransform->SetWorldPosition(640.f, 360.f, 0.f);
	
	CUICollider* pCollider = m_pGameObject->AddComponent<CUICollider>();
	SAFE_RELEASE(pCollider);

	return true;
}

void CRadioButton::Start()
{
	if (m_bCheck)
	{
		CSpriteRenderer* pSpriteRenderer = m_pGameObject->GetComponent<CSpriteRenderer>();
		pSpriteRenderer->SetDiffuseTexture("Check_True", L"UI\\Check_True.png");
		SAFE_RELEASE(pSpriteRenderer);
	}
}

void CRadioButton::OnMouseStay(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
	if (GETKEYDOWN(MOUSE_LEFT))
	{
		m_bCheck = !m_bCheck;

		if (m_bCheck)
		{
			CSpriteRenderer* pSpriteRenderer = m_pGameObject->GetComponent<CSpriteRenderer>();
			pSpriteRenderer->SetDiffuseTexture("Check_True", L"UI\\Check_True.png");
			SAFE_RELEASE(pSpriteRenderer);
		}
		else
		{
			CSpriteRenderer* pSpriteRenderer = m_pGameObject->GetComponent<CSpriteRenderer>();
			pSpriteRenderer->SetDiffuseTexture("Check_False", L"UI\\Check_False.png");
			SAFE_RELEASE(pSpriteRenderer);
		}
	}
}

bool CRadioButton::GetCheck() const
{
	return m_bCheck;
}
void CRadioButton::SetCheck(const bool check)
{
	m_bCheck = check;
}