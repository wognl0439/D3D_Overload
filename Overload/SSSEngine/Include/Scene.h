#pragma once
#include "Ref.h"
#include "PxPhysicsAPI.h"
using namespace physx;

SSS_BEGIN

class SSS_DLL CScene :
	public CRef
{
private:
	friend class CSceneManager;
	friend class CCollisionManager;

public:
	CScene();
	virtual ~CScene();

private:
	PxScene*							m_pPhysicsScene;
	PxDefaultCpuDispatcher*		m_pDispatcher;
	PxGpuDispatcher*				m_pGPUDispatcher;
	PxCudaContextManager*		m_pCudaContextManager;


public:
	bool InitializePhysics();
	PxPhysics* GetPhysics() const;
	PxScene* GetPhysicsScene() const;

	static PxFilterFlags FilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);
	


protected:
	vector<class CLayer*>										m_vecLayer;
	unordered_map<string, class CGameObject*>		m_mapCamera;
	class CGameObject*											m_pMainCamera;
	class CGameObject*										m_pSkybox;
	list<class CUICollider*> m_UIColliderList;
	bool m_bInitialized;
	bool* m_pCollisionMatrix;

public:
	bool IsInitialized();
	vector<class CLayer*>* GetLayers();
	bool IsCamera(class CGameObject* pGameObject) const;
	void SetSkybox(const string& strKey, const wchar_t* pFileName, const string& strPathKey = TEXTURE_PATH);
	class CGameObject* GetSkybox() const;
	
	void ReserveCollisionMatrix(size_t iLayerCount);
	void AddCollisionMatrix();
	void SetCollisionMatrix(class CLayer* pLayer1, class CLayer* pLayer2, bool bCollision);
	bool GetLayerCollision(CLayer * pLayer1, CLayer* pLayer2);
	CLayer* CreateLayer(const string& strTag, int iZOrder = 0);
	CLayer* FindLayer(unsigned int iIndex) const ;
	CLayer* FindLayer(const string& strTag);
	size_t GetLayerCount() const;
	bool ChangeLayerZOrder(const string& strTag, int iZOrder);
	class CGameObject* CreateCamera(const string& strKey, const Vector3& vPos, const Vector3& vRot = Vector3::Zero);
	class CGameObject* FindCameraObject(const string& strKey);
	class CCamera* FindCamera(const string& strKey);
	class CTransform* FindCameraTransform(const string& strKey);
	class CGameObject* GetMainCameraObject();
	class CCamera* GetMainCamera();
	class CTransform* GetMainCameraTransform();
	size_t GetColliderCount() const;
	const list<class CUICollider*>* GetButtonColliderList() const;

	void RemoveButtonCollider(class CUICollider* pCollider);

	void AddUICollider(class CUICollider* pCollider);

	bool Save(FILE* pFile);
	bool Load(FILE* pFile);
private:
	static bool LayerSort(class CLayer* p1, class CLayer* p2);

public:
	virtual bool Initialize();
	virtual void Start();
	int Update(float fTime);
	int LateUpdate(float fTime);
	void Render(float fTime);

	void RenderDebug(float fTime);


};

SSS_END
