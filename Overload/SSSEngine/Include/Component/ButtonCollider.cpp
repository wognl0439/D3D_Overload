#include "ButtonCollider.h"
#include "../Scene.h"
#include "../Transform.h"
#include "../ShaderManager.h"
#include "Camera.h"
#include "Renderer.h"
#include "../ResourcesManager.h"
#include "../Mesh.h"
#include "../Shader.h"
#include "../Device.h"

SSS_USING

CButtonCollider::CButtonCollider():
	m_bCollided(false)
{
	m_eComponentType = CT_BUTTON_COLLIDER;
	SetTag("ButtonCollider");


#ifdef _DEBUG
	m_pShader = GET_SINGLE(CShaderManager)->FindShader(COLLIDER_SHADER);
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(ENGINE_MESH_FRAME_RECT_COLOR);
	m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_COLOR_LAYOUT);

	m_vColor = Vector4::green;

#endif // _DEBUG
}

CButtonCollider::CButtonCollider(const CButtonCollider & component)
{
}

CButtonCollider::~CButtonCollider()
{
#ifdef _DEBUG
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pMesh);	
#endif // _DEBUG
}


#ifdef _DEBUG
void CButtonCollider::SetColor(const Vector4 & vColor)
{
	m_vColor = vColor;
}

Vector4 CButtonCollider::GetColor() const
{
	return m_vColor;
}
#endif // _DEBUG

Vector2 CButtonCollider::GetPivot() const
{
	return m_vPivot;
}

Vector2 CButtonCollider::GetScale() const
{
	return m_vScale;
}

bool CButtonCollider::GetCollision() const
{
	return m_bCollided;
}

void CButtonCollider::SetPivot(const Vector2 & vPivot)
{
	m_vPivot = vPivot;
}

void CButtonCollider::SetPivot(float x, float y)
{
	m_vPivot = Vector2(x, y);
}

void CButtonCollider::SetScale(const Vector2 & vScale)
{
	m_vScale = vScale;
}

void CButtonCollider::SetScale(float fWidth, float fHeight)
{
	m_vScale = Vector2(fWidth, fHeight);
}

void CButtonCollider::SetCollision(bool bCollision)
{
	m_bCollided = bCollision;
}

bool CButtonCollider::Initialize()
{
	m_pScene->AddButtonCollider(this);
	return true;
}

void CButtonCollider::Render(float fTime)
{
#ifdef _DEBUG
	DEVICE_CONTEXT->IASetInputLayout(m_pLayout);
	m_pShader->SetShader();

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


	
	//UI 이므로 z 축은 필요가 없다.
	Vector3 vScale = Vector3(m_vScale.x, m_vScale.y, 1.0f);
	
	Matrix	matScale, matPos, matWorld;
	matScale = Matrix::Scaling(vScale);
	matPos = Matrix::Translation(vPos);
	matWorld = matScale * m_pTransform->GetWorldRotationMatrix() * matPos;

	CCamera*	pCamera = m_pScene->GetMainCamera();

	tBuffer.matWorld = matWorld;
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProjection = pCamera->GetProjectionMatrix();
	tBuffer.matWV = matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
	tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
	tBuffer.matWP = tBuffer.matWorld * tBuffer.matProjection;
	tBuffer.vPivot = m_vPivot;
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

#endif // _DEBUG
}

bool CButtonCollider::CheckCollision(const Vector2 & vPosition)
{
	Vector2 position = m_pTransform->GetWorldPosition();

	Vector2 LT = Vector2(position.x , position.y) - Vector2(m_vScale.x * m_vPivot.x, m_vScale.y * m_vPivot.y);
	Vector2 RB = LT + m_vScale;
	
	if (vPosition.x >= LT.x && vPosition.x <= RB.x && vPosition.y <= RB.y && vPosition.y >= LT.y)
	{
		return true;
	}

	return false;
}
