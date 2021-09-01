#pragma once
#include "../Component.h"

SSS_BEGIN

class SSS_DLL CTerrain :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CTerrain();
	CTerrain(const CTerrain& Terrain);
	~CTerrain();

private:
	unsigned int	m_iWidth;
	unsigned int	m_iHeight;
	int				m_iSplatCount;
	float			m_fMaxHeight;
	float			m_fMinHeight;
	float			m_fDetailLevel;
	bool			m_bNaviMesh;
	TERRAINCBUFFER	m_tCBuffer;

	class CMeshRenderer* m_pRenderer;
	class CMaterial* m_pMaterial;
	class CMesh* m_pMesh;
	vector<Vector3>	m_vecPos;
	vector<Vector3>	m_vecFaceNormal;
	vector<VERTEXBUMP>	m_vecNowVertex;
	vector<unsigned int>	m_vecIdx;
public:
	Vector2 GetTerrainSize() const;
	bool SaveTerrainData(const wchar_t* pTerrainFileName, const wchar_t* pMeshFileName, const string& strPathKey = TEXTURE_PATH);
	bool LoadTerrainData(const wchar_t* pTerrainFileName, const wchar_t* pMeshFileName, const string& strPathKey = TEXTURE_PATH);

	void SetBaseDiffuseTexture(const string& strKey, const wchar_t* pFileName, const string& strPathKey = TEXTURE_PATH);
	void SetBaseNormalTexture(const string& strKey, const wchar_t* pFileName, const string& strPathKey = TEXTURE_PATH);
	void SetBaseSpecularTexture(const string& strKey, const wchar_t* pFileName, const string& strPathKey = TEXTURE_PATH);

	void AddSplatDiffuseTexture(const string& strMultiTexKey, const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void AddSplatNormalTexture(const string& strMultiTexKey, const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void AddSplatSpecularTexture(const string& strMultiTexKey, const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);
	void AddSplatAlphaTexture(const string& strMultiTexKey, const string& strKey, const vector<wstring>& vecFileName, int iContainer = 0, int iSubset = 0, const string& strPathKey = TEXTURE_PATH);

	void SetSampler();

	Vector2 GetUVFromWorldPosition(const Vector3& vPosition);
	float GetMaxHeight() const;
	void SetMaxHeight(float fHeight);
	float GetMinHeight() const;
	void SetMinHeight(float fHeight);
	void SetSplatCount(int iCount);
	int GetSplatCount() const;
	void SetDetailLevel(int DetailLevel);
	int GetDetailLevel() const;
	void SetNavigationMesh(bool bNavi);
	bool GetNavigationMesh() const;

	bool LoadHeightMap(const string& strKey, float fDetailLevel, const char* pFileName, const string& strPathKey = TEXTURE_PATH);
	void SaveHeightMapForBMP(const wchar_t * pFileName);

	void SaveAlphaTexture(int Count, const wstring& pFileName);
	void LoadAlphaTexture(int Count, const wstring& pFileName);

	void BrushEditHeight(float BrushPower, int BrushDir, float fRadius, const Vector2 & vPosition, const Vector2 & vCenterPosition);
	void BrushEditSplat(float BrushPower, int BrushDir, float Radius, Vector2 UVPos, Vector2 CenterPos, int Count);
	Vector3 GetClickPosition(Vector2 & vCenterPosition);
public:
	void ComputeNormal(vector<VERTEXBUMP>& vecVtx, const vector<UINT>& vecIdx);
	void ComputeTangent(vector<VERTEXBUMP>& vecVtx, const vector<UINT>& vecIdx);

	bool UpdateNormal();
	void SetMouseUV(Vector2 vPos, float Radius);
public:
	void Start() override;
	int Prerender(class CMeshRenderer* pRenderer) override;
	bool Initialize() override;


	bool Save(FILE* pFile);
	bool Load(FILE* pFile);


};

SSS_END