#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"
#include "Layer.h"
#include "Component.h"
#include "Collider.h"
#include "Device.h"
#include "PathManager.h"
#include "SceneManager.h"


#include "Component/MeshRenderer.h"
#include "Component/SpriteRenderer.h"
#include "Component/CircleCollider2D.h"
#include "Component/RectCollider2D.h"
#include "Component/BoxCollider.h"
#include "Component/SphereCollider.h"
#include "Component/UICollider.h"
#include "Component/Animator.h"
#include "Component/Camera.h"
#include "Component/DirectionalLight.h"
#include "Component/PointLight.h"
#include "Component/SpotLight.h"
#include "Component/PostProcessing.h"
#include "Component/CardinalSpline.h"
#include "Component/OutLine.h"
#include "Component/Terrain.h"
#include "Component/Billboard.h"
#include "Component/Text.h"
#include "Component/Rigidbody.h"
#include "Component/TerrainCollider.h"
#include "Component/MeshCollider.h"
#include "Component/Fire.h"
#include "Component/Water.h"
#include "Component/Distortion.h"
#include "Component/Trail.h"
#include "Component/Effect.h"
#include "Component/SpawningPool.h"
#include "Debug.h"

//Exclude
#include "Component/Gizmo.h"

SSS_USING

unordered_map<class CScene*, unordered_map<string, CGameObject*>>	CGameObject::m_mapPrototype;
function<bool(CGameObject*, size_t, FILE*)>CGameObject::m_pLoadComponentFromExternalProject = NULL;

CGameObject::CGameObject() :
	m_pTransform(NULL),
	m_pScene(NULL),
	m_pLayer(NULL),
	m_bUseTransparency(false),
	m_bStatic(false),
	m_bOnCollision(false),
	m_bCull(false)

{
	m_pTransform = new CTransform;
	m_pTransform->Initialize();
	m_pTransform->m_pGameObject = this;
	m_pTransform->SetTag(m_strTag);
}

CGameObject::CGameObject(const CGameObject & pGameObject)
{
	*this = pGameObject;

	m_pTransform = pGameObject.m_pTransform->Clone();

	m_ComponentList.clear();
	list<CComponent*>::const_iterator	iter;
	list<CComponent*>::const_iterator	iterEnd = pGameObject.m_ComponentList.end();

	for (iter = pGameObject.m_ComponentList.begin(); iter != iterEnd; ++iter)
	{
		CComponent*	pComponent = (*iter)->Clone();
		pComponent->SetGameObject(this);
		pComponent->SetTransform(m_pTransform);
		m_ComponentList.push_back(pComponent);
	}
}

CGameObject::~CGameObject()
{
	Safe_Release_VecList(m_ComponentList);
	SAFE_RELEASE(m_pTransform);
}

CTransform* CGameObject::GetTransform() const
{
	if (m_pTransform)
		m_pTransform->AddRef();

	return m_pTransform;
}

CGameObject * CGameObject::GetRoot()
{	
	CTransform* pParent = m_pTransform->m_pParent;
	if (pParent)
	{
		return pParent->m_pGameObject->GetRoot();
	}

	return this;
}

void CGameObject::CollisionEnterAll(CCollider * pThis, CCollider * pTarget, float fTime)
{
	OnCollisionEnter(pThis, pTarget, fTime);
	
	vector<CTransform*>* vecChildren = m_pTransform->GetChildren();
	size_t iChildCount = vecChildren->size();
	for (size_t i = 0; i < iChildCount; ++i)
	{
		vecChildren->at(i)->m_pGameObject->CollisionEnterAll(pThis, pTarget, fTime);
	}
}

void CGameObject::CollisionStayAll(CCollider * pThis, CCollider * pTarget, float fTime)
{
	OnCollisionStay(pThis, pTarget, fTime);

	vector<CTransform*>* vecChildren = m_pTransform->GetChildren();
	size_t iChildCount = vecChildren->size();
	for (size_t i = 0; i < iChildCount; ++i)
	{
		vecChildren->at(i)->m_pGameObject->CollisionStayAll(pThis, pTarget, fTime);
	}
}

