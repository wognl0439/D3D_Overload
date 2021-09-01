#include "Splash.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Debug.h"
#include "Component/SpriteRenderer.h"
#include "Layer.h"
#include "Material.h"
#include "ResourcesManager.h"
#include "Transform.h"
#include "Device.h"
#include "ShaderManager.h"
#include "Thread.h"
#include "ThreadManager.h"
#include "LoadingThread.h"
#include "LoadingPanel.h"
#include "DataManager.h"
#include "GameSystem.h"
#include "SoundManager.h"

CSplash::CSplash():
	m_fFadingTime(0.5f)
	, m_fTimer(0.0f)
	, m_pBackgroundPanel(NULL)
	, m_pForegroundPanel(NULL)
	, m_ePhase(SIP_NIL)
	, m_pLoadedScene(NULL)
	, m_bSoundPlay(false)
{
}


CSplash::~CSplash()
{
	//간접 참조중이라 해제하지 않음.
	//SAFE_RELEASE(m_pBackgroundPanel);
	//SAFE_RELEASE(m_pForegroundPanel);                 
}

void CSplash::SetSplashState(SPLASH_STATE eState)
{
	m_eSplashState = eState;
}


void CSplash::SetPath(const wstring & strPath)
{
	m_strPath = strPath;
}

void CSplash::SetPathKey(const string & strPathKey)
{
	m_strPathKey = strPathKey;
}

void CSplash::SetFadingTime(float fTime)
{
	m_fFadingTime = fTime;
}

const wstring & CSplash::GetPath() const
{
	return m_strPath;
}

const string & CSplash::GetPathKey() const
{
	return m_strPathKey;
}

float CSplash::GetFadingTime() const
{
	return m_fFadingTime;
}

bool CSplash::Initialize()
{
	CLayer* pLayer = m_pScene->FindLayer("UI");

#pragma region Background Panel
	{
		CGameObject* pBackground = CGameObject::CreateObject("BackgroundPanel", pLayer);
		m_pBackgroundPanel = pBackground;

		CTransform* pTransform = pBackground->GetTransform();
		pTransform->SetPivot(0.0f, 0.0f, 0.0f);
		pTransform->SetWorldPosition(0.0f, 0.0f, 0.0f);
		pTransform->SetWorldScale(DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 1.0f);
		SAFE_RELEASE(pTransform);

		CSpriteRenderer* pRenderer = pBackground->AddComponent<CSpriteRenderer>();
		pRenderer->SetZOrder(20000);
		CMaterial* pMaterial = pRenderer->GetMaterial();
		pMaterial->SetDiffuseTexture("DefaultTexture");
		pMaterial->SetDiffuseColor(Vector4::black);

		SAFE_RELEASE(pMaterial);
		SAFE_RELEASE(pRenderer);		
		SAFE_RELEASE(pBackground);
	}
#pragma endregion


#pragma region Foreground Panel
	{
		CGameObject* pForeground = CGameObject::CreateObject("ForegroundPanel", pLayer);
		m_pForegroundPanel = pForeground;
		CSpriteRenderer* pRenderer = pForeground->AddComponent<CSpriteRenderer>();
		pRenderer->SetZOrder(25000);
		CMaterial* pMaterial = pRenderer->GetMaterial();
		pMaterial->SetDiffuseTexture("DefaultTexture");
		pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
		SAFE_RELEASE(pMaterial);
		SAFE_RELEASE(pRenderer);
		
		CLoadingPanel* pLoadingPanel = pForeground->AddComponent<CLoadingPanel>();	
		SAFE_RELEASE(pLoadingPanel);

		SAFE_RELEASE(pForeground);
	}
#pragma endregion
		

	SAFE_RELEASE(pLayer);
	return true;
}

void CSplash::Start()
{
	//if (!m_strPath.empty())
	//{
	//	GET_SINGLE(CSceneManager)->CreateSceneAsynchronously(m_strPath.c_str(), m_strPathKey);
	//}
}


