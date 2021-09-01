#include "Water.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "../Scene.h"
#include "../Transform.h"
#include "../ShaderManager.h"
#include "../Material.h"
#include "../RenderManager.h"

SSS_USING

CWater::CWater()
{
	memset(&m_tCBuffer, 0, sizeof(WATERCBUFFER));
	m_fCurrentSpeed = 0.f;
	m_fFlowingSpeed = 0.1f;
}


CWater::~CWater()
{
}

bool CWater::Initialize()
{
	CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();

	// Render Group ����
	m_pGameObject->SetTransparency(true);

	if (!pRenderer)
	{
		pRenderer = m_pGameObject->AddComponent<CMeshRenderer>();

		pRenderer->SetMesh("WaterMesh", true);
		pRenderer->SetShader("WaterShader");
		pRenderer->SetInputLayout(BUMP_LAYOUT);

		pRenderer->SetRenderState(ALPHA_BLEND);
		pRenderer->SetRenderState(CULLNONE);

		CMaterial* pMaterial = pRenderer->GetMaterial();

		pMaterial->SetSampler(SAMPLER_LINEAR);
		// Normal Texure ���� Bump�� ������ Normal Texture�� ���ϴ�
		// Specular Texure ���� ������ ������ Derangement Texure�� ���ϴ�

		SAFE_RELEASE(pMaterial);

		SAFE_RELEASE(pRenderer);
	}

	else
	{
		pRenderer->SetMesh("WaterMesh");
		pRenderer->SetShader("WaterShader");
		pRenderer->SetInputLayout(BUMP_LAYOUT);

		pRenderer->SetRenderState(ALPHA_BLEND);
		pRenderer->SetRenderState(CULLNONE);

		CMaterial* pMaterial = pRenderer->GetMaterial();

		pMaterial->SetSampler(SAMPLER_LINEAR);
		// Normal Texure ���� Bump�� ������ Normal Texture�� ���ϴ�
		// Specular Texure ���� ������ ������ Derangement Texure�� ���ϴ�

		SAFE_RELEASE(pMaterial);

		SAFE_RELEASE(pRenderer);
	}

	return true;
}

int CWater::LateUpdate(float fTime)
{
	m_fCurrentSpeed += fTime * m_fFlowingSpeed;

	if (m_fCurrentSpeed > 1000.f)
		m_fCurrentSpeed = 0.f;
	
	//�� ������ ����
	Vector3 vPos = m_pTransform->GetWorldPosition();
	vPos.y = -10.f + 0.1f * sinf(m_fCurrentSpeed * 40.f);
	m_pTransform->SetWorldPosition(vPos);

	m_tCBuffer.fWaterSpeed = m_fCurrentSpeed;

	CTransform*	pCameraTr = m_pScene->GetMainCameraTransform();

	Vector3 vCameraPos = pCameraTr->GetWorldPosition();

	// ���� y ���� ���� ���� ��ġ���ִ��� �Ʒ��� ��ġ�� �ִ��� �˻��Ѵ�
	if (vCameraPos.y < m_pTransform->GetWorldPosition().y)
		m_tCBuffer.iCameraPos = 0;
	else
		m_tCBuffer.iCameraPos = 1;

	SAFE_RELEASE(pCameraTr);

	return 0;
}

int CWater::Prerender(CMeshRenderer * pRenderer)
{
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer("WaterCBuffer", &m_tCBuffer, CBT_VERTEX | CBT_PIXEL);
	
	GET_SINGLE(CRenderManager)->SetDirectionalLight();

	return 0;
}

bool CWater::Save(FILE * pFile)
{
	return true;
}

bool CWater::Load(FILE * pFile)
{
	return true;
}
