#pragma once
#include "Component.h"
#include "../Client.h"

SSS_USING

class CCameraMove :
	public CComponent
{
private:
	friend class CGameObject;

private:
	CCameraMove(const CCameraMove& component);
	CCameraMove();
	~CCameraMove();
	CCameraMove* Clone();
	
private:
	class CGameObject* m_pPlayer;
	float m_fDistance;
	float m_fSpeed;

public:
	void SetPlayer(class CGameObject* pPlayer);
	CGameObject* GetPlayer();
	void SetSpeed(float fSpeed);
	float GetSpeed();

public:
	bool Initialize() override;
	int Update(float fTime) override;

};

