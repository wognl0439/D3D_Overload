#pragma once
#include "Ref.h"

SSS_BEGIN

class SSS_DLL CRenderState :
	public CRef
{
protected:
	CRenderState();
	virtual ~CRenderState() = 0;

protected:
	//ID3D11DeviceChild가 BlendState, DepthStencilState, RasterizerState에 상속을 내려주고 있으므로, 이거 하나로 처리함.
	ID3D11DeviceChild*		m_pState;
	ID3D11DeviceChild*		m_pPrevState;
	RENDER_STATE			m_eRenderStateType;
	string						m_strKey;

public:
	void SetKey(const string& strKey);
	const string& GetKey() const;
	RENDER_STATE GetRenderStateType()	const;

public:
	virtual void SetState() = 0;
	virtual void ResetState() = 0;
};

SSS_END
