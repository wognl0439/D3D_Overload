#pragma once
#include "UI.h"

SSS_BEGIN

class SSS_DLL CUISlider :
	public CUI
{
private:
	friend class CGameObject;

private:
	CUISlider();
	~CUISlider();

private:
	class CTransform* m_pTextureTransform;
	class CAlphaMask* m_pAlphaMask;

private:
	int m_iSliderCursor;
	int m_iMinIndex;
	int m_iMaxIndex;

	float m_fOffset;

public:
	void OffSet(float _iOffSet);
	float OffSet() const;

public:
	int MinIndex();
	void MinIndex(int _index);
	int MaxIndex();
	void MaxIndex(int _index);

public:
	void TexturePosition(Vector3 _Transform);
	void TexturePosition(Vector2 _Transform);
	Vector3 TexturePosition() const;
	void ObjectPosition(Vector3 _Transform);
	void ObjectPosition(Vector2 _Transform);
	Vector3 ObjectPosition() const;

public:
	bool Initialize() override;
	int Update(float fTime) override;
	int LateUpdate(float fTime) override;
	void Render(float fTime) override;
	CUISlider* Clone()	const override;

public:
	void OnMouseEnter(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition);
	void OnMouseStay(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime);
	void OnMouseExit(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition);

};

SSS_END