#pragma once
#include "Ref.h"

SSS_BEGIN

class SSS_DLL CGameObject :
	public CRef
{
private:
	friend class CSceneManager;
	friend class CScene;
	friend class CCollider;
	friend class CCollisionManager;

private:
	CGameObject();
	CGameObject(const CGameObject& obj);
	virtual ~CGameObject();
	
public:
	static CGameObject* CreateObject(const string& strTag, class CLayer* pLayer = NULL);

private:
	class CScene*		m_pScene;
	class CLayer*		m_pLayer;
	class CTransform*	m_pTransform;
	list<class CComponent*> m_ComponentList;
	list<class CCollider*> m_ColliderList;
	list<class CComponent*> m_FindComponentList;

#pragma region For Deferred
private:
	bool			m_bUseTransparency;
	
public:
	bool UseTransparency() const;
	void SetTransparency(bool bUse);
	
#pragma endregion

private:
	bool		m_bStatic;
	bool		m_bOnCollision;
	bool		m_bCull;

public:
	void SetStatic(const bool bStatic);
	bool IsStatic() const;

#pragma region Prototype

private:
	static unordered_map<class CScene*, unordered_map<string, CGameObject*>>	m_mapPrototype;

public:
	static CGameObject* CreatePrototype(const string& strTag, class CScene* pScene);
	static CGameObject* CreateClone(const string& strTag, class CScene* pScene,
		class CLayer* pLayer = NULL);
	static bool ChangePrototypeScene(const string& strTag, class CScene* pCurScene,
		class CScene* pChangeScene);
	static bool ErasePrototype(const string& strTag, class CScene* pScene);
	static bool ErasePrototypeList(class CScene* pScene);

private:
	static unordered_map<string, CGameObject*>* FindPrototypeList(class CScene* pScene);
	static CGameObject* FindPrototype(const string& strTag, class CScene* pScene);
	 
#pragma endregion
public:
	CScene* GetScene() const;
	CLayer* GetLayer() const;
	void SetScene(class CScene* pScene);
	void SetLayer(class CLayer* pLayer);
	void ChangeLayer(class CLayer* pTargetLayer);
	class CTransform* GetTransform() const;
	CGameObject* GetRoot();
	void CollisionEnterAll(class CCollider* pThis, class CCollider* pTarget, float fTime);
	void CollisionStayAll(class CCollider* pThis, class CCollider* pTarget, float fTime);
	void CollisionExitAll(class CCollider* pThis, class CCollider* pTarget, float fTime);
	 
	static function<bool(CGameObject*, size_t, FILE*)> m_pLoadComponentFromExternalProject;
	static void SetClientComponentLoadCallbackFunction(bool(*pFunc)(CGameObject*, size_t, FILE*));
	bool Save(const wchar_t* pFileName, const string& strPathKey = DATA_PATH);
	bool Save(FILE* pFile);
	static CGameObject* Load(class CScene* pScene, const wchar_t* pFileName, const string& strPathKey = DATA_PATH);
	static CGameObject* Load(class CScene* pScene, FILE* pFile, CGameObject* pTarget = NULL);
	

public:
	void Reset();
	bool Initialize();
	void Start();
	int Update(float fTime);
	int LateUpdate(float fTime);
	int OnCollisionEnter(class CCollider* pThis, class CCollider* pTarget, float fTime);
	int OnCollisionStay(class CCollider* pThis, class CCollider* pTarget, float fTime);
	int OnCollisionExit(class CCollider* pThis, class CCollider* pTarget, float fTime);

	int OnMouseEnter(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition);
	int OnMouseStay(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime);
	int OnMouseExit(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition);
	int OnMouseDown(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition);
	int OnMouseDrag(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime);
	int OnMouseUp(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition);

	void Render(float fTime);
	void RenderDebug(float fTime);
	void RenderShadowMap(float fTime);

	CGameObject* Clone()	const;

public:
	size_t GetComponentCount() const;
	const list<class CCollider*>* GetColliderList() const;
	list<CComponent*>& GetAllComponents();
	const list<CComponent*>* GetComponentsFromTag(const string& strTag);
	CComponent* AddComponent(class CComponent* pComponent);
	void RemoveComponentFromList(class CComponent* pComponent);
	
	template<typename T>
	bool HasComponent()
	{
		size_t iTypeHash = typeid(T).hash_code();

		list<CComponent*>::const_iterator	iter;
		list<CComponent*>::const_iterator	iterEnd = m_ComponentList.end();

		for (iter = m_ComponentList.begin(); iter != iterEnd; ++iter)
		{
			if (iTypeHash == (*iter)->m_iTypeHash || iTypeHash == (*iter)->m_iBaseTypeHash)
				return true;
		}
		return false;
	};

	template<typename T>
	T* GetComponentInChildren()
	{
		T* pResult = GetComponent<T>();

		if (pResult)
		{
			return pResult;
		}
		else
		{
			vector<CTransform*>* vecChildren = m_pTransform->GetChildren();
			size_t iCount = vecChildren->size();
			for (size_t i = 0; i < iCount; ++i)
			{
				CGameObject* pChild = vecChildren->at(i)->GetGameObject();
				pResult = pChild->GetComponentInChildren<T>();

				if (pResult)
				{
					return pResult;
				}
				else
				{
					continue;
				}
			}
		}

		return NULL;
	}

	template<typename T>
	T* GetComponent()
	{
		size_t ConvertTypeHash = typeid(T).hash_code();

		list<CComponent*>::iterator iter;
		list<CComponent*>::iterator iterEnd = m_ComponentList.end();

		for (iter = m_ComponentList.begin(); iter != iterEnd; ++iter)
		{
			if (ConvertTypeHash == (*iter)->m_iTypeHash || (*iter)->m_iBaseTypeHash == ConvertTypeHash)
			{
				(*iter)->AddRef();
				return (T*)(*iter);
			}
		}

		return NULL;
	}

	template <typename T>
	T* GetComponentFromTag(const string& strTag)
	{
		list<CComponent*>::iterator	iter;
		list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

		for (iter = m_ComponentList.begin(); iter != iterEnd; ++iter)
		{
			if ((*iter)->GetTag() == strTag)
			{
				(*iter)->AddRef();
				return (T*)*iter;
			}
		}

		return NULL;
	}

	//여러 컴포넌트 타입을 캐싱해서 쓰면 안됨
	//Collider 찾을 때 주의.
	template<typename T>
	list<CComponent*>* CGameObject::GetComponents()
	{
		size_t iTypeHash = typeid(T).hash_code();

		m_FindComponentList.clear();

		list<CComponent*>::const_iterator	iter;
		list<CComponent*>::const_iterator	iterEnd = m_ComponentList.end();

		for (iter = m_ComponentList.begin(); iter != iterEnd; ++iter)
		{
			if ((*iter)->m_iTypeHash == iTypeHash || (*iter)->m_iBaseTypeHash == iTypeHash)
			{
				(*iter)->AddRef();
				m_FindComponentList.push_back(*iter);
			}
		}

		return &m_FindComponentList;
	}


	template <typename T>
	T* AddComponent(const string& strTag)
	{
		T*	pCom = new T;
		((CComponent*)pCom)->m_iTypeHash = typeid(T).hash_code();
		pCom->SetTag(strTag);
		pCom->SetScene(m_pScene);
		pCom->SetLayer(m_pLayer);
		pCom->SetGameObject(this);
		pCom->SetTransform(m_pTransform);

		if (!pCom->Initialize())
		{
			SAFE_RELEASE(pCom);
			return NULL;
		}

		return (T*)AddComponent((CComponent*)pCom);
	}



	template <typename T>
	T* AddComponent()
	{
		T*	pCom = new T;

		string strClassName = typeid(T).name();
		strClassName = string(strClassName.begin() + 6, strClassName.end());

		((CComponent*)pCom)->m_iTypeHash = typeid(T).hash_code();
		pCom->SetTag(strClassName);
		pCom->SetScene(m_pScene);
		pCom->SetLayer(m_pLayer);
		pCom->SetGameObject(this);
		pCom->SetTransform(m_pTransform);

		if (!pCom->Initialize())
		{
			SAFE_RELEASE(pCom);
			return NULL;
		}

		return (T*)AddComponent((CComponent*)pCom);
	}
};

SSS_END