int CSplash::Update(float fTime)
{
	if (m_eSplashState == SPLASH_TITLE)
	{
		m_fTimer += fTime;
		CTransform* pForegroundTransform = m_pForegroundPanel->GetTransform();
		CSpriteRenderer* pRenderer = m_pForegroundPanel->GetComponent<CSpriteRenderer>();
		if (pRenderer)
		{
			CMaterial* pMaterial = pRenderer->GetMaterial();
			switch (m_ePhase)
			{
			case SIP_NIL:
			{				
				if (!m_bSoundPlay)
				{
					m_fTimer = 0;
					m_bSoundPlay = true;
					GET_SINGLE(CSoundManager)->Play("Logo_AR");
					
					pForegroundTransform->SetPivot(0.5f, 0.5f, 0.5f);
					pForegroundTransform->SetWorldScale(500.0f, 500.0f, 1.0f);
					pForegroundTransform->SetWorldPosition(DEVICE_RESOLUTION.iWidth / 2.0f, DEVICE_RESOLUTION.iHeight / 2.0f, 0.0f);
					pMaterial->SetDiffuseTexture("AR_Logo_Single", L"UI\\AR_Logo_Single.png", 0, 0, TEXTURE_PATH);
				}
				else
				{
					float fDelay = 1.5f;
					if (m_fTimer >= fDelay)
					{
						m_ePhase = SIP_000;
						m_fTimer = 0.0f;
						m_bSoundPlay = false;
					}
				}
			}break;
			case SIP_000:
			{
				//Assort Rock Logo Fade In			
				float fProgress = m_fTimer / m_fFadingTime;
				Vector4 vColor = pMaterial->GetDiffuseColor();
				vColor.w = fProgress;
				pMaterial->SetDiffuseColor(vColor);

				if (fProgress >= 1.0f)
				{
					m_ePhase = SIP_001;
					m_fTimer = 0.0f;
				}
			}break;
			case SIP_001:
			{
				//Assort Rock Logo Fade Stay
				float fProgress = m_fTimer / (m_fFadingTime * 4.0f);
				Vector4 vColor = pMaterial->GetDiffuseColor();
				vColor.w = 1.0f;
				pMaterial->SetDiffuseColor(vColor);
				if (fProgress >= 1.0f)
				{
					m_ePhase = SIP_002;
					m_fTimer = 0.0f;
				}
			}break;
			case SIP_002:
			{
				//Assort Rock Logo Fade Out
				float fProgress = m_fTimer / m_fFadingTime;
				Vector4 vColor = pMaterial->GetDiffuseColor();
				vColor.w = 1.0f - fProgress;
				pMaterial->SetDiffuseColor(vColor);
				if (fProgress >= 1.0f)
				{
					//Change Texture
					pForegroundTransform->SetPivot(0.5f, 0.5f, 0.5f);
					pForegroundTransform->SetWorldScale(700.0f, 500.0f, 1.0f);
					pForegroundTransform->SetWorldPosition(DEVICE_RESOLUTION.iWidth / 2.0f, DEVICE_RESOLUTION.iHeight / 2.0f, 0.0f);

					pMaterial->SetDiffuseTexture("SSS_Logo", L"UI\\SSS_Logo.png", 0, 0, TEXTURE_PATH);
					pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
					m_ePhase = SIP_003;
					m_fTimer = 0.0f;
					GET_SINGLE(CSoundManager)->Play("Logo_SSS");
				}
			}break;
			case SIP_003:
			{
				//Project Name Fade In
				float fProgress = m_fTimer / m_fFadingTime;
				Vector4 vColor = pMaterial->GetDiffuseColor();
				vColor.w = fProgress;
				pMaterial->SetDiffuseColor(vColor);
				if (fProgress >= 1.0f)
				{
					m_ePhase = SIP_004;
					m_fTimer = 0.0f;
				}
			}break;
			case SIP_004:
			{
				//Project Name Fade Stay
				float fProgress = m_fTimer / (m_fFadingTime * 4.0f);
				Vector4 vColor = pMaterial->GetDiffuseColor();
				vColor.w = 1.0f;
				pMaterial->SetDiffuseColor(vColor);
				if (fProgress >= 1.0f)
				{
					m_ePhase = SIP_005;
					m_fTimer = 0.0f;
				}
			}break;
			case SIP_005:
			{
				//Project Name Fade Out
				float fProgress = m_fTimer / m_fFadingTime;
				Vector4 vColor = pMaterial->GetDiffuseColor();
				vColor.w = 1.0f - fProgress;
				pMaterial->SetDiffuseColor(vColor);
				if (fProgress >= 1.0f)
				{
					//Start To Loading
					pForegroundTransform->SetPivot(0.5f, 0.5f, 0.5f);
					pForegroundTransform->SetWorldScale(768.0f, 384.0f, 1.0f);
					pForegroundTransform->SetWorldPosition(DEVICE_RESOLUTION.iWidth / 2.0f, DEVICE_RESOLUTION.iHeight / 2.0f, 0.0f);
					
					pRenderer->SetShader(LOADING_SHADER);
					
					pMaterial->SetDiffuseTexture("LoadingTitle", L"UI\\LoadingTitle.png", 0, 0, TEXTURE_PATH);
					pMaterial->SetSampler(SAMPLER_LINEAR);
					pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
					m_ePhase = SIP_006;
					m_fTimer = 0.0f;

					CLoadingPanel* pLoadingPanel = m_pForegroundPanel->GetComponent<CLoadingPanel>();
					pLoadingPanel->SetLoadingProgress(0.0f);
					SAFE_RELEASE(pLoadingPanel);
				}
			}break;
			case SIP_006:
			{
				//On Loading Entry Fade In
				float fProgress = m_fTimer / m_fFadingTime;
				Vector4 vColor = pMaterial->GetDiffuseColor();
				vColor.w = fProgress;
				pMaterial->SetDiffuseColor(vColor);
				if (fProgress >= 1.0f)
				{
					m_ePhase = SIP_007;
					m_fTimer = 0.0f;

					CLoadingPanel* pLoadingPanel = m_pForegroundPanel->GetComponent<CLoadingPanel>();
					pLoadingPanel->SetLoadingProgress(0.0f);
					SAFE_RELEASE(pLoadingPanel);

					if (!m_strPath.empty())
					{
						GET_SINGLE(CSceneManager)->CreateSceneAsynchronously(m_strPath.c_str(), m_strPathKey);
					}
				}
			}break; 
			case SIP_007:
			{
				//On Loading
				bool bOnLoading = GET_SINGLE(CSceneManager)->OnLoading();
				if (bOnLoading)
				{
					float fProgress = GET_SINGLE(CSceneManager)->LoadingProgress();
					if (fProgress <= 1.0f)
					{
						CLoadingPanel* pLoadingPanel = m_pForegroundPanel->GetComponent<CLoadingPanel>();
						pLoadingPanel->SetLoadingProgress(fProgress);
						SAFE_RELEASE(pLoadingPanel);
					}					
				}	
				else
				{
					//로딩이 끝나거나 시작하지 않았을 떄
					CThread* pThread = GET_SINGLE(CThreadManager)->FindThread("Loading");
					if (pThread)
					{
						m_pLoadedScene = ((CLoadingThread*)pThread)->GetScene();
						if (m_pLoadedScene)
						{
							GET_SINGLE(CThreadManager)->RemoveThread("Loading");
							m_ePhase = SIP_008;
							m_fTimer = 0.0f;
							CLoadingPanel* pLoadingPanel = m_pForegroundPanel->GetComponent<CLoadingPanel>();
							pLoadingPanel->SetLoadingProgress(1.0f);
							SAFE_RELEASE(pLoadingPanel);
						}
					}
				}
			}break;
			case SIP_008:
			{
				//Completed Delay
				float fProgress = m_fTimer / (m_fFadingTime * 6.0f);
				Vector4 vColor = pMaterial->GetDiffuseColor();
				vColor.w = 1.0f;
				pMaterial->SetDiffuseColor(vColor);
				if (fProgress >= 1.0f)
				{
					m_ePhase = SIP_009;
					m_fTimer = 0.0f;
					CLoadingPanel* pLoadingPanel = m_pForegroundPanel->GetComponent<CLoadingPanel>();
					pLoadingPanel->SetLoadingProgress(1.0f);
					SAFE_RELEASE(pLoadingPanel);
				}
			}break;
			case SIP_009:
			{
				//Loading Fade Out
				float fProgress = m_fTimer / m_fFadingTime;
				Vector4 vColor = pMaterial->GetDiffuseColor();
				vColor.w = 1.0f - fProgress;
				pMaterial->SetDiffuseColor(vColor);
				if (fProgress >= 1.0f)
				{
					m_ePhase = SIP_010;
					m_fTimer = 0.0f;
					CLoadingPanel* pLoadingPanel = m_pForegroundPanel->GetComponent<CLoadingPanel>();
					pLoadingPanel->SetLoadingProgress(1.0f);
					SAFE_RELEASE(pLoadingPanel);

					if (m_pLoadedScene)
					{
						PLAYERDATA tData = GET_SINGLE(CDataManager)->GetPlayerData();
						tData.iTransitionMapFrom = 15;
						GET_SINGLE(CDataManager)->SetPlayerData(tData);
						GET_SINGLE(CDataManager)->LoadPlayer(m_pLoadedScene);
						GET_SINGLE(CSceneManager)->SetNextScene(m_pLoadedScene);
						SAFE_RELEASE(m_pLoadedScene);
						m_eSplashState = SPLASH_DESTROY;
					}
				}
			}break;
			default:
				break;
			}
			SAFE_RELEASE(pMaterial);
		}	
		SAFE_RELEASE(pRenderer);
		SAFE_RELEASE(pForegroundTransform);
	}
	else if (m_eSplashState == SPLASH_ENTER)
	{
		m_fTimer += fTime;
		CTransform* pBackgroundTransform = m_pBackgroundPanel->GetTransform();
		CTransform* pForegroundTransform = m_pForegroundPanel->GetTransform();
		CSpriteRenderer* pBackgroundRenderer = m_pBackgroundPanel->GetComponent<CSpriteRenderer>();
		CSpriteRenderer* pForegoundRenderer = m_pForegroundPanel->GetComponent<CSpriteRenderer>();
		CMaterial* pBackgroundMaterial = pBackgroundRenderer->GetMaterial();
		CMaterial* pForegroundMaterial = pForegoundRenderer->GetMaterial();
		switch (m_ePhase)
		{
		case SIP_NIL:
		{
				m_fTimer = 0.0f;
				m_ePhase = SIP_000;
		}
			break;
		case SIP_000:
		{
			if (m_fTimer >= 2.0f)
			{
				//Initialize
				pForegroundTransform->SetPivot(0.5f, 0.5f, 0.5f);
				pForegroundTransform->SetWorldScale(0.0f, 0.0f, 0.0f);
				pForegroundTransform->SetWorldPosition(DEVICE_RESOLUTION.iWidth / 2.0f, DEVICE_RESOLUTION.iHeight / 2.0f, 0.0f);
				pForegroundMaterial->SetDiffuseTexture("DefaultTexture", L"DefaultTexture.png", 0, 0, TEXTURE_PATH);
				pForegroundMaterial->SetDiffuseColor(Vector4::clear);

				pBackgroundTransform->SetPivot(0.5f, 0.5f, 0.5f);
				pBackgroundTransform->SetWorldScale(DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 1.0f);
				pBackgroundTransform->SetWorldPosition(DEVICE_RESOLUTION.iWidth / 2.0f, DEVICE_RESOLUTION.iHeight / 2.0f, 0.0f);
				pBackgroundMaterial->SetDiffuseTexture("DefaultTexture", L"DefaultTexture.png", 0, 0, TEXTURE_PATH);
				pBackgroundMaterial->SetDiffuseColor(Vector4::black);

				m_ePhase = SIP_001;
				m_fTimer = 0.0f;
			}
			else
			{
				break;
			}
		}
		break;

		case SIP_001:
		{
			float fProgress = m_fTimer / m_fFadingTime;
			Vector4 vColor = pBackgroundMaterial->GetDiffuseColor();
			vColor.w = 1.0f - fProgress;
			pBackgroundMaterial->SetDiffuseColor(vColor);
			if (fProgress >= 1.0f)
			{
				m_eSplashState = SPLASH_DESTROY;
			}
		}
			break;
		default:
			break;
		}

		SAFE_RELEASE(pForegroundMaterial);
		SAFE_RELEASE(pBackgroundMaterial);
		SAFE_RELEASE(pForegoundRenderer);
		SAFE_RELEASE(pBackgroundRenderer);
		SAFE_RELEASE(pForegroundTransform);
		SAFE_RELEASE(pBackgroundTransform);

	}
	else if (m_eSplashState == SPLASH_EXIT)
	{
		m_fTimer += fTime;
		CTransform* pForegroundTransform = m_pForegroundPanel->GetTransform();
		CTransform* pBackgroundTransform = m_pBackgroundPanel->GetTransform();
		CSpriteRenderer* pBackgroundRenderer = m_pBackgroundPanel->GetComponent<CSpriteRenderer>();
		CSpriteRenderer* pForegoundRenderer = m_pForegroundPanel->GetComponent<CSpriteRenderer>();
		CMaterial* pBackgroundMaterial = pBackgroundRenderer->GetMaterial();
		CMaterial* pForegroundMaterial = pForegoundRenderer->GetMaterial();

		switch (m_ePhase)
		{
		case SIP_NIL:
		{
			//Initialize
			pForegroundTransform->SetPivot(0.5f, 0.5f, 0.5f);
			pForegroundTransform->SetWorldScale(0.0f, 0.0f, 0.0f);
			pForegroundTransform->SetWorldPosition(DEVICE_RESOLUTION.iWidth / 2.0f, DEVICE_RESOLUTION.iHeight / 2.0f, 0.0f);
			pForegroundMaterial->SetDiffuseTexture("DefaultTexture", L"DefaultTexture.png", 0, 0, TEXTURE_PATH);
			pForegroundMaterial->SetDiffuseColor(Vector4::clear);

			pBackgroundTransform->SetPivot(0.5f, 0.5f, 0.5f);
			pBackgroundTransform->SetWorldScale(DEVICE_RESOLUTION.iWidth, DEVICE_RESOLUTION.iHeight, 1.0f);
			pBackgroundTransform->SetWorldPosition(DEVICE_RESOLUTION.iWidth / 2.0f, DEVICE_RESOLUTION.iHeight / 2.0f, 0.0f);
			pBackgroundMaterial->SetDiffuseTexture("DefaultTexture", L"DefaultTexture.png", 0, 0, TEXTURE_PATH);
			pBackgroundMaterial->SetDiffuseColor(Vector4::clear);

			m_ePhase = SIP_000;
			m_fTimer = 0.0f;
		}
		break;
		case SIP_000:
		{
			//Background Fade Out
			float fProgress = m_fTimer / m_fFadingTime;
			Vector4 vColor = pBackgroundMaterial->GetDiffuseColor();
			vColor.w = fProgress;
			pBackgroundMaterial->SetDiffuseColor(vColor);
			if (fProgress >= 1.0f)
			{
				//Start To Loading
				pForegroundTransform->SetPivot(0.5f, 0.5f, 0.5f);
				pForegroundTransform->SetWorldScale(768.0f, 384.0f, 1.0f);
				pForegroundTransform->SetWorldPosition(DEVICE_RESOLUTION.iWidth / 2.0f, DEVICE_RESOLUTION.iHeight / 2.0f, 0.0f);

				pForegoundRenderer->SetShader(LOADING_SHADER);
				pForegroundMaterial->SetDiffuseTexture("LoadingTitle", L"UI\\LoadingTitle.png", 0, 0, TEXTURE_PATH);
				pForegroundMaterial->SetSampler(SAMPLER_LINEAR);
				pForegroundMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
				m_fTimer = 0.0f;
				m_ePhase = SIP_001;

				CLoadingPanel* pLoadingPanel = m_pForegroundPanel->GetComponent<CLoadingPanel>();
				pLoadingPanel->SetLoadingProgress(0.0f);
				SAFE_RELEASE(pLoadingPanel);
			}
		}
		break;
		case SIP_001:
		{
			//On Loading Entry Fade In
			float fProgress = m_fTimer / m_fFadingTime;
			Vector4 vColor = pForegroundMaterial->GetDiffuseColor();
			vColor.w = fProgress;
			pForegroundMaterial->SetDiffuseColor(vColor);
			if (fProgress >= 1.0f)
			{
				m_ePhase = SIP_002;
				m_fTimer = 0.0f;

				CLoadingPanel* pLoadingPanel = m_pForegroundPanel->GetComponent<CLoadingPanel>();
				pLoadingPanel->SetLoadingProgress(0.0f);
				SAFE_RELEASE(pLoadingPanel);

				if (!m_strPath.empty())
				{
					GET_SINGLE(CSceneManager)->CreateSceneAsynchronously(m_strPath.c_str(), m_strPathKey);
				}
			}
		}
		break;
		case SIP_002:
		{
			//On Loading
			bool bOnLoading = GET_SINGLE(CSceneManager)->OnLoading();
			if (bOnLoading)
			{
				float fProgress = GET_SINGLE(CSceneManager)->LoadingProgress();
				if (fProgress <= 1.0f)
				{
					CLoadingPanel* pLoadingPanel = m_pForegroundPanel->GetComponent<CLoadingPanel>();
					pLoadingPanel->SetLoadingProgress(fProgress);
					SAFE_RELEASE(pLoadingPanel);
				}
			}
			else
			{
				//로딩이 끝나거나 시작하지 않았을 떄
				CThread* pThread = GET_SINGLE(CThreadManager)->FindThread("Loading");
				if (pThread)
				{
					m_pLoadedScene = ((CLoadingThread*)pThread)->GetScene();
					if (m_pLoadedScene)
					{
						GET_SINGLE(CThreadManager)->RemoveThread("Loading");
						m_ePhase = SIP_003;
						m_fTimer = 0.0f;
						CLoadingPanel* pLoadingPanel = m_pForegroundPanel->GetComponent<CLoadingPanel>();
						pLoadingPanel->SetLoadingProgress(1.0f);
						SAFE_RELEASE(pLoadingPanel);
					}
				}
			}
		}break;
		case SIP_003:
		{
			//Completed Delay
			float fProgress = m_fTimer / (m_fFadingTime * 6.0f);
			Vector4 vColor = pForegroundMaterial->GetDiffuseColor();
			vColor.w = 1.0f;
			pForegroundMaterial->SetDiffuseColor(vColor);
			if (fProgress >= 1.0f)
			{
				m_ePhase = SIP_004;
				m_fTimer = 0.0f;
				CLoadingPanel* pLoadingPanel = m_pForegroundPanel->GetComponent<CLoadingPanel>();
				pLoadingPanel->SetLoadingProgress(1.0f);
				SAFE_RELEASE(pLoadingPanel);
			}
		}break;
		case SIP_004:
		{
			//Loading Fade Out
			float fProgress = m_fTimer / m_fFadingTime;
			Vector4 vColor = pForegroundMaterial->GetDiffuseColor();
			vColor.w = 1.0f - fProgress;
			pForegroundMaterial->SetDiffuseColor(vColor);
			if (fProgress >= 1.0f)
			{
				m_ePhase = SIP_010;
				m_fTimer = 0.0f;
				CLoadingPanel* pLoadingPanel = m_pForegroundPanel->GetComponent<CLoadingPanel>();
				pLoadingPanel->SetLoadingProgress(1.0f);
				SAFE_RELEASE(pLoadingPanel);

				if (m_pLoadedScene)
				{
					GET_SINGLE(CDataManager)->LoadPlayer(m_pLoadedScene);
					GET_SINGLE(CSceneManager)->SetNextScene(m_pLoadedScene);
					SAFE_RELEASE(m_pLoadedScene);
					m_eSplashState = SPLASH_DESTROY;
				}
			}
		}break;
		default:
			break;
		}

		SAFE_RELEASE(pForegroundMaterial);
		SAFE_RELEASE(pBackgroundMaterial);
		SAFE_RELEASE(pForegoundRenderer);
		SAFE_RELEASE(pBackgroundRenderer);
		SAFE_RELEASE(pForegroundTransform);
		SAFE_RELEASE(pBackgroundTransform);

	}
	
	if (m_eSplashState == SPLASH_DESTROY)
	{
		//m_pForegroundPanel->Destroy(true);
		//m_pBackgroundPanel->Destroy(true);
		Destroy(true);
	}

	return 0;
}

bool CSplash::Save(FILE * pFile)
{
	size_t iLength = m_strPath.length();
	fwrite(&iLength, sizeof(size_t), 1, pFile);
	fwrite(m_strPath.c_str(), sizeof(wchar_t), iLength, pFile);

	iLength = m_strPathKey.length();
	fwrite(&iLength, sizeof(size_t), 1, pFile);
	fwrite(m_strPathKey.c_str(), sizeof(char), iLength, pFile);
	
	fwrite(&m_fFadingTime, sizeof(float), 1, pFile);

	return true;
}

bool CSplash::Load(FILE * pFile)
{
	size_t iLength = 0;
	wchar_t strPath[512] = {};
	char strPathKey[256] = {};

	fread(&iLength, sizeof(size_t), 1, pFile);
	fread(strPath, sizeof(wchar_t), iLength, pFile);
	m_strPath = strPath;
	iLength = 0;
	fread(&iLength, sizeof(size_t), 1, pFile);
	fread(strPathKey, sizeof(char), iLength, pFile);
	
	m_strPathKey = strPathKey;
	fread(&m_fFadingTime, sizeof(float), 1, pFile);	

	return true;
}
