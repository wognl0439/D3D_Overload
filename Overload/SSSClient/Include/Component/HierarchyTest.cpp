#include "HierarchyTest.h"
#include "Transform.h"
#include "Input.h"


CHierarchyTest::CHierarchyTest()
{
}


CHierarchyTest::~CHierarchyTest()
{
}

bool CHierarchyTest::Initialize()
{
	return true;
}

int CHierarchyTest::Update(float fTime)
{
	if (GETKEY(KEY_W))
	{
		m_pTransform->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * fTime);
	}

	if (GETKEY(KEY_A))
	{
		m_pTransform->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->left() * fTime);
	}

	if (GETKEY(KEY_S))
	{
		m_pTransform->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->back() * fTime);
	}

	if (GETKEY(KEY_D))
	{
		m_pTransform->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->right() * fTime);
	}
	if (GETKEY(KEY_Q))
	{
		m_pTransform->SetWorldRotation(m_pTransform->GetWorldRotation() + m_pTransform->up() * (90.0f * D2R * fTime));
	}
	if (GETKEY(KEY_E))
	{
		m_pTransform->SetWorldRotation(m_pTransform->GetWorldRotation() + m_pTransform->up() * (-90.0f * D2R * fTime));
	}
	if (GETKEY(KEY_1))
	{
		m_pTransform->SetWorldScale(m_pTransform->GetWorldScale() + Vector3(1.0f, 0.0f, 0.0f) * fTime);
	}
	if (GETKEY(KEY_2))
	{
		m_pTransform->SetWorldScale(m_pTransform->GetWorldScale() + Vector3(-1.0f, 0.0f, 0.0f) * fTime);
	}

	CTransform* pChild = m_pTransform->GetChild(0);
	Matrix matLocal = pChild->GetLocalRotationMatrix();
	Vector3 vChildRotation = CTransform::GetAngleFromRotationMatrix(matLocal);

	return 0;
}