void CGameObject::CollisionExitAll(CCollider * pThis, CCollider * pTarget, float fTime)
{
	OnCollisionExit(pThis, pTarget, fTime);

	vector<CTransform*>* vecChildren = m_pTransform->GetChildren();
	size_t iChildCount = vecChildren->size();
	for (size_t i = 0; i < iChildCount; ++i)
	{
		vecChildren->at(i)->m_pGameObject->CollisionExitAll(pThis, pTarget, fTime);
	}
}

void CGameObject::SetClientComponentLoadCallbackFunction(bool(*pFunc)(CGameObject *, size_t, FILE*))
{
	m_pLoadComponentFromExternalProject = bind(pFunc, placeholders::_1, placeholders::_2, placeholders::_3);
}

bool CGameObject::Save(const wchar_t * pFileName, const string & strPathKey)
{
	wstring strFullPath;
	if (strPathKey != NO_USE_PATH)
	{
		strFullPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);
		if (strFullPath.empty())
		{
			return false;
		}
	}

	//전체 경로를 만들어준다.
	strFullPath += pFileName;
	string strFullPathA(strFullPath.begin(), strFullPath.end());

	FILE* pFile = NULL;
	fopen_s(&pFile, strFullPathA.c_str(), "wb");

	if (pFile)
	{
		if (Save(pFile))
		{
			fclose(pFile);
			return true;
		}
		else
		{
			fclose(pFile);
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CGameObject::Save(FILE * pFile)
{
	size_t iGameObjectNameLength = GetTag().length();
	char strGameObjectName[256] = {};
	strcpy_s(strGameObjectName, m_strTag.c_str());

	fwrite(&iGameObjectNameLength, sizeof(size_t), 1, pFile);
	fwrite(&strGameObjectName[0], sizeof(char), iGameObjectNameLength, pFile);
	
	bool bHasLayer = m_pLayer != NULL;
	fwrite(&bHasLayer, sizeof(bool), 1, pFile);

	size_t iLayerNameLength = 0;
	char strLayerName[256] = {};

	if (bHasLayer)
	{
		iLayerNameLength = m_pLayer->GetTag().length();
		strcpy_s(strLayerName, m_pLayer->GetTag().c_str());

		fwrite(&iLayerNameLength, sizeof(size_t), 1, pFile);
		fwrite(&strLayerName[0], sizeof(char), iLayerNameLength, pFile);		
	}

	size_t iComponentCount = m_ComponentList.size();
	fwrite(&iComponentCount, sizeof(size_t), 1, pFile);

	list<CComponent*>::iterator iter;
	list<CComponent*>::iterator iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd; ++iter)
	{ 
 		size_t iHash = (*iter)->m_iTypeHash;
		fwrite(&iHash, sizeof(size_t), 1, pFile);
		(*iter)->Save(pFile);
	}

	size_t iChildCount = m_pTransform->GetChildrenCount();
	fwrite(&iChildCount, sizeof(size_t), 1, pFile);
		
	m_pTransform->Save(pFile);

	for (size_t i = 0; i < iChildCount; ++i)
	{
		m_pTransform->m_vecChildren[i]->m_pGameObject->Save(pFile);
	}

	return true;
}

CGameObject * CGameObject::Load(CScene* pScene, const wchar_t * pFileName, const string & strPathKey)
{
	wstring strFullPath;
	if (strPathKey != NO_USE_PATH)
	{
		strFullPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);
		if (strFullPath.empty())
		{
			return NULL;
		}
	}

	//전체 경로를 만들어준다.
	strFullPath += pFileName;
	string strFullPathA(strFullPath.begin(), strFullPath.end());

	FILE* pFile = NULL;
	fopen_s(&pFile, strFullPathA.c_str(), "rb");

	if (pFile)
	{
		CGameObject* pResult = Load(pScene, pFile);

		if(pResult)
		{
			fclose(pFile);
			return pResult;
		}
		else
		{
			fclose(pFile);
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

CGameObject * CGameObject::Load(CScene * pScene, FILE * pFile, CGameObject* pTarget)
{
	CGameObject* pGameObject = new CGameObject;

	if (pTarget)
	{
		SAFE_RELEASE(pGameObject);
		pGameObject = pTarget;
	}

	pGameObject->SetScene(pScene);

	size_t iGameObjectNameLength = 0;
	char strGameObjectName[256] = {};
	fread(&iGameObjectNameLength, sizeof(size_t), 1, pFile);
	fread(&strGameObjectName[0], sizeof(char), iGameObjectNameLength, pFile);	
	
	pGameObject->SetTag(strGameObjectName);
	
	bool bHasLayer = false;
	fread(&bHasLayer, sizeof(bool), 1, pFile);

	size_t iLayerNameLength = 0;
	char strLayerName[256] = {};

	if (bHasLayer)
	{
		fread(&iLayerNameLength, sizeof(size_t), 1, pFile);
		fread(&strLayerName[0], sizeof(char), iLayerNameLength, pFile);

		CLayer* pLayer = pScene->FindLayer(strLayerName);

		if (pLayer)
		{
			pGameObject->SetLayer(pLayer);
			pLayer->AddGameObject(pGameObject); //여기서 Ref오름
		}
		else
		{
			//Scene에 존재하지 않는 Layer에 할당하려 했음.
			assert(false);
		}

		SAFE_RELEASE(pLayer);
	}
	else
	{
		pGameObject->SetLayer(NULL);
	}	

/*
	size_t iChildCount = 0;
	fread(&iChildCount, sizeof(size_t), 1, pFile);

	CTransform* pTransform = pGameObject->GetTransform();
	pTransform->Load(pFile);

	for (size_t i = 0; i < iChildCount; ++i)
	{
		CGameObject* pChild = CGameObject::Load(pScene, pFile);
		CTransform* pChildTransform = pChild->GetTransform();
		pChildTransform->SetParent(pTransform);
		SAFE_RELEASE(pChildTransform);
		SAFE_RELEASE(pChild);
	}
	SAFE_RELEASE(pTransform);
*/

	size_t iComponentCount = 0;
	fread(&iComponentCount, sizeof(size_t), 1, pFile);
	bool bHasAnimator = false;
	for (size_t i = 0; i < iComponentCount; ++i)
	{
		size_t iHash = 0;
		fread(&iHash, sizeof(size_t), 1, pFile);

		if (iHash == typeid(CMeshRenderer).hash_code())
		{
			CMeshRenderer* pComponent = pGameObject->AddComponent<CMeshRenderer>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CSpriteRenderer).hash_code())
		{
			CSpriteRenderer* pComponent = pGameObject->AddComponent<CSpriteRenderer>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CCircleCollider2D).hash_code())
		{
			CCircleCollider2D* pComponent = pGameObject->AddComponent<CCircleCollider2D>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CRectCollider2D).hash_code())
		{
			CRectCollider2D* pComponent = pGameObject->AddComponent<CRectCollider2D>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CBoxCollider).hash_code())
		{
			CBoxCollider* pComponent = pGameObject->AddComponent<CBoxCollider>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CSphereCollider).hash_code())
		{
			CSphereCollider* pComponent = pGameObject->AddComponent<CSphereCollider>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CRigidbody).hash_code())
		{
			CRigidbody* pComponent = pGameObject->AddComponent<CRigidbody>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CTerrainCollider).hash_code())
		{
			CTerrainCollider* pComponent = pGameObject->AddComponent<CTerrainCollider>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CMeshCollider).hash_code())
		{
			CMeshCollider* pComponent = pGameObject->AddComponent<CMeshCollider>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CUICollider).hash_code())
		{
			CUICollider* pComponent = pGameObject->AddComponent<CUICollider>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CAnimator).hash_code())
		{
			CAnimator* pComponent = pGameObject->AddComponent<CAnimator>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
				bHasAnimator = true;
			}
		}
		else if (iHash == typeid(CCamera).hash_code())
		{
			CCamera* pComponent = pGameObject->AddComponent<CCamera>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CDirectionalLight).hash_code())
		{
			CDirectionalLight* pComponent = pGameObject->AddComponent<CDirectionalLight>();
			pComponent->Load(pFile);
			SAFE_RELEASE(pComponent);
		}
		else if (iHash == typeid(CPointLight).hash_code())
		{
			CPointLight* pComponent = pGameObject->AddComponent<CPointLight>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CSpotLight).hash_code())
		{
			CSpotLight* pComponent = pGameObject->AddComponent<CSpotLight>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CPostProcessing).hash_code())
		{
			CPostProcessing* pComponent = pGameObject->AddComponent<CPostProcessing>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CCardinalSpline).hash_code())
		{
			CCardinalSpline* pComponent = pGameObject->AddComponent<CCardinalSpline>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(COutLine).hash_code())
		{
			COutLine* pComponent = pGameObject->AddComponent<COutLine>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CTerrain).hash_code())
		{
			CTerrain* pComponent = pGameObject->AddComponent<CTerrain>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CBillboard).hash_code())
		{
			CBillboard* pComponent = pGameObject->AddComponent<CBillboard>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CText).hash_code())
		{
			CText* pComponent = pGameObject->AddComponent<CText>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CFire).hash_code())
		{
			CFire* pComponent = pGameObject->AddComponent<CFire>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CWater).hash_code())
		{
			CWater* pComponent = pGameObject->AddComponent<CWater>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CDistortion).hash_code())
		{
			CDistortion* pComponent = pGameObject->AddComponent<CDistortion>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CTrail).hash_code())
		{
			CTrail* pComponent = pGameObject->AddComponent<CTrail>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CEffect).hash_code())
		{
			CEffect* pComponent = pGameObject->AddComponent<CEffect>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else if (iHash == typeid(CSpawningPool).hash_code())
		{
			CSpawningPool* pComponent = pTarget->AddComponent<CSpawningPool>();
			if (pComponent)
			{
				pComponent->Load(pFile);
				SAFE_RELEASE(pComponent);
			}
		}
		else
		{
			if (m_pLoadComponentFromExternalProject)
			{
				m_pLoadComponentFromExternalProject(pGameObject, iHash, pFile);
			}	
		}
	}

	size_t iChildCount = 0;
	fread(&iChildCount, sizeof(size_t), 1, pFile);

	CTransform* pTransform = pGameObject->GetTransform();
	pTransform->Load(pFile);

	for (size_t i = 0; i < iChildCount; ++i)
	{
		CGameObject* pChild = CGameObject::Load(pScene, pFile);
		CTransform* pChildTransform = pChild->GetTransform();
		pChildTransform->SetParent(pTransform);
		SAFE_RELEASE(pChildTransform);
		SAFE_RELEASE(pChild);
	}	
	SAFE_RELEASE(pTransform);

	if (bHasAnimator)
	{
		CAnimator* pAnimator = pGameObject->GetComponent<CAnimator>();
		pAnimator->SetBoneTransformFromReservation();
		SAFE_RELEASE(pAnimator);
	}

	return pGameObject;
}

