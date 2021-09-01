#pragma once

#include "Engine.h"

SSS_BEGIN
class SSS_DLL CPathManager
{
	DECLARE_SINGLE(CPathManager)
	
private:
	unordered_map<string, wstring>	m_mapPath;
	char m_strPath[MAX_PATH];

public:
	bool Initialize(HWND hWnd);
	bool CreatePath(const string& strKey, const wchar_t* pPath, const string& strBaseKey = ROOT_PATH);
	const wchar_t* FindPath(const string& strKey);
	const char* FindPathMultiByte(const string& strKey);
	bool ExistFile(const string& strPath);
	bool ExistFile(const wstring& strPath);
	size_t GetFileNameIndex(const string& strPath);
	size_t GetFileNameIndex(const wstring& strPath);

	size_t GetPaddingCount(const wstring* strPath);
	bool ExtractRelativeDirectoryFromFullPath(const char * __in strPath, char ** __out strRelativePath, char ** __out strPathKey);
};
SSS_END	

