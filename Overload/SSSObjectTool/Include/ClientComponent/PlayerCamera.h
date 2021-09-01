#pragma once
#include "Component.h"

SSS_USING

//"Player" 라는 이름의 오브젝트는 유일해야 한다.
//이 컴포넌트가 포함된 오브젝트 :: A
//이 컴포넌트가 포함된 오브젝트의 자식 오브젝트 :: B
//카메라 오브젝트 :: C

//A와 B는 항상 플레이어의 위치와 동일해야한다.
//A는 기저 Y축으로만 회전할 수 있고, 상·하한을 가지지 않는다.
//B는 로컬 X축으로만 회전할 수 있고, 상·하한을 가진다.
//C는 로컬 Z축으로만 이동할 수 있고, 그 외의 이동 및 회전은 불가능하다.

class CPlayerCamera :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CPlayerCamera();
	~CPlayerCamera();

private:
	float m_fHeightOffset;
	Vector3 m_vOffset;
	float m_fAngularSpeed; // Degree
	float m_fScrollDistance; // 스크롤 시 이동할 거리
	float m_fScrollSpeed; // 스크롤 시 부드러운 이동의 초당 이동속도
	float m_fScrollTargetDistance;
	class CGameObject* m_pPlayer;
	Vector2	m_vDistanceRange;
	Vector2 m_vVerticalAngleRange;
	float m_fHorizontalAngleTarget;
	float m_fVerticalAngleTarget;

private:
	bool m_bShake;
	float m_fShakeAmplitude;
	float m_fShakeFrequency;
	float m_fShakeTime;
	float m_fShakeTimeLapse;
	
	Vector3 UpdateShake(float fTime);

public:
	void StopShake();
	void StartShake(float fAmplitude, float fFrequency, float fTime);
	void SetDistanceRange(float fMin, float fMax);
	void SetDistanceRange(const Vector2& vMinMax);
	Vector2 GetDistanceRange() const;
	void SetAngleRange(float fMin, float fMax);
	void SetAngleRange(const Vector2& vMinMax);
	Vector2 GetAngleRange() const;

	void SetPlayer(class CGameObject* pTarget);
	float GetScrollTargetDistance() const;
	void SetScrollTargetDistance(float fDistance);

	float GetCameraPostionZ() const;
	void SetCameraPostionZ(float fZ);

public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;
	int LateUpdate(float fTime) override;

	bool Save(FILE* pFile);
	bool Load(FILE* pFile);
};

