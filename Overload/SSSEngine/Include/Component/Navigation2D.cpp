#include "Navigation2D.h"
#include "../NavigationManager.h"
#include "Tile.h"
#include "../Transform.h"

SSS_USING

CNavigation2D::CNavigation2D()
{
	SetTag("Navigation2D");
#ifdef _DEBUG
	m_bDebug = false;
#endif
	m_fMoveSpeed = 0.f;
	m_bLastPath = false;
	m_bPathEnd = false;
}

CNavigation2D::CNavigation2D(const CNavigation2D & nav) :
	CComponent(nav)
{
	m_fMoveSpeed = nav.m_fMoveSpeed;
	m_bLastPath = false;
	m_bPathEnd = false;
}

CNavigation2D::~CNavigation2D()
{
}

void CNavigation2D::SetMoveSpeed(float fMoveSpeed)
{
	m_fMoveSpeed = fMoveSpeed;
}

bool CNavigation2D::Initialize()
{
	return true;
}

int CNavigation2D::Update(float fTime)
{
	return 0;
}

int CNavigation2D::LateUpdate(float fTime)
{
	if (!m_bPathEnd)
	{
		// ���� ������Ʈ�� ��ġ�� Ÿ���� ���´�.
		TILE_FLAG	eTile = GET_SINGLE(CNavigationManager)->GetTileFlag(m_pTransform->GetWorldPosition());

		switch (eTile)
		{
		case TF_NOMOVE:
		case TF_OBJECT:
			//m_pTransform->Move(-m_pTransform->GetWorldMove());
			break;
		}
	}

	else
	{
		Vector3	vDir = m_vMovePos - m_pTransform->GetWorldPosition();
		vDir = vDir.Normalize();

		//m_pTransform->Move(vDir, m_fMoveSpeed, fTime);

		if (m_vMovePos.Distance(m_pTransform->GetWorldPosition()) < 3.f)
		{
			NAVPATH	tPath = {};
			if (!m_Path.empty())
				tPath = m_Path.top();

			if (m_bLastPath)
			{
				m_bPathEnd = false;
				m_bLastPath = false;
				if (!m_Path.empty())
					m_Path.pop();
			}

			else if (m_Path.size() > 1)
				m_Path.pop();

			else
				m_bLastPath = true;

			if (!m_Path.empty())
			{
				CTile*	pTile = GET_SINGLE(CNavigationManager)->GetTile(tPath.iStage,
					tPath.idx);
				m_vMovePos = pTile->GetPos() + pTile->GetScale() / 2.f;
			}
		}

		// �־��� ��δ�� �̵��Ѵ�.
		//while (!m_Path.empty() && !m_bDebug)
		//{
		//	NAVPATH tPath = m_Path.top();
		//	m_Path.pop();
		//	wchar_t	strPath[32] = {};
		//	wsprintf(strPath, L"%d -> ", tPath.idx);
		//	OutputDebugString(strPath);

		//	//CTile*	pTile = GET_SINGLE(CNavigationManager)->GetTile(tPath.iStage,
		//	//	tPath.idx);
		//	//pTile->SetTileFlag(TF_NORMAL);
		//}

		//if(!m_bDebug)
		//	OutputDebugString(L"\n");

		//if (!m_Path.empty())
		//	m_bDebug = true;
	}

	return 0;
}

int CNavigation2D::Collision(float fTime)
{
	return 0;
}

void CNavigation2D::Render(float fTime)
{
}

