#include "Player.h"
#include "Collider.h"
#include "AnimationStateController.h"
#include "Component/Animator.h"
#include "Transform.h"
#include "Input.h"
#include "Component/MeshRenderer.h"
#include "ResourcesManager.h"
#include "Flag.h"



CPlayer::CPlayer()
{
	m_fMoveSpeed = 2.0f;
	m_fJumpHeight = 2.0f;
	m_fDamage = 2.0f;
	m_fDefence = 2.0f;
	m_fHP = 20.0f;
	m_fHPMax = 20.0f;
	m_fMP = 20.0f;
	m_fMPMax = 20.0f;
	m_fSP = 20.0f;
	m_fSPMax = 20.0f;
	m_fRotateSpeed = 15.0f;
}

CPlayer::~CPlayer()
{
}

int CPlayer::GetItemIndex(const string & strItemName)
{
	unordered_map<string, ITEMINFO>::iterator iter = m_mapEquipItemInfo.find(strItemName);

	if (iter != m_mapEquipItemInfo.end())
	{
		return iter->second.iID;
	}

	return -1;
}

void CPlayer::ChangeEquipment(const string & strOnEquipItemName)
{
	ITEMINFO tItem = GetEquipItemInfo(strOnEquipItemName);

	unordered_map<string, ITEMINFO>::iterator iter = m_mapEquipItemInfo.begin();
	unordered_map<string, ITEMINFO>::iterator iterEnd = m_mapEquipItemInfo.end();

	CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();
	for (; iter != iterEnd; ++iter)
	{
		int iIndex = iter->second.iID;
		if (iIndex > 0)
		{
			if (iter->second.eEquipPart == tItem.eEquipPart)
			{
				//같은 부위 전부 끔
				pRenderer->SetEnableMeshContainer(iter->second.iID, false);
			}
		}
	}

	//들어온 아이 켬
	pRenderer->SetEnableMeshContainer(tItem.iID, true);

	SAFE_RELEASE(pRenderer);
}

ITEMINFO CPlayer::GetEquipItemInfo(const string & strItemName)
{
	unordered_map<string, ITEMINFO>::iterator iter = m_mapEquipItemInfo.find(strItemName);
	if (iter != m_mapEquipItemInfo.end())
	{
		return iter->second;
	}
	else
	{
		return ITEMINFO();
	}
}

bool CPlayer::Initialize()
{
	ITEMINFO tInfo;
	tInfo.eType = IT_EQUIPMENT;
	tInfo.eEquipPart = IEP_HAIR;
	tInfo.eGrade = IG_LEGENDARY;
	tInfo.iPrice = 58000000;

	tInfo.iID = 11;
	tInfo.strName = "HAIR004_DEFAULT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	tInfo.iID = 0;
	tInfo.strName = "HAIR113";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	tInfo.iID = 10;
	tInfo.strName = "HAIR109";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	tInfo.iID = 12;
	tInfo.strName = "HAIR007";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	tInfo.iID = 13;
	tInfo.strName = "HAIR101";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	tInfo.iID = 1;
	tInfo.eEquipPart = IEP_NONE;
	tInfo.strName = "FACE";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));	
	tInfo.iID = 2;
	tInfo.strName = "EAR";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

#pragma region Upper
	tInfo.iID = 52;
	tInfo.eEquipPart = IEP_UPPER;
	tInfo.strName = "UW0001_UPPER_DEFAULT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 41;
	tInfo.strName = "RB0203_UPPER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 34;
	tInfo.strName = "MS0102_UPPER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 42;
	tInfo.strName = "MD2009_UPPER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 27;
	tInfo.strName = "LT0201_UPPER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 20;
	tInfo.strName = "HV3001_UPPER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 9;
	tInfo.strName = "EC0011_UPPER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 41;
	tInfo.strName = "RB0203_UPPER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

#pragma endregion

