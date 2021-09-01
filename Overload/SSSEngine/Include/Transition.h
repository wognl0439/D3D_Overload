#pragma once
#include "Engine.h"
#include "AnimationStateController.h"

SSS_BEGIN

class SSS_DLL CTransition
{
private:
	friend class CAnimationState;
	friend class CAnimationStateController;
	friend class CAnimator;

public:
	CTransition();
	~CTransition();

private:
	class CAnimator* m_pAnimator;
	class CAnimationStateController* m_pAnimationController;

	class CAnimationState* m_pBaseState;
	class CAnimationState* m_pTargetState;
	vector<PPARAMETER>m_vecParameter;
	vector<PARAMETER_CONDITION> m_vecComparisonCondition;
	vector<PARAMETER> m_vecTransitionValue;
	bool m_bHasExitTime; // false 일 경우 애니메이션이 즉시 변경된다. true일 경우 아래의 환경에 맞춰서 Transition 된다.
	float m_fExitTime; // AnimationState 탈출 시간 (%). 해당 프레임이 되면 조건에 맞는 State로 Transition 된다. 
	float m_fExitDuration; // m_bOnExit == true 일 때 이 시간동안 이전 State와 다음  State의 키프레임을 보간한다. 
	float m_fTimeBeginExit; // Exit이 시작되면 기록 시작

public: 
	vector<PPARAMETER>* GetTargetParameter();
	vector<PARAMETER_CONDITION>* GetOperator();
	vector<PARAMETER>* GetValue();
	size_t GetParameterCount();

	void ResetTransition();
	float GetTransitionProgress() const;
	float GetTimeBeginExit() const;
	void SetExitTime(float fExitTime);
	void SetExitDuration(float fExitDuration);
	void UseExitTime(bool bHasExitTime);
	bool HasExitTime() const;

	float GetExitTime() const;
	float GetExitDuration() const;
	void SetBaseState(class CAnimationState* pState);
	void SetTargetState(class CAnimationState* pState);
	void EditParameterCondition(size_t iIndex, const string& strParameterKeyInController = "", PARAMETER_CONDITION eCompareCondition = PC_DEFAULT, PARAMETER tTargetValue = PARAMETER());
	//void AddParameterCondition(const string& strParameterKeyInController,  PARAMETER_CONDITION eCompareCondition = PC_DEFAULT, PARAMETER tTargetValue = PARAMETER());
	void AddParameterCondition(const string& strParameterKeyInController, PARAMETER_CONDITION eCompareCondition = PC_DEFAULT, bool bValue = false);
	void AddParameterCondition(const string& strParameterKeyInController, PARAMETER_CONDITION eCompareCondition = PC_DEFAULT, int iValue= 0);
	void AddParameterCondition(const string& strParameterKeyInController, PARAMETER_CONDITION eCompareCondition = PC_DEFAULT, float fValue = 0.0f);
	void AddParameterCondition(const string& strParameterKeyInController, bool bTrigger);

	void AddEmptyParameter();
	
	
	bool GetTransitionCondition();
	bool CheckResult();
	class CAnimationState* GetTargetState() const;
	class CAnimationState* GetBaseState() const;

	float UpdateTimer(float fTime);
	void Save(FILE* pFile);
	void Load(FILE* pFile);

};

SSS_END
