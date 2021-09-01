#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"
#include "Device.h"
#include "Component\Camera.h"
#include "Transform.h"
#include "Collider.h"
#include "Component\UICollider.h"
#include "Component\MeshRenderer.h"
#include "Material.h"
#include "Component.h"

#include "Component/Gizmo.h"
#include "CollisionManager.h"
#include "Debug.h"

SSS_USING

CScene::CScene():
	m_bInitialized(false),
	m_pCollisionMatrix(NULL),
	m_pSkybox(NULL),
	m_pPhysicsScene(NULL)
{
	//Foundation부터 새로 만들어야 한다. 
	//하지만, 비동기 로드를 할 때, Scene이 다른 Foundation에 올라가 있기 때문에, Foundation을 보관 하고 있다가. 
	//Scene이 변경 되는 순간 PVD에 얹어질 Scene을 교체해주도록 한다.

	m_pDispatcher = NULL;
	m_pGPUDispatcher = NULL;
	m_pCudaContextManager = NULL;

	InitializePhysics();

	CLayer*	pLayer = CreateLayer("Default");
	SAFE_RELEASE(pLayer);

	pLayer = CreateLayer("UI", 1000000);
	SAFE_RELEASE(pLayer);

	pLayer = CreateLayer("Debug", 1001000);
	SAFE_RELEASE(pLayer);

	// 기본 메인카메라 생성
	m_pMainCamera = CreateCamera("Main Camera", Vector3(0.f, 0.f, -5.f));

	CCamera*	pCamera = m_pMainCamera->GetComponent<CCamera>();

	pCamera->SetPerspectiveProjection(DEVICE_RESOLUTION.iWidth / (float)DEVICE_RESOLUTION.iHeight, PI / 3.f, 0.5f);
	pCamera->SetOrthographyProjection(0.f, (float)DEVICE_RESOLUTION.iWidth, 0.f, (float)DEVICE_RESOLUTION.iHeight, -100.0f);

	SAFE_RELEASE(pCamera);


	SetSkybox("Default Skybox", L"DefaultSkybox.dds");


	//PVD갱신때문에 추가함.
	m_pPhysicsScene->simulate(0.01f); 
	m_pPhysicsScene->fetchResults(true);
	m_pPhysicsScene->simulate(0.01f);
	m_pPhysicsScene->fetchResults(true);
}

CScene::~CScene()
{
	if (m_pPhysicsScene)
	{
		//끝날떄까지 기다림.
		m_pPhysicsScene->simulate(0.01f);
		m_pPhysicsScene->fetchResults(true);		
	}
	
	SAFE_RELEASE(m_pSkybox);
	CGameObject::ErasePrototypeList(this);
	SAFE_RELEASE(m_pMainCamera);
	Safe_Release_Map(m_mapCamera);
	Safe_Release_VecList(m_vecLayer);
	if (m_pCollisionMatrix)
	{
		delete[] m_pCollisionMatrix;
		m_pCollisionMatrix = NULL;
	}

	if (m_pPhysicsScene)
		m_pPhysicsScene->release();

	if (m_pCudaContextManager)
		m_pCudaContextManager->release();

	if (m_pDispatcher)
		m_pDispatcher->release();
}

