#include "RenderState.h"

SSS_USING

CRenderState::~CRenderState()
{
	SAFE_RELEASE(m_pState);
}

CRenderState::CRenderState():
	m_pState(NULL),
	m_pPrevState(NULL)
{
}

void CRenderState::SetKey(const string & strKey)
{
	m_strKey = strKey;
}

const string & CRenderState::GetKey() const
{
	return m_strKey;
}

RENDER_STATE CRenderState::GetRenderStateType() const
{
	return m_eRenderStateType;
}
