#include "UIButton.h"
#include "../Component.h"
#include "SpriteRenderer.h"
#include "../GameObject.h"
#include "../Material.h"
#include "../Transform.h"
#include "UICollider.h"

#include "../Input.h"

SSS_USING

CUIButton::CUIButton()
{
	m_eUIType = UT_BUTTON;
	m_bCheck = false;
}

CUIButton::CUIButton(const CUIButton & component) :
	CUI(component)
{
}

CUIButton::~CUIButton()
{
}

bool CUIButton::Initialize()
{
	CSpriteRenderer*	pRenderer = m_pGameObject->AddComponent<CSpriteRenderer>("ButtonRenderer");
	CMaterial*	pMaterial = pRenderer->GetMaterial();

	pMaterial->SetMaterial(Vector4::white);
	pMaterial->SetSampler(SAMPLER_LINEAR);
	pMaterial->SetDiffuseTexture("Start", L"UIImage/Start.png");

	SAFE_RELEASE(pMaterial);
	SAFE_RELEASE(pRenderer);

	CTransform*	pTransform = m_pGameObject->GetTransform();

	pTransform->SetWorldPosition(100.0f, 100.0f, 0.0f);
	pTransform->SetWorldScale(1000.f, 500.f, 1.f);
	SAFE_RELEASE(pTransform);

	CUICollider* pCollider = m_pGameObject->AddComponent<CUICollider>("ButtonCollider");
	SAFE_RELEASE(pCollider);



	return true;
}

void CUIButton::OnMouseEnter(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{

}

void CUIButton::OnMouseStay(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
	CSpriteRenderer*	pRenderer = m_pGameObject->GetComponent<CSpriteRenderer>();
	CMaterial*	pMaterial = pRenderer->GetMaterial();

	pMaterial->SetMaterial(Vector4::white);
	pMaterial->SetSampler(SAMPLER_LINEAR);
	pMaterial->SetDiffuseTexture("Stay", L"UIImage/Stay.png");

	if (GETKEY(MOUSE_LEFT))
	{
		pMaterial->SetDiffuseTexture("Click", L"UIImage/Click.png");
	}

	SAFE_RELEASE(pMaterial);
	SAFE_RELEASE(pRenderer);

}

void CUIButton::OnMouseExit(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	CSpriteRenderer*	pRenderer = m_pGameObject->GetComponent<CSpriteRenderer>();
	CMaterial*	pMaterial = pRenderer->GetMaterial();

	pMaterial->SetMaterial(Vector4::white);
	pMaterial->SetSampler(SAMPLER_LINEAR);
	pMaterial->SetDiffuseTexture("Start");

	SAFE_RELEASE(pMaterial);
	SAFE_RELEASE(pRenderer);
}
