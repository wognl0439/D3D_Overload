#pragma once
#include "Component.h"

SSS_USING

enum POTAL_LOAD_SCENE
{
	PLS_NONE = 15,
	PLS_TERNY_DAY = 0,
	PLS_TERNY_NIGHT,
	PLS_KINGSROAD_DAY,
	PLS_KINGSROAD_NIGHT,
	PLS_BATTLFIELD_DAY
};

//�� ������Ʈ�� ���� ������ ����� �߰� �� �����Ѵ�.
class CGameSystem :
	public CComponent
{
private:
	friend class CGameObject;
public:
	CGameSystem();
	~CGameSystem();

private:
	class CGameObject* m_pPlayer;
	uint32_t m_iVersion;
	uint32_t m_iSceneID;

public:
	void SetSceneID(size_t iID);	
	uint32_t GetSceneID() const;

public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;


	bool Save(FILE* pFile);
	bool Load(FILE* pFile);

};

