#include "RasterizerState.h"
#include "Device.h"


SSS_USING

CRasterizerState::CRasterizerState()
{
	SetTag("RasterizerState");
	m_eRenderStateType = RS_RASTERIZER;
}

CRasterizerState::~CRasterizerState()
{
}

bool CRasterizerState::CreateRasterizerState(D3D11_FILL_MODE eFillMode, D3D11_CULL_MODE eCullMode)
{
	D3D11_RASTERIZER_DESC 	tDesc = {};

	tDesc.FillMode = eFillMode;
	tDesc.CullMode = eCullMode;
	tDesc.DepthClipEnable = true;

	if (FAILED(DEVICE->CreateRasterizerState(&tDesc, (ID3D11RasterizerState**)&m_pState)))
		return false;

	return true;
}

void CRasterizerState::SetState()
{
	DEVICE_CONTEXT->RSSetState((ID3D11RasterizerState*)m_pState);
}

void CRasterizerState::ResetState()
{
	DEVICE_CONTEXT->RSSetState(NULL);
}
