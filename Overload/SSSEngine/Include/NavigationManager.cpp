#include "NavigationManager.h"
#include "Component/Tile.h"
#include "Scene.h"
#include "NavigationMesh.h"

SSS_USING

DEFINITION_SINGLE(CNavigationManager)

CNavigationManager::CNavigationManager()
{
}

CNavigationManager::~CNavigationManager()
{
	for (size_t i = 0; i < m_vecStage.size(); ++i)
	{
		Safe_Release_VecList(m_vecStage[i].vecTile);
	}

	Safe_Delete_Map(m_mapNavMesh);
}

float CNavigationManager::GetY(const Vector3 & vPos)
{
	CNavigationMesh*	pMesh = FindNavigationMesh(vPos);

	if (!pMesh)
		return 0.f;

	return pMesh->GetY(vPos);
}

CNavigationMesh * CNavigationManager::CreateNavigationMesh(const string & strKey)
{
	CNavigationMesh*	pNaviMesh = FindNavigationMesh(strKey);

	if (pNaviMesh)
		return pNaviMesh;

	pNaviMesh = new CNavigationMesh;

	if (!pNaviMesh->Init())
	{
		SAFE_DELETE(pNaviMesh);
		return NULL;
	}

	m_mapNavMesh.insert(make_pair(strKey, pNaviMesh));

	return pNaviMesh;
}

CNavigationMesh * CNavigationManager::FindNavigationMesh(const string & strKey)
{
	unordered_map<string, CNavigationMesh*>::iterator	iter = m_mapNavMesh.find(strKey);

	if (iter == m_mapNavMesh.end())
		return NULL;

	return iter->second;
}

CNavigationMesh * CNavigationManager::FindNavigationMesh(const Vector3 & vPos)
{
	unordered_map<string, CNavigationMesh*>::iterator	iter;
	unordered_map<string, CNavigationMesh*>::iterator	iterEnd = m_mapNavMesh.end();

	for (iter = m_mapNavMesh.begin(); iter != iterEnd; ++iter)
	{
		Vector3	vMin = iter->second->GetMin();
		Vector3	vMax = iter->second->GetMax();

		if (vMin.x <= vPos.x && vPos.x <= vMax.x &&
			vMin.y <= vPos.y && vPos.y <= vMax.y &&
			vMin.z <= vPos.z && vPos.z <= vMax.z)
		{
			return iter->second;
		}
	}

	return NULL;
}

const list<Vector3>* CNavigationManager::FindPath(const Vector3 & vStart, const Vector3 & vEnd)
{
	unordered_map<string, class CNavigationMesh*>::iterator	iter;
	unordered_map<string, class CNavigationMesh*>::iterator	iterEnd = m_mapNavMesh.end();

	CNavigationMesh*	pStartMesh = NULL;
	CNavigationMesh*	pEndMesh = NULL;

	for (iter = m_mapNavMesh.begin(); iter != iterEnd; ++iter)
	{
		Vector3	vMin = iter->second->GetMin();
		Vector3	vMax = iter->second->GetMax();

		if (vMin.x <= vStart.x && vStart.x <= vMax.x &&
			vMin.y <= vStart.y && vStart.y <= vMax.y &&
			vMin.z <= vStart.z && vStart.z <= vMax.z)
		{
			pStartMesh = iter->second;
		}

		if (vMin.x <= vEnd.x && vEnd.x <= vMax.x &&
			vMin.y <= vEnd.y && vEnd.y <= vMax.y &&
			vMin.z <= vEnd.z && vEnd.z <= vMax.z)
		{
			pEndMesh = iter->second;
		}
	}

	if (!pStartMesh || !pEndMesh)
		return NULL;

	else if (pStartMesh != pEndMesh)
		return NULL;

	pStartMesh->FindPath(vStart, vEnd);

	return pStartMesh->GetPathList();
}

Vector3 CNavigationManager::IntersectCell(const Vector3 & vOrigin, const Vector3 & vDir)
{
	unordered_map<string, CNavigationMesh*>::iterator	iter;
	unordered_map<string, CNavigationMesh*>::iterator	iterEnd = m_mapNavMesh.end();

	Vector3	vResult;

	for (iter = m_mapNavMesh.begin(); iter != iterEnd; ++iter)
	{
		vResult = iter->second->IntersectCell(vOrigin, vDir);

		if (vResult != Vector3::Zero)
			return vResult;
	}

	return Vector3();
}

bool CNavigationManager::Init()
{
	return true;
}

void CNavigationManager::SetStage(const vector<class CTile*>& vecTile,
	TILE_TYPE eTileType, CScene* pScene,
	int iNumX, int iNumY, const Vector3& vTileScale,
	const Vector3& vStageScale, const Vector3& vStartPos)
{
	STAGE2DINFO		tInfo;

	tInfo.eTileType = eTileType;
	tInfo.iNumX = iNumX;
	tInfo.iNumY = iNumY;
	tInfo.vTileScale = vTileScale;
	tInfo.vStageScale = vStageScale;
	tInfo.vStartPos = vStartPos;
	tInfo.pScene = pScene;

	for (size_t i = 0; i < vecTile.size(); ++i)
	{
		vecTile[i]->AddRef();
		tInfo.vecTile.push_back(vecTile[i]);
	}

	m_vecStage.push_back(tInfo);
}

