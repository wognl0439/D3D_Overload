#include "Stage2DIsoTile.h"
#include "Tile.h"
#include "../GameObject.h"
#include "../Transform.h"
#include "SpriteRenderer.h"
#include "../NavigationManager.h"
#include "../Device.h"
#include "Camera.h"
#include "../Scene.h"
#include "../PathManager.h"

SSS_USING

CStage2DIsoTile::CStage2DIsoTile()
{
	m_bTileEditOnOff = false;
	SetTag("Stage2DIsoTile");
	m_iViewMinX = 0;
	m_iViewMaxX = 0;
	m_iViewMinY = 0;
	m_iViewMaxY = 0;
}


CStage2DIsoTile::CStage2DIsoTile(const CStage2DIsoTile & stage2D) :
	CComponent(stage2D)
{
	m_bTileEditOnOff = false;
	m_iNumX = stage2D.m_iNumX;
	m_iNumY = stage2D.m_iNumY;
	m_vTileScale = stage2D.m_vTileScale;
	m_vStageScale = stage2D.m_vStageScale;
	m_vStartPos = stage2D.m_vStartPos;
	m_iViewMinX = stage2D.m_iViewMinX;
	m_iViewMaxX = stage2D.m_iViewMaxX;
	m_iViewMinY = stage2D.m_iViewMinY;
	m_iViewMaxY = stage2D.m_iViewMaxY;

	m_vecTileCom.clear();

	for (size_t i = 0; i < stage2D.m_vecTile.size(); ++i)
	{
		CGameObject*	pTile = stage2D.m_vecTile[i]->Clone();

		m_vecTileCom.push_back(pTile->GetComponent<CTile>());

		m_vecTile.push_back(pTile);
	}
}

CStage2DIsoTile::~CStage2DIsoTile()
{
	Safe_Release_VecList(m_vecTileCom);
	Safe_Release_VecList(m_vecTile);
}

void CStage2DIsoTile::EnableTileEdit(bool bEditOn)
{
	m_bTileEditOnOff = bEditOn;
}

bool CStage2DIsoTile::CreateTile(int iNumX, int iNumY,
	const Vector3 & vTileScale, const Vector3& vStageScale,
	const Vector3 & vStartPos)
{
	m_iNumX = iNumX;
	m_iNumY = iNumY;
	m_vTileScale = vTileScale;
	m_vStageScale = vStageScale;
	m_vStartPos = vStartPos;

	m_vecTile.reserve(m_iNumX * m_iNumY);

	for (int i = 0; i < iNumY; ++i)
	{
		for (int j = 0; j < iNumX; ++j)
		{
			CGameObject*	pTileObj = CGameObject::CreateObject(
				"TileObj");

			pTileObj->SetScene(m_pScene);

			CTransform*	pTileTr = pTileObj->GetTransform();

			Vector3	vLineStartPos = vStartPos;

			if (i % 2 == 1)
				vLineStartPos.x += vTileScale.x / 2.f;

			Vector3	vScale = vTileScale;
			Vector3	vPos = vLineStartPos + Vector3(j * vTileScale.x,
				i * (vTileScale.y / 2.f), 0);

			pTileTr->SetWorldScale(vScale);
			pTileTr->SetWorldPosition(vPos);

			SAFE_RELEASE(pTileTr);

			CTile*	pTile = pTileObj->AddComponent<CTile>("Tile");

			pTile->m_eType = TT_ISO;
			pTile->SetPos(vPos);
			pTile->SetScale(vScale);

			pTile->SetIndexX(j);
			pTile->SetIndexY(i);
			pTile->SetIndex(i * iNumX + j);

			m_vecTileCom.push_back(pTile);

			//SAFE_RELEASE(pTile);

			m_vecTile.push_back(pTileObj);
		}
	}

	// 타일을 내비게이션 시스템에 등록한다.
	GET_SINGLE(CNavigationManager)->SetStage(m_vecTileCom, TT_ISO,
		m_pScene, m_iNumX, m_iNumY, m_vTileScale, m_vStageScale,
		m_vStartPos);

	return true;
}

