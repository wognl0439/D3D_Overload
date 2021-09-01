#include "Renderer.h"
#include "../Mesh.h"
#include "../ResourcesManager.h"
#include "../Shader.h"
#include "../ShaderManager.h"
#include "../Device.h"
#include "Camera.h"
#include "../Scene.h"
#include "../Transform.h"
#include "../Material.h"
#include "../RenderManager.h"
#include "../RenderState.h"
#include "../DataManager.h"


SSS_USING

CRenderer::CRenderer() :
	m_pMesh(NULL),
	m_pShader(NULL),
	m_pInputLayout(NULL),
	m_pMaterial(NULL),
	m_eRenderMode(RM_3D)
{
	m_eComponentType = CT_RENDERER;
	SetTag("Renderer");

	CreateRendererConstantBuffer(ANIMATION2D_CBUFFER, sizeof(ANIMATION2DCBUFFER), CBT_VERTEX | CBT_PIXEL);

	m_tAnimCBuffer.iType = AT_NONE;
	m_tAnimCBuffer.iFrameX = 0;
	m_tAnimCBuffer.iFrameY = 0;
	m_tAnimCBuffer.iStartX = 0;
	m_tAnimCBuffer.iStartY = 0;
	m_tAnimCBuffer.iLengthX = 0;
	m_tAnimCBuffer.iLengthY = 0;
	m_tAnimCBuffer.iMaxX = 0;
	m_tAnimCBuffer.iMaxY = 0;

	UpdateRendererConstantBuffer(ANIMATION2D_CBUFFER, &m_tAnimCBuffer);

	memset(m_pRenderState, 0, sizeof(CRenderState*) * RS_END);


	if (GET_SINGLE(CRenderManager)->GetRenderMode() == RM_2D)
	{
		SetRenderState(CULLNONE);
		SetRenderState(DEPTH_DISABLE);
	}
	else if (GET_SINGLE(CRenderManager)->GetRenderMode() == RM_3D)
	{
		SetRenderState(CULLBACK);
		SetRenderState(DEPTH_ENABLE);
	}

}


CRenderer::CRenderer(const CRenderer & renderer):
	CComponent(renderer)
{
	*this = renderer;

	if (m_pShader)
		m_pShader->AddRef();

	if (m_pMesh)
		m_pMesh->AddRef();

	if (renderer.m_pMaterial)
		m_pMaterial = renderer.m_pMaterial->Clone();


	m_mapRendererConstantBuffer.clear();
	unordered_map<string, PRENDERERCBUFFER>::const_iterator	iter;
	unordered_map<string, PRENDERERCBUFFER>::const_iterator	iterEnd = renderer.m_mapRendererConstantBuffer.end();

	for (iter = renderer.m_mapRendererConstantBuffer.begin(); iter != iterEnd; ++iter)
	{
		PRENDERERCBUFFER	pBuffer = new RENDERERCBUFFER;
		pBuffer->iSize = iter->second->iSize;
		pBuffer->iTransfer = iter->second->iTransfer;
		pBuffer->pData = new char[pBuffer->iSize];
		memcpy(pBuffer->pData, iter->second->pData, pBuffer->iSize);

		m_mapRendererConstantBuffer.insert(make_pair(iter->first, pBuffer));
	}

	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->AddRef();
	}
}

CRenderer::~CRenderer()
{
	for (int i = 0; i < RS_END; ++i)
	{
		SAFE_RELEASE(m_pRenderState[i]);
	}

	unordered_map<string, PRENDERERCBUFFER>::iterator	iter;
	unordered_map<string, PRENDERERCBUFFER>::iterator	iterEnd = m_mapRendererConstantBuffer.end();

	for (iter = m_mapRendererConstantBuffer.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE_ARRAY(iter->second->pData);
		SAFE_DELETE(iter->second);
	}

	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pMesh);

}


bool CRenderer::Initialize()
{
	return true;
}


