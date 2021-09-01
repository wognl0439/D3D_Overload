#pragma once

#include "Engine.h"

SSS_BEGIN

typedef struct SSS_DLL _tagLightSection
{
	class CLight**	pLightList;
	int				iSize;
	int				iCapacity;

	_tagLightSection() :
		iSize(0),
		iCapacity(100)
	{
		pLightList = new CLight*[iCapacity];
	}
}LIGHTSECTION, *PLIGHTSECTION;


typedef struct SSS_DLL _tagRenderGroup
{
	class CGameObject**	pObjList;
	int				iSize;
	int				iCapacity;

	_tagRenderGroup() :
		iSize(0),
		iCapacity(100)
	{
		pObjList = new CGameObject*[iCapacity];
	}
}RENDERGROUP, *PRENDERGROUP;

// Normal ~ End ¼øÀ¸·Î Render
enum RENDER_GROUP_TYPE
{
	RGT_NORMAL,
	RGT_DECAL,
	RGT_ALPHA,
	RGT_PREVBACBUFFER,
	RGT_UI,
	RGT_HUD,
	RGT_END
};

typedef struct SSS_DLL _tagMRT
{
	vector<class CRenderingTarget*>	vecTarget;
	vector<ID3D11RenderTargetView*>	vecOldTarget;
	ID3D11DepthStencilView*		pOldDepth;
}MRT, *PMRT;


class SSS_DLL CRenderManager
{
	DECLARE_SINGLE(CRenderManager)

private:
	unordered_map<string, class CRenderState*>	m_mapRenderState;
	RENDER_MODE		m_eRenderMode;
	class CRenderState*		m_pDepthDisable;
	class CRenderState*		m_pCullNone;
	LIGHTSECTION			m_tLightSection;


	RENDERGROUP				m_tRenderGroup[RGT_END];
	bool					m_bDeferred;
	class CShader*			m_pLightAccDirShader;
	class CShader*			m_pLightBlendShader;
	class CShader*			m_pRenderBlendShader;
	class CShader*			m_pRenderPrevBackBufferShader;
	class CPostProcessing*	m_pPostProcessing;

	bool		m_bShadowEnable;
	bool		m_bCameraMotionBlur;
	float		m_fShadowBias;

	// Point Volume Light
	class CMesh*			m_pPointLightMesh;
	class CShader*			m_pLightAccPointShader;
	ID3D11InputLayout*		m_pPointLightInputLayout;

private:
	unordered_map<string, class CRenderingTarget*>	m_mapRenderTarget;
	unordered_map<string, PMRT>	m_mapMRT;

public:
	bool Initialize();

public:
	RENDER_MODE GetRenderMode() const;
	void SetRenderMode(RENDER_MODE eMode);
	void SetDepth();
	void ResetDepth();
	
	void AddLight(class CLight* pLight);
	void ClearLight();
	void SetLight();
	void SetDirectionalLight();

	class CBlendState* CreateBlendState(const string& strKey, BOOL bAlphaCoverage = FALSE, BOOL bIndependent = FALSE);
	bool AddRenderTargetBlendDesc(const string& strKey, BOOL bBlendEnable = TRUE,
		D3D11_BLEND eSrc = D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND eDest = D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_OP eOption = D3D11_BLEND_OP_ADD,
		D3D11_BLEND eSrcAlpha = D3D11_BLEND_ONE,
		D3D11_BLEND eDestAlpha = D3D11_BLEND_ZERO,
		D3D11_BLEND_OP eOptionAlpha = D3D11_BLEND_OP_ADD,
		UINT iWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL);

	class CDepthStencilState* CreateDepthStencilState(const string& strKey, BOOL bEnable = TRUE,
		D3D11_DEPTH_WRITE_MASK eDepthMask = D3D11_DEPTH_WRITE_MASK_ALL,
		D3D11_COMPARISON_FUNC eDepthFunc = D3D11_COMPARISON_LESS,
		BOOL bStencilEnable = FALSE,
		UINT8 iStencilReadMask = 0xff,
		UINT8 iStencilWriteMask = 0xff,
		const D3D11_DEPTH_STENCILOP_DESC& tFrontFace = {},
		const D3D11_DEPTH_STENCILOP_DESC& tBackFace = {});


	class CRasterizerState* CreateRasterizerState(const string& strKey,
		D3D11_FILL_MODE eFillMode = D3D11_FILL_SOLID,
		D3D11_CULL_MODE eCullMode = D3D11_CULL_BACK);


	class CRenderState* FindRenderState(const string& strKey);


	/* Deferred Rendering*/
private:
	void SetPostProcessingProperties(float fTime);
public:
	bool SetPostProcessing(class CPostProcessing* pComponent);

public:
	void SetDeferredRendering(bool bDeferred);


public:
	bool CreateRenderTarget(const string& strName, UINT iWidth, UINT iHeight, UINT iMipLevel,
		DXGI_FORMAT eFmt, const Vector4& vClearColor,
		DXGI_FORMAT eDepthFmt = DXGI_FORMAT_UNKNOWN);
	bool CreateMultiRenderTarget(const string& strMRTName, const vector<string>& vecTarget);

private:
	class CRenderingTarget* FindRenderTarget(const string& strName);
	PMRT FindMRT(const string& strName);
	bool SetRenderTargetDebugInfo(const string& strName, const Vector3& vPos,
		const Vector3& vScale);
	void ClearMRT(const string& strName);
	void SetMRT(const string& strName);
	void ResetMRT(const string& strName);

public:
	void AddRenderObject(class CGameObject* pObj);
	void Render(float fTime);

private:
	void RenderForward(float fTime);
	void RenderDeferred(float fTime);
	void RenderGBuffer(float fTime);
	void RenderDecal(float fTime);
	void RenderLightAcc(float fTime);
	void RenderLightBlend(float fTime);
	void RenderBlendTarget(float fTime);
	void RenderFinalTarget(float fTime);
	void RenderMotionBlur(float fTime);
	void RenderShadowMap(float fTime);
	void UpdateShadowCBuffer(float fTime);

private:
	void RenderLightPoint(float fTime, class CPointLight* pLight);
	void AddRenderGroup(PRENDERGROUP pGroup, class CGameObject* pObj);

public:
	static bool SortZ(class CGameObject* pSrc, class CGameObject* pDest);
	static bool SortAlpha(class CGameObject* pSrc, class CGameObject* pDest);
	static bool SortZOrder(class CGameObject* pSrc, class CGameObject* pDest);




private:
	bool m_bInitialUpdate;

	//HDR
	ID3D11Buffer*							m_pHDROutputBuffer;
	ID3D11UnorderedAccessView*	m_pHDROutputUAV;
	ID3D11ShaderResourceView*		m_pHDROutputSRV;
	ID3D11ComputeShader*				m_pAverageLuminanceCS;

	bool SetHDRResources();
	float CalculateAverageLuminance();
	bool CreateComputeShader();
	bool LoadComputeShader(const wchar_t * pFileName, char * pEntry, const string & strPathKey, ID3DBlob** ppShaderBlob, ID3D11ComputeShader** pTargetShader);


	// PS HDR
	void RenderHDR(float fTime);
	void DrawFullScreenQuad();
	void BlurHorizontal(ID3D11ShaderResourceView* pSrc, ID3D11RenderTargetView* pDst);
	void BlurVertical(ID3D11ShaderResourceView* pSrc, ID3D11RenderTargetView* pDst);
	void Add(ID3D11ShaderResourceView* pSrc0, ID3D11ShaderResourceView* pSrc1, ID3D11RenderTargetView* pDst);
};

SSS_END