int CStage2DIsoTile::GetTileIndexX(const Vector3 & vPos)
{
	return GetTileIndexX(vPos.x, vPos.y);
}

int CStage2DIsoTile::GetTileIndexX(float x, float y)
{
	if (m_iNumX == 0 || m_iNumY == 0 || m_vTileScale.x == 0.f ||
		m_vTileScale.y == 0.f)
		return -1;

	int	iY = GetTileIndexY(x, y);

	if (iY < 0 || iY >= m_iNumY)
		return -1;

	int	idx = -1;

	if (iY % 2 == 0)
		idx = (int)x / (int)m_vTileScale.x;

	else
		idx = ((int)x - (int)m_vTileScale.x / 2) / (int)m_vTileScale.x;

	if (idx < 0 || idx >= m_iNumX)
		return -1;

	return idx;
}

int CStage2DIsoTile::GetTileIndexY(const Vector3 & vPos)
{
	return GetTileIndexY(vPos.x, vPos.y);
}

int CStage2DIsoTile::GetTileIndexY(float x, float y)
{
	if (m_iNumX == 0 || m_iNumY == 0 || m_vTileScale.x == 0.f ||
		m_vTileScale.y == 0.f)
		return -1;

	// x, y 위치를 타일의 가로, 세로 크기로 나누어서 1, 1 공간으로
	// 변환시켜준다.
	float	fX = x / m_vTileScale.x;
	float	fY = y / m_vTileScale.y;

	// 사각 타일을 기준으로 이 위치가 어느정도쯤에 있는지 비율을
	// 구해준다.
	fX -= (int)fX;
	fY -= (int)fY;

	int	iXIdx = (int)x / (int)m_vTileScale.x;
	int	iYIdx = (int)y / (int)m_vTileScale.y;

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
				if (iXIdx > m_iNumX - 1)
					return -1;

				return 1;
			}

			else
			{
				if (iXIdx > m_iNumX - 1)
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
				if (iXIdx > m_iNumX - 1)
					return -1;

				else
					return 1;
			}

			else if (iYIdx == 1)
			{
				if (iXIdx > m_iNumX - 1)
					return -1;

				else
					return 3;
			}

			else
			{
				if (iXIdx > m_iNumX - 1)
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

int CStage2DIsoTile::GetTileIndex(const Vector3 & vPos)
{
	return GetTileIndex(vPos.x, vPos.y);
}

int CStage2DIsoTile::GetTileIndex(float x, float y)
{
	if (m_iNumX == 0 || m_iNumY == 0 || m_vTileScale.x == 0.f ||
		m_vTileScale.y == 0.f)
		return -1;

	int	idxX = GetTileIndexX(x, y);
	if (idxX == -1)
		return -1;

	int	idxY = GetTileIndexY(x, y);
	if (idxY == -1)
		return -1;

	return idxY * m_iNumX + idxX;
}

CTile * CStage2DIsoTile::GetTile(const Vector3 & vPos)
{
	return GetTile(vPos.x, vPos.y);
}

CTile * CStage2DIsoTile::GetTile(float x, float y)
{
	int	idx = GetTileIndex(x, y);

	if (idx == -1)
		return NULL;

	return m_vecTile[idx]->GetComponent<CTile>();
}

bool CStage2DIsoTile::Initialize()
{
	return true;
}

int CStage2DIsoTile::Update(float fTime)
{
	CTransform*	pCameraTr = m_pScene->GetMainCameraTransform();

	Vector3	vMin = pCameraTr->GetWorldPosition();
	Vector3	vMax = vMin;
	vMax.x += DEVICE_RESOLUTION.iWidth;
	vMax.y += DEVICE_RESOLUTION.iHeight;

	m_iViewMinX = (int)(vMin.x - m_vStartPos.x) / (int)m_vTileScale.x;
	m_iViewMaxX = (int)(vMax.x - m_vStartPos.x) / (int)m_vTileScale.x;
	m_iViewMinY = (int)(vMin.y - m_vStartPos.y) / (int)m_vTileScale.y;
	m_iViewMaxY = (int)(vMax.y - m_vStartPos.y) / (int)m_vTileScale.y;
	m_iViewMinY *= 2;
	m_iViewMaxY *= 2;

	m_iViewMinX = m_iViewMinX < 0 ? 0 : m_iViewMinX;
	m_iViewMinX = m_iViewMinX >= m_iNumX ? m_iNumX - 1 : m_iViewMinX;
	m_iViewMaxX = m_iViewMaxX < 0 ? 0 : m_iViewMaxX;
	m_iViewMaxX = m_iViewMaxX >= m_iNumX ? m_iNumX - 1 : m_iViewMaxX;

	m_iViewMinY = m_iViewMinY < 0 ? 0 : m_iViewMinY;
	m_iViewMinY = m_iViewMinY >= m_iNumY ? m_iNumY - 1 : m_iViewMinY;
	m_iViewMaxY = m_iViewMaxY < 0 ? 0 : m_iViewMaxY;
	m_iViewMaxY = m_iViewMaxY >= m_iNumY ? m_iNumY - 1 : m_iViewMaxY;

	SAFE_RELEASE(pCameraTr);

	/*for (int i = m_iViewMinY; i <= m_iViewMaxY; ++i)
	{
	for (int j = m_iViewMinX; j <= m_iViewMaxX; ++j)
	{
	m_vecTileCom[i * m_iNumX + j]->SetColor(Vector4::Green);
	}
	}*/

	return 0;
}

int CStage2DIsoTile::LateUpdate(float fTime)
{
	return 0;
}

int CStage2DIsoTile::Collision(float fTime)
{
	return 0;
}

void CStage2DIsoTile::Render(float fTime)
{
	if (m_vecTile.empty())
		return;

	if (m_vecTile[0]->GetComponent<CSpriteRenderer>())
	{
		for (int i = m_iViewMinY; i <= m_iViewMaxY; ++i)
		{
			for (int j = m_iViewMinX; j <= m_iViewMaxX; ++j)
			{
				CSpriteRenderer*	pRenderer = m_vecTile[i * m_iNumX + j]->GetComponent<CSpriteRenderer>();

				pRenderer->Render(fTime);

				SAFE_RELEASE(pRenderer);
			}
		}
	}

	if (m_bTileEditOnOff)
	{
		for (int i = m_iViewMinY; i <= m_iViewMaxY; ++i)
		{
			for (int j = m_iViewMinX; j <= m_iViewMaxX; ++j)
			{
				//CTile*	pTile = m_vecTile[i * m_iNumX + j]->FindComponentFromType<CTile>(CT_TILE);

				//pTile->Render(fTime);

				m_vecTileCom[i * m_iNumX + j]->Render(fTime);
				m_vecTileCom[i * m_iNumX + j]->SetColor(Vector4::green);

				//SAFE_RELEASE(pTile);
			}
		}
	}
}

bool CStage2DIsoTile::Save(const wchar_t * pFullPath)
{
	char	strPath[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, pFullPath, -1,
		strPath, lstrlenW(pFullPath) * 2, 0, 0);

	return Save(strPath);
}

bool CStage2DIsoTile::Save(const wchar_t * pFileName, const string & strPathKey)
{
	const wchar_t* pPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);

	wstring	strPath;
	if (pPath)
		strPath = pPath;

	strPath += pFileName;

	char	strPath1[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, strPath.c_str(), -1,
		strPath1, strPath.length() * 2, 0, 0);

	return Save(strPath1);
}

