#pragma once
#include "../Component.h"

SSS_BEGIN

class SSS_DLL CMeshRenderer :
	public CComponent
{
private:
	friend class CGameObject;

private:
	CMeshRenderer();
	CMeshRenderer(const CMeshRenderer& component);
	~CMeshRenderer();


private:
	class CMesh*		m_pMesh;
	class CShader*		m_pShader;
	ID3D11InputLayout*	m_pInputLayout;
	class CMaterial*	m_pMaterial;
	class CRenderState*	m_pRenderState[RS_END];
	int32_t		m_iHasAnimation;

	bool	 m_bMotionBlur;
	bool	m_bUseMeshLocal;

public:
	bool GetEnableMeshContainer(int iContainer) const;
	void SetEnableMeshContainer(int iContainer, bool bEnable);
	void SetAnimationCondition(int32_t iCondition = AC_NONE);
	int32_t GetAnimationCondition() const;
	class CMaterial* GetMaterial()	const;
	void SetMaterial(class CMaterial* pMaterial);

public:
	bool Initialize() override;
	void Render(float fTime)override;
	CMeshRenderer* Clone()	 const override;

private:
	void UpdateTransform();
	void UpdatePrerender();
	void UpdatePostrender();
	void SetDefaultMaterial();

public:
	size_t GetContainerCount() const;
	bool ChangeMeshContainer(const string& strMesh, int iContainer);
	bool ChangeMeshContainer(CMesh* pMesh, int iContainer);
	//ÇØ´ç ÀÎµ¦½ºÀÇ µÚÂÊ¿¡ »ðÀÔµÊ
	bool InsertMeshContainer(const string& strMesh, int iContainer);
	//ÇØ´ç ÀÎµ¦½ºÀÇ µÚÂÊ¿¡ »ðÀÔµÊ
	bool InsertMeshContainer(CMesh* pMesh, int iContainer);
	bool RemoveMeshContainer(int iContainer = 0);

	void SetLocalTransformFromMesh(int iContainer = 0);
	ID3D11InputLayout* GetInputLayout() const;
	class CMesh* GetMesh() const;
	class CShader* GetShader() const;
	
	void SetMesh(class CMesh* pMesh, bool bOverrideMaterial = false);
	void SetMesh(const string& strKey, bool bOverrideMaterial = false);
	void SetShader(class CShader* pShader);
	void SetShader(const string& strKey);
	void SetInputLayout(ID3D11InputLayout* pInputLayout);
	void SetInputLayout(const string& strKey);
	void SetRenderState(const string& strKey);

	void RenderShadowMap(float fTime);
	void RenderMotionBlurMap();
	void SetMotionBlur(bool bSwitch);

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END