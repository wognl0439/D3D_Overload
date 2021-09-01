#pragma once
#include "RenderState.h"

SSS_BEGIN

class SSS_DLL CBlendState :
	public CRenderState
{
private:
	friend class CRenderManager;

private:
	CBlendState();
	~CBlendState();

private:
	vector<D3D11_RENDER_TARGET_BLEND_DESC>	m_vecBlendDescription;
	float		m_fBlendFactor[4];
	UINT		m_iSampleMask;
	float		m_fPrevBlendFactor[4];
	UINT		m_iPrevSampleMask;

public:
	void SetBlendFactor(float fFactor[4]);
	void SetSampleMask(UINT iMask);
	bool CreateBlendState(BOOL bAlphaCoverage, BOOL bInDependentBlendEnable);
	void AddRenderTargetBlendDesc(BOOL bBlendEnable = TRUE, D3D11_BLEND eSrc = D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND eDest = D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_OP eOption = D3D11_BLEND_OP_ADD, 
		D3D11_BLEND eSrcAlpha = D3D11_BLEND_ONE,
		D3D11_BLEND eDestAlpha = D3D11_BLEND_ZERO,
		D3D11_BLEND_OP eOptionAlpha = D3D11_BLEND_OP_ADD,
		UINT iWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL);

public:
	void SetState() override;
	void ResetState() override;
};

SSS_END
