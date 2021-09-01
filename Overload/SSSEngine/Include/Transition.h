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
	bool m_bHasExitTime; // false �� ��� �ִϸ��̼��� ��� ����ȴ�. true�� ��� �Ʒ��� ȯ�濡 ���缭 Transition �ȴ�.
	float m_fExitTime; // AnimationState Ż�� �ð� (%). �ش� �������� �Ǹ� ���ǿ� �´� State�� Transition �ȴ�. 
	float m_fExitDuration; // m_bOnExit == true �� �� �� �ð����� ���� State�� ����  State�� Ű�������� �����Ѵ�. 
	float m_fTimeBeginExit; // Exit�� ���۵Ǹ� ��� ����

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
