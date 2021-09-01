#pragma once
#include "../Component.h"

SSS_BEGIN

class SSS_DLL CStage2DIsoTile :
	public CComponent
{
private:
	friend class CGameObject;

protected:
	CStage2DIsoTile();
	CStage2DIsoTile(const CStage2DIsoTile& stage2D);
	~CStage2DIsoTile();

private:
	vector<class CGameObject*>	m_vecTile;
	vector<class CTile*>	m_vecTileCom;
	TILE_TYPE		m_eTileType;
	int		m_iNumX;
	int		m_iNumY;
	Vector3	m_vTileScale;
	Vector3	m_vStageScale;
	Vector3	m_vStartPos;
	bool	m_bTileEditOnOff;
	int		m_iViewMinX;
	int		m_iViewMaxX;
	int		m_iViewMinY;
	int		m_iViewMaxY;

public:
	void EnableTileEdit(bool bEditOn);
	bool CreateTile(int iNumX, int iNumY, const Vector3& vTileScale,
		const Vector3& vStageScale, const Vector3& vStartPos = Vector3::Zero);
	int GetTileIndexX(const Vector3& vPos);
	int GetTileIndexX(float x, float y);
	int GetTileIndexY(const Vector3& vPos);
	int GetTileIndexY(float x, float y);
	int GetTileIndex(const Vector3& vPos);
	int GetTileIndex(float x, float y);
	class CTile* GetTile(const Vector3& vPos);
	class CTile* GetTile(float x, float y);

public:
	virtual bool Initialize();
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual void Render(float fTime);

public:
	bool Save(const wchar_t* pFullPath);
	bool Save(const wchar_t* pFileName, const string& strPathKey);
	bool Save(const char* pFullPath);
	bool Load(const wchar_t* pFullPath);
	bool Load(const wchar_t* pFileName, const string& strPathKey);
	bool Load(const char* pFullPath);
};

SSS_END
