#pragma once
#include "../Component.h"

SSS_BEGIN

class SSS_DLL CUI :
	public CComponent
{

private:
	friend class CGameObject;

protected:
	CUI();
	CUI(const CUI& ui);
	virtual ~CUI();

protected:
	UI_TYPE m_eUIType;
	//bool m_bVisible;
protected:
	void SetUIType(UI_TYPE _type);

public:
	UI_TYPE GetUIType()	const;
	

public:
	bool Initialize() override;
	int Update(float fTime) override;
	int LateUpdate(float fTime) override;
	void Render(float fTime) override;
	CUI* Clone()	const override;

public:
	virtual bool Save(FILE* pFile);
	virtual bool Load(FILE* pFile);

};

SSS_END