CGameObject * CGameObject::CreateObject(const string & strTag, CLayer * pLayer)
{
	CGameObject*	pObj = new CGameObject;

	pObj->SetTag(strTag);

	if (!pObj->Initialize())
	{
		SAFE_RELEASE(pObj);
		return NULL;
	}

	if (pLayer)
		pLayer->AddGameObject(pObj);
	return pObj;
}

bool CGameObject::UseTransparency() const
{
	return m_bUseTransparency;
}

void CGameObject::SetTransparency(bool bUse)
{
	m_bUseTransparency = bUse;
}

void CGameObject::SetStatic(const bool bStatic)
{
	m_bStatic = bStatic;
}

bool CGameObject::IsStatic()	const
{
	return m_bStatic;
}

CGameObject * CGameObject::CreatePrototype(const string & strTag, CScene * pScene)
{

	unordered_map<CScene*, unordered_map<string, CGameObject*>>::iterator	iter = m_mapPrototype.find(pScene);

	if (iter == m_mapPrototype.end())
	{
		unordered_map<string, CGameObject*>	map;

		m_mapPrototype.insert(make_pair(pScene, map));
	}

	CGameObject*	pPrototype = FindPrototype(strTag, pScene);

	if (pPrototype)
	{
		pPrototype->AddRef();
		return pPrototype;
	}

	pPrototype = new CGameObject;

	pPrototype->SetTag(strTag);
	pPrototype->SetScene(pScene);

	if (!pPrototype->Initialize())
	{
		SAFE_RELEASE(pPrototype);
		return NULL;
	}

	unordered_map<string, CGameObject*>*	pMap = FindPrototypeList(pScene);

	pPrototype->AddRef();
	pMap->insert(make_pair(strTag, pPrototype));

	return pPrototype;
}

