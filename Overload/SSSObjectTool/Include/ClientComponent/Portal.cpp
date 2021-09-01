#include "Portal.h"

#include "SceneManager.h"
#include "PlayerController.h"
#include "CollisionManager.h"
#include "Collider.h"
#include "SoundManager.h"
#include "DataManager.h"
#include "GameSystem.h"
#include "Splash.h"


void CPortal::SetSceneType(POTAL_LOAD_SCENE eState)
{
	m_eSceneState = eState;
}

POTAL_LOAD_SCENE CPortal::GetScene()
{
	return m_eSceneState;
}

CPortal::CPortal()
{
}


CPortal::~CPortal()
{
}

bool CPortal::Initialize()
{
	m_eSceneState = PLS_NONE;
	return true;
}

void CPortal::Start()
{
}

int CPortal::Update(float fTime)
{
	return 0;
}

int CPortal::OnCollisionEnter(CCollider * pThis, CCollider * pTarget, float fTime)
{
	CGameObject*	pTargetObject = pTarget->GetGameObject();
	if (pTargetObject)
	{
		if (pTargetObject->HasComponent<CPlayerController>())
		{
			////이벤트 확인
			{
				CGameSystem* pSystemComponent = GET_SINGLE(CDataManager)->GetGameSystem()->GetComponent<CGameSystem>();
				uint32_t iCurrentScene = pSystemComponent->GetSceneID();
				SAFE_RELEASE(pSystemComponent);
			
				if (iCurrentScene == static_cast<uint32_t>(PLS_BATTLFIELD_DAY))
				{
					PLAYERDATA tData = GET_SINGLE(CDataManager)->GetPlayerData();
					//완료
					if (tData.iQuestProgress == 30)
					{
						m_eSceneState = PLS_KINGSROAD_NIGHT;
					}
					//미완료
					else
					{
						m_eSceneState = PLS_KINGSROAD_DAY;
					}
				}
			}

			GET_SINGLE(CSoundManager)->Stop(SOUND_BGM);
			GET_SINGLE(CSoundManager)->Stop(SOUND_ENV);
	
			switch (m_eSceneState)
			{
			case PLS_TERNY_DAY:
			{
				CGameSystem* pSystemComponent = GET_SINGLE(CDataManager)->GetGameSystem()->GetComponent<CGameSystem>();
				uint32_t iCurrentScene = pSystemComponent->GetSceneID();
				SAFE_RELEASE(pSystemComponent);
				
				PLAYERDATA tData = GET_SINGLE(CDataManager)->GetPlayerData();
				tData.iTransitionMapFrom = iCurrentScene;
				tData.iTransitionMapTo = static_cast<uint32_t>(PLS_TERNY_DAY);
				GET_SINGLE(CDataManager)->SetPlayerData(tData);
	
				CGameObject* pSystem = GET_SINGLE(CDataManager)->GetGameSystem();
				CSplash* pSplash = pSystem->AddComponent<CSplash>();
				pSplash->SetSplashState(SPLASH_EXIT);
				pSplash->SetPath(SCENE_TERNY_DAY_RELATIVE_PATH);
				pSplash->SetPathKey(DATA_PATH);
				SAFE_RELEASE(pSplash);
			}
			break;
			case PLS_TERNY_NIGHT:
			{
				CGameSystem* pSystemComponent = GET_SINGLE(CDataManager)->GetGameSystem()->GetComponent<CGameSystem>();
				uint32_t iCurrentScene = pSystemComponent->GetSceneID();
				SAFE_RELEASE(pSystemComponent);
	
				PLAYERDATA tData = GET_SINGLE(CDataManager)->GetPlayerData();
				tData.iTransitionMapFrom = iCurrentScene;
				tData.iTransitionMapTo = static_cast<uint32_t>(PLS_TERNY_NIGHT);
				GET_SINGLE(CDataManager)->SetPlayerData(tData);
	
				CGameObject* pSystem = GET_SINGLE(CDataManager)->GetGameSystem();
				CSplash* pSplash = pSystem->AddComponent<CSplash>();
				pSplash->SetSplashState(SPLASH_EXIT);
				pSplash->SetPath(SCENE_TERNY_NIGHT_RELATIVE_PATH);
				pSplash->SetPathKey(DATA_PATH);
				SAFE_RELEASE(pSplash);
			}
			break;
			case PLS_KINGSROAD_DAY:
			{
				CGameSystem* pSystemComponent = GET_SINGLE(CDataManager)->GetGameSystem()->GetComponent<CGameSystem>();
				uint32_t iCurrentScene = pSystemComponent->GetSceneID();
				SAFE_RELEASE(pSystemComponent);
	
				PLAYERDATA tData = GET_SINGLE(CDataManager)->GetPlayerData();
				tData.iTransitionMapFrom = iCurrentScene;
				tData.iTransitionMapTo = static_cast<uint32_t>(PLS_KINGSROAD_DAY);
				GET_SINGLE(CDataManager)->SetPlayerData(tData);
	
				CGameObject* pSystem = GET_SINGLE(CDataManager)->GetGameSystem();
				CSplash* pSplash = pSystem->AddComponent<CSplash>();
				pSplash->SetSplashState(SPLASH_EXIT);
				pSplash->SetPath(SCENE_KINGSROAD_DAY_RELATIVE_PATH);
				pSplash->SetPathKey(DATA_PATH);
				SAFE_RELEASE(pSplash);
			}
			break;
			case PLS_KINGSROAD_NIGHT:
			{
				CGameSystem* pSystemComponent = GET_SINGLE(CDataManager)->GetGameSystem()->GetComponent<CGameSystem>();
				uint32_t iCurrentScene = pSystemComponent->GetSceneID();
				SAFE_RELEASE(pSystemComponent);
	
				PLAYERDATA tData = GET_SINGLE(CDataManager)->GetPlayerData();
				tData.iTransitionMapFrom = iCurrentScene;
				tData.iTransitionMapTo = static_cast<uint32_t>(PLS_KINGSROAD_NIGHT);
				GET_SINGLE(CDataManager)->SetPlayerData(tData);
	
				CGameObject* pSystem = GET_SINGLE(CDataManager)->GetGameSystem();
				CSplash* pSplash = pSystem->AddComponent<CSplash>();
				pSplash->SetSplashState(SPLASH_EXIT);
				pSplash->SetPath(SCENE_KINGSROAD_NIGHT_RELATIVE_PATH);
				pSplash->SetPathKey(DATA_PATH);
				SAFE_RELEASE(pSplash);
			}
			break;
			case PLS_BATTLFIELD_DAY:
			{
				CGameSystem* pSystemComponent = GET_SINGLE(CDataManager)->GetGameSystem()->GetComponent<CGameSystem>();
				uint32_t iCurrentScene = pSystemComponent->GetSceneID();
				SAFE_RELEASE(pSystemComponent);
	
				PLAYERDATA tData = GET_SINGLE(CDataManager)->GetPlayerData();
				tData.iTransitionMapFrom = iCurrentScene;
				tData.iTransitionMapTo = static_cast<uint32_t>(PLS_BATTLFIELD_DAY);
				GET_SINGLE(CDataManager)->SetPlayerData(tData);
	
				CGameObject* pSystem = GET_SINGLE(CDataManager)->GetGameSystem();
				CSplash* pSplash = pSystem->AddComponent<CSplash>();
				pSplash->SetSplashState(SPLASH_EXIT);
				pSplash->SetPath(SCENE_BATTLEFIELD_DAY_RELATIVE_PATH);
				pSplash->SetPathKey(DATA_PATH);
				SAFE_RELEASE(pSplash);
			}
			break;
			}
		}
		SAFE_RELEASE(pTargetObject);
	}
	return 0;
}

int CPortal::OnCollisionStay(CCollider * pThis, CCollider * pTarget, float fTime)
{	
	return 0;
}

int CPortal::OnCollisionExit(CCollider * pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

bool CPortal::Save(FILE * pFile)
{
	fwrite(&m_eSceneState, sizeof(POTAL_LOAD_SCENE), 1, pFile);
	return true;
}

bool CPortal::Load(FILE * pFile)
{
	fread(&m_eSceneState, sizeof(POTAL_LOAD_SCENE), 1, pFile);
	return true;
}
