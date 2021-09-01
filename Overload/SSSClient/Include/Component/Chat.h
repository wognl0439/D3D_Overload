#pragma once
#include "Component.h"

SSS_USING

enum CHAT_TYPE
{
	CT_PLAYER,
	CT_SYSTEM
};

typedef struct _tagChatData
{
	CHAT_TYPE eType;
	wstring strText;
}CHATDATA, *PCHATDATA;

class CChat :
	public CComponent
{
private:
	friend class CGameObject;

public:
	CChat();
	~CChat();


private:	
	bool m_bOnTranslation;
	Vector2 m_vRelativePosition;


	Vector2 m_vPannelSize;
	Vector2 m_vDialoguePannelPosition;
	Vector2 m_vDialoguePannelSize;
	// 9 - Slice Texture Key. 비어있는 아이는 렌더하지 않도록 한다. 추후 쉐이더 단위에서 하나로 쓰까서 쓰도록 변경예정
	string m_strAtlasKey[9]; 	
	unsigned int m_iLimitLine;
	queue<CHATDATA> m_qChatData;
	vector<class CGameObject*> m_vecBoards;
	//패널에 마스킹도 할 수 있도록할것


	

public:
	bool Initialize() override;
	int Update(float fTime) override;
	void OnMouseStay(class CCollider* pTarget, const Vector2& mousePosition, const Vector3& vIntersectedPosition, float fTime) override;
};

