#include "Layer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Scene.h"
#include "Device.h"
#include "RenderManager.h"
#include "Component/Camera.h"
#include "Component/MeshRenderer.h"
#include "Transform.h"
#include "Mesh.h"
#include "Component/Particle.h"
#include "Component/SpriteRenderer.h"
#include "Component/Trail.h"

SSS_USING


CLayer::CLayer() :
	m_iZOrder(0)
{
}

CLayer::~CLayer()
{
	Safe_Release_VecList(m_GameObjectList);
}

void CLayer::SetScene(CScene * pScene)
{
	m_pScene = pScene;
}

void CLayer::SetZOrder(int iZOrder)
{
	m_iZOrder = iZOrder;
}

int CLayer::GetZOrder() const
{
	return m_iZOrder;
}

void CLayer::RemoveGameObject(CGameObject * pGameObject)
{
	list<CGameObject*>::iterator iter;
	list<CGameObject*>::iterator iterEnd = m_GameObjectList.end();

	for (iter = m_GameObjectList.begin(); iter != iterEnd; ++iter)
	{
		if (*iter == pGameObject)
		{
			m_GameObjectList.erase(iter);
			break;
		}
	}
}

void CLayer::AddGameObject(CGameObject * pObj)
{
	pObj->SetScene(m_pScene);
	pObj->SetLayer(this);
	pObj->AddRef();
	m_GameObjectList.push_back(pObj);
}

 list<class CGameObject*>* CLayer::GetAllGameObjects() 
{
	return &m_GameObjectList;
}

CGameObject* CLayer::FindGameObjectWithTag(const string & strTag)
{
	list<CGameObject*>::iterator iter;
	list<CGameObject*>::iterator iterEnd = m_GameObjectList.end();

	for (iter = m_GameObjectList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strTag)
		{
			(*iter)->AddRef();
			return (*iter);
		}
	}
	return NULL;
}

bool CLayer::Save(FILE * pFile)
{
	return false;
}

bool CLayer::Load(FILE * pFile)
{
	return false;
}


bool CLayer::Initialize()
{
	return true;
}

int CLayer::Start()
{
	list<CGameObject*>::iterator	iter;
	list<CGameObject*>::iterator	iterEnd = m_GameObjectList.end();

	for (iter = m_GameObjectList.begin(); iter != iterEnd;)
	{
		if ((*iter)->IsDestroy())
		{
			SAFE_RELEASE((*iter));
			iter = m_GameObjectList.erase(iter);
			iterEnd = m_GameObjectList.end();
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Start();

		++iter;
	}

	return 0;
}

int CLayer::Update(float fTime)
{
	list<CGameObject*>::iterator	iter;
	list<CGameObject*>::iterator	iterEnd = m_GameObjectList.end();

	for (iter = m_GameObjectList.begin(); iter != iterEnd;)
	{
		if ((*iter)->IsDestroy())
		{
			SAFE_RELEASE((*iter));
			iter = m_GameObjectList.erase(iter);
			iterEnd = m_GameObjectList.end();
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Update(fTime);

		++iter;
	}
	return 0;
}

int CLayer::LateUpdate(float fTime)
{
	list<CGameObject*>::iterator	iter;
	list<CGameObject*>::iterator	iterEnd = m_GameObjectList.end();

	for (iter = m_GameObjectList.begin(); iter != iterEnd;)
	{
		if ((*iter)->IsDestroy())
		{
			SAFE_RELEASE((*iter));
			iter = m_GameObjectList.erase(iter);
			iterEnd = m_GameObjectList.end();
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->LateUpdate(fTime);

		++iter;
	}
	return 0;
}

int CLayer::Collision(float fTime)
{	
	return 0;
}

void CLayer::Render(float fTime)
{
	//화면 밖의 오브젝트는 렌더하지 않음.
	//CTransform* pCameraTransform = m_pScene->GetMainCameraTransform();
	//Vector3 vCameraPosition = pCameraTransform->GetWorldPosition();
	//Vector3 vOuterScreen = vCameraPosition + Vector3(DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 0.0f);
	//SAFE_RELEASE(pCameraTransform);



	list<CGameObject*>::iterator	iter;
	list<CGameObject*>::iterator	iterEnd = m_GameObjectList.end();

	for (iter = m_GameObjectList.begin(); iter != iterEnd;)
	{
		if ((*iter)->IsDestroy())
		{
			SAFE_RELEASE((*iter));
			iter = m_GameObjectList.erase(iter);
			iterEnd = m_GameObjectList.end();
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		//Frustum Culling
		
		bool bCulling = true;
		Vector3 vCenter;
		float fRadius = 0;
		CCamera* pCamera = m_pScene->GetMainCamera();
		CMeshRenderer* pRenderer = (*iter)->GetComponent<CMeshRenderer>();			
		if (pRenderer)
		{
			CTransform* pTransform = (*iter)->GetTransform();
			float fMaxScale = pTransform->GetWorldScale().Max();			
			fRadius = pRenderer->GetMesh()->GetRadius() * fMaxScale * 2.0f;
			vCenter = pTransform->GetWorldPosition();
			SAFE_RELEASE(pTransform);

			bCulling = pCamera->SphereOutsideFrustum(vCenter, fRadius);
		}
		else
		{
			//렌더러가 없으면 컬링
			bCulling = true;
		}



		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pCamera);

		if (bCulling)
		{
			if ((*iter)->HasComponent<CParticle>() || (*iter)->HasComponent<CSpriteRenderer>()
				|| (*iter)->HasComponent<CTrail>())
			{
				GET_SINGLE(CRenderManager)->AddRenderObject(*iter);
				++iter;
			}
			else
			{
				++iter;
			}
		}
		else
		{
			GET_SINGLE(CRenderManager)->AddRenderObject(*iter);
			++iter;
		}		
	}
}
void CLayer::RenderDebug(float fTime)
{
	list<CGameObject*>::iterator	iter;
	list<CGameObject*>::iterator	iterEnd = m_GameObjectList.end();

	for (iter = m_GameObjectList.begin(); iter != iterEnd;)
	{
		if ((*iter)->IsDestroy())
		{
			SAFE_RELEASE((*iter));
			iter = m_GameObjectList.erase(iter);
			iterEnd = m_GameObjectList.end();
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->RenderDebug(fTime);
		++iter;

	}
}