CGameObject * CGameObject::CreateClone(const string & strTag, CScene * pScene, CLayer * pLayer)
{
	CGameObject*	pPrototype = FindPrototype(strTag, pScene);

	if (!pPrototype)
		return NULL;

	CGameObject*	pClone = pPrototype->Clone();
	
	//Safe_Release_VecList(ColliderList);

	if (pLayer)
		pLayer->AddGameObject(pClone);	

	return pClone;
}

//다른 Scene으로 Prototype을 이전 해 줄 필요가 있을 때, 지워질 SceneComponent 의 소멸자에서 사용하도록 한다.
bool CGameObject::ChangePrototypeScene(const string & strTag, CScene * pCurScene, CScene * pChangeScene)
{
	unordered_map<CScene*, unordered_map<string, CGameObject*>>::iterator	iter = m_mapPrototype.find(pChangeScene);

	if (iter == m_mapPrototype.end())
	{
		unordered_map<string, CGameObject*>	map;

		m_mapPrototype.insert(make_pair(pChangeScene, map));
	}

	CGameObject*	pPrototype = FindPrototype(strTag, pCurScene);

	if (!pPrototype)
		return false;

	pPrototype->SetScene(pChangeScene);

	unordered_map<string, CGameObject*>*	pMap = FindPrototypeList(pCurScene);

	unordered_map<string, CGameObject*>::iterator	iterProto = pMap->find(strTag);

	pMap->erase(iterProto);

	pMap = FindPrototypeList(pChangeScene);

	pMap->insert(make_pair(strTag, pPrototype));

	return true;
}

