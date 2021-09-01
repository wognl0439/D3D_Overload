#pragma once
#include "../Component.h"

SSS_BEGIN

enum GIZMO_STATE
{
	GS_DISABLE,
	GS_TRANSFORMATION,
	GS_ROTATION,
	GS_SCALE,
	GS_END
};

enum GIZMO_CONTROL_AXIS
{
	GCA_X = 0x01,
	GCA_Y = 0x02,
	GCA_Z = 0x04
};

class SSS_DLL CGizmo :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CGizmo();
	~CGizmo();


private:
	bool m_bLocalMode;

	float m_fSensitivity;
	GIZMO_STATE m_eState;
	uint32_t m_iControlAxis;
	bool m_bClicked;

	Vector4 m_vGizmoColor[3];

	class CMeshRenderer* m_pRendererX;
	class CMeshRenderer* m_pRendererY;
	class CMeshRenderer* m_pRendererZ;
	class CMeshRenderer* m_pRendererXY;
	class CMeshRenderer* m_pRendererXZ;
	class CMeshRenderer* m_pRendererYZ;
	class CBoxCollider* m_pColliderX;
	class CBoxCollider* m_pColliderY;
	class CBoxCollider* m_pColliderZ;
	class CBoxCollider* m_pColliderXY;
	class CBoxCollider* m_pColliderXZ;
	class CBoxCollider* m_pColliderYZ;

	class CGameObject* m_pTarget;

	float m_fLocalScaleFactor;

private:
	void UpdateGizmo();

public:
	void SetLocalMode(bool bForLocal);
	void UpdateGizmoTransform();
	void UpdateGizmoLocalTransform();
	class CGameObject* GetGizmoTarget() const;
	void SetGizmoState(GIZMO_STATE eState);
	void SetGizmoTarget(class CGameObject* pTarget);

public:
	bool Initialize() override;
	int Update(float fTime) override;

	void OnMouseEnter(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;
	void OnMouseStay(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime) override;
	void OnMouseExit(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;
	void OnMouseDown(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;
	void OnMouseDrag(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime) override;
	void OnMouseUp(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;

};

SSS_END