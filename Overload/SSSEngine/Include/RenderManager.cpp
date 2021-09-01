#include "RenderManager.h"
#include "ShaderManager.h"
#include "DepthStencilState.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "Component/Light.h"
#include "Device.h"
#include "RenderingTarget.h"
#include "Component\SpriteRenderer.h"
#include "Component\MeshRenderer.h"
#include "Component\Camera.h"
#include "Input.h"
#include "SceneManager.h"
#include "Transform.h"
#include "GameObject.h"
#include "ResourcesManager.h"
#include "Sampler.h"
#include "Shader.h"
#include "Component/PostProcessing.h"
#include "PathManager.h"
#include "Component/Decal.h"
#include "Component/Distortion.h"
#include "Mesh.h"
#include "Texture.h"
#include "Core.h"

#include "Component/Light.h"
#include "Component/PointLight.h"
#include "Debug.h"

SSS_USING

DEFINITION_SINGLE(CRenderManager);

CRenderManager::CRenderManager() :
	m_bDeferred(true)
{
	m_pDepthDisable = NULL;
	m_pCullNone = NULL;
	m_eRenderMode = RM_3D;
	m_pLightAccDirShader = NULL;
	m_pLightBlendShader = NULL;
	m_pRenderBlendShader = NULL;
	m_pRenderPrevBackBufferShader = NULL;
	m_pPostProcessing = NULL;

	//HDR Resources
	m_pHDROutputBuffer = NULL;
	m_pHDROutputUAV = NULL;
	m_pHDROutputSRV = NULL;

	//Average Luminance Resources
	m_pAverageLuminanceCS = NULL;

	m_bInitialUpdate = false;

	m_bCameraMotionBlur = true;
	m_fShadowBias = 0.001562817954f;

	//Point Volume Light 멤버변수
	m_pPointLightMesh = NULL;
	m_pLightAccPointShader = NULL;
	m_pPointLightInputLayout = NULL;

	m_bShadowEnable = true;
}

CRenderManager::~CRenderManager()
{
	SAFE_RELEASE(m_pAverageLuminanceCS);
	SAFE_RELEASE(m_pHDROutputBuffer);
	SAFE_RELEASE(m_pHDROutputUAV);
	SAFE_RELEASE(m_pHDROutputSRV);

	SAFE_RELEASE(m_pRenderBlendShader);
	SAFE_RELEASE(m_pLightBlendShader);
	SAFE_RELEASE(m_pLightAccDirShader);

	SAFE_DELETE_ARRAY(m_tLightSection.pLightList);

	SAFE_RELEASE(m_pCullNone);
	SAFE_RELEASE(m_pDepthDisable);
	Safe_Release_Map(m_mapRenderState);
	Safe_Delete_Map(m_mapRenderTarget);
	Safe_Delete_Map(m_mapMRT);

	SAFE_RELEASE(m_pRenderPrevBackBufferShader);

	SAFE_RELEASE(m_pPointLightMesh);
	SAFE_RELEASE(m_pLightAccPointShader);

	DESTROY_SINGLE(CShaderManager);

	for (int i = 0; i < RGT_END; ++i)
	{
		SAFE_DELETE_ARRAY(m_tRenderGroup[i].pObjList);
	}
}


RENDER_MODE CRenderManager::GetRenderMode() const
{
	return m_eRenderMode;
}

void CRenderManager::SetRenderMode(RENDER_MODE eMode)
{
	m_eRenderMode = eMode;

	if (eMode == RM_2D)
	{
		m_pDepthDisable = (CDepthStencilState*)FindRenderState(DEPTH_DISABLE);
		m_pCullNone = (CRasterizerState*)FindRenderState(CULLNONE);
	}
	else
	{
		SAFE_RELEASE(m_pCullNone);
		SAFE_RELEASE(m_pDepthDisable);
	}
}

void CRenderManager::SetDepth()
{
	/*if (m_pDepthDisable)
	m_pDepthDisable->SetState();

	if (m_pCullNone)
	m_pCullNone->SetState();*/
}

void CRenderManager::ResetDepth()
{
	/*if (m_pDepthDisable)
	m_pDepthDisable->ResetState();

	if (m_pCullNone)
	m_pCullNone->ResetState();*/
}


void CRenderManager::AddLight(CLight * pLight)
{
	if (m_tLightSection.iSize == m_tLightSection.iCapacity)
	{
		m_tLightSection.iCapacity *= 2;
		CLight**	pLightList = new CLight*[m_tLightSection.iCapacity];
		memcpy(pLightList, m_tLightSection.pLightList, sizeof(CLight*) * m_tLightSection.iSize);

		SAFE_DELETE_ARRAY(m_tLightSection.pLightList);
		m_tLightSection.pLightList = pLightList;
	}

	m_tLightSection.pLightList[m_tLightSection.iSize] = pLight;
	++m_tLightSection.iSize;
}

void CRenderManager::ClearLight()
{
	m_tLightSection.iSize = 0;
}

void CRenderManager::SetLight()
{
	for (int i = 0; i < m_tLightSection.iSize; ++i)
	{
		m_tLightSection.pLightList[i]->SetLight();
	}
}

void CRenderManager::SetDirectionalLight()
{
	for (int i = 0; i < m_tLightSection.iSize; ++i)
	{
		if (m_tLightSection.pLightList[i]->GetLightType() == LT_DIRECTIONAL)
		{
			m_tLightSection.pLightList[i]->SetLight();
			break;
		}
	}
}