bool CGameObject::ErasePrototype(const string & strTag, CScene * pScene)
{
	unordered_map<string, CGameObject*>*	pMap = FindPrototypeList(pScene);

	if (!pMap)
		return false;

	unordered_map<string, CGameObject*>::iterator	iter = pMap->find(strTag);

	if (iter == pMap->end())
		return false;

	SAFE_RELEASE(iter->second);

	pMap->erase(iter);

	return true;
}

bool CGameObject::ErasePrototypeList(CScene * pScene)
{
	unordered_map<CScene*, unordered_map<string, CGameObject*>>::iterator	iterMap = m_mapPrototype.find(pScene);

	if (iterMap == m_mapPrototype.end())
		return false;

	unordered_map<string, CGameObject*>::iterator	iter;
	unordered_map<string, CGameObject*>::iterator	iterEnd = iterMap->second.end();

	for (iter = iterMap->second.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE(iter->second);
	}

	iterMap->second.clear();

	m_mapPrototype.erase(iterMap);

	return true;
}

unordered_map<string, CGameObject*>* CGameObject::FindPrototypeList(CScene * pScene)
{
	unordered_map<class CScene*, unordered_map<string, CGameObject*>>::iterator	iter = m_mapPrototype.find(pScene);

	if (iter == m_mapPrototype.end())
		return NULL;

	return &iter->second;
}

