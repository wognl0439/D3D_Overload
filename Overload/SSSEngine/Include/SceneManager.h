#pragma once
#include "Engine.h"
#include "Scene.h"

SSS_BEGIN

enum SCENE_CHANGE_CONDITION
{
	SCC_TO_PREVIOUS,
	SCC_TO_NEXT
};

enum SCENE_STATE
{
	SS_PLAY,
	SS_PAUSE,
	SS_STOP
};

class SSS_DLL CSceneManager
{
	DECLARE_SINGLE(CSceneManager)
	
private:
	friend class CCore;
	friend class CLoadingThread;

private:
	//class CScene*	m_pPrevScene;
	class CScene*	m_pCurrentScene;
	class CScene*	m_pNextScene;

	SCENE_STATE	m_eState;
	FILE*				m_pCurrentRunningLoader;
	int						m_iFileSize;
	
private:
	void ChangeScene();
	void GarbageCollection(class CScene** pScene);

public:
	void ReleaseAllScene();
	SCENE_STATE GetSceneState() const;
	void Play();
	void Pause();
	void Stop(bool bOnInitialSetting = false);
	bool OnLoading();
	float LoadingProgress();

	
	//void ClearPrevScene();
	void ClearNextScene();
	void SetCurrentScene(CScene* pScene);
	class CScene* GetNextScene() const;
	//class CScene* GetPrevScene() const;
	void SetNextScene(class CScene* pNextScene);
	class CScene* GetCurrentScene() const;
	void ChangeScene(class CScene* pNextScene);
	void ChangeScene(SCENE_CHANGE_CONDITION eCondition);

public:
	bool Initialize();
	int Update(float fTime);
	int LateUpdate(float fTime);
	void Render(float fTime);


private:
	CScene* LoadSceneAsynchronously(const wchar_t* pFileName, const string& strPathKey = DATA_PATH);
public:
	//파일에서 Scene을 읽어 온다.
	//만약 strPathKey를 NO_USE_PATH 를 줄 경우, FileName을 FullPath로 사용하도록 한다.
	void CreateSceneAsynchronously(const wchar_t* pFileName, const string& strPathKey = DATA_PATH);
	class CScene* CreateScene(const wchar_t* pFileName, const string& strPathKey = DATA_PATH);
	class CScene* CreateScene();
	
	//Editor Only
	bool SaveScene(CScene* pScene, const wchar_t* pFileName, const string& strPathKey = DATA_PATH);


	template<typename T>
	T* CreateScene(const string& strTag)
	{
		T* pScene = new T;

		if (!pScene->Initialize())
		{
			SAFE_RELEASE(pScene);
			return NULL;
		}
		((CScene*)pScene)->AddRef();
		return pScene;
	}

};

SSS_END