bool CNavigation2D::FindRoad(const Vector3 & vStart, const Vector3 & vEnd)
{
	size_t	iSize = m_Path.size();
	for (int i = 0; i < iSize; ++i)
	{
		NAVPATH tPath = m_Path.top();
		CTile*	pTile = GET_SINGLE(CNavigationManager)->GetTile(tPath.iStage,
			tPath.idx);
		pTile->SetTileFlag(TF_NORMAL);
		m_Path.pop();
	}

	// ����Ÿ�ϰ� ���� Ÿ���� ���´�.
	int	iStageIndex = GET_SINGLE(CNavigationManager)->GetStage(vStart);

	// ���� ���ϰ��� �ϴ� ���������� ��� Ÿ���� G, H, total ���� 0���� �ʱ�ȭ
	// ���ش�.
	GET_SINGLE(CNavigationManager)->ClearStageFindInfo(iStageIndex);

	CTile*	pStartTile = GET_SINGLE(CNavigationManager)->GetTile(vStart);
	CTile*	pEndTile = GET_SINGLE(CNavigationManager)->GetTile(vEnd);

	if (!pStartTile || !pEndTile)
	{
		return false;
	}

	else if (pStartTile == pEndTile)
	{
		return false;
	}

	else if (pEndTile->GetTileFlag() == TF_NOMOVE ||
		pEndTile->GetTileFlag() == TF_OBJECT)
	{
		return false;
	}

#ifdef _DEBUG
	m_bDebug = false;
#endif
	m_bPathEnd = false;

	pStartTile->SetParentIndex(-1);

	pStartTile->SetParentIndex(-1);
	pStartTile->SetG(0.f);
	pStartTile->SetH(pStartTile->GetPos().Distance(pEndTile->GetPos()));
	pStartTile->ComputeTotal();
	pStartTile->SetAStarType(TAT_OPEN);

	// ���������� ������Ͽ� �־��ش�.
	m_OpenList.push_back(pStartTile);

	while (!m_OpenList.empty() && !m_bPathEnd)
	{
		// ������Ͽ��� �ϳ��� �����´�.
		CTile*	pNode = m_OpenList.front();

		m_OpenList.pop_front();

		// ������Ͽ� �־��ش�.
		pNode->SetAStarType(TAT_CLOSE);
		m_CloseList.push_back(pNode);

		FindTile(pNode, pEndTile, iStageIndex);
	}

	list<CTile*>::iterator	iter;
	list<CTile*>::iterator	iterEnd = m_OpenList.end();

	for (iter = m_OpenList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->SetG(-1.f);
		(*iter)->SetH(-1.f);
		(*iter)->SetTotal(-1.f);
		(*iter)->SetParentIndex(-1);
		(*iter)->SetAStarType(TAT_NONE);
	}

	iterEnd = m_CloseList.end();

	for (iter = m_CloseList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->SetG(-1.f);
		(*iter)->SetH(-1.f);
		(*iter)->SetTotal(-1.f);
		(*iter)->SetParentIndex(-1);
		(*iter)->SetAStarType(TAT_NONE);
	}

	m_CloseList.clear();
	m_OpenList.clear();

	return true;
}

bool CNavigation2D::FindTile(CTile* pNode, class CTile* pEndTile, int iStage)
{
	int	iLeft, iRight;

	if (pNode->GetIndexY() % 2 == 0)
	{
		iLeft = -1;
		iRight = 0;
	}

	else
	{
		iLeft = 0;
		iRight = 1;
	}

	// �� Ÿ��
	if (!AddOpenNode(iStage, pNode->GetIndexX(), pNode->GetIndexY() - 2,
		pNode, pEndTile, AND_UP))
		return false;

	// ������ Ÿ��
	if (!AddOpenNode(iStage, pNode->GetIndexX() + iLeft, pNode->GetIndexY() - 1,
		pNode, pEndTile, AND_LEFT_UP))
		return false;

	// ������ �� Ÿ��
	if (!AddOpenNode(iStage, pNode->GetIndexX() + iRight, pNode->GetIndexY() - 1,
		pNode, pEndTile, AND_RIGHT_UP))
		return false;

	// ���� Ÿ��
	if (!AddOpenNode(iStage, pNode->GetIndexX() - 1, pNode->GetIndexY(),
		pNode, pEndTile, AND_LEFT))
		return false;

	// ������ Ÿ��
	if (!AddOpenNode(iStage, pNode->GetIndexX() + 1, pNode->GetIndexY(),
		pNode, pEndTile, AND_RIGHT))
		return false;

	// ���� �Ʒ� Ÿ��
	if (!AddOpenNode(iStage, pNode->GetIndexX() + iLeft, pNode->GetIndexY() + 1,
		pNode, pEndTile, AND_LEFT_DOWN))
		return false;

	// ������ �Ʒ� Ÿ��
	if (!AddOpenNode(iStage, pNode->GetIndexX() + iRight, pNode->GetIndexY() + 1,
		pNode, pEndTile, AND_RIGHT_DOWN))
		return false;

	// �Ʒ� Ÿ��
	if (!AddOpenNode(iStage, pNode->GetIndexX(), pNode->GetIndexY() + 2,
		pNode, pEndTile, AND_DOWN))
		return false;

	return true;
}

