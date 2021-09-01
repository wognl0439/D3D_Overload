#include "TestComponent.h"
#include "Debug.h"
#include "Transform.h"
#include "Component/Animator.h"
#include "AnimationStateController.h"
#include "Input.h"


CTestComponent::CTestComponent():
	m_bClicked(false)
{
}


CTestComponent::~CTestComponent()
{
}

bool CTestComponent::Initialize()
{
	CAnimator* pAnimator = GetComponent<CAnimator>();
	CAnimationStateController* pController = pAnimator->GetAnimationController();
	pController->SetBool("Run", false);
	SAFE_RELEASE(pAnimator);
	return true;
}

int CTestComponent::Update(float fTime)
{
	if (GETKEY(MOUSE_RIGHT))
	{
		m_pTransform->SetWorldRotation(m_pTransform->GetWorldRotation() + Vector3(0.0f, PI / 180.0f, 0.0f) * GET_SINGLE(CInput)->GetCursorDeltaPosition(SPACE_DEVICE).x);
	}




	CAnimator* pAnimator = GetComponent<CAnimator>();

	CAnimationStateController* pController = pAnimator->GetAnimationController();

	if (GETKEYDOWN(KEY_1))
	{
		bool bValue = pController->GetBool("Run");
		pController->SetBool("Run", !bValue);
	}
	SAFE_RELEASE(pAnimator);

	//if (GETKEYDOWN('2'))
	//{
	//	pController->SetBool("Attack1", true);
	//}
	//
	//if (GETKEYDOWN('3'))
	//{
	//	pController->SetBool("Attack2", true);
	//}
	//
	//if (GETKEYDOWN('4'))
	//{
	//	pController->SetBool("Damaged1", true);
	//}
	//
	//if (GETKEYDOWN('5'))
	//{
	//	pController->SetBool("Idle", true);
	//}

		//GET_SINGLE(CDebug)->DrawLine(position, position + Vector3(0.0f, 1.0f, 0.0f), Vector4::black);

	return 0;
}
