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

	// 현재 오브젝트에 Renderer가 없을경우 Renderer 컴포넌트 새로 추가
	if (!pRenderer)
	{
		pRenderer = m_pGameObject->AddComponent<CMeshRenderer>();
		pRenderer->SetShader("DistortionShader");
		pRenderer->SetInputLayout(BUMP_LAYOUT);
		pRenderer->SetRenderState(CULLNONE);
	}

	// Renderer가 있을경우 기존의 컴포넌트에 셋팅
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
	// Distortion 용 Clamp 샘플러 
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
