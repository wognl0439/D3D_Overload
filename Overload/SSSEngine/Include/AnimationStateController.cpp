#include "AnimationStateController.h"
#include "AnimationState.h"
#include "Component\Animator.h"
#include "AnimationClip.h"
#include "Transition.h"
#include "Mathf.h"

SSS_USING

CAnimationStateController::CAnimationStateController() :
	m_pEntryState(NULL),
	m_pAnyState(NULL),
	m_pExitState(NULL),
	m_pCurrentState(NULL),
	m_pAnimator(NULL)
{
	m_pEntryState = new CAnimationState;
	m_pEntryState->m_pAnimator = m_pAnimator;
	m_pEntryState->m_pController = this;
	m_pEntryState->SetStateName("Entry State");
	m_pAnyState = new CAnimationState;
	m_pAnyState->m_pAnimator = m_pAnimator;
	m_pAnyState->m_pController = this;
	m_pAnyState->SetStateName("Any State");
	m_pExitState = new CAnimationState;
	m_pExitState->m_pAnimator = m_pAnimator;
	m_pExitState->m_pController = this;
	m_pExitState->SetStateName("Exit State");
}

CAnimationStateController::CAnimationStateController(CAnimator * pAnimator) :
	m_pEntryState(NULL),
	m_pAnyState(NULL),
	m_pExitState(NULL),
	m_pCurrentState(NULL),
	m_pAnimator(pAnimator)
{
	m_pEntryState = new CAnimationState;
	m_pEntryState->m_pAnimator = m_pAnimator;
	m_pEntryState->m_pController = this;
	m_pEntryState->SetStateName("Entry State");
	m_pAnyState = new CAnimationState;
	m_pAnyState->m_pAnimator = m_pAnimator;
	m_pAnyState->m_pController = this;
	m_pAnyState->SetStateName("Any State");
	m_pExitState = new CAnimationState;
	m_pExitState->m_pAnimator = m_pAnimator;
	m_pExitState->m_pController = this;
	m_pExitState->SetStateName("Exit State");

	m_pCurrentState = m_pEntryState;
}

CAnimationStateController::~CAnimationStateController()
{
	SAFE_DELETE(m_pEntryState);
	SAFE_DELETE(m_pAnyState);
	SAFE_DELETE(m_pExitState);

	unordered_map<string, CAnimationState*>::iterator iter;
	unordered_map<string, CAnimationState*>::iterator iterEnd = m_mapState.end();

	for (iter = m_mapState.begin(); iter != iterEnd; ++iter)
	{
		delete iter->second;
	}
	m_mapState.clear();
		
	Safe_Delete_Map(m_mapParameter);
}

void CAnimationStateController::SetTrigger(const string & strKey)
{
	unordered_map<string, PPARAMETER>::iterator iter = m_mapParameter.find(strKey);

	if (iter == m_mapParameter.end())
	{
		return;
	}

	if (iter->second->eType == PT_TRIGGER)
	{
		iter->second->bValue = true;
	}
	else
	{
		return;
	}
}

void CAnimationStateController::SetBool(const string & strKey, bool bValue)
{
	unordered_map<string, PPARAMETER>::iterator iter = m_mapParameter.find(strKey);

	if (iter == m_mapParameter.end())
	{		
		return;
	}

	if (iter->second->eType == PT_BOOL)
	{
		iter->second->bValue = bValue;
	}
	else
	{
		return;
	}
}

void CAnimationStateController::SetInteger(const string & strKey, int iValue)
{
	unordered_map<string, PPARAMETER>::iterator iter = m_mapParameter.find(strKey);

	if (iter == m_mapParameter.end())
	{
		return;
	}

	if (iter->second->eType == PT_INT)
	{
		iter->second->iValue = iValue;
	}
	else
	{
		return;
	}
}

void CAnimationStateController::SetFloat(const string & strKey, float fValue)
{
	unordered_map<string, PPARAMETER>::iterator iter = m_mapParameter.find(strKey);

	if (iter == m_mapParameter.end())
	{
		return;
	}

	if (iter->second->eType == PT_FLOAT)
	{
		iter->second->fValue = fValue;
	}
	else
	{
		return;
	}
}

