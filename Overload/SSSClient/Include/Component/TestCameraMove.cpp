#include "TestCameraMove.h"
#include "Transform.h"
#include "Input.h"

CTestCameraMove::CTestCameraMove()
{
}


CTestCameraMove::~CTestCameraMove()
{
}

bool CTestCameraMove::Initialize()
{
	return true;
}

int CTestCameraMove::Update(float fTime)
{
	Vector3 vPosition = m_pTransform->GetWorldPosition();
	
	if (GETKEY(KEY_W))
	{
		m_pTransform->SetWorldPosition(vPosition + Vector3(0.0f, 1.0f, 0.0f)* 1.0f * fTime);
	}
	if (GETKEY(KEY_S))
	{
		m_pTransform->SetWorldPosition(vPosition - Vector3(0.0f, 1.0f, 0.0f)* 1.0f * fTime);
	}
	return 0;
}
