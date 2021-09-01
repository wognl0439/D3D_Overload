#include "PostOption.h"
#include "input.h"
#include "GameObject.h"
#include "Transform.h"
#include "Component/SpriteRenderer.h"
#include "RadioButton.h"

#include "Component/Camera.h"
#include "Scene.h"
#include "Layer.h"

CPostOption::CPostOption() :
	m_bVisible(false),
	m_pPostProcessing(NULL)
{
}


CPostOption::~CPostOption()
{
	Safe_Release_VecList(m_vecButton);
}

void CPostOption::CreatePanel()
{
	CSpriteRenderer* pSpriteRenderer = m_pGameObject->AddComponent<CSpriteRenderer>();
	pSpriteRenderer->SetDiffuseTexture("PostProcessingOption", L"UI\\PostProcessingOption_None.png");
	pSpriteRenderer->Enable(false);
	pSpriteRenderer->SetZOrder(10000);
	SAFE_RELEASE(pSpriteRenderer);

	m_pTransform->SetPivot(0.5f, 0.5f, 0.f);
	m_pTransform->SetWorldScale(510, 420.f, 1.f);
	m_pTransform->SetWorldPosition(640.f, 360.f, 0.f);

	uint32_t iFlags = 0;
	bool bShadow = false;
	CCamera* pCamera = m_pScene->GetMainCamera();
	bShadow = pCamera->GetShadowState();
	CPostProcessing* pPostProcessing = pCamera->GetComponent<CPostProcessing>();
	if (pPostProcessing)
	{
		iFlags = pPostProcessing->GetPostProcessingFlags();
		SAFE_RELEASE(pPostProcessing);
	}
	SAFE_RELEASE(pCamera);

	for (size_t i = 0; i < 9; ++i)
	{
		bool bResult = false;
		if (i < 8)
		{
			uint32_t index = 0x01 << i;
			bResult = iFlags & index;
		}
		else
		{
			bResult = bShadow;
		}
		
		CGameObject* pObj = CGameObject::CreateObject("RadioButton", m_pLayer);
		CRadioButton* pRadio = pObj->AddComponent<CRadioButton>();
		pRadio->SetCheck(bResult);
		m_vecButton.push_back(pRadio);
		SAFE_RELEASE(pRadio);
		CTransform* pTr = pObj->GetTransform();
		pTr->SetWorldPosition(640.f + 60.f, 167.f + i * 35.f, 0.f);
		SAFE_RELEASE(pTr);
		SAFE_RELEASE(pObj);
	}
}

void CPostOption::SetPanelState(bool bOn)
{
	CSpriteRenderer* pRenderer = GetComponent<CSpriteRenderer>();
	pRenderer->Enable(bOn);
	SAFE_RELEASE(pRenderer);

	size_t iCount = m_vecButton.size();
	for (size_t i = 0; i < iCount; ++i)
	{
		CGameObject* pObj = m_vecButton[i]->GetGameObject();
		if (pObj)
		{
			pObj->Enable(bOn);
			SAFE_RELEASE(pObj);
		}
	}
}

void CPostOption::UpdateButton()
{
	uint32_t iFlags = 0;
	bool bShadow = false;

	CCamera* pCamera = m_pScene->GetMainCamera();
	
	CPostProcessing* pPostProcessing = pCamera->GetComponent<CPostProcessing>();
	if (pPostProcessing)
	{
		iFlags = pPostProcessing->GetPostProcessingFlags();
	}
	bShadow = pCamera->GetShadowState();

	SAFE_RELEASE(pCamera); 

	size_t iCount = m_vecButton.size();
	for (size_t i = 0; i < iCount; ++i)
	{
		bool bState = false;
		if (i < 8)
		{
			uint32_t index = 0x01 << i;
			bState = iFlags & index;
			m_vecButton[i]->SetCheck(bState);
		}
		else
		{
			bState = bShadow;
			m_vecButton[i]->SetCheck(bState);
		}
	}
	
	SAFE_RELEASE(pPostProcessing);
}

bool CPostOption::Initialize()
{
	CreatePanel();
	return true;
}

void CPostOption::Start()
{
	SetPanelState(m_bVisible);
	UpdateButton();
}

int CPostOption::Update(float fTime)
{
	if (GETKEYDOWN(KEY_ESCAPE)) // 옵션창 보이기 유무 및 하위 버튼들 기능 작동 유무 선택
	{
		m_bVisible = !m_bVisible;
		UpdateButton();
		SetPanelState(m_bVisible);
	}
	GET_SINGLE(CInput)->SetCursorVisible(m_bVisible);
	if (m_bVisible && GETKEYUP(MOUSE_LEFT)) // 옵션창 보이기 유무 및 하위 버튼들 기능 작동 유무 선택
	{
		uint32_t iFlags = 0;

		size_t iCount = m_vecButton.size() - 1 ; // 맨 뒤는 그림자 이기 때문에 하나 뺌
		for (size_t i = 0; i < iCount; ++i)
		{
			iFlags |= (uint32_t)m_vecButton[i]->GetCheck() << i;
		}

		CCamera* pCamera = m_pScene->GetMainCamera();
		CPostProcessing* pPostProcessing = pCamera->GetComponent<CPostProcessing>();
		if (pPostProcessing)
		{
			pPostProcessing->SetPostProcessingFlags(iFlags);
		}
		pCamera->SetShadowState(m_vecButton[m_vecButton.size() - 1]->GetCheck());
		SAFE_RELEASE(pCamera);
		SAFE_RELEASE(pPostProcessing);		
	}

	return 0;
}