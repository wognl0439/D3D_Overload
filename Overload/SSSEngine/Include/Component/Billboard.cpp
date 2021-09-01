#include "Billboard.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "../Scene.h"
#include "../Transform.h"
#include "../ShaderManager.h"
#include "Fire.h"
#include "Distortion.h"

SSS_USING

CBillboard::CBillboard()
{
	memset(&m_tCBuffer, 0, sizeof(BILLBOARDCBUFFER));
}

CBillboard::~CBillboard()
{
}

bool CBillboard::Initialize()
{
	m_bRotate = false;

	CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();
	if (!pRenderer)
	{
		pRenderer = m_pGameObject->AddComponent<CMeshRenderer>();
		pRenderer->SetMesh(ENGINE_MESH_BILLBOARD, true);
		pRenderer->SetShader(BILLBOARD_SHADER);
		pRenderer->SetRenderState(ALPHA_BLEND);
		pRenderer->SetInputLayout(POS_LAYOUT);
	}
	else
	{
		pRenderer->SetMesh(ENGINE_MESH_BILLBOARD);
		pRenderer->SetShader(BILLBOARD_SHADER);
		pRenderer->SetRenderState(ALPHA_BLEND);
		pRenderer->SetInputLayout(POS_LAYOUT);
	}

	CFire* pFire = GetComponent<CFire>();
	if (pFire)
	{
		pRenderer->SetShader("FireBillBoardShader");
	}
	SAFE_RELEASE(pFire);
	
	SAFE_RELEASE(pRenderer);
	
	return true;
}

void CBillboard::SetRotate(bool bRotate)
{
	m_bRotate = bRotate;
}

int CBillboard::Prerender(CMeshRenderer * pRenderer)
{
	memset(&m_tCBuffer, 0, sizeof(BILLBOARDCBUFFER));

	m_tCBuffer.vCenter = m_pTransform->GetWorldPosition();
	m_tCBuffer.fSizeX = m_pTransform->GetWorldScale().x;
	m_tCBuffer.fSizeY = m_pTransform->GetWorldScale().y;
	//m_tCBuffer.fPadding = m_pTransform->GetWorldRotation().z;

	CTransform*	pCameraTransform = m_pScene->GetMainCameraTransform();

	//m_tCBuffer.vAxisX = pCameraTr->GetWorldAxis(AX_X);
	//m_tCBuffer.vAxisY = pCameraTr->GetWorldAxis(AX_Y);
	float fAngle = m_pTransform->GetWorldRotation().z;
	Matrix mat = pCameraTransform->GetWorldMatrix();
	if(m_bRotate)
		mat = XMMatrixRotationZ(fAngle) * mat;
		
	Vector3 AxisX = Vector3(mat.fMat[0][0], mat.fMat[0][1], mat.fMat[0][2]).Normalize();
	Vector3 AxisY = Vector3(mat.fMat[1][0], mat.fMat[1][1], mat.fMat[1][2]).Normalize();
	
	m_tCBuffer.vAxisX = AxisX;
	m_tCBuffer.vAxisY = AxisY;

	SAFE_RELEASE(pCameraTransform);

	if (m_pGameObject->HasComponent<CDistortion>())
	{
		m_tCBuffer.fPadding = 1.0f;
	}

	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(BILLBOARD_CBUFFER, &m_tCBuffer, CBT_VERTEX | CBT_GEOMETRY | CBT_PIXEL);
	
	return 0;
}

bool CBillboard::Save(FILE * pFile)
{
	//fwrite(&m_tCBuffer, sizeof(BILLBOARDCBUFFER), 1, pFile);

	return true;
}

bool CBillboard::Load(FILE * pFile)
{
	//fread(&m_tCBuffer, sizeof(BILLBOARDCBUFFER), 1, pFile);

	return true;
}
