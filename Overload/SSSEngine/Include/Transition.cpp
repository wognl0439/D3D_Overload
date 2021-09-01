#include "Transition.h"
#include "AnimationClip.h"
#include "AnimationState.h"
#include "AnimationStateController.h"
#include "Component\Animator.h"
#include "Mathf.h"


SSS_USING

CTransition::CTransition():
	m_pAnimator(NULL),
	m_pAnimationController(NULL),
	m_pBaseState(NULL),
	m_pTargetState(NULL),
	m_bHasExitTime(false),
	m_fExitTime(1.0f),
	m_fExitDuration(0.0f),
	m_fTimeBeginExit(0.0f)
{
}


CTransition::~CTransition()
{
}

vector<PPARAMETER>* CTransition::GetTargetParameter()
{
	return &m_vecParameter;
}

vector<PARAMETER_CONDITION>* CTransition::GetOperator()
{
	return &m_vecComparisonCondition;
}

vector<PARAMETER>* CTransition::GetValue()
{
	return &m_vecTransitionValue;
}

size_t CTransition::GetParameterCount()
{
	return m_vecParameter.size();
}

void CTransition::ResetTransition()
{
	m_fTimeBeginExit = 0.0f;
}

float CTransition::GetTransitionProgress() const
{
	return CMathf::Clamp(m_fTimeBeginExit / m_fExitDuration, 0.0f, 1.0f);
}

float CTransition::GetTimeBeginExit() const
{
	return m_fTimeBeginExit;
}

void CTransition::SetExitTime(float fExitTime)
{
	m_fExitTime = fExitTime;
}

void CTransition::SetExitDuration(float fExitDuration)
{
	m_fExitDuration = fExitDuration;
}

void CTransition::UseExitTime(bool bHasExitTime)
{
	m_bHasExitTime = bHasExitTime;
}

bool CTransition::HasExitTime() const
{
	return m_bHasExitTime;
}

float CTransition::GetExitTime() const
{
	return m_fExitTime;
}

float CTransition::GetExitDuration() const
{
	return m_fExitDuration;
}

void CTransition::SetBaseState(CAnimationState * pState)
{
	m_pBaseState = pState;
}

void CTransition::SetTargetState(CAnimationState * pState)
{
	m_pTargetState = pState;
}

//iIndex					: Transition 내의 Parameter 벡터 내의 인덱스
//pParameter			: Controller의 Parameter의 주소
//eCompareType		: 변수 타입
//eComparisionCondition : 비교 연산자
//tParameter			: 비교할 값
void CTransition::EditParameterCondition(size_t iIndex, const string & strParameterKeyInController, PARAMETER_CONDITION eCompareCondition, PARAMETER tTargetValue)
{
	if (iIndex >= 0 && iIndex < m_vecParameter.size())
	{
		if (!strParameterKeyInController.empty())
		{
			PPARAMETER pParameter = m_pAnimationController->FindParameter(strParameterKeyInController);
		}
	
		if (eCompareCondition != PC_DEFAULT)
		{
			m_vecComparisonCondition[iIndex] = eCompareCondition;
		}

		if (tTargetValue.bValue != false || tTargetValue.fValue != 0.0f || tTargetValue.iValue != 0)
		{
			m_vecTransitionValue[iIndex] = tTargetValue;
		}
	}
}

