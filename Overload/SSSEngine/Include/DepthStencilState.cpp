#include "DepthStencilState.h"
#include "Device.h"

SSS_USING

CDepthStencilState::CDepthStencilState()
{
	SetTag("DepthStencilState");
	m_eRenderStateType = RS_DEPTHSTENCIL;

	m_iStencilRef = 1;
}


CDepthStencilState::~CDepthStencilState()
{
}

bool CDepthStencilState::CreateDepthStencilState(BOOL bEnable, D3D11_DEPTH_WRITE_MASK eDepthMask,
	D3D11_COMPARISON_FUNC eDepthFunc, BOOL bStencilEnable, UINT8 iStencilReadMask, UINT8 iStencilWriteMask,
	const D3D11_DEPTH_STENCILOP_DESC & tFrontFace, const D3D11_DEPTH_STENCILOP_DESC & tBackFace)
{
	D3D11_DEPTH_STENCIL_DESC	tDesc = {};
	tDesc.DepthEnable = bEnable;
	tDesc.DepthWriteMask = eDepthMask;
	tDesc.DepthFunc = eDepthFunc;
	tDesc.StencilEnable = bStencilEnable;
	tDesc.StencilReadMask = iStencilReadMask;
	tDesc.StencilWriteMask = iStencilWriteMask;
	tDesc.FrontFace = tFrontFace;
	tDesc.BackFace = tBackFace;

	if (FAILED(DEVICE->CreateDepthStencilState(&tDesc, (ID3D11DepthStencilState**)&m_pState)))
	{
		assert(false);
		return false;
	}

	return true;
}

void CDepthStencilState::SetState()
{
	DEVICE_CONTEXT->OMSetDepthStencilState((ID3D11DepthStencilState*)m_pState,	m_iStencilRef);
}

void CDepthStencilState::ResetState()
{
	DEVICE_CONTEXT->OMSetDepthStencilState(0, 0);
}
