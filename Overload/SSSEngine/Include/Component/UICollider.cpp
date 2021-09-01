#include "UICollider.h"
#include "../Scene.h"
#include "../Transform.h"
#include "../ShaderManager.h"
#include "Camera.h"
#include "SpriteRenderer.h"
#include "../ResourcesManager.h"
#include "../Mesh.h"
#include "../Shader.h"
#include "../Device.h"
#include "../Core.h"

SSS_USING

CUICollider::CUICollider() :
	m_bCollided(false),
	m_vLocalScale(Vector2(1.0f, 1.0f))
{
	SetTag("UICollider");

	m_pShader = GET_SINGLE(CShaderManager)->FindShader(COLLIDER_SHADER);
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(ENGINE_MESH_FRAME_RECT_COLOR);
	m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_COLOR_LAYOUT);
	m_vColor = Vector4::green;

}

CUICollider::CUICollider(const CUICollider & component)
{
	if (this == GET_SINGLE(CCollisionManager)->GetCoveredUI())
	{
		GET_SINGLE(CCollisionManager)->ClearCoveredUI();
	}
}

CUICollider::~CUICollider()
{
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pMesh);
}


void CUICollider::SetColor(const Vector4 & vColor)
{
	m_vColor = vColor;
}

Vector4 CUICollider::GetColor() const
{
	return m_vColor;
}

Vector2 CUICollider::GetLocalScale() const
{
	return m_vLocalScale;
}

bool CUICollider::GetCollision() const
{
	return m_bCollided;
}

void CUICollider::SetLocalScale(const Vector2 & vScale)
{
	m_vLocalScale = vScale;
}

void CUICollider::SetLocalScale(float fWidth, float fHeight)
{
	m_vLocalScale = Vector2(fWidth, fHeight);
}

void CUICollider::SetCollision(bool bCollision)
{
	m_bCollided = bCollision;
}

bool CUICollider::Initialize()
{
	m_pScene->AddUICollider(this);
	return true;
}

void CUICollider::RenderDebug(float fTime)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		DEVICE_CONTEXT->IASetInputLayout(m_pLayout);
		m_pShader->SetShader();

		TRANSFORMCBUFFER	tBuffer = {};

		Vector2 vPivot = m_pTransform->GetPivot();
		Vector3 vPosition = m_pTransform->GetWorldPosition();
		Vector3 vScale = m_vLocalScale * m_pTransform->GetWorldScale();

		Matrix	matScale, matPos, matWorld;
		matScale = Matrix::Scaling(vScale);
		matPos = Matrix::Translation(vPosition - ( vPivot * vScale));
		matWorld = matScale * m_pTransform->GetWorldRotationMatrix() * matPos;

		CCamera*	pCamera = m_pScene->GetMainCamera();

		tBuffer.matWorld = matWorld;
		tBuffer.matView = pCamera->GetViewMatrix();
		tBuffer.matProjection = pCamera->GetProjectionMatrix();
		tBuffer.matWV = matWorld * tBuffer.matView;
		tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
		tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
		tBuffer.matWP = tBuffer.matWorld * tBuffer.matProjection;
		tBuffer.vPivot = Vector3::Zero;
		tBuffer.vLength = m_pMesh->GetLength();

		SAFE_RELEASE(pCamera);

		tBuffer.matWorld = tBuffer.matWorld.Transpose();
		tBuffer.matView = tBuffer.matView.Transpose();
		tBuffer.matProjection = tBuffer.matProjection.Transpose();
		tBuffer.matWV = tBuffer.matWV.Transpose();
		tBuffer.matWVP = tBuffer.matWVP.Transpose();
		tBuffer.matVP = tBuffer.matVP.Transpose();
		tBuffer.matWP = tBuffer.matWP.Transpose();

		// Transform 정보를 Shader 상수버퍼로 넘겨준다.
		GET_SINGLE(CShaderManager)->UpdateConstantBuffer(TRANSFORM_CBUFFER, &tBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);
		COLLIDERCBUFFER tColliderBuffer = {};

		tColliderBuffer.bUI = 1;
		tColliderBuffer.vColor = m_vColor;

		GET_SINGLE(CShaderManager)->UpdateConstantBuffer(COLLIDER_CBUFFER, &tColliderBuffer, CBT_VERTEX);

		m_pMesh->Render();
	}

}

bool CUICollider::CheckCollision(const Vector2 & vPosition)
{
	Vector2 vPivot = m_pTransform->GetPivot();
	Vector2 vCenterPosition = m_pTransform->GetWorldPosition() ;
	Vector2 vLength = m_vLocalScale * m_pTransform->GetWorldScale();

	Vector2 LT = vCenterPosition - (vPivot * vLength);
	Vector2 RB = LT + vLength;

	if (vPosition.x >= LT.x && vPosition.x <= RB.x && vPosition.y <= RB.y && vPosition.y >= LT.y)
	{
		return true;
	}

	return false;
}

void CUICollider::OnMouseEnter(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
 		m_vColor = Vector4::red;
	}
}

void CUICollider::OnMouseStay(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
}

void CUICollider::OnMouseExit(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		m_vColor = Vector4::green;
	}
}

void CUICollider::OnMouseDown(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		m_vColor = Vector4::blue;
	}
}

void CUICollider::OnMouseDrag(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
}

void CUICollider::OnMouseUp(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		if (m_bCollided)
		{
			m_vColor = Vector4::red;
		}
	}
}

bool CUICollider::Save(FILE * pFile)
{
	fwrite(&m_vLocalScale, sizeof(Vector2), 1, pFile);
	return true;
}

bool CUICollider::Load(FILE * pFile)
{
	fread(&m_vLocalScale, sizeof(Vector2), 1, pFile);
	return true;
}
