#include "PointLight.h"
#include "../Transform.h"

SSS_USING

CPointLight::CPointLight()
{
}


CPointLight::CPointLight(const CPointLight & light) :
	CLight(light)
{
	*this = light;
}

CPointLight::~CPointLight()
{
}

void CPointLight::SetRange(float fRange)
{
	m_tLightInfo.fRange = fRange;
}

float CPointLight::GetRange()
{
	return m_tLightInfo.fRange;
}

bool CPointLight::Initialize()
{
	CLight::Initialize();
	m_tLightInfo.iType = LT_POINT;
	return true;
}

void CPointLight::Start()
{
	return;
}

int CPointLight::Update(float fTime)
{
	return 0;
}

int CPointLight::LateUpdate(float fTime)
{
	CLight::LateUpdate(fTime);

	m_tLightInfo.vPosition = m_pTransform->GetWorldPosition();

	return 0;
}

void CPointLight::Render(float fTime)
{
	CLight::Render(fTime);
}

CPointLight * CPointLight::Clone() const
{
	return new CPointLight(*this);
}

bool CPointLight::Save(FILE * pFile)
{
	fwrite(&m_tLightInfo, sizeof(LIGHTCBUFFER), 1, pFile);

	return true;
}

bool CPointLight::Load(FILE * pFile)
{
	fread(&m_tLightInfo, sizeof(LIGHTCBUFFER), 1, pFile);

	return true;
}
