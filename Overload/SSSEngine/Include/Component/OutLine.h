#pragma once
#include "../Component.h"

SSS_BEGIN

class SSS_DLL	COutLine	:
	public CComponent
{
private:
	friend	class CGameObject;

public:
	COutLine();
	~COutLine();

private:
	FXOUTLINE m_tConstantBuffer;
	class CShader*		m_pOutLineShader;
	
public:
	void SetOutlineSize(float fSize);
	void SetOutlineColor(const Vector4& vColor);
	void SetOutlineOffset(const Vector3& vOffset);
	float GetOutlineSize();
	Vector4 GetOutlineColor();
	Vector4 GetOutlineOffset();

public:
	bool Initialize() override;
	int Prerender(class CMeshRenderer*	pRenderer) override;
	void SetOutLineShader(const string& strKey);

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END