bool CRenderManager::Initialize()
{
	if (!GET_SINGLE(CShaderManager)->Initialize())
		return false;

	AddRenderTargetBlendDesc(ALPHA_BLEND);
	CBlendState*	pBlendState = CreateBlendState(ALPHA_BLEND, false);
	SAFE_RELEASE(pBlendState);


	AddRenderTargetBlendDesc("LightAcc", TRUE, D3D11_BLEND_ONE, D3D11_BLEND_ONE);
	pBlendState = CreateBlendState("LightAcc");
	SAFE_RELEASE(pBlendState);

	AddRenderTargetBlendDesc(ALPHA_BLEND_DISABLE, FALSE);
	pBlendState = CreateBlendState(ALPHA_BLEND_DISABLE);
	SAFE_RELEASE(pBlendState);

	CDepthStencilState*	pDepthStencilState = CreateDepthStencilState(DEPTH_DISABLE, FALSE);
	SAFE_RELEASE(pDepthStencilState);

	pDepthStencilState = CreateDepthStencilState(DEPTH_ENABLE, TRUE);
	SAFE_RELEASE(pDepthStencilState);

	pDepthStencilState = CreateDepthStencilState(DEPTH_SKYBOX, TRUE,	D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);
	SAFE_RELEASE(pDepthStencilState);

	pDepthStencilState = CreateDepthStencilState("LightBack", TRUE,
		D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_GREATER_EQUAL);

	SAFE_RELEASE(pDepthStencilState);


	CRasterizerState* pRasterizer = CreateRasterizerState(CULLNONE, D3D11_FILL_SOLID, D3D11_CULL_NONE);
	SAFE_RELEASE(pRasterizer);

	pRasterizer = CreateRasterizerState(CULLFRONT, D3D11_FILL_SOLID, D3D11_CULL_FRONT);
	SAFE_RELEASE(pRasterizer);

	pRasterizer = CreateRasterizerState(CULLBACK, D3D11_FILL_SOLID, D3D11_CULL_BACK);
	SAFE_RELEASE(pRasterizer);

	pRasterizer = CreateRasterizerState(WIREFRAME, D3D11_FILL_WIREFRAME, D3D11_CULL_NONE);
	SAFE_RELEASE(pRasterizer);


	//Render Targets
#pragma region GBuffer

	CreateRenderTarget("Albedo", DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	SetRenderTargetDebugInfo("Albedo", Vector3(0.0f, 0.0f, 0.0f), Vector3(100.f, 100.f, 1.f));

	CreateRenderTarget("Normal", DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	SetRenderTargetDebugInfo("Normal", Vector3(0.0f, 100.f, 0.0f), Vector3(100.f, 100.f, 1.f));

	CreateRenderTarget("Depth", DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	SetRenderTargetDebugInfo("Depth", Vector3(0.0f, 200.f, 0.0f), Vector3(100.f, 100.f, 1.f));

	CreateRenderTarget("Specular", DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	SetRenderTargetDebugInfo("Specular", Vector3(0.0f, 300.f, 0.0f), Vector3(100.f, 100.f, 1.f));

	CreateRenderTarget("Tangent", DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	SetRenderTargetDebugInfo("Tangent", Vector3(0.0f, 400.f, 0.0f), Vector3(100.f, 100.f, 1.f));

	CreateRenderTarget("Binormal", DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	SetRenderTargetDebugInfo("Binormal", Vector3(0.0f, 500.f, 0.0f), Vector3(100.f, 100.f, 1.f));

	vector<string>	vecMRT;

	// GBuffer MRT 생성
	vecMRT.push_back("Albedo");
	vecMRT.push_back("Normal");
	vecMRT.push_back("Depth");
	vecMRT.push_back("Specular");
	vecMRT.push_back("Tangent");
	vecMRT.push_back("Binormal");
	CreateMultiRenderTarget("GBuffer", vecMRT);
	vecMRT.clear();

	// Decal MRT 생성
	vecMRT.push_back("Albedo");
	vecMRT.push_back("Normal");
	vecMRT.push_back("Specular");
	CreateMultiRenderTarget("Decal", vecMRT);
	vecMRT.clear();

#pragma endregion

	// 그림자맵용 타겟
	CreateRenderTarget("ShadowMap", 2000.f, 2000.f, 1, DXGI_FORMAT_R32G32B32A32_FLOAT,
		Vector4(0.f, 0.f, 0.f, 0.f), DXGI_FORMAT_D24_UNORM_S8_UINT);
	SetRenderTargetDebugInfo("ShadowMap", Vector3(0.f, 600.f, 0.f), Vector3(100.f, 100.f, 1.f));

	// 오브젝트 모션블러용 타겟
	CreateRenderTarget("MotionBlur", DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT,
		Vector4(0.f, 0.f, 0.f, 0.f), DXGI_FORMAT_D24_UNORM_S8_UINT);
	SetRenderTargetDebugInfo("MotionBlur", Vector3(100.f, 600.f, 0.f), Vector3(100.f, 100.f, 1.f));

#pragma region Light

	CreateRenderTarget("LightDiffuse", DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.f, 0.f, 0.f, 0.f));
	SetRenderTargetDebugInfo("LightDiffuse", Vector3(100.f, 0.f, 0.f), Vector3(100.f, 100.f, 1.f));

	CreateRenderTarget("LightSpecular", DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.f, 0.f, 0.f, 0.f));
	SetRenderTargetDebugInfo("LightSpecular", Vector3(100.f, 100.f, 0.f), Vector3(100.f, 100.f, 1.f));

	CreateRenderTarget("LightBlend", DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.f, 0.f, 0.f, 0.f));
	SetRenderTargetDebugInfo("LightBlend", Vector3(200.f, 0.f, 0.f), Vector3(100.f, 100.f, 1.f));

	vecMRT.push_back("LightDiffuse");
	vecMRT.push_back("LightSpecular");
	//vecMRT.push_back("LightBlend");

	CreateMultiRenderTarget("LightAcc", vecMRT);

	// 최종적으로 완성된 타겟 저장용 타겟
	CreateRenderTarget("FinalColorWithPostProcess", DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	SetRenderTargetDebugInfo("FinalColorWithPostProcess", Vector3(400.f, 0.0f, 0.0f), Vector3(100.f, 100.f, 1.f));

	//조명 누적 버퍼용
	m_pLightAccDirShader = GET_SINGLE(CShaderManager)->FindShader("LightAccDir");
	m_pLightBlendShader = GET_SINGLE(CShaderManager)->FindShader("LightBlend");
	m_pRenderBlendShader = GET_SINGLE(CShaderManager)->FindShader("RenderBlend");
	
	m_pRenderPrevBackBufferShader = GET_SINGLE(CShaderManager)->FindShader("RenderPrevBackBufferShader");
#pragma endregion

	bool bASDF = false;

#pragma region HDR RenderTarget
	bASDF =CreateRenderTarget("PrevAvgLuminanceTarget", 1, 1, 1, DXGI_FORMAT_R32_FLOAT, Vector4(0.f, 0.f, 0.f, 0.f));
	SetRenderTargetDebugInfo("PrevAvgLuminanceTarget", Vector3(1080.f, 200.f, 0.f), Vector3(100.f, 100.f, 1.f));

	bASDF = CreateRenderTarget("LuminanceTarget", 512, 512, 10, DXGI_FORMAT_R32_FLOAT, Vector4(0.f, 0.f, 0.f, 0.f));
	SetRenderTargetDebugInfo("LuminanceTarget", Vector3(1180.f, 100.f, 0.f), Vector3(100.f, 100.f, 1.f));

	bASDF = CreateRenderTarget("AvgLuminanceTarget", 16, 16, 1, DXGI_FORMAT_R32_FLOAT, Vector4(0.f, 0.f, 0.f, 0.f));
	SetRenderTargetDebugInfo("AvgLuminanceTarget", Vector3(1180.f, 200.f, 0.f), Vector3(100.f, 100.f, 1.f));

	bASDF = CreateRenderTarget("BrightTarget", DEVICE_RESOLUTION.iWidth / 2, DEVICE_RESOLUTION.iHeight / 2,
		4, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.f, 0.f, 0.f, 0.f));
	SetRenderTargetDebugInfo("BrightTarget", Vector3(1180.f, 300.f, 0.f), Vector3(100.f, 100.f, 1.f));

	bASDF = CreateRenderTarget("SummedTarget", DEVICE_RESOLUTION.iWidth / 2, DEVICE_RESOLUTION.iHeight / 2,
		4, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.f, 0.f, 0.f, 0.f));
	SetRenderTargetDebugInfo("SummedTarget", Vector3(1180.f, 400.f, 0.f), Vector3(100.f, 100.f, 1.f));

	bASDF= CreateRenderTarget("BlurredHTarget", DEVICE_RESOLUTION.iWidth / 2, DEVICE_RESOLUTION.iHeight / 2,
		4, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.f, 0.f, 0.f, 0.f));
	SetRenderTargetDebugInfo("BlurredHTarget", Vector3(1180.f, 500.f, 0.f), Vector3(100.f, 100.f, 1.f));

	bASDF = CreateRenderTarget("BlurredHVTarget", DEVICE_RESOLUTION.iWidth / 2, DEVICE_RESOLUTION.iHeight / 2,
		4, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.f, 0.f, 0.f, 0.f));
	SetRenderTargetDebugInfo("BlurredHVTarget", Vector3(1180.f, 600.f, 0.f), Vector3(100.f, 100.f, 1.f));
#pragma endregion



	m_pLightAccPointShader = GET_SINGLE(CShaderManager)->FindShader("LightPointAcc");
	m_pPointLightMesh = GET_SINGLE(CResourcesManager)->FindMesh("PosSphere");
	m_pPointLightInputLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_LAYOUT);

	return true;
}

CBlendState * CRenderManager::CreateBlendState(const string & strKey, BOOL bAlphaCoverage, BOOL bIndependent)
{
	CBlendState*	pBlendState = (CBlendState*)FindRenderState(strKey);
	if (!pBlendState)
	{
		assert(false);
		return NULL;
	}
	if (!pBlendState->CreateBlendState(bAlphaCoverage, bIndependent))
	{
		pBlendState->Release();
		SAFE_RELEASE(pBlendState);
		m_mapRenderState.erase(strKey);
		assert(false);
		return NULL;
	}

	pBlendState->SetKey(strKey);
	return pBlendState;
}

bool CRenderManager::AddRenderTargetBlendDesc(const string & strKey, BOOL bBlendEnable, D3D11_BLEND eSrc, D3D11_BLEND eDest, D3D11_BLEND_OP eOption, D3D11_BLEND eSrcAlpha, D3D11_BLEND eDestAlpha, D3D11_BLEND_OP eOptionAlpha, UINT iWriteMask)
{
	CBlendState*	pBlendState = (CBlendState*)FindRenderState(strKey);

	if (!pBlendState)
	{
		pBlendState = new CBlendState;
		pBlendState->AddRef();

		pBlendState->SetKey(strKey);
		m_mapRenderState.insert(make_pair(strKey, pBlendState));
	}

	pBlendState->AddRenderTargetBlendDesc(bBlendEnable, eSrc, eDest, eOption,
		eSrcAlpha, eDestAlpha, eOptionAlpha, iWriteMask);

	SAFE_RELEASE(pBlendState);

	return true;
}

CDepthStencilState * CRenderManager::CreateDepthStencilState(const string & strKey, BOOL bEnable, D3D11_DEPTH_WRITE_MASK eDepthMask, D3D11_COMPARISON_FUNC eDepthFunc, BOOL bStencilEnable, UINT8 iStencilReadMask, UINT8 iStencilWriteMask, const D3D11_DEPTH_STENCILOP_DESC & tFrontFace, const D3D11_DEPTH_STENCILOP_DESC & tBackFace)
{
	CDepthStencilState*	pDepthStencil = (CDepthStencilState*)FindRenderState(strKey);

	if (pDepthStencil)
		return pDepthStencil;

	pDepthStencil = new CDepthStencilState;

	if (!pDepthStencil->CreateDepthStencilState(bEnable, eDepthMask, eDepthFunc,
		bStencilEnable, iStencilReadMask, iStencilWriteMask, tFrontFace,
		tBackFace))
	{
		SAFE_RELEASE(pDepthStencil);
		assert(false);
		return NULL;
	}

	pDepthStencil->AddRef();

	pDepthStencil->SetKey(strKey);
	m_mapRenderState.insert(make_pair(strKey, pDepthStencil));

	return pDepthStencil;
}

CRasterizerState * CRenderManager::CreateRasterizerState(const string & strKey, D3D11_FILL_MODE eFillMode, D3D11_CULL_MODE eCullMode)
{
	CRasterizerState* pRasterizer = (CRasterizerState*)FindRenderState(strKey);

	if (pRasterizer)
		return pRasterizer;

	pRasterizer = new CRasterizerState;

	if (!pRasterizer->CreateRasterizerState(eFillMode, eCullMode))
	{
		SAFE_RELEASE(pRasterizer);
		assert(false);

		return NULL;
	}

	pRasterizer->AddRef();

	pRasterizer->SetKey(strKey);
	m_mapRenderState.insert(make_pair(strKey, pRasterizer));

	return pRasterizer;
}

CRenderState * CRenderManager::FindRenderState(const string & strKey)
{
	unordered_map<string, CRenderState*>::iterator	iter = m_mapRenderState.find(strKey);

	if (iter == m_mapRenderState.end())
		return NULL;

	iter->second->AddRef();

	return iter->second;
}

