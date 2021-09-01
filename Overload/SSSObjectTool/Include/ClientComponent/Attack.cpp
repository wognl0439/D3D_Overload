#include "Attack.h"
#include "Component/MeshRenderer.h"
#include "Component/Effect.h"
#include "Component/Trail.h"
#include "Component/Distortion.h"
#include "Collider.h"
#include "MonsterAI.h"

#include "Soundmanager.h"
#include "BoneDragonAI.h"

CAttack::CAttack()
{
	m_fActiveTime = 0.f;
	m_iValue = 0;
	m_fActiveMaxTime = 0.5f;
	m_fDamagePoint = 1.f;
	m_eAttribute = ATTACK_NORMAL;
	m_bDamageSound = false;
}


CAttack::~CAttack()
{
}

void CAttack::SetValue(int iValue)
{
	m_iValue = iValue;
}

int CAttack::GetValue() const
{
	return m_iValue;
}

void CAttack::SetAttribute(ATTACK_ATTRIBUTE eAttribute)
{
	m_eAttribute = eAttribute;
}

ATTACK_ATTRIBUTE CAttack::GetAttribute() const
{
	return m_eAttribute;
}

void CAttack::SetActiveMaxTime(float fTime)
{
	m_fActiveMaxTime = fTime;
}

void CAttack::SetDamagePoint(float fDMG)
{
	m_fDamagePoint = fDMG;
}

float CAttack::GetDamagePoint() const
{
	return m_fDamagePoint;
}

bool CAttack::Initialize()
{
	m_iValue = -1;

	return true;
}

void CAttack::Start()
{
}

int CAttack::Update(float fTime)
{
	if (m_iValue != -1
		&& m_iValue != 1)
		return 0;

	m_fActiveTime += fTime;
	float fProgress = m_fActiveTime / m_fActiveMaxTime;
	if (fProgress >= 1.f)
	{
		m_pGameObject->Destroy(true);
	}

	return 0;
}

int CAttack::OnCollisionEnter(CCollider * pThis, CCollider * pTarget, float fTime)
{
	CMonsterAI* pMonster = pTarget->GetComponent<CMonsterAI>();
	if (pMonster && m_iValue == 1)
	{
		if (!m_bDamageSound)
		{
			GET_SINGLE(CSoundManager)->Play("Damage");
			m_bDamageSound = true;
		}
		SAFE_RELEASE(pMonster);
	}
	else
	{
		CBoneDragonAI* pBone = pTarget->GetComponent<CBoneDragonAI>();
		if (pBone && m_iValue == 1)
		{
			if (!m_bDamageSound)
			{
				GET_SINGLE(CSoundManager)->Play("Damage");
				m_bDamageSound = true;
			}
			SAFE_RELEASE(pBone);
		}
	}
	return 0;
}

int CAttack::OnCollisionStay(CCollider * pThis, CCollider * pTarget, float fTime)
{
	CMonsterAI* pMonster = pTarget->GetComponent<CMonsterAI>();
	if (pMonster)
	{
		pMonster->SetIsHited(true);
		SAFE_RELEASE(pMonster);	
	}
	return 0;
}

int CAttack::OnCollisionExit(CCollider * pThis, CCollider * pTarget, float fTime)
{
	return 0;
}
