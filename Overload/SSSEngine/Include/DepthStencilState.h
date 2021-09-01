#pragma once
#include "RenderState.h"

SSS_BEGIN

class SSS_DLL CDepthStencilState :
	public CRenderState
{
private:
	friend class CRenderManager;

private:
	CDepthStencilState();
	~CDepthStencilState();

private:
	UINT		m_iStencilRef;
	UINT		m_iPrevStencilRef;

public:
	bool CreateDepthStencilState(BOOL bEnable = TRUE,
		D3D11_DEPTH_WRITE_MASK eDepthMask = D3D11_DEPTH_WRITE_MASK_ALL,
		D3D11_COMPARISON_FUNC eDepthFunc = D3D11_COMPARISON_LESS,
		BOOL bStencilEnable = FALSE,
		UINT8 iStencilReadMask = 0xff,
		UINT8 iStencilWriteMask = 0xff,
		const D3D11_DEPTH_STENCILOP_DESC& tFrontFace = {},
		const D3D11_DEPTH_STENCILOP_DESC& tBackFace = {});

public:
	virtual void SetState();
	virtual void ResetState();
};

SSS_END