void CRenderManager::SetPostProcessingProperties(float fTime)
{
	if (!m_bInitialUpdate)
	{
		//SetHDRResources();
		m_bInitialUpdate = true;
	}
	POSTPROCESSCBUFFER tBuffer = {};

	if (m_pPostProcessing)
	{

		uint32_t iFlags = m_pPostProcessing->m_iPostProcessFlags;
		tBuffer.iFlags = iFlags;


		if (iFlags == 0x0000)
		{
			tBuffer.iDeviceHeight = DEVICE_RESOLUTION.iHeight;
			tBuffer.iDeviceWidth = DEVICE_RESOLUTION.iWidth;
			GET_SINGLE(CShaderManager)->UpdateConstantBuffer(POSTPROCESS_CBUFFER, &tBuffer, CBT_PIXEL);
			return;
		}

		tBuffer.iDeviceHeight = DEVICE_RESOLUTION.iHeight;
		tBuffer.iDeviceWidth = DEVICE_RESOLUTION.iWidth;

		if (iFlags & PT_ANTIALIASING)
		{
		}
		
		if (iFlags & PT_BLOOM)
		{
			//tBuffer.iBlurSamplingCount = m_pPostProcessing->m_iBlurSampleCount;
		}

		if (iFlags & PT_FOG)
		{
			tBuffer.fFogMinDistance = m_pPostProcessing->m_fFogMinDistance;
			tBuffer.fFogMaxDistance = m_pPostProcessing->m_fFogMaxDistance;
			tBuffer.vFogColor = m_pPostProcessing->m_vFogColor;
		}
		if (iFlags & PT_HDR)
		{
			// CS HDR
			//if (iFlags & PT_EYE_ADAPTION)
			//{
			//	float fCurrent = CalculateAverageLuminance();
			//	float fPrev = m_pPostProcessing->m_fHDRPrevAverageLuminance;
			//	
			//	tBuffer.fHDRAverageLuminance = Lerp(fPrev, fCurrent, fTime * 5.0f);//CalculateAverageLuminance();
			//	tBuffer.fHDRExposure = m_pPostProcessing->m_fHDRExposure;
			//	tBuffer.fHDRWhiteThreshold = m_pPostProcessing->m_fHDRWhiteThreshold;
			//	
			//	m_pPostProcessing->m_fHDRPrevAverageLuminance = tBuffer.fHDRAverageLuminance;
			//}
			//else
			//{
			//	tBuffer.fHDRAverageLuminance = CalculateAverageLuminance();
			//	tBuffer.fHDRExposure = m_pPostProcessing->m_fHDRExposure;
			//	tBuffer.fHDRWhiteThreshold = m_pPostProcessing->m_fHDRWhiteThreshold;
			//}

			// PS HDR
			tBuffer.fHDRExposure = m_pPostProcessing->m_fHDRExposure;
			tBuffer.fHDRWhiteThreshold = m_pPostProcessing->m_fHDRWhiteThreshold;
		}

		if (iFlags & PT_MOTION_BLUR)
		{
			CScene* pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
			if (pScene)
			{
				CCamera* pCamera = pScene->GetMainCamera();

				tBuffer.matPreView = pCamera->GetOldView();
				tBuffer.matPreViewProj = pCamera->GetOldVP();

				tBuffer.matPreView = tBuffer.matPreView.Transpose();
				tBuffer.matPreViewProj = tBuffer.matPreViewProj.Transpose();

				/*tBuffer.iMotionBlurMode = 0;

				if (m_bCameraMotionBlur)
					tBuffer.iMotionBlurMode = 1;*/

				SAFE_RELEASE(pCamera);
				SAFE_RELEASE(pScene);
			}
		}

		if (iFlags & PT_AMBIENT_OCCLUSION)
		{
		}

		if (iFlags & PT_DEPTH_OF_FIELD)
		{
			tBuffer.fDepthOfFieldMinRange = m_pPostProcessing->m_fDepthOfFieldMinRange;
			tBuffer.fDepthOfFieldMaxRange = m_pPostProcessing->m_fDepthOfFieldMaxRange;
			tBuffer.fDepthOfFieldMaxSigma = m_pPostProcessing->m_fDepthOfFieldMaxSigma;
		}
	}

	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(POSTPROCESS_CBUFFER, &tBuffer, CBT_PIXEL); 
}

bool CRenderManager::SetHDRResources()
{
	//m_pHDRBuffer : Output Buffer
	HRESULT hr;

	//다운스케일링에 필요한 총 스레드그룹 갯수 정의
	UINT iWidth = DEVICE_RESOLUTION.iWidth;
	UINT iHeight = DEVICE_RESOLUTION.iHeight;
	UINT iDownScaleGroups = 256;

	// 다운스케일링 된 휘도를 저장할 버퍼
	D3D11_BUFFER_DESC tBufferDesc = {};
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	tBufferDesc.CPUAccessFlags = 0;
	tBufferDesc.ByteWidth = sizeof(float);
	tBufferDesc.StructureByteStride = sizeof(float);
	tBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, NULL, &m_pHDROutputBuffer)))
	{
		assert(false);
		return false;
	}
	
    D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVOutputDesc = {};
	tUAVOutputDesc.Format = DXGI_FORMAT_UNKNOWN;
	tUAVOutputDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	tUAVOutputDesc.Buffer.NumElements = 1;
	
	if (FAILED(DEVICE->CreateUnorderedAccessView(m_pHDROutputBuffer, &tUAVOutputDesc, &m_pHDROutputUAV)))
	{
		assert(false);
		return false;
	}
	
	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVOutputDesc = {};
	tSRVOutputDesc.Format = DXGI_FORMAT_UNKNOWN;
	tSRVOutputDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	tSRVOutputDesc.Buffer.NumElements = 1;
	if (FAILED(DEVICE->CreateShaderResourceView(m_pHDROutputBuffer, &tSRVOutputDesc, &m_pHDROutputSRV)))
	{
		assert(false);
		return false;
	}

	WCHAR str[MAX_PATH];
	ID3DBlob* pShaderBlob = NULL;
	if (!LoadComputeShader(L"Light.fx", "AverageLuminanceCS", SHADER_PATH, &pShaderBlob, &m_pAverageLuminanceCS))
	{
		assert(false);
		return false;
	}
	SAFE_RELEASE(pShaderBlob);


	return true;
}

float CRenderManager::CalculateAverageLuminance()
{

	CSampler*	pSampler = GET_SINGLE(CResourcesManager)->FindSampler(SAMPLER_LINEAR);
	pSampler->SetSamplerCS(11);


	UINT iWidth = DEVICE_RESOLUTION.iWidth;
	UINT iHeight = DEVICE_RESOLUTION.iHeight;
	UINT iDownScaleGroups = 256;

	D3D11_BUFFER_DESC tOutputDescription = {};
	tOutputDescription.Usage = D3D11_USAGE_STAGING;
	tOutputDescription.BindFlags = 0;
	tOutputDescription.ByteWidth = sizeof(float);
	tOutputDescription.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	tOutputDescription.StructureByteStride = sizeof(float);
	tOutputDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	ID3D11Buffer* pOutputBuffer = NULL;
	DEVICE->CreateBuffer(&tOutputDescription, NULL, &pOutputBuffer);


	// Output
	ID3D11UnorderedAccessView* pUAV = m_pHDROutputUAV;
	DEVICE_CONTEXT->CSSetUnorderedAccessViews(0, 1, &pUAV, (UINT*)(&pUAV));

	//Input Target
	//ID3D11ShaderResourceView* pSRV = m_pHDROutputSRV;
	ID3D11ShaderResourceView* pSRV = FindRenderTarget("LightBlend")->m_pSRV;
	DEVICE_CONTEXT->CSSetShaderResources(19, 1, &pSRV);

	//// Shader
	DEVICE_CONTEXT->CSSetShader(m_pAverageLuminanceCS, NULL, 0);
	
	//Compute Shader에 Thread 할당
	DEVICE_CONTEXT->Dispatch(1, 1, 1);
	

	//복사
	DEVICE_CONTEXT->CopyResource(pOutputBuffer, m_pHDROutputBuffer);

	D3D11_MAPPED_SUBRESOURCE tMappedResource = {};
	DEVICE_CONTEXT->Map(pOutputBuffer, NULL, D3D11_MAP_READ, NULL, &tMappedResource);
	float vAverageLuminance;
	memcpy(&vAverageLuminance, tMappedResource.pData, sizeof(float));
	//Vector3 vOriginColor = Vector3(vAverageColor.x, vAverageColor.y, vAverageColor.z);
	//vOriginColor = vOriginColor / ((1280 * 720) - vAverageColor.w); // w는 클리핑 된 픽셀의 갯수
	//float fLuminance = vOriginColor.Dot(Vector3(0.299f, 0.587f, 0.114f)); // 어짜피 여기서는 w계산 안함
	DEVICE_CONTEXT->Unmap(pOutputBuffer, NULL);
	

	// Cleanup
	DEVICE_CONTEXT->CSSetShader(NULL, NULL, 0);	
	memset(&pSRV, 0, sizeof(pSRV));
	DEVICE_CONTEXT->CSSetShaderResources(19, 1, &pSRV);
	memset(&pUAV, 0, sizeof(pUAV));
	DEVICE_CONTEXT->CSSetUnorderedAccessViews(0, 1, &pUAV, (UINT*)(&pUAV));

	pOutputBuffer->Release();

	SAFE_RELEASE(pSampler);

	return vAverageLuminance;
}


bool CRenderManager::CreateComputeShader()
{
	unsigned int iBackBufferSize = DEVICE_RESOLUTION.iHeight * DEVICE_RESOLUTION.iHeight;

	D3D11_BUFFER_DESC tOutputDescription = {};
	tOutputDescription.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	tOutputDescription.StructureByteStride = sizeof(float);
	tOutputDescription.ByteWidth = sizeof(float);
	tOutputDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc = {};
	tUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	tUAVDesc.Buffer.NumElements = iBackBufferSize / (16 * 1024);

	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};
	tSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;

	return true;
}

bool CRenderManager::LoadComputeShader(const wchar_t * pFileName, char * pEntry, const string & strPathKey, ID3DBlob** ppShaderBlob, ID3D11ComputeShader** pTargetShader)
{
	UINT	iFlag = 0;
#ifdef _DEBUG
//	iFlag = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
	iFlag |= D3D10_SHADER_DEBUG;
#endif // _DEBUG

	// 경로를 만들어준다.
	const wchar_t*	pPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);
	wstring	strFullPath;

	if (pPath)
		strFullPath = pPath;

	strFullPath += pFileName;

	ID3DBlob*	pErr = NULL;

 	if (FAILED(D3DCompileFromFile(strFullPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pEntry, "cs_5_0", iFlag, 0, ppShaderBlob, &pErr)))
	{
		MessageBoxA(NULL, (char*)pErr->GetBufferPointer(), "Shader Error", MB_OK);
		return false;
	}


	if (FAILED(DEVICE->CreateComputeShader((*ppShaderBlob)->GetBufferPointer(), (*ppShaderBlob)->GetBufferSize(), NULL, pTargetShader)))
	{
		MessageBoxA(NULL, (char*)pErr->GetBufferPointer(),
			"Shader Error", MB_OK);
		return false;
	}

	return true;	
}

bool CRenderManager::SetPostProcessing(CPostProcessing * pComponent)
{
	m_pPostProcessing = pComponent;
	return true;
}

void CRenderManager::SetDeferredRendering(bool bDeferred)
{
	m_bDeferred = bDeferred;
}

