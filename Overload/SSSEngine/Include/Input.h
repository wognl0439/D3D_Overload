#pragma once

#include "Engine.h"
#include "Component/Text.h"

//Input
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define DIRECTINPUT_VERSION 0x0800

SSS_BEGIN

typedef struct _tagKeyState
{
	bool bDown;
	bool bPressed;
	bool bUp;

	_tagKeyState() :
		bDown(false),
		bPressed(false),
		bUp(false)
	{
	}

}KEYSTATE, *PKEYSTATE;


class SSS_DLL CInput
{
	DECLARE_SINGLE(CInput)

private:
	friend class CCore;

private:
	bool m_bInitialized;

	IDirectInput8*			m_pInputDevice; 
	IDirectInputDevice8*	m_pKeyboardInput;
	IDirectInputDevice8*	m_pMouseInput;

	Vector2 m_vDeviceResolution;
	Vector2 m_vWindowResolution;
	Vector2 m_vResolutionRate;

	unsigned char m_pPreviousKeyState[261];
	unsigned char m_pCurrentKeyState[261];
	DIMOUSESTATE m_tPreviousMouseState;
	DIMOUSESTATE m_tCurrentMouseState;

	Vector2 m_vCursorPositionAsWindow;
	Vector2 m_vCursorPositionAsDevice;
	Vector2 m_vCursorDeltaPositionAsWindow;
	Vector2 m_vCursorDeltaPositionAsDevice;

	bool m_bEnableInput; // Async¿ë
	bool m_bCursorVisible;

	bool m_bOnTextInput;// IME¿ë
	CText* m_pOutputTarget;
	int		m_iCarrotPosition;

	vector<wchar_t> m_vecString;
	char m_strComposedString[256];
	wchar_t m_strContainerString[128];
	char m_cComposition[16];
	int m_iCompositionCharacterCount;
	bool m_bReturn;
	
private:
	void GetCursorPositionFromWindow();
	bool ReadKeyboard();
	bool ReadMouse();

	bool HasReturned() const;
	int ImeTextUpdate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	void UpdateCompositionText();

public:
	bool IsInitialized() const;
	void ResetResolution();
	void SetEnableToInputText(CText* pOutputTarget);
	bool OnTextInput() const;
	void EndTextInput();

	bool GetCursorVisible() const;
	void SetCursorVisible(bool bVisible);
	Vector2 GetCursorPosition(SPACE eSpace = SPACE_DEVICE) const;
	Vector2 GetCursorDeltaPosition(SPACE eSpace = SPACE_DEVICE) const;
	
public:
	void EnableInput();
	void DisableInput();
	bool GetAsyncKeyDown(KEY_CODE eKey);
	bool GetAsyncKeyUp(KEY_CODE eKey);
	bool GetAsyncKey(KEY_CODE eKey);
	bool GetKeyDown(KEY_CODE eKey);
	bool GetKeyUp(KEY_CODE eKey);
	bool GetKey(KEY_CODE eKey);
	void Update();
	bool Initialize();

};


SSS_END	