#pragma once
#include "../Component.h"

SSS_BEGIN

class SSS_DLL CButtonCollider :
	public CComponent
{
public:
	CButtonCollider();
	CButtonCollider(const CButtonCollider& component);
	~CButtonCollider();

private:
	Vector2 m_vPivot;
	Vector2 m_vScale;
	bool		m_bCollided;

#ifdef _DEBUG

	class CShader*			m_pShader;
	class CMesh*				m_pMesh;
	ID3D11InputLayout*	m_pLayout;
	Vector4 m_vColor;

public:
	void SetColor(const Vector4& vColor);
	Vector4 GetColor() const;



#endif // _DEBUG
public:
	Vector2 GetPivot() const;
	Vector2 GetScale() const;
	bool GetCollision() const;

	void SetPivot(const Vector2& vPos);
	void SetPivot(float x, float y);
	void SetScale(const Vector2& vScale);
	void SetScale(float fWidth, float fHeight);
	void SetCollision(bool bCollision);


public:
	bool Initialize() override;
	void Render(float fTime) override;
	bool CheckCollision(const Vector2& vPosition);

};

SSS_END