#pragma region SHOULDER


	tInfo.iID = 8;
	tInfo.eEquipPart = IEP_SHOULDER;
	tInfo.strName = "RB2007_SHOULDER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 40;
	tInfo.strName = "RB0203_SHOULDER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 33;
	tInfo.strName = "MS0102_SHOULDER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 48;
	tInfo.strName = "MD2009_SHOULDER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 26;
	tInfo.strName = "LT0201_SHOULDER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 19;
	tInfo.strName = "HV3001_SHOULDER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

#pragma endregion

#pragma region LOWER
	tInfo.iID = 51;
	tInfo.eEquipPart = IEP_LOWER;
	tInfo.strName = "UW0001_LOWER_DEFAULT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	
	tInfo.iID = 39;
	tInfo.strName = "RB0203_LOWER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	
	tInfo.iID = 32;
	tInfo.strName = "MS0102_LOWER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 47;
	tInfo.strName = "MD2009_LOWER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 25;
	tInfo.strName = "LT0201_LOWER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 18;
	tInfo.strName = "HV3001_LOWER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 6;
	tInfo.strName = "MD0008_LOWER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
#pragma endregion

#pragma region HELMET
	
	tInfo.iID = 38;
	tInfo.eEquipPart = IEP_HELMET;
	tInfo.strName = "RB0203_HELMET";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	
	tInfo.iID = 31;
	tInfo.strName = "MS0102_HELMET";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 46;
	tInfo.strName = "MD2009_HELMET";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 24;
	tInfo.strName = "LT0201_HELMET";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 17;
	tInfo.strName = "HV3001_HELMET";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 3;
	tInfo.strName = "EC0016_HELMET";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

#pragma endregion

#pragma region GLOVES

	tInfo.iID = 50;
	tInfo.eEquipPart = IEP_GLOVES;
	tInfo.strName = "UW0001_GLOVES_DEFAULT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 37;
	tInfo.strName = "RB0203_GLOVES";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 30;
	tInfo.strName = "MS0102_GLOVES";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 45;
	tInfo.strName = "MD2009_GLOVES";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 23;
	tInfo.strName = "LT0201_GLOVES";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 16;
	tInfo.strName = "HV3001_GLOVES";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 5;
	tInfo.strName = "HV0101_GLOVES";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

#pragma endregion

#pragma region BOOTS

	tInfo.iID = 49;
	tInfo.eEquipPart = IEP_BOOTS;
	tInfo.strName = "UW0001_BOOTS_DEFAULT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 36;
	tInfo.strName = "RB0203_BOOTS";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 29;
	tInfo.strName = "MS0102_BOOTS";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 44;
	tInfo.strName = "MD2009_BOOTS";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 22;
	tInfo.strName = "LT0201_BOOTS";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 15;
	tInfo.strName = "HV3001_BOOTS";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 4;
	tInfo.strName = "HV0101_BOOTS";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	
#pragma endregion

#pragma region BELT

	tInfo.iID = 7;
	tInfo.eEquipPart = IEP_BELT;
	tInfo.strName = "RB1003_BELT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 35;
	tInfo.strName = "RB0203_BELT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 28;
	tInfo.strName = "MS0102_BELT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 43;
	tInfo.strName = "MD2009_BELT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 21;
	tInfo.strName = "LT0201_BELT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 14;
	tInfo.strName = "HV3001_BELT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));


#pragma endregion	
	return true;
}

void CPlayer::Start()
{
	CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();	
	size_t iContainerCount = pRenderer->GetContainerCount();

	for (size_t i = 0; i < iContainerCount; ++i)
	{
		pRenderer->SetEnableMeshContainer(i, false);
	}


	pRenderer->SetEnableMeshContainer(GetItemIndex("FACE"), true);
	pRenderer->SetEnableMeshContainer(GetItemIndex("EAR"), true);
	pRenderer->SetEnableMeshContainer(GetItemIndex("UW0001_BOOTS_DEFAULT"), true);
	pRenderer->SetEnableMeshContainer(GetItemIndex("UW0001_GLOVES_DEFAULT"), true);
	pRenderer->SetEnableMeshContainer(GetItemIndex("UW0001_LOWER_DEFAULT"), true);
	pRenderer->SetEnableMeshContainer(GetItemIndex("HAIR004_DEFAULT"), true);
	pRenderer->SetEnableMeshContainer(GetItemIndex("UW0001_UPPER_DEFAULT"), true);

	SAFE_RELEASE(pRenderer);
}



