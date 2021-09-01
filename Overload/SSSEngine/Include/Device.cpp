#include "Device.h"

SSS_USING

DEFINITION_SINGLE(CDevice)

CDevice::CDevice():
	m_hWnd(NULL),
	m_pDevice(NULL),
	m_pContext(NULL),
	m_pSwapChain(NULL),
	m_pDepthBuffer(NULL),
	m_pDepthView(NULL),
	m_pTargetView(NULL),
	m_pD2DTarget(NULL),
	m_pD2DFactory(NULL)
{
	InitializeCriticalSection(&m_tCriticalSection);
}


CDevice::~CDevice()
{
	DeleteCriticalSection(&m_tCriticalSection);

	SAFE_RELEASE(m_pD2DTarget);
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pTargetView);
	SAFE_RELEASE(m_pDepthBuffer);
	SAFE_RELEASE(m_pDepthView);
	SAFE_RELEASE(m_pSwapChain);

	if (m_pContext)
		m_pContext->ClearState();

	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pDevice);
}

ID3D11RenderTargetView * CDevice::GetTargetView() const
{
	return m_pTargetView;
}

ID3D11Device * CDevice::GetDevice() const
{
	return m_pDevice;
}

ID3D11DeviceContext * CDevice::GetContext() const
{
	return m_pContext;
}

RESOLUTION CDevice::GetDeviceResolution() const
{
	return m_tResolution;
}

bool CDevice::Initialize(HWND hWnd, const RESOLUTION & tResolution, bool bWindowMode)
{
	m_hWnd = hWnd;
	m_tResolution = tResolution;

	UINT iFlag = 0;
#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	//Text사용시 필요
	iFlag |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;


#pragma region Set Feature Level 

	//지원 할 다른 버전의 DX가 있다면, 배열로 추가하도록 해야함.
	D3D_FEATURE_LEVEL eFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL eCurrentFeatureLevel;





#pragma endregion

#pragma region Create Swap Chain

	DXGI_SWAP_CHAIN_DESC tSwapChainDescription = {};

	tSwapChainDescription.BufferDesc.Width = tResolution.iWidth;
	tSwapChainDescription.BufferDesc.Height = tResolution.iHeight;
	tSwapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tSwapChainDescription.BufferDesc.RefreshRate.Numerator = 60;
	tSwapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
	tSwapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	tSwapChainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	tSwapChainDescription.BufferCount = 1;
	tSwapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	tSwapChainDescription.OutputWindow = hWnd;
	tSwapChainDescription.SampleDesc.Count = 1;
	tSwapChainDescription.SampleDesc.Quality = 0;
	tSwapChainDescription.Windowed = bWindowMode;
	tSwapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		
	if (FAILED(D3D11CreateDeviceAndSwapChain(
		/* in	*/		NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, iFlag, &eFeatureLevel, 1, D3D11_SDK_VERSION,
		/*out*/		&tSwapChainDescription, &m_pSwapChain, &m_pDevice, &eCurrentFeatureLevel, &m_pContext)))
	{
		MessageBox(NULL, TEXT("Fail"), TEXT("Failed to Create Device"), MB_OK);
		return false;
	}

#pragma endregion


#pragma region Create Back Buffer


	ID3D11Texture2D* pBackBuffer = NULL;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pTargetView);

	//GetBuffer에서 RefCount 올라가므로
	SAFE_RELEASE(pBackBuffer);

	D3D11_TEXTURE2D_DESC tDepthDescription = {};
	tDepthDescription.Width = tResolution.iWidth;
	tDepthDescription.Height = tResolution.iHeight;
	tDepthDescription.MipLevels = 1;
	tDepthDescription.ArraySize = 1;
	tDepthDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Depth에 24비트, Stencil에 8비트할당하도록 함
	tDepthDescription.SampleDesc.Count = 1;		//샘플링 하지 않음
	tDepthDescription.SampleDesc.Quality = 0;
	//tDepthDescription.Usage = D3D11_USAGE_DYNAMIC;
	tDepthDescription.Usage = D3D11_USAGE_DEFAULT;
	tDepthDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL; // Format에서 스텐실 비트필드를 할당했음

															//Depth Description의 값을 가지는 깊이버퍼용 텍스쳐를 만들어준 뒤, 
															//Format에서 Stencil과 Bind 하도록 비트필드를 할당 했으므로, DepthBuffer로 StencilView를 만들어준 후, 묶어준다.
	HRESULT result;

	D3D11_DEPTH_STENCIL_VIEW_DESC tDepthStencilViewDescription = {};
	tDepthStencilViewDescription.Format = tDepthDescription.Format;
	tDepthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	tDepthStencilViewDescription.Texture2D.MipSlice = 0;

	result = m_pDevice->CreateTexture2D(&tDepthDescription, NULL, &m_pDepthBuffer);
	if (result != S_OK)
	{
		return false;
	}
	result = m_pDevice->CreateDepthStencilView(m_pDepthBuffer, &tDepthStencilViewDescription, &m_pDepthView);
	if (result != S_OK)
	{
		return false;
	}

	//BackBuffer와 Depth+Stencil을 묶어준다.
	m_pContext->OMSetRenderTargets(1, &m_pTargetView, m_pDepthView);
#pragma endregion

#pragma region Set View Port
	
	//뷰포트를 설정한다.
	D3D11_VIEWPORT tViewPort = {};

	tViewPort.Width = tResolution.iWidth;
	tViewPort.Height = tResolution.iHeight;
	tViewPort.MaxDepth = 1.0f;

	//Device에 등록해줄 ViewPort의 갯수와 해당 뷰포트를 걸어준다.
	//다수일 경우, 배열 혈태로 들어간다.
	m_pContext->RSSetViewports(1, &tViewPort);

#pragma endregion


#pragma region Set Text View Port
	
	//Text
	D2D1_FACTORY_OPTIONS	tOption;
	tOption.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, tOption, &m_pD2DFactory)))
		return false;

	IDXGISurface*	pBackSurface = NULL;
	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackSurface));
	D2D1_RENDER_TARGET_PROPERTIES	props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_HARDWARE,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

	if (FAILED(m_pD2DFactory->CreateDxgiSurfaceRenderTarget(pBackSurface, props, &m_pD2DTarget)))
		return false;

	SAFE_RELEASE(pBackSurface);

#pragma endregion



	return true;
}

void CDevice::Clear(float fClearColor[4], float fDepth, UINT8 iStencil)
{
	// 1번 인자의 타겟을 2번인자의 색상으로 초기화 한다.
	// 백버퍼의 경우, 검정색은 피하도록 한다.
	m_pContext->ClearRenderTargetView(m_pTargetView, fClearColor);

	//깊이 버퍼도 초기화 한다. 2번 인자의 경우, 초기화 대상을 설정할 수 있다.
	m_pContext->ClearDepthStencilView(m_pDepthView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, fDepth, iStencil);
}

void CDevice::Present()
{
	m_pSwapChain->Present(0, 0);
}

ID2D1RenderTarget * CDevice::Get2DRenderTarget() const
{
	return m_pD2DTarget;
}

void CDevice::Enter()
{
	EnterCriticalSection(&m_tCriticalSection);
}

void CDevice::Leave()
{
	LeaveCriticalSection(&m_tCriticalSection);
}
