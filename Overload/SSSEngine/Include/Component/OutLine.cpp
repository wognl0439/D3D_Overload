#include "OutLine.h"
#include "../GameObject.h"
#include "../Transform.h"
#include "MeshRenderer.h"
#include "../Mesh.h"
#include "../Material.h"
#include "../Shader.h"
#include "../ShaderManager.h"
#include "../Device.h"
#include "../RasterizerState.h"
#include "../RenderManager.h"
#include "../Input.h"

SSS_USING

COutLine::COutLine() :
	m_pOutLineShader(NULL)
{
	memset(&m_tConstantBuffer, 0, sizeof(FXOUTLINE));

	m_tConstantBuffer.fFXOutlineSize = 1.05f;
	m_tConstantBuffer.vFXOutlineColor = Vector4::blue;
	m_tConstantBuffer.vFXOutlineOffset = Vector3::Zero;
	m_tConstantBuffer.vFXOutlineOffset.y = -0.4f;
}

COutLine::~COutLine()
{
	SAFE_RELEASE(m_pOutLineShader);
}


void COutLine::SetOutlineSize(float fSize)
{
}

void COutLine::SetOutlineColor(const Vector4 & vColor)
{
}

void COutLine::SetOutlineOffset(const Vector3 & vOffset)
{
}

float COutLine::GetOutlineSize()
{
	return 0.0f;
}

Vector4 COutLine::GetOutlineColor()
{
	return Vector4();
}

Vector4 COutLine::GetOutlineOffset()
{
	return Vector4();
}

bool COutLine::Initialize()
{
	//	아웃 라인용 fx를 쓰기 위해 셰이더를 등록해준다.
	SetOutLineShader(OUTLINE_SHADER);	
	return true;
}

int COutLine::Prerender(CMeshRenderer*	pRenderer)
{	
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(FX_OUTLINE_CBUFFER, &m_tConstantBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);

	CMaterial*	pMaterial = pRenderer->GetMaterial();

	CRenderState* pRenderState = GET_SINGLE(CRenderManager)->FindRenderState(CULLFRONT);

	pRenderState->SetState();




	// 재질정보를 설정한다.
	for (size_t i = 0; i < pMaterial->GetContainerCount(); ++i)
	{
		for (int j = 0; j < pMaterial->GetSubsetCount(i); ++j)
		{

			DEVICE_CONTEXT->IASetInputLayout(pRenderer->GetInputLayout());

			m_pOutLineShader->SetShader();	// 아웃라인 셰이더의 버텍스 셰이더와 픽셀 셰이더를 렌더링 파이프 라인에 등록한다.

			pRenderer->GetMesh()->Render(i, j);	// 메쉬의 정점을 출력한다.

		}
	}

	pRenderState->ResetState();

	SAFE_RELEASE(pRenderState);
	SAFE_RELEASE(pMaterial);

	//UpdateConstantBuffer();
	return 0;
}

void COutLine::SetOutLineShader(const string & strKey)
{
	SAFE_RELEASE(m_pOutLineShader);
	m_pOutLineShader = GET_SINGLE(CShaderManager)->FindShader(strKey);
}

bool COutLine::Save(FILE * pFile)
{
	fwrite(&m_tConstantBuffer, sizeof(FXOUTLINE), 1, pFile);

	string strShaderKey = m_pOutLineShader->GetTag();
	size_t iKeyLength = strShaderKey.length();

	fwrite(&iKeyLength, sizeof(size_t), 1, pFile);
	fwrite(strShaderKey.c_str(), sizeof(char), iKeyLength, pFile);

	return true;
}

bool COutLine::Load(FILE * pFile)
{
	fread(&m_tConstantBuffer, sizeof(FXOUTLINE), 1, pFile);

	char strShaderKey[256] = {};
	size_t iKeyLength = 0;

	fread(&iKeyLength, sizeof(size_t), 1, pFile);
	fread(&strShaderKey[0], sizeof(char), iKeyLength, pFile);

	SetOutLineShader(strShaderKey);

	return true;
}
