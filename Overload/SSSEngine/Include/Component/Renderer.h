#pragma once
#include "../Component.h"


SSS_BEGIN

class SSS_DLL CRenderer :
	public CComponent
{
private:
	friend class CGameObject;
	
public:
	CRenderer();
	CRenderer(const CRenderer& renderer);
	~CRenderer();

private:
	RENDER_MODE			m_eRenderMode;
	class CMesh*				m_pMesh;
	class CShader*			m_pShader;
	ID3D11InputLayout*	m_pInputLayout;
	class CMaterial*	m_pMaterial;
	class CRenderState*	m_pRenderState[RS_END];
	ANIMATIONCLIP2DINFO	m_tAnimCBuffer;
	unordered_map<string, PRENDERERCBUFFER> m_mapRendererConstantBuffer;
	string	m_strAtlasKey;

public:
	bool Initialize() override;
	void Render(float fTime) override;
	CRenderer* Clone()	const override;

public:
	void UpdateTransform();

public:
	const string& GetAtlasKey() const;
	void SetImageInfo(const string& strKey);
	void SetImageInfo(int iStartX, int iStartY, int iLengthX, int iLengthY, int iMaxSizeX, int iMaxSizeY, ANIMATION_TYPE eType = AT_CUSTOM);
	void SetRenderMode(RENDER_MODE eRenderMode);
	void SetMesh(class CMesh* pMesh);
	void SetMesh(const string& strKey);
	void SetShader(class CShader* pShader);
	void SetShader(const string& strKey);
	void SetInputLayout(ID3D11InputLayout* pInputLayout);
	void SetInputLayout(const string& strKey);

	class CMaterial* GetMaterial() const;
	class CMaterial* CreateMaterial();

	PRENDERERCBUFFER CreateRendererConstantBuffer(const string& strKey, int iSize, int iTransfer);
	PRENDERERCBUFFER FindRendererConstantBuffer(const string& strKey);
	bool UpdateRendererConstantBuffer(const string& strKey, void* pData);

	void SetDiffuse(const Vector4& vDiffuse);
	bool SetDiffuseTexture(const string& strKey, const wchar_t* pFileName,	const string& strPathKey = TEXTURE_PATH);
	bool SetSampler(const string& strKey);
	void SetRenderState(const string& strKey);

	const Vector4& GetDiffuse() const;
};

SSS_END
