#include "SpriteRenderer.h"
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


#include "../Debug.h"


SSS_USING

CSpriteRenderer::CSpriteRenderer() :
	m_pMesh(NULL),
	m_pShader(NULL),
	m_pInputLayout(NULL),
	m_pMaterial(NULL),
	m_iHasAnimation(AC_NONE)
{
	SetTag("Renderer");

	m_tAnimCBuffer.fX = 0;
	m_tAnimCBuffer.fY = 0;
	m_tAnimCBuffer.fWidth = 0;
	m_tAnimCBuffer.fHeght = 0;
	m_tAnimCBuffer.fAtlasWidth = 0;
	m_tAnimCBuffer.fAtlasHeight = 0;
		
	memset(m_pRenderState, 0, sizeof(CRenderState*) * RS_END);
}


CSpriteRenderer::CSpriteRenderer(const CSpriteRenderer & renderer):
	CComponent(renderer)
{
	*this = renderer;

	if (m_pShader)
		m_pShader->AddRef();

	if (m_pMesh)
		m_pMesh->AddRef();
	
	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->AddRef();
	}

	// 만약 재질 정보가 있을 경우 재질 정보를 얻어온다.
	//m_pMaterial = GetComponent<CMaterial>();
}

CSpriteRenderer::~CSpriteRenderer()
{
	for (int i = 0; i < RS_END; ++i)
	{
		SAFE_RELEASE(m_pRenderState[i]);
	}


	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pMesh);

}


void CSpriteRenderer::SetAnimationCondition(int32_t iCondition)
{
	m_iHasAnimation |= iCondition;
}

int32_t CSpriteRenderer::GetAnimationCondition() const
{
	return m_iHasAnimation;
}

bool CSpriteRenderer::Initialize()
{
	SetMesh(ENGINE_MESH_ORTHORECT_TEX);

	if (!m_pMaterial)
	{
		SetDefaultMaterial();
	}


	SetRenderState(ALPHA_BLEND);
	SetRenderState(CULLNONE);
	SetRenderState(DEPTH_DISABLE);
	return true;
}


void CSpriteRenderer::UpdateTransform()
{
	TRANSFORMCBUFFER	tBuffer;

	// Camera를 얻어온다.
	CCamera*	pCamera = m_pScene->GetMainCamera();

	tBuffer.matWorld = m_pTransform->GetWorldMatrix();
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProjection = pCamera->GetOrthographyProjectionMatrix();
	tBuffer.matWV = tBuffer.matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
	tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
	tBuffer.matWP = tBuffer.matWorld * tBuffer.matProjection;
	tBuffer.matInvProjection = Matrix::Inverse(tBuffer.matProjection);
	tBuffer.matInvVP = Matrix::Inverse(tBuffer.matVP);
	tBuffer.matInvWV = Matrix::Inverse(tBuffer.matWV);
	tBuffer.vPivot = m_pTransform->GetPivot();
	tBuffer.iRenderMode = GET_SINGLE(CRenderManager)->GetRenderMode() ? 1 : 0;

	
	if (m_pMesh)
	{
		tBuffer.vLength = m_pMesh->GetLength();
	}
	else
	{
		tBuffer.vLength = Vector3::Zero;
	}
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
	
	tBuffer.iHasAnimation = m_iHasAnimation;

	// Transform 정보를 Shader 상수버퍼로 넘겨준다.
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(TRANSFORM_CBUFFER, &tBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);
}

void CSpriteRenderer::UpdatePrerender()
{
	list<CComponent*> ComponentList = GetAllComponents();
	list<CComponent*>::iterator iter;
	list<CComponent*>::iterator iterEnd = ComponentList.end();

	for (iter = ComponentList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->Prerender(NULL);
	}
}

void CSpriteRenderer::UpdatePostrender()
{
	list<CComponent*> ComponentList = GetAllComponents();
	list<CComponent*>::iterator iter;
	list<CComponent*>::iterator iterEnd = ComponentList.end();

	for (iter = ComponentList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->Postrender(NULL);
	}
}

void CSpriteRenderer::SetDefaultMaterial()
{
	if (!m_pMaterial)
	{
		//m_pMaterial = m_pGameObject->AddComponent<CMaterial>("Material");
		m_pMaterial = new CMaterial;
		m_pMaterial->SetDiffuseTexture("DefaultTexture", L"DefaultTexture.png");
	}
}


void CSpriteRenderer::Render(float fTime)
{
	UpdateTransform();
	UpdatePrerender();

	if (m_iHasAnimation & AC_2D || m_iHasAnimation & AC_USE_ATLAS)
	{
		GET_SINGLE(CShaderManager)->UpdateConstantBuffer("Animation2D", &m_tAnimCBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);
	}

	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->SetState();
	}

	//재질이 없다면 그릴 수 없으므로 걸러준다.
	//보통은 기본 재질이 들어가도록 한다.
	if (m_pMaterial)
	{
		for (size_t i = 0; i < m_pMaterial->GetContainerCount(); ++i)
		{
			for (int j = 0; j < m_pMaterial->GetSubsetCount(i); ++j)
			{
				m_pMaterial->SetMaterial(i, j);

				m_pShader->SetShader();

				DEVICE_CONTEXT->IASetInputLayout(m_pInputLayout);

				m_pMesh->Render(i, j);

				m_pMaterial->ResetMaterial(i, j);
			}
		}
	}
	else	
	{
		wstring strLog(m_pGameObject->GetTag().begin(), m_pGameObject->GetTag().end());
		strLog += L" 오브젝트의 CSpriteRenderer 컴포넌트가 Material이 존재하지 않습니다.\n";
		GET_SINGLE(CDebug)->Log(strLog);
	}
	
	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->ResetState();
	}

	UpdatePostrender();
}

