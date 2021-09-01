#include "Chat.h"
#include "GameObject.h"
#include "Transform.h"
#include "Component/SpriteRenderer.h"
#include "DataManager.h"
#include "Material.h"
#include "Input.h"
#include "Component/UICollider.h"


CChat::CChat():
	m_vPannelSize(500.0f, 450.0f),
	m_vDialoguePannelPosition(30.0f, 30.0f),
	m_vDialoguePannelSize(340.0f, 190.0f),
	m_iLimitLine(5),

	m_bOnTranslation(false)
{
}


CChat::~CChat()
{
}

bool CChat::Initialize()
{
	

	//for (int i = 0; i < 9; ++i)
	//{
	//	char index[4] = {};
	//	itoa(i, index, 10);
	//	string strAtlasKey = string("Slice0") + string(index);
	//
	//	CGameObject* pGameObject = CGameObject::CreateObject("Chat Background", m_pLayer);
	//	CTransform* pTransform = pGameObject->GetTransform();
	//	pTransform->SetParent(m_pTransform);
	//	SAFE_RELEASE(pTransform);
	//
	//	CSpriteRenderer* pRenderer = pGameObject->AddComponent<CSpriteRenderer>("Renderer");
	//	CMaterial* pMaterial = pRenderer->GetMaterial();
	//	pMaterial->SetSampler(SAMPLER_LINEAR);
	//	pMaterial->SetDiffuseTexture("nineslice_test", L"nineslice_test.png");
	//	pRenderer->SetImageInfo(strAtlasKey);
	//	pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	//	SAFE_RELEASE(pMaterial);
	//	SAFE_RELEASE(pRenderer);
	//
	//	m_vecBoards.push_back(pGameObject);
	//	SAFE_RELEASE(pGameObject);
	//}
	//
	//
	//
	////LT
	//CTransform* pTransform = m_vecBoards[0]->GetTransform();
	//pTransform->SetLocalPosition(0.0f, 0.0f, 0.0f);
	//pTransform->SetLocalScale(10.0f, 10.0f, 1.0f);
	//SAFE_RELEASE(pTransform);
	//
	////T
	//pTransform = m_vecBoards[1]->GetTransform();
	//pTransform->SetLocalPosition(10.0f, 0.0f, 0.0f);
	//pTransform->SetLocalScale(m_vPannelSize.x - 20.0f, 10.0f, 1.0f);
	//SAFE_RELEASE(pTransform);
	//
	////RT
	//pTransform = m_vecBoards[2]->GetTransform();
	//pTransform->SetLocalPosition(m_vPannelSize.x - 10.0f, 0.0f, 0.0f);
	//pTransform->SetLocalScale(10.0f, 10.0f, 1.0f);
	//SAFE_RELEASE(pTransform);
	//
	////L
	//pTransform = m_vecBoards[3]->GetTransform();
	//pTransform->SetLocalPosition(0.0f, 10.0f, 0.0f);
	//pTransform->SetLocalScale(10.0f, m_vPannelSize.y - 20.0f, 1.0f);
	//SAFE_RELEASE(pTransform);
	//
	////C
	//pTransform = m_vecBoards[4]->GetTransform();
	//pTransform->SetLocalPosition(10.0f, 10.0f, 0.0f);
	//pTransform->SetLocalScale(m_vPannelSize.x - 20.0f, m_vPannelSize.y - 20.0f, 1.0f);
	//SAFE_RELEASE(pTransform);
	//
	////R
	//pTransform = m_vecBoards[5]->GetTransform();
	//pTransform->SetLocalPosition(m_vPannelSize.x - 10.0f, 10.0f, 0.0f);
	//pTransform->SetLocalScale(10.0f, m_vPannelSize.y - 20.0f, 1.0f);
	//SAFE_RELEASE(pTransform);
	//
	////LB
	//pTransform = m_vecBoards[6]->GetTransform();
	//pTransform->SetLocalPosition(0.0f, m_vPannelSize.y - 10.0f, 0.0f);
	//pTransform->SetLocalScale(10.0f, 10.0f, 1.0f);
	//SAFE_RELEASE(pTransform);
	//
	////B
	//pTransform = m_vecBoards[7]->GetTransform();
	//pTransform->SetLocalPosition(10.0f, m_vPannelSize.y - 10.0f, 0.0f);
	//pTransform->SetLocalScale(m_vPannelSize.x - 20.0f, 10.0f, 1.0f);
	//SAFE_RELEASE(pTransform);
	//
	////RB
	//pTransform = m_vecBoards[8]->GetTransform();
	//pTransform->SetLocalPosition(m_vPannelSize.x - 10.0f, m_vPannelSize.y - 10.0f, 0.0f);
	//pTransform->SetLocalScale(10.0f, 10.0f, 1.0f);
	//SAFE_RELEASE(pTransform);


	return true;
}

int CChat::Update(float fTime)
{
	//if (m_bOnTranslation)
	//{
	//	Vector2 vCursorPosition = GET_SINGLE(CInput)->GetCursorPosition(SPACE_DEVICE);
	//	
	//	Vector2 vPosition = vCursorPosition - m_vRelativePosition;
	//	m_pTransform->SetWorldPosition(vPosition.x, vPosition.y, m_pTransform->GetWorldPosition().z);
	//}
	return 0;
}

void CChat::OnMouseStay(CCollider* pTarget, const Vector2 & mousePosition, const Vector3 & vIntersectedPosition, float fTime)
{
	//if (GETKEYDOWN(MOUSE_LEFT))
	//{
	//	Vector3 vTempPosition = m_pTransform->GetWorldPosition();
	//	Vector2 vPosition(vTempPosition.x, vTempPosition.y);
	//	m_bOnTranslation = true;
	//	m_vRelativePosition = GET_SINGLE(CInput)->GetCursorPosition(SPACE_DEVICE) - vPosition;
	//
	//	size_t iSize = m_vecBoards.size();
	//	for (size_t i = 0; i < iSize; ++i)
	//	{
	//		CSpriteRenderer* pRenderer = m_vecBoards[i]->GetComponent<CSpriteRenderer>();
	//		pRenderer->SetDiffuse(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
	//		SAFE_RELEASE(pRenderer);
	//	}
	//}
	//
	//if (GETKEYUP(MOUSE_LEFT))
	//{
	//	m_bOnTranslation = false;
	//
	//	size_t iSize = m_vecBoards.size();
	//	for (size_t i = 0; i < iSize; ++i)
	//	{
	//		CSpriteRenderer* pRenderer = m_vecBoards[i]->GetComponent<CSpriteRenderer>();
	//		pRenderer->SetDiffuse(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	//		SAFE_RELEASE(pRenderer);
	//	}
	//}
}