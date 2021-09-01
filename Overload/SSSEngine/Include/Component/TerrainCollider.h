#pragma once
#include "../Collider.h"

SSS_BEGIN

class SSS_DLL CTerrainCollider :
	public CCollider
{
public:
	CTerrainCollider();
	~CTerrainCollider();
	
private:
	class CTexture* m_pHeightTexture;
	float m_fMaxHeight;
	float m_fMinHeight;
	float m_fRow;
	float m_fColumn;
	Vector3 m_vLocalScale;

	void CreateActor(bool bStatic) override;
	void SetPhysicsTransform() override;
public:
	class CTexture* GetTexture() const;
	Vector2 GetHeightRange() const;
	Vector3 GetVolume() const;
	void SetHeight(float fMin, float fMax);
	void SetHeight(const Vector2& vMinMax);
	void SetVolume(const Vector2& vMinMax);
	void LoadTerrain(float fMaxHeight, const Vector3& vScale, const string& strKey, const string& strFileName, const string& strPathKey = NO_USE_PATH);
	void LoadTerrain(const string& strKey, const string& strFileName, const string& strPathKey = NO_USE_PATH);

public:
	void Start() override;
	bool Initialize() override;
	int LateUpdate(float fTime) override;

	void RenderDebug(float fDeltaTime) override;

	int OnCollisionEnter(class CCollider* pThis, class CCollider* pTarget, float fTime)override;
	int OnCollisionStay(class CCollider* pThis, class CCollider* pTarget, float fTime)override;
	int OnCollisionExit(class CCollider* pThis, class CCollider* pTarget, float fTime)override;

	void OnMouseEnter(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;
	void OnMouseExit(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END