void CRenderer::UpdateTransform()
{
	TRANSFORMCBUFFER	tBuffer;

	// Camera를 얻어온다.
	CCamera*	pCamera = m_pScene->GetMainCamera();

	tBuffer.matWorld = m_pTransform->GetWorldMatrix();
	tBuffer.matView = pCamera->GetViewMatrix();
	if (m_eRenderMode == RM_2D)
	{
		tBuffer.matProjection = pCamera->GetOrthographyProjectionMatrix();
	}
	else if (m_eRenderMode == RM_3D)
	{
		tBuffer.matProjection = pCamera->GetPerspectiveProjectionMatrix();
	}
	else
	{
		tBuffer.matProjection = pCamera->GetOrthographyProjectionMatrix();
		//tBuffer.matProjection = pCamera->GetProjectionMatrix();
	}
	tBuffer.matWV = tBuffer.matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
	tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
	tBuffer.matWP = tBuffer.matWorld * tBuffer.matProjection;
	tBuffer.vPivot = m_pTransform->GetPivot();
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
}


void CRenderer::Render(float fTime)
{
	UpdateTransform();

	if (m_tAnimCBuffer.iType != AT_NONE)
	{
		UpdateRendererConstantBuffer(ANIMATION2D_CBUFFER, &m_tAnimCBuffer);
	}

	//Animation 부분
	unordered_map<string, PRENDERERCBUFFER>::iterator	iter;
	unordered_map<string, PRENDERERCBUFFER>::iterator	iterEnd = m_mapRendererConstantBuffer.end();

	// 사용자정의 상수버퍼를 설정한다.
	for (iter = m_mapRendererConstantBuffer.begin(); iter != iterEnd; ++iter)
	{
		PANIMATION2DCBUFFER	pBuffer = (PANIMATION2DCBUFFER)iter->second->pData;
		GET_SINGLE(CShaderManager)->UpdateConstantBuffer(iter->first, iter->second->pData,
			iter->second->iTransfer);
	}

	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->SetState();
	}


	// 재질정보를 설정한다.
	if (m_pMaterial)
		m_pMaterial->SetMaterial();

	m_pShader->SetShader();

	DEVICE_CONTEXT->IASetInputLayout(m_pInputLayout);
	
	m_pMesh->Render();


	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->ResetState();
	}
}

CRenderer * CRenderer::Clone() const
{
	return new CRenderer(*this);
}

const string & CRenderer::GetAtlasKey() const
{
	return m_strAtlasKey;
}

///<summary>
///<para>　</para>
///<para>현재 멤버로 등록되어진 텍스쳐의 데이터(meta파일)를 읽어온 뒤 생성된 키로 텍스쳐의 일부를 잘라서 사용합니다.</para>
///<para>매개변수 "strKey"를 사용하려면 반드시 meta파일이 파싱이 되어 있어야 합니다.</para>
///<para>meta파일이 파싱되어져 있지 않아서 key가 생성되어있지 않다면, 다른 오버로드 함수를 사용하십시오.</para>
///</summary>
void CRenderer::SetImageInfo(const string & strKey)
{
	PATLASDATA pData = GET_SINGLE(CDataManager)->GetAtlasData(strKey);

	if (!pData)
		return;

	else
	{
		m_strAtlasKey = strKey;
		SetImageInfo(pData->wX, pData->wY, pData->wWidth, pData->wHeight, pData->iAtlasWidth, pData->iAtlasHeight);
	}
}

///<summary>
///<para>　</para>
///<para>현재 멤버로 등록되어진 텍스쳐의 일부를 잘라서 사용합니다.</para>
///<para>iStartX : X 위치의 시작점</para>
///<para>iStartY : Y 위치의 시작점</para>
///<para>iLengthX : X 위치의 끝점</para>
///<para>iLengthY : Y 위치의 끝점</para>
///<para>iMaxSizeX : 원본 텍스쳐의 가로 길이 (Pixel 단위)</para>
///<para>iMaxSizeY : 원본 텍스쳐의 세로 길이 (Pixel 단위)</para>
///<para>eType : 쉐이더 로직 분기를 위한 type </para>
///</summary>
void CRenderer::SetImageInfo(int iStartX, int iStartY, int iLengthX, int iLengthY, int iMaxSizeX, int iMaxSizeY, ANIMATION_TYPE eType)
{
	if (!m_pMaterial)
		return;

	m_tAnimCBuffer.iType = (int)eType;
	m_tAnimCBuffer.iFrameX = iStartX;
	m_tAnimCBuffer.iFrameY = iStartY;
	m_tAnimCBuffer.iLengthX = iLengthX;
	m_tAnimCBuffer.iLengthY = iLengthY;
	m_tAnimCBuffer.iMaxX = iMaxSizeX;
	m_tAnimCBuffer.iMaxY = iMaxSizeY;
}

