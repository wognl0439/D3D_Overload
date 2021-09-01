#pragma once

#include "Engine.h"
#include <stack>
SSS_BEGIN

enum NAVIMESH_CELL_LIST_TYPE
{
	NCLT_NONE,
	NCLT_OPEN,
	NCLT_CLOSE
};

typedef struct SSS_DLL _tagAdjInfo
{
	int iIndex;
	int iEdgeIndex;
}ADJINFO, *PADJINFO;

class SSS_DLL CNavigationCell
{
private:
	friend class CNavigationMesh;

private:
	CNavigationCell();

public:
	~CNavigationCell();

private:
	NAVIMESH_CELL_LIST_TYPE	m_eType;
	Vector3		m_vPos[3];
	Vector3		m_vEdge[3];
	Vector3		m_vEdgeCenter[3];
	Vector3		m_vCenter;
	vector<ADJINFO>	m_vecAdj;
	int			m_iIndex;
	int			m_iParentIdx;
	float		m_fG;
	float		m_fH;
	float		m_fTotal;
	bool		m_bEnable;


	void Save(FILE* pFile)
	{
		fwrite(&m_eType, sizeof(NAVIMESH_CELL_LIST_TYPE), 1, pFile);
		fwrite(&m_vPos[0], sizeof(Vector3), 3, pFile);
		fwrite(&m_vEdge[0], sizeof(Vector3), 3, pFile);
		fwrite(&m_vEdgeCenter[0], sizeof(Vector3), 3, pFile);
		fwrite(&m_vCenter, sizeof(Vector3), 1, pFile);

		size_t iAdjCount = m_vecAdj.size();
		fwrite(&iAdjCount, sizeof(size_t), 1, pFile);
		for(size_t i = 0; i < iAdjCount; ++i)
		{
			fwrite(&m_vecAdj[i].iIndex, sizeof(int), 1, pFile);
			fwrite(&m_vecAdj[i].iEdgeIndex, sizeof(int), 1, pFile);
		}


		fwrite(&m_iIndex, sizeof(int), 1, pFile);
		fwrite(&m_iParentIdx, sizeof(int), 1, pFile);
		fwrite(&m_fG, sizeof(float), 1, pFile);
		fwrite(&m_fH, sizeof(float), 1, pFile);
		fwrite(&m_fTotal, sizeof(float), 1, pFile);
		fwrite(&m_bEnable, sizeof(bool), 1, pFile);		
	}

	void Load(FILE* pFile)
	{
		fread(&m_eType, sizeof(NAVIMESH_CELL_LIST_TYPE), 1, pFile);
		fread(&m_vPos[0], sizeof(Vector3), 3, pFile);
		fread(&m_vEdge[0], sizeof(Vector3), 3, pFile);
		fread(&m_vEdgeCenter[0], sizeof(Vector3), 3, pFile);
		fread(&m_vCenter, sizeof(Vector3), 1, pFile);

		size_t iAdjCount = 0;
		fread(&iAdjCount, sizeof(size_t), 1, pFile);
		for (size_t i = 0; i < iAdjCount; ++i)
		{
			ADJINFO tInfo;
			fread(&tInfo.iIndex, sizeof(int), 1, pFile);
			fread(&tInfo.iEdgeIndex, sizeof(int), 1, pFile);
			m_vecAdj.push_back(tInfo);
		}
		
		fread(&m_iIndex, sizeof(int), 1, pFile);
		fread(&m_iParentIdx, sizeof(int), 1, pFile);
		fread(&m_fG, sizeof(float), 1, pFile);
		fread(&m_fH, sizeof(float), 1, pFile);
		fread(&m_fTotal, sizeof(float), 1, pFile);
		fread(&m_bEnable, sizeof(bool), 1, pFile);
	}


};


class SSS_DLL CNavigationMesh
{
private:
	friend class CNavigationManager;

private:
	CNavigationMesh();

public:
	~CNavigationMesh();

private:
	vector<CNavigationCell*>	m_vecCell;
	// ¿­¸°¸ñ·Ï, ´ÝÈù¸ñ·Ï
	vector<CNavigationCell*>	m_OpenList;
	vector<CNavigationCell*>	m_CloseList;
	stack<int>					m_Path;
	list<Vector3>				m_PathList;
	Vector3		m_vMin;
	Vector3		m_vMax;
	bool		m_bFindEnd;
	bool		m_bGrid;
	int			m_iRectCount;

public:
	Vector3 GetMin()	const;
	Vector3 GetMax()	const;
	const list<Vector3>* GetPathList()	const;

public:
	bool Init();
	void AddCell(const Vector3 vPos[3]);
	void AddAdj(int iCellIdx, int iAdjIdx);
	void CreateGridMapAdj(int iLineRectCount);
	void CreateAdj();

public:
	void FindPath(const Vector3& vStart, const Vector3& vEnd);

private:
	void AddOpenList(CNavigationCell* pCell, CNavigationCell* pEndCell,
		const Vector3& vEnd, const Vector3& vStart);
	CNavigationCell* FindGridNavigationCell(const Vector3& vPos);
	CNavigationCell* FindNavigationCell(const Vector3& vPos);
	bool CheckOnEdge(int iSrc, int iDest, const Vector3& vOrigin1, const Vector3& vOrigin2,
		const Vector3& vEdge, float fEdgeLength, int iEdge1, int iEdge2);
	bool CheckPathDir(const Vector3& vSrc1, const Vector3& vSrc2,
		const Vector3& vDest1, const Vector3& vDest2, Vector3& vIntersect);

public:
	void Save(const char* pFileName, const string& strPathKey = DATA_PATH);
	void SaveFromFullPath(const char* pFullPath);
	void Load(const char* pFileName, const string& strPathKey = DATA_PATH);
	void LoadFromFullPath(const char* pFullPath);

public:
	float GetY(const Vector3& vPos);
	Vector3 IntersectCell(const Vector3& vOrigin, const Vector3& vDir);

private:
	bool RayIntersectTriangle(Vector3 rayOrigin, Vector3 rayDir, Vector3 v0,
		Vector3 v1, Vector3 v2, float& t, Vector3& vIntersect);

public:
	static bool Sort(CNavigationCell* pSrc, CNavigationCell* pDest);
};

SSS_END