bool CStage2DIsoTile::Save(const char * pFullPath)
{
	FILE*	pFile = NULL;

	fopen_s(&pFile, pFullPath, "wb");

	if (!pFile)
		return false;

	// Tag 저장
	int	iLength = m_strTag.length();
	fwrite(&iLength, sizeof(int), 1, pFile);
	fwrite(m_strTag.c_str(), sizeof(char), iLength, pFile);

	/*
	vector<class CTile*>	m_vecTileCom;
	*/
	fwrite(&m_iNumX, sizeof(int), 1, pFile);
	fwrite(&m_iNumY, sizeof(int), 1, pFile);

	fwrite(&m_vTileScale, sizeof(Vector3), 1, pFile);
	fwrite(&m_vStageScale, sizeof(Vector3), 1, pFile);
	fwrite(&m_vStartPos, sizeof(Vector3), 1, pFile);

	fwrite(&m_eTileType, sizeof(TILE_TYPE), 1, pFile);

	for (size_t i = 0; i < m_vecTileCom.size(); ++i)
	{
		m_vecTileCom[i]->Save(pFile);
	}

	fclose(pFile);

	return true;
}

bool CStage2DIsoTile::Load(const wchar_t * pFullPath)
{
	char	strPath[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, pFullPath, -1,
		strPath, lstrlenW(pFullPath) * 2, 0, 0);

	return Load(strPath);
}