bool CRenderManager::CreateRenderTarget(const string & strName, UINT iWidth, UINT iHeight, UINT iMipLevel, DXGI_FORMAT eFormat, const Vector4 & vClearColor, DXGI_FORMAT eDepthFormat)
{
	CRenderingTarget*	pTarget = FindRenderTarget(strName);
	
	if (pTarget)
		return false;

	pTarget = new CRenderingTarget;

	if (!pTarget->CreateTarget(strName, iWidth, iHeight, iMipLevel, eFormat, vClearColor, eDepthFormat))
	{
		SAFE_DELETE(pTarget);
		return false;
	}

	m_mapRenderTarget.insert(make_pair(strName, pTarget));

	return true;
}

bool CRenderManager::CreateMultiRenderTarget(const string & strMRTName, const vector<string>& vecTarget)
{
	PMRT	pMRT = FindMRT(strMRTName);

	if (pMRT)
		return false;

	pMRT = new MRT;

	for (size_t i = 0; i < vecTarget.size(); ++i)
	{
		CRenderingTarget*	pTarget = FindRenderTarget(vecTarget[i]);

		if (!pTarget)
		{
			SAFE_DELETE(pMRT);
			return false;
		}

		pMRT->vecTarget.push_back(pTarget);
	}

	pMRT->vecOldTarget.resize(vecTarget.size());

	m_mapMRT.insert(make_pair(strMRTName, pMRT));

	return true;
}

CRenderingTarget * CRenderManager::FindRenderTarget(const string & strName)
{
	unordered_map<string, class CRenderingTarget*>::iterator	iter = m_mapRenderTarget.find(strName);

	if (iter == m_mapRenderTarget.end())
		return NULL;

	return iter->second;
}

PMRT CRenderManager::FindMRT(const string & strName)
{
	unordered_map<string, PMRT>::iterator	iter = m_mapMRT.find(strName);

	if (iter == m_mapMRT.end())
		return NULL;

	return iter->second;
}

bool CRenderManager::SetRenderTargetDebugInfo(const string & strName, const Vector3 & vPos, const Vector3 & vScale)
{
	CRenderingTarget*	pTarget = FindRenderTarget(strName);

	if (!pTarget)
		return false;

	pTarget->SetDebugInfo(vPos, vScale);

	return true;
}

void CRenderManager::ClearMRT(const string & strName)
{
	PMRT	pMRT = FindMRT(strName);

	if (!pMRT)
		return;

	for (size_t i = 0; i < pMRT->vecTarget.size(); ++i)
	{
		pMRT->vecTarget[i]->ClearTarget();
	}
}

void CRenderManager::SetMRT(const string & strName)
{
	PMRT	pMRT = FindMRT(strName);

	if (!pMRT)
		return;

	vector<ID3D11RenderTargetView*>	vecTarget;

	for (size_t i = 0; i < pMRT->vecTarget.size(); ++i)
	{
		vecTarget.push_back(pMRT->vecTarget[i]->GetRenderTargetView());
	}

	ID3D11DepthStencilView*	pDepth = NULL;

	DEVICE_CONTEXT->OMGetRenderTargets(vecTarget.size(), &pMRT->vecOldTarget[0],
		&pMRT->pOldDepth);

	pDepth = pMRT->pOldDepth;

	DEVICE_CONTEXT->OMSetRenderTargets(vecTarget.size(), &vecTarget[0], pDepth);
}

void CRenderManager::ResetMRT(const string & strName)
{
	PMRT	pMRT = FindMRT(strName);

	if (!pMRT)
		return;

	DEVICE_CONTEXT->OMSetRenderTargets(pMRT->vecOldTarget.size(), &pMRT->vecOldTarget[0],
		pMRT->pOldDepth);

	SAFE_RELEASE(pMRT->pOldDepth);
	for (size_t i = 0; i < pMRT->vecOldTarget.size(); ++i)
	{
		SAFE_RELEASE(pMRT->vecOldTarget[i]);
	}
}

void CRenderManager::AddRenderObject(CGameObject * pObj)
{
	if (pObj->HasComponent<CSpriteRenderer>() || pObj->HasComponent<CText>())
	{
		AddRenderGroup(&m_tRenderGroup[RGT_UI], pObj);
	}

	else if (pObj->HasComponent<CDecal>())
	{
		AddRenderGroup(&m_tRenderGroup[RGT_DECAL], pObj);
	}

	else if (pObj->HasComponent<CDistortion>())
	{
		AddRenderGroup(&m_tRenderGroup[RGT_PREVBACBUFFER], pObj);
	}

	else if (pObj->UseTransparency())
	{
		AddRenderGroup(&m_tRenderGroup[RGT_ALPHA], pObj);
	}
	
	else
	{
		AddRenderGroup(&m_tRenderGroup[RGT_NORMAL], pObj);
	}
}

void CRenderManager::Render(float fTime)
{
	if (m_bDeferred)
		RenderDeferred(fTime);

	else
		RenderForward(fTime);
}

void CRenderManager::RenderForward(float fTime)
{
	for (int i = 0; i < RGT_END; ++i)
	{
		if (m_tRenderGroup[i].iSize >= 2)
		{
			switch (i)
			{
			case RGT_NORMAL:
				sort(m_tRenderGroup[i].pObjList,
					&m_tRenderGroup[i].pObjList[m_tRenderGroup[i].iSize - 1],
					CRenderManager::SortZ);
				break;
			case RGT_UI:
				sort(m_tRenderGroup[i].pObjList,
					&m_tRenderGroup[i].pObjList[m_tRenderGroup[i].iSize - 1],
					CRenderManager::SortZOrder);
				break;
			default:
				sort(m_tRenderGroup[i].pObjList,
					&m_tRenderGroup[i].pObjList[m_tRenderGroup[i].iSize - 1],
					CRenderManager::SortAlpha);
				break;
			}
		}

		for (int j = 0; j < m_tRenderGroup[i].iSize; ++j)
		{
			m_tRenderGroup[i].pObjList[j]->Render(fTime);
		}

		m_tRenderGroup[i].iSize = 0;
	}
}

void CRenderManager::RenderDeferred(float fTime)
{
	// 오브젝트를 사용 목적에 따라 정렬한다.
	for (int i = 0; i < RGT_END; ++i)
	{
		if (m_tRenderGroup[i].iSize >= 2)
		{
			switch (i)
			{
			case RGT_NORMAL:
				sort(m_tRenderGroup[i].pObjList,
					&m_tRenderGroup[i].pObjList[m_tRenderGroup[i].iSize - 1],
					CRenderManager::SortZ);
				break;
			case RGT_UI:
				sort(m_tRenderGroup[i].pObjList,
					&m_tRenderGroup[i].pObjList[m_tRenderGroup[i].iSize - 1],
					CRenderManager::SortZOrder);
				break;
			default:
				sort(m_tRenderGroup[i].pObjList,
					&m_tRenderGroup[i].pObjList[m_tRenderGroup[i].iSize - 1],
					CRenderManager::SortAlpha);
				break;
			}
		}
	}

	CScene* pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
	CCamera* pCamera = pScene->GetMainCamera();
	m_bShadowEnable = pCamera->GetShadowState();
	SAFE_RELEASE(pCamera);
	SAFE_RELEASE(pScene);
	
	// 그림자 맵 생성 단계
	if (m_bShadowEnable)
		RenderShadowMap(fTime);

	// Motion Blur 용 Target
	//if (!m_bCameraMotionBlur)
	//	RenderMotionBlur(fTime);
	
	// GBuffer를 만들어준다.
	RenderGBuffer(fTime);

	// GBuffer에 Decal을 덧씌워준다.
	RenderDecal(fTime);

	// 조명 누적버퍼를 만들어준다.
	RenderLightAcc(fTime);

	// 최종 조명처리를 한 화면 버퍼를 만들어낸다.
	// 그림자 처리도 같이 해줍니다.
	// 그후, Alpha 오브젝트와 SkyBox도 함께 출력해줍니다
	RenderLightBlend(fTime);
	
	// HDR 처리 단계
	RenderHDR(fTime);

	// 최종 조명처리된 화면을 최종타겟에 출력한다.
	//Post-Processing도 같이 해준다.
	RenderBlendTarget(fTime);

	// BackBuffer 에 그려준다.
	RenderFinalTarget(fTime);

	if (GET_SINGLE(CCore)->GetEngineState())
	{
		GET_SINGLE(CDebug)->Render(fTime);
	}


#pragma region BackBuffer

	CRenderingTarget*	pLightFinalTarget = FindRenderTarget("FinalColorWithPostProcess");
	CRenderingTarget*	pDepthTarget = FindRenderTarget("Depth");

	for (int j = 0; j < m_tRenderGroup[RGT_PREVBACBUFFER].iSize; ++j)
	{
		pLightFinalTarget->SetShader(15);
		pDepthTarget->SetShader(17);

		m_tRenderGroup[RGT_PREVBACBUFFER].pObjList[j]->Render(fTime);

		pLightFinalTarget->ResetShader(15);
		pDepthTarget->ResetShader(17);
	}

	
	for (int i = RGT_UI; i < RGT_END; ++i)
	{
		for (int j = 0; j < m_tRenderGroup[i].iSize; ++j)
		{
			pDepthTarget->SetShader(17);
			m_tRenderGroup[i].pObjList[j]->Render(fTime);
			pDepthTarget->ResetShader(17);
		}
	}

#pragma endregion


	for (int i = 0; i < RGT_END; ++i)
	{
		m_tRenderGroup[i].iSize = 0;
	}

	// 렌더타겟 디버깅 출력
	
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		unordered_map<string, CRenderingTarget*>::iterator	iter;
		unordered_map<string, CRenderingTarget*>::iterator	iterEnd = m_mapRenderTarget.end();

		for (iter = m_mapRenderTarget.begin(); iter != iterEnd; ++iter)
		{
			iter->second->Render(fTime);
		}
	}
}

void CRenderManager::RenderGBuffer(float fTime)
{
	// MRT를 설정한다.
	ClearMRT("GBuffer");
	SetMRT("GBuffer");

	for (int j = 0; j < m_tRenderGroup[RGT_NORMAL].iSize; ++j)
	{
		m_tRenderGroup[RGT_NORMAL].pObjList[j]->Render(fTime);
	}

	ResetMRT("GBuffer");
}

