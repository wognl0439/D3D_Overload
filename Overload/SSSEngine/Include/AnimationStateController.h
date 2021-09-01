#pragma once
#include "Ref.h"

SSS_BEGIN

typedef struct SSS_DLL _tagParameter
{
	string strKey;
	PARAMETER_TYPE eType;
	bool	bValue;
	int		iValue;
	float	fValue;

	_tagParameter() :
		eType(PT_DEFAULT),
		bValue(false),
		iValue(0),
		fValue(0.0f)
	{
	}


	void operator =(bool bVal)
	{
		bValue = bVal;
	}
	void operator = (int iVal)
	{
		iValue = iVal;
	}
	void operator = (float fVal)
	{
		fValue = fVal;
	}
	operator bool() const
	{
		return bValue;
	}
	operator int() const
	{
		return iValue;
	}
	operator float() const
	{
		return fValue;
	}
	bool operator ==(const _tagParameter& target) const
	{
		if (target.eType == eType)
		{
			switch (eType)
			{
			case SSS::PT_BOOL:
				return bValue == target.bValue;
			case SSS::PT_INT:
				return iValue == target.iValue;
			case SSS::PT_FLOAT:
				return fValue == target.fValue;
			default:
				return false;
			}
		}
		return false;
	}
	bool operator !=(const _tagParameter& target) const
	{
		if (target.eType == eType)
		{
			switch (eType)
			{
			case SSS::PT_BOOL:
				return bValue != target.bValue;
			case SSS::PT_INT:
				return iValue != target.iValue;
			case SSS::PT_FLOAT:
				return fValue != target.fValue;
			default:
				return true;
			}
		}
		return true;
	}
	bool operator > (const _tagParameter& target) const
	{
		if (target.eType == eType)
		{
			switch (eType)
			{
			case SSS::PT_INT:
				return iValue > target.iValue;
			case SSS::PT_FLOAT:
				return fValue > target.fValue;
			default:
				return false;
			}
		}
		return false;
	}
	bool operator >= (const _tagParameter& target) const
	{
		if (target.eType == eType)
		{
			switch (eType)
			{
			case SSS::PT_INT:
				return iValue >= target.iValue;
			case SSS::PT_FLOAT:
				return fValue >= target.fValue;
			default:
				return false;
			}
		}
		return false;
	}
	bool operator < (const _tagParameter& target) const
	{
		if (target.eType == eType)
		{
			switch (eType)
			{
			case SSS::PT_INT:
				return iValue < target.iValue;
			case SSS::PT_FLOAT:
				return fValue < target.fValue;
			default:
				return false;
			}
		}
		return false;
	}
	bool operator <= (const _tagParameter& target) const
	{
		if (target.eType == eType)
		{
			switch (eType)
			{
			case SSS::PT_INT:
				return iValue <= target.iValue;
			case SSS::PT_FLOAT:
				return fValue <= target.fValue;
			default:
				return false;
			}
		}
		return false;
	}
	
}PARAMETER, *PPARAMETER;

class SSS_DLL CAnimationStateController
{
private:
	friend class CAnimationState;
	friend class CAnimator;

public:
	CAnimationStateController();
	CAnimationStateController(CAnimator* pAnimator);
	~CAnimationStateController();

private:
	class CAnimator* m_pAnimator;
	unordered_map<string, class CAnimationState*> m_mapState;
	unordered_map<string, PPARAMETER> m_mapParameter;
	unordered_map<string, PPARAMETER> m_mapDefaultParameter;
	class CAnimationState* m_pEntryState;
	class CAnimationState* m_pAnyState;
	class CAnimationState* m_pExitState;
	class CAnimationState* m_pCurrentState;
	
private:
	bool CheckAnyState(float fTime);

public:
	void SetTrigger(const string& strKey);
	void SetBool(const string& strKey, bool bValue);
	void SetInteger(const string& strKey, int iValue);
	void SetFloat(const string& strKey, float fValue);
	bool GetBool(const string& strKey) const;
	int GetInteger(const string& strKey) const;
	float GetFloat(const string& strKey) const;

	class CAnimationState* GetCurrentState() const;
	class CAnimator* GetAnimator() const;
	void SetEntryState(const string& strKey);
	PPARAMETER AddParameter(const string& strKey, PARAMETER_TYPE eType);
	PPARAMETER FindParameter(const string& strKey);
	void RemoveParameter(const string& strKey);
	
public:
	const unordered_map<string, class CAnimationState*>& GetAnimationState() const;
	const unordered_map<string, PPARAMETER>& GetParameter() const;
	class CAnimationState* CreateAnimationState(const string& strStateName);
	class CAnimationState* CreateAnimationState(const string& strStateName, const string& strClipName, size_t iStartFrameIndex, size_t iEndFrameIndex);
	class CAnimationState* GetAnimationState(const string& strTag);
	void EraseAnimationState(const string& strTag);
	void TransitionState(const string& strKey);
	void TransitionState(class CAnimationState* pTargetState);
	void TransitionState(const string& strKey, float fProgressOffset);
	void TransitionState(class CAnimationState* pTargetState, float fProgressOffset);

	void Save(FILE* pFile);
	void Load(FILE* pFile);

public:
	void Reset();
	void Start();
	void Update(float fTime);
};

SSS_END
