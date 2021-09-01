#include "Input.h"
#include "Core.h"
#include "Device.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"
#include "Component\Cursor.h"
#include "Debug.h"

SSS_USING

DEFINITION_SINGLE(CInput)


CInput::CInput():
	m_bCursorVisible(true),
	m_bEnableInput(true),
	m_iCarrotPosition(0),
	m_bReturn(false),
	m_bOnTextInput(false)
{
	m_bInitialized = false;
	m_pInputDevice = NULL;
	m_pKeyboardInput = NULL;
	m_pMouseInput = NULL;
}


CInput::~CInput()
{	
	// Release the mouse.
	if(m_pMouseInput) 
	{ 
		m_pMouseInput->Unacquire();
		m_pMouseInput->Release();
		m_pMouseInput = 0; 
	} 
	// Release the keyboard. 
	if(m_pKeyboardInput) 
	{ 
		m_pKeyboardInput->Unacquire();
		m_pKeyboardInput->Release();
		m_pKeyboardInput = 0;
	}
	// Release the main interface to direct input. 
	if(m_pInputDevice)
	{
		m_pInputDevice->Release();
		m_pInputDevice = 0;
	}

	return;
}

bool CInput::Initialize()
{
	HRESULT result;

	result = DirectInput8Create(GET_SINGLE(CCore)->GetWindowInstance(),
		DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputDevice, NULL);
	if(FAILED(result))
	{
		return false; 
	}

	result = m_pInputDevice->CreateDevice(GUID_SysKeyboard, &m_pKeyboardInput, NULL);
	if(FAILED(result)) 
	{ 
		return false; 
	} 

	result = m_pKeyboardInput->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result)) 
	{
		return false;
	}
	// Set the cooperative level of the keyboard to not share with other programs. 
	//result = m_pKeyboardInput->SetCooperativeLevel(GET_SINGLE(CCore)->GetWindowHandle(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	//if(FAILED(result)) 
	//{ 
	//	return false; 
	//}

	// Now acquire the keyboard. 
	result = m_pKeyboardInput->Acquire();
	if(FAILED(result)) 
	{ 
		return false;
	}		

	// Initialize the direct input interface for the mouse.
	result = m_pInputDevice->CreateDevice(GUID_SysMouse, &m_pMouseInput, NULL);
	if(FAILED(result)) 
	{
		return false; 
	} 

	result = m_pMouseInput->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result)) 
	{ 
		return false; 
	}

	// Set the cooperative level of the mouse to share with other programs.
	//result = m_pMouseInput->SetCooperativeLevel(GET_SINGLE(CCore)->GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	//if (FAILED(result))
	//{
	//	return false;
	//}

	// Acquire the mouse.
	result = m_pMouseInput->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	ResetResolution();
	m_bInitialized = true;
	return true;
}

void CInput::Update()
{
	ReadKeyboard(); 
	ReadMouse(); 

	GetCursorPositionFromWindow();

	if (!m_bCursorVisible)
	{
		Vector2 vCursorPosition = GET_SINGLE(CInput)->GetCursorPosition(SPACE_DEVICE);

		if (vCursorPosition.x < 0 || vCursorPosition.x > DEVICE_RESOLUTION.iWidth ||
			vCursorPosition.y < 0 || vCursorPosition.y > DEVICE_RESOLUTION.iHeight)
		{
			while (ShowCursor(TRUE) < 0)
			{
			}
			m_bCursorVisible = TRUE;		
		}
		else
		{
			while (ShowCursor(FALSE) >= 0);
			{
			}
			m_bCursorVisible = FALSE;
		}
	}
	else
	{
		while (ShowCursor(TRUE) < 0)
		{
		}

		m_bCursorVisible = TRUE;
	}

}


void CInput::GetCursorPositionFromWindow()
{
	POINT pos;
	GetCursorPos(&pos);
	HWND hWnd = GET_SINGLE(CCore)->GetWindowHandle();
	ScreenToClient(hWnd, &pos);

	ResetResolution();


	Vector2 vPrevDevice = m_vCursorPositionAsDevice;
	Vector2 vPrevWindow = m_vCursorPositionAsWindow;
	m_vCursorPositionAsWindow.x = pos.x;
	m_vCursorPositionAsWindow.y = pos.y;
	m_vCursorPositionAsDevice.x = m_vCursorPositionAsWindow.x * m_vResolutionRate.x;
	m_vCursorPositionAsDevice.y = m_vCursorPositionAsWindow.y * m_vResolutionRate.y;
	m_vCursorDeltaPositionAsWindow = m_vCursorPositionAsWindow - vPrevWindow;
	m_vCursorDeltaPositionAsDevice = m_vCursorPositionAsDevice - vPrevDevice;
}

