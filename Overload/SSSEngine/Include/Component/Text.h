#pragma once
#include "../Component.h"

SSS_BEGIN

enum HORIZONTAL_ALIGN
{
	HA_LEFT,
	HA_CENTER,
	HA_RIGHT
};

enum VERTICAL_ALIGN
{
	VA_TOP,
	VA_CENTER,
	VA_BOTTOM
};

class SSS_DLL CText :
	public CComponent
{
private:
	friend class CGameObject;
private:
	CText();
	CText(const CText& text);
	~CText();

private:
	IDWriteTextFormat*	m_pTextFormat;
	ID2D1SolidColorBrush*	m_pBrush;
	ID2D1SolidColorBrush*	m_pShadowBrush;
	wstring	m_strText;
	wstring	m_strFont;
	float	m_fSize;
	HORIZONTAL_ALIGN	m_eHorizontalAlign;
	VERTICAL_ALIGN	m_eVerticalAlign;
	bool			m_bShadow;
	bool			m_bAlpha;
	bool			m_bShadowAlpha;
	float			m_fOpacity;
	float			m_fShadowOpacity;
	Vector4			m_vColor;
	Vector4			m_vShadowColor;
	Vector3			m_vShadowOffset;
	D2D1_RECT_F		m_tRenderArea;
	bool m_bRelativeCamera;

public:
	float GetOpacity() const;
	void SetRelativeCamera(bool bRelativeCamera);
	bool GetRelativeCamera() const;
	void SetText(const wstring& strText);
	void SetFont(const wstring& strFont, float fSize);
	void SetHorizontalAlign(HORIZONTAL_ALIGN eAlign);
	void SetVerticalAlign(VERTICAL_ALIGN eAlign);
	void AlphaBlend(bool bAlpha);
	void SetOpacity(float fOpacity);
	void SetColor(float r, float g, float b, float a);
	void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void SetColor(const Vector4& vColor);
	void SetRenderArea(float l, float t, float r, float b);

public:
	bool Initialize() override;
	void Render(float fTime) override;
	CText* Clone()	const;
	
	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};



SSS_END
