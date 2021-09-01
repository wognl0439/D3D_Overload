#pragma once
#include "UI.h"

SSS_BEGIN

class SSS_DLL CUIRadioButton : public CUI
{
private:
	friend class CGameObject;

protected:
	CUIRadioButton();
	CUIRadioButton(const CUIRadioButton& button);
	~CUIRadioButton();

private:
	string m_strGroupName;

private:
	vector<class CUICheckBox*> m_vButton;
	Vector3 m_vButtonPosition;

public:
	void AddButton();
	void SetGroupName(const string& strName);

public:
	string GetGroupName()const;
	size_t GetGroupSize() const;

public:
	void RadioCheck(int iIndex);

public:
	bool Initialize() override;
	int Update(float fTime) override;
	int LateUpdate(float fTime) override;
	void Render(float fTime) override;
	CUIRadioButton* Clone()	const override;

public:
	bool Save(FILE* pFile);
	bool Load(FILE* pFile);

};

SSS_END