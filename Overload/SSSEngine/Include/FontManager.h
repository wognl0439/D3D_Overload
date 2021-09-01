#pragma once
#include "Engine.h"

SSS_BEGIN

class SSS_DLL CFontManager
{
	DECLARE_SINGLE(CFontManager)


private:
	IDWriteFactory*		m_pWriteFactory;
	unordered_map<wstring, IDWriteTextFormat*>	m_mapFont;
	unordered_map<unsigned int, ID2D1SolidColorBrush*>	m_mapBrush;

public:
	bool Initialize();

	IDWriteTextFormat* MakeFont(const wstring & strName,int iWeight, int iStyle, int iStretch, float fSize, const wchar_t * pLocalName);
	IDWriteTextFormat* FindFont(const wstring& strKey);
	ID2D1SolidColorBrush* CreateBrush(float r, float g, float b, float a);
	ID2D1SolidColorBrush* CreateBrush(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	ID2D1SolidColorBrush* CreateBrush(const Vector4& vColor);
	ID2D1SolidColorBrush* FindBrush(float r, float g, float b, float a);
	ID2D1SolidColorBrush* FindBrush(unsigned char r, unsigned char g,	unsigned char b, unsigned char a);
	ID2D1SolidColorBrush* FindBrush(const Vector4& vColor);
	unsigned int CreateColorKey(float r, float g, float b, float a);
	unsigned int CreateColorKey(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
};

SSS_END
