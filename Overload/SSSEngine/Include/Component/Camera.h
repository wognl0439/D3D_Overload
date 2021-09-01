#pragma once
#include "../Component.h"
#include "../CollisionManager.h"

SSS_BEGIN

class SSS_DLL CCamera :
	public CComponent
{
private:
	friend class CGameObject;
	friend class CCollisionManager;

private:
	CCamera();
	CCamera(const CCamera& component);
	~CCamera();

private:
	PMatrix		m_matView;
	PMatrix		m_matPerspectiveProjection;
	PMatrix		m_matOrthoProjection;
	FRUSTUM	m_tFrustum;

public:
	void CreateFrustum(const Matrix& matInvVP);
	bool PointOutsideFrustum(const Vector3& vPos);
	bool SphereOutsideFrustum(const Vector3& vCenter, float fRadius);

	Matrix GetPerspectiveProjectionMatrix() const;
	Matrix GetOrthographyProjectionMatrix() const;
	Matrix GetViewMatrix() const;
	Matrix GetProjectionMatrix() const;
	void SetPerspectiveProjection(float fAspect, float fViewAngle, float fNear = 0.3f, float fFar = 5000.f);
	void SetOrthographyProjection(float fLeft, float fRight, float fTop, float fBottom,	float fNear = 0.f, float fFar = 5000.f);
	RAY ScreenPointToRay(const Vector2& vScreenPoint);

	//월드 공간상에서의 위치를 화면 공간 상의 위치로 변환한다.
	//vPosition이 Frustum 밖에 있다면 INF를 리턴한다.
	Vector2 WorldToScreenPoint(Vector3 vPosition);

#pragma region MotionBlur
private:
	PMatrix		m_matOldView;
	PMatrix		m_matOldVP;

public:
	Matrix GetOldView() const;
	Matrix GetOldVP() const;

#pragma endregion

#pragma region Shadow
private:
	bool			m_bUseShadow;
	PMatrix		m_matShadowView;
	PMatrix		m_matShadowProj;
	Vector3		m_vLightforShadowPos;
	Vector3		m_vLightLookAtforShadowPos;
	RESOLUTION	m_tShadowRS;

public:
	void SetShadowState(bool bUse);
	bool GetShadowState() const;
	
	void ComputeViewMatrix();
	Matrix GetShadowViewMatrix() const;
	Matrix GetShadowProjMatrix() const;
	Vector3 GetLightPos() const;
	Vector3 GetLightLookAtPos() const;
	Vector2 GetShadowResolution() const;
	void SetShadowResolution(RESOLUTION tRS, float fNear = 0.03f, float fFar = 1000.f);
	void SetLightPos(Vector3& vPos);
	void SetLightLookAtPos(Vector3& vPos);

#pragma endregion

public:
	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;

	bool Initialize()				override;
	int Update(float fTime)			override;
	int LateUpdate(float fTime)		override;
	
};

SSS_END	