bool CScene::InitializePhysics()
{
	//PxCudaContextManagerDesc tCudaContextManagerDesc;
	//tCudaContextManagerDesc.interopMode = PxCudaInteropMode::NO_INTEROP;
	//m_pCudaContextManager = PxCreateCudaContextManager(*m_pFoundation, tCudaContextManagerDesc);	//Create the CUDA context manager, required for GRB to dispatch CUDA kernels.
	//if (m_pCudaContextManager)
	//{
	//	if (!m_pCudaContextManager->contextIsValid())
	//	{
	//		GET_SINGLE(CDebug)->Log(L"PhysX를 GPU에서 사용 할 수 없습니다. CPU에서 사용하도록 설정 됩니다.");
	//		m_pCudaContextManager->release();
	//		m_pCudaContextManager = NULL;
	//	}
	//}


	//Thread는 무조건 최대로 사용
	m_pDispatcher = PxDefaultCpuDispatcherCreate((PxU32)thread::hardware_concurrency());

	Vector3 vGravity = GET_SINGLE(CCollisionManager)->GetGravity();

	PxSceneDesc sceneDesc(GET_SINGLE(CCollisionManager)->GetPhysics()->getTolerancesScale());
	sceneDesc.gravity = PxVec3(vGravity.x, vGravity.y, vGravity.z);
	sceneDesc.cpuDispatcher = m_pDispatcher;
	sceneDesc.simulationEventCallback = GET_SINGLE(CCollisionManager); // 얘가 상속을 받았으니 인스턴스를 넘겨줌.
	sceneDesc.filterShader = FilterShader;
	//sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
	sceneDesc.flags |= PxSceneFlag::eENABLE_KINEMATIC_PAIRS;
	sceneDesc.flags |= PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS;
	sceneDesc.flags |= PxSceneFlag::eDEPRECATED_TRIGGER_TRIGGER_REPORTS;

	//if (m_pCudaContextManager)
	//{
	//	sceneDesc.gpuDispatcher = m_pCudaContextManager->getGpuDispatcher();
	//	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;
	//	sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
	//	sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	//	sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	//	sceneDesc.gpuMaxNumPartitions = 8;
	//}


	m_pPhysicsScene = GET_SINGLE(CCollisionManager)->GetPhysics()->createScene(sceneDesc);

	if (GET_SINGLE(CCollisionManager)->GetPVD())
	{

		if (GET_SINGLE(CCollisionManager)->GetPVD()->isConnected())
		{
			GET_SINGLE(CCollisionManager)->GetPVD()->disconnect();
			PxPvdTransport* pTransport = GET_SINGLE(CCollisionManager)->GetPVD()->getTransport();
			if (pTransport)
			{
				pTransport->release();
			}
		}


		char* strCallBackIP = "127.0.0.1";
		PxPvdTransport* pTransport = PxDefaultPvdSocketTransportCreate(strCallBackIP, 5425, 10);
		bool bPVDConnectionResult = GET_SINGLE(CCollisionManager)->GetPVD()->connect(*pTransport, PxPvdInstrumentationFlag::eALL);
		if (!bPVDConnectionResult)
		{
			pTransport->release();
			GET_SINGLE(CDebug)->Log(L"PVD 연결에 실패했습니다. PVD가 켜져있는지 확인해주세요.\n");
			GET_SINGLE(CDebug)->Log(L"PVD를 사용하지 않을경우 무시해도 됩니다.\n");
		}
		else
		{
			PxPvdSceneClient* pvdClient = m_pPhysicsScene->getScenePvdClient();
			if (pvdClient)
			{
				pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
				pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
				pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
			}
		}
	}
	else
	{
		GET_SINGLE(CDebug)->Log(L"PVD 연결에 실패했습니다. PVD가 켜져있는지 확인해주세요.\n");
		GET_SINGLE(CDebug)->Log(L"PVD를 사용하지 않을경우 무시해도 됩니다.\n");
	}
	//Test
	//PxMaterial* pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	//PxRigidStatic* groundPlane = PxCreatePlane(*m_pPhysics, PxPlane(0, 1, 0, 0),*pMaterial);
	//groundPlane->setName("Plane");
	//m_pPhysicsScene->addActor(*groundPlane);

	return true;
}

PxPhysics * CScene::GetPhysics() const
{
	return GET_SINGLE(CCollisionManager)->GetPhysics();
}

PxScene * CScene::GetPhysicsScene() const
{
	return m_pPhysicsScene;
}

PxFilterFlags CScene::FilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags & pairFlags, const void * constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	if (PxFilterObjectIsKinematic(attributes0) || PxFilterObjectIsKinematic(attributes1))
	{

	}

	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;
}