void CNavigationManager::ChangeScene(CScene * pScene)
{
	vector<STAGE2DINFO>::iterator	iter;
	vector<STAGE2DINFO>::iterator	iterEnd = m_vecStage.end();

	for (iter = m_vecStage.begin(); iter != iterEnd;)
	{
		if ((*iter).pScene != pScene)
		{
			Safe_Release_VecList((*iter).vecTile);
			iter = m_vecStage.erase(iter);
			iterEnd = m_vecStage.end();
		}

		else
			++iter;
	}
}

int CNavigationManager::GetTileIndexX(const Vector3 & vPos)
{
	return GetTileIndexX(vPos.x, vPos.y);
}

int CNavigationManager::GetTileIndexX(float x, float y)
{
	PSTAGE2DINFO	pStage = FindStage(x, y);

	if (!pStage)
		return -1;

	if (pStage->iNumX == 0 || pStage->iNumY == 0 || pStage->vTileScale.x == 0.f ||
		pStage->vTileScale.y == 0.f)
		return -1;

	int	iY = GetTileIndexY(x, y);

	if (iY < 0 || iY >= pStage->iNumY)
		return -1;

	int	idx = -1;

	if (iY % 2 == 0)
		idx = (int)x / (int)pStage->vTileScale.x;

	else
		idx = ((int)x - (int)pStage->vTileScale.x / 2) / (int)pStage->vTileScale.x;

	if (idx < 0 || idx >= pStage->iNumX)
		return -1;

	return idx;
}

int CNavigationManager::GetTileIndexY(const Vector3 & vPos)
{
	return GetTileIndexY(vPos.x, vPos.y);
}

int CNavigationManager::GetTileIndexY(float x, float y)
{
	PSTAGE2DINFO	pStage = FindStage(x, y);

	if (!pStage)
		return -1;

	if (pStage->iNumX == 0 || pStage->iNumY == 0 || pStage->vTileScale.x == 0.f ||
		pStage->vTileScale.y == 0.f)
		return -1;

	// x, y 위치를 타일의 가로, 세로 크기로 나누어서 1, 1 공간으로
	// 변환시켜준다.
	float	fX = x / pStage->vTileScale.x;
	float	fY = y / pStage->vTileScale.y;

	// 사각 타일을 기준으로 이 위치가 어느정도쯤에 있는지 비율을
	// 구해준다.
	fX -= (int)fX;
	fY -= (int)fY;

	int	iXIdx = (int)x / (int)pStage->vTileScale.x;
	int	iYIdx = (int)y / (int)pStage->vTileScale.y;

	// 현재 Y의 비율이 0.5 보다 작다면 사각형의 위쪽 부분에
	// 존재한다는 것이다.
	if (fY < 0.5f)
	{
		// 현재 비율을 이용해서 현재 점이 왼쪽 상단 마름모 밖의
		// 삼각형 안일 경우를 구해준다.
		if (fY < 0.5f - fX)
		{
			if (iYIdx == 0)
				return -1;

			else if (iYIdx == 1)
			{
				if (iXIdx == 0)
					return -1;

				else
					return 1;
			}

			else
			{
				if (iXIdx == 0)
					return -1;

				else
					return (iYIdx - 1) * 3 - (iYIdx - 2);
			}
		}

		// 우상단 일경우
		else if (fY < fX - 0.5f)
		{
			if (iYIdx == 0)
				return -1;

			else if (iYIdx == 1)
			{
				if (iXIdx > pStage->iNumX - 1)
					return -1;

				return 1;
			}

			else
			{
				if (iXIdx > pStage->iNumX - 1)
					return -1;

				return (iYIdx - 1) * 3 - (iYIdx - 2);
			}
		}

		else
		{
			if (iYIdx == 0)
				return 0;

			else if (iYIdx == 1)
				return 2;

			else
				return (iYIdx - 1) * 3 - (iYIdx - 2) + 1;
		}
	}

	// 아래쪽 라인일 경우
	else if (fY > 0.5f)
	{
		// 좌표지점이 좌 하단 지점일 경우
		if (fY - 0.5f > fX)
		{
			if (iYIdx == 0)
			{
				if (iXIdx == 0)
					return -1;

				else
					return 1;
			}

			else if (iYIdx == 1)
			{
				if (iXIdx == 0)
					return -1;

				else
					return 3;
			}

			else
			{
				if (iXIdx == 0)
					return -1;

				return (iYIdx - 1) * 3 - (iYIdx - 2) + 2;
			}
		}

		// 좌표지점이 우 하단 지점일 경우
		else if (fY - 0.5f > 1.f - fX)
		{
			if (iYIdx == 0)
			{
				if (iXIdx > pStage->iNumX - 1)
					return -1;

				else
					return 1;
			}

			else if (iYIdx == 1)
			{
				if (iXIdx > pStage->iNumX - 1)
					return -1;

				else
					return 3;
			}

			else
			{
				if (iXIdx > pStage->iNumX - 1)
					return -1;

				return (iYIdx - 1) * 3 - (iYIdx - 2) + 2;
			}
		}

		else
		{
			if (iYIdx == 0)
				return 0;

			else if (iYIdx == 1)
				return 2;

			else
				return (iYIdx - 1) * 3 - (iYIdx - 2) + 1;
		}
	}

	else
	{
		if (iYIdx == 0)
			return 0;

		else if (iYIdx == 1)
			return 2;

		else
			return (iYIdx - 1) * 3 - (iYIdx - 2) + 1;
	}

	return -1;
}