CGameObject * CGameObject::FindPrototype(const string & strTag, CScene * pScene)
{
	unordered_map<string, CGameObject*>*	pMap = FindPrototypeList(pScene);

	if (!pMap)
		return NULL;

	unordered_map<string, CGameObject*>::iterator	iter = pMap->find(strTag);

	if (iter == pMap->end())
		return NULL;

	return iter->second;
}

CScene* CGameObject::GetScene() const
{
	m_pScene->AddRef();
	return m_pScene;
}

CLayer* CGameObject::GetLayer() const
{
	m_pLayer->AddRef();
	return m_pLayer;
}

void CGameObject::SetScene(CScene * pScene)
{
	m_pScene = pScene;

	m_pTransform->SetScene(pScene);

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->SetScene(pScene);
	}
}

void CGameObject::SetLayer(CLayer * pLayer)
{
	m_pLayer = pLayer;

	m_pTransform->SetLayer(pLayer);

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->SetLayer(pLayer);
	}
}

void CGameObject::ChangeLayer(CLayer * pTargetLayer)
{
	m_pLayer->RemoveGameObject(this);
	pTargetLayer->AddGameObject(this);
	this->Release();
}

void CGameObject::Reset()
{
	list<CComponent*>::iterator iter;
	list<CComponent*>::iterator iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd; ++iter)
	{
		if (!(*iter)->m_bDestroy)
		{
			(*iter)->Reset();
		}
	}
}

bool CGameObject::Initialize()
{
	return true;
}