bool CScene::IsInitialized()
{
	return m_bInitialized;
}

vector<class CLayer*>* CScene::GetLayers()
{
	return &m_vecLayer;
}

bool CScene::IsCamera(CGameObject * pGameObject) const
{
	unordered_map<string, CGameObject*>::const_iterator iter;
	unordered_map<string, CGameObject*>::const_iterator iterEnd = m_mapCamera.end();

	for (iter = m_mapCamera.begin(); iter != iterEnd; ++iter)
	{
		if (pGameObject == iter->second)
		{
			return true;
		}
	}

	return false;	
}

void CScene::SetSkybox(const string& strKey, const wchar_t* pFileName, const string& strPathKey)
{
	if (m_pSkybox)
	{
		CMeshRenderer* pRenderer = m_pSkybox->GetComponent<CMeshRenderer>();
		CMaterial* pMaterial = pRenderer->GetMaterial();
		pMaterial->SetDiffuseTexture(strKey, pFileName,0, 0, strPathKey);
		SAFE_RELEASE(pMaterial);
		SAFE_RELEASE(pRenderer);
	}
	else
	{
		m_pSkybox = CGameObject::CreateObject("Skybox", NULL);		
		m_pSkybox->SetScene(this);

		CTransform* pTransform = m_pSkybox->GetTransform();
		pTransform->SetWorldScale(100000.0f, 100000.0f, 100000.0f);
		SAFE_RELEASE(pTransform);

		CMeshRenderer* pRenderer = m_pSkybox->AddComponent<CMeshRenderer>();
		pRenderer->SetMesh(MESH_SKYBOX, true);
		pRenderer->SetShader(SKYBOX_SHADER);
		pRenderer->SetRenderState(CULLFRONT);
		pRenderer->SetRenderState(DEPTH_SKYBOX);

		CMaterial* pMaterial = pRenderer->GetMaterial();
		pMaterial->SetDiffuseTexture(strKey, pFileName, 0, 0, strPathKey);
		pMaterial->SetDiffuseTextureRegister(10);
		pMaterial->SetSampler(SAMPLER_LINEAR);
		pMaterial->SetDiffuseColor(Vector4::white);
		SAFE_RELEASE(pMaterial);
		SAFE_RELEASE(pRenderer);
	}
}

CGameObject * CScene::GetSkybox() const
{
	if (m_pSkybox)
	{
		m_pSkybox->AddRef();
	}
	return m_pSkybox;
}


void CScene::ReserveCollisionMatrix(size_t iLayerCount)
{
	size_t iMatrixSize = iLayerCount * iLayerCount;
	bool* pMatrix = new bool[iMatrixSize];
	memset(pMatrix, 0, iMatrixSize);

	if (m_pCollisionMatrix)
	{
		delete[] m_pCollisionMatrix;
		m_pCollisionMatrix = NULL;
	}

	m_pCollisionMatrix = pMatrix;
}

void CScene::AddCollisionMatrix()
{
	size_t iMatrixSize = m_vecLayer.size();

	bool* pMatrix = new bool[iMatrixSize * iMatrixSize];

	memset(pMatrix, 0, iMatrixSize * iMatrixSize);

	if (m_pCollisionMatrix)
	{	
		delete[] m_pCollisionMatrix;
		m_pCollisionMatrix = NULL;
	}

	m_pCollisionMatrix = pMatrix;
}

void CScene::SetCollisionMatrix(CLayer * pLayer1, CLayer * pLayer2, bool bCollision)
{
	int iLayer1Index = 0;
	int iLayer2Index = 0;

	for (size_t i = 0; i < m_vecLayer.size(); ++i)
	{
		if (m_vecLayer[i] == pLayer1)
		{
			iLayer1Index = i;
		}
		if (m_vecLayer[i] == pLayer2)
		{
			iLayer2Index = i;
		}
	}
	m_pCollisionMatrix[iLayer1Index * m_vecLayer.size() + iLayer2Index] = bCollision;
	m_pCollisionMatrix[iLayer2Index * m_vecLayer.size() + iLayer1Index] = bCollision;
}

