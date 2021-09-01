#include "UIRadioButton.h"
#include "UICheckBox.h"
#include "SpriteRenderer.h"
#include "../Material.h"
#include "../Transform.h"
#include "../SceneManager.h"
#include "../Layer.h"
#include "UICollider.h"
#include "../Input.h"

SSS_USING

CUIRadioButton::CUIRadioButton()
{
	m_strGroupName = "DefaultGroup";
}

CUIRadioButton::CUIRadioButton(const CUIRadioButton & button) :
	CUI(button)
{
}

CUIRadioButton::~CUIRadioButton()
{
	Safe_Release_VecList(m_vButton);
}

void CUIRadioButton::AddButton()
{
	CScene* pScene = CSceneManager::GetInstance()->GetCurrentScene();
	CLayer* pLayer = pScene->FindLayer("UI");

	CGameObject* pButtonObj = CGameObject::CreateObject("RadioButton", pLayer);
	CUICheckBox* pButton = pButtonObj->AddComponent<CUICheckBox>("Radio");
	pButton->SetOptionType(BOT_RADIO);
	pButton->SetGroup(this);
	pButton->Index(m_vButton.size());
	m_vButton.push_back(pButton);

	CTransform* pTransform = pButtonObj->GetTransform();
	CTransform* pObjTr = m_pGameObject->GetTransform();

	m_vButtonPosition.x += 100.0f;
	//m_vButtonPosition.y = 0.0f;
	m_vButtonPosition.z = 0.0f;

	pTransform->SetWorldPosition(m_vButtonPosition);

	SAFE_RELEASE(pObjTr);
	SAFE_RELEASE(pTransform);
	SAFE_RELEASE(pScene);
	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(pButtonObj);
	SAFE_RELEASE(pButton);
}

void CUIRadioButton::SetGroupName(const string & strName)
{
	m_strGroupName = strName;
}

string CUIRadioButton::GetGroupName() const
{
	return m_strGroupName;
}

size_t CUIRadioButton::GetGroupSize() const
{
	return m_vButton.size();
}

void CUIRadioButton::RadioCheck(int iIndex)
{
	vector<CUICheckBox*>::iterator StartIter = m_vButton.begin();
	vector<CUICheckBox*>::iterator EndIter = m_vButton.end();

	for (; StartIter != EndIter; ++StartIter)
	{
		if ((*StartIter)->Index() == iIndex)
		{
			(*StartIter)->SetCheck(true);
			continue;
		}
		(*StartIter)->SetCheck(false);
	}
}

bool CUIRadioButton::Initialize()
{
	CTransform* pTransform = m_pGameObject->GetTransform();
	m_vButtonPosition = pTransform->GetWorldPosition();
	SAFE_RELEASE(pTransform);

	AddButton();
	return true;
}

int CUIRadioButton::Update(float fTime)
{
	return 0;
}

int CUIRadioButton::LateUpdate(float fTime)
{
	return 0;
}

void CUIRadioButton::Render(float fTime)
{
}

CUIRadioButton * CUIRadioButton::Clone() const
{
	return new CUIRadioButton(*this);
}

bool CUIRadioButton::Save(FILE * pFile)
{	
	return true;
}

bool CUIRadioButton::Load(FILE * pFile)
{

	return true;
}
