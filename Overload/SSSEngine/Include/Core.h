#pragma once


#include "Engine.h"
#include "Macro.h"

SSS_BEGIN

class SSS_DLL CCore
{
	DECLARE_SINGLE(CCore)
	
private:
	static bool	m_bLoop;
	static bool m_bDebug;

private:
	HINSTANCE	m_hInst;
	HWND		m_hWnd;
	RESOLUTION	m_tResolution;
	float		m_fClearColor[4];

public:
	HINSTANCE GetWindowInstance()	const;
	HWND GetWindowHandle()	const;
	RESOLUTION GetWindowResolution()	const;
	void SetClearColor(float fColor[4]);
	void SetClearColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a);
	void SetRenderMode(RENDER_MODE eMode);
	

public:
	void SetEngineState(bool bDebug);
	bool GetEngineState() const;

	void ExitProcess();
	bool Init(HINSTANCE hInst, const RESOLUTION& tResolution, const wchar_t* pTitle,
		const wchar_t* pClass, int iIconID, int iSmallIconID,
		bool bWindowMode);
	bool Init(HINSTANCE hInst, HWND hWnd, const RESOLUTION& tResolution, bool bWindowMode);
	int Run();
	void Logic();

private:
	int Input(float fTime);
	int Update(float fTime);
	int LateUpdate(float fTime);
	void Render(float fTime);


private:
	ATOM MyRegisterClass(const wchar_t* pClass,
		int iIconID, int iSmallIconID);
	BOOL Create(const wchar_t * pTitle, const wchar_t * pClass);

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);





};

SSS_END