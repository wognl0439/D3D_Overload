#include "Shader.h"
#include "PathManager.h"
#include "Device.h"

SSS_USING

CShader::CShader() :
	m_pVSBlob(NULL),
	m_pVS(NULL),
	m_pPSBlob(NULL),
	m_pPS(NULL),
	m_pGSBlob(NULL),
	m_pGS(NULL)
{
}


CShader::~CShader()
{
	SAFE_RELEASE(m_pVSBlob);
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPSBlob);
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pGSBlob);
	SAFE_RELEASE(m_pGS);
}

void CShader::SetKey(const string & strKey)
{
	m_strKey = strKey;
}

const string & CShader::GetKey() const
{
	return m_strKey;
}

ID3DBlob * CShader::GetVSBlob() const
{
	return m_pVSBlob;
}

ID3DBlob * CShader::GetPSBlob() const
{
	return m_pPSBlob;
}

ID3DBlob * CShader::GetGSBlob() const
{
	return m_pGSBlob;
}

bool CShader::LoadShader(const string & strTag, const wchar_t * pFileName, char * pEntry[ST_END], const string & strPathKey)
{
	SetTag(strTag);

	if (!LoadVertexShader(pFileName, pEntry[ST_VERTEX], strPathKey))
		return false;

	if (!LoadPixelShader(pFileName, pEntry[ST_PIXEL], strPathKey))
		return false;

	if (pEntry[ST_GEOMETRY])
	{
		if (!LoadGeometryShader(pFileName, pEntry[ST_GEOMETRY], strPathKey))
			return false;
	}

	return true;
}

bool CShader::LoadVertexShader(const wchar_t * pFileName, char * pEntry, const string & strPathKey)
{
	UINT	iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D10_SHADER_DEBUG;
#endif // _DEBUG

	// 경로를 만들어준다.
	const wchar_t*	pPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);
	wstring	strFullPath;

	if (pPath)
		strFullPath = pPath;

	strFullPath += pFileName;

	ID3DBlob*	pErr = NULL;

	


	if (FAILED(D3DCompileFromFile(strFullPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pEntry, "vs_5_0", iFlag, 0, &m_pVSBlob, &pErr)))
	{
		MessageBoxA(NULL, (char*)pErr->GetBufferPointer(),
			"Shader Error", MB_OK);
		return false;
	}

	// Shader를 만들어준다.
	if (FAILED(DEVICE->CreateVertexShader(m_pVSBlob->GetBufferPointer(),
		m_pVSBlob->GetBufferSize(), NULL,
		&m_pVS)))
		return false;

	return true;
}

bool CShader::LoadPixelShader(const wchar_t * pFileName, char * pEntry, const string & strPathKey)
{
	UINT	iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D10_SHADER_DEBUG;
#endif // _DEBUG

	// 경로를 만들어준다.
	const wchar_t*	pPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);
	wstring	strFullPath;

	if (pPath)
		strFullPath = pPath;

	strFullPath += pFileName;

	ID3DBlob*	pErr = NULL;
	if (FAILED(D3DCompileFromFile(strFullPath.c_str(), NULL,	D3D_COMPILE_STANDARD_FILE_INCLUDE, pEntry, "ps_5_0", iFlag, 0, &m_pPSBlob, &pErr)))
	{
		MessageBoxA(NULL, (char*)pErr->GetBufferPointer(),
			"Shader Error", MB_OK);
		return false;
	}

	// Shader를 만들어준다.
	if (FAILED(DEVICE->CreatePixelShader(m_pPSBlob->GetBufferPointer(),
		m_pPSBlob->GetBufferSize(), NULL,
		&m_pPS)))
		return false;

	return true;
}

bool CShader::LoadGeometryShader(const wchar_t * pFileName, char * pEntry, const string & strPathKey)
{
	UINT	iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D10_SHADER_DEBUG;
#endif // _DEBUG

	// 경로를 만들어준다.
	const wchar_t*	pPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);
	wstring	strFullPath;

	if (pPath)
		strFullPath = pPath;

	strFullPath += pFileName;

	ID3DBlob*	pErr = NULL;
	if (FAILED(D3DCompileFromFile(strFullPath.c_str(), NULL,	D3D_COMPILE_STANDARD_FILE_INCLUDE, pEntry, "gs_5_0", iFlag, 0, &m_pGSBlob, &pErr)))
	{
		MessageBoxA(NULL, (char*)pErr->GetBufferPointer(),"Shader Error", MB_OK);
		return false;
	}

	// Shader를 만들어준다.
	if (FAILED(DEVICE->CreateGeometryShader(m_pGSBlob->GetBufferPointer(),m_pGSBlob->GetBufferSize(), NULL, &m_pGS)))
		return false;

	return true;
}

// 렌더링 파이프라인에 해당 Shader를 지정한다.
void CShader::SetShader()
{
	DEVICE_CONTEXT->VSSetShader(m_pVS, NULL, 0);
	DEVICE_CONTEXT->PSSetShader(m_pPS, NULL, 0);
	DEVICE_CONTEXT->GSSetShader(m_pGS, NULL, 0);
}