int CPlayer::Update(float fTime)
{
	//캐릭터 FSM 기본 예외 구문
	

	if (GETKEY(KEY_LEFT_SHIFT))
	{

		if (GETKEYDOWN(KEY_1))
		{
			ChangeEquipment("EC0011_UPPER");
		}

		if (GETKEYDOWN(KEY_2))
		{
			ChangeEquipment("LT0201_LOWER");
		}

		if (GETKEYDOWN(KEY_3))
		{
			ChangeEquipment("MS0102_HELMET");
		}

		if (GETKEYDOWN(KEY_4))
		{
			ChangeEquipment("MD2009_GLOVES");
		}

		if (GETKEYDOWN(KEY_5))
		{
			ChangeEquipment("MS0102_BOOTS");
		}

		if (GETKEYDOWN(KEY_6))
		{
			ChangeEquipment("HAIR113");
		}
	}
	else
	{
		if (m_pTransform->GetChild(0))
		{
			CGameObject* pChild = m_pTransform->GetChild(0)->GetGameObject();
			CAnimator* pAnimator = pChild->GetComponent<CAnimator>();
			SAFE_RELEASE(pChild);
			if (pAnimator)
			{
				CAnimationStateController* pController = pAnimator->GetAnimationController();
				if (pController)
				{
					//요기까지
					if (GETKEY(KEY_W))
					{
						pController->SetBool("Walk Front", true);
						m_pTransform->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * m_fMoveSpeed * fTime);
					}
					else
					{
						pController->SetBool("Walk Front", false);
					}

					if (GETKEY(KEY_A))
					{
						Vector3 vRotation = m_pTransform->GetWorldRotation();
						float fAngle = vRotation.y;
						fAngle -= m_fRotateSpeed * D2R * fTime;
						m_pTransform->SetWorldRotation(vRotation.x, fAngle, vRotation.z);
					}

					if (GETKEY(KEY_S))
					{
						pController->SetBool("Walk Back", true);
						m_pTransform->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->back() * m_fMoveSpeed* fTime);
					}
					else
					{
						pController->SetBool("Walk Back", false);
					}

					if (GETKEY(KEY_D))
					{
						Vector3 vRotation = m_pTransform->GetWorldRotation();
						float fAngle = vRotation.y;
						fAngle += m_fRotateSpeed * D2R * fTime;
						m_pTransform->SetWorldRotation(vRotation.x, fAngle, vRotation.z);
					}

					if (GETKEYDOWN(KEY_1))
					{
						pController->SetTrigger("Attack01");
					}

					if (GETKEYDOWN(KEY_2))
					{
						pController->SetBool("Skill", true);
					}
				}
			}
			SAFE_RELEASE(pAnimator);
		}
	}

	return 0;
}

int CPlayer::OnCollisionEnter(CCollider * pThis, CCollider * pTarget, float fTime)
{
	CGameObject* pTargetObject = pTarget->GetGameObject();
	CGameObject* pTargetRoot = pTargetObject->GetRoot();
	SAFE_RELEASE(pTargetObject);
	CAnimator* pAnimator = pTargetRoot->GetComponentInChildren<CAnimator>();

	if (pAnimator)
	{
		pAnimator->GetAnimationController()->SetTrigger("Damaged");
	}

	SAFE_RELEASE(pAnimator);



	return 0;
}

int CPlayer::OnCollisionStay(CCollider * pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

int CPlayer::OnCollisionExit(CCollider * pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

bool CPlayer::Save(FILE * pFile)
{
	return true;
}

bool CPlayer::Load(FILE * pFile)
{
	return true;
}
