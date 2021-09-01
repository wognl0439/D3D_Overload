#include "AnimationState.h"
#include "Transition.h"
#include "AnimationClip.h"
#include "Component\Animator.h"
#include "AnimationStateController.h"
#include "Component/MeshRenderer.h"
#include "Mathf.h"

SSS_USING

CAnimationState::CAnimationState():
	m_pController(NULL),
	m_pAnimator(NULL),
	m_bOnTransition(false),
	m_pAnimationClip(NULL),
	m_pReservedTransition(NULL),
	m_bEnd(false),
	m_fSpeed(1.0f),
	m_bLoop(true)
{
	//m_strStateName 은 Controller에서 State 추가 시 설정 해줌.
}

CAnimationState::~CAnimationState()
{
	SAFE_DELETE(m_pAnimationClip);
	Safe_Delete_VecList(m_vecTransition);
}

CAnimator * CAnimationState::GetAnimator() const
{
	return m_pAnimator;
}

CAnimationStateController * CAnimationState::GetAnimationController() const
{
	return m_pController;
}

bool CAnimationState::IsLoop() const
{
	return m_bLoop;
}

bool CAnimationState::Loop(bool bUse)
{
	m_bLoop = bUse;
	return m_bLoop;
}

bool CAnimationState::End() const
{
	return m_bEnd;
}

CAnimationClip * CAnimationState::GetClip() const
{
	return m_pAnimationClip;
}

void CAnimationState::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;
}

void CAnimationState::SetStateName(const string & strStateName)
{
	if (strStateName == "Entry State" || strStateName == "Exit State" || strStateName == "Any State")
	{
		m_strStateName = strStateName;
		return;
	}

	unordered_map<string, CAnimationState*>::iterator iter = m_pController->m_mapState.find(m_strStateName);

	if (iter != m_pController->m_mapState.end())
	{
		m_pController->m_mapState.erase(iter);
	}
	else
	{
	}
	m_strStateName = strStateName;
	m_pController->m_mapState.insert(make_pair(m_strStateName, this));
}

float CAnimationState::GetSpeed() const
{
	return m_fSpeed;
}

CAnimationClip* CAnimationState::CreateEmptyClip(const string& strClipName)
{
	SAFE_DELETE(m_pAnimationClip);
	m_pAnimationClip = new CAnimationClip;
	m_pAnimationClip->m_pAnimationController = m_pController;
	m_pAnimationClip->m_pAnimationState = this;
	m_pAnimationClip->m_pAnimator = m_pAnimator;
	m_pAnimationClip->SetClipName(strClipName);	
	
	return m_pAnimationClip;
}

void CAnimationState::SetClip(const string & strClipName, size_t iStartFrame, size_t iEndFrame)
{
	SAFE_DELETE(m_pAnimationClip);

	m_pAnimationClip = new CAnimationClip;	
	m_pAnimationClip->m_pAnimationController = m_pController;
	m_pAnimationClip->m_pAnimationState = this;
	m_pAnimationClip->m_pAnimator = m_pAnimator;
	m_pAnimationClip->SetKeyFrame(strClipName, iStartFrame, iEndFrame);
}

void CAnimationState::SetClip(const string & strClipName)
{
	SAFE_DELETE(m_pAnimationClip);
	m_pAnimationClip = new CAnimationClip;
	m_pAnimationClip->m_pAnimationController = m_pController;
	m_pAnimationClip->m_pAnimationState = this;
	m_pAnimationClip->m_pAnimator = m_pAnimator;
	m_pAnimationClip->SetKeyFrame(strClipName);
}

void CAnimationState::SetClip(size_t iClipIndexFromMesh)
{
	SAFE_DELETE(m_pAnimationClip);
	m_pAnimationClip = new CAnimationClip;
	m_pAnimationClip->m_pAnimationController = m_pController;
	m_pAnimationClip->m_pAnimationState = this;
	m_pAnimationClip->m_pAnimator = m_pAnimator;
	m_pAnimationClip->SetKeyFrame(iClipIndexFromMesh);
}

size_t CAnimationState::GetTransitionCount() const
{
	return m_vecTransition.size();
}

CTransition * CAnimationState::GetTransition(size_t idx)
{
	if (idx < m_vecTransition.size())
	{
		return m_vecTransition[idx];
	}

	return NULL;
}

const vector<class CTransition*>& CAnimationState::GetTransition() const
{
	return m_vecTransition;
}

void CAnimationState::AddTransition(CTransition * pTransition)
{
	pTransition->m_pBaseState = this;
	m_vecTransition.push_back(pTransition);
}

//NULL 들어오면 빈 Transition 생성함
CTransition* CAnimationState::AddTransition(CAnimationState * pTarget)
{
	CTransition* pTransition = new CTransition;	
	pTransition->SetTargetState(pTarget);
	pTransition->m_pAnimator = m_pAnimator;
	pTransition->m_pAnimationController = m_pController;
	pTransition->m_pBaseState = this;

	m_vecTransition.push_back(pTransition);
	
	return pTransition;
}

