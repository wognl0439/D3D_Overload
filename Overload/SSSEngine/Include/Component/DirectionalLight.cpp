#include "DirectionalLight.h"
#include "../Transform.h"

SSS_USING

CDirectionalLight::CDirectionalLight()
{
}

CDirectionalLight::CDirectionalLight(const CDirectionalLight & light) :
	CLight(light)
{
	*this = light;
}

CDirectionalLight::~CDirectionalLight()
{
}

bool CDirectionalLight::Initialize()
{
	CLight::Initialize();
	m_tLightInfo.iType = LT_DIRECTIONAL;
	return true;
}

void CDirectionalLight::Start()
{
}


int CDirectionalLight::Update(float fTime)
{
	return 0;
}

int CDirectionalLight::LateUpdate(float fTime)
{
	CLight::LateUpdate(fTime);

	m_tLightInfo.vDirection= m_pTransform->GetWorldAxis(AX_Z);

	return 0;
}

void CDirectionalLight::Render(float fTime)
{
	CLight::Render(fTime);
}

CDirectionalLight * CDirectionalLight::Clone() const
{
	return new CDirectionalLight(*this);
}

bool CDirectionalLight::Save(FILE * pFile)
{
	fwrite(&m_tLightInfo, sizeof(LIGHTCBUFFER), 1, pFile);

	return true;
}

bool CDirectionalLight::Load(FILE * pFile)
{
	fread(&m_tLightInfo, sizeof(LIGHTCBUFFER), 1, pFile);

	return true;
}
