#pragma once
#include "UI.h"

SSS_BEGIN

class SSS_DLL CUIPanel : public CUI
{
private:
	friend class CGameObject;

public:
	CUIPanel();
	CUIPanel(const CUIPanel& ui);
	~CUIPanel();

public:
	bool Initialize() override;
	int Update(float fTime) override;
	int LateUpdate(float fTime) override;
	void Render(float fTime) override;
	CUIPanel* Clone()	const override;
};

SSS_END