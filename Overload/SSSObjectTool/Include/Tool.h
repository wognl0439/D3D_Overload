
// Tool.h : Tool 응용 프로그램에 대한 주 헤더 파일
//
#pragma once


#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "Engine64_Debug")
#else
#pragma comment(lib, "Engine64")
#endif // _DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib, "Engine_Debug")
#else
#pragma comment(lib, "Engine")
#endif // _DEBUG
#endif //




#ifndef __AFXWIN_H__
	//#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.



// CToolApp:
// 이 클래스의 구현에 대해서는 Tool.cpp을 참조하십시오.
//

class CToolApp : public CWinAppEx
{
public:
	CToolApp();
	
// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 구현입니다.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
	virtual int Run();
	afx_msg void OnCreateObject();
	afx_msg void OnCreateUI();
	afx_msg void OnAnimationDlg();
	afx_msg void SceneSave();
	afx_msg void SceneLoad();
	afx_msg void SavePrefab();
	afx_msg void LoadPrefab();
	afx_msg void EditLayer();
};

extern CToolApp theApp;