bool CAnimationStateController::GetBool(const string & strKey) const
{
	unordered_map<string, PPARAMETER>::const_iterator iter = m_mapParameter.find(strKey);

	if (iter == m_mapParameter.end())
	{
		return false;
	}

	if (iter->second->eType == PT_BOOL)
	{
		return iter->second->bValue;
	}
	else
	{
		return false;
	}
}

int CAnimationStateController::GetInteger(const string & strKey) const
{
	unordered_map<string, PPARAMETER>::const_iterator iter = m_mapParameter.find(strKey);

	if (iter == m_mapParameter.end())
	{
		return 0;
	}

	if (iter->second->eType == PT_INT)
	{
		return iter->second->iValue;
	}
	else
	{
		return false;
	}
}

float CAnimationStateController::GetFloat(const string & strKey) const
{
	unordered_map<string, PPARAMETER>::const_iterator iter = m_mapParameter.find(strKey);

	if (iter == m_mapParameter.end())
	{
		return 0;
	}

	if (iter->second->eType == PT_INT)
	{
		return iter->second->fValue;
	}
	else
	{
		return false;
	}
}

CAnimationState * CAnimationStateController::GetCurrentState() const
{
	return m_pCurrentState;
}

CAnimator * CAnimationStateController::GetAnimator() const
{
	return m_pAnimator;
}

void CAnimationStateController::SetEntryState(const string & strKey)
{
	CAnimationState* pState = GetAnimationState(strKey);
	
	if (pState)
	{
		m_pEntryState->ClearTransition();
		m_pEntryState->AddTransition(pState);		
	}
}

PPARAMETER CAnimationStateController::AddParameter(const string & strKey, PARAMETER_TYPE eType)
{
	unordered_map<string, PPARAMETER>::iterator iter = m_mapParameter.find(strKey);

	if (iter == m_mapParameter.end())
	{
		PPARAMETER pParam = new PARAMETER;
		pParam->strKey = strKey;
		pParam->eType = eType;
		m_mapParameter.insert(make_pair(strKey, pParam));
		return pParam;
	}
}

PPARAMETER CAnimationStateController::FindParameter(const string & strKey)
{
	unordered_map<string, PPARAMETER>::iterator iter = m_mapParameter.find(strKey);

	if (iter != m_mapParameter.end())
	{
		return (*iter).second;
	}
	return NULL;
}

void CAnimationStateController::RemoveParameter(const string & strKey)
{
	unordered_map<string, PPARAMETER>::iterator iter = m_mapParameter.find(strKey);

	if (iter != m_mapParameter.end())
	{	
		SAFE_DELETE(iter->second);
		m_mapParameter.erase(iter);
	}
}

const unordered_map<string, class CAnimationState*>& CAnimationStateController::GetAnimationState() const
{
	return m_mapState;
}

const unordered_map<string, PPARAMETER>& CAnimationStateController::GetParameter() const
{
	return m_mapParameter;
}

CAnimationState * CAnimationStateController::CreateAnimationState(const string & strStateName)
{
	CAnimationState* pState = GetAnimationState(strStateName);

	if (pState)
		return pState;

	pState = new CAnimationState;
	pState->m_pController = this;
	pState->m_pAnimator = m_pAnimator;
	pState->m_strStateName = strStateName;

	m_mapState.insert(make_pair(strStateName, pState));

	return pState;
}

CAnimationState * CAnimationStateController::CreateAnimationState(const string & strStateName, const string& strClipName, size_t iStartFrameIndex, size_t iEndFrameIndex)
{
	CAnimationState* pState = CreateAnimationState(strStateName);
	pState->SetClip(strClipName, iStartFrameIndex, iEndFrameIndex);

	return pState;

}

CAnimationState * CAnimationStateController::GetAnimationState(const string & strTag)
{
	if (strTag == "Entry State")
	{
		return m_pEntryState;
	}
	else if (strTag == "Any State")
	{
		return m_pAnyState;
	}
	else if (strTag == "Exit State")
	{
		return m_pExitState;
	}
	else
	{
		unordered_map<string, CAnimationState*>::iterator iter = m_mapState.find(strTag);

		if (iter == m_mapState.end())
		{
			return NULL;
		}
		return iter->second;
	}
}

