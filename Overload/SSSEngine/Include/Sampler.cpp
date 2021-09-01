#include "Sampler.h"
#include "Device.h"

SSS_USING

CSampler::CSampler() :
	m_pSampler(NULL)
{
}


CSampler::~CSampler()
{
	SAFE_RELEASE(m_pSampler);
}

const string & CSampler::GetKey() const
{
	return m_strKey;
}

bool CSampler::CreateSampler(const string & strKey, D3D11_FILTER eFilter, D3D11_TEXTURE_ADDRESS_MODE eU, D3D11_TEXTURE_ADDRESS_MODE eV, D3D11_TEXTURE_ADDRESS_MODE eW, float fMipLODBias, UINT iMaxAnisotropy, D3D11_COMPARISON_FUNC eFunc, const Vector4 & vBorderColor, float fMinLOD, float fMaxLOD)
{
	D3D11_SAMPLER_DESC	tDesc = {};
	tDesc.Filter = eFilter;
	tDesc.AddressU = eU;
	tDesc.AddressV = eV;
	tDesc.AddressW = eW;
	tDesc.MinLOD = fMinLOD;
	tDesc.MaxLOD = fMaxLOD;
	memcpy(tDesc.BorderColor, &vBorderColor, sizeof(float) * 4);
	tDesc.ComparisonFunc = eFunc;
	tDesc.MaxAnisotropy = iMaxAnisotropy;
	tDesc.MipLODBias = fMipLODBias;

	if (FAILED(DEVICE->CreateSamplerState(&tDesc, &m_pSampler)))
	{
		assert(false);
		return false;
	}

	m_strKey = strKey;
	return true;
}

void CSampler::SetSampler(int iRegister)
{
	DEVICE_CONTEXT->PSSetSamplers(iRegister, 1, &m_pSampler);
}

void CSampler::SetSamplerCS(int iRegister)
{
	DEVICE_CONTEXT->CSSetSamplers(iRegister, 1, &m_pSampler);
}