bool CStage2DIsoTile::Load(const wchar_t * pFileName, const string & strPathKey)
{
	const wchar_t* pPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);

	wstring	strPath;
	if (pPath)
		strPath = pPath;

	strPath += pFileName;

	char	strPath1[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, strPath.c_str(), -1,
		strPath1, strPath.length() * 2, 0, 0);

	return Load(strPath1);
}

bool CStage2DIsoTile::Load(const char * pFullPath)
{
	//m_bTileEditOnOff = true;

	FILE*	pFile = NULL;

	fopen_s(&pFile, pFullPath, "rb");

	if (!pFile)
		return false;

	// Tag 저장
	int	iLength = 0;
	char	strTag[256] = {};
	fread(&iLength, sizeof(int), 1, pFile);
	fread(strTag, sizeof(char), iLength, pFile);
	m_strTag = strTag;

	/*
	vector<class CTile*>	m_vecTileCom;
	*/
	fread(&m_iNumX, sizeof(int), 1, pFile);
	fread(&m_iNumY, sizeof(int), 1, pFile);

	fread(&m_vTileScale, sizeof(Vector3), 1, pFile);
	fread(&m_vStageScale, sizeof(Vector3), 1, pFile);
	fread(&m_vStartPos, sizeof(Vector3), 1, pFile);

	m_pTransform->SetWorldScale(m_vStageScale);
	m_pTransform->SetWorldPosition(m_vStartPos);

	fread(&m_eTileType, sizeof(TILE_TYPE), 1, pFile);

	for (int i = 0; i < m_iNumY; ++i)
	{
		for (int j = 0; j < m_iNumX; ++j)
		{
			CGameObject*	pTileObj = CGameObject::CreateObject(
				"TileObj");

			pTileObj->SetScene(m_pScene);

			CTransform*	pTileTr = pTileObj->GetTransform();

			Vector3	vLineStartPos = m_vStartPos;

			if (i % 2 == 1)
				vLineStartPos.x += m_vTileScale.x / 2.f;

			Vector3	vScale = m_vTileScale;
			Vector3	vPos = vLineStartPos + Vector3(j * m_vTileScale.x,
				i * (m_vTileScale.y / 2.f), 0);

			pTileTr->SetWorldScale(vScale);
			pTileTr->SetWorldPosition(vPos);

			SAFE_RELEASE(pTileTr);

			CTile*	pTile = pTileObj->AddComponent<CTile>("Tile");

			pTile->Load(pFile);

			m_vecTileCom.push_back(pTile);

			//SAFE_RELEASE(pTile);

			m_vecTile.push_back(pTileObj);
		}
	}

	// 타일을 내비게이션 시스템에 등록한다.
	GET_SINGLE(CNavigationManager)->SetStage(m_vecTileCom, TT_ISO,
		m_pScene, m_iNumX, m_iNumY, m_vTileScale, m_vStageScale,
		m_vStartPos);

	fclose(pFile);

	return true;
}