void CRenderer::SetRenderMode(RENDER_MODE eRenderMode)
{
	m_eRenderMode = eRenderMode;

	if (m_eRenderMode == RM_2D)
	{
		SetRenderState(CULLNONE);
		SetRenderState(DEPTH_DISABLE);
	}
	else if (m_eRenderMode == RM_3D)
	{
		SetRenderState(CULLBACK);
		SetRenderState(DEPTH_ENABLE);
	}
}

void CRenderer::SetMesh(CMesh * pMesh)
{
	SAFE_RELEASE(m_pMesh);
	pMesh->AddRef();
	m_pMesh = pMesh;
}

void CRenderer::SetMesh(const string & strKey)
{
	SAFE_RELEASE(m_pMesh);
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(strKey);
}

void CRenderer::SetShader(CShader * pShader)
{
	SAFE_RELEASE(m_pShader);
	pShader->AddRef();
	m_pShader = pShader;
}

void CRenderer::SetShader(const string & strKey)
{
	SAFE_RELEASE(m_pShader);
	m_pShader = GET_SINGLE(CShaderManager)->FindShader(strKey);
}

void CRenderer::SetInputLayout(ID3D11InputLayout * pInputLayout)
{
	m_pInputLayout = pInputLayout;
}

void CRenderer::SetInputLayout(const string & strKey)
{
	m_pInputLayout = GET_SINGLE(CShaderManager)->FindInputLayout(strKey);
}

CMaterial * CRenderer::GetMaterial() const
{
	m_pMaterial->AddRef();
	return m_pMaterial;
}

CMaterial * CRenderer::CreateMaterial()
{
	SAFE_RELEASE(m_pMaterial);

	m_pMaterial = new CMaterial;

	m_pMaterial->AddRef();

	return m_pMaterial;
}

PRENDERERCBUFFER CRenderer::CreateRendererConstantBuffer(const string & strKey, int iSize, int iTransfer)
{
	PRENDERERCBUFFER	pBuffer = FindRendererConstantBuffer(strKey);

	if (pBuffer)
		return pBuffer;

	pBuffer = new RENDERERCBUFFER;

	pBuffer->iTransfer = iTransfer;
	pBuffer->iSize = iSize;
	pBuffer->pData = new char[iSize];

	m_mapRendererConstantBuffer.insert(make_pair(strKey, pBuffer));

	return pBuffer;
}

PRENDERERCBUFFER CRenderer::FindRendererConstantBuffer(const string & strKey)
{
	unordered_map<string, PRENDERERCBUFFER>::iterator	iter = m_mapRendererConstantBuffer.find(strKey);

	if (iter == m_mapRendererConstantBuffer.end())
		return NULL;

	return iter->second;
}

bool CRenderer::UpdateRendererConstantBuffer(const string & strKey, void * pData)
{
	PRENDERERCBUFFER	pBuffer = FindRendererConstantBuffer(strKey);

	if (!pBuffer)
	{
		assert(false);
		return false;
	}

	memcpy(pBuffer->pData, pData, pBuffer->iSize);

	return true;
}

void CRenderer::SetDiffuse(const Vector4 & vDiffuse)
{
	m_pMaterial->SetMaterial(vDiffuse);
}

bool CRenderer::SetDiffuseTexture(const string & strKey, const wchar_t * pFileName, const string & strPathKey)
{
	if (!m_pMaterial)
		return false;

	m_pMaterial->SetDiffuseTexture(strKey, pFileName, strPathKey);

	return true;
}

bool CRenderer::SetSampler(const string & strKey)
{
	if (!m_pMaterial)
		return false;

	m_pMaterial->SetSampler(strKey);

	return true;
}

void CRenderer::SetRenderState(const string & strKey)
{
	CRenderState*	pRenderState = GET_SINGLE(CRenderManager)->FindRenderState(strKey);

	if (!pRenderState)
		return;

	SAFE_RELEASE(m_pRenderState[pRenderState->GetRenderStateType()]);

	m_pRenderState[pRenderState->GetRenderStateType()] = pRenderState;
}

const Vector4& CRenderer::GetDiffuse() const
{
	if (!m_pMaterial)
		return Vector4::clear;

	return m_pMaterial->GetDiffuse();
}