void CGameObject::Start()
{
	SCENE_STATE eState = GET_SINGLE(CSceneManager)->GetSceneState();

	bool bIsGizmo = HasComponent<CGizmo>();
	bool bIsLight = HasComponent<CLight>();

	if (eState == SS_PLAY || m_pScene->IsCamera(this) || bIsGizmo || bIsLight)
	{
		list<CComponent*>::iterator	iter;
		list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

		for (iter = m_ComponentList.begin(); iter != iterEnd;)
		{
			if ((*iter)->IsDestroy())
			{
				SAFE_RELEASE((*iter));
				iter = m_ComponentList.erase(iter);
				iterEnd = m_ComponentList.end();
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
	}
}

int CGameObject::Update(float fTime)
{
	SCENE_STATE eState = GET_SINGLE(CSceneManager)->GetSceneState();

	bool bIsGizmo = HasComponent<CGizmo>();
	bool bIsLight = HasComponent<CLight>();

	if (eState == SS_PLAY || m_pScene->IsCamera(this) || bIsGizmo || bIsLight)
	{
		list<CComponent*>::iterator	iter;
		list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

		for (iter = m_ComponentList.begin(); iter != iterEnd;)
		{
			if (!(*iter)->m_bStarted)
			{
				(*iter)->Start();
				(*iter)->m_bStarted = true;
			}


			if ((*iter)->IsDestroy())
			{
				SAFE_RELEASE((*iter));
				iter = m_ComponentList.erase(iter);
				iterEnd = m_ComponentList.end();
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
	}


	if (eState != SS_PLAY)
	{
		m_pTransform->Update(fTime);
	}
	else
	{
		if (!m_pTransform->m_bDoNotUpdateInProcedural && !m_bStatic)
		{
			m_pTransform->Update(fTime);
		}
	}

	return 0;
}

int CGameObject::LateUpdate(float fTime)
{
	SCENE_STATE eState = GET_SINGLE(CSceneManager)->GetSceneState();

	bool bIsGizmo = HasComponent<CGizmo>();
	bool bIsLight = HasComponent<CLight>();

	if (eState == SS_PLAY || m_pScene->IsCamera(this) || bIsGizmo || bIsLight)
	{
		list<CComponent*>::iterator	iter;
		list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

		for (iter = m_ComponentList.begin(); iter != iterEnd;)
		{
			if ((*iter)->IsDestroy())
			{
				SAFE_RELEASE((*iter));
				iter = m_ComponentList.erase(iter);
				iterEnd = m_ComponentList.end();
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
	}


	if (eState != SS_PLAY)
	{
		m_pTransform->Update(fTime);
	}
	else
	{
		if (!m_pTransform->m_bDoNotUpdateInProcedural && !m_bStatic)
		{
			m_pTransform->Update(fTime);
		}
	}


	return 0;
}


int CGameObject::OnCollisionEnter(class CCollider* pThis, class CCollider* pTarget, float fTime)
{
	m_bOnCollision = true;
	SCENE_STATE eState = GET_SINGLE(CSceneManager)->GetSceneState();

	if (eState == SS_PLAY)
	{
		list<CComponent*>::iterator	iter;
		list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

		for (iter = m_ComponentList.begin(); iter != iterEnd;)
		{
			if ((*iter)->IsDestroy())
			{
				SAFE_RELEASE((*iter));
				iter = m_ComponentList.erase(iter);
				iterEnd = m_ComponentList.end();
				continue;
			}
			else if (!(*iter)->IsEnable())
			{
				++iter;
				continue;
			}
			(*iter)->OnCollisionEnter(pThis, pTarget, fTime);

			++iter;
		}
	}
	return 0;
}

int CGameObject::OnCollisionStay(class CCollider* pThis, class CCollider* pTarget, float fTime)
{
	SCENE_STATE eState = GET_SINGLE(CSceneManager)->GetSceneState();

	if (eState == SS_PLAY)
	{
		list<CComponent*>::iterator	iter;
		list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

		for (iter = m_ComponentList.begin(); iter != iterEnd;)
		{
			if ((*iter)->IsDestroy())
			{
				SAFE_RELEASE((*iter));
				iter = m_ComponentList.erase(iter);
				iterEnd = m_ComponentList.end();
				continue;
			}
			else if (!(*iter)->IsEnable())
			{
				++iter;
				continue;
			}
			(*iter)->OnCollisionStay(pThis, pTarget, fTime);

			++iter;
		}
	}
	return 0;
}

int CGameObject::OnCollisionExit(class CCollider* pThis, class CCollider* pTarget, float fTime)
{
	m_bOnCollision = false;
	SCENE_STATE eState = GET_SINGLE(CSceneManager)->GetSceneState();

	if (eState == SS_PLAY)
	{
		list<CComponent*>::iterator	iter;
		list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

		for (iter = m_ComponentList.begin(); iter != iterEnd;)
		{
			if ((*iter)->IsDestroy())
			{
				SAFE_RELEASE((*iter));
				iter = m_ComponentList.erase(iter);
				iterEnd = m_ComponentList.end();
				continue;
			}
			else if (!(*iter)->IsEnable())
			{
				++iter;
				continue;
			}
			(*iter)->OnCollisionExit(pThis, pTarget, fTime);

			++iter;
		}
	}
	return 0;
}

int CGameObject::OnMouseEnter(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd;)
	{
		if ((*iter)->IsDestroy())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComponentList.erase(iter);
			iterEnd = m_ComponentList.end();
			continue;
		}
		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}
		(*iter)->OnMouseEnter(pTarget, vPosition, vIntersectPosition);

		++iter;
	}
	return 0;
}

int CGameObject::OnMouseStay(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd;)
	{
		if ((*iter)->IsDestroy())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComponentList.erase(iter);
			iterEnd = m_ComponentList.end();
			continue;
		}
		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}
		(*iter)->OnMouseStay(pTarget, vPosition, vIntersectPosition, fTime);

		++iter;
	}
	return 0;
}

