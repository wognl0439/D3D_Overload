#include "SceneComponent.h"

SSS_USING


CSceneComponent::CSceneComponent() :
	m_pScene(NULL)
{
}

CSceneComponent::~CSceneComponent()
{
}


void CSceneComponent::SetScene(CScene * pScene)
{
	m_pScene = pScene;
}

bool CSceneComponent::Initialize()
{
	return true;
}

int CSceneComponent::Input(float fTime)
{
	return 0;
}

int CSceneComponent::Update(float fTime)
{
	return 0;
}

int CSceneComponent::LateUpdate(float fTime)
{
	return 0;
}

int CSceneComponent::Collision(float fTime)
{
	return 0;
}

void CSceneComponent::Render(float fTime)
{
}