bool CScene::GetLayerCollision(CLayer * pLayer1, CLayer* pLayer2)
{
	int iLayer1Index = 0;
	int iLayer2Index = 0;
	for (size_t i = 0; i < m_vecLayer.size(); ++i)
	{
		if (m_vecLayer[i] == pLayer1)
		{
			iLayer1Index = i;
		}
		if (m_vecLayer[i] == pLayer2)
		{
			iLayer2Index = i;
		}
	}
	return m_pCollisionMatrix[iLayer1Index * m_vecLayer.size() + iLayer2Index];
}

CLayer * CScene::CreateLayer(const string & strTag, int iZOrder)
{
	CLayer*	pLayer = new CLayer;

	pLayer->SetTag(strTag);
	pLayer->SetZOrder(iZOrder);
	pLayer->SetScene(this);

	if (!pLayer->Initialize())
	{
		SAFE_RELEASE(pLayer);
		return NULL;
	}

	pLayer->AddRef();
	m_vecLayer.push_back(pLayer);
	AddCollisionMatrix();

	if (m_vecLayer.size() >= 2)
	{
		sort(m_vecLayer.begin(), m_vecLayer.end(), CScene::LayerSort);
	}
	return pLayer;
}

CLayer * CScene::FindLayer(unsigned int iIndex) const
{
	m_vecLayer[iIndex]->AddRef();
	return m_vecLayer[iIndex];
}

CLayer * CScene::FindLayer(const string & strTag)
{
	for (size_t i = 0; i < m_vecLayer.size(); ++i)
	{
		if (m_vecLayer[i]->GetTag() == strTag)
		{
			m_vecLayer[i]->AddRef();
			return m_vecLayer[i];
		}
	}

	return NULL;
}

size_t CScene::GetLayerCount() const
{
	return m_vecLayer.size();
}

bool CScene::ChangeLayerZOrder(const string & strTag, int iZOrder)
{
	CLayer*	pLayer = FindLayer(strTag);

	if (!pLayer)
		return false;

	pLayer->SetZOrder(iZOrder);

	SAFE_RELEASE(pLayer);

	if (m_vecLayer.size() >= 2)
		sort(m_vecLayer.begin(), m_vecLayer.end(), CScene::LayerSort);

	return true;
}

CGameObject * CScene::CreateCamera(const string & strKey, const Vector3 & vPos, const Vector3 & vRot)
{
	CGameObject*	pCameraObject = FindCameraObject(strKey);

	if (pCameraObject)
		return pCameraObject;

	pCameraObject = CGameObject::CreateObject(strKey);
	pCameraObject->SetScene(this);

	CTransform*	pTransform = pCameraObject->GetTransform();
	
	pTransform->SetWorldPosition(vPos);
	pTransform->SetWorldRotation(vRot);
	
	SAFE_RELEASE(pTransform);

	CCamera*	pCamera = pCameraObject->AddComponent<CCamera>(strKey);

	SAFE_RELEASE(pCamera);

	pCameraObject->AddRef();

	m_mapCamera.insert(make_pair(strKey, pCameraObject));

	return pCameraObject;
}

CGameObject * CScene::FindCameraObject(const string & strKey)
{
	unordered_map<string, class CGameObject*>::iterator	iter = m_mapCamera.find(strKey);

	if (iter == m_mapCamera.end())
		return NULL;

	iter->second->AddRef();

	return iter->second;
}

CCamera * CScene::FindCamera(const string & strKey)
{
	CGameObject*	pCameraObj = FindCameraObject(strKey);

	if (!pCameraObj)
		return NULL;

	CCamera*	pCamera = pCameraObj->GetComponent<CCamera>();

	SAFE_RELEASE(pCameraObj);

	return pCamera;
}