bool CNavigation2D::AddOpenNode(int iStage, int x, int y, CTile* pNode,
	CTile* pEndTile, ASTAR_NODE_DIR eDir)
{
	CTile*	pFindTile = GET_SINGLE(CNavigationManager)->GetTile(iStage, x, y);

	if (pFindTile &&
		(pFindTile->GetTileFlag() != TF_NOMOVE &&
			pFindTile->GetTileFlag() != TF_OBJECT) &&
		pFindTile->GetAStarType() != TAT_CLOSE)
	{
		int	iParentIndex = pNode->GetIndex();
		float	fG, fH, fTotal;

		//pFindTile->SetParentIndex(pNode->GetIndex());

		if (pFindTile == pEndTile)
		{
			pEndTile->SetG(-1.f);
			pEndTile->SetH(-1.f);
			pEndTile->SetTotal(-1.f);
			pEndTile->SetAStarType(TAT_NONE);

			pFindTile->SetParentIndex(iParentIndex);

			// ���� ���κ��� �θ� Ž���س����鼭 ��θ� ������.
			// ������ �ε����� ���ÿ� �߰��Ѵ�.
			while (pFindTile->GetParentIndex() != -1)
			{
				NAVPATH	tPath = {};
				tPath.iStage = iStage;
				tPath.idx = pFindTile->GetIndex();
				m_Path.push(tPath);
				pFindTile->SetTileFlag(TF_DEBUG_PATH);

				pFindTile = GET_SINGLE(CNavigationManager)->GetTile(iStage,
					pFindTile->GetParentIndex());
			}

			m_bPathEnd = true;
			NAVPATH	tPath = m_Path.top();
			m_Path.pop();
			pFindTile = GET_SINGLE(CNavigationManager)->GetTile(tPath.iStage,
				tPath.idx);
			m_vMovePos = pFindTile->GetPos() + pFindTile->GetScale() / 2.f;

			return false;
		}

		switch (eDir)
		{
		case AND_LEFT_UP:
		case AND_RIGHT_UP:
		case AND_LEFT_DOWN:
		case AND_RIGHT_DOWN:
		{
			float	fW = pFindTile->GetScale().x / 2.f;
			float	fH = pFindTile->GetScale().y / 2.f;
			fG = pNode->GetG() + sqrtf(fW * fW + fH * fH);
		}
		break;
		case AND_LEFT:
		case AND_RIGHT:
			fG = pNode->GetG() + pFindTile->GetScale().x;
			break;
		default:
			fG = pNode->GetG() + pFindTile->GetScale().y;
			break;
		}
		fH = pFindTile->GetPos().Distance(pEndTile->GetPos());
		fTotal = fG + fH;

		// ���� ��� �߿��� ���� ����� �ε����� ���� ����� �ε����� �ִ�����
		// ã�� ���� ��� ��������� ���Ͽ� �� ���� �������� �θ��� �ε�����
		// �������, �� ����� �����Ѵ�.
		if (pFindTile->GetAStarType() == TAT_NONE)
		{
			pFindTile->SetG(fG);
			pFindTile->SetH(fH);
			pFindTile->ComputeTotal();
			pFindTile->SetParentIndex(pNode->GetIndex());
			pFindTile->SetAStarType(TAT_OPEN);
			m_OpenList.push_back(pFindTile);
		}

		else if (pFindTile->GetG() > fG)
		{
			pFindTile->SetG(fG);
			pFindTile->SetH(fH);
			pFindTile->ComputeTotal();
			pFindTile->SetParentIndex(pNode->GetIndex());
		}

		m_OpenList.sort(CNavigation2D::AStarNodeSort);
	}

	return true;
}

bool CNavigation2D::CheckCloseList(int idx)
{
	list<CTile*>::iterator	iter;
	list<CTile*>::iterator	iterEnd = m_CloseList.end();

	for (iter = m_CloseList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetIndex() == idx)
			return true;
	}

	return false;
}

bool CNavigation2D::AStarNodeSort(const CTile* src, const CTile* dest)
{
	return src->GetTotal() < dest->GetTotal();
}
