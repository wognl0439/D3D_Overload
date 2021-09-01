#include "SpotLight.h"
#include "../Transform.h"

SSS_USING

CSpotLight::CSpotLight()
{
}

CSpotLight::CSpotLight(const CSpotLight & light) :
	CLight(light)
{
	*this = light;
}

CSpotLight::~CSpotLight()
{
}

void CSpotLight::SetRange(float fRange)
{
	m_tLightInfo.fRange = fRange;
}

void CSpotLight::SetInAngle(float fAngle)
{
	m_tLightInfo.fInAngle = cosf(fAngle);
}

void CSpotLight::SetOutAngle(float fAngle)
{
	m_tLightInfo.fOutAngle = cosf(fAngle);
}

float CSpotLight::GetRange()
{
	return m_tLightInfo.fRange;
}

float CSpotLight::GetInAngle()
{
	return acosf(m_tLightInfo.fInAngle);
}

float CSpotLight::GetOutAngle()
{
	return acosf(m_tLightInfo.fOutAngle);
}

bool CSpotLight::Initialize()
{
	CLight::Initialize();
	m_tLightInfo.iType = LT_SPOT;
	return true;
}

void CSpotLight::Start()
{
}


int CSpotLight::Update(float fTime)
{
	return 0;
}

int CSpotLight::LateUpdate(float fTime)
{
	CLight::LateUpdate(fTime);

	m_tLightInfo.vPosition = m_pTransform->GetWorldPosition();
	m_tLightInfo.vDirection = m_pTransform->GetWorldAxis(AX_Z);

	return 0;
}

void CSpotLight::Render(float fTime)
{
	CLight::Render(fTime);
}

CSpotLight * CSpotLight::Clone() const
{
	return new CSpotLight(*this);
}

bool CSpotLight::Save(FILE * pFile)
{
	fwrite(&m_tLightInfo, sizeof(LIGHTCBUFFER), 1, pFile);

	return true;
}

bool CSpotLight::Load(FILE * pFile)
{
	fread(&m_tLightInfo, sizeof(LIGHTCBUFFER), 1, pFile);

	return true;
}
