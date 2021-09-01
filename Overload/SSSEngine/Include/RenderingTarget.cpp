#include "RenderingTarget.h"

#include "Device.h"
#include "ShaderManager.h"
#include "Shader.h"
#include "Mesh.h"
#include "ResourcesManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Component/Camera.h"
#include "Sampler.h"
#include "Device.h"

SSS_USING

CRenderingTarget::CRenderingTarget() :
	m_pTargetView(NULL),
	m_pDepthView(NULL),
	m_pTargetTexture(NULL),
	m_pDepthTexture(NULL),
	m_pTargetOldView(NULL),
	m_pDepthOldView(NULL),
	m_pMatWorld(NULL),
	m_ppTargetView(NULL),
	m_ilevels(0)
{
	m_pMatWorld = new Matrix;
	m_pSRV = NULL;
	m_bDebug = false;

	memset(m_fClearColor, 0, sizeof(float) * 4);

	m_pShader = GET_SINGLE(CShaderManager)->FindShader(UI_RENDERTARGET_SHADER);
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(ENGINE_MESH_ORTHORECT_TEX);
	m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_UV_LAYOUT);
	m_pSampler = GET_SINGLE(CResourcesManager)->FindSampler(SAMPLER_LINEAR);
}


CRenderingTarget::~CRenderingTarget()
{
	SAFE_DELETE(m_pMatWorld);

	SAFE_RELEASE(m_pSampler);
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pTargetOldView);
	SAFE_RELEASE(m_pDepthOldView);
	SAFE_RELEASE(m_pSRV);
	SAFE_RELEASE(m_pTargetTexture);
	SAFE_RELEASE(m_pDepthTexture);
	SAFE_RELEASE(m_pDepthView);
	SAFE_RELEASE(m_pTargetView);

	if (m_ilevels > 1)
	{
		for (int i = 0; i < m_ilevels; ++i)
		{
			SAFE_RELEASE(m_ppTargetView[i]);
			free(m_ppTargetView[0]);
		}
		free(m_ppTargetView);
	}
	
}

ID3D11RenderTargetView * CRenderingTarget::GetRenderTargetView() const
{
	return m_pTargetView;
}

ID3D11ShaderResourceView * CRenderingTarget::GetShaderResourceView() const
{
	return m_pSRV;
}

ID3D11RenderTargetView ** CRenderingTarget::GetRenderTargetViewMip() const
{
	return m_ppTargetView;
}

void CRenderingTarget::GenerateMips()
{
	DEVICE_CONTEXT->GenerateMips(m_pSRV);
}

void CRenderingTarget::SetShader(int iRegister)
{
	DEVICE_CONTEXT->PSSetShaderResources(iRegister, 1, &m_pSRV);
}

void CRenderingTarget::ResetShader(int iRegister)
{
	ID3D11ShaderResourceView*	pSRV = NULL;
	DEVICE_CONTEXT->PSSetShaderResources(iRegister, 1, &pSRV);
}

