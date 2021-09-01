#pragma once
#include "../Component.h"

SSS_BEGIN

//커서는 복사생성하지 않도록 함.
//따라서 항상 레퍼런스 형태로만 전달되도록 한다
class SSS_DLL CCursor :
	public CComponent
{
private:
	friend class CGameObject;	

private:
	CCursor();
	~CCursor();

private:
	bool m_bVisible;

public:
	void SetCursorVisible(bool bVisible);
	bool GetCursorVisible() const;

public:
	bool Initialize() override;
	int LateUpdate(float fTime) override;
};

SSS_END
