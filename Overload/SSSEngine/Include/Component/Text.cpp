#include "Text.h"
#include "../FontManager.h"
#include "../GameObject.h"
#include "../Transform.h"
#include "../Device.h"
#include "../Scene.h"

SSS_USING

CText::CText() :
	m_pTextFormat(NULL),
	m_pBrush(NULL),
	m_pShadowBrush(NULL),
	m_fSize(0.f),
	m_fOpacity(1.f),
	m_fShadowOpacity(1.f),
	m_bRelativeCamera(true)
{
	SetTag("Text");

	m_eHorizontalAlign = HA_LEFT;
	m_eVerticalAlign = VA_CENTER;
	m_bShadow = false;
	m_bAlpha = false;
	m_bShadowAlpha = false;
	SetRenderArea(0.f, 0.f, 100.f, 50.f);
	m_vShadowOffset = Vector3(1.f, 1.f, 0.f);

	m_vColor = Vector4::white;
	m_strFont = L"나눔고딕 ExtraBold30";
	m_fSize = 30.f;
	
	m_pTextFormat = GET_SINGLE(CFontManager)->FindFont(L"나눔고딕 ExtraBold30");
	m_pBrush = GET_SINGLE(CFontManager)->CreateBrush(m_vColor);
	m_pShadowBrush = GET_SINGLE(CFontManager)->CreateBrush(Vector4::black);
}


CText::CText(const CText & text)
{
	m_strText = text.m_strText;
	m_strFont = text.m_strFont;
	m_eHorizontalAlign = text.m_eHorizontalAlign;
	m_eVerticalAlign = text.m_eVerticalAlign;
	m_bShadow = text.m_bShadow;
	m_fOpacity = text.m_fOpacity;
	m_fShadowOpacity = text.m_fShadowOpacity;
	m_bAlpha = text.m_bAlpha;
	m_bShadowAlpha = text.m_bShadowAlpha;
	m_fSize = text.m_fSize;
	m_vColor = text.m_vColor;
	m_tRenderArea = text.m_tRenderArea;
	m_pTextFormat = text.m_pTextFormat;
	m_pBrush = text.m_pBrush;
	m_pShadowBrush = text.m_pShadowBrush;
	m_vShadowColor = text.m_vShadowColor;
	m_vShadowOffset = text.m_vShadowOffset;
}

CText::~CText()
{
}

float CText::GetOpacity() const
{
	return m_fOpacity;
}

void CText::SetRelativeCamera(bool bRelativeCamera)
{
	m_bRelativeCamera = bRelativeCamera;
}

bool CText::GetRelativeCamera() const
{
	return m_bRelativeCamera;
}

void CText::SetText(const wstring & strText)
{
	m_strText = strText;
}

void CText::SetFont(const wstring & strFont, float fSize)
{
	m_strFont = strFont;
	m_fSize = fSize;
	m_vColor = Vector4::black;

	m_pTextFormat = GET_SINGLE(CFontManager)->MakeFont(strFont,
		fSize, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STRETCH_ULTRA_EXPANDED,
		fSize, L"ko");

	m_pBrush = GET_SINGLE(CFontManager)->CreateBrush(m_vColor.x,
		m_vColor.y, m_vColor.z, m_vColor.w);
}

void CText::SetHorizontalAlign(HORIZONTAL_ALIGN eAlign)
{
	m_eHorizontalAlign = eAlign;

	if (m_pTextFormat)
	{
		switch (eAlign)
		{
		case HA_LEFT:
			m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			break;
		case HA_CENTER:
			m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			break;
		case HA_RIGHT:
			m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
			break;
		}
	}
}

void CText::SetVerticalAlign(VERTICAL_ALIGN eAlign)
{
	m_eVerticalAlign = eAlign;

	switch (eAlign)
	{
	case VA_TOP:
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	case VA_CENTER:
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	case VA_BOTTOM:
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		break;
	}
}

void CText::AlphaBlend(bool bAlpha)
{
	m_bAlpha = bAlpha;
}

void CText::SetOpacity(float fOpacity)
{
	m_fOpacity = fOpacity;
}

void CText::SetColor(float r, float g, float b, float a)
{
	m_vColor = Vector4(r, g, b, a);

	m_pBrush = GET_SINGLE(CFontManager)->CreateBrush(r, g, b, a);
}

void CText::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	m_vColor = Vector4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);

	m_pBrush = GET_SINGLE(CFontManager)->CreateBrush(r, g, b, a);
}

void CText::SetColor(const Vector4 & vColor)
{
	m_vColor = vColor;

	m_pBrush = GET_SINGLE(CFontManager)->CreateBrush(m_vColor.x,
		m_vColor.y, m_vColor.z, m_vColor.w);
}

void CText::SetRenderArea(float l, float t, float r, float b)
{
	m_tRenderArea = D2D1::RectF(l, t, r, b);
}

bool CText::Initialize()
{
	m_pTextFormat = GET_SINGLE(CFontManager)->FindFont(L"나눔고딕OTF ExtraBold30");
	return true;
}

