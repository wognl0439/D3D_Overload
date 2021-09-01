#include "BlendState.h"
#include "Device.h"

SSS_USING

CBlendState::CBlendState()
{
	m_fBlendFactor[0] = 1.f;
	m_fBlendFactor[1] = 1.f;
	m_fBlendFactor[2] = 1.f;
	m_fBlendFactor[3] = 1.f;
	m_iSampleMask = 0xffffffff;

	SetTag("BlendState");
	m_eRenderStateType = RS_BLEND;

}


CBlendState::~CBlendState()
{
}

void CBlendState::SetBlendFactor(float fFactor[4])
{
	memcpy(m_fBlendFactor, fFactor, sizeof(float) * 4);
}

void CBlendState::SetSampleMask(UINT iMask)
{
	m_iSampleMask = iMask;
}

void CBlendState::AddRenderTargetBlendDesc(BOOL bBlendEnable, D3D11_BLEND eSrc, D3D11_BLEND eDest, D3D11_BLEND_OP eOption, D3D11_BLEND eSrcAlpha, D3D11_BLEND eDestAlpha, D3D11_BLEND_OP eOptionAlpha, UINT iWriteMask)
{
	D3D11_RENDER_TARGET_BLEND_DESC	tDesc = {};
	tDesc.BlendEnable = bBlendEnable;
	tDesc.SrcBlend = eSrc;
	tDesc.DestBlend = eDest;
	tDesc.BlendOp = eOption;
	tDesc.SrcBlendAlpha = eSrcAlpha;
	tDesc.DestBlendAlpha = eDestAlpha;
	tDesc.BlendOpAlpha = eOptionAlpha;
	tDesc.RenderTargetWriteMask = iWriteMask;

	m_vecBlendDescription.push_back(tDesc);
}

bool CBlendState::CreateBlendState(BOOL bAlphaCoverage, BOOL bInDependentBlendEnable)
{
	D3D11_BLEND_DESC	tDesc = {};

	tDesc.AlphaToCoverageEnable = bAlphaCoverage;
	tDesc.IndependentBlendEnable = bInDependentBlendEnable;
	memcpy(tDesc.RenderTarget, &m_vecBlendDescription[0], sizeof(D3D11_RENDER_TARGET_BLEND_DESC) * m_vecBlendDescription.size());

	if (FAILED(DEVICE->CreateBlendState(&tDesc, (ID3D11BlendState**)&m_pState)))
		return false;

	m_vecBlendDescription.clear();

	return true;
}

void CBlendState::SetState()
{
	DEVICE_CONTEXT->OMGetBlendState((ID3D11BlendState**)&m_pPrevState, m_fPrevBlendFactor,	&m_iPrevSampleMask);
	DEVICE_CONTEXT->OMSetBlendState((ID3D11BlendState*)m_pState, m_fBlendFactor,	m_iSampleMask);
}

void CBlendState::ResetState()
{
	DEVICE_CONTEXT->OMSetBlendState(NULL, m_fPrevBlendFactor, m_iPrevSampleMask);

	SAFE_RELEASE(m_pPrevState);
}