bool CRenderingTarget::CreateTarget(const string & strName, UINT iWidth, UINT iHeight, UINT iMipLevel, DXGI_FORMAT eFmt, const Vector4 & vClearColor, DXGI_FORMAT eDepthFmt)
{
	m_strName = strName;

	m_ilevels = iMipLevel;

	D3D11_TEXTURE2D_DESC	tDesc = {};

	tDesc.Width = iWidth;
	tDesc.Height = iHeight;
	tDesc.MipLevels = iMipLevel;
	tDesc.ArraySize = 1;
	tDesc.Format = eFmt;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (tDesc.MipLevels > 1 && (D3D11_BIND_RENDER_TARGET & tDesc.BindFlags) != 0)
		tDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;


	if (FAILED(DEVICE->CreateTexture2D(&tDesc, NULL, &m_pTargetTexture)))
		return false;

	if (FAILED(DEVICE->CreateRenderTargetView(m_pTargetTexture, NULL, &m_pTargetView)))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC sSRVDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC();
	sSRVDesc.Format = eFmt;
	sSRVDesc.Texture2D.MipLevels = iMipLevel;
	sSRVDesc.Texture2D.MostDetailedMip = 0;
	sSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	if (FAILED(DEVICE->CreateShaderResourceView(m_pTargetTexture, &sSRVDesc, &m_pSRV)))
		return false;

	if (tDesc.MiscFlags == D3D11_RESOURCE_MISC_GENERATE_MIPS)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rDesc;
		rDesc.Format = eFmt;
		rDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		m_ppTargetView = (ID3D11RenderTargetView**)malloc(sizeof(ID3D11RenderTargetView*) * (m_ilevels));
		for (int i = 0; i < m_ilevels; i++)
		{
			rDesc.Texture2D.MipSlice = i;
			if (FAILED(DEVICE->CreateRenderTargetView(m_pTargetTexture, &rDesc, &m_ppTargetView[i])))
				return false;
		}
	}
	
	if (DEVICE_RESOLUTION.iWidth != iWidth || DEVICE_RESOLUTION.iHeight != iHeight)
	{
		memset(&tDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
		tDesc.Width = iWidth;
		tDesc.Height = iHeight;
		tDesc.MipLevels = 1;
		tDesc.ArraySize = 1;
		tDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		tDesc.SampleDesc.Count = 1;
		tDesc.SampleDesc.Quality = 0;
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;


		if (FAILED(DEVICE->CreateTexture2D(&tDesc, NULL, &m_pDepthTexture)))
			return false;

		if (FAILED(DEVICE->CreateDepthStencilView(m_pDepthTexture, NULL, &m_pDepthView)))
			return false;
	}

	else if (eDepthFmt != DXGI_FORMAT_UNKNOWN)
	{
		memset(&tDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
		tDesc.Width = iWidth;
		tDesc.Height = iHeight;
		tDesc.MipLevels = 1;
		tDesc.ArraySize = 1;
		tDesc.Format = eDepthFmt;
		tDesc.SampleDesc.Count = 1;
		tDesc.SampleDesc.Quality = 0;
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		if (FAILED(DEVICE->CreateTexture2D(&tDesc, NULL, &m_pDepthTexture)))
			return false;

		if (FAILED(DEVICE->CreateDepthStencilView(m_pDepthTexture, NULL, &m_pDepthView)))
			return false;
	}

	ClearTarget();

	return true;
}

void CRenderingTarget::SetDebugInfo(const Vector3 & vPos, const Vector3 & vScale)
{
	Matrix*	matPos = new Matrix;
	Matrix*	matScale = new Matrix;

	m_vPos = vPos;
	m_vScale = vScale;

	*matPos = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
	*matScale = XMMatrixScaling(vScale.x, vScale.y, vScale.z);

	*m_pMatWorld = *matScale * *matPos;

	SAFE_DELETE(matPos);
	SAFE_DELETE(matScale);

	m_bDebug = true;
}

void CRenderingTarget::ClearTarget()
{
	DEVICE_CONTEXT->ClearRenderTargetView(m_pTargetView, m_fClearColor);

	if (m_pDepthView)
		DEVICE_CONTEXT->ClearDepthStencilView(m_pDepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void CRenderingTarget::SetTarget()
{
	DEVICE_CONTEXT->OMGetRenderTargets(1, &m_pTargetOldView, &m_pDepthOldView);
	ID3D11DepthStencilView*	pDepth = m_pDepthView;

	if (!pDepth)
		pDepth = m_pDepthOldView;

	DEVICE_CONTEXT->OMSetRenderTargets(1, &m_pTargetView, pDepth);
}

void CRenderingTarget::ResetTarget()
{
	DEVICE_CONTEXT->OMSetRenderTargets(1, &m_pTargetOldView, m_pDepthOldView);
	SAFE_RELEASE(m_pTargetOldView);
	SAFE_RELEASE(m_pDepthOldView);
}

void CRenderingTarget::Render(float fTime)
{
	if (!m_bDebug)
		return;

	TRANSFORMCBUFFER	tBuffer = {};

	// Camera를 얻어온다.
	CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();

	CCamera*	pCamera = pScene->GetMainCamera();

	SAFE_RELEASE(pScene);

	tBuffer.matWorld = *m_pMatWorld;
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProjection = pCamera->GetOrthographyProjectionMatrix();
	tBuffer.matWV = tBuffer.matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
	tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
	tBuffer.matWP = tBuffer.matWorld * tBuffer.matProjection;
	tBuffer.vPivot = Vector3(0.f, 0.f, 0.f);
	tBuffer.vLength = m_pMesh->GetLength();

	SAFE_RELEASE(pCamera);

	tBuffer.matWorld = tBuffer.matWorld.Transpose();
	tBuffer.matView = tBuffer.matView.Transpose();
	tBuffer.matProjection = tBuffer.matProjection.Transpose();
	tBuffer.matWV = tBuffer.matWV.Transpose();
	tBuffer.matWVP = tBuffer.matWVP.Transpose();
	tBuffer.matVP = tBuffer.matVP.Transpose();
	tBuffer.matWP = tBuffer.matWP.Transpose();

	// Transform 정보를 Shader 상수버퍼로 넘겨준다.
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(TRANSFORM_CBUFFER, &tBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);

	DEVICE_CONTEXT->PSSetShaderResources(0, 1, &m_pSRV);
	m_pSampler->SetSampler(0);

	m_pShader->SetShader();

	DEVICE_CONTEXT->IASetInputLayout(m_pLayout);

	m_pMesh->Render();

	ID3D11ShaderResourceView*	pNULL = NULL;
	DEVICE_CONTEXT->PSSetShaderResources(0, 1, &pNULL);
}