CTransition * CAnimationState::AddTransition()
{
	CTransition* pTransition = new CTransition;
	pTransition->m_pAnimator = m_pAnimator;
	pTransition->m_pAnimationController = m_pController;
	pTransition->m_pBaseState = this;

	m_vecTransition.push_back(pTransition);

	return pTransition;
}

void CAnimationState::RemoveTransition(CTransition * pTransition)
{
	if (pTransition)
	{
		vector<CTransition*>::iterator iter;
		vector<CTransition*>::iterator iterEnd = m_vecTransition.end();
		for (iter = m_vecTransition.begin(); iter != iterEnd; ++iter)
		{
			if ((*iter)== pTransition)
			{
				SAFE_DELETE((*iter));
				m_vecTransition.erase(iter);
				break;
			}
		}
	}
}

void CAnimationState::RemoveTransition(CAnimationState * pTarget)
{
	if (pTarget)
	{
		vector<CTransition*>::iterator iter;
		vector<CTransition*>::iterator iterEnd = m_vecTransition.end();
		for (iter = m_vecTransition.begin(); iter != iterEnd; ++iter)
		{
			if ((*iter)->GetTargetState() == pTarget)
			{
				SAFE_DELETE((*iter));
				m_vecTransition.erase(iter);
				break;
			}
		}
	}
}

void CAnimationState::RemoveTransition(size_t idx)
{
	if (idx < m_vecTransition.size())
	{
		vector<CTransition*>::iterator iter = m_vecTransition.begin() + idx;
		SAFE_DELETE(m_vecTransition[idx]);
		m_vecTransition.erase(iter);
	}
}

void CAnimationState::ClearTransition()
{
	Safe_Delete_VecList(m_vecTransition);
}

void CAnimationState::ResetState()
{
	m_bOnTransition = false;
	m_pReservedTransition = NULL;
	m_bEnd = false;
	size_t iTransitionCount = m_vecTransition.size();
	for (size_t i = 0; i < iTransitionCount; ++i)
	{
		m_vecTransition[i]->ResetTransition();
	}
}

const string & CAnimationState::GetStateName() const
{
	return m_strStateName;
}

void CAnimationState::UpdateCondition(float fTime)
{
	if (m_pAnimationClip)
		m_bEnd = m_pAnimationClip->Update(fTime);

	//예약 되어 있는 Transition이 없을 경우에만 진입
	if (!m_bOnTransition)
	{
		//Transition이 한개도 없다면 예약 자체를 못하기 때문에 Clip의 상태에 따라 반복, 또는 GameObject 제거.
		for (size_t i = 0; i < m_vecTransition.size(); ++i)
		{
			if (m_vecTransition[i]->GetTransitionCondition())
			{
				//Transition 될 수 있도록 예약함.
				m_bOnTransition = true;
				m_pReservedTransition = m_vecTransition[i];
				
				if (!m_pReservedTransition->m_bHasExitTime)
				{
					m_pReservedTransition->m_fExitTime = m_pAnimationClip->GetProgress();
				}
				break;
			}
		}
	}

	if (m_bOnTransition)
	{
		float fTransitionProgress = CMathf::Clamp(m_pReservedTransition->UpdateTimer(fTime) / m_pReservedTransition->m_fExitDuration, 0.0f, 1.0f);

		if (fTransitionProgress >= 1.0f)
		{
			float fProgressOffset = m_pReservedTransition->GetTargetState()->GetClip()->GetProgressAtTime(m_pReservedTransition->m_fExitDuration);
			m_pController->TransitionState(m_pReservedTransition->GetTargetState(), fProgressOffset);	
		}		
	}
}

void CAnimationState::Save(FILE * pFile)
{
	size_t iLength = m_strStateName.length();
	fwrite(&iLength, sizeof(size_t), 1, pFile);
	fwrite(m_strStateName.c_str(), sizeof(char), iLength, pFile);
	fwrite(&m_bLoop, sizeof(bool), 1, pFile);
	fwrite(&m_fSpeed, sizeof(float), 1, pFile);
	m_pAnimationClip->Save(pFile);
}

void CAnimationState::Load(FILE * pFile)
{
	size_t iLength = m_strStateName.length();
	char strStateName[256] = {};
	fread(&iLength, sizeof(size_t), 1, pFile);
	fread(strStateName, sizeof(char), iLength, pFile);
	fread(&m_bLoop, sizeof(bool), 1, pFile);
	fread(&m_fSpeed, sizeof(float), 1, pFile);

	m_pAnimationClip = new CAnimationClip;
	m_pAnimationClip->m_pAnimator = m_pAnimator;
	m_pAnimationClip->m_pAnimationController = m_pController;	
	m_pAnimationClip->m_pAnimationState = this;
	m_pAnimationClip->Load(pFile);
}
