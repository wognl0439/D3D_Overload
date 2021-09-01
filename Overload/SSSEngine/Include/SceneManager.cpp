#include "SceneManager.h"
#include "Scene.h"
#include "PathManager.h"
#include "CollisionManager.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Layer.h"
#include "ThreadManager.h"
#include "Thread.h"
#include "LoadingThread.h"

SSS_USING

DEFINITION_SINGLE(CSceneManager)

CSceneManager::CSceneManager() :
	m_pCurrentScene(NULL),
	m_pNextScene(NULL),
	//m_pPrevScene(NULL),
	m_pCurrentRunningLoader(NULL),
	m_iFileSize(-1)
{
#ifdef CLIENT
	m_eState = SS_PLAY;	
#else
	#ifdef EDITOR
		m_eState = SS_STOP;
	#endif // EDITOR
#endif  // CLIENT
	
}

CSceneManager::~CSceneManager()
{
	GarbageCollection(&m_pCurrentScene);
	GarbageCollection(&m_pNextScene);
	//GarbageCollection(&m_pPrevScene);
	//SAFE_RELEASE(m_pCurrentScene);
	//SAFE_RELEASE(m_pNextScene);
	//SAFE_RELEASE(m_pPrevScene);	
}

//void CSceneManager::ClearPrevScene()
//{
//	//	SAFE_RELEASE(m_pPrevScene);
//}

void CSceneManager::ClearNextScene()
{
	SAFE_RELEASE(m_pNextScene);
}

void CSceneManager::SetCurrentScene(CScene * pScene)
{
	SAFE_RELEASE(m_pCurrentScene);
	GET_SINGLE(CCollisionManager)->Reset();
	m_pCurrentScene = pScene;
}

CScene * CSceneManager::GetNextScene() const
{
	if(m_pNextScene)
		m_pNextScene->AddRef();

	return m_pNextScene;
}
//CScene * CSceneManager::GetPrevScene() const
//{
//	if (m_pPrevScene)
//		m_pPrevScene->AddRef();
//
//	return m_pPrevScene;
//}

void CSceneManager::SetNextScene(CScene * pNextScene)
{
	SAFE_RELEASE(m_pNextScene);
	m_pNextScene = pNextScene;
}

CScene * CSceneManager::GetCurrentScene() const
{
	m_pCurrentScene->AddRef();
	return m_pCurrentScene;
}

void CSceneManager::ChangeScene() 
{
	if (m_pNextScene)
	{
		//바뀌는 Scene을 Prev에 넣어서 보관하도록 한다.
		//SAFE_RELEASE(m_pPrevScene);
		//m_pPrevScene = m_pCurrentScene;
		GarbageCollection(&m_pCurrentScene);
		m_pCurrentScene = m_pNextScene;
		m_pCurrentScene->AddRef();
		SAFE_RELEASE(m_pNextScene);
	}
}

void CSceneManager::GarbageCollection(CScene** pScene)
{
	if (!(*pScene))
		return;

	//모든 레이어의 모든 오브젝트의 모든 컴포넌트부터 해제
	vector<CLayer*>::iterator iterLayer = (*pScene)->m_vecLayer.begin();
	vector<CLayer*>::iterator iterLayerEnd = (*pScene)->m_vecLayer.end();
	for (; iterLayer != iterLayerEnd; )
	{
		list<CGameObject*>::iterator iterGameObject = (*iterLayer)->m_GameObjectList.begin();
		list<CGameObject*>::iterator iterGameObjectEnd = (*iterLayer)->m_GameObjectList.end();
		for (; iterGameObject != iterGameObjectEnd;)
		{
			list<CComponent*>::iterator iterComponent = (*iterGameObject)->m_ComponentList.begin();
			list<CComponent*>::iterator iterComponentEnd = (*iterGameObject)->m_ComponentList.end();
			for (; iterComponent != iterComponentEnd;)
			{
				int iRefCount = (*iterComponent)->m_iRefCount;
				
				for(int i = 0 ; i < iRefCount; ++i)
				{
					(*iterComponent)->Release();
				}

				iterComponent = (*iterGameObject)->m_ComponentList.erase(iterComponent);
				iterComponentEnd = (*iterGameObject)->m_ComponentList.end();
			}
		
			//위에서 이 오브젝트의 모든 컴포넌트 해제했으니까
			//인제 오브젝트의 트랜스폼 해제
			{
				int iRefCount = (*iterGameObject)->m_pTransform->m_iRefCount;
				for (int i = 0; i < iRefCount; ++i)
				{
					(*iterGameObject)->m_pTransform->Release();
				}
				(*iterGameObject)->m_pTransform = NULL;
			}
			//인제 오브젝트의 레퍼런스  해제
			{
				int iRefCount = (*iterGameObject)->m_iRefCount;
				for (int i = 0; i < iRefCount; ++i)
				{
					(*iterGameObject)->Release();
				}
			}
			//레이어에서 제거
			iterGameObject = (*iterLayer)->m_GameObjectList.erase(iterGameObject);
			iterGameObjectEnd = (*iterLayer)->m_GameObjectList.end();
		}
	
		

		//레이어의 모든 오브젝트 제거했으므로
		//레이어 날려줌
		int iLayerRefCount = (*iterLayer)->m_iRefCount;
		for (int i = 0 ; i < iLayerRefCount; ++i)
		{
			(*iterLayer)->Release();
		}

		iterLayer = (*pScene)->m_vecLayer.erase(iterLayer);
		iterLayerEnd = (*pScene)->m_vecLayer.end();
	}

	//Scene 날려줌

	int iSceneRefCount = (*pScene)->m_iRefCount;
	for (int i = 0; i < iSceneRefCount; ++i)
	{
		(*pScene)->Release();
	}

	(*pScene) = NULL;
}

void CSceneManager::ReleaseAllScene()
{
	//중간에 터지면 100% 언더플로우 혹은 댕글링

	//if (m_pPrevScene)
	//{
	//	GarbageCollection(&m_pPrevScene);
	//	m_pPrevScene = NULL;
	//}

	if (m_pCurrentScene)
	{
		GarbageCollection(&m_pCurrentScene);
		m_pCurrentScene = NULL;
	}

	if (m_pNextScene)
	{
		GarbageCollection(&m_pNextScene);
		m_pNextScene = NULL;
	}
}

SCENE_STATE CSceneManager::GetSceneState() const
{
	return m_eState;
}

void CSceneManager::Play()
{
	if (m_eState == SS_STOP)
	{
		string strTempFileName = GET_SINGLE(CPathManager)->FindPathMultiByte(DATA_PATH);
		if (!strTempFileName.empty())
		{
			strTempFileName += "TempScene.scene";
		}
		else
		{
			//경로가 명확하지 않음
			assert(false);
		}

		//파일 제거
		remove(strTempFileName.c_str());

		m_eState = SS_PLAY;
		SaveScene(m_pCurrentScene, L"TempScene.scene", DATA_PATH);
		m_pCurrentScene->m_bInitialized = false;
	}
	else if (m_eState == SS_PAUSE)
	{
		m_eState = SS_PLAY;
	}
}

void CSceneManager::Pause()
{
	if (m_eState == SS_PAUSE)
	{
		m_eState = SS_PLAY;
	}
	else
	{
		m_eState = SS_PAUSE;
	}
}

void CSceneManager::Stop(bool bOnInitialSetting)
{

	if (bOnInitialSetting)
	{
		m_eState = SS_STOP;
	}
	else
	{
		if (m_eState != SS_STOP)
		{
			m_eState = SS_STOP;

			GET_SINGLE(CThreadManager)->ClearThread();
			CScene* pTempScene = CreateScene(L"TempScene.scene", DATA_PATH);
			if (pTempScene)
			{
				ReleaseAllScene();
				SetCurrentScene(pTempScene);
				SAFE_RELEASE(pTempScene);
			}
		}
	}
}

bool CSceneManager::OnLoading()
{
	if (m_pCurrentRunningLoader != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float CSceneManager::LoadingProgress()
{
	if (m_pCurrentRunningLoader && m_iFileSize != -1)
	{
		int iCurrentPosition = (int)ftell(m_pCurrentRunningLoader);		
		return (float)iCurrentPosition / (float)m_iFileSize;
	}
	else
	{
		return 0.0f;
	}
}

void CSceneManager::ChangeScene(CScene * pNextScene)
{
	SAFE_RELEASE(m_pNextScene);
	m_pNextScene = pNextScene;
}

//미리 Prev 또는 Next Scene이 설정이 되어 있어야 유효하다.
//Current -> Prev 시 Prev 가 Next로 이전되고, 기존의 Next는 해제.
//
void CSceneManager::ChangeScene(SCENE_CHANGE_CONDITION eCondition)
{
	//if (eCondition == SCC_TO_PREVIOUS)
	//{
	//	//if (!m_pPrevScene)
	//	//	return;
	//
	//	
	//	//Next에 Prev를 넣어주고, Scene을 전환할 것이기 때문에 이미 Next에 Scene이 있다면 해제해준다.
	//	if (m_pNextScene)
	//	{
	//		SAFE_RELEASE(m_pNextScene);
	//	}
	//	else
	//	{
	//		// ChangeScene할 때 Release하기때문에 Ref추가함
	//		m_pPrevScene->AddRef();
	//		m_pNextScene = m_pPrevScene;
	//	}		
	//}
	if (eCondition == SCC_TO_NEXT)
	{
		if (!m_pNextScene)
		{
			return;
		}
	}
}

bool CSceneManager::Initialize()
{
	return true;
}

int CSceneManager::Update(float fTime)
{
	m_pCurrentScene->Update(fTime);

	if (!m_pCurrentScene->m_bInitialized)
	{
		m_pCurrentScene->m_bInitialized = true;
	}
	return 0;
}

int CSceneManager::LateUpdate(float fTime)
{
	return m_pCurrentScene->LateUpdate(fTime);
}


void CSceneManager::Render(float fTime)
{
	m_pCurrentScene->Render(fTime);
}

CScene* CSceneManager::LoadSceneAsynchronously(const wchar_t * pFileName, const string & strPathKey)
{
	wstring strFullPath;
	if (strPathKey != NO_USE_PATH)
	{
		strFullPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);		
	}

	strFullPath += pFileName;

	string strFullPathA(strFullPath.begin(), strFullPath.end());

	FILE* pFile = NULL;

	fopen_s(&pFile, strFullPathA.c_str(), "rb");

	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);
		m_iFileSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		m_pCurrentRunningLoader = pFile;

		CScene* pScene = new CScene;
		if (pScene->Load(pFile))
		{
			m_iFileSize = -1;
			m_pCurrentRunningLoader = NULL;
			fclose(pFile);
			pScene->AddRef();
			return pScene;
		}
		else
		{
			m_iFileSize = -1;
			m_pCurrentRunningLoader = NULL;
			fclose(pFile);
			return NULL;
		}
	}
	else
	{
		m_iFileSize = -1;
		m_pCurrentRunningLoader = NULL;
		return NULL;
	}
}

CScene * CSceneManager::CreateScene()
{
	CScene* pScene = new CScene;

	if (!pScene->Initialize())
	{
		SAFE_RELEASE(pScene);
		return NULL;
	}

	pScene->AddRef();
	return pScene;
}


void CSceneManager::CreateSceneAsynchronously(const wchar_t * pFileName, const string & strPathKey)
{
	CLoadingThread* pLoadingThread = new CLoadingThread;
	pLoadingThread->Create("Loading");
	pLoadingThread->SetInfo(pFileName, strPathKey);
	pLoadingThread->SetFunction();
	pLoadingThread->Start();
}

CScene * CSceneManager::CreateScene(const wchar_t * pFileName, const string & strPathKey)
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
	


	if(pFile)
	{
		fseek(pFile, 0, SEEK_END);
		m_iFileSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		m_pCurrentRunningLoader = pFile;

		CScene* pScene = new CScene;
		
		if (pScene->Load(pFile))
		{
			m_iFileSize = -1;
			m_pCurrentRunningLoader = NULL;
			fclose(pFile);
			pScene->AddRef();
			return pScene;
		}
		else
		{
			m_iFileSize = -1;
			m_pCurrentRunningLoader = NULL;
			fclose(pFile);
			return NULL;
		}
	}
	else
	{
		m_iFileSize = -1;
		m_pCurrentRunningLoader = NULL;
		return NULL;
	}
}

bool CSceneManager::SaveScene(CScene* pScene, const wchar_t * pFileName, const string & strPathKey)
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

	fopen_s(&pFile, strFullPathA.c_str(), "wb");


	if (pFile)
	{
		if (pScene->Save(pFile))
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