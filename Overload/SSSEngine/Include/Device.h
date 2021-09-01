#pragma once

#include "Engine.h"

SSS_BEGIN

class SSS_DLL CDevice
{
	DECLARE_SINGLE(CDevice)

private:
	RESOLUTION	m_tResolution;
	HWND			m_hWnd;
	ID3D11Device*					m_pDevice;
	ID3D11DeviceContext*		m_pContext;
	IDXGISwapChain*				m_pSwapChain;
	ID3D11RenderTargetView*	m_pTargetView;
	ID3D11Texture2D*				m_pDepthBuffer;
	ID3D11DepthStencilView*	m_pDepthView;
	CRITICAL_SECTION			m_tCriticalSection;

public:
	ID3D11RenderTargetView* GetTargetView() const;
	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetContext() const;
	RESOLUTION GetDeviceResolution() const;

public:
	bool Initialize(HWND hWnd, const RESOLUTION& tResolution, bool bWindowMode);
	void Clear(float fClearColor[4], float fDepth, UINT8 iStencil);
	void Present();



private:
	// =========== 2D Interface ============
	ID2D1RenderTarget*	m_pD2DTarget;
	ID2D1Factory*		m_pD2DFactory;

public:
	ID2D1RenderTarget* Get2DRenderTarget()	const;
	void Enter();
	void Leave();

};

SSS_END

