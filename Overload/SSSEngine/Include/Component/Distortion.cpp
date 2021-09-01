#include "Distortion.h"
#include "MeshRenderer.h"
#include "../Material.h"
#include "Trail.h"
#include "../ResourcesManager.h"
#include "../Sampler.h"
#include "Billboard.h"

SSS_USING

CDistortion::CDistortion()
{
}

CDistortion::~CDistortion()
{
}

bool CDistortion::Initialize()
{
	CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();

	// ���� ������Ʈ�� Renderer�� ������� Renderer ������Ʈ ���� �߰�
	if (!pRenderer)
	{
		pRenderer = m_pGameObject->AddComponent<CMeshRenderer>();
		pRenderer->SetShader("DistortionShader");
		pRenderer->SetInputLayout(BUMP_LAYOUT);
		pRenderer->SetRenderState(CULLNONE);
	}

	// Renderer�� ������� ������ ������Ʈ�� ����
	else
	{
		pRenderer->SetShader("DistortionShader");
		pRenderer->SetInputLayout(BUMP_LAYOUT);
		pRenderer->SetRenderState(CULLNONE);
	}

	if (m_pGameObject->HasComponent<CBillboard>())
	{
		pRenderer->SetMesh(ENGINE_MESH_BILLBOARD, true);
		pRenderer->SetShader(BILLBOARD_SHADER);
		pRenderer->SetRenderState(ALPHA_BLEND);
		pRenderer->SetInputLayout(POS_LAYOUT);
	}

	SAFE_RELEASE(pRenderer);

	return true;
}

int CDistortion::Prerender(CMeshRenderer * pRenderer)
{
	// Distortion �� Clamp ���÷� 
	CSampler* pSampler = GET_SINGLE(CResourcesManager)->FindSampler("POINT_MIRROR");
	pSampler->SetSampler(1);
	SAFE_RELEASE(pSampler);

	return 0;
}

bool CDistortion::Save(FILE * pFile)
{
	return true;
}

bool CDistortion::Load(FILE * pFile)
{
	return true;
}