//pParameter			: Controller의 Parameter의 주소
//eCompareType		: 변수 타입
//eComparisionCondition : 비교 연산자
//tParameter			: 비교할 값
//void CTransition::AddParameterCondition(const string& strParameterKeyInController, PARAMETER_CONDITION eCompareCondition, PARAMETER tTargetValue)
//{
//	PPARAMETER pParameter = m_pAnimationController->FindParameter(strParameterKeyInController);
//
//	if (!pParameter)
//		return;
//
//	if (pParameter->eType == PT_BOOL)
//	{
//		PARAMETER tParam;
//		tParam.eType = pParameter->eType;
//		tParam.bValue = tTargetValue.bValue;
//		m_vecParameter.push_back(pParameter);
//		m_vecComparisonCondition.push_back(eCompareCondition);
//		m_vecTransitionValue.push_back(tParam);
//	}
//	else if (pParameter->eType == PT_INT)
//	{
//		PARAMETER tParam;
//		tParam.eType = pParameter->eType;
//		tParam.iValue = tTargetValue.iValue;
//		m_vecParameter.push_back(pParameter);
//		m_vecComparisonCondition.push_back(eCompareCondition);
//		m_vecTransitionValue.push_back(tParam);
//	}
//	else if (pParameter->eType == PT_FLOAT)
//	{
//		PARAMETER tParam;
//		tParam.eType = pParameter->eType;
//		tParam.fValue = tTargetValue.fValue;
//		m_vecParameter.push_back(pParameter);
//		m_vecComparisonCondition.push_back(eCompareCondition);
//		m_vecTransitionValue.push_back(tParam);
//	}
//	else
//	{
//		return;
//	}
//
//}

void CTransition::AddParameterCondition(const string & strParameterKeyInController, PARAMETER_CONDITION eCompareCondition, bool bValue)
{
	PPARAMETER pParameter = m_pAnimationController->FindParameter(strParameterKeyInController);

	if (!pParameter)
		return;

	if (pParameter->eType == PT_BOOL)
	{
		PARAMETER tParam;
		tParam.eType = pParameter->eType;
		tParam.bValue = bValue;
		m_vecParameter.push_back(pParameter);
		m_vecComparisonCondition.push_back(eCompareCondition);
		m_vecTransitionValue.push_back(tParam);
	}
}

void CTransition::AddParameterCondition(const string & strParameterKeyInController, PARAMETER_CONDITION eCompareCondition, int iValue)
{
	PPARAMETER pParameter = m_pAnimationController->FindParameter(strParameterKeyInController);

	if (!pParameter)
		return;

	if (pParameter->eType == PT_INT)
	{
		PARAMETER tParam;
		tParam.eType = pParameter->eType;
		tParam.iValue = iValue;
		m_vecParameter.push_back(pParameter);
		m_vecComparisonCondition.push_back(eCompareCondition);
		m_vecTransitionValue.push_back(tParam);
	}
}

void CTransition::AddParameterCondition(const string & strParameterKeyInController, PARAMETER_CONDITION eCompareCondition, float fValue)
{
	PPARAMETER pParameter = m_pAnimationController->FindParameter(strParameterKeyInController);

	if (!pParameter)
		return;

	if (pParameter->eType == PT_FLOAT)
	{
		PARAMETER tParam;
		tParam.eType = pParameter->eType;
		tParam.fValue = fValue;
		m_vecParameter.push_back(pParameter);
		m_vecComparisonCondition.push_back(eCompareCondition);
		m_vecTransitionValue.push_back(tParam);
	}
}

void CTransition::AddParameterCondition(const string & strParameterKeyInController, bool bTrigger)
{

	PPARAMETER pParameter = m_pAnimationController->FindParameter(strParameterKeyInController);

	if (!pParameter)
		return;

	if (pParameter->eType == PT_TRIGGER)
	{
		PARAMETER tParam;
		tParam.eType = pParameter->eType;
		tParam.bValue = true;
		m_vecParameter.push_back(pParameter);
		m_vecComparisonCondition.push_back(PC_EQUALS);
		m_vecTransitionValue.push_back(tParam);
	}
}

void CTransition::AddEmptyParameter()
{
	PPARAMETER pParam = NULL;
	PARAMETER_CONDITION eCondition = PC_DEFAULT;
	PARAMETER tValue;

	m_vecParameter.push_back(pParam);
	m_vecComparisonCondition.push_back(eCondition);
	m_vecTransitionValue.push_back(tValue);
}



	
//Transition가능		: True
//Transition불가능	: False
bool CTransition::GetTransitionCondition()
{
	if (m_bHasExitTime)
	{
		if (m_fExitTime <= m_pBaseState->GetClip()->GetProgress())
		{
			return CheckResult();
		}
		else
		{
			return false;
		}
	}
	else
	{
		return CheckResult();
	}	
}

