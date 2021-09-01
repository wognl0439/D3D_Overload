#pragma once
#include "Ref.h"
#include "AnimationStateController.h"

SSS_BEGIN


class SSS_DLL CAnimationState
{
private:
	friend class CAnimationStateController;
	friend class CAnimator;

private:
	CAnimationState();
	~CAnimationState();

private:
	class CAnimationStateController* m_pController;
	class CAnimator* m_pAnimator;

	string m_strStateName;//Get;Set;

	class CAnimationClip*		m_pAnimationClip; //Get;Set;
	vector<class CTransition*> m_vecTransition; //Get;Set(Intrinsic);
	class CTransition* m_pReservedTransition;//Get;
	bool m_bOnTransition; // Set;
	bool m_bEnd; //Set;
	bool m_bLoop; // Get;Set;
	float m_fSpeed; // Get;Set;

public:
	class CAnimator* GetAnimator() const;
	class CAnimationStateController* GetAnimationController() const;
	//Getter
	bool IsLoop() const;
	bool End() const;
	class CAnimationClip* GetClip() const;
	float GetSpeed() const;
	const string& GetStateName() const;

	//Setter
	bool Loop(bool bUse = true);
	void SetSpeed(float fSpeed);
	void SetStateName(const string& strStateName);


	CAnimationClip* CreateEmptyClip(const string& strClipName);
	void SetClip(const string& strClipName, size_t iStartFrame, size_t iEndFrame);
	void SetClip(const string& strClipName);
	void SetClip(size_t iClipIndexFromMesh);

	size_t GetTransitionCount() const;
	class CTransition* GetTransition(size_t idx);
	const vector<class CTransition*>& GetTransition() const;
	void AddTransition(class CTransition* pTransition);
	class CTransition* AddTransition(class CAnimationState* pTarget);
	class CTransition* AddTransition();

	void RemoveTransition(class CTransition* pTransition);
	void RemoveTransition(class CAnimationState* pTarget);
	void RemoveTransition(size_t idx);
	void ClearTransition();

	void ResetState();

	void UpdateCondition(float fTime);

	void Save(FILE* pFile);
	void Load(FILE* pFile);


};

SSS_END
