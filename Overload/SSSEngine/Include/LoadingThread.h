#pragma once
#include "Thread.h"
SSS_BEGIN
class SSS_DLL CLoadingThread :
	public CThread
{
	friend class CSceneManager;

public:
	CLoadingThread();
	~CLoadingThread();

private:
	wstring m_strFilePath;
	string m_strPathKey;
	class CScene* m_pScene;
	bool m_bOnLoading;

public:
	class CScene* GetScene();
	string GetPathKey() const;
	wstring GetPath() const;
	void SetScene(class CScene* pScene);

public:
	void SetInfo(const wstring& strPath, const string& strPathKey);

public:
	void SetFunction();
	static void Loading();
};

SSS_END