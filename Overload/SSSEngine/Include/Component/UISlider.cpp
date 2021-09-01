#include "UISlider.h"
#include "../Input.h"
#include "../Transform.h"
#include "UIPanel.h"
#include "UICollider.h"
#include "../SceneManager.h"
#include "../Scene.h"
#include "../Layer.h"
#include "SpriteRenderer.h"
#include "../Material.h"

SSS_USING

CUISlider::CUISlider() :
	m_pTextureTransform(nullptr),
	m_pAlphaMask(nullptr),
	m_fOffset(30.f)
{
}

CUISlider::~CUISlider()
{
	SAFE_RELEASE(m_pTextureTransform);
}

void CUISlider::OffSet(float _iOffSet)
{
	m_fOffset = _iOffSet;
}

float CUISlider::OffSet() const
{
	return m_fOffset;
}

int CUISlider::MinIndex()
{
	return m_iMinIndex;
}

void CUISlider::MinIndex(int _index)
{
	m_iMinIndex = _index;
}

int CUISlider::MaxIndex()
{
	return m_iMaxIndex;
}

void CUISlider::MaxIndex(int _index)
{
	m_iMaxIndex = _index;
}

void CUISlider::TexturePosition(Vector3 _Transform)
{
	m_pTextureTransform->SetWorldPosition(_Transform);
}

void CUISlider::TexturePosition(Vector2 _Transform)
{
	Vector3 vec = Vector3(_Transform.x, _Transform.y, 0.0f);
	TexturePosition(vec);
}

Vector3 CUISlider::TexturePosition() const
{
	return m_pTextureTransform->GetWorldPosition();
}

void CUISlider::ObjectPosition(Vector3 _Transform)
{
	m_pTransform->SetWorldPosition(_Transform);
}

void CUISlider::ObjectPosition(Vector2 _Transform)
{
	Vector3 vec = Vector3(_Transform.x, _Transform.y, 0.0f);
	ObjectPosition(vec);
}

Vector3 CUISlider::ObjectPosition() const
{
	return m_pTransform->GetWorldPosition();
}

bool CUISlider::Initialize()
{
	m_iSliderCursor = 0;
	m_iMinIndex = 0;
	m_iMaxIndex = 30;

	//충돌체 추가
	CUICollider* pUIColl = m_pGameObject->AddComponent<CUICollider>("SliderCollider");
	SAFE_RELEASE(pUIColl);

	//main object 는 기존 gameobject 를 사용하고
	//main object 는 기본적으로 Alpha Mask를 갖고있다.

	//Texture 는 Component 내에서 새로운 GameObject 를 생성하여 사용한다.
	//Texture 관련 변수는 Texture 를 붙여 명시할것.



	//m_pTransform->SetPivot(0.5f, 0.35f, 0.0f);
	//m_pTransform->SetPivot(0.0f, 0.0f, 0.0f);
	

	CScene* pScene = CSceneManager::GetInstance()->GetCurrentScene();
	CLayer* pLayer = pScene->FindLayer("UI");
	CGameObject* pTextureGameObject = CGameObject::CreateObject("Texture", pLayer);
	CSpriteRenderer* pTextureRenderer = pTextureGameObject->AddComponent<CSpriteRenderer>("SliderTexture");
	CMaterial* pTextureMaterial = pTextureRenderer->GetMaterial();

	m_pTextureTransform = pTextureGameObject->GetTransform();

	m_pTransform->SetWorldScale(500.0f, 500.0f, 0.0f);
	m_pTransform->SetWorldPosition(100.0f, 100.0f, 0.0f);

	//m_pTextureTransform->SetPivot(0.5f, 0.1f, 0.0f);
	//m_pTransform->SetPivot(0.5f, 0.1f, 0.0f);
	//m_pTransform->SetLocalPosition(1.0f, 0.7f, 0.0f);

	m_pTextureTransform->SetWorldScale(500.0f, 500.0f, 0.0f);
	m_pTextureTransform->SetWorldPosition(100.0f, 100.0f, 0.0f);

	if (pTextureMaterial == nullptr)
	{
		assert(false);
		return false;
	}

	pTextureRenderer->SetShader(SLIDER_SHADER);
	//pTextureRenderer->SetShader(ALPHAMASK_SHADER);
	SAFE_RELEASE(pTextureRenderer);
	pTextureMaterial->SetDiffuseTexture("SliderTexture", L"SliderTestImage.png");
	pTextureMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	pTextureMaterial->SetDiffuseTextureRegister(10);

	CSpriteRenderer* pMaskRenderer = m_pGameObject->AddComponent<CSpriteRenderer>("SliderMask");

	pMaskRenderer->SetShader(SLIDER_SHADER);
	//pMaskRenderer->SetShader(ALPHAMASK_SHADER);
	CMaterial* pMaterial = pMaskRenderer->GetMaterial();
	pMaterial->SetSampler(SAMPLER_POINT);

	//m_pTransform->SetPivot(0.1f, 0.1f, 0.0f);
	pMaterial->SetDiffuseMultiTexture("SliderMask", "MaskTex", L"TestSliderMask2.png");
	pMaterial->SetDiffuseMultiTextureRegister("SliderMask", 9);
	pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	SAFE_RELEASE(pMaskRenderer);
	SAFE_RELEASE(pMaterial);
	SAFE_RELEASE(pTextureMaterial);
	SAFE_RELEASE(pScene);
	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(pTextureGameObject);

	return true;
}

int CUISlider::Update(float fTime)
{
	return 0;
}

int CUISlider::LateUpdate(float fTime)
{
	return 0;
}

void CUISlider::Render(float fTime)
{
}

CUISlider * CUISlider::Clone() const
{
	return new CUISlider(*this);
}

void CUISlider::OnMouseEnter(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
}

void CUISlider::OnMouseStay(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
	//마우스 휠 작동시 TextureTransform 의 위치를 옮겨주어야 한다.	

	if (GETKEYDOWN(MOUSE_WHEEL_DOWN))
	{
		if (m_iSliderCursor < m_iMaxIndex)
		{
			Vector3	 text = m_pTextureTransform->GetWorldPosition();
			text.z += m_fOffset;			
			m_pTextureTransform->SetWorldPosition(text);
			++m_iSliderCursor;
		}
	}
	else if (GETKEYDOWN(MOUSE_WHEEL_UP))
	{
		if (m_iSliderCursor > m_iMinIndex)
		{
			Vector3	 text = m_pTextureTransform->GetWorldPosition();
			text.z -= m_fOffset;
			m_pTextureTransform->SetWorldPosition(text);		
			--m_iSliderCursor;
		}
	}
}

void CUISlider::OnMouseExit(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
}