#pragma once
#include "Ref.h"

SSS_BEGIN

class SSS_DLL CSampler :
	public CRef
{
private:
	friend class CResourcesManager;

private:
	CSampler();
	~CSampler();

private:
	string		m_strKey;
	ID3D11SamplerState*	m_pSampler;

public:
	const string& GetKey() const;
	bool CreateSampler(const string& strKey,
		D3D11_FILTER eFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_MODE eU = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE eV = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE eW = D3D11_TEXTURE_ADDRESS_WRAP,
		float fMipLODBias = 0.f,
		UINT iMaxAnisotropy = 1,
		D3D11_COMPARISON_FUNC eFunc = D3D11_COMPARISON_NEVER,
		const Vector4& vBorderColor = Vector4(0.f, 0.f, 0.f, 0.f),
		float fMinLOD = -FLT_MAX,
		float fMaxLOD = FLT_MAX);

	void SetSampler(int iRegister);
	void SetSamplerCS(int iRegister);
	


};

SSS_END