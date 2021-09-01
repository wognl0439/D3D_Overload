#include "UI.h"

SSS_USING

CUI::CUI() 
{
}

CUI::CUI(const CUI & ui):
	CComponent(ui)
{
}

CUI::~CUI()
{
}

void CUI::SetUIType(UI_TYPE _type)
{
	m_eUIType = _type;
}

UI_TYPE CUI::GetUIType() const
{
	return m_eUIType;
}

bool CUI::Initialize()
{
	return true;
}

int CUI::Update(float fTime)
{
	return 0;
}

int CUI::LateUpdate(float fTime)
{
	return 0;
}

void CUI::Render(float fTime)
{
	//if (m_bVisible == false)
	//{
	//	return;
	//}
	//else
	//{
	//	CComponent::Render(fTime);
	//}
}

CUI * CUI::Clone() const
{
	return new CUI(*this);
}

bool CUI::Save(FILE * pFile)
{
	return true;
}


bool CUI::Load(FILE * pFile)
{
	return true;
}