void CText::Render(float fTime)
{
	if (m_pTextFormat)
	{
		switch (m_eHorizontalAlign)
		{
		case HA_LEFT:
			m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			break;
		case HA_CENTER:
			m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			break;
		case HA_RIGHT:
			m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
			break;
		}
	}

	switch (m_eVerticalAlign)
	{
	case VA_TOP:
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	case VA_CENTER:
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	case VA_BOTTOM:
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		break;
	}

	Vector3		vPos = m_pTransform->GetWorldPosition();

	GET_SINGLE(CDevice)->Get2DRenderTarget()->BeginDraw();

	if (!m_bRelativeCamera)
	{
		CTransform*	pCameraTr = m_pScene->GetMainCameraTransform();

		Vector3	vCameraPos = pCameraTr->GetWorldPosition();

		vPos -= vCameraPos;

		SAFE_RELEASE(pCameraTr);
	}

	if (m_bShadow)
	{
		Vector3	vShadowPos = vPos + m_vShadowOffset;

		if (m_bShadowAlpha)
			m_pShadowBrush->SetOpacity(m_fShadowOpacity);

		else
			m_pShadowBrush->SetOpacity(1.f);

		ID2D1RenderTarget* pTarget = GET_SINGLE(CDevice)->Get2DRenderTarget();

		GET_SINGLE(CDevice)->Get2DRenderTarget()->DrawText(m_strText.c_str(), m_strText.length(), m_pTextFormat,
			D2D1::RectF(m_tRenderArea.left + vShadowPos.x, m_tRenderArea.top + vShadowPos.y,
				m_tRenderArea.right + vShadowPos.x, m_tRenderArea.bottom + vShadowPos.y), m_pShadowBrush);
	}

	if (m_bAlpha)
		m_pBrush->SetOpacity(m_fOpacity);

	else
		m_pBrush->SetOpacity(1.f);

	GET_SINGLE(CDevice)->Get2DRenderTarget()->DrawText(m_strText.c_str(), m_strText.length(), m_pTextFormat,
		D2D1::RectF(m_tRenderArea.left + vPos.x, m_tRenderArea.top + vPos.y,
			m_tRenderArea.right + vPos.x, m_tRenderArea.bottom + vPos.y), m_pBrush);

	GET_SINGLE(CDevice)->Get2DRenderTarget()->EndDraw();
}


CText * CText::Clone() const
{
	return new CText(*this);
}

bool CText::Save(FILE * pFile)
{
	// m_strText 저장
	size_t iTextLength = m_strText.length();
	fwrite(&iTextLength, sizeof(size_t), 1, pFile);
	fwrite(m_strText.c_str(), sizeof(wchar_t), iTextLength, pFile);
	// m_strFont 저장
	size_t iFontLength = m_strText.length();
	fwrite(&iFontLength, sizeof(size_t), 1, pFile);
	fwrite(m_strFont.c_str(), sizeof(wchar_t), iFontLength, pFile);

	fwrite(&m_fSize, sizeof(float), 1, pFile);
	fwrite(&m_eHorizontalAlign, sizeof(HORIZONTAL_ALIGN), 1, pFile);
	fwrite(&m_eVerticalAlign, sizeof(VERTICAL_ALIGN), 1, pFile);
	fwrite(&m_bShadow, sizeof(bool), 1, pFile);
	fwrite(&m_bAlpha, sizeof(bool), 1, pFile);
	fwrite(&m_bShadowAlpha, sizeof(bool), 1, pFile);
	fwrite(&m_fOpacity, sizeof(float), 1, pFile);
	fwrite(&m_fShadowOpacity, sizeof(float), 1, pFile);
	fwrite(&m_vColor, sizeof(Vector4), 1, pFile);
	fwrite(&m_vShadowColor, sizeof(Vector4), 1, pFile);
	fwrite(&m_vShadowOffset, sizeof(Vector3), 1, pFile);
	fwrite(&m_tRenderArea, sizeof(D2D1_RECT_F), 1, pFile);
	fwrite(&m_bRelativeCamera, sizeof(bool), 1, pFile);

	return true;
}

bool CText::Load(FILE * pFile)
{
	// m_strText 로드
	size_t iTextLength = 0;
	fread(&iTextLength, sizeof(size_t), 1, pFile);
	wchar_t	strTextBuffer[256] = {};
	fread(strTextBuffer, sizeof(wchar_t), iTextLength, pFile);

	SetText(strTextBuffer);

	// m_strFont 로드
	size_t iFontLength = 0;
	fread(&iFontLength, sizeof(size_t), 1, pFile);
	wchar_t	strFontBuffer[256] = {};
	fread(strFontBuffer, sizeof(wchar_t), iFontLength, pFile);

	float fTextSize = 0.f;
	fread(&fTextSize, sizeof(float), 1, pFile);

	// m_strFont 와 m_iSize 를 읽어들인후 셋팅..
	SetFont(strFontBuffer, fTextSize);

	fread(&m_eHorizontalAlign, sizeof(HORIZONTAL_ALIGN), 1, pFile);
	fread(&m_eVerticalAlign, sizeof(VERTICAL_ALIGN), 1, pFile);
	fread(&m_bShadow, sizeof(bool), 1, pFile);
	fread(&m_bAlpha, sizeof(bool), 1, pFile);
	fread(&m_bShadowAlpha, sizeof(bool), 1, pFile);
	fread(&m_fOpacity, sizeof(float), 1, pFile);
	fread(&m_fShadowOpacity, sizeof(float), 1, pFile);

	// Text Color 로드
	Vector4 vColor;
	fread(&vColor, sizeof(Vector4), 1, pFile);
	SetColor(vColor);

	fread(&m_vShadowColor, sizeof(Vector4), 1, pFile);
	fread(&m_vShadowOffset, sizeof(Vector3), 1, pFile);
	fread(&m_tRenderArea, sizeof(D2D1_RECT_F), 1, pFile);
	fread(&m_bRelativeCamera, sizeof(bool), 1, pFile);

	return true;
}
