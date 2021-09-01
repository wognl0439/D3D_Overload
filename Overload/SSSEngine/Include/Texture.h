#pragma once
#include "Ref.h"
#include "DirectXTex.h"

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex64_Debug")
#else
#pragma comment(lib, "DirectXTex64")
#endif // _DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_Debug")
#else
#pragma comment(lib, "DirectXTex")
#endif // _DEBUG
#endif //

SSS_BEGIN

enum TEXTURE_FORMAT
{
	TF_BMP24,
	TF_BMP32,
	TF_PNG,
	TF_DDS,
	TF_TGA,
};

class SSS_DLL CTexture :
	public CRef
{
private:
	friend class CResourcesManager;
	friend class CMaterial;

private:
	CTexture();
	~CTexture();

private:
	vector<ScratchImage*>		m_vecScratchImage;
	ID3D11ShaderResourceView*	m_pRSView;
	string		m_strKey;
	wstring		m_strRelativePath;
	vector<wstring>	m_vecFileName;
	string		m_strPathKey;

public:
	string GetKey() const;
	string GetPathKey() const;
	wstring GetRelativePath() const;
	vector<wstring> GetvecFileName() const;
	void SetKey(const string& strKey);
	void SetPathKey(const string& strPathKey);
	void SetRelativePath(const wstring& strPath);
	void SetvecFileName(const vector<wstring>& vecFileName);

	ScratchImage* GetImage(size_t idx) const;
	Vector2 GetTextureSize(size_t idx);
	bool LoadTexture(const string& strKey, const wchar_t* pFileName, const string& strPathKey);
	bool LoadTexture(const string& strKey, const vector<wstring>& vecFileName, const string& strPathKey);
	bool LoadTextureFromFullPath(const string& strKey, const wchar_t* pFullPath);
	bool LoadTextureFromFullPath(const string& strKey, const vector<wstring>& vecFileName);
	bool CreateShaderResourceView();
	bool CreateShaderResourceArrayView();
	void SetTexture(int iRegister);
	void SavePNG(const wchar_t* pFileName, const string& strPathKey, size_t iMipIndex = 0);
	void SavePNG(Image img, const wchar_t* pFileName, const string& strPathKey, size_t iMipIndex = 0);
	void SaveBMP(Image img, const wchar_t * pFileName, const string & strPathKey, size_t iMipIndex = 0);
	void LoadTexture(Image * img, int Count);

};

SSS_END