CTransform * CScene::FindCameraTransform(const string & strKey)
{
	CGameObject*	pCameraObject = FindCameraObject(strKey);

	if (!pCameraObject)
		return NULL;

	CTransform*	pTransform = pCameraObject->GetTransform();
	
	SAFE_RELEASE(pCameraObject);

	return pTransform;
	return NULL;
}

CGameObject * CScene::GetMainCameraObject()
{
	m_pMainCamera->AddRef();
	return m_pMainCamera;
}

CCamera * CScene::GetMainCamera()
{
	return m_pMainCamera->GetComponent<CCamera>();
}

CTransform * CScene::GetMainCameraTransform()
{
	//return NULL;
	CTransform* pTransform = m_pMainCamera->GetTransform();
	return pTransform;
}

size_t CScene::GetColliderCount() const
{
	return (size_t)m_pPhysicsScene->getNbActors(
		PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC |
		PxActorTypeFlag::eCLOTH | PxActorTypeFlag::ePARTICLE_SYSTEM |
		PxActorTypeFlag::ePARTICLE_FLUID);
}


const list<class CUICollider*>* CScene::GetButtonColliderList() const
{
	return &m_UIColliderList;
}


void CScene::RemoveButtonCollider(CUICollider * pCollider)
{
	list<CUICollider*>::iterator iter;
	list<CUICollider*>::iterator iterEnd = m_UIColliderList.end();

	for (iter = m_UIColliderList.begin(); iter != iterEnd; ++iter)
	{
		if (*iter == pCollider)
		{
			m_UIColliderList.erase(iter);
			return;
		}
	}
}

void CScene::AddUICollider(CUICollider * pCollider)
{
	m_UIColliderList.push_back(pCollider);
}

bool CScene::Save(FILE * pFile)
{
	size_t iLayerCount = m_vecLayer.size();

	fwrite(&iLayerCount, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iLayerCount; ++i)
	{
		string strLayerName = m_vecLayer[i]->GetTag();
		size_t iLayerNameLength = strLayerName.length();
		int iLayerZOrder = m_vecLayer[i]->GetZOrder();

		char pBuffer[256] = {};
		strcpy_s(pBuffer, strLayerName.c_str());
		fwrite(&iLayerNameLength, sizeof(size_t), 1, pFile);
		fwrite(&pBuffer, sizeof(char), iLayerNameLength, pFile);
		fwrite(&iLayerZOrder, sizeof(int), 1, pFile);
	}

	

	//최상위 부모가 되는 오브젝트의 갯수를 파악한다.
	size_t iRootObjectCount = 0;
	for (size_t i = 0; i < iLayerCount; ++i)
	{
		list<CGameObject*>::const_iterator iter = m_vecLayer[i]->GetAllGameObjects()->begin();
		list<CGameObject*>::const_iterator iterEnd = m_vecLayer[i]->GetAllGameObjects()->end();

		for (; iter != iterEnd; ++iter)
		{
			CTransform* pTransform = (*iter)->GetTransform();
			CTransform* pParentTransform = pTransform->GetParent();			
			SAFE_RELEASE(pTransform);			

			bool bHasGizmo = (*iter)->HasComponent<CGizmo>();
			if (!pParentTransform && !bHasGizmo)
			{
				++iRootObjectCount;
			}
		}
	}

	fwrite(&iRootObjectCount, sizeof(size_t), 1, pFile);
	//최상위 오브젝트만 저장한다.
	//그러므는 그 오브젝트에 딸린 자식 오브젝트는 내부적으로 돌면서 저장한다.
	for (size_t i = 0; i < iLayerCount; ++i)
	{
		list<CGameObject*>::const_iterator iter = m_vecLayer[i]->GetAllGameObjects()->begin();
		list<CGameObject*>::const_iterator iterEnd = m_vecLayer[i]->GetAllGameObjects()->end();
		for (; iter != iterEnd; ++iter)
		{
			CTransform* pTransform = (*iter)->GetTransform();
			CTransform* pParentTransform = pTransform->GetParent();
			SAFE_RELEASE(pTransform);

			bool bHasGizmo = (*iter)->HasComponent<CGizmo>();
			if (!pParentTransform && !bHasGizmo)
			{
				if (!(*iter)->Save(pFile))
				{
					return false;
				}
			}
		}
	}

	size_t iCameraObjectCount = m_mapCamera.size();
	fwrite(&iCameraObjectCount, sizeof(size_t), 1, pFile);
	
	unordered_map<string, CGameObject*>::iterator iter;
	unordered_map<string, CGameObject*>::iterator iterEnd = m_mapCamera.end();
	for (iter = m_mapCamera.begin(); iter != iterEnd; ++iter)
	{
		bool bIsMainCamera = false;
		if (iter->second == m_pMainCamera)
		{
			bIsMainCamera = true;
		}
	
		fwrite(&bIsMainCamera, sizeof(bool), 1, pFile);
		
		if (!iter->second->Save(pFile))
		{
			return false;
		}	
	}
	
	
	if (!m_pSkybox->Save(pFile))
	{
		return false;
	}

	fwrite(&m_pCollisionMatrix[0], sizeof(bool), iLayerCount * iLayerCount, pFile);

	return true;
}

