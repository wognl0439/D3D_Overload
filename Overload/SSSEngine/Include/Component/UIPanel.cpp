#include "UIPanel.h"
#include "../SceneManager.h"
#include "../Layer.h"
#include "../Transform.h"
#include "Renderer.h"
#include "SpriteRenderer.h"
#include "../Material.h"

SSS_USING

CUIPanel::CUIPanel()
{
}


CUIPanel::CUIPanel(const CUIPanel& ui)
{
}

CUIPanel::~CUIPanel()
{
}

bool CUIPanel::Initialize()
{
	CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
	CLayer*	pUILayer = pScene->FindLayer("UI");
	CUI*	pUI = m_pGameObject->AddComponent<CUI>("UIPanel");
	CTransform* pTransform = m_pGameObject->GetTransform();
	
	pTransform->SetWorldPosition(100.0f, 100.0f, 0.0f);
	pTransform->SetWorldScale(500.0f, 500.0f, 0.0f);
	SAFE_RELEASE(pTransform);

	CSpriteRenderer* pRenderer = m_pGameObject->AddComponent<CSpriteRenderer>("UIRenderer");
	CMaterial* pMaterial = pRenderer->GetMaterial();
	pMaterial->SetSampler(SAMPLER_LINEAR);

	pMaterial->SetDiffuseTexture("None", L"Sample01.png");
	pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	SAFE_RELEASE(pUI);
	SAFE_RELEASE(pMaterial);
	SAFE_RELEASE(pRenderer);
	SAFE_RELEASE(pUILayer);
	SAFE_RELEASE(pScene);
	return true;
}

int CUIPanel::Update(float fTime)
{
	return 0;
}

int CUIPanel::LateUpdate(float fTime)
{
	return 0;
}

void CUIPanel::Render(float fTime)
{
}

CUIPanel * CUIPanel::Clone() const
{
	return new CUIPanel(*this);
}