void CAnimationStateController::EraseAnimationState(const string & strTag)
{
	unordered_map<string, CAnimationState*>::iterator iter = m_mapState.find(strTag);

	if (iter == m_mapState.end())
		return;

	m_mapState.erase(iter);
}

void CAnimationStateController::TransitionState(const string & strKey)
{
	CAnimationState* pState = GetAnimationState(strKey);
	TransitionState(pState);
}

void CAnimationStateController::	TransitionState(CAnimationState * pTargetState)
{
	if (pTargetState)
	{
		if (m_pCurrentState)
		{
			m_pCurrentState->ResetState();
		}
		m_pCurrentState = pTargetState;
		m_pCurrentState->ResetState();
		
		if (m_pCurrentState->GetClip())
		{
			m_pCurrentState->GetClip()->ResetClip();
		}

		m_pAnimator->SetAnimationStateTransitioned();
	}
}

void CAnimationStateController::TransitionState(const string & strKey, float fProgressOffset)
{
	CAnimationState* pState = GetAnimationState(strKey);
	TransitionState(pState);
	//이후 CurrentState에는 Transition 된 State가 들어있게 된다.
	m_pCurrentState->GetClip()->SetProgressOffset(fProgressOffset);
}

void CAnimationStateController::TransitionState(CAnimationState * pTargetState, float fProgressOffset)
{
	TransitionState(pTargetState);
	//이후 CurrentState에는 Transition 된 State가 들어있게 된다.
	m_pCurrentState->GetClip()->SetProgressOffset(fProgressOffset);
}

void CAnimationStateController::Save(FILE * pFile)
{
	//State 갯수 저장
	size_t iStateCount = m_mapState.size();
	fwrite(&iStateCount, sizeof(size_t), 1, pFile);

	//Animstion State 저장
	unordered_map<string, CAnimationState*>::iterator iter;
	unordered_map<string, CAnimationState*>::iterator iterEnd = m_mapState.end();
	for (iter = m_mapState.begin(); iter != iterEnd; ++iter)
	{
		size_t iLength = iter->first.length();
		fwrite(&iLength, sizeof(size_t), 1, pFile);
		fwrite(iter->first.c_str(), sizeof(char), iLength, pFile);
		iter->second->Save(pFile);
	}

	//Parameter 저장

	size_t iParameterCount = m_mapParameter.size();
	fwrite(&iParameterCount, sizeof(size_t), 1, pFile);

	unordered_map<string, PPARAMETER>::iterator iterParam;
	unordered_map<string, PPARAMETER>::iterator iterParamEnd = m_mapParameter.end();
	for (iterParam = m_mapParameter.begin(); iterParam != iterParamEnd; ++iterParam)
	{
		size_t iLength = iterParam->first.length();

		fwrite(&iLength, sizeof(size_t), 1, pFile);
		fwrite(iterParam->first.c_str(), sizeof(char), iLength, pFile);

		iLength = iterParam->second->strKey.length();
		fwrite(&iLength, sizeof(size_t), 1, pFile);
		fwrite(iterParam->second->strKey.c_str(), sizeof(char), iLength, pFile);

		fwrite(&iterParam->second->eType, sizeof(PARAMETER_TYPE), 1, pFile);
		fwrite(&iterParam->second->bValue, sizeof(bool), 1, pFile);
		fwrite(&iterParam->second->iValue, sizeof(int), 1, pFile);
		fwrite(&iterParam->second->fValue, sizeof(float), 1, pFile);
	}


	//Transition 저장
	//Load시에 어느 State의 Transition인지 바로는 특정할 수 없으므로, 전체 갯수를 저장후 
	//Save된 Transition 전체를 돌면서 State를 찾아간 뒤, 추가하도록 한다.

	size_t iTotalTransitionCount = 0;
	for (iter = m_mapState.begin(); iter != iterEnd; ++iter)
	{
		iTotalTransitionCount += iter->second->GetTransitionCount();
	}

	fwrite(&iTotalTransitionCount, sizeof(size_t), 1, pFile);
	for (iter = m_mapState.begin(); iter != iterEnd; ++iter)
	{
		size_t iTransitionCount = iter->second->GetTransitionCount();
		
		for (size_t i = 0; i < iTransitionCount; ++i)
		{
			//State Key 저장
			size_t iLength = iter->first.length();
			fwrite(&iLength, sizeof(size_t), 1, pFile);
			fwrite(iter->first.c_str(), sizeof(char), iLength, pFile); 
			iter->second->m_vecTransition[i]->Save(pFile);
		}
	}

	//Entry State Transition Info
	{
		size_t iTransitionCount = m_pEntryState->GetTransitionCount();
		fwrite(&iTransitionCount, sizeof(size_t), 1, pFile);
		for (size_t i = 0; i < iTransitionCount; ++i)
		{
			m_pEntryState->m_vecTransition[i]->Save(pFile);
		}
	}	

	//Any State Transition Info
	{
		size_t iTransitionCount = m_pAnyState->GetTransitionCount();
		fwrite(&iTransitionCount, sizeof(size_t), 1, pFile);
		for (size_t i = 0; i < iTransitionCount; ++i)
		{
			m_pAnyState->m_vecTransition[i]->Save(pFile);
		}
	}

	//Exit State Transition Info
	{ 
		size_t iTransitionCount = m_pExitState->GetTransitionCount();
		fwrite(&iTransitionCount, sizeof(size_t), 1, pFile);
		for (size_t i = 0; i < iTransitionCount; ++i)
		{
			m_pExitState->m_vecTransition[i]->Save(pFile);
		}
	}
}