bool CTransition::CheckResult()
{
	bool bResult = true;
	for (size_t i = 0; i < m_vecParameter.size(); ++i)
	{
		if (m_vecParameter[i])
		{
			switch (m_vecParameter[i]->eType)
			{
			case SSS::PT_BOOL:
			{
				switch (m_vecComparisonCondition[i])
				{
				case SSS::PC_EQUALS:
				{
					bResult &= m_vecParameter[i]->bValue == m_vecTransitionValue[i].bValue;
				}break;
				case SSS::PC_NOT_EQUAL:
				{
					bResult &= m_vecParameter[i]->bValue != m_vecTransitionValue[i].bValue;
				}break;
				}
			}break;
			case SSS::PT_INT:
			{
				switch (m_vecComparisonCondition[i])
				{
				case SSS::PC_GRATER:
				{
					bResult &= m_vecParameter[i]->iValue > m_vecTransitionValue[i].iValue;
				}break;
				case SSS::PC_GRATER_EQUAL:
				{
					bResult &= m_vecParameter[i]->iValue >= m_vecTransitionValue[i].iValue;
				}break;
				case SSS::PC_LESS:
				{
					bResult &= m_vecParameter[i]->iValue < m_vecTransitionValue[i].iValue;
				}break;
				case SSS::PC_LESS_EQUAL:
				{
					bResult &= m_vecParameter[i]->iValue <= m_vecTransitionValue[i].iValue;
				}break;
				case SSS::PC_EQUALS:
				{
					bResult &= m_vecParameter[i]->iValue == m_vecTransitionValue[i].iValue;
				}break;
				case SSS::PC_NOT_EQUAL:
				{
					bResult &= m_vecParameter[i]->iValue != m_vecTransitionValue[i].iValue;
				}break;
				}
			}break;
			case SSS::PT_FLOAT:
			{
				switch (m_vecComparisonCondition[i])
				{
				case SSS::PC_GRATER:
				{
					bResult &= m_vecParameter[i]->fValue > m_vecTransitionValue[i].fValue;
				}break;
				case SSS::PC_GRATER_EQUAL:
				{
					bResult &= m_vecParameter[i]->fValue >= m_vecTransitionValue[i].fValue;
				}break;
				case SSS::PC_LESS:
				{
					bResult &= m_vecParameter[i]->fValue < m_vecTransitionValue[i].fValue;
				}break;
				case SSS::PC_LESS_EQUAL:
				{
					bResult &= m_vecParameter[i]->fValue <= m_vecTransitionValue[i].fValue;
				}break;
				case SSS::PC_EQUALS:
				{
					bResult &= m_vecParameter[i]->fValue == m_vecTransitionValue[i].fValue;
				}break;
				case SSS::PC_NOT_EQUAL:
				{
					bResult &= m_vecParameter[i]->fValue != m_vecTransitionValue[i].fValue;
				}break;
				}
			}break;
			case SSS::PT_TRIGGER:
			{
				if (m_vecParameter[i]->bValue == true)
				{
					bResult &= true;
					m_vecParameter[i]->bValue = false;
				}
				else
				{
					bResult &= false;
				}
			}break;			
			}
		}
	}

	return bResult;
}

CAnimationState * CTransition::GetTargetState() const
{
	return m_pTargetState;
}

CAnimationState * CTransition::GetBaseState() const
{
	return m_pBaseState;
}

float CTransition::UpdateTimer(float fTime)
{
	m_fTimeBeginExit += fTime;

	return m_fTimeBeginExit;
}