void CRenderManager::RenderDecal(float fTime)
{
	// MRT를 설정한다.
	SetMRT("Decal");

	PMRT	pMRT = FindMRT("GBuffer");

	CSampler*	pSampler = GET_SINGLE(CResourcesManager)->FindSampler(SAMPLER_POINT);
	pSampler->SetSampler(11);
	pMRT->vecTarget[2]->SetShader(13);
	pMRT->vecTarget[4]->SetShader(15);
	pMRT->vecTarget[5]->SetShader(16);

	for (int j = 0; j < m_tRenderGroup[RGT_DECAL].iSize; ++j)
	{
		m_tRenderGroup[RGT_DECAL].pObjList[j]->Render(fTime);
	}

	m_tRenderGroup[RGT_DECAL].iSize = 0;

	SAFE_RELEASE(pSampler);

	pMRT->vecTarget[2]->ResetShader(13);
	pMRT->vecTarget[4]->ResetShader(15);
	pMRT->vecTarget[5]->ResetShader(16);

	ResetMRT("Decal");
}

void CRenderManager::RenderLightAcc(float fTime)
{
	// MRT를 설정한다.
	ClearMRT("LightAcc");
	SetMRT("LightAcc");

	PMRT pMRT = FindMRT("GBuffer");

	CBlendState* pLightBlendState = (CBlendState*)FindRenderState("LightAcc");
	CDepthStencilState* pLightDepthDisable = (CDepthStencilState*)FindRenderState(DEPTH_DISABLE);
	CSampler* pLightSampler = GET_SINGLE(CResourcesManager)->FindSampler(SAMPLER_POINT);

	TRANSFORMCBUFFER tBuffer = {};

	CScene* pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
	CCamera* pCamera = pScene->GetMainCamera();
	SAFE_RELEASE(pScene);

	tBuffer.matWorld = XMMatrixIdentity();
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProjection = pCamera->GetPerspectiveProjectionMatrix();
	tBuffer.matWV = tBuffer.matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
	tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
	tBuffer.matWP = tBuffer.matWorld * tBuffer.matProjection;
	tBuffer.matInvProjection = Matrix::Inverse(tBuffer.matProjection);
	tBuffer.matInvVP = Matrix::Inverse(tBuffer.matVP);
	tBuffer.vPivot = Vector3::Zero;
	tBuffer.iRenderMode = 1;
	tBuffer.vLength = Vector3::Zero;

	SAFE_RELEASE(pCamera);

	tBuffer.matWorld = tBuffer.matWorld.Transpose();
	tBuffer.matView = tBuffer.matView.Transpose();
	tBuffer.matProjection = tBuffer.matProjection.Transpose();
	tBuffer.matWV = tBuffer.matWV.Transpose();
	tBuffer.matWVP = tBuffer.matWVP.Transpose();
	tBuffer.matVP = tBuffer.matVP.Transpose();
	tBuffer.matWP = tBuffer.matWP.Transpose();
	tBuffer.matInvProjection = tBuffer.matInvProjection.Transpose();
	tBuffer.matInvVP = tBuffer.matInvVP.Transpose();

	for (int i = 0; i < m_tLightSection.iSize; ++i)
	{
		pLightSampler->SetSampler(11);
		pMRT->vecTarget[1]->SetShader(12);
		pMRT->vecTarget[2]->SetShader(13);
		pMRT->vecTarget[3]->SetShader(14);

		pLightBlendState->SetState();

		m_tLightSection.pLightList[i]->SetLight();

		// Transform 정보를 Shader 상수버퍼로 넘겨준다.
		GET_SINGLE(CShaderManager)->UpdateConstantBuffer(TRANSFORM_CBUFFER, &tBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);

		switch (m_tLightSection.pLightList[i]->GetLightType())
		{
		case LT_DIRECTIONAL:
		{
			pLightDepthDisable->SetState();
			m_pLightAccDirShader->SetShader();

			// NULL Buffer로 전체 화면크기의 사각형을 출력한다.
			DEVICE_CONTEXT->IASetInputLayout(NULL);

			UINT	iOffset = 0;
			DEVICE_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			DEVICE_CONTEXT->IASetVertexBuffers(0, 0, NULL, 0, &iOffset);
			DEVICE_CONTEXT->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
			DEVICE_CONTEXT->Draw(4, 0);

			pLightDepthDisable->ResetState();
		}
		break;
		case LT_SPOT:
			break;
		case LT_POINT:
			RenderLightPoint(fTime, (CPointLight*)m_tLightSection.pLightList[i]);
			break;
		}

		pLightBlendState->ResetState();

		pMRT->vecTarget[1]->ResetShader(12);
		pMRT->vecTarget[2]->ResetShader(13);
		pMRT->vecTarget[3]->ResetShader(14);
	}

	SAFE_RELEASE(pLightDepthDisable);
	SAFE_RELEASE(pLightBlendState);
	SAFE_RELEASE(pLightSampler);

	ResetMRT("LightAcc");
}

void CRenderManager::RenderLightBlend(float fTime)
{
	CRenderingTarget*	pTarget = FindRenderTarget("LightBlend");

	pTarget->ClearTarget();
	pTarget->SetTarget();

	// Shadow 계산에 사용할 Shadow, Transform 상수버퍼를 설정해 넘겨줍니다
	UpdateShadowCBuffer(fTime);

	CDepthStencilState*	pDepthDisable = (CDepthStencilState*)FindRenderState(DEPTH_DISABLE);

	CSampler*	pSampler = GET_SINGLE(CResourcesManager)->FindSampler(SAMPLER_POINT);
	PMRT	pGBuffer = FindMRT("GBuffer");
	PMRT	pLightAcc = FindMRT("LightAcc");
	CRenderingTarget*	pShadowMap = FindRenderTarget("ShadowMap");

	pSampler->SetSampler(11);
	pGBuffer->vecTarget[0]->SetShader(11);
	pGBuffer->vecTarget[1]->SetShader(12);
	pGBuffer->vecTarget[2]->SetShader(13);
	pGBuffer->vecTarget[3]->SetShader(14);
	pLightAcc->vecTarget[0]->SetShader(15);
	pLightAcc->vecTarget[1]->SetShader(16);
	pShadowMap->SetShader(18);

	pDepthDisable->SetState();

	m_pLightBlendShader->SetShader();

	// Fog 를 계산하기 위해 Fog와 관련된 상수버퍼만 넘깁니다.
	POSTPROCESSCBUFFER tBuffer = {};
	if (m_pPostProcessing)
	{
		uint32_t iFlags = m_pPostProcessing->m_iPostProcessFlags;
		tBuffer.iFlags = iFlags;
		if (iFlags & PT_FOG)
		{
			tBuffer.fFogMinDistance = m_pPostProcessing->m_fFogMinDistance;
			tBuffer.fFogMaxDistance = m_pPostProcessing->m_fFogMaxDistance;
			tBuffer.vFogColor = m_pPostProcessing->m_vFogColor;
		}
	}
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(POSTPROCESS_CBUFFER, &tBuffer, CBT_PIXEL);

	// NULL Buffer로 전체 화면크기의 사각형을 출력한다.
	DEVICE_CONTEXT->IASetInputLayout(NULL);

	UINT iOffset = 0;
	DEVICE_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DEVICE_CONTEXT->IASetVertexBuffers(0, 0, NULL, 0, &iOffset);
	DEVICE_CONTEXT->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	DEVICE_CONTEXT->Draw(4, 0);


	pDepthDisable->ResetState();
	pGBuffer->vecTarget[0]->ResetShader(11);
	pGBuffer->vecTarget[1]->ResetShader(12);
	pGBuffer->vecTarget[2]->ResetShader(13);
	pGBuffer->vecTarget[3]->ResetShader(14);
	pLightAcc->vecTarget[0]->ResetShader(15);
	pLightAcc->vecTarget[1]->ResetShader(16);
	pShadowMap->ResetShader(18);

	SAFE_RELEASE(pDepthDisable);
	SAFE_RELEASE(pSampler);
	
	// 스카이박스 픽셀정보가 Post Processing 단계에서 필요하기 때문에 LightBlend 타겟에 넣어주었습니다
	CScene* pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
	CGameObject* pSkybox = pScene->GetSkybox();
	if (pSkybox)
	{
		pSkybox->Render(fTime);
	}
	SAFE_RELEASE(pSkybox);
	SAFE_RELEASE(pScene);

	// Light 연산이 끝난뒤 타겟을 바꾸지 않고 알파 오브젝트를 덮어 그려줍니다
	for (int j = 0; j < m_tRenderGroup[RGT_ALPHA].iSize; ++j)
	{
		pGBuffer->vecTarget[2]->SetShader(17);

		m_tRenderGroup[RGT_ALPHA].pObjList[j]->Render(fTime);

		pGBuffer->vecTarget[2]->ResetShader(17);
	}

	pTarget->ResetTarget();
}

void CRenderManager::RenderBlendTarget(float fTime)
{
	CRenderingTarget*	pTarget = FindRenderTarget("FinalColorWithPostProcess");

	pTarget->ClearTarget();
	pTarget->SetTarget();

	CDepthStencilState*	pDepthDisable = (CDepthStencilState*)FindRenderState(DEPTH_DISABLE);

	CSampler*	pSampler = GET_SINGLE(CResourcesManager)->FindSampler(SAMPLER_LINEAR);

	CSampler* pHDRSampler = GET_SINGLE(CResourcesManager)->FindSampler("HDRSampler");

	pHDRSampler->SetSampler(12);

	CRenderingTarget*	pMotionTarget = FindRenderTarget("MotionBlur");

	CRenderingTarget*	pLightBlendTarget = FindRenderTarget("LightBlend");

	PMRT	pGBuffer = FindMRT("GBuffer");
	
	CRenderingTarget*	pBlurredHVTarget = FindRenderTarget("BlurredHVTarget");

	CRenderingTarget*	pAvgLumTarget = FindRenderTarget("AvgLuminanceTarget");

	CRenderingTarget*	pPrevAvgLumTarget = FindRenderTarget("PrevAvgLuminanceTarget");

	pSampler->SetSampler(11);
	pHDRSampler->SetSampler(12);
	pLightBlendTarget->SetShader(11);
	pGBuffer->vecTarget[1]->SetShader(12);
	pGBuffer->vecTarget[2]->SetShader(13);
	pMotionTarget->SetShader(17);

	pBlurredHVTarget->SetShader(19);
	pAvgLumTarget->SetShader(20);
	pPrevAvgLumTarget->SetShader(21);


	pDepthDisable->SetState();

	UpdateShadowCBuffer(fTime);

	//Post-Processing 과 관련된 상수버퍼를 설정해준다.
	SetPostProcessingProperties(fTime);

	m_pRenderBlendShader->SetShader();
	
	// NULL Buffer로 전체 화면크기의 사각형을 출력한다.
	DEVICE_CONTEXT->IASetInputLayout(NULL);



	UINT iOffset = 0;
	DEVICE_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DEVICE_CONTEXT->IASetVertexBuffers(0, 0, NULL, 0, &iOffset);
	DEVICE_CONTEXT->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	DEVICE_CONTEXT->Draw(4, 0);


	pGBuffer->vecTarget[2]->ResetShader(13);
	pGBuffer->vecTarget[1]->ResetShader(12);
	pLightBlendTarget->ResetShader(11);
	pMotionTarget->ResetShader(17);

	pBlurredHVTarget->ResetShader(19);
	pAvgLumTarget->ResetShader(20);
	pPrevAvgLumTarget->ResetShader(21);

	pDepthDisable->ResetState();

	SAFE_RELEASE(pDepthDisable);
	SAFE_RELEASE(pSampler);

	SAFE_RELEASE(pHDRSampler);

	pTarget->ResetTarget();
}

