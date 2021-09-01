#include "CircleCollider2D.h"
#include "../Transform.h"
#include "../GameObject.h"
#include "../CollisionManager.h"
#include "../ResourcesManager.h"
#include "../ShaderManager.h"
#include "Camera.h"
#include "../Mesh.h"
#include "../Scene.h"
#include "../Layer.h"
#include "../Core.h"

SSS_USING

CCircleCollider2D::CCircleCollider2D():
	m_fRadius(10.0f)
{
	SetTag("CircleCollider2D");

	m_pShader = GET_SINGLE(CShaderManager)->FindShader(STANDARD_COLOR_SHADER);
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(ENGINE_MESH_FRAME_CIRCLE_COLOR);
	m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_COLOR_LAYOUT);
}


CCircleCollider2D::CCircleCollider2D(const CCircleCollider2D & component) :
	CCollider(component)
{
	*this = component;
}

CCircleCollider2D::~CCircleCollider2D()
{
}

float CCircleCollider2D::GetRadius() const
{
	return m_fRadius;
}

void CCircleCollider2D::SetRadius(float fRadius)
{
}


int CCircleCollider2D::LateUpdate(float fTime)
{
	return 0;
}

void CCircleCollider2D::RenderDebug(float fDeltaTime)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{

		TRANSFORMCBUFFER	tBuffer = {};

		//영벡터
		Vector3 vMin;
		//메쉬의 버텍스는 길이가 1이므로, scale이 max가 된다.
		Vector3 vMax = m_vScale;
		Vector3 vLength = vMax - vMin;
		Vector3 vRotation = m_pTransform->GetWorldRotation();

		// 피봇을 적용한 위치를 구해준다.
		Vector3	vPos = vMin + vLength * m_vPivot;
		Vector3 vLocalTransformPivotLength = (m_pTransform->GetWorldPosition() - m_pTransform->GetPivot() * m_pTransform->GetWorldScale());
		Vector3 vLocalColliderPivotLength = m_vPivot * vLength;

		vPos = m_pTransform->GetWorldPosition();


		//m_vPoint[0] = vPos + Vector3( m_pTransform->right().x * m_vScale.x, m_pTransform->up());


		//z축의 길이가 0이면 출력 할 수 없으므로, 1로 변경해준다.
		if (m_vScale.z == 0.f)
			m_vScale.z = 1.f;

		Matrix	matScale, matPos, matWorld;
		matScale = Matrix::Scaling(m_vScale);
		matPos = Matrix::Translation(vPos);
		matWorld = matScale * m_pTransform->GetWorldRotationMatrix() * matPos;

		CCamera*	pCamera = m_pScene->GetMainCamera();

		tBuffer.matWorld = matWorld;
		tBuffer.matView = pCamera->GetViewMatrix();
		tBuffer.matProjection = pCamera->GetProjectionMatrix();
		tBuffer.matWV = matWorld * tBuffer.matView;
		tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
		tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
		tBuffer.vPivot = m_vPivot;

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

		// Transform 정보를 Shader 상수버퍼로 넘겨준다.
		GET_SINGLE(CShaderManager)->UpdateConstantBuffer(TRANSFORM_CBUFFER, &tBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);

		CCollider::RenderDebug(fDeltaTime);
	}
}

CCircleCollider2D * CCircleCollider2D::Clone() const
{
	return new CCircleCollider2D(*this);
}

bool CCircleCollider2D::Save(FILE * pFile)
{
	fwrite(&m_fRadius, sizeof(float), 1, pFile);
	fwrite(&m_vScale, sizeof(Vector3), 1, pFile);
	fwrite(&m_vPivot, sizeof(Vector3), 1, pFile);

	return true;
}

bool CCircleCollider2D::Load(FILE * pFile)
{
	fread(&m_fRadius, sizeof(float), 1, pFile);
	fread(&m_vScale, sizeof(Vector3), 1, pFile);
	fread(&m_vPivot, sizeof(Vector3), 1, pFile);

	return true;
}