void CAnimationStateController::Load(FILE * pFile)
{
	//State 갯수 저장
	size_t iStateCount = 0;
	fread(&iStateCount, sizeof(size_t), 1, pFile);

	//Animstion State 저장
	for (size_t i = 0; i < iStateCount; ++i)
	{
		size_t iLength = 0;
		char cBuffer[256] = {};
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(cBuffer, sizeof(char), iLength, pFile);
		
		CAnimationState* pState = new CAnimationState;
		pState->m_pAnimator = m_pAnimator;
		pState->m_pController = this;		
		pState->m_strStateName = cBuffer;
		
		m_mapState.insert(make_pair(pState->m_strStateName, pState));
		pState->Load(pFile);
	}

	//Parameter 저장
	size_t iParameterCount = 0;
	fread(&iParameterCount, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iParameterCount; ++i)
	{

		size_t iLength = 0;
		char cBuffer[256] = {};

		//Map Key
		string strKey;
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(cBuffer, sizeof(char), iLength, pFile);
		strKey = cBuffer;
		
		iLength = 0;
		memset(cBuffer, 0, sizeof(char) * 256);

		PPARAMETER pParam = new PARAMETER;
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(cBuffer, sizeof(char), iLength, pFile);
		fread(&pParam->eType, sizeof(PARAMETER_TYPE), 1, pFile);
		fread(&pParam->bValue, sizeof(bool), 1, pFile);
		fread(&pParam->iValue, sizeof(int), 1, pFile);
		fread(&pParam->fValue, sizeof(float), 1, pFile);
		pParam->strKey = cBuffer;

		m_mapParameter.insert(make_pair(strKey, pParam));
	}


	//Transition 저장
	size_t iTotalTransitionCount = 0;
	fread(&iTotalTransitionCount, sizeof(size_t), 1, pFile);
	for(size_t i = 0 ; i < iTotalTransitionCount; ++i)
	{
		size_t iTransitionCount = 0;

		//State Key 저장
		size_t iLength = 0;
		char cBuffer[256] = {};
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(cBuffer, sizeof(char), iLength, pFile);

		CAnimationState* pBaseState = GetAnimationState(cBuffer);
		if (pBaseState)
		{
			CTransition* pTransition = new CTransition;
			pTransition->m_pAnimator = m_pAnimator;
			pTransition->m_pAnimationController = this;
			pTransition->Load(pFile);
			pBaseState->AddTransition(pTransition);
		}
	}

	//Entry State Transition Info
	{
		size_t iTransitionCount = 0;
		fread(&iTransitionCount, sizeof(size_t), 1, pFile);
		for (size_t i = 0; i < iTransitionCount; ++i)
		{
			CTransition* pTransition = new CTransition;
			pTransition->m_pAnimator = m_pAnimator;
			pTransition->m_pAnimationController = this;
			pTransition->Load(pFile);
			m_pEntryState->AddTransition(pTransition);
		}
	}

	//Any State Transition Info
	{
		size_t iTransitionCount = 0;
		fread(&iTransitionCount, sizeof(size_t), 1, pFile);
		for (size_t i = 0; i < iTransitionCount; ++i)
		{
			CTransition* pTransition = new CTransition;
			pTransition->m_pAnimator = m_pAnimator;
			pTransition->m_pAnimationController = this;
			pTransition->Load(pFile);
			m_pAnyState->AddTransition(pTransition);
		}
	}

	//Exit State Transition Info
	{
		size_t iTransitionCount = 0;
		fread(&iTransitionCount, sizeof(size_t), 1, pFile);
		for (size_t i = 0; i < iTransitionCount; ++i)
		{
			CTransition* pTransition = new CTransition;
			pTransition->m_pAnimator = m_pAnimator;
			pTransition->m_pAnimationController = this;
			pTransition->Load(pFile);
			m_pExitState->AddTransition(pTransition);
		}
	}
}

