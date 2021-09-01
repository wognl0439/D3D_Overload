#include "ShaderManager.h"
#include "Shader.h"
#include "Device.h"

SSS_USING
DEFINITION_SINGLE(CShaderManager)

CShaderManager::CShaderManager() :
	m_iElementSize(0)
{
}


CShaderManager::~CShaderManager()
{
	unordered_map<string, PCONSTANTBUFFER>::iterator iter;
	unordered_map<string, PCONSTANTBUFFER>::iterator iterEnd = m_mapConstantBuffer.end();
	for (iter = m_mapConstantBuffer.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE(iter->second->pBuffer);
		SAFE_DELETE(iter->second);
	}


	Safe_Release_Map(m_mapInputLayout);
	Safe_Release_Map(m_mapShader);
}

bool CShaderManager::Initialize()
{
	char*	pEntry[ST_END] = {};

	pEntry[ST_VERTEX] = "StandardColorVS";
	pEntry[ST_PIXEL] = "StandardColorPS";
	pEntry[ST_GEOMETRY] = NULL;
	CShader*	pShader = LoadShader(STANDARD_COLOR_SHADER, L"Standard.fx", pEntry);
	SAFE_RELEASE(pShader);


	pEntry[ST_VERTEX] = "StandardColorNormalVS";
	pEntry[ST_PIXEL] = "StandardColorNormalPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(STANDARD_COLOR_NORMAL_SHADER, L"Standard.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "StandardTextureVS";
	pEntry[ST_PIXEL] = "StandardTexturePS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(STANDARD_TEXTURE_SHADER, L"Standard.fx", pEntry);
	SAFE_RELEASE(pShader);
	
	pEntry[ST_VERTEX] = "StandardTexNormalVS";
	pEntry[ST_PIXEL] = "StandardTexNormalPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(STANDARD_TEXTURE_NORMAL_SHADER, L"Standard.fx", pEntry);
	SAFE_RELEASE(pShader);
	
	pEntry[ST_VERTEX] = "StandardBumpVS";
	pEntry[ST_PIXEL] = "StandardBumpPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(STANDARD_BUMP_SHADER, L"Standard.fx", pEntry);
	SAFE_RELEASE(pShader);


	pEntry[ST_VERTEX] = "BurnVS";
	pEntry[ST_PIXEL] = "BurnPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("BurnShader", L"Effect.fx", pEntry);
	SAFE_RELEASE(pShader);


	pEntry[ST_VERTEX] = "ColliderVS";
	pEntry[ST_PIXEL] = "ColliderPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(COLLIDER_SHADER, L"Collider.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "ColoredTextureVS";
	pEntry[ST_PIXEL] = "ColoredTexturePS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(COLORED_TEXTURE_SHADER, L"Standard.fx", pEntry);
	SAFE_RELEASE(pShader);


	pEntry[ST_VERTEX] = "StandardTexNormalAnimVS";
	pEntry[ST_PIXEL] = "StandardTexNormalPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(STANDARD_TEXTURE_NORMAL_ANIM_SHADER, L"Standard.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "StandardBumpAnimVS";
	pEntry[ST_PIXEL] = "StandardBumpPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(STANDARD_BUMP_ANIM_SHADER, L"Standard.fx", pEntry);
	SAFE_RELEASE(pShader);



	pEntry[ST_VERTEX] = "UITargetVS";
	pEntry[ST_PIXEL] = "UITargetPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(UI_RENDERTARGET_SHADER, L"Standard.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "UITextureVS";
	pEntry[ST_PIXEL] = "UITexturePS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(UI_TEXTURE_SHADER, L"Standard.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "LightAccDirVS";
	pEntry[ST_PIXEL] = "LightAccPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("LightAccDir", L"Light.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "LightAccDirVS";
	pEntry[ST_PIXEL] = "LightBlendPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("LightBlend", L"Light.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "LightAccDirVS";
	pEntry[ST_PIXEL] = "RenderBlendPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("RenderBlend", L"Light.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "LightAccDirVS";
	pEntry[ST_PIXEL] = "RenderPrevBackBufferPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("RenderPrevBackBufferShader", L"Light.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "LightAccPointVS";
	pEntry[ST_PIXEL] = "LightAccPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("LightPointAcc",
		L"Light.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "SkyVS";
	pEntry[ST_PIXEL] = "SkyPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(SKYBOX_SHADER, L"Skybox.fx", pEntry);
	SAFE_RELEASE(pShader);


	pEntry[ST_VERTEX] = "RimLightVS";
	pEntry[ST_PIXEL] = "RimLightPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(RIM_LIGHT_SHADER, L"Effect.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "OutLineVS";
	pEntry[ST_PIXEL] = "OutLinePS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(OUTLINE_SHADER, L"Effect.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "BUMPANIM_VS";
	pEntry[ST_PIXEL] = "DISTORTION_PS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("DistortionAnimShader", L"Effect.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "TerrainVS";
	pEntry[ST_PIXEL] = "TerrainPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(TERRAIN_SHADER, L"Terrain.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "BillboardVS";
	pEntry[ST_PIXEL] = "BillboardPS";
	pEntry[ST_GEOMETRY] = "BillboardGS";
	pShader = LoadShader(BILLBOARD_SHADER, L"Billboard.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "SpawningPoolDebugVS";
	pEntry[ST_PIXEL] = "SpawningPoolDebugPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(SPAWNINGPOOL_SHADER, L"SpawningPoolDebug.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "ParticleVS";
	pEntry[ST_PIXEL] = "ParticlePS";
	pEntry[ST_GEOMETRY] = "ParticleGS";
	pShader = LoadShader("Particle", L"Particle.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "ShadowVS";
	pEntry[ST_PIXEL] = "ShadowPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("ShadowShader",
		L"Standard.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "ShadowTexNormalAnimVS";
	pEntry[ST_PIXEL] = "ShadowPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("ShadowTexNormalAnimShader",
		L"Standard.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "ShadowBumpAnimVS";
	pEntry[ST_PIXEL] = "ShadowPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("ShadowBumpAnimShader",
		L"Standard.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "FireVS";
	pEntry[ST_PIXEL] = "FirePS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("FireShader",
		L"Fire.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "BillboardFireVS";
	pEntry[ST_PIXEL] = "FirePS";
	pEntry[ST_GEOMETRY] = "BillboardFireGS";
	pShader = LoadShader("FireBillBoardShader",
		L"Fire.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "WaterVS";
	pEntry[ST_PIXEL] = "WaterPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("WaterShader",
		L"Water.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "DISTORTION_VS";
	pEntry[ST_PIXEL] = "DISTORTION_PS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("DistortionShader",
		L"Effect.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "TrailVS";
	pEntry[ST_PIXEL] = "TrailPS";
	pEntry[ST_GEOMETRY] = "TrailGS";
	pShader = LoadShader("TrailShader",
		L"Trail.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "TrailVS";
	pEntry[ST_PIXEL] = "TRAIL_DISTORTION_PS";
	pEntry[ST_GEOMETRY] = "TrailGS";
	pShader = LoadShader("TrailDistortionShader",
		L"Trail.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "ScreenSpaceDecalVS";
	pEntry[ST_PIXEL] = "ScreenSpaceDecalPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("DecalShader",
		L"Decal.fx", pEntry);
	SAFE_RELEASE(pShader);

#pragma region PS_HDR
	pEntry[ST_VERTEX] = "DrawFullScreenQuad";
	pEntry[ST_PIXEL] = "CalcLuminance";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("CalcLuminance", L"HDR.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "DrawFullScreenQuad";
	pEntry[ST_PIXEL] = "AvgLuminance";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("AvgLuminance", L"HDR.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "DrawFullScreenQuad";
	pEntry[ST_PIXEL] = "Bright";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("Bright", L"HDR.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "DrawFullScreenQuad";
	pEntry[ST_PIXEL] = "Add";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("Add", L"HDR.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "DrawFullScreenQuad";
	pEntry[ST_PIXEL] = "BlurHorizontal";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("BlurHorizontal", L"HDR.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "DrawFullScreenQuad";
	pEntry[ST_PIXEL] = "BlurVertical";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("BlurVertical", L"HDR.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "DrawFullScreenQuad";
	pEntry[ST_PIXEL] = "PrevAvgLuminance";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader("PrevAvgLuminance", L"HDR.fx", pEntry);
	SAFE_RELEASE(pShader);
#pragma endregion

	//AlphaMask Shader
	pEntry[ST_VERTEX] = "AlphaMaskVS";
	pEntry[ST_PIXEL] = "AlphaMaskPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(ALPHAMASK_SHADER, L"AlphaMask.fx", pEntry);
	SAFE_RELEASE(pShader);

	//SliderShader Test
	pEntry[ST_VERTEX] = "SliderVS";
	pEntry[ST_PIXEL] = "SliderPS";
	pEntry[ST_GEOMETRY] = NULL;
	pShader = LoadShader(SLIDER_SHADER, L"AlphaMask.fx", pEntry);
	SAFE_RELEASE(pShader);

	//HealthBar Shader
	pEntry[ST_VERTEX] = "HealthBarVS";
	pEntry[ST_GEOMETRY] = NULL;
	pEntry[ST_PIXEL] = "HealthBarPS";
	pShader = LoadShader(HEALTHBAR_SHADER, L"HealthBar.fx", pEntry);
	SAFE_RELEASE(pShader);

	pEntry[ST_VERTEX] = "HealthBarVS";
	pEntry[ST_GEOMETRY] = NULL;
	pEntry[ST_PIXEL] = "UIBossHealthBarPS";
	pShader = LoadShader("BossHealthBarShader", L"HealthBar.fx", pEntry);
	SAFE_RELEASE(pShader);

	//HealthBar Shader
	pEntry[ST_VERTEX] = "LoadingVS";
	pEntry[ST_GEOMETRY] = NULL;
	pEntry[ST_PIXEL] = "LoadingPS";
	pShader = LoadShader(LOADING_SHADER, L"Loading.fx", pEntry);
	SAFE_RELEASE(pShader);


	//Slider Test
	//AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	//AddInputElement("POSITION1", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	//AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8);
	//CreateInputLayout(POS_TEX_SIZE, SLIDER_SHADER);

	
	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16);
	CreateInputLayout(POS_COLOR_LAYOUT, STANDARD_COLOR_SHADER);

	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8);
	CreateInputLayout(POS_UV_LAYOUT, STANDARD_TEXTURE_SHADER);

	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16);
	CreateInputLayout(POS_COLOR_NORMAL_LAYOUT, STANDARD_COLOR_NORMAL_SHADER);

	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8);
	CreateInputLayout(POS_UV_NORMAL_LAYOUT, STANDARD_TEXTURE_NORMAL_SHADER);

	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	CreateInputLayout(POS_LAYOUT, BILLBOARD_SHADER);

	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8);
	AddInputElement("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	CreateInputLayout(BUMP_LAYOUT, STANDARD_BUMP_SHADER);

	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8);
	AddInputElement("BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16);
	AddInputElement("BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16);
	CreateInputLayout(POS_TEXTURE_NORMAL_ANIM_LAYOUT, STANDARD_TEXTURE_NORMAL_ANIM_SHADER);

	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8);
	AddInputElement("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16);
	AddInputElement("BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16);
	CreateInputLayout(BUMP_ANIM_LAYOUT, STANDARD_BUMP_ANIM_SHADER);
	
	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12);
	AddInputElement("SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8);
	AddInputElement("ALPHA", 0, DXGI_FORMAT_R32_FLOAT, 0, 4);
	AddInputElement("ENABLED", 0, DXGI_FORMAT_R32_UINT, 0, 4);
	CreateInputLayout(PARTICLE_LAYOUT, "Particle");

	// ================= 상수버퍼 ====================
	CreateConstantBuffer(TRANSFORM_CBUFFER, sizeof(TRANSFORMCBUFFER), 0);

	CreateConstantBuffer(COLLIDER_CBUFFER, sizeof(COLLIDERCBUFFER), 11);

	CreateConstantBuffer(SPAWNINGPOOL_CBUFFER, sizeof(Vector4), 11);

	CreateConstantBuffer(MATERIAL_CBUFFER, sizeof(MATERIAL), 1);

	CreateConstantBuffer(LIGHT_CBUFFER, sizeof(LIGHTCBUFFER), 2);

	CreateConstantBuffer("Animation2D", sizeof(ANIMATIONCLIP2DINFO), 9);
	//CreateConstantBuffer(ANIMATION2D_CBUFFER, sizeof(ANIMATION2DCBUFFER), 9);
	
	CreateConstantBuffer("ShadowRendering", sizeof(SHADOWRENDERING), 3);

	CreateConstantBuffer("Terrain", sizeof(TERRAINCBUFFER), 11);
	CreateConstantBuffer(BILLBOARD_CBUFFER, sizeof(BILLBOARDCBUFFER), 11);
	CreateConstantBuffer(HEALTHBAR_CBUFFER, sizeof(HEALTHBARCBUFFER), 11);
	CreateConstantBuffer(LOADING_CBUFFER, sizeof(LOADINGCBUFFER), 11);
	
	CreateConstantBuffer("FireCBuffer", sizeof(FIRECBUFFER), 5);
	CreateConstantBuffer("WaterCBuffer", sizeof(WATERCBUFFER), 6);
	CreateConstantBuffer("BOSSHEALTHBAR", 32, 7);
	CreateConstantBuffer("TrailCBuffer", sizeof(TRAILCBUFFER), 13);

	// ================= Post Processing 상수버퍼 ====================
	CreateConstantBuffer(POSTPROCESS_CBUFFER, sizeof(POSTPROCESSCBUFFER), 8);
	CreateConstantBuffer("HDRCbuffer", sizeof(MIPLEVELCBUFFER), 8);

	// ================= FX 상수버퍼 ====================
	CreateConstantBuffer(Effect_CBUFFER, sizeof(EFFECT), 10);
	CreateConstantBuffer(FX_OUTLINE_CBUFFER, sizeof(FXOUTLINE), 12);


	return true;
}

CShader * CShaderManager::LoadShader(const string & strTag, const wchar_t * pFileName, char * pEntry[ST_END], const string & strPathKey)
{
	CShader* pShader = FindShader(strTag);

	if (pShader)
	{
		return pShader;
	}
	pShader = new CShader;

	if (!pShader->LoadShader(strTag, pFileName, pEntry, strPathKey))
	{
		SAFE_RELEASE(pShader);
		return NULL;
	}

	pShader->SetKey(strTag);
	pShader->AddRef();
	m_mapShader.insert(make_pair(strTag, pShader));
	return pShader;
}

//++Reference
CShader * CShaderManager::FindShader(const string & strTag)
{
	unordered_map<string, CShader*>::iterator iter = m_mapShader.find(strTag);

	if (iter == m_mapShader.end())
		return NULL;

	iter->second->AddRef();
	return iter->second;
}

void CShaderManager::AddInputElement(const char * pSemantic, int iSemanticIdx, DXGI_FORMAT eFmt, UINT iInputSlot, UINT iSize, D3D11_INPUT_CLASSIFICATION eInputClass, UINT iInstanceStepRate)
{
	D3D11_INPUT_ELEMENT_DESC	tElement = {};
	tElement.SemanticName = pSemantic;
	tElement.SemanticIndex = iSemanticIdx;
	tElement.Format = eFmt;
	tElement.InputSlot = iInputSlot;
	tElement.AlignedByteOffset = m_iElementSize;
	tElement.InputSlotClass = eInputClass;
	tElement.InstanceDataStepRate = iInstanceStepRate;
	m_iElementSize += iSize;

	m_vecAddElement.push_back(tElement);
}

ID3D11InputLayout * CShaderManager::CreateInputLayout(const string & strKey, const string & strShaderKey)
{
	ID3D11InputLayout*	pLayout = FindInputLayout(strKey);

	if (pLayout)
		return pLayout;

	CShader*	pShader = FindShader(strShaderKey);

	if (!pShader)
		return NULL;

	if (FAILED(DEVICE->CreateInputLayout(&m_vecAddElement[0], m_vecAddElement.size(), pShader->GetVSBlob()->GetBufferPointer(),
		pShader->GetVSBlob()->GetBufferSize(), &pLayout)))
	{
		m_vecAddElement.clear();
		m_iElementSize = 0;
		SAFE_RELEASE(pShader);
		return false;
	}

	m_vecAddElement.clear();
	m_iElementSize = 0;
	SAFE_RELEASE(pShader);

	m_mapInputLayout.insert(make_pair(strKey, pLayout));

	return pLayout;
}

ID3D11InputLayout * CShaderManager::FindInputLayout(const string & strKey)
{
	unordered_map<string, ID3D11InputLayout*>::iterator	iter = m_mapInputLayout.find(strKey);

	if (iter == m_mapInputLayout.end())
		return NULL;

	return iter->second;
}

const string & CShaderManager::FindInputLayoutKey(ID3D11InputLayout * pInputLayout)
{
	unordered_map<string, ID3D11InputLayout*>::iterator iter;
	unordered_map<string, ID3D11InputLayout*>::iterator iterEnd = m_mapInputLayout.end();

	for (iter = m_mapInputLayout.begin(); iter != iterEnd; ++iter)
	{
		if (iter->second == pInputLayout)
		{
			return iter->first;
		}
	}

	return string();
}

PCONSTANTBUFFER CShaderManager::CreateConstantBuffer(const string & strKey, UINT iSize, int iRegister)
{
	PCONSTANTBUFFER	pConstantBuffer = FindConstantBuffer(strKey);

	if (pConstantBuffer)
		return pConstantBuffer;

	pConstantBuffer = new CONSTANTBUFFER;

	pConstantBuffer->iSize = iSize;
	pConstantBuffer->iRegister = iRegister;

	D3D11_BUFFER_DESC	tDesc = {};
	tDesc.ByteWidth = iSize;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, NULL, &pConstantBuffer->pBuffer)))
	{
		SAFE_DELETE(pConstantBuffer);
		return NULL;
	}

	m_mapConstantBuffer.insert(make_pair(strKey, pConstantBuffer));

	return pConstantBuffer;
}

bool CShaderManager::UpdateConstantBuffer(const string & strKey, void * pData, int iCBufferTransfer)
{
	PCONSTANTBUFFER	pCBuffer = FindConstantBuffer(strKey);

	if (!pCBuffer)
		return false;

	D3D11_MAPPED_SUBRESOURCE	tMap = {};

	DEVICE_CONTEXT->Map(pCBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tMap);

	memcpy(tMap.pData, pData, pCBuffer->iSize);

	DEVICE_CONTEXT->Unmap(pCBuffer->pBuffer, 0);

	if (iCBufferTransfer & CBT_VERTEX)
		DEVICE_CONTEXT->VSSetConstantBuffers(pCBuffer->iRegister, 1, &pCBuffer->pBuffer);

	if (iCBufferTransfer & CBT_PIXEL)
		DEVICE_CONTEXT->PSSetConstantBuffers(pCBuffer->iRegister, 1, &pCBuffer->pBuffer);

	if (iCBufferTransfer & CBT_GEOMETRY)
		DEVICE_CONTEXT->GSSetConstantBuffers(pCBuffer->iRegister, 1, &pCBuffer->pBuffer);

	return true;
}

PCONSTANTBUFFER CShaderManager::FindConstantBuffer(const string & strKey)
{
	unordered_map<string, PCONSTANTBUFFER>::iterator	iter = m_mapConstantBuffer.find(strKey);

	if (iter == m_mapConstantBuffer.end())
		return NULL;

	return iter->second;
}
