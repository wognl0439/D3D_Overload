#pragma once
#include "../Component.h"

SSS_BEGIN

class SSS_DLL	CTrail :
	public CComponent
{
private:
	friend	class CGameObject;

public:
	CTrail();
	~CTrail();

private:
	class CShader*	m_pTrailShader;
	TRAILCBUFFER	m_pTrailCBuffer;

	list<Vector3>	m_VertexPosList;

	int				m_iCount;
	float			m_fTime;

	float			m_fSize;

public:
	void SetSize(float fSize);
	float GetSize()		const;
	void SetPositionInAllList(Vector3 vPos);

public:
	bool Initialize() override;
	int LateUpdate(float fTime) override;
	int Prerender(class CMeshRenderer*	pRenderer) override;


	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END