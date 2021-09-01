#include "Camera.h"
#include "../Device.h"
#include "../Transform.h"
#include "../Input.h"
#include "../Core.h"

SSS_USING


CCamera::CCamera()
{
	m_bUseShadow = false;

	SetTag("Camera");

	m_matView = new Matrix;
	m_matOrthoProjection = new Matrix;
	m_matPerspectiveProjection = new Matrix;

	m_matOldView = new Matrix;
	m_matOldVP = new Matrix;
	m_matShadowView = new Matrix;
	m_matShadowProj = new Matrix;

	m_vLightforShadowPos = Vector3(474.f, 452.f, -160.f);
	m_vLightLookAtforShadowPos = Vector3(200.0f, 50.0f, -100.0f);

	m_tShadowRS.iWidth = 2000;
	m_tShadowRS.iHeight = 2000;
}

CCamera::CCamera(const CCamera & component):
	CComponent(component)
{
	*this = component;

	m_matView = new Matrix;
	m_matOrthoProjection = new Matrix;
	m_matPerspectiveProjection = new Matrix;

	*m_matView = *component.m_matView;
	*m_matOrthoProjection = *component.m_matOrthoProjection;
	*m_matPerspectiveProjection = *component.m_matPerspectiveProjection;

	m_matOldView = new Matrix;
	m_matOldVP = new Matrix;
	m_matShadowView = new Matrix;
	m_matShadowProj = new Matrix;

	*m_matOldView = *component.m_matOldView;
	*m_matOldVP = *component.m_matOldVP;
	*m_matShadowView = *component.m_matShadowView;
	*m_matShadowProj = *component.m_matShadowProj;
	m_vLightforShadowPos = component.m_vLightforShadowPos;
	m_vLightLookAtforShadowPos = component.m_vLightLookAtforShadowPos;
}


CCamera::~CCamera()
{
	SAFE_DELETE(m_matView);
	SAFE_DELETE(m_matOrthoProjection);
	SAFE_DELETE(m_matPerspectiveProjection);
	SAFE_DELETE(m_matOldView);
	SAFE_DELETE(m_matOldVP);
	SAFE_DELETE(m_matShadowView);
	SAFE_DELETE(m_matShadowProj);
}

void CCamera::CreateFrustum(const Matrix & matInvVP)
{
	FRUSTUM tFrustum;
	Vector3	vPos[8];

	// 절두체의 육면체를 구성할 월드공간에서의 위치정보를 구한다.
	for (int i = 0; i < 8; ++i)
	{
		vPos[i] = tFrustum.vPos[i].TransformCoord(matInvVP);
	}

	// 평면의 방정식 : ax + by + cz + d = 0
	// 임의의점 : P
	// 평면의 법선 : N
	// N.P + d = 0
	// d = -N.P
	// 6개의 평면정보를 만든다.
	// 왼쪽
	tFrustum.vPlane[FP_LEFT] = XMPlaneFromPoints(vPos[4].Convert(), vPos[0].Convert(), vPos[2].Convert());

	// 오른쪽
	tFrustum.vPlane[FP_RIGHT] = XMPlaneFromPoints(vPos[1].Convert(), vPos[5].Convert(), vPos[7].Convert());

	// 위
	tFrustum.vPlane[FP_TOP] = XMPlaneFromPoints(vPos[4].Convert(), vPos[5].Convert(), vPos[1].Convert());

	// 아래
	tFrustum.vPlane[FP_BOTTOM] = XMPlaneFromPoints(vPos[2].Convert(), vPos[3].Convert(), vPos[7].Convert());

	// 앞
	tFrustum.vPlane[FP_NEAR] = XMPlaneFromPoints(vPos[0].Convert(), vPos[1].Convert(), vPos[3].Convert());

	// 뒤
	tFrustum.vPlane[FP_FAR] = XMPlaneFromPoints(vPos[5].Convert(), vPos[4].Convert(), vPos[6].Convert());

	m_tFrustum = tFrustum;
}

bool CCamera::PointOutsideFrustum(const Vector3 & vPos)
{
	for (int i = 0; i < FP_END; ++i)
	{
		float	fDist = XMVectorGetX(XMPlaneDotCoord(m_tFrustum.vPlane[i].Convert(), vPos.Convert()));

		if (fDist > 0.f)
			return true;
	}

	return false;
}

