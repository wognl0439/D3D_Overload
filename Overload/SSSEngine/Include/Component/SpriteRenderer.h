#pragma once
#include "../Component.h"


SSS_BEGIN

class SSS_DLL CSpriteRenderer :
	public CComponent
{
private:
	friend class CGameObject;
	
public:
	CSpriteRenderer();
	CSpriteRenderer(const CSpriteRenderer& renderer);
	~CSpriteRenderer();

private:

	int m_iZOrder;
	class CMesh*				m_pMesh;
	class CShader*			m_pShader;
	ID3D11InputLayout*	m_pInputLayout;
	class CMaterial*			m_pMaterial;
	class CRenderState*	m_pRenderState[RS_END];
	ANIMATIONCLIP2DINFO	m_tAnimCBuffer;
	string	m_strAtlasKey;
	int32_t		m_iHasAnimation;

public:
	void SetAnimationCondition(int32_t iCondition = AC_NONE);
	int32_t GetAnimationCondition() const;
	bool Initialize() override;
	void Render(float fTime) override;
	CSpriteRenderer* Clone()	const override;

public:
	void UpdateTransform();
	void UpdatePrerender();
	void UpdatePostrender();
	void SetDefaultMaterial();

public:
	int GetZOrder() const;
	void SetZOrder(int iOrder);
	const string& GetAtlasKey() const;
	void SetImageInfo(const string& strKey);
	void SetImageInfo(int iStartX, int iStartY, int iLengthX, int iLengthY, int iMaxSizeX, int iMaxSizeY, ANIMATION_TYPE eType = AT_CUSTOM);
	void SetMesh(class CMesh* pMesh);
	void SetMesh(const string& strKey);
	void SetShader(class CShader* pShader);
	void SetShader(const string& strKey);
	void SetInputLayout(ID3D11InputLayout* pInputLayout);
	void SetInputLayout(const string& strKey);

	class CMaterial* GetMaterial() const;
	void SetMaterial(class CMaterial* pMaterial);
	
	void SetDiffuse(const Vector4& vDiffuse);
	bool SetDiffuseTexture(const string& strKey, const wchar_t* pFileName,	const string& strPathKey = TEXTURE_PATH);
	bool SetSampler(const string& strKey);
	void SetRenderState(const string& strKey);

	const Vector4& GetDiffuse() const;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END
