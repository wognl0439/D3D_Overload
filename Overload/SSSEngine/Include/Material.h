#pragma once
#include "Component.h"
#include "Ref.h"

SSS_BEGIN

typedef	struct SSS_DLL _tagShaderResource
{
	class CTexture*	pDiffuseTexture;
	class CTexture*	pNormalTexture;
	class CTexture*	pSpecularTexture;
	class CTexture*	pAlphaTexture;
	class CSampler*	pSampler;
	int		iDiffuseTextureRegister;
	int		iNormalTextureRegister;
	int		iSpecularTextureRegister;
	int		iAlphaTextureRegister;
	int		iSamplerRegister;

	_tagShaderResource() :
		pDiffuseTexture(NULL),
		pNormalTexture(NULL),
		pSpecularTexture(NULL),
		pAlphaTexture(NULL),
		pSampler(NULL),
		iDiffuseTextureRegister(0),
		iNormalTextureRegister(1),
		iSpecularTextureRegister(2),
		iAlphaTextureRegister(3),
		iSamplerRegister(0)
	{
	}
}SHADERRESOURCE, *PSHADERRESOURCE;


typedef struct SSS_DLL _tagMaterialSubset
{
	MATERIAL		tMaterial;
	PSHADERRESOURCE	pShaderResource;
	unordered_map<string, SHADERRESOURCE>	mapMultiTexture;

	_tagMaterialSubset() :
		pShaderResource(NULL)
	{
		tMaterial.vDiffuse = Vector4(1.f, 1.f, 1.f, 1.f);
		tMaterial.vAmbient = Vector4(1.f, 1.f, 1.f, 0.f);
		tMaterial.vSpecular = Vector4(1.f, 1.f, 1.f, 3.2f);
		tMaterial.vEmissive = Vector4(1.f, 1.f, 1.f, 0.f);
		tMaterial.vAlpha = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		tMaterial.fNormalScale = 1.f;
	}
}MATERIALSUBSET, *PMATERIALSUBSET;

typedef struct SSS_DLL _tagMaterialContainer
{
	vector<MATERIALSUBSET>	vecSubset;
}MATERIALCONTAINER, *PMATERIALCONTAINER;


