#include "LoadingPanel.h"
#include "Engine.h"
#include "ShaderManager.h"




void CLoadingPanel::SetLoadingProgress(float fProgress)
{
	m_tLoadingConstantBuffer.fProgress = fProgress;
}

CLoadingPanel::CLoadingPanel()
{
}

CLoadingPanel::~CLoadingPanel()
{
}

bool CLoadingPanel::Initialize()
{
	return true;
}

int CLoadingPanel::Prerender(CMeshRenderer * pRenderer)
{
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(LOADING_CBUFFER, &m_tLoadingConstantBuffer, CBT_PIXEL | CBT_VERTEX);
	return 0;
}

bool CLoadingPanel::Save(FILE * pFile)
{
	return true;
}

bool CLoadingPanel::Load(FILE * pFile)
{
	return true;
}
