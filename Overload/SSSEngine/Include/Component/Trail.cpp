#include "Trail.h"
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
#include "../Scene.h"
#include "Distortion.h"

SSS_USING


CTrail::CTrail() :
	m_pTrailShader(NULL),
	m_iCount(0)
{
	m_fTime = 0.f;
	m_fSize = 0.f;
}


CTrail::~CTrail()
{
	SAFE_RELEASE(m_pTrailShader);
}

void CTrail::SetSize(float fSize)
{
	m_fSize = fSize;
}

float CTrail::GetSize() const
{
	return m_fSize;
}

void CTrail::SetPositionInAllList(Vector3 vPos)
{
	m_VertexPosList.clear();
	for (int i = 0; i < 10; ++i)
		m_VertexPosList.push_front(vPos);
}



bool CTrail::Initialize()
{
	m_pGameObject->SetTransparency(true);

	CMeshRenderer*	pRenderer = GetComponent<CMeshRenderer>();

	if (pRenderer)
	{
		pRenderer->SetMesh(ENGINE_MESH_BILLBOARD);
		pRenderer->SetShader("TrailShader");
		pRenderer->SetRenderState(ALPHA_BLEND);
		pRenderer->SetRenderState(CULLNONE);
	}

	CDistortion* pDistortion = GetComponent<CDistortion>();
	if (pDistortion)
	{
		pRenderer->SetShader("TrailDistortionShader");
		SAFE_RELEASE(pDistortion);
	}

	SAFE_RELEASE(pRenderer);

	return true;
}

int CTrail::LateUpdate(float fTime)
{
	m_fTime += fTime;
	CMeshRenderer*	pRenderer = GetComponent<CMeshRenderer>();
	if (pRenderer)
	{
		CTransform* pTr = m_pTransform->GetParent();

		if (pTr == NULL)
			return 0;

		//m_pTrailCBuffer.vTrailAxisX = pTr->GetLocalAxis(AX_Z);
		m_pTrailCBuffer.vTrailAxisX = pTr->GetWorldAxis(AX_Z);
		//m_pTrailCBuffer.vTrailAxisX = m_pTransform->GetLocalAxis(AX_Z);
		//m_pTrailCBuffer.vTrailAxisX = Vector3(0.f, 0.f, 1.f);
		
		if (m_fTime > 0.01f)
		{
			m_fTime = 0.f;

			m_pTrailCBuffer.vTrailCurrentPos = pTr->GetWorldPosition();
			m_VertexPosList.push_front(m_pTrailCBuffer.vTrailCurrentPos);
		}

		m_pTrailCBuffer.fTrailScale = m_fSize;

		list<Vector3>::iterator	iter;
		list<Vector3>::iterator	iterEnd = m_VertexPosList.end();

		int iCount = 0;

		for (iter = m_VertexPosList.begin(); iter != iterEnd; ++iter)
		{
			if (iCount >= 10)
			{
				iCount = 10;
				// 10 ~ 마지막은 범위를 벗어나므로..
				m_VertexPosList.erase(iter, iterEnd);
				break;
			}

			m_pTrailCBuffer.vTrailVelocity[iCount] = *iter;

			++iCount;
		}

		m_pTrailCBuffer.iTrailSize = iCount;

		SAFE_RELEASE(pRenderer);
	}

	return 0;
}

int CTrail::Prerender(CMeshRenderer * pRenderer)
{
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer("TrailCBuffer", &m_pTrailCBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);

	return 0;
}

bool CTrail::Save(FILE * pFile)
{
	fwrite(&m_fSize, sizeof(float), 1, pFile);

	return true;
}

bool CTrail::Load(FILE * pFile)
{
	fread(&m_fSize, sizeof(float), 1, pFile);

	return true;
}