void CRenderManager::RenderFinalTarget(float fTime)
{
	CDepthStencilState*	pDepthDisable = (CDepthStencilState*)FindRenderState(DEPTH_DISABLE);

	CSampler*	pSampler = GET_SINGLE(CResourcesManager)->FindSampler(SAMPLER_LINEAR);

	CRenderingTarget*	pLightFinalTarget = FindRenderTarget("FinalColorWithPostProcess");

	pSampler->SetSampler(11);
	pLightFinalTarget->SetShader(11);

	pDepthDisable->SetState();

	m_pRenderPrevBackBufferShader->SetShader();

	// NULL Buffer로 전체 화면크기의 사각형을 출력한다.
	DEVICE_CONTEXT->IASetInputLayout(NULL);

	UINT iOffset = 0;
	DEVICE_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DEVICE_CONTEXT->IASetVertexBuffers(0, 0, NULL, 0, &iOffset);
	DEVICE_CONTEXT->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	DEVICE_CONTEXT->Draw(4, 0);

	pLightFinalTarget->ResetShader(11);
	pDepthDisable->ResetState();

	SAFE_RELEASE(pDepthDisable);
	SAFE_RELEASE(pSampler);
}

void CRenderManager::RenderMotionBlur(float fTime)
{
	//D3D11_VIEWPORT	tVP = {};
	//tVP.MaxDepth = 1.f;
	//tVP.Width = DEVICE_RESOLUTION.iWidth;
	//tVP.Height = DEVICE_RESOLUTION.iHeight;
	//
	//D3D11_VIEWPORT	tOldVP = {};
	//UINT	iViewPortCount = 1;
	//DEVICE_CONTEXT->RSGetViewports(&iViewPortCount, &tOldVP);
	//DEVICE_CONTEXT->RSSetViewports(1, &tVP);

	CRenderingTarget*	pMotionBlurMap = FindRenderTarget("MotionBlur");

	pMotionBlurMap->ClearTarget();
	pMotionBlurMap->SetTarget();

	for (int j = 0; j < m_tRenderGroup[RGT_NORMAL].iSize; ++j)
	{
		CMeshRenderer* pRender = m_tRenderGroup[RGT_NORMAL].pObjList[j]->GetComponent<CMeshRenderer>();
		if (pRender)
		{
			pRender->RenderMotionBlurMap();
			SAFE_RELEASE(pRender);
		}
	}

	pMotionBlurMap->ResetTarget();

	//DEVICE_CONTEXT->RSSetViewports(1, &tOldVP);
}

void CRenderManager::RenderShadowMap(float fTime)
{
	CScene* pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
	CCamera* pCamera = pScene->GetMainCamera();
	Vector2 vShadowRS = pCamera->GetShadowResolution();
	SAFE_RELEASE(pCamera);
	SAFE_RELEASE(pScene);
	// 그림자맵 크기에 맞춰 뷰포트를 변경합니다
	D3D11_VIEWPORT	tVP = {};
	tVP.MaxDepth = 1.f;
	tVP.Width = vShadowRS.x;
	tVP.Height = vShadowRS.y;

	D3D11_VIEWPORT	tOldVP = {};
	UINT	iViewPortCount = 1;
	DEVICE_CONTEXT->RSGetViewports(&iViewPortCount, &tOldVP);
	DEVICE_CONTEXT->RSSetViewports(1, &tVP);

	CRenderingTarget*	pShadowMap = FindRenderTarget("ShadowMap");
	pShadowMap->ClearTarget();
	pShadowMap->SetTarget();

	for (int j = 0; j < m_tRenderGroup[RGT_NORMAL].iSize; ++j)
	{
		m_tRenderGroup[RGT_NORMAL].pObjList[j]->RenderShadowMap(fTime);
	}

	pShadowMap->ResetTarget();
	DEVICE_CONTEXT->RSSetViewports(1, &tOldVP);
}

void CRenderManager::UpdateShadowCBuffer(float fTime)
{
	// 상수버퍼 설정
	CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
	CCamera*	pCamera = pScene->GetMainCamera();
	SAFE_RELEASE(pScene);

	SHADOWRENDERING	tShadowBuffer = {};
	tShadowBuffer.iShadowEnable = m_bShadowEnable ? 1 : 0;

	/*if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_fShadowBias -= fTime * 0.1f;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_fShadowBias += fTime * 0.1f;
	}*/

	tShadowBuffer.fShadowBias = m_fShadowBias;
	tShadowBuffer.vShadowLightPos = pCamera->GetLightPos();
	tShadowBuffer.vShadowResolution = pCamera->GetShadowResolution();

	GET_SINGLE(CShaderManager)->UpdateConstantBuffer("ShadowRendering",
		&tShadowBuffer, CBT_PIXEL);


	TRANSFORMCBUFFER	tBuffer;

	tBuffer.matWorld = Matrix::IdentityMatrix();
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProjection = pCamera->GetPerspectiveProjectionMatrix();
	tBuffer.matWV = tBuffer.matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
	tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
	tBuffer.matWP = tBuffer.matWorld * tBuffer.matProjection;
	tBuffer.matInvProjection = Matrix::Inverse(tBuffer.matProjection);
	tBuffer.matInvVP = Matrix::Inverse(tBuffer.matVP);
	tBuffer.matInvWV = Matrix::Inverse(tBuffer.matWV);
	tBuffer.matInvWVP = Matrix::Inverse(tBuffer.matWVP);

	tBuffer.matShadowView = pCamera->GetShadowViewMatrix();
	tBuffer.matShadowProj = pCamera->GetShadowProjMatrix();
	tBuffer.matShadowVP = tBuffer.matShadowView * tBuffer.matShadowProj;
	tBuffer.matShadowWVP = tBuffer.matWorld * tBuffer.matShadowVP;

	SAFE_RELEASE(pCamera);

	tBuffer.matWorld = tBuffer.matWorld.Transpose();
	tBuffer.matView = tBuffer.matView.Transpose();
	tBuffer.matProjection = tBuffer.matProjection.Transpose();
	tBuffer.matWV = tBuffer.matWV.Transpose();
	tBuffer.matWVP = tBuffer.matWVP.Transpose();
	tBuffer.matVP = tBuffer.matVP.Transpose();
	tBuffer.matWP = tBuffer.matWP.Transpose();
	tBuffer.matInvProjection = tBuffer.matInvProjection.Transpose();
	tBuffer.matInvVP = tBuffer.matInvVP.Transpose();
	tBuffer.matInvWV = tBuffer.matInvWV.Transpose();
	tBuffer.matInvWVP = tBuffer.matInvWVP.Transpose();
	tBuffer.matShadowView = tBuffer.matShadowView.Transpose();
	tBuffer.matShadowProj = tBuffer.matShadowProj.Transpose();
	tBuffer.matShadowVP = tBuffer.matShadowVP.Transpose();
	tBuffer.matShadowWVP = tBuffer.matShadowWVP.Transpose();

	// Transform 정보를 Shader 상수버퍼로 넘겨준다.
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(TRANSFORM_CBUFFER,
		&tBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);
}

void CRenderManager::RenderLightPoint(float fTime, CPointLight * pLight)
{
	TRANSFORMCBUFFER	tBuffer;

	CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();

	// Camera를 얻어온다.
	CCamera*	pCamera = pScene->GetMainCamera();

	SAFE_RELEASE(pScene);

	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProjection = pCamera->GetPerspectiveProjectionMatrix();

	CTransform*	pTr = pLight->GetTransform();

	Vector3	vPos = pTr->GetWorldPosition();
	float	fRange = pLight->GetRange() * 2.f;

	SAFE_RELEASE(pTr);

	Matrix	matScale, matPos;
	matScale = Matrix::Scaling(Vector3(fRange, fRange, fRange));
	matPos = Matrix::Translation(vPos);

	tBuffer.matWorld = matScale * matPos;
	tBuffer.matWV = tBuffer.matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
	tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
	tBuffer.matWP = tBuffer.matWorld * tBuffer.matProjection;
	tBuffer.matInvProjection = Matrix::Inverse(tBuffer.matProjection);
	tBuffer.matInvVP = Matrix::Inverse(tBuffer.matVP);
	tBuffer.vPivot = Vector3::Zero;
	tBuffer.iRenderMode = 1;
	tBuffer.vLength = Vector3::Zero;

	tBuffer.matWorld = tBuffer.matWorld.Transpose();
	tBuffer.matView = tBuffer.matView.Transpose();
	tBuffer.matProjection = tBuffer.matProjection.Transpose();
	tBuffer.matWV = tBuffer.matWV.Transpose();
	tBuffer.matWVP = tBuffer.matWVP.Transpose();
	tBuffer.matVP = tBuffer.matVP.Transpose();
	tBuffer.matWP = tBuffer.matWP.Transpose();
	tBuffer.matInvProjection = tBuffer.matInvProjection.Transpose();
	tBuffer.matInvVP = tBuffer.matInvVP.Transpose();

	// Transform 정보를 Shader 상수버퍼로 넘겨준다.
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(TRANSFORM_CBUFFER,
		&tBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);

	SAFE_RELEASE(pCamera);

	CDepthStencilState*	pDepthState = (CDepthStencilState*)FindRenderState("LightBack");
	CRasterizerState*	pCullNone = (CRasterizerState*)FindRenderState(CULLNONE);

	pDepthState->SetState();
	pCullNone->SetState();

	// NULL Buffer로 전체 화면크기의 사각형을 출력한다.
	DEVICE_CONTEXT->IASetInputLayout(m_pPointLightInputLayout);

	m_pLightAccPointShader->SetShader();

	m_pPointLightMesh->Render();

	pCullNone->ResetState();
	pDepthState->ResetState();

	SAFE_RELEASE(pCullNone);
	SAFE_RELEASE(pDepthState);
}

