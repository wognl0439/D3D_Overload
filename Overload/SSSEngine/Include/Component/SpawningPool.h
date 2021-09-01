#pragma once
#include "../Component.h"
#include "../GameObject.h"


SSS_BEGIN

typedef SSS_DLL struct _tagPool
{
	string strKey;
	float fInterval;
	uint32_t iMaxCount;
	vector<Vector3> vecRangeInfo; // X,Y :: ���� �� X,Z������ �߽� ��ġ, Z : ������
	wstring wstrPrefabRelativePath; // ������ ������Ʈ�� Prefab �����
	string strPrefabPathKey;
	string strPrefabKey;
	vector<CGameObject*> vecGameObjects; //���� �� ������Ʈ��
	uint32_t iCurrentLoadedCount; // �ε��� ���� ������Ʈ ����
	uint32_t iCurrentSpawnCount;
	vector<Vector4> vecRangeColor; // Debug Color;

	float fTimer;
	_tagPool()
	{
		iCurrentLoadedCount = 0;
		iCurrentSpawnCount = 0;
		fTimer = 0;
	}

	_tagPool(uint32_t iMaxSpawnCount, float fSpawnInterval)
	{
		iCurrentLoadedCount = 0;
		fInterval = fSpawnInterval;
		iMaxCount = fSpawnInterval;
		string strPrefabs; // ������ ������Ʈ���� Prefab ���
		vector<CGameObject*> vecGameObjects; //���� �� ������Ʈ��

		iCurrentSpawnCount = 0;
		fTimer = 0;
	}

}OBJECTPOOL, *POBJECTPOOL;

class SSS_DLL CSpawningPool :
	public CComponent
{
private:
	friend CGameObject;

private:
	class CShader*			m_pShader;
	class CMesh*				m_pMesh;
	class CSampler*			m_pSampler;
	ID3D11InputLayout*	m_pLayout;
	class CRenderState*	m_pRenderState[RS_END];
	Vector4						m_vColliderColor;

private:
	unordered_map<string, POBJECTPOOL> m_mapPool;

public:
	void AddPool(const string& strKey);
	void AddRange(const string& strTargetPool, const Vector2& vXZPlaneCenter, float fRadius, const Vector4& vDebugColor = Vector4(0.0f, 1.0f, 0.0f, 0.375f));
	void SetPrefab(const string& strTargetPool, const string& strPrefabRelativePath, const string& strPrefabPathKey = NO_USE_PATH);
	void SetInterval(const string& strTargetPool, float fInterval);
	void SetMaxSpawnCount(const string& strTargetPool, uint32_t iMax);

public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;
	void RenderDebug(float fTime) override;
private:
	void DrawRange(const Vector3& vCenter, float fRadius, float fTime);

public:
	CSpawningPool();
	~CSpawningPool();

public:
	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;


};

SSS_END