#pragma once
#include "../Component.h"

SSS_BEGIN

enum TILE_ASTARLIST_TYPE
{
	TAT_NONE,
	TAT_OPEN,
	TAT_CLOSE
};

class SSS_DLL CTile :
	public CComponent
{
private:
	friend class CStage2DIsoTile;
	friend class CGameObject;

private:
	CTile();
	~CTile();

private:
	TILE_ASTARLIST_TYPE	m_eAStarType;
	TILE_TYPE		m_eType;
	Vector3			m_vPos;
	Vector3			m_vScale;
	class CMesh*	m_pMesh;
	class CShader*	m_pShader;
	ID3D11InputLayout*	m_pLayout;
	class CRenderState*	m_pDepthDisable;
	Vector4		m_vDebugColor;
	TILE_FLAG	m_eTileFlag;
	float		m_fG;
	float		m_fH;
	float		m_fTotal;
	int			m_iIndexX;
	int			m_iIndexY;
	int			m_iIndex;
	int			m_iParentIndex;

public:
	TILE_ASTARLIST_TYPE GetAStarType()	const;
	Vector3 GetPos()	const;
	Vector3	GetScale()	const;
	int GetIndexX()	const;
	int GetIndexY()	const;
	int GetIndex()	const;
	int GetParentIndex()	const;
	float GetG()	const;
	float GetH()	const;
	float GetTotal()	const;
	TILE_FLAG GetTileFlag()	const;
	TILE_TYPE GetTileType()	const;

public:
	void SetAStarType(TILE_ASTARLIST_TYPE eType);
	void SetIndexX(int iIndexX);
	void SetIndexY(int iIndexY);
	void SetIndex(int iIndex);
	void SetParentIndex(int iParentIndex);
	void SetG(float fG);
	void SetH(float fH);
	void SetTotal(float fTotal);
	void SetTileFlag(TILE_FLAG eFlag);
	void SetColor(const Vector4& vColor);
	void SetPos(const Vector3& vPos);
	void SetScale(const Vector3& vScale);
	void ComputeTotal();

public:
	virtual bool Initialize();
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual void Render(float fTime);

public:
	bool Save(FILE* pFile);
	bool Load(FILE* pFile);
};

SSS_END

