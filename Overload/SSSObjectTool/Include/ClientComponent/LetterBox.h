#pragma once
#include "Component.h"

SSS_USING

class CLetterBox :
	public CComponent
{
	friend class CGameObject;


public:
	CLetterBox();
	~CLetterBox();

private:
	class CGameObject* pBlackBoad_High;
	class CGameObject* pBlackBoad_Low;

	bool bVisible;
	bool bStart;
	float fVisibleTime;
public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;

	void SetVisibleLetterBox(bool start);

	void SlowUpdateLetterBox(float fTime);

};

