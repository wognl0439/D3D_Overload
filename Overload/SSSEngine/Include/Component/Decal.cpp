#include "Decal.h"
#include "MeshRenderer.h"

SSS_USING

CDecal::CDecal()
{
}

CDecal::~CDecal()
{
}

// Decal Object�� Material�� Diffuse, Normal, Specular �ؽ��ĸ� ������ �־�� �մϴ�..
bool CDecal::Initialize()
{
	CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();

	// ���� ������Ʈ�� Renderer�� ������� Renderer ������Ʈ ���� �߰�
	if (!pRenderer)
	{
		pRenderer = m_pGameObject->AddComponent<CMeshRenderer>();

		pRenderer->SetMesh("EngineDecalMesh");
		pRenderer->SetShader("DecalShader");
		pRenderer->SetInputLayout(POS_LAYOUT);
		pRenderer->SetRenderState(CULLNONE);

		SAFE_RELEASE(pRenderer);
	}

	// Renderer�� ������� ������ ������Ʈ�� ����
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