bool CScene::Load(FILE * pFile)
{	
	size_t iLayerCount = 0;

	fread(&iLayerCount, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iLayerCount; ++i)
	{
		char strLayerName[256] = {};
		size_t iLayerNameLength = 0;
		int iLayerZOrder = 0;

		fread(&iLayerNameLength, sizeof(size_t), 1, pFile);
		fread(&strLayerName[0], sizeof(char), iLayerNameLength, pFile);
		fread(&iLayerZOrder, sizeof(int), 1, pFile);

		if (strcmp(strLayerName, "Default") == 0 || strcmp(strLayerName, "UI") == 0)
		{
			CLayer* pLayer = FindLayer(strLayerName);
			pLayer->SetZOrder(iLayerZOrder);
			SAFE_RELEASE(pLayer);
		}
		else
		{
			CLayer* pLayer = CreateLayer(strLayerName, iLayerZOrder);
			SAFE_RELEASE(pLayer);
		}
	}
	
	size_t iRootObjectCount = 0;
	fread(&iRootObjectCount, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iRootObjectCount; ++i)
	{
		CGameObject* pObj = CGameObject::Load(this, pFile);
		SAFE_RELEASE(pObj);
	}

	size_t iCameraObjectCount = m_mapCamera.size();
	fread(&iCameraObjectCount, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iCameraObjectCount; ++i)
	{
		bool bIsMainCamera = false;
		fread(&bIsMainCamera, sizeof(bool), 1, pFile);
	
		if (bIsMainCamera)
		{
			list<CComponent*>::const_iterator iter;
			list<CComponent*>::const_iterator iterEnd = m_pMainCamera->GetAllComponents().end();
	
			for (iter = m_pMainCamera->GetAllComponents().begin(); iter != iterEnd; )
			{
				CComponent* pComponent = *iter;
				SAFE_RELEASE(pComponent);
				iter = m_pMainCamera->m_ComponentList.erase(iter);
			}
	
			CGameObject::Load(this, pFile, m_pMainCamera);
			m_pMainCamera->Release();

			CLayer* pLayer = m_pMainCamera->GetLayer();
			pLayer->RemoveGameObject(m_pMainCamera);
			SAFE_RELEASE(pLayer);
			
		}
		else
		{
			CGameObject* pCamera = CreateCamera("Temp", Vector3::Zero, Vector3::Zero);
	
			list<CComponent*>::const_iterator iter;
			list<CComponent*>::const_iterator iterEnd = pCamera->GetAllComponents().end();
	
			for (iter = pCamera->GetAllComponents().begin(); iter != iterEnd; )
			{
				CComponent* pComponent = *iter;
				SAFE_RELEASE(pComponent);
				iter = pCamera->m_ComponentList.erase(iter);
			}
	
			CGameObject::Load(this, pFile, pCamera);
	
			CLayer* pLayer = pCamera->GetLayer();
			pLayer->RemoveGameObject(pCamera);
			SAFE_RELEASE(pLayer);
	
			SAFE_RELEASE(pCamera);
		}
	}

	

	list<CComponent*>::const_iterator iter;
	list<CComponent*>::const_iterator iterEnd = m_pSkybox->GetAllComponents().end();
	
	for (iter = m_pSkybox->GetAllComponents().begin(); iter != iterEnd; )
	{
		CComponent* pComponent = *iter;
		SAFE_RELEASE(pComponent);
		iter = m_pSkybox->m_ComponentList.erase(iter);
	}
	
	CGameObject* pSkybox = CGameObject::Load(this, pFile, m_pSkybox);
		
	
	ReserveCollisionMatrix(iLayerCount);
	fread(&m_pCollisionMatrix[0], sizeof(bool), iLayerCount * iLayerCount, pFile);
	
	return true;
}