void CTransition::Save(FILE * pFile)
{
	string strBaseState = m_pBaseState->GetStateName();
	string strTargetState = m_pTargetState->GetStateName();

	size_t iLength = strBaseState.length();	
	fwrite(&iLength, sizeof(size_t), 1, pFile);
	fwrite(strBaseState.c_str(), sizeof(char), iLength, pFile);

	iLength = strTargetState.length();
	fwrite(&iLength, sizeof(size_t), 1, pFile);
	fwrite(strTargetState.c_str(), sizeof(char), iLength, pFile);
	
	fwrite(&m_bHasExitTime, sizeof(bool), 1, pFile);
	fwrite(&m_fExitTime, sizeof(float), 1, pFile);
	fwrite(&m_fExitDuration, sizeof(float), 1, pFile);


	size_t iParameterCount = m_vecParameter.size();
	fwrite(&iParameterCount, sizeof(size_t), 1, pFile);
	
	//Target Parameter 저장
	
	for (size_t i = 0; i < iParameterCount; ++i)
	{
		iLength = m_vecParameter[i]->strKey.length();
		fwrite(&iLength, sizeof(size_t), 1, pFile);
		fwrite(m_vecParameter[i]->strKey.c_str(), sizeof(char), iLength, pFile);
	}
	
	//Operator 저장
	for (size_t i = 0; i < iParameterCount; ++i)
	{
		fwrite(&m_vecComparisonCondition[i], sizeof(PARAMETER_CONDITION), 1, pFile);
	}

	//Value 저장
	for (size_t i = 0; i < iParameterCount; ++i)
	{
		iLength = m_vecTransitionValue[i].strKey.length();
		fwrite(&iLength, sizeof(size_t), 1, pFile);
		fwrite(m_vecTransitionValue[i].strKey.c_str(), sizeof(char), iLength, pFile);

		fwrite(&m_vecTransitionValue[i].eType, sizeof(PARAMETER_TYPE), 1, pFile);
		fwrite(&m_vecTransitionValue[i].bValue, sizeof(bool), 1, pFile);
		fwrite(&m_vecTransitionValue[i].iValue, sizeof(int), 1, pFile);
		fwrite(&m_vecTransitionValue[i].fValue, sizeof(float), 1, pFile);
	}

}

void CTransition::Load(FILE * pFile)
{
	char strBaseState[256] = {};
	char strTargetState[256] = {};

	size_t iLength = 0;
	fread(&iLength, sizeof(size_t), 1, pFile);
	fread(strBaseState, sizeof(char), iLength, pFile);

	iLength = 0;
	fread(&iLength, sizeof(size_t), 1, pFile);
	fread(strTargetState, sizeof(char), iLength, pFile);

	m_pBaseState = m_pAnimationController->GetAnimationState(strBaseState);
	m_pTargetState = m_pAnimationController->GetAnimationState(strTargetState);

	fread(&m_bHasExitTime, sizeof(bool), 1, pFile);
	fread(&m_fExitTime, sizeof(float), 1, pFile);
	fread(&m_fExitDuration, sizeof(float), 1, pFile);

	size_t iParameterCount = 0;
	fread(&iParameterCount, sizeof(size_t), 1, pFile);

	//Target Parameter 저장

	for (size_t i = 0; i < iParameterCount; ++i)
	{
		char strParameterKey[256] = {};
		iLength = 0;
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(strParameterKey, sizeof(char), iLength, pFile);

		PPARAMETER pParameter = m_pAnimationController->FindParameter(strParameterKey);
		m_vecParameter.push_back(pParameter);
	}

	//Operator 저장
	for (size_t i = 0; i < iParameterCount; ++i)
	{
		PARAMETER_CONDITION eOperator = PC_DEFAULT;
		fread(&eOperator, sizeof(PARAMETER_CONDITION), 1, pFile);
		m_vecComparisonCondition.push_back(eOperator);
	}

	//Value 저장
	for (size_t i = 0; i < iParameterCount; ++i)
	{
		PARAMETER tParameter = {};

		char strValueKey[256] = {};
		iLength = 0;
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(strValueKey, sizeof(char), iLength, pFile);
		tParameter.strKey = strValueKey;
		fread(&tParameter.eType, sizeof(PARAMETER_TYPE), 1, pFile);
		fread(&tParameter.bValue, sizeof(bool), 1, pFile);
		fread(&tParameter.iValue, sizeof(int), 1, pFile);
		fread(&tParameter.fValue, sizeof(float), 1, pFile);

		m_vecTransitionValue.push_back(tParameter);
	}
}
