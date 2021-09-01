#include "PathManager.h"

SSS_USING

DEFINITION_SINGLE(CPathManager)

CPathManager::CPathManager()
{
}


CPathManager::~CPathManager()
{
}


bool CPathManager::Initialize(HWND hWnd)
{
	wchar_t strPath[MAX_PATH] = {};

	//현재 실행파일이 들어있는 절대경로를 얻어오는 함수.
	GetModuleFileNameW(NULL, strPath, MAX_PATH);

	//null문자를 제외한 문자열의 맨 뒤에서부터 순회를 한다.
	for (int i = lstrlenW(strPath) - 1; i >= 0; --i)
	{
		//문자열 분리를 해준다.
		if (strPath[i] == '/' || strPath[i] == '\\')
		{
			//포인터를 사용해 문자열의 주소값 + 이동한 수 를 구해와서 뒷부분을 0으로 다 날려버린다
			//그로 인해 현재 원하는 경로를 구해 올 수 있다.
			memset(strPath + (i + 1), 0, sizeof(wchar_t) * (MAX_PATH - (i + 1)));
			break;
		}
	}


	m_mapPath.insert(make_pair(ROOT_PATH, strPath));

	// Shader 경로 설정
	if (CreatePath(SHADER_PATH, L"Shader\\") == false)
	{
		return false;
	}

	if (CreatePath(TEXTURE_PATH, L"Texture\\") == false) 
	{
		return false;
	}
	if (CreatePath(DATA_PATH, L"Data\\") == false)
	{
		return false;
	}
	if (CreatePath(MESH_PATH, L"Mesh\\") == false)
	{
		return false;
	}
	if (CreatePath(SOUND_PATH, L"Sound\\") == false)
	{
		return false;
	}


	return true;
}

bool CPathManager::CreatePath(const string & strKey,
	const wchar_t * pPath, const string & strBaseKey)
{
	const wchar_t* pBasePath = FindPath(strBaseKey);

	wstring strPath;

	//경로를 제대로 찾아왔으면 문자열을 받아와 패스를 만들어준다.
	if (pBasePath)
	{
		strPath = pBasePath;
	}

	//문자열을 이어 붙여준다.
	strPath += pPath;

	m_mapPath.insert(make_pair(strKey, strPath));

	return true;
}

const wchar_t * CPathManager::FindPath(const string & strKey)
{
	unordered_map<string, wstring>::iterator iter = m_mapPath.find(strKey);

	//키값을 찾지 못했다면 nullptr 을 반환
	if (iter == m_mapPath.end())
	{
		return nullptr;
	}

	//키값을 제대로 찾았다면
	//해당 문자열을 반환해준다.
	return iter->second.c_str();
}

const char * CPathManager::FindPathMultiByte(const string & strKey)
{
	const wchar_t* pPath = FindPath(strKey);

	if (pPath == nullptr)
	{
		return nullptr;
	}

	memset(m_strPath, 0, sizeof(char) * MAX_PATH);

	WideCharToMultiByte(CP_ACP, 0, pPath, -1, m_strPath, lstrlenW(pPath), 0, 0);

	return m_strPath;
}

bool CPathManager::ExistFile(const string & strPath)
{
	FILE *pFile = NULL;
	if (pFile = fopen(strPath.c_str(), "r"))
	{
		fclose(pFile);
		return true;
	}
	return false;
}

bool CPathManager::ExistFile(const wstring & strPath)
{
	FILE *pFile = NULL;
	if (pFile = fopen(string(strPath.begin(), strPath.end()).c_str(), "r"))
	{
		fclose(pFile);
		return true;
	}
	return false;
}

size_t CPathManager::GetFileNameIndex(const string & strPath)
{
	bool bExist = ExistFile(strPath);

	if (bExist)
	{
		//귀찮다..
	}
	return size_t();
}

size_t CPathManager::GetFileNameIndex(const wstring & strPath)
{
	return size_t();
}

size_t CPathManager::GetPaddingCount(const wstring * strPath)
{
	return size_t();
}

bool CPathManager::ExtractRelativeDirectoryFromFullPath( const char * __in strPath, char ** __out strRelativePath, char ** __out strPathKey)
{
	char strBaseFullPath[512] = {};
	strcpy_s(strBaseFullPath, strPath);

	//Full Path 에서 Root Path와 중복 되는 부분 제거
	string strFullPath(strBaseFullPath);

	//남은 부분의 토큰을 비교해 PathKey 추출함
	if (strFullPath.find("\\Texture\\") != string::npos)
	{
		string strKeyPath = "\\Texture\\";
		size_t iIndex = strFullPath.find(strKeyPath) + strKeyPath.length();
		string strRelativeTempPath;
		strRelativeTempPath.append(strFullPath.begin() + iIndex, strFullPath.end());
		char pRelativePath[512] = {};
		strcpy_s(pRelativePath, strRelativeTempPath.c_str());
		*strPathKey = TEXTURE_PATH;
		*strRelativePath = pRelativePath;
		return true;
	}
	else if (strFullPath.find("\\Shader\\") != string::npos)
	{
		string strKeyPath = "\\Shader\\";
		size_t iIndex = strFullPath.find(strKeyPath) + strKeyPath.length();
		string strRelativeTempPath;
		strRelativeTempPath.append(strFullPath.begin() + iIndex, strFullPath.end());
		char pRelativePath[512] = {};
		strcpy_s(pRelativePath, strRelativeTempPath.c_str());
		*strPathKey = SHADER_PATH;
		*strRelativePath = pRelativePath;
		return true;
	}
	else if (strFullPath.find("\\Data\\") != string::npos)
	{
		string strKeyPath = "\\Data\\";
		size_t iIndex = strFullPath.find(strKeyPath) + strKeyPath.length();
		string strRelativeTempPath;
		strRelativeTempPath.append(strFullPath.begin() + iIndex, strFullPath.end());
		char pRelativePath[512] = {};
		strcpy_s(pRelativePath, strRelativeTempPath.c_str());
		*strPathKey = DATA_PATH;
		*strRelativePath = pRelativePath;
		return true;
	}
	else if (strFullPath.find("\\Mesh\\") != string::npos)
	{
		string strKeyPath = "\\Mesh\\";
		size_t iIndex = strFullPath.find(strKeyPath) + strKeyPath.length();
		string strRelativeTempPath;
		strRelativeTempPath.append(strFullPath.begin() + iIndex, strFullPath.end());
		char pRelativePath[512] = {};
		strcpy_s(pRelativePath, strRelativeTempPath.c_str());
		*strPathKey = MESH_PATH;
		*strRelativePath = pRelativePath;
		return true;
	}
	else if (strFullPath.find("\\Sound\\") != string::npos)
	{
		string strKeyPath = "\\Sound\\";
		size_t iIndex = strFullPath.find(strKeyPath) + strKeyPath.length();
		string strRelativeTempPath;
		strRelativeTempPath.append(strFullPath.begin() + iIndex, strFullPath.end());
		char pRelativePath[512] = {};
		strcpy_s(pRelativePath, strRelativeTempPath.c_str());
		*strPathKey = SOUND_PATH;
		*strRelativePath = pRelativePath;
		return true;
	}



	//경로가 Bin 내부가 아니거나 Bin 내에 존재하지 않는 폴더명임.
	return false;
}
