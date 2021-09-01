#include "Navigation3D.h"
#include "../NavigationManager.h"
#include "../Transform.h"

SSS_USING

CNavigation3D::CNavigation3D()
{
	SetTag("Navigation3D");
	m_bFind = false;
	m_bMove = false;
}


CNavigation3D::CNavigation3D(const CNavigation3D & nav) :
	CComponent(nav)
{
	*this = nav;
	m_bFind = false;
	m_bMove = false;
}

CNavigation3D::~CNavigation3D()
{
}

void CNavigation3D::FindNavigation(const Vector3 & vStart, const Vector3 & vEnd)
{
	m_vStart = vStart;
	m_vEnd = vEnd;

	m_bFind = true;

	m_PathList.clear();

	const list<Vector3>* pPathList = GET_SINGLE(CNavigationManager)->FindPath(vStart, vEnd);

	if (pPathList)
	{
		list<Vector3>::const_iterator	iter;
		list<Vector3>::const_iterator	iterEnd = pPathList->end();

		for (iter = pPathList->begin(); iter != iterEnd; ++iter)
		{
			m_PathList.push_back(*iter);
		}

		m_bMove = true;

		if (!m_PathList.empty())
		{
			m_vTargetPos = m_PathList.front();
			m_PathList.pop_front();
		}
	}
}

void CNavigation3D::FindNavigation(const Vector3 & vStart, const Vector3 & vOrigin, const Vector3 & vDir)
{
	m_vStart = vStart;
	m_vEnd = GET_SINGLE(CNavigationManager)->IntersectCell(vOrigin, vDir);

	m_bFind = true;

	m_PathList.clear();

	const list<Vector3>* pPathList = GET_SINGLE(CNavigationManager)->FindPath(vStart, m_vEnd);

	if (pPathList)
	{
		list<Vector3>::const_iterator	iter;
		list<Vector3>::const_iterator	iterEnd = pPathList->end();

		for (iter = pPathList->begin(); iter != iterEnd; ++iter)
		{
			m_PathList.push_back(*iter);
		}

		m_bMove = true;

		if (!m_PathList.empty())
		{
			m_vTargetPos = m_PathList.front();
			m_PathList.pop_front();
		}
	}
}

bool CNavigation3D::Initialize()
{
	return true;
}

int CNavigation3D::Update(float fTime)
{
	if (m_bMove)
	{
		Vector3	vPos = m_pTransform->GetWorldPosition();
		//m_pTransform->LookAtY(m_vTargetPos);

		Vector3	vDir = m_vTargetPos - vPos;
		vDir = vDir.Normalize();

		//m_pTransform->Move(vDir, 5.f, fTime);

		vPos = m_pTransform->GetWorldPosition();

		// Ÿ�� ��ġ�� ���� �̵��� ��ġ�� �Ÿ��� ���Ѵ�.
		float	fDist = vPos.Distance(m_vTargetPos);

		if (fDist < 0.05f)
		{
			if (m_PathList.empty())
			{
				m_bMove = false;
				m_vTargetPos = Vector3::Zero;
			}

			else
			{
				m_vTargetPos = m_PathList.front();
				m_PathList.pop_front();
			}
		}
	}

	float	fY = GET_SINGLE(CNavigationManager)->GetY(m_pTransform->GetWorldPosition());

	Vector3 WP = m_pTransform->GetWorldPosition();
	WP.y = fY;

	m_pTransform->SetWorldPosition(WP);

	return 0;
}

int CNavigation3D::LateUpdate(float fTime)
{
	return 0;
}

int CNavigation3D::Collision(float fTime)
{
	return 0;
}

void CNavigation3D::Render(float fTime)
{
}
