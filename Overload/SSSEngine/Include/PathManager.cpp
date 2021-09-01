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

	//���� ���������� ����ִ� �����θ� ������ �Լ�.
	GetModuleFileNameW(NULL, strPath, MAX_PATH);

	//null���ڸ� ������ ���ڿ��� �� �ڿ������� ��ȸ�� �Ѵ�.
	for (int i = lstrlenW(strPath) - 1; i >= 0; --i)
	{
		//���ڿ� �и��� ���ش�.
		if (strPath[i] == '/' || strPath[i] == '\\')
		{
			//�����͸� ����� ���ڿ��� �ּҰ� + �̵��� �� �� ���ؿͼ� �޺κ��� 0���� �� ����������
			//�׷� ���� ���� ���ϴ� ��θ� ���� �� �� �ִ�.
			memset(strPath + (i + 1), 0, sizeof(wchar_t) * (MAX_PATH - (i + 1)));
			break;
		}
	}


	m_mapPath.insert(make_pair(ROOT_PATH, strPath));

	// Shader ��� ����
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

	//��θ� ����� ã�ƿ����� ���ڿ��� �޾ƿ� �н��� ������ش�.
	if (pBasePath)
	{
		strPath = pBasePath;
	}

	//���ڿ��� �̾� �ٿ��ش�.
	strPath += pPath;

	m_mapPath.insert(make_pair(strKey, strPath));

	return true;
}

const wchar_t * CPathManager::FindPath(const string & strKey)
{
	unordered_map<string, wstring>::iterator iter = m_mapPath.find(strKey);

	//Ű���� ã�� ���ߴٸ� nullptr �� ��ȯ
	if (iter == m_mapPath.end())
	{
		return nullptr;
	}

	//Ű���� ����� ã�Ҵٸ�
	//�ش� ���ڿ��� ��ȯ���ش�.
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
		//������..
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

	//Full Path ���� Root Path�� �ߺ� �Ǵ� �κ� ����
	string strFullPath(strBaseFullPath);

	//���� �κ��� ��ū�� ���� PathKey ������
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



	//��ΰ� Bin ���ΰ� �ƴϰų� Bin ���� �������� �ʴ� ��������.
	return false;
}