bool CInput::ReadKeyboard()
{
	memcpy(m_pPreviousKeyState, m_pCurrentKeyState, sizeof(unsigned char) * 256);

	HRESULT result; 
	// Read the keyboard device. 
	result = m_pKeyboardInput->GetDeviceState(sizeof(unsigned char) * 256, (LPVOID)&m_pCurrentKeyState);
	if(FAILED(result)) 
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) 
		{ 
			m_pKeyboardInput->Acquire();
		} 
		else 
		{ 
			return false;
		} 
	} return true;
}

bool CInput::ReadMouse()
{
	HRESULT result;
	
	memcpy(&m_pPreviousKeyState[256], &m_pCurrentKeyState[256], sizeof(unsigned char) * 5);
	memcpy(&m_tPreviousMouseState, &m_tCurrentMouseState, sizeof(DIMOUSESTATE));
	// Read the mouse device.
	result = m_pMouseInput->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_tCurrentMouseState);
	if (FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_pMouseInput->Acquire();
		}
		else
		{
			return false;
		}
	}

	m_pCurrentKeyState[256] = m_tCurrentMouseState.rgbButtons[0]; // L
	m_pCurrentKeyState[257] = m_tCurrentMouseState.rgbButtons[1]; // R
	m_pCurrentKeyState[258] = m_tCurrentMouseState.rgbButtons[2]; // M
	
	if (m_tCurrentMouseState.lZ > 0)
	{
		m_pCurrentKeyState[259] = 0x80;
	}
	else if (m_tCurrentMouseState.lZ < 0)
	{
		m_pCurrentKeyState[260] = 0x80;
	}
	else
	{
		m_pCurrentKeyState[259] = 0x00;
		m_pCurrentKeyState[260] = 0x00;
	}

	return true;
}

bool CInput::HasReturned() const
{
	return m_bReturn;
}

int CInput::ImeTextUpdate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int iLength = 0;

	if (m_bOnTextInput)
	{
		if (m_pOutputTarget != NULL)
		{
			switch (msg)
			{
			case WM_IME_COMPOSITION:
				{
					HIMC hIMC = ImmGetContext(hWnd);

					if (lParam & GCS_RESULTSTR)
					{
						if ((iLength = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0)) > 0)
						{
							ImmGetCompositionString(hIMC, GCS_RESULTSTR, m_cComposition, iLength);
							m_cComposition[iLength] = 0;
							wchar_t wcCompositionWord = 0;
#ifdef _DEBUG
							memcpy(&wcCompositionWord, m_cComposition, 2);
#else
							MultiByteToWideChar(CP_ACP, 0, m_cComposition, -1, &wcCompositionWord, strlen(m_cComposition));
#endif // _DEBUG


							m_vecString.push_back(wcCompositionWord);
							memset(m_cComposition, 0, 16);
						}
					}
					else if (lParam & GCS_COMPSTR)
					{
						iLength = ImmGetCompositionString(hIMC, GCS_COMPSTR, NULL, 0);
						if (iLength == NULL)
						{
							memset(m_cComposition, 0, 16);
						}

						ImmGetCompositionString(hIMC, GCS_COMPSTR, m_cComposition, iLength);
						m_cComposition[iLength] = 0;
					}

					ImmReleaseContext(hWnd, hIMC);
					UpdateCompositionText();
					return 0;
				}
			case WM_CHAR://영어, 1byte 특수문자
			{	
				if (wParam == 0x0d)
				{
					m_bReturn = true;
				}
				if (wParam == 0x08)
				{
					int iLength = m_vecString.size();
					if (iLength > 0)
					{
						m_vecString.erase(m_vecString.begin() + (iLength - 1));
						memset(m_cComposition, 0, 16);
						UpdateCompositionText();
					}
				}
				else
				{
					wchar_t cWord = wParam;
					m_vecString.push_back(cWord);
					UpdateCompositionText();
				}
			}
				return 0;
			case WM_IME_NOTIFY:// 한자
			{
			}
				return 0;
			case WM_KEYDOWN:// 키다운..
			{
				switch (wParam)
				{
				case VK_ESCAPE:
				{
					EndTextInput();
				}
				break;
				default:
					break;
				}
			}
				return 0;
			}
			return 1;
		}
		return 1;
	}
	return 1;
}