bool CScene::LayerSort(CLayer * p1, CLayer * p2)
{
	return p1->GetZOrder() < p2->GetZOrder();
}

bool CScene::Initialize()
{
	return true;
}
void CScene::Start()
{
	{
		vector<CLayer*>::iterator	iter;
		vector<CLayer*>::iterator	iterEnd = m_vecLayer.end();

		for (iter = m_vecLayer.begin(); iter != iterEnd;)
		{
			if ((*iter)->IsDestroy())
			{
				SAFE_RELEASE((*iter));
				iter = m_vecLayer.erase(iter);
				iterEnd = m_vecLayer.end();
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

	m_pMainCamera->Start();
}


int CScene::Update(float fTime)
{
	if (m_pSkybox)
	{
		m_pSkybox->Update(fTime);
	}


	{
		vector<CLayer*>::iterator	iter;
		vector<CLayer*>::iterator	iterEnd = m_vecLayer.end();

		for (iter = m_vecLayer.begin(); iter != iterEnd;)
		{
			if ((*iter)->IsDestroy())
			{
				SAFE_RELEASE((*iter));
				iter = m_vecLayer.erase(iter);
				iterEnd = m_vecLayer.end();
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

	m_pMainCamera->Update(fTime);

	return 0;
}

int CScene::LateUpdate(float fTime)
{
	if (m_pSkybox)
	{
		m_pSkybox->LateUpdate(fTime);
	}

	{
		vector<CLayer*>::iterator	iter;
		vector<CLayer*>::iterator	iterEnd = m_vecLayer.end();

		for (iter = m_vecLayer.begin(); iter != iterEnd;)
		{
			if ((*iter)->IsDestroy())
			{
				SAFE_RELEASE((*iter));
				iter = m_vecLayer.erase(iter);
				iterEnd = m_vecLayer.end();
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

	m_pMainCamera->LateUpdate(fTime);

	return 0;
}

void CScene::Render(float fTime)
{
	m_pMainCamera->Render(fTime);	

	{
		vector<CLayer*>::iterator	iter;
		vector<CLayer*>::iterator	iterEnd = m_vecLayer.end();

		for (iter = m_vecLayer.begin(); iter != iterEnd;)
		{
			if ((*iter)->IsDestroy())
			{
				SAFE_RELEASE((*iter));
				iter = m_vecLayer.erase(iter);
				iterEnd = m_vecLayer.end();
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
}

void CScene::RenderDebug(float fTime)
{
		vector<CLayer*>::iterator	iter;
		vector<CLayer*>::iterator	iterEnd = m_vecLayer.end();

		for (iter = m_vecLayer.begin(); iter != iterEnd;)
		{
			if ((*iter)->IsDestroy())
			{
				SAFE_RELEASE((*iter));
				iter = m_vecLayer.erase(iter);
				iterEnd = m_vecLayer.end();
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
