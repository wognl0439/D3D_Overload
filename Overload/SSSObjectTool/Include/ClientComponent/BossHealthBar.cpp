#include "BossHealthBar.h"
#include "Transform.h"
#include "Component/SpriteRenderer.h"
#include "Material.h"
#include "ShaderManager.h"
#include "../Mathf.h"
#include "Component/Text.h"

CBossHealthBar::CBossHealthBar()
{
	m_tCBuffer = {};
	m_iMaxLineCount = 1;
}


CBossHealthBar::~CBossHealthBar()
{
}

bool CBossHealthBar::UpdateHealthPoint(float fHealthPoint, float fMaxHealthPoint)
{
	float fProgress = fHealthPoint / fMaxHealthPoint;
	if (fProgress <= 0.f)
	{
		m_tCBuffer.fProgress = 0.f;
		m_tCBuffer.iLineCount = 0;
		m_tCBuffer.vEmpty = Vector3(0.f, 0.f, 0.f);
		m_tCBuffer.vColor = Vector3(0.f, 0.f, 0.f);
		return false;
	}

	fProgress = fProgress * m_iMaxLineCount;
	float fCurrentCount = 0;
	fProgress = modf(fProgress, &fCurrentCount);

	m_tCBuffer.fProgress = fProgress;
	m_tCBuffer.iLineCount = fCurrentCount;

	// ¾ÕÁÙ
	float fFrontLineColorProgress = fCurrentCount / (float)m_iMaxLineCount;

	Vector3 vHealthPointColor = {};
	vHealthPointColor.x = CMathf::Clamp(CMathf::Lerp(1.50f, 0.0f, fFrontLineColorProgress), 0.0f, 1.0f); // Red
	vHealthPointColor.y = CMathf::Clamp(CMathf::Lerp(0.0f, 1.50f, fFrontLineColorProgress), 0.0f, 1.0f); // Green
	m_tCBuffer.vColor = vHealthPointColor;

	// µÞÁÙ
	--fCurrentCount;
	if (fCurrentCount <= -1.f)
	{
		m_tCBuffer.vEmpty = Vector3(0.f, 0.f, 0.f);
	}
	else
	{
		float fBackLineColorProgress = fCurrentCount / (float)m_iMaxLineCount;
		vHealthPointColor.x = CMathf::Clamp(CMathf::Lerp(1.50f, 0.0f, fBackLineColorProgress), 0.0f, 1.0f); // Red
		vHealthPointColor.y = CMathf::Clamp(CMathf::Lerp(0.0f, 1.50f, fBackLineColorProgress), 0.0f, 1.0f); // Green
		m_tCBuffer.vEmpty = vHealthPointColor;
	}

	return true;
}

void CBossHealthBar::SetLineCount(int iCount)
{
	m_iMaxLineCount = iCount;
}

bool CBossHealthBar::Initialize()
{
	m_pTransform->SetWorldPosition(640.f, 50.f, 0.f);
	m_pTransform->SetWorldScale(825.f, 50.f, 1.f);
	m_pTransform->SetPivot(0.5f, 0.5f, 0.f);

	CSpriteRenderer* pSpriteRenderer = m_pGameObject->AddComponent<CSpriteRenderer>();
	pSpriteRenderer->SetShader("BossHealthBarShader");

	CMaterial* pMaterial = pSpriteRenderer->GetMaterial();
	pMaterial->SetDiffuseTexture("HealthBarBackground", L"UI\\HealthBarBackground.png");
	pMaterial->SetNormalTexture("HealthBarForeground", L"UI\\HealthBarForeground.png");
	SAFE_RELEASE(pMaterial);

	SAFE_RELEASE(pSpriteRenderer);

	return true;
}

void CBossHealthBar::Start()
{
}

int CBossHealthBar::Update(float fTime)
{
	return 0;
}

int CBossHealthBar::Prerender(CMeshRenderer * pRenderer)
{
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer("BOSSHEALTHBAR", &m_tCBuffer, CBT_PIXEL);

	return 0;
}