void CInput::UpdateCompositionText()
{
	wchar_t strTemp[256] = {};
	
	size_t iLength = m_vecString.size();
	
	//memcpy
	for (size_t i = 0; i < iLength; ++i)
	{
		strTemp[i] = m_vecString[i];
	}

	wchar_t wcWord[8] = {};

#ifdef _DEBUG
	memcpy(wcWord, m_cComposition, 16);
#else
	MultiByteToWideChar(CP_ACP, 0, m_cComposition, -1, wcWord, strlen(m_cComposition));
#endif // _DEBUG

	wstring wstrStringBuffer(strTemp);
	wstring wstrWordBuffer(wcWord);
	m_pOutputTarget->SetText(wstrStringBuffer + wstrWordBuffer);
}

bool CInput::IsInitialized() const
{
	return m_bInitialized;
}

void CInput::ResetResolution()
{
	RECT tWindowRect;
	GetClientRect(GET_SINGLE(CCore)->GetWindowHandle(), &tWindowRect);
	m_vWindowResolution.x = tWindowRect.right - tWindowRect.left;
	m_vWindowResolution.y = tWindowRect.bottom - tWindowRect.top;
	m_vDeviceResolution = Vector2((float)GET_SINGLE(CDevice)->GetDeviceResolution().iWidth, (float)GET_SINGLE(CDevice)->GetDeviceResolution().iHeight);
	m_vResolutionRate = Vector2(m_vDeviceResolution.x / m_vWindowResolution.x, (float)m_vDeviceResolution.y / m_vWindowResolution.y);

	POINT pos;
	GetCursorPos(&pos);
	HWND hWnd = GET_SINGLE(CCore)->GetWindowHandle();
	ScreenToClient(hWnd, &pos);

	m_vCursorPositionAsWindow.x = pos.x;
	m_vCursorPositionAsWindow.y = pos.y;
}

void CInput::SetEnableToInputText(CText * pOutputTarget)
{
	m_vecString.clear();
	memset(m_strComposedString, 0, 256);
	memset(m_strContainerString, 0, 256);
	memset(m_cComposition, 0, 16);
	m_iCompositionCharacterCount = 0;

	m_pOutputTarget = pOutputTarget;
	m_bOnTextInput = true;
}

bool CInput::OnTextInput() const
{
	return m_bOnTextInput;
}

void CInput::EndTextInput()
{
	m_vecString.clear();
	memset(m_strComposedString, 0, 256);
	memset(m_strContainerString, 0, 256);
	memset(m_cComposition, 0, 16);
	m_iCompositionCharacterCount = 0;

	m_bOnTextInput = false;
	m_pOutputTarget = NULL;
}

bool CInput::GetCursorVisible() const
{
	return m_bCursorVisible;
}

void CInput::SetCursorVisible(bool bVisible)
{
	m_bCursorVisible = bVisible;
}

Vector2 CInput::GetCursorPosition(SPACE eSpace) const
{	
	switch (eSpace)
	{
		case SSS::SPACE_WINDOW:
		{
			return m_vCursorPositionAsWindow;
		}	break;
		case SSS::SPACE_DEVICE:
		{
			return m_vCursorPositionAsDevice;
		}
		break;
	}

	//Default
	return m_vCursorPositionAsDevice;
}

//Default Space : DEVICE
Vector2 CInput::GetCursorDeltaPosition(SPACE eSpace) const
{
	if (eSpace == SPACE_DEVICE)
		return m_vCursorDeltaPositionAsDevice;
	else if (eSpace == SPACE_WINDOW)
		return m_vCursorDeltaPositionAsWindow;
	else
		return m_vCursorDeltaPositionAsDevice;
}

void CInput::EnableInput()
{
	m_bEnableInput = true;
}

void CInput::DisableInput()
{
	m_bEnableInput = false;
}

bool CInput::GetAsyncKeyDown(KEY_CODE eKey)
{
	if (!(m_pPreviousKeyState[eKey] & 0x80) && m_pCurrentKeyState[eKey] & 0x80)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CInput::GetAsyncKeyUp(KEY_CODE eKey)
{
	if ((m_pPreviousKeyState[eKey] & 0x80) && !(m_pCurrentKeyState[eKey] & 0x80))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CInput::GetAsyncKey(KEY_CODE eKey)
{
	if (m_pCurrentKeyState[eKey] & 0x80)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CInput::GetKeyDown(KEY_CODE eKey)
{
	if (m_bEnableInput)
	{
		if (!(m_pPreviousKeyState[eKey] & 0x80) && m_pCurrentKeyState[eKey] & 0x80)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CInput::GetKeyUp(KEY_CODE eKey)
{
	if (m_bEnableInput)
	{
		if ((m_pPreviousKeyState[eKey] & 0x80) && !(m_pCurrentKeyState[eKey] & 0x80))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CInput::GetKey(KEY_CODE eKey)
{
	if (m_bEnableInput)
	{
		if (m_pCurrentKeyState[eKey] & 0x80)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}