void CRenderManager::AddRenderGroup(PRENDERGROUP pGroup, CGameObject * pObj)
{
	if (pGroup->iSize == pGroup->iCapacity)
	{
		pGroup->iCapacity *= 2;

		CGameObject**	pList = new CGameObject*[pGroup->iCapacity];
		memcpy(pList, pGroup->pObjList, sizeof(CGameObject*) * pGroup->iSize);

		SAFE_DELETE_ARRAY(pGroup->pObjList);

		pGroup->pObjList = pList;
	}

	pGroup->pObjList[pGroup->iSize] = pObj;
	++pGroup->iSize;
}

bool CRenderManager::SortZ(CGameObject * pSrc, CGameObject * pDest)
{
	CTransform*	pSrcTr = pSrc->GetTransform();
	CTransform*	pDestTr = pDest->GetTransform();

	Vector3	vSrcPos = pSrcTr->GetWorldPosition();
	Vector3	vDestPos = pDestTr->GetWorldPosition();

	CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();

	CCamera*	pCamera = pScene->GetMainCamera();

	vSrcPos = vSrcPos.TransformCoord(pCamera->GetViewMatrix());
	vDestPos = vDestPos.TransformCoord(pCamera->GetViewMatrix());

	SAFE_RELEASE(pCamera);

	SAFE_RELEASE(pScene);

	SAFE_RELEASE(pSrcTr);
	SAFE_RELEASE(pDestTr);

	return vSrcPos.z < vDestPos.z;
}

bool CRenderManager::SortAlpha(CGameObject * pSrc, CGameObject * pDest)
{
	CTransform*	pSrcTr = pSrc->GetTransform();
	CTransform*	pDestTr = pDest->GetTransform();

	Vector3	vSrcPos = pSrcTr->GetWorldPosition();
	Vector3	vDestPos = pDestTr->GetWorldPosition();

	CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();

	CCamera*	pCamera = pScene->GetMainCamera();

	vSrcPos = vSrcPos.TransformCoord(pCamera->GetViewMatrix());
	vDestPos = vDestPos.TransformCoord(pCamera->GetViewMatrix());

	SAFE_RELEASE(pCamera);

	SAFE_RELEASE(pScene);

	SAFE_RELEASE(pSrcTr);
	SAFE_RELEASE(pDestTr);

	return vSrcPos.z > vDestPos.z;
}


//UI에서 사용
bool CRenderManager::SortZOrder(CGameObject * pSrc, CGameObject * pDest)
{
	CSpriteRenderer*	pSrcUI = pSrc->GetComponent<CSpriteRenderer>();
	CSpriteRenderer*	pDestUI = pDest->GetComponent<CSpriteRenderer>();

	int iSrcZ = pSrcUI->GetZOrder();
	int iDestZ = pDestUI->GetZOrder();

	SAFE_RELEASE(pDestUI);
	SAFE_RELEASE(pSrcUI);

	return iSrcZ < iDestZ;
}