int CGameObject::OnMouseExit(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd;)
	{
		if ((*iter)->IsDestroy())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComponentList.erase(iter);
			iterEnd = m_ComponentList.end();
			continue;
		}
		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}
		(*iter)->OnMouseExit(pTarget, vPosition, vIntersectPosition);

		++iter;
	}
	return 0;
}

int CGameObject::OnMouseDown(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd;)
	{
		if ((*iter)->IsDestroy())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComponentList.erase(iter);
			iterEnd = m_ComponentList.end();
			continue;
		}
		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}
		(*iter)->OnMouseDown(pTarget, vPosition, vIntersectPosition);

		++iter;
	}
	return 0;
}

int CGameObject::OnMouseDrag(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd;)
	{
		if ((*iter)->IsDestroy())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComponentList.erase(iter);
			iterEnd = m_ComponentList.end();
			continue;
		}
		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}
		(*iter)->OnMouseDrag(pTarget, vPosition, vIntersectPosition, fTime);

		++iter;
	}
	return 0;
} 

int CGameObject::OnMouseUp(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd;)
	{
		if ((*iter)->IsDestroy())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComponentList.erase(iter);
			iterEnd = m_ComponentList.end();
			continue;
		}
		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}
		(*iter)->OnMouseUp(pTarget, vPosition, vIntersectPosition);

		++iter;
	}
	return 0;
}

void CGameObject::Render(float fTime)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd;)
	{
		if ((*iter)->IsDestroy())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComponentList.erase(iter);
			iterEnd = m_ComponentList.end();
			continue;
		}
		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Render(fTime);
		++iter;
	}
}

void CGameObject::RenderDebug(float fTime)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd;)
	{
		if ((*iter)->IsDestroy())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComponentList.erase(iter);
			iterEnd = m_ComponentList.end();
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

void CGameObject::RenderShadowMap(float fTime)
{
	CMeshRenderer*	pRenderer = GetComponent<CMeshRenderer>();

	if (!pRenderer)
		return;

	pRenderer->RenderShadowMap(fTime);

	SAFE_RELEASE(pRenderer);
}

CGameObject * CGameObject::Clone() const
{
	return new CGameObject(*this);
}

CComponent * CGameObject::AddComponent(CComponent * pComponent)
{
	pComponent->SetScene(m_pScene);
	pComponent->SetLayer(m_pLayer);
	pComponent->SetGameObject(this);
	pComponent->SetTransform(m_pTransform);
	pComponent->AddRef();

	m_ComponentList.push_back(pComponent);

	return pComponent;
}

void CGameObject::RemoveComponentFromList(CComponent * pComponent)
{
	size_t iComponentCount = m_ComponentList.size();

	list<CComponent*>::iterator iter;
	list<CComponent*>::iterator iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter) == pComponent)
		{
			m_ComponentList.erase(iter);
			return;
		}
	}
}

size_t CGameObject::GetComponentCount() const
{	
	return m_ComponentList.size();
}

const list<class CCollider*>* CGameObject::GetColliderList() const
{
	return &m_ColliderList;
}

list<CComponent*>& CGameObject::GetAllComponents()
{
	return m_ComponentList;
}

//여러 컴포넌트 타입을 캐싱해서 쓰면 안됨
const list<CComponent*>* CGameObject::GetComponentsFromTag(const string & strTag)
{
	m_FindComponentList.clear();

	list<CComponent*>::const_iterator	iter;
	list<CComponent*>::const_iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strTag)
		{
			(*iter)->AddRef();
			m_FindComponentList.push_back(*iter);
		}
	}

	return &m_FindComponentList;
}

