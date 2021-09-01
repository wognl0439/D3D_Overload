#pragma once
#include "../Component.h"

SSS_BEGIN

//Collider를 상속받는 클래스는 자동으로 Scene의 Collider에 종속되기 때문에
//3D오브젝트와의 분리를 위해 Collider 클래스를 상속받지 않도록 한다.

class SSS_DLL CUICollider :
	public CComponent
{
public:
	CUICollider();
	CUICollider(const CUICollider& component);
	~CUICollider();

private:
	Vector2 m_vLocalScale;

	bool		m_bCollided;
	class CShader*			m_pShader;
	class CMesh*			m_pMesh;
	ID3D11InputLayout*	m_pLayout;
	Vector4 m_vColor;

public:
	void SetColor(const Vector4& vColor);
	Vector4 GetColor() const;

public:
	Vector2 GetLocalScale() const;
	bool GetCollision() const;

	void SetLocalScale(const Vector2& vScale);
	void SetLocalScale(float fWidth, float fHeight);
	void SetCollision(bool bCollision);

public:
	bool Initialize() override;
	void RenderDebug(float fTime) override;
	bool CheckCollision(const Vector2& vPosition);

	void OnMouseEnter(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;
	void OnMouseStay(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime) override;
	void OnMouseExit(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;
	void OnMouseDown(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;
	void OnMouseDrag(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime) override;
	void OnMouseUp(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;


	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END