void CRenderManager::RenderHDR(float fTime)
{
	if (!m_pPostProcessing)
		return;

	uint32_t iFlags = m_pPostProcessing->m_iPostProcessFlags;

	if (!(iFlags & PT_HDR))
		return;

	D3D11_VIEWPORT viewport;
	UINT numViewPorts = 1;
	// 초기 설정된 뷰포트 정보를 받아옵니다
	DEVICE_CONTEXT->RSGetViewports(&numViewPorts, &viewport);

	// HDR 연산 전용 샘플러를 가져옵니다
	CSampler* pSampler = GET_SINGLE(CResourcesManager)->FindSampler("HDRSampler");
	// HDR 연산 전용 샘플러를 설정해줍니다
	pSampler->SetSampler(12);

	ID3D11RenderTargetView* pTargetOldView;
	ID3D11DepthStencilView* pDepthOldView;
	// 기존에 설정되어 있는 BackBuffer 렌더타겟을 가져옵니다 
	DEVICE_CONTEXT->OMGetRenderTargets(1, &pTargetOldView, &pDepthOldView);

	unsigned int width = 512;
	unsigned int height = 512;

	D3D11_VIEWPORT quadViewPort = {};
	quadViewPort.Height = (float)height;
	quadViewPort.Width = (float)width;
	quadViewPort.MaxDepth = 1.0f;
	quadViewPort.MinDepth = 0.0f;
	quadViewPort.TopLeftX = 0;
	quadViewPort.TopLeftY = 0;
	// 설정해둔 다운스케일 렌더타겟에 맞춘 크기로 뷰포트를 설정합니다
	DEVICE_CONTEXT->RSSetViewports(1, &quadViewPort);

	MIPLEVELCBUFFER tMipLevelBuffer = {};
	tMipLevelBuffer.mipLevel0 = 0;
	tMipLevelBuffer.mipLevel1 = 0;

	GET_SINGLE(CShaderManager)->UpdateConstantBuffer("HDRCbuffer", &tMipLevelBuffer, CBT_PIXEL);

	// 빛 연산이 완전히 수행된 렌더타겟을 가져옵니다
	CRenderingTarget*	pMainTarget = FindRenderTarget("LightBlend");

	CRenderingTarget*	pAvgLumTarget = FindRenderTarget("AvgLuminanceTarget");
	ID3D11RenderTargetView* pAvgLumView = pAvgLumTarget->GetRenderTargetView();
	{
		CRenderingTarget*	pLumTarget = FindRenderTarget("LuminanceTarget");
		ID3D11RenderTargetView* pLumView = pLumTarget->GetRenderTargetView();

		// 메인타겟의 rgb값을 넘겨줘 각 픽셀의 휘도값을 저장해 둡니다
		pLumTarget->ClearTarget();
		DEVICE_CONTEXT->OMSetRenderTargets(1, &pLumView, NULL);
		pMainTarget->SetShader(0);
		CShader* pShader = GET_SINGLE(CShaderManager)->FindShader("CalcLuminance");
		pShader->SetShader();
		DrawFullScreenQuad();
		SAFE_RELEASE(pShader);
		pMainTarget->ResetShader(0);

		quadViewPort.Height = 1.f;
		quadViewPort.Width = 1.f;
		DEVICE_CONTEXT->RSSetViewports(1, &quadViewPort);

		// 클리어 해주기전에 이전 단계에서 평균휘도를 넣어줘보장...
		//if (m_bAvgLumDeferred)
		//{
		//m_bAvgLumDeferred = false;
		CRenderingTarget*	pPrevAvgLumTarget = FindRenderTarget("PrevAvgLuminanceTarget");
		ID3D11RenderTargetView* pPrevTargetView = pPrevAvgLumTarget->GetRenderTargetView();
		pPrevAvgLumTarget->ClearTarget();
		DEVICE_CONTEXT->OMSetRenderTargets(1, &pPrevTargetView, NULL);
		pAvgLumTarget->SetShader(0);
		pShader = GET_SINGLE(CShaderManager)->FindShader("PrevAvgLuminance");
		pShader->SetShader();
		DrawFullScreenQuad();
		pAvgLumTarget->ResetShader(0);
		SAFE_RELEASE(pShader);

		/*}
		else
		{
		m_fAvgLumDeferredTime += fTime;
		if (m_fAvgLumDeferredTime >= 1.f)
		{
		m_fAvgLumDeferredTime -= -1.f;
		m_bAvgLumDeferred = true;
		}
		}*/

		quadViewPort.Height = 16.f;
		quadViewPort.Width = 16.f;
		DEVICE_CONTEXT->RSSetViewports(1, &quadViewPort);

		// 윗단계에서 계산되어진 픽셀의 휘도값의 평균을 구합니다
		pAvgLumTarget->ClearTarget();
		DEVICE_CONTEXT->OMSetRenderTargets(1, &pAvgLumView, NULL);
		pLumTarget->GenerateMips();
		//DEVICE_CONTEXT->GenerateMips(pLumSRV);
		pLumTarget->SetShader(0);
		pShader = GET_SINGLE(CShaderManager)->FindShader("AvgLuminance");
		pShader->SetShader();
		DrawFullScreenQuad();
		SAFE_RELEASE(pShader);
		pLumTarget->ResetShader(0);
	}

	quadViewPort.Height = (float)(DEVICE_RESOLUTION.iHeight / 2);
	quadViewPort.Width = (float)(DEVICE_RESOLUTION.iWidth / 2);
	DEVICE_CONTEXT->RSSetViewports(1, &quadViewPort);

	CRenderingTarget*	pBrightTarget = FindRenderTarget("BrightTarget");
	ID3D11RenderTargetView* pBrightTargetView = pBrightTarget->GetRenderTargetView();
	ID3D11ShaderResourceView* pBrightSRV = pBrightTarget->GetShaderResourceView();

	{
		// 메인타겟의 rgb값을 넘겨주어 밝기 정도를 측정합니다. 
		pBrightTarget->ClearTarget();
		DEVICE_CONTEXT->OMSetRenderTargets(1, &pBrightTargetView, NULL);
		pMainTarget->SetShader(0);
		CShader* pShader = GET_SINGLE(CShaderManager)->FindShader("Bright");
		pShader->SetShader();
		DrawFullScreenQuad();
		SAFE_RELEASE(pShader);
		pMainTarget->ResetShader(0);
	}

	// 작은 크기 ~  큰 크기의 텍스쳐 순서로 반복하면서 최종 512 x 512 사이즈 타겟에 Blur 효과가 적용된 텍스쳐를 만듭니다
	ID3D11RenderTargetView* pNULLRT[] = { NULL };
	DEVICE_CONTEXT->OMSetRenderTargets(1, pNULLRT, NULL);
	pBrightTarget->GenerateMips();

	int mipLevel0 = 3;
	int mipLevel1 = 4;
	width = DEVICE_RESOLUTION.iWidth >> (mipLevel0 + 1);
	height = DEVICE_RESOLUTION.iHeight >> (mipLevel0 + 1);
	quadViewPort.Height = (float)height;
	quadViewPort.Width = (float)width;
	DEVICE_CONTEXT->RSSetViewports(1, &quadViewPort);

	tMipLevelBuffer.mipLevel0 = mipLevel0;
	tMipLevelBuffer.mipLevel1 = mipLevel1;
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer("HDRCbuffer", &tMipLevelBuffer, CBT_PIXEL);


	CRenderingTarget*	pSummedTarget = FindRenderTarget("SummedTarget");
	ID3D11RenderTargetView* pSummedTargetView = pSummedTarget->GetRenderTargetView();
	ID3D11ShaderResourceView* pSummedSRV = pSummedTarget->GetShaderResourceView();
	ID3D11RenderTargetView** ppSummedTargetView = pSummedTarget->GetRenderTargetViewMip();

	CRenderingTarget*	pBlurredHTarget = FindRenderTarget("BlurredHTarget");
	ID3D11RenderTargetView* pBlurredHTargetView = pBlurredHTarget->GetRenderTargetView();
	ID3D11ShaderResourceView* pBlurredHSRV = pBlurredHTarget->GetShaderResourceView();
	ID3D11RenderTargetView** ppBlurredHTargetView = pBlurredHTarget->GetRenderTargetViewMip();

	CRenderingTarget*	pBlurredHVTarget = FindRenderTarget("BlurredHVTarget");
	ID3D11RenderTargetView* pBlurredHVTargetView = pBlurredHVTarget->GetRenderTargetView();
	ID3D11ShaderResourceView* pBlurredHVSRV = pBlurredHVTarget->GetShaderResourceView();
	ID3D11RenderTargetView** ppBlurredHVTargetView = pBlurredHVTarget->GetRenderTargetViewMip();

	pSummedTarget->ClearTarget();
	pBlurredHTarget->ClearTarget();
	pBlurredHVTarget->ClearTarget();

	float fClearColor[4] = { 0.f, 0.f, 0.f, 0.f };

	for (int i = 1; i < 4; ++i)
	{
		DEVICE_CONTEXT->ClearRenderTargetView(ppSummedTargetView[i], fClearColor);
		DEVICE_CONTEXT->ClearRenderTargetView(ppBlurredHTargetView[i], fClearColor);
		DEVICE_CONTEXT->ClearRenderTargetView(ppBlurredHVTargetView[i], fClearColor);
	}

	// 밉맵수준 만큼 반복하면서 Bright 타겟에 저장된 rgb값을 받아와 Bloom 처리를 합니다
	BlurHorizontal(pBrightSRV, ppBlurredHTargetView[mipLevel0]);
	BlurVertical(pBlurredHSRV, ppBlurredHVTargetView[mipLevel0]);

	mipLevel0--;
	mipLevel1--;
	width = DEVICE_RESOLUTION.iWidth >> (mipLevel0 + 1);;
	height = DEVICE_RESOLUTION.iHeight >> (mipLevel0 + 1);
	quadViewPort.Height = (float)height;
	quadViewPort.Width = (float)width;
	DEVICE_CONTEXT->RSSetViewports(1, &quadViewPort);

	tMipLevelBuffer.mipLevel0 = mipLevel0;
	tMipLevelBuffer.mipLevel1 = mipLevel1;
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer("HDRCbuffer", &tMipLevelBuffer, CBT_PIXEL);
	Add(pBrightSRV, pBlurredHVSRV, ppSummedTargetView[mipLevel0]);
	BlurHorizontal(pSummedSRV, ppBlurredHTargetView[mipLevel0]);
	BlurVertical(pBlurredHSRV, ppBlurredHVTargetView[mipLevel0]);

	mipLevel0--;
	mipLevel1--;
	width = DEVICE_RESOLUTION.iWidth >> (mipLevel0 + 1);
	height = DEVICE_RESOLUTION.iHeight >> (mipLevel0 + 1);
	quadViewPort.Height = (float)height;
	quadViewPort.Width = (float)width;
	DEVICE_CONTEXT->RSSetViewports(1, &quadViewPort);

	tMipLevelBuffer.mipLevel0 = mipLevel0;
	tMipLevelBuffer.mipLevel1 = mipLevel1;
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer("HDRCbuffer", &tMipLevelBuffer, CBT_PIXEL);
	Add(pBrightSRV, pBlurredHVSRV, ppSummedTargetView[mipLevel0]);
	BlurHorizontal(pSummedSRV, ppBlurredHTargetView[mipLevel0]);
	BlurVertical(pBlurredHSRV, ppBlurredHVTargetView[mipLevel0]);

	mipLevel0--;
	mipLevel1--;
	width = DEVICE_RESOLUTION.iWidth >> (mipLevel0 + 1);
	height = DEVICE_RESOLUTION.iHeight >> (mipLevel0 + 1);
	quadViewPort.Height = (float)height;
	quadViewPort.Width = (float)width;
	DEVICE_CONTEXT->RSSetViewports(1, &quadViewPort);

	tMipLevelBuffer.mipLevel0 = mipLevel0;
	tMipLevelBuffer.mipLevel1 = mipLevel1;
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer("HDRCbuffer", &tMipLevelBuffer, CBT_PIXEL);
	Add(pBrightSRV, pBlurredHVSRV, pSummedTargetView);
	BlurHorizontal(pSummedSRV, pBlurredHTargetView);
	BlurVertical(pBlurredHSRV, pBlurredHVTargetView);

	DEVICE_CONTEXT->RSSetViewports(1, &viewport);

	DEVICE_CONTEXT->OMSetRenderTargets(1, &pTargetOldView, pDepthOldView);

	pTargetOldView->Release();
	pDepthOldView->Release();

	SAFE_RELEASE(pSampler);

	// 이 단계가 종료되면 렌더링 타겟 BlurredHVTarget에 Bloom 텍스쳐가 들어갑니다
	// AvgLuminance 타겟에는 평균 휘도값이 저장되어 있습니다
	// PrevAvgLuminance 타겟에는 직전프레임의 평균 휘도값이 저장되어 있습니다
}

void CRenderManager::DrawFullScreenQuad()
{
	CDepthStencilState*	pDepthDisable = (CDepthStencilState*)FindRenderState(DEPTH_DISABLE);
	pDepthDisable->SetState();
	DEVICE_CONTEXT->IASetInputLayout(NULL);
	UINT iOffset = 0;
	DEVICE_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DEVICE_CONTEXT->IASetVertexBuffers(0, 0, NULL, 0, &iOffset);
	DEVICE_CONTEXT->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	DEVICE_CONTEXT->Draw(4, 0);
	pDepthDisable->ResetState();
	SAFE_RELEASE(pDepthDisable);
}

void CRenderManager::BlurHorizontal(ID3D11ShaderResourceView * pSrc, ID3D11RenderTargetView * pDst)
{
	ID3D11RenderTargetView* pOldTV;
	ID3D11DepthStencilView* pOldDSV;
	{
		DEVICE_CONTEXT->OMGetRenderTargets(1, &pOldTV, &pOldDSV);
	}
	DEVICE_CONTEXT->OMSetRenderTargets(1, &pDst, NULL);
	ID3D11ShaderResourceView* pSR[] = { pSrc };
	DEVICE_CONTEXT->PSSetShaderResources(0, 1, pSR);
	CShader* pShader = GET_SINGLE(CShaderManager)->FindShader("BlurHorizontal");
	pShader->SetShader();
	DrawFullScreenQuad();
	SAFE_RELEASE(pShader);
	{
		DEVICE_CONTEXT->OMSetRenderTargets(1, &pOldTV, pOldDSV);
	}
}

void CRenderManager::BlurVertical(ID3D11ShaderResourceView * pSrc, ID3D11RenderTargetView * pDst)
{
	ID3D11RenderTargetView* pOldTV;
	ID3D11DepthStencilView* pOldDSV;
	{
		DEVICE_CONTEXT->OMGetRenderTargets(1, &pOldTV, &pOldDSV);
	}
	ID3D11ShaderResourceView* pSR[] = { NULL, NULL };
	DEVICE_CONTEXT->PSSetShaderResources(0, 2, pSR);
	DEVICE_CONTEXT->OMSetRenderTargets(1, &pDst, NULL);
	pSR[0] = pSrc;
	DEVICE_CONTEXT->PSSetShaderResources(0, 1, pSR);
	CShader* pShader = GET_SINGLE(CShaderManager)->FindShader("BlurVertical");
	pShader->SetShader();
	DrawFullScreenQuad();
	SAFE_RELEASE(pShader);
	{
		DEVICE_CONTEXT->OMSetRenderTargets(1, &pOldTV, pOldDSV);
	}
}

void CRenderManager::Add(ID3D11ShaderResourceView * pSrc0, ID3D11ShaderResourceView * pSrc1, ID3D11RenderTargetView * pDst)
{
	ID3D11RenderTargetView* pOldTV;
	ID3D11DepthStencilView* pOldDSV;
	{
		DEVICE_CONTEXT->OMGetRenderTargets(1, &pOldTV, &pOldDSV);
	}
	DEVICE_CONTEXT->OMSetRenderTargets(1, &pDst, NULL);
	ID3D11ShaderResourceView* pSR[] = { pSrc0, pSrc1 };
	DEVICE_CONTEXT->PSSetShaderResources(0, 2, pSR);
	CShader* pShader = GET_SINGLE(CShaderManager)->FindShader("Add");
	pShader->SetShader();
	DrawFullScreenQuad();
	SAFE_RELEASE(pShader);
	{
		DEVICE_CONTEXT->OMSetRenderTargets(1, &pOldTV, pOldDSV);
	}
}