class SSS_DLL CMaterial :
	public CRef
{
private:
	friend class CSpriteRenderer;
	friend class CMeshRenderer;
	friend class CResourcesManager;
	friend class CMesh;

public:
	CMaterial();
	CMaterial(const CMaterial& mtrl);
	~CMaterial();

private:
	vector<MATERIALCONTAINER>	m_vecContainer;

private:
	bool ChangeMaterialContainer(MATERIALCONTAINER* pSourceContainer, int iChangeContainerIndex);
	bool InsertMaterialContainer(MATERIALCONTAINER* pSourceContainer, int iInsertContainerIndex);
	bool RemoveMaterialContainer(int iContainerIndex);

public:
	MATERIALCONTAINER* GetMaterialContainer(int iContainer = 0);
	class CTexture* GetDiffuseTexture(int iContainer = 0, int iSubset = 0)	const;
	class CTexture* GetNormalTexture(int iContainer = 0, int iSubset = 0)	const;
	class CTexture* GetSpecularTexture(int iContainer = 0, int iSubset = 0)	const;
	class CTexture* GetAlphaTexture(int iContainer = 0, int iSubset = 0) const;

	class CTexture * GetAlphaMultiTexture(const string & strMultiTexKey, int iContainer = 0, int iSubset = 0);

	const Vector4& GetDiffuseColor(int iContainer = 0, int iSubset = 0)	const;
	const Vector4& GetAmbientColor(int iContainer = 0, int iSubset = 0)	const;
	const Vector4& GetSpecularColor(int iContainer = 0, int iSubset = 0)	const;
	const Vector4& GetEmissiveColor(int iContainer = 0, int iSubset = 0)	const;
	const Vector4& GetAlphaColor(int iContainer = 0, int iSubset = 0) const;

	int GetDiffuseTextureRegister(int iContainer = 0, int iSubset = 0) const;
	int GetNormalTextureRegister(int iContainer = 0, int iSubset = 0) const;
	int GetSpecularTextureRegister(int iContainer = 0, int iSubset = 0) const;
	int GetAlphaTextureRegister(int iContainer = 0, int iSubset = 0) const;

	void SetDiffuseColor(const Vector4& vColor, int iContainer = 0, int iSubset = 0);
	void SetAmbientColor(const Vector4& vColor, int iContainer = 0, int iSubset = 0);
	void SetSpecularColor(const Vector4& vColor, int iContainer = 0, int iSubset = 0);
	void SetEmissiveColor(const Vector4& vColor, int iContainer = 0, int iSubset = 0);
	void SetAlphaColor(const Vector4& vColor, int iContainer = 0, int iSubset = 0);

	size_t GetContainerCount()	const;
	size_t GetSubsetCount(int iContainer = 0);

	size_t GetMultiTextureCount(int iContainer = 0, int iSubset = 0);
	const unordered_map<string, SHADERRESOURCE>* GetMultiTexture(int iContainer = 0, int iSubset = 0) const;
	class CSampler* GetSampler(int iContainer = 0, int iSubset = 0);
	int GetSamplerRegister(int iContainer = 0, int iSubset = 0);

public:
	void ClearContainer();
	void AddContainer();
	void AddSubset(int iContainer = 0);
	void ResizeContainer(size_t iSize);
	void ResizeSubset(size_t iSize , int iContainer = 0);

	void SetMaterial(const Vector4& vDiffuse = Vector4::white,
		const Vector4& vAmbient = Vector4::white,
		const Vector4& vSpecular = Vector4::white,
		float fSpecularPower = 3.2f,
		const Vector4& vEmissive = Vector4::black,
		int iContainer = 0, int iSubset = 0);

public:
	/*Diffuse*/
	void SetDiffuseTexture(const string& strKey, int iContainer = 0, int iSubset = 0);
	void SetDiffuseTexture(class CTexture* pTexture, int iContainer = 0, int iSubset = 0);
	void SetDiffuseTexture(const string& strKey, const wchar_t* pFileName,	int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetDiffuseTextureFromFullPath(const string& strKey, const wchar_t* pFullPath, 	int iContainer = 0, int iSubset = 0);
	void SetDiffuseTexture(const string& strKey, const vector<wstring>& vecFileName,	int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetDiffuseTextureFromFullPath(const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0);
	void SetDiffuseTextureRegister(int iRegister, int iContainer = 0, int iSubset = 0);


public:
	/*Normal*/
	void SetNormalTexture(const string& strKey, int iContainer = 0, int iSubset = 0);
	void SetNormalTexture(class CTexture* pTexture, int iContainer = 0, int iSubset = 0);
	void SetNormalTexture(const string& strKey, const wchar_t* pFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetNormalTextureFromFullPath(const string& strKey, const wchar_t* pFullPath, int iContainer = 0, int iSubset = 0);
	void SetNormalTexture(const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetNormalTextureFromFullPath(const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0);
	void SetNormalTextureRegister(int iRegister, int iContainer = 0, int iSubset = 0);


public:
	/*Specular*/
	void SetSpecularTexture(const string& strKey, int iContainer = 0, int iSubset = 0);
	void SetSpecularTexture(class CTexture* pTexture, int iContainer = 0, int iSubset = 0);
	void SetSpecularTexture(const string& strKey, const wchar_t* pFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetSpecularTextureFromFullPath(const string& strKey, const wchar_t* pFullPath, int iContainer = 0, int iSubset = 0);
	void SetSpecularTexture(const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetSpecularTextureFromFullPath(const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0);
	void SetSpecularTextureRegister(int iRegister, int iContainer = 0, int iSubset = 0);

public:
	/*Alpha Mask*/
	void SetAlphaTexture(const string& strKey, int iContainer = 0, int iSubset = 0);
	void SetAlphaTexture(class CTexture* pTexture, int iContainer = 0, int iSubset = 0);
	void SetAlphaTexture(const string& strKey, const wchar_t* pFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetAlphaTextureFromFullPath(const string& strKey, const wchar_t* pFullPath, int iContainer = 0, int iSubset = 0);
	void SetAlphaTexture(const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetAlphaTextureFromFullPath(const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0);
	void SetAlphaTextureRegister(int iRegister, int iContainer = 0, int iSubset = 0);

public:
	/*Multi Texture*/
	void SetDiffuseMultiTexture(const string& strMultiTexKey, const string& strKey, const wchar_t* pFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetDiffuseMultiTextureFromFullPath(const string& strMultiTexKey, const string& strKey, const wchar_t* pFullPath, int iContainer = 0, int iSubset = 0);
	void SetDiffuseMultiTexture(const string& strMultiTexKey, const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetDiffuseMultiTextureFromFullPath(const string& strMultiTexKey, const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0);
	void SetDiffuseMultiTextureRegister(const string& strMultiTexKey, int iRegister, int iContainer = 0, int iSubset = 0);

	void SetNormalMultiTexture(const string& strMultiTexKey, const string& strKey, const wchar_t* pFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetNormalMultiTextureFromFullPath(const string& strMultiTexKey, const string& strKey, const wchar_t* pFullPath, int iContainer = 0, int iSubset = 0);
	void SetNormalMultiTexture(const string& strMultiTexKey, const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetNormalMultiTextureFromFullPath(const string& strMultiTexKey, const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0);
	void SetNormalMultiTextureRegister(const string& strMultiTexKey, int iRegister, int iContainer = 0, int iSubset = 0);

	void SetSpecularMultiTexture(const string& strMultiTexKey, const string& strKey, const wchar_t* pFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetSpecularMultiTextureFromFullPath(const string& strMultiTexKey, const string& strKey, const wchar_t* pFullPath, int iContainer = 0, int iSubset = 0);
	void SetSpecularMultiTexture(const string& strMultiTexKey, const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetSpecularMultiTextureFromFullPath(const string& strMultiTexKey, const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0);
	void SetSpecularMultiTextureRegister(const string& strMultiTexKey, int iRegister, int iContainer = 0, int iSubset = 0);

	void SetAlphaMultiTexture(const string& strMultiTexKey, const string& strKey, const wchar_t* pFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetAlphaMultiTextureFromFullPath(const string& strMultiTexKey, const string& strKey, const wchar_t* pFullPath, int iContainer = 0, int iSubset = 0);
	void SetAlphaMultiTexture(const string& strMultiTexKey, const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void SetAlphaMultiTextureFromFullPath(const string& strMultiTexKey, const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0);
	void SetAlphaMultiTextureRegister(const string& strMultiTexKey, int iRegister, int iContainer = 0, int iSubset = 0);


	void SetMultiTextureSampler(const string& strMultiTexKey, const string& strSamplerKey, int iContainer = 0, int iSubset = 0);
	void SetMultiTextureSamplerRegister(const string& strMultiTexKey, int iSamplerRegister, int iContainer = 0, int iSubset = 0);
	void SetSamplerMultiTextureRegister(const string & strMultiTexKey, int iSamplerRegister, const string & strSamplerKey, int iContainer = 0, int iSubset = 0);

	void ChangeAlphaTexture(int Count, const string & strMultiTexKey, const wchar_t * pFileName, int iContainer = 0, int iSubset = 0, const string & strPathKey = TEXTURE_PATH);

	/*Normal Scale*/
	void SetNormalScale(float fScale, int iContainer = 0, int iSubset = 0);
	float GetNormalScale(int iContainer = 0, int iSubset = 0)	const;

public:
	/*Sampler*/
	void SetSampler(const string& strKey, int iContainer = 0, int iSubset = 0);
	void SetSampler(class CSampler* pSampler, int iContainer = 0, int iSubset = 0);
	void SetSamplerRegister(int iRegister, int iContainer = 0, int iSubset = 0);

public:
	bool Save(FILE* pFile);
	bool Load(FILE* pFile);

private:
	bool TextureSave(class CTexture* pTexture, FILE* pFile);
	bool TextureMultiSave(class CTexture* pTexture, FILE* pFile);


	class CTexture* TextureLoad(FILE* pFile);
	class CTexture* TextureMultiLoad(FILE* pFile);

public:
	bool Initialize();
	CMaterial* Clone()	const;

public:
	void SetMaterial(int iContainer = 0, int iSubset = 0);
	void ResetMaterial(int iContainer, int iSubset);
};

SSS_END


