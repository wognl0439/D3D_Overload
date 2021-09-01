#pragma once
#include "Ref.h"

SSS_BEGIN

class SSS_DLL CShader :
	public CRef
{

private:
	friend class CShaderManager;
	
private:
	CShader();
	~CShader();


private:
	ID3DBlob*		m_pVSBlob;
	ID3D11VertexShader*	m_pVS;
	ID3DBlob*		m_pPSBlob;
	ID3D11PixelShader*	m_pPS;
	ID3DBlob*		m_pGSBlob;
	ID3D11GeometryShader*	m_pGS;
	string m_strKey;

public:
	void SetKey(const string& strKey);
	const string& GetKey() const;
	ID3DBlob* GetVSBlob()	const;
	ID3DBlob* GetPSBlob()	const;
	ID3DBlob* GetGSBlob()	const;

public:
	bool LoadShader(const string& strTag, const wchar_t* pFileName,	char* pEntry[ST_END], const string& strPathKey = SHADER_PATH);
	bool LoadVertexShader(const wchar_t* pFileName, char* pEntry,	const string& strPathKey = SHADER_PATH);
	bool LoadPixelShader(const wchar_t* pFileName, char* pEntry,	const string& strPathKey = SHADER_PATH);
	bool LoadGeometryShader(const wchar_t* pFileName, char* pEntry, const string& strPathKey = SHADER_PATH);
	void SetShader();

};

SSS_END
