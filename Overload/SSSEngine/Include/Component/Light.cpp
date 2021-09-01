#include "Light.h"
#include "../ShaderManager.h"
#include "../RenderManager.h"
#include "../GameObject.h"

SSS_USING

CLight::CLight()
{
	SetTag("Light");
	m_iBaseTypeHash = typeid(CLight).hash_code();
}

CLight::CLight(const CLight & light) :
	CComponent(light)
{
	*this = light;
}

CLight::~CLight()
{
}

LIGHT_TYPE CLight::GetLightType() const
{
	return (LIGHT_TYPE)m_tLightInfo.iType;
}

void CLight::SetLightColor(const Vector4 & vDif, const Vector4 & vAmb, const Vector4 & vSpc)
{
	m_tLightInfo.vDiffuse = vDif;
	m_tLightInfo.vAmbient = vAmb;
	m_tLightInfo.vSpecular = vSpc;
}

const Vector4 & CLight::GetDiffuseColor() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_tLightInfo.vDiffuse;
}

const Vector4 & CLight::GetAmbientColor() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_tLightInfo.vAmbient;
}

const Vector4 & CLight::GetSpecularColor() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_tLightInfo.vSpecular;
}

bool CLight::Initialize()
{
	memset(&m_tLightInfo, 0, sizeof(m_tLightInfo));

	m_tLightInfo.vDiffuse = Vector4::white;
	m_tLightInfo.vAmbient = Vector4(0.5f, 0.5f, 0.5f, 1.f);
	m_tLightInfo.vSpecular = Vector4(1.0f, 1.0f, 1.0f, 1.f);

	return true;
}

void CLight::Start()
{
}

int CLight::LateUpdate(float fTime)
{
	if (m_pGameObject->IsEnable())
		GET_SINGLE(CRenderManager)->AddLight(this);

	return 0;
}

void CLight::Render(float fTime)
{
}

void CLight::SetLight()
{
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(LIGHT_CBUFFER, &m_tLightInfo, CBT_PIXEL);
}
