#include "Component.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Collider.h"

SSS_USING


CComponent::CComponent():
	m_pScene(NULL),
	m_pTransform(NULL),
	m_pLayer(NULL),
	m_pGameObject(NULL),
	m_iTypeHash(0),
	m_iBaseTypeHash(0),
	m_bStarted(false)
{
}

CComponent::CComponent(const CComponent & component)
{
	*this = component;
}

CComponent::~CComponent()
{
}

CScene * CComponent::GetScene() const
{
	if (m_pScene)
		m_pScene->AddRef();
	return m_pScene;
}

CLayer * CComponent::GetLayer() const
{
	if (m_pLayer)
		m_pLayer->AddRef();
	return m_pLayer;
}

CGameObject * CComponent::GetGameObject() const
{
	if (m_pGameObject)
		m_pGameObject->AddRef();
	return m_pGameObject;
}

CTransform * CComponent::GetTransform() const
{
	if (m_pTransform)
		m_pTransform->AddRef();

	return m_pTransform;
}

void CComponent::SetScene(CScene * pScene)
{
	m_pScene = pScene;
}

void CComponent::SetLayer(CLayer * pLayer)
{
	m_pLayer = pLayer;	
}

void CComponent::SetGameObject(CGameObject * pGameObject)
{
	m_pGameObject = pGameObject;
}

void CComponent::SetTransform(CTransform * pTransform)
{
	m_pTransform = pTransform;
}

list<CComponent*>& CComponent::GetAllComponents()
{
	return m_pGameObject->GetAllComponents();
}

void CComponent::Reset()
{
}

void CComponent::Start()
{
}

int CComponent::Update(float fTime)
{
	return 0;
}

int CComponent::LateUpdate(float fTime)
{
	return 0;
}

int CComponent::OnCollisionEnter(CCollider* pThis, CCollider* pTarget, float fTime)
{
	return 0;
}

int CComponent::OnCollisionStay(CCollider* pThis, CCollider* pTarget, float fTime)
{
	return 0;
}

int CComponent::OnCollisionExit(CCollider* pThis, CCollider* pTarget, float fTime)
{
	return 0;
}

void CComponent::OnMouseEnter(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
}

void CComponent::OnMouseStay(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
}

void CComponent::OnMouseExit(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
}

void CComponent::OnMouseDown(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
}

void CComponent::OnMouseDrag(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition, float fTime)
{
}

void CComponent::OnMouseUp(CCollider* pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
}


int CComponent::Prerender(class CMeshRenderer* pRenderer)
{
	return 0;
}

int CComponent::Postrender(CMeshRenderer * pRenderer)
{
	return 0;
}

void CComponent::RenderDebug(float fTime)
{
}

void CComponent::Render(float fTime)
{
}

CComponent * CComponent::Clone() const
{
	return nullptr;
}

bool CComponent::Save(FILE * pFile)
{
	return true;
}

bool CComponent::Load(FILE * pFile)
{
	return true;
}
