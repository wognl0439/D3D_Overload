#include "UICheckBox.h"
#include "../SceneManager.h"
#include "SpriteRenderer.h"
#include "../Transform.h"
#include "../Material.h"
#include "../Layer.h"
#include "../Input.h"
#include "UICollider.h"
#include "UIRadioButton.h"

SSS_USING

CUICheckBox::CUICheckBox()
{
	m_eOption = BOT_CHECK;
	m_bCheck = false;
	m_pRadioGroup = nullptr;
	m_iGroupIndex = 0;
}

CUICheckBox::CUICheckBox(const CUICheckBox& checkbutton)
	:CUI(checkbutton)
{
}

CUICheckBox::~CUICheckBox()
{
	SAFE_RELEASE(m_pRadioGroup);
}

void CUICheckBox::SetGroup(const CUIRadioButton * pGroup)
{
	m_pRadioGroup = (CUIRadioButton*)pGroup;
}

void CUICheckBox::AddGroup(const string & strGroupName)
{
	m_strGroupName = strGroupName;
}

void CUICheckBox::SetOptionType(BUTTON_OPTION_TYPE option)
{
	m_eOption = option;

	CSpriteRenderer* pRenderer = m_pGameObject->GetComponent<CSpriteRenderer>();
	CMaterial* pMaterial = pRenderer->GetMaterial();

	switch (m_eOption)
	{
	case BOT_CHECK:
	{
		pMaterial->SetDiffuseTexture("CheckNone", L"UIImage/NoneCheck.png");
	}
	break;
	case BOT_RADIO:
	{
		pMaterial->SetDiffuseTexture("RadioNone", L"UIImage/RadioNoneCheck.png");
	}
	break;
	}

	SAFE_RELEASE(pRenderer);
	SAFE_RELEASE(pMaterial);
}

void CUICheckBox::SetCheck(bool Check)
{
	m_bCheck = Check;

	CSpriteRenderer* pRenderer = m_pGameObject->GetComponent<CSpriteRenderer>();
	CMaterial* pMaterial = pRenderer->GetMaterial();

	if (m_bCheck)
	{
		pMaterial->SetDiffuseTexture("RadioCheck", L"UIImage/RadioCheck.png");
	}
	else
	{
		pMaterial->SetDiffuseTexture("RadioNoneCheck", L"UIImage/RadioNoneCheck.png");
	}

	SAFE_RELEASE(pRenderer);
	SAFE_RELEASE(pMaterial);
}

BUTTON_OPTION_TYPE CUICheckBox::GetOptionType() const
{
	return m_eOption;
}

void CUICheckBox::Index(int index)
{
	m_iGroupIndex = index;
}

int CUICheckBox::Index() const
{
	return m_iGroupIndex;
}

bool CUICheckBox::Initialize()
{
	CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
	CLayer*	pUILayer = pScene->FindLayer("UI");
	CUI*	pUI = m_pGameObject->AddComponent<CUI>("UIPanel");
	CTransform* pTransform = m_pGameObject->GetTransform();

	m_pTransform->SetWorldPosition(100.f, 100.f, 0.0f);
	m_pTransform->SetWorldScale(100.0f, 100.0f, 0.0f);

	CUICollider* pCollider = m_pGameObject->AddComponent<CUICollider>("ButtonCollider");	

	Vector2 vCollSize;
	//vCollSize.x = m_pTransform->GetWorldScale().x;
	//vCollSize.y = m_pTransform->GetWorldScale().y;

	//pCollider->SetVolume(vCollSize);


	SAFE_RELEASE(pCollider);
	SAFE_RELEASE(pTransform);

	CSpriteRenderer* pRenderer = m_pGameObject->AddComponent<CSpriteRenderer>("UIRenderer");
	CMaterial* pMaterial = pRenderer->GetMaterial();
	pMaterial->SetSampler(SAMPLER_LINEAR);

	pMaterial->SetDiffuseTexture("NoneCheck", L"UIImage/NoneCheck.png");
	pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	SAFE_RELEASE(pUI);
	SAFE_RELEASE(pMaterial);
	SAFE_RELEASE(pRenderer);
	SAFE_RELEASE(pUILayer);
	SAFE_RELEASE(pScene);

	return true;
}

int CUICheckBox::Update(float fTime)
{
	return 0;
}

int CUICheckBox::LateUpdate(float fTime)
{
	return 0;
}

void CUICheckBox::Render(float fTime)
{
}

CUICheckBox * CUICheckBox::Clone() const
{
	return new CUICheckBox(*this);
}

void CUICheckBox::OnMouseEnter(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	int a = 0;
}

void CUICheckBox::OnMouseStay(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
	CSpriteRenderer* pRenderer = m_pGameObject->GetComponent<CSpriteRenderer>();
	CMaterial* pMaterial = pRenderer->GetMaterial();

	switch (m_eOption)
	{
	case BOT_CHECK:
	{
		if (GETKEYDOWN(MOUSE_LEFT))
		{
			m_bCheck = !m_bCheck;
			if (m_bCheck)
			{
				pMaterial->SetDiffuseTexture("Check", L"UIImage/Check.png");
			}
			else
			{
				pMaterial->SetDiffuseTexture("NoneCheck", L"UIImage/NoneCheck.png");
			}
		}
	}
	break;
	case BOT_RADIO:
	{
		if (GETKEYDOWN(MOUSE_LEFT))
		{
			m_pRadioGroup->RadioCheck(m_iGroupIndex);
		}
	}
	break;
	}

	SAFE_RELEASE(pRenderer);
	SAFE_RELEASE(pMaterial);
}

void CUICheckBox::OnMouseExit(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	int a = 0;
}

bool CUICheckBox::Save(FILE* pFile)
{
	return true;
}

bool CUICheckBox::Load(FILE* pFile)
{

	fopen("CheckBox.txt", "rt");

	fread(&m_iGroupIndex, sizeof(int), 1, pFile);
	fread(&m_bCheck, sizeof(bool), 1, pFile);
	fread(&m_eOption, sizeof(BUTTON_OPTION_TYPE), 1, pFile);

	fclose(pFile);

	return true;
}
