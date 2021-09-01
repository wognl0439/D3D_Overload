#pragma once

#include "Engine.h"
#include <stack>

SSS_BEGIN

typedef struct _tagStage2DInfo
{
	class CScene*			pScene;
	vector<class CTile*>	vecTile;
	TILE_TYPE				eTileType;
	int						iNumX;
	int						iNumY;
	Vector3					vTileScale;
	Vector3					vStageScale;
	Vector3					vStartPos;
}STAGE2DINFO, *PSTAGE2DINFO;

class SSS_DLL CNavigationManager
{
private:
	vector<STAGE2DINFO>		m_vecStage;
	unordered_map<string, class CNavigationMesh*>	m_mapNavMesh;

public:
	float GetY(const Vector3& vPos);
	CNavigationMesh* CreateNavigationMesh(const string& strKey);
	CNavigationMesh* FindNavigationMesh(const string& strKey);
	CNavigationMesh* FindNavigationMesh(const Vector3& vPos);
	const list<Vector3>* FindPath(const Vector3& vStart, const Vector3& vEnd);
	Vector3 IntersectCell(const Vector3& vOrigin, const Vector3& vDir);

public:
	bool Init();
	void SetStage(const vector<class CTile*>& vecTile,
		TILE_TYPE eTileType, class CScene* pScene,
		int iNumX, int iNumY, const Vector3& vTileScale,
		const Vector3& vStageScale, const Vector3& vStartPos);
	void ChangeScene(class CScene* pScene);
	int GetTileIndexX(const Vector3& vPos);
	int GetTileIndexX(float x, float y);
	int GetTileIndexY(const Vector3& vPos);
	int GetTileIndexY(float x, float y);
	int GetTileIndex(const Vector3& vPos);
	int GetTileIndex(float x, float y);
	class CTile* GetTile(const Vector3& vPos);
	class CTile* GetTile(float x, float y);
	class CTile* GetTile(int iStage, int x, int y);
	class CTile* GetTile(int iStage, int iIndex);
	int GetStage(const Vector3& vPos);
	int GetStage(float x, float y);
	TILE_FLAG GetTileFlag(const Vector3& vPos);
	TILE_FLAG GetTileFlag(float x, float y);
	PSTAGE2DINFO FindStage(const Vector3& vPos);
	PSTAGE2DINFO FindStage(float x, float y);
	void ClearStageFindInfo(int iStage);

	DECLARE_SINGLE(CNavigationManager)
};

SSS_END