int CNavigationManager::GetTileIndex(const Vector3 & vPos)
{
	return GetTileIndex(vPos.x, vPos.y);
}

int CNavigationManager::GetTileIndex(float x, float y)
{
	PSTAGE2DINFO	pStage = FindStage(x, y);

	if (!pStage)
		return -1;

	if (pStage->iNumX == 0 || pStage->iNumY == 0 || pStage->vTileScale.x == 0.f ||
		pStage->vTileScale.y == 0.f)
		return -1;

	int	idxX = GetTileIndexX(x, y);
	if (idxX == -1)
		return -1;

	int	idxY = GetTileIndexY(x, y);
	if (idxY == -1)
		return -1;

	return idxY * pStage->iNumX + idxX;
}

CTile * CNavigationManager::GetTile(const Vector3 & vPos)
{
	return GetTile(vPos.x, vPos.y);
}

CTile * CNavigationManager::GetTile(float x, float y)
{
	PSTAGE2DINFO	pStage = FindStage(x, y);

	if (!pStage)
		return NULL;

	int	idx = GetTileIndex(x, y);

	if (idx == -1)
		return NULL;

	return pStage->vecTile[idx];
}

CTile * CNavigationManager::GetTile(int iStage, int x, int y)
{
	if (iStage < 0 || iStage >= m_vecStage.size())
		return NULL;

	else if (x < 0 || x >= m_vecStage[iStage].iNumX)
		return NULL;

	else if (y < 0 || y >= m_vecStage[iStage].iNumY)
		return NULL;

	int	idx = y * m_vecStage[iStage].iNumX + x;
	return m_vecStage[iStage].vecTile[idx];
}

CTile * CNavigationManager::GetTile(int iStage, int iIndex)
{
	if (iStage < 0 || iStage >= m_vecStage.size())
		return NULL;

	return m_vecStage[iStage].vecTile[iIndex];
}

int CNavigationManager::GetStage(const Vector3 & vPos)
{
	return GetStage(vPos.x, vPos.y);
}

int CNavigationManager::GetStage(float x, float y)
{
	PSTAGE2DINFO	pInfo = NULL;

	for (size_t i = 0; i < m_vecStage.size(); ++i)
	{
		pInfo = &m_vecStage[i];
		if (pInfo->vStartPos.x <= x && x < pInfo->vStartPos.x + pInfo->vStageScale.x &&
			pInfo->vStartPos.y <= y && y < pInfo->vStartPos.y + pInfo->vStageScale.y)
			return i;
	}

	return -1;
}

TILE_FLAG CNavigationManager::GetTileFlag(const Vector3 & vPos)
{
	return GetTileFlag(vPos.x, vPos.y);
}

TILE_FLAG CNavigationManager::GetTileFlag(float x, float y)
{
	CTile*	pTile = GetTile(x, y);

	if (!pTile)
		return TF_NONE;

	TILE_FLAG	eFlag = pTile->GetTileFlag();

	return eFlag;
}

PSTAGE2DINFO CNavigationManager::FindStage(const Vector3 & vPos)
{
	return FindStage(vPos.x, vPos.y);
}

PSTAGE2DINFO CNavigationManager::FindStage(float x, float y)
{
	PSTAGE2DINFO	pInfo = NULL;

	for (size_t i = 0; i < m_vecStage.size(); ++i)
	{
		pInfo = &m_vecStage[i];
		if (pInfo->vStartPos.x <= x && x < pInfo->vStartPos.x + pInfo->vStageScale.x &&
			pInfo->vStartPos.y <= y && y < pInfo->vStartPos.y + pInfo->vStageScale.y)
			return pInfo;
	}

	return NULL;
}

void CNavigationManager::ClearStageFindInfo(int iStage)
{
	for (size_t i = 0; i < m_vecStage[iStage].vecTile.size(); ++i)
	{
		m_vecStage[iStage].vecTile[i]->SetG(-1);
		m_vecStage[iStage].vecTile[i]->SetH(-1);
		m_vecStage[iStage].vecTile[i]->SetTotal(-1);
		m_vecStage[iStage].vecTile[i]->SetParentIndex(-1);
	}
}