void CAnimationStateController::Reset()
{
	unordered_map<string, PPARAMETER>::iterator iter;
	unordered_map<string, PPARAMETER>::iterator iterEnd = m_mapDefaultParameter.end();
	for (iter = m_mapDefaultParameter.begin(); iter != iterEnd; ++iter)
	{
		unordered_map<string, PPARAMETER>::iterator Finder = m_mapParameter.find(iter->first);
		if (Finder != m_mapParameter.end())
		{
			Finder->second->bValue = iter->second->bValue;
			Finder->second->eType = iter->second->eType;
			Finder->second->fValue = iter->second->fValue;
			Finder->second->iValue = iter->second->iValue;
		}
	}

	TransitionState(m_pEntryState, 0.0f);
}

void CAnimationStateController::Start()
{	
	m_mapDefaultParameter.clear();

	unordered_map<string, PPARAMETER>::iterator iter;
	unordered_map<string, PPARAMETER>::iterator iterEnd = m_mapParameter.end();
	for (iter = m_mapParameter.begin(); iter != iterEnd; ++iter)
	{	
		m_mapDefaultParameter.insert(make_pair(iter->first, iter->second));
	}
}

void CAnimationStateController::Update(float fTime)
{
	if (m_pCurrentState)
	{
		if (m_pCurrentState == m_pEntryState)
		{
			CTransition* pTransition = m_pEntryState->GetTransition(0);
			if (pTransition)
			{
				if (pTransition->GetTargetState())
				{
					m_pCurrentState = pTransition->GetTargetState();
				}
			}
		}
		else if (m_pCurrentState == m_pExitState)
		{
			CGameObject* pGameObject = m_pAnimator->GetGameObject();
			pGameObject->Destroy(true);
			SAFE_RELEASE(pGameObject);
		}
		

		if (!CheckAnyState(fTime))
		{
			m_pCurrentState->UpdateCondition(fTime);
		}
	}	
}



bool CAnimationStateController::CheckAnyState(float fTime)
{
	//Any State의 조건을 체크한 뒤, 전환할 지 반환,
	//true이면 CurrentState에서 AnyState의 Transition Target State로 Transtion;

	bool bResult = false;
	vector<CTransition*>vecTransition = m_pAnyState->GetTransition();

	CTransition* pReserve = NULL;

	size_t iTransitionCount = vecTransition.size();
	for (size_t i = 0; i < iTransitionCount; ++i)
	{	
		if (vecTransition[i]->CheckResult())
		{
			bResult = true;
			pReserve = vecTransition[i];
			m_pCurrentState->m_bOnTransition = true;

			m_pCurrentState->m_pReservedTransition = vecTransition[i];

			if (!pReserve->m_bHasExitTime)
			{
				pReserve->m_fExitTime = m_pCurrentState->m_pAnimationClip->GetProgress();
			}
			break;
		}
	}
	

	if (bResult)
	{
		float fTransitionProgress = CMathf::Clamp(pReserve->UpdateTimer(fTime) / pReserve->m_fExitDuration, 0.0f, 1.0f);

		if (fTransitionProgress >= 1.0f)
		{
			float fProgressOffset = pReserve->GetTargetState()->GetClip()->GetProgressAtTime(pReserve->m_fExitDuration);
			TransitionState(pReserve->GetTargetState(), fProgressOffset);
		}
	}
	

	return bResult;
}