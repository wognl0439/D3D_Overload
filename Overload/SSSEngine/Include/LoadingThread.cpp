#include "LoadingThread.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ThreadManager.h"
#include "Debug.h"

SSS_USING

CLoadingThread::CLoadingThread():
	m_pScene(NULL),
	m_bOnLoading(false)
{
}

CLoadingThread::~CLoadingThread()
{
}

CScene * CLoadingThread::GetScene()
{
	return m_pScene;
}

string CLoadingThread::GetPathKey() const
{
	return m_strPathKey;
}

wstring CLoadingThread::GetPath() const
{
	return m_strFilePath;
}

void CLoadingThread::SetScene(CScene * pScene)
{
	m_pScene = pScene;
}

void CLoadingThread::SetInfo(const wstring & strPath, const string & strPathKey)
{
	m_strFilePath = strPath;
	m_strPathKey = strPathKey;
}

void CLoadingThread::SetFunction()
{
	m_pFunction = bind(&Loading);
}

void CLoadingThread::Loading()
{
	CThread* pThread = GET_SINGLE(CThreadManager)->FindThread("Loading");

	if (pThread)
	{
		wstring strPath = ((CLoadingThread*)pThread)->GetPath();
		string strPathKey = ((CLoadingThread*)pThread)->GetPathKey();
		if (!strPath.empty())
		{
			CScene* pScene = GET_SINGLE(CSceneManager)->LoadSceneAsynchronously(strPath.c_str(), strPathKey);
			((CLoadingThread*)pThread) ->SetScene(pScene);
		}
	}
}


//class CScene* CLoadingThread::GetScene()
//{
//	return m_pScene;
//}
//
//void CLoadingThread::LoadScene(const wstring & strPath, const string & strPathKey)
//{
//	//m_strFilePath = strPath;
//	//m_strPathKey = strPathKey;
//	//m_bOnLoading = true;
//}

//void CLoadingThread::Run()
//{	
	//while (!m_bDestroy)
	//{
	//	//if (m_bOnLoading == TRUE)
	//	//{
	//	//	//m_pScene = GET_SINGLE(CSceneManager)->LoadSceneAsynchronously(m_strFilePath.c_str(), m_strPathKey);
	//	//	m_bOnLoading = false;
	//	//	break;
	//	//}		
	//}
//}
