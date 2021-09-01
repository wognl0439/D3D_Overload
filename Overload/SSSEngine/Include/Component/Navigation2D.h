#pragma once
#include "../Component.h"
#include <stack>

SSS_BEGIN

typedef struct _tagAStarNode
{
	int	iParentIndex;
	int	iIndex;
	int	iIndexX;
	int	iIndexY;
	float	fG;
	float	fH;
	float	fTotal;
}ASTARNODE, *PASTARNODE;

enum ASTAR_NODE_DIR
{
	AND_UP,
	AND_LEFT_UP,
	AND_RIGHT_UP,
	AND_LEFT,
	AND_RIGHT,
	AND_LEFT_DOWN,
	AND_RIGHT_DOWN,
	AND_DOWN,
	AND_END
};

typedef struct _tagNavPath
{
	int		iStage;
	int		idx;
}NAVPATH, *PNAVPATH;

class SSS_DLL CNavigation2D :
	public CComponent
{
private:
	friend class CGameObject;

private:
	CNavigation2D();
	CNavigation2D(const CNavigation2D& nav);
	~CNavigation2D();

private:
	list<class CTile*>				m_OpenList;
	list<class CTile*>				m_CloseList;
	stack<NAVPATH>					m_Path;
	Vector3							m_vMovePos;
	float							m_fMoveSpeed;
	bool							m_bLastPath;
	bool							m_bPathEnd;
#ifdef _DEBUG
	bool							m_bDebug;
#endif // DEBUG

public:
	void SetMoveSpeed(float fMoveSpeed);

public:
	virtual bool Initialize();
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual void Render(float fTime);

public:
	bool FindRoad(const Vector3& vStart, const Vector3& vEnd);

private:
	bool FindTile(class CTile* pNode, class CTile* pEndTile, int iStage);
	bool AddOpenNode(int iStage, int x, int y, class CTile* pNode,
		class CTile* pEndTile, ASTAR_NODE_DIR eDir);
	bool CheckCloseList(int idx);

	static bool AStarNodeSort(const CTile* src, const CTile* dest);
};

SSS_END
