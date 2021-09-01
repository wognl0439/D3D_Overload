#pragma once
#include "Ref.h"
#include "GameObject.h"

SSS_BEGIN

class SSS_DLL CComponent :
	public CRef
{	
private:
	friend class CSceneManager;
	friend class CGameObject;
	size_t m_iTypeHash;

protected:
	size_t m_iBaseTypeHash;


protected:
	CComponent();
	CComponent(const CComponent& component);
	virtual ~CComponent() = 0;

	

protected:
	bool m_bStarted;
	class CScene*			m_pScene;
	class CLayer*				m_pLayer;
	class CGameObject*	m_pGameObject;
	class CTransform*		m_pTransform;

public:
	class CScene* GetScene()	const;
	class CLayer* GetLayer()	const;
	class CGameObject* GetGameObject()	const;
	class CTransform* GetTransform()	const;

public:
	void SetScene(class CScene* pScene);
	void SetLayer(class CLayer* pLayer);
	void SetGameObject(class CGameObject* pGameObject);
	void SetTransform(class CTransform* pTransform);

	list<CComponent*>& GetAllComponents();

	template<typename T>
	T* GetComponent()
	{
		return m_pGameObject->GetComponent<T>();
	}

	template<typename T>
	T* GetComponentInChildren()
	{
		return m_pGameObject->GetComponentInChildren<T>();
	}

	template<typename T>
	bool IsComponent()
	{	
		return m_iTypeHash == typeid(T).hash_code();
	}

	template<typename T>
	bool IsBaseComponent()
	{
		return m_iBaseTypeHash == typeid(T).hash_code();
	}

	
public:
	virtual void Reset();
	virtual bool Initialize() = 0;
	virtual void Start();
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int OnCollisionEnter(class CCollider* pThis, class CCollider* pTarget , float fTime);
	virtual int OnCollisionStay(class CCollider* pThis, class CCollider* pTarget, float fTime);
	virtual int OnCollisionExit(class CCollider* pThis, class CCollider* pTarget, float fTime);

	virtual void OnMouseEnter(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition);
	virtual void OnMouseStay(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime);
	virtual void OnMouseExit(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition );
	virtual void OnMouseDown(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition);
	virtual void OnMouseDrag(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime);
	virtual void OnMouseUp(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition);
	
	virtual int Prerender(class CMeshRenderer* pRenderer);
	virtual int Postrender(class CMeshRenderer* pRenderer);
	virtual void RenderDebug(float fTime);
	virtual void Render(float fTime);
	virtual CComponent* Clone()	const;

	virtual bool Save(FILE* pFile);
	virtual bool Load(FILE* pFile);
};

SSS_END
