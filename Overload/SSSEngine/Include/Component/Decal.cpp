#include "Decal.h"
#include "MeshRenderer.h"

SSS_USING

CDecal::CDecal()
{
}

CDecal::~CDecal()
{
}

// Decal Object의 Material에 Diffuse, Normal, Specular 텍스쳐를 지정해 주어야 합니당..
bool CDecal::Initialize()
{
	CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();

	// 현재 오브젝트에 Renderer가 없을경우 Renderer 컴포넌트 새로 추가
	if (!pRenderer)
	{
		pRenderer = m_pGameObject->AddComponent<CMeshRenderer>();

		pRenderer->SetMesh("EngineDecalMesh");
		pRenderer->SetShader("DecalShader");
		pRenderer->SetInputLayout(POS_LAYOUT);
		pRenderer->SetRenderState(CULLNONE);

		SAFE_RELEASE(pRenderer);
	}

	// Renderer가 있을경우 기존의 컴포넌트에 셋팅
	else
	{
		pRenderer->SetMesh("EngineDecalMesh");
		pRenderer->SetShader("DecalShader");
		pRenderer->SetInputLayout(POS_LAYOUT);
		pRenderer->SetRenderState(CULLNONE);

		SAFE_RELEASE(pRenderer);
	}

	return true;
}
