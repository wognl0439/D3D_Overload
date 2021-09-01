#pragma once
#include "UI.h"

//조건에 따라서 Callback 등록해서 쓸수 있게.

SSS_BEGIN

enum SSS_DLL BUTTON_OPTION_TYPE
{
	BOT_CHECK,
	BOT_RADIO
};

class SSS_DLL CUICheckBox : public CUI
{
private:
	friend class CGameObject;
	friend class CUIRadioButton;

public:
	CUICheckBox();
	CUICheckBox(const CUICheckBox& checkbutton);
	~CUICheckBox();

private:
	int m_iGroupIndex;
	bool m_bCheck;
	string m_strGroupName;
	BUTTON_OPTION_TYPE m_eOption;

private:
	class CUIRadioButton* m_pRadioGroup;

public:
	void SetGroup(const CUIRadioButton* pGroup);
	void AddGroup(const string& strGroupName);
	void SetOptionType(BUTTON_OPTION_TYPE option);
	void SetCheck(bool Check);

public:
	BUTTON_OPTION_TYPE GetOptionType() const;

public:
	void Index(int index);
	int Index() const;

public:
	bool Initialize() override;
	int Update(float fTime) override;
	int LateUpdate(float fTime) override;
	void Render(float fTime) override;
	CUICheckBox* Clone()	const override;

public:
	//virtual void OnMouseEnter(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;
	//virtual void OnMouseStay(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime) override;
	//virtual void OnMouseExit(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition) override;


	void OnMouseEnter(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition);
	void OnMouseStay(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition, float fTime);
	void OnMouseExit(class CCollider* pTarget, const Vector2& vPosition, const Vector3& vIntersectPosition);

public:
	bool Save(FILE* pFile);
	bool Load(FILE* pFile);
};

SSS_END