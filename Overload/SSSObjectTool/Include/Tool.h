
// Tool.h : Tool ���� ���α׷��� ���� �� ��� ����
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
	//#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.



// CToolApp:
// �� Ŭ������ ������ ���ؼ��� Tool.cpp�� �����Ͻʽÿ�.
//

class CToolApp : public CWinAppEx
{
public:
	CToolApp();
	
// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
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