CSpriteRenderer * CSpriteRenderer::Clone() const
{
	return new CSpriteRenderer(*this);
}

int CSpriteRenderer::GetZOrder() const
{
	return m_iZOrder;
}

void CSpriteRenderer::SetZOrder(int iOrder)
{
	m_iZOrder = iOrder;
}

const string & CSpriteRenderer::GetAtlasKey() const
{
	return m_strAtlasKey;
}

///<summary>
///<para>　</para>
///<para>현재 멤버로 등록되어진 텍스쳐의 데이터(meta파일)를 읽어온 뒤 생성된 키로 텍스쳐의 일부를 잘라서 사용합니다.</para>
///<para>매개변수 "strKey"를 사용하려면 반드시 meta파일이 파싱이 되어 있어야 합니다.</para>
///<para>meta파일이 파싱되어져 있지 않아서 key가 생성되어있지 않다면, 다른 오버로드 함수를 사용하십시오.</para>
///</summary>
void CSpriteRenderer::SetImageInfo(const string & strKey)
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
void CSpriteRenderer::SetImageInfo(int iStartX, int iStartY, int iLengthX, int iLengthY, int iMaxSizeX, int iMaxSizeY, ANIMATION_TYPE eType)
{
	if (!m_pMaterial)
		return;

	m_tAnimCBuffer.fX = iStartX;
	m_tAnimCBuffer.fY = iStartY;
	m_tAnimCBuffer.fWidth = iLengthX;
	m_tAnimCBuffer.fHeght = iLengthY;
	m_tAnimCBuffer.fAtlasWidth = iMaxSizeX;
	m_tAnimCBuffer.fAtlasHeight = iMaxSizeY;

	SetAnimationCondition(AC_USE_ATLAS);
}

void CSpriteRenderer::SetMesh(CMesh * pMesh)
{
	SAFE_RELEASE(m_pMesh);
	pMesh->AddRef();
	m_pMesh = pMesh;

	CMaterial* pMaterial = m_pMesh->GetMaterial();
	if (!pMaterial)
	{
		m_pMaterial = pMaterial;
	}

	SetShader(m_pMesh->GetShaderKey());
	SetInputLayout(m_pMesh->GetLayoutKey());
}

void CSpriteRenderer::SetMesh(const string & strKey)
{
	SAFE_RELEASE(m_pMesh);
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(strKey);

	CMaterial* pMaterial = m_pMesh->GetMaterial();
	if (!pMaterial)
	{
		m_pMaterial = pMaterial;
	}

	SetShader(m_pMesh->GetShaderKey());
	SetInputLayout(m_pMesh->GetLayoutKey());
}

void CSpriteRenderer::SetShader(CShader * pShader)
{
	SAFE_RELEASE(m_pShader);
	pShader->AddRef();
	m_pShader = pShader;
}

void CSpriteRenderer::SetShader(const string & strKey)
{
	SAFE_RELEASE(m_pShader);
	m_pShader = GET_SINGLE(CShaderManager)->FindShader(strKey);
}

void CSpriteRenderer::SetInputLayout(ID3D11InputLayout * pInputLayout)
{
	m_pInputLayout = pInputLayout;
}

void CSpriteRenderer::SetInputLayout(const string & strKey)
{
	m_pInputLayout = GET_SINGLE(CShaderManager)->FindInputLayout(strKey);
}

CMaterial * CSpriteRenderer::GetMaterial() const
{
	m_pMaterial->AddRef();
	return m_pMaterial;
}

void CSpriteRenderer::SetMaterial(CMaterial * pMaterial)
{
	pMaterial->AddRef();
	m_pMaterial = pMaterial;
}

void CSpriteRenderer::SetDiffuse(const Vector4 & vDiffuse)
{
	m_pMaterial->SetMaterial(vDiffuse);
}

bool CSpriteRenderer::SetDiffuseTexture(const string & strKey, const wchar_t * pFileName, const string & strPathKey)
{
	if (!m_pMaterial)
		return false;

	
	m_pMaterial->SetDiffuseTexture(strKey, pFileName, 0, 0, strPathKey);

	return true;
}

bool CSpriteRenderer::SetSampler(const string & strKey)
{
	if (!m_pMaterial)
		return false;

	m_pMaterial->SetSampler(strKey);

	return true;
}

void CSpriteRenderer::SetRenderState(const string & strKey)
{
	CRenderState*	pRenderState = GET_SINGLE(CRenderManager)->FindRenderState(strKey);

	if (!pRenderState)
		return;

	SAFE_RELEASE(m_pRenderState[pRenderState->GetRenderStateType()]);

	m_pRenderState[pRenderState->GetRenderStateType()] = pRenderState;

	if (pRenderState->GetRenderStateType() == RS_BLEND)
	{
		m_pGameObject->SetTransparency(true);
	}

}

const Vector4& CSpriteRenderer::GetDiffuse() const
{
	if (!m_pMaterial)
		return Vector4::clear;

	return m_pMaterial->GetDiffuseColor();
}

bool CSpriteRenderer::Save(FILE * pFile)
{
	return false;
}

bool CSpriteRenderer::Load(FILE * pFile)
{
	return false;
}

