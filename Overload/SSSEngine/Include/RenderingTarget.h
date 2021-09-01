#pragma once

#include "Engine.h"

SSS_BEGIN

class SSS_DLL CRenderingTarget
{
	friend class CRenderManager;

private:
	CRenderingTarget();

public:
	~CRenderingTarget();

private:
	string			m_strName;
	ID3D11RenderTargetView*		m_pTargetView;
	ID3D11RenderTargetView*		m_pTargetOldView;
	ID3D11Texture2D*			m_pTargetTexture;
	ID3D11ShaderResourceView*	m_pSRV;
	ID3D11DepthStencilView*		m_pDepthView;
	ID3D11DepthStencilView*		m_pDepthOldView;
	ID3D11Texture2D*			m_pDepthTexture;
	float						m_fClearColor[4];
	Vector3						m_vPos;
	Vector3						m_vScale;
	Matrix*						m_pMatWorld;

	bool						m_bDebug;
	class CShader*				m_pShader;
	class CMesh*				m_pMesh;
	ID3D11InputLayout*			m_pLayout;
	class CSampler*				m_pSampler;

	ID3D11RenderTargetView**	m_ppTargetView;
	int		m_ilevels;

public:
	ID3D11RenderTargetView* GetRenderTargetView()	const;
	ID3D11ShaderResourceView* GetShaderResourceView() const;
	ID3D11RenderTargetView**	GetRenderTargetViewMip() const;

public:
	void GenerateMips();
	void SetShader(int iRegister);
	void ResetShader(int iRegister);
	bool CreateTarget(const string& strName, UINT iWidth, UINT iHeight, UINT iMipLevel,
		DXGI_FORMAT eFmt, const Vector4& vClearColor, DXGI_FORMAT eDepthFmt = DXGI_FORMAT_UNKNOWN);
	void SetDebugInfo(const Vector3& vPos, const Vector3& vScale);
	void ClearTarget();
	void SetTarget();
	void ResetTarget();
	void Render(float fTime);
};

SSS_END

