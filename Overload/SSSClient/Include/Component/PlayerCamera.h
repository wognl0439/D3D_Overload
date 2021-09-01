#pragma once
#include "Component.h"

SSS_USING

//"Player" ��� �̸��� ������Ʈ�� �����ؾ� �Ѵ�.
//�� ������Ʈ�� ���Ե� ������Ʈ :: A
//�� ������Ʈ�� ���Ե� ������Ʈ�� �ڽ� ������Ʈ :: B
//ī�޶� ������Ʈ :: C

//A�� B�� �׻� �÷��̾��� ��ġ�� �����ؾ��Ѵ�.
//A�� ���� Y�����θ� ȸ���� �� �ְ�, �������� ������ �ʴ´�.
//B�� ���� X�����θ� ȸ���� �� �ְ�, �������� ������.
//C�� ���� Z�����θ� �̵��� �� �ְ�, �� ���� �̵� �� ȸ���� �Ұ����ϴ�.

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
	float m_fScrollDistance; // ��ũ�� �� �̵��� �Ÿ�
	float m_fScrollSpeed; // ��ũ�� �� �ε巯�� �̵��� �ʴ� �̵��ӵ�
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

