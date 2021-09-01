#pragma once
#include "Ref.h"

SSS_BEGIN

class SSS_DLL CLayer :
	public CRef
{
private:
	friend class CSceneManager;
	friend class CScene;

private:
	CLayer();
	~CLayer();


private:
	list<class CGameObject*>	m_GameObjectList;
	class CScene*		m_pScene;
	int	m_iZOrder;

private:
	void SetScene(class CScene* pScene);
	void SetZOrder(int iZOrder);

public:
	int GetZOrder()	const;
	void RemoveGameObject(class CGameObject* pGameObject);
	void AddGameObject(class CGameObject* pGameObject);
	list<class CGameObject*>* GetAllGameObjects();
	CGameObject* FindGameObjectWithTag(const string& strTag);

	bool Save(FILE* pFile);
	bool Load(FILE* pFile);
public:
	bool Initialize();
	int Start();
	int Update(float fTime);
	int LateUpdate(float fTime);
	int Collision(float fTime);
	void Render(float fTime);

	void RenderDebug(float fTime);

};

SSS_END