bool CCamera::SphereOutsideFrustum(const Vector3 & vCenter, float fRadius)
{
	for (int i = 0; i < FP_END; ++i)
	{
		float	fDist = XMVectorGetX(XMPlaneDotCoord(m_tFrustum.vPlane[i].Convert(), vCenter.Convert()));

		//각 평면을 돌면서 평면과 원점 사이의 거리가 반지름보다 멀리 있는 애를 찾는다.
		if (fDist > fRadius)
			//Frustum 밖에 있음.
			return true;
	}

	//Frustum 안에 있음.
	return false;
}

Matrix CCamera::GetPerspectiveProjectionMatrix() const
{
	return *m_matPerspectiveProjection;
}

Matrix CCamera::GetOrthographyProjectionMatrix() const
{
	return *m_matOrthoProjection;
}

Matrix CCamera::GetViewMatrix() const
{
	return *m_matView;
}

Matrix CCamera::GetProjectionMatrix() const
{
	return *m_matOrthoProjection;
}

Matrix CCamera::GetOldView() const
{
	return *m_matOldView;
}

Matrix CCamera::GetOldVP() const
{
	return *m_matOldVP;
}


void CCamera::SetPerspectiveProjection(float fAspect, float fViewAngle, float fNear, float fFar)
{
	*m_matPerspectiveProjection = XMMatrixPerspectiveFovLH(fViewAngle, fAspect, fNear, fFar);
}

void CCamera::SetOrthographyProjection(float fLeft, float fRight, float fTop, float fBottom, float fNear, float fFar)
{
	*m_matOrthoProjection = XMMatrixOrthographicOffCenterLH(fLeft, fRight, fBottom, fTop, fNear, fFar);
}

RAY CCamera::ScreenPointToRay(const Vector2 & vScreenPoint)
{	
	RAY tRay;

	tRay.vDirection.x = (vScreenPoint.x / (DEVICE_RESOLUTION.iWidth / 2.0f) - 1.0f) / m_matPerspectiveProjection->_11;
	tRay.vDirection.y = (-vScreenPoint.y / (DEVICE_RESOLUTION.iHeight / 2.0f) + 1.0f) / m_matPerspectiveProjection->_22;
	tRay.vDirection.z = 1.0f;

	Matrix matInvView = *m_matView;
	matInvView = matInvView.Inverse();

	tRay.vOrigin = tRay.vOrigin.TransformCoord(matInvView);
	tRay.vDirection = tRay.vDirection.TransformNormal(matInvView);
	tRay.vDirection = tRay.vDirection.Normalize();
	tRay.fDistance = 10000000.0f;
	
	return tRay;
}

Vector2 CCamera::WorldToScreenPoint(Vector3 vPosition)
{
		Matrix matVP = *m_matView * *m_matPerspectiveProjection;			
				
		Vector4 vPos = XMVector4Transform(Vector4(vPosition.x, vPosition.y, vPosition.z, 1.0f), matVP);
		Vector3 vProjection(vPos.x / vPos.w, vPos.y / vPos.w, vPos.z / vPos.w);

		if (vProjection.x > 1.0f)
			return Vector2::Infinity;
		if(vProjection.x < -1.0f)
			return Vector2::Infinity;
		if(vProjection.y > 1.0f)
			return Vector2::Infinity;
		if(vProjection.y < -1.0f)
			return Vector2::Infinity;
		if(vProjection.z < 0.0f)
			return Vector2::Infinity;
		
		Vector2 vView((vProjection.x * 0.5f) + 0.5f, (vProjection.y * 0.5f) + 0.5f);
		return Vector2(vView.x * (float)DEVICE_RESOLUTION.iWidth, (1.0f - vView.y) * (float)DEVICE_RESOLUTION.iHeight);
}


void CCamera::SetShadowState(bool bUse)
{
	m_bUseShadow = bUse;
}

bool CCamera::GetShadowState() const
{
	return m_bUseShadow;
}

void CCamera::ComputeViewMatrix()
{
	*m_matOldView = *m_matView;
	Matrix matProjection = *m_matPerspectiveProjection;
	Matrix matOldVP = *m_matOldView * matProjection;

	*m_matOldVP = matOldVP;

	*m_matView = XMMatrixIdentity();

	for (int i = 0; i < 3; ++i)
	{
		Vector3	vAxis = m_pTransform->GetWorldAxis((AXIS)i);

		memcpy(&m_matView->fMat[i][0], &vAxis, sizeof(Vector3));
	}

	m_matView->Transpose();

	Vector3	vPos = m_pTransform->GetWorldPosition();

	for (int i = 0; i < 3; ++i)
	{
		Vector3	vAxis = m_pTransform->GetWorldAxis((AXIS)i);
		m_matView->fMat[3][i] = -vPos.Dot(vAxis);
	}

	// 그림자 처리용 뷰행렬을 계산한다.
	Vector3	vDir = m_vLightLookAtforShadowPos - m_vLightforShadowPos;
	vDir = vDir.Normalize();

	Vector3	vUp = Vector3(0.f, 1.f, 0.f);

	Vector3	vRight = vUp.Cross(vDir);
	vRight = vRight.Normalize();

	vUp = vDir.Cross(vRight);
	vUp = vUp.Normalize();

	*m_matShadowView = Matrix::IdentityMatrix();

	memcpy(&m_matShadowView->fMat[0][0], &vRight, sizeof(Vector3));
	memcpy(&m_matShadowView->fMat[1][0], &vUp, sizeof(Vector3));
	memcpy(&m_matShadowView->fMat[2][0], &vDir, sizeof(Vector3));

	*m_matShadowView = Matrix::Transpose(*m_matShadowView);

	m_matShadowView->fMat[3][0] = -m_vLightforShadowPos.Dot(vRight);
	m_matShadowView->fMat[3][1] = -m_vLightforShadowPos.Dot(vUp);
	m_matShadowView->fMat[3][2] = -m_vLightforShadowPos.Dot(vDir);
}

Matrix CCamera::GetShadowViewMatrix() const
{
	return *m_matShadowView;
}

Matrix CCamera::GetShadowProjMatrix() const
{
	return *m_matShadowProj;
}

Vector3 CCamera::GetLightPos() const
{
	return m_vLightforShadowPos;
}

Vector3 CCamera::GetLightLookAtPos() const
{
	return m_vLightLookAtforShadowPos;
}

Vector2 CCamera::GetShadowResolution() const
{
	return Vector2((float)m_tShadowRS.iWidth, (float)m_tShadowRS.iHeight);
}

void CCamera::SetShadowResolution(RESOLUTION tRS, float fNear, float fFar)
{
	m_tShadowRS = tRS;

	*m_matShadowProj = XMMatrixPerspectiveFovLH(PI / 3.f, m_tShadowRS.iWidth / (float)m_tShadowRS.iHeight, fNear, fFar);
}

void CCamera::SetLightPos(Vector3 & vPos)
{
	m_vLightforShadowPos = vPos;
}

void CCamera::SetLightLookAtPos(Vector3 & vPos)
{
	m_vLightLookAtforShadowPos = vPos;
}

bool CCamera::Save(FILE * pFile)
{
	fwrite(m_matView, sizeof(Matrix), 1, pFile);
	fwrite(m_matPerspectiveProjection, sizeof(Matrix), 1, pFile);
	fwrite(m_matOrthoProjection, sizeof(Matrix), 1, pFile);
	return true;
}

bool CCamera::Load(FILE * pFile)
{
	//생성자에서 힙에 할당하기 떄문에 추가로 할당 하지 않음.
	fread(m_matView, sizeof(Matrix), 1, pFile);
	fread(m_matPerspectiveProjection, sizeof(Matrix), 1, pFile);
	fread(m_matOrthoProjection, sizeof(Matrix), 1, pFile);
	return true;
}

bool CCamera::Initialize()
{
	// 그림자 투영 행렬
	*m_matShadowProj = XMMatrixPerspectiveFovLH(PI / 3.f, m_tShadowRS.iWidth / (float)m_tShadowRS.iHeight, 0.03f, 10000.f);

	return true;
}

int CCamera::Update(float fTime)
{
	if (GETKEYDOWN(KEY_F6))
	{
		GET_SINGLE(CCore)->SetEngineState(!GET_SINGLE(CCore)->GetEngineState());
	}
	return 0;
}

int CCamera::LateUpdate(float fTime)
{
	return 0;
}
