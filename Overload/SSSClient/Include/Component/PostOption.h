#pragma once
#include "Component.h"
#include "Component/PostProcessing.h"
SSS_USING

class CPostOption :
	public CComponent
{
private:
	friend class CGameObject;
public:
	CPostOption();
	~CPostOption();

private:
	bool m_bVisible;
	CPostProcessing* m_pPostProcessing;
	vector<class CRadioButton*> m_vecButton;

private:
	void CreatePanel();
	void SetPanelState(bool bOn);
	void UpdateButton();


public:
	bool Initialize() override;
	void Start() override;
	int Update(float fTime) override;
};

