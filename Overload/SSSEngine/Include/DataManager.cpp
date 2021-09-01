#include "DataManager.h"
#include "PathManager.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Transform.h"

SSS_USING

DEFINITION_SINGLE(CDataManager)

CDataManager::CDataManager():
	m_pCurrentGameSystem(NULL)
{
	m_tPlayerData.iEventProgress = 0;
	m_tPlayerData.iTransitionMapFrom = FLT_MAX;
	m_tPlayerData.iTransitionMapTo = FLT_MAX;
	m_pCameraGimbal = NULL;
	m_pPlayer = NULL;
}


CDataManager::~CDataManager()
{
	unordered_map<string, PATLASDATA>::iterator iter;
	unordered_map<string, PATLASDATA>::iterator iterEnd = m_mapAtlasData.end();

	for (iter = m_mapAtlasData.begin(); iter != iterEnd; ++iter)
	{
		delete iter->second;
	}
	m_mapAtlasData.clear();
}


PLAYERDATA CDataManager::GetPlayerData() const
{
	return m_tPlayerData;
}

CGameObject * CDataManager::GetGameSystem() const
{
	return m_pCurrentGameSystem;
}

CGameObject * CDataManager::GetCameraGimbal() const
{
	return m_pCameraGimbal;
}

CGameObject * CDataManager::GetPlayer() const
{
	return m_pPlayer;
}

void CDataManager::SetPlayerData(PLAYERDATA tData)
{
	m_tPlayerData = tData;
}

void CDataManager::SetGameSystem(CGameObject * pGameObject)
{
	m_pCurrentGameSystem = pGameObject;
}

void CDataManager::LoadPlayer(CScene * pScene)
{
	CScene* pSpawnScene = pScene;
	if (!pScene)
	{
		pSpawnScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
		pSpawnScene->Release();
	}

	if (!pSpawnScene)
		return;


	//PLS_TERNY_DAY = 0,
	//PLS_TERNY_NIGHT,
	//PLS_KINGSROAD_DAY,
	//PLS_KINGSROAD_NIGHT,
	//PLS_BATTLFIELD_DAY

	Vector3 vSpawnPosition;
	Vector3 vSpawnRotation;


	//To King's Road
	 if (m_tPlayerData.iTransitionMapFrom == 15)
	{
		vSpawnPosition = Vector3(8.7603f, 28.0398f, 70.736f);
		vSpawnRotation = Vector3(0.0f, 0.0f, 0.0f);
	}
	else if (m_tPlayerData.iTransitionMapTo == 2 || m_tPlayerData.iTransitionMapTo == 3)
	{
		if (m_tPlayerData.iTransitionMapFrom == 0 || m_tPlayerData.iTransitionMapFrom == 1)
		{
			vSpawnPosition = Vector3(210.0f, 2.25f, 50.0f);
			vSpawnRotation = Vector3(0.0f, -114.0f * D2R, 0.0f);
		}
		else if (m_tPlayerData.iTransitionMapFrom == 4)
		{
			vSpawnPosition = Vector3(-207.50f, 0.0f, -84.0f);
			vSpawnRotation = Vector3(0.0f, PI / 2.0f, 0.0f);
		}
	}
	//To Terny
	else if (m_tPlayerData.iTransitionMapTo == 0 || m_tPlayerData.iTransitionMapTo == 1)
	{
		vSpawnPosition = Vector3(-148.0f, 0.4f, -178.0f);
		vSpawnRotation = Vector3(0.0f, PI / 4.0f, 0.0f);
	}
	else if (m_tPlayerData.iTransitionMapTo == 4)
	{
		vSpawnPosition = Vector3(172.5f, 3.89f, -177.5f);
		vSpawnRotation = Vector3(0.0f, -PI / 4.0f, 0.0f);
	}

	CGameObject* pPlayer = CGameObject::Load(pSpawnScene, PREFAB_PLAYER_RELATIVE_PATH, DATA_PATH);	
	CTransform* pTransform = pPlayer->GetTransform();
	pTransform->SetWorldPosition(vSpawnPosition);
	pTransform->SetWorldRotation(vSpawnRotation);
	SAFE_RELEASE(pTransform);

	m_pPlayer = pPlayer;
	SAFE_RELEASE(pPlayer);

	CGameObject* pCameraGimbal = CGameObject::Load(pSpawnScene, PREFAB_CAMERA_GIMBAL_RELATIVE_PATH, DATA_PATH);
	m_pCameraGimbal = pCameraGimbal;
	SAFE_RELEASE(pCameraGimbal);
}


bool CDataManager::LoadPSAClipInfo(__out vector<PANIMATIONCLIP>* vecClipInfo, bool bReplaceAll, const wchar_t * pFileName, const string & strPathKey)
{
	wstring wstrFileName ;

	if (strPathKey != NO_USE_PATH)
	{
		wstrFileName = GET_SINGLE(CPathManager)->FindPath(strPathKey);
	}
	wstrFileName += pFileName;
	string strFullPath(wstrFileName.begin(), wstrFileName.end());
	
	FILE* pFile = NULL;
	fopen_s(&pFile, strFullPath.c_str(), "rb");
	if (pFile)
	{
		PANIMATIONCLIP pEndIndex = NULL;

		if (bReplaceAll)
		{
			Safe_Delete_VecList(*vecClipInfo);
			vecClipInfo->clear();
		}
		else
		{
			if (vecClipInfo->size() > 0)
				pEndIndex = (*vecClipInfo)[vecClipInfo->size() - 1];
			else
				pEndIndex = NULL;
		}
		vector<PSABONE> vecBone;
		vector<PSACLIPINFO> vecClip;

		PSAChunkHeader* pAnimHeader = new PSAChunkHeader;
		PSAChunkHeader* pBoneHeader = new PSAChunkHeader;
		PSAChunkHeader* pClipInfoHeader = new PSAChunkHeader;

		//FILE HEADER
		pAnimHeader->Load(pFile);

		//BONE
		pBoneHeader->Load(pFile);
		for (uint32_t i = 0; i < pBoneHeader->iDataCount; ++i)
		{
			PSABONE tBone;
			tBone.Load(pFile);
			vecBone.push_back(tBone);
		}

		//ANIM CLIP INFO
		pClipInfoHeader->Load(pFile);
		for (uint32_t i = 0; i < pClipInfoHeader->iDataCount; ++i)
		{
			PSACLIPINFO tInfo;
			tInfo.Load(pFile);
			vecClip.push_back(tInfo);
		
			PANIMATIONCLIP pClipInfo = new ANIMATIONCLIP;
			pClipInfo->strName = tInfo.strClipName;
			if (bReplaceAll)
			{
				pClipInfo->iStartFrame = tInfo.iStartFrameIndex;
				pClipInfo->iEndFrame = tInfo.iEndFrameIndex;
				pClipInfo->iLength = pClipInfo->iEndFrame - pClipInfo->iStartFrame + 1;
			}
			else
			{
				int iEnd = -1;
				if (pEndIndex)
				{
					iEnd = pEndIndex->iEndFrame;
				}

				uint32_t iLength = tInfo.iEndFrameIndex - tInfo.iStartFrameIndex + 1;
				pClipInfo->iStartFrame = iEnd + tInfo.iStartFrameIndex + 1;
				pClipInfo->iEndFrame = iEnd + tInfo.iEndFrameIndex + 1;
				pClipInfo->iLength = iLength;
			}
			vecClipInfo->push_back(pClipInfo);
		}		


		delete pAnimHeader ;
		delete pBoneHeader ;
		delete pClipInfoHeader;

		fclose(pFile);
		return true;
	}
	else
	{
		return false;
	}
}

/*
작성 : 김은수
일자 : 2018. 07. 15
요약 : Texture Metadata 파일 로드.
내용 : Atlas Texture에 관련된 Metadata 파일 로드.  'pFileName' 에 확장자를 제외한 Directory를 기입합니다.
반환 : 파일 로드에 실패했을 경우 false를 반환합니다.
*/
bool CDataManager::LoadAtlasData(const wchar_t * pFileName, const string & strPathKey)
{
	char strTemp[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, pFileName, -1, strTemp, lstrlenW(pFileName) * 2, 0, 0);
	string strFullPath = GET_SINGLE(CPathManager)->FindPathMultiByte(strPathKey);
	strFullPath += string(strTemp) + string(".meta");

	FILE* pFile = NULL;
	fopen_s(&pFile, strFullPath.c_str(), "rb");

	if (!pFile)
		return false;
	else
	{
		METADATA_FLAG eFlag = MF_NONE;
		fread(&eFlag, sizeof(METADATA_FLAG), 1, pFile);

		if (eFlag != MF_ATLAS)
		{
			fclose(pFile);
			return false;
		}

		int iCount = 0;
		fread(&iCount, 4, 1, pFile);

		for (int i = 0; i < iCount; ++i)
		{
			ATLASDATA tData = {};
			char strKey[512] = {};

			int iLength = 0;
			fread(&iLength, 4, 1, pFile);
			fread(&strKey, iLength, 1, pFile);
			tData.strKey = strKey;

			fread(&tData.wX, 2, 1, pFile);
			fread(&tData.wY, 2, 1, pFile);
			fread(&tData.wWidth, 2, 1, pFile);
			fread(&tData.wHeight, 2, 1, pFile);
			fread(&tData.iAtlasWidth, 4, 1, pFile);
			fread(&tData.iAtlasHeight, 4, 1, pFile);

			if (tData.wHeight == 0 && tData.wWidth == 0 && tData.wX == 0 && tData.wY == 0)
			{
				continue;
			}
			
			InsertAtlasData(strKey, tData.wX, tData.wY, tData.wWidth, tData.wHeight, tData.iAtlasWidth, tData.iAtlasHeight);
		}

		fclose(pFile);
		return true;
	}
}



/*
작성 : 김은수
일자 : 2018. 07. 15
요약 : Texture Metadata 파일 세이브.
내용 : 현재 DataManager에 등록되어져 있는 모든 Atlas Texture Metadata를 저장합니다. 'pFileName' 에 확장자를 제외한 Directory를 기입합니다.
반환 : 파일 로드에 실패했을 경우 false를 반환합니다.
*/
bool CDataManager::SaveAtlasData(const wchar_t * pFileName, const string & strPathKey) const
{
	char strTemp[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, pFileName, -1, strTemp, lstrlenW(pFileName) * 2, 0, 0);
	string strFullPath = GET_SINGLE(CPathManager)->FindPathMultiByte(strPathKey);
	strFullPath += string(strTemp) + string(".meta");

	FILE* pFile = NULL;
	fopen_s(&pFile, strFullPath.c_str(), "wb");

	if (!pFile)
	{
		return false;
	}
	else
	{
		METADATA_FLAG eFlag = MF_ATLAS;
		fwrite(&eFlag, sizeof(METADATA_FLAG), 1, pFile);

		int iCount = m_mapAtlasData.size();
		fwrite(&iCount, 4, 1, pFile);

		unordered_map<string, PATLASDATA>::const_iterator iter;
		unordered_map<string, PATLASDATA>::const_iterator iterEnd = m_mapAtlasData.end();

		for (iter = m_mapAtlasData.begin(); iter != iterEnd; ++iter)
		{
			PATLASDATA pData = iter->second;

			int iLength = pData->strKey.length();
			char strKey[512] = {};
			strcpy_s(strKey, pData->strKey.c_str());

			fwrite(&iLength, 4, 1, pFile);
			fwrite(&strKey, iLength, 1, pFile);
			fwrite(&pData->wX, 2, 1, pFile);
			fwrite(&pData->wY, 2, 1, pFile);
			fwrite(&pData->wWidth, 2, 1, pFile);
			fwrite(&pData->wHeight, 2, 1, pFile);
			fwrite(&pData->iAtlasWidth, 4, 1, pFile);
			fwrite(&pData->iAtlasHeight, 4, 1, pFile);			
		}
		fclose(pFile);
		return true;
	}
}

/*
작성 : 김은수
일자 : 2018. 07. 15
요약 : Texture Metadata 파일 세이브.
내용 : 외부의 Atlas Texture Metadata를 저장합니다. 'pFileName' 에 확장자를 제외한 Directory를 기입합니다.
반환 : 파일 로드에 실패했을 경우 false를 반환합니다.
*/
bool CDataManager::SaveAtlasData(unordered_map<string, PATLASDATA>* mapData, const wchar_t * pFileName, const string & strPathKey) const
{
	char strTemp[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, pFileName, -1, strTemp, lstrlenW(pFileName) * 2, 0, 0);
	string strFullPath = GET_SINGLE(CPathManager)->FindPathMultiByte(strPathKey);
	strFullPath += string(strTemp) + string(".meta");

	FILE* pFile = NULL;
	fopen_s(&pFile, strFullPath.c_str(), "wb");

	if (!pFile)
	{
		return false;
	}
	else
	{
		METADATA_FLAG eFlag = MF_ATLAS;
		fwrite(&eFlag, sizeof(METADATA_FLAG), 1, pFile);

		int iCount = mapData->size();
		fwrite(&iCount, 4, 1, pFile);

		unordered_map<string, PATLASDATA>::const_iterator iter;
		unordered_map<string, PATLASDATA>::const_iterator iterEnd = mapData->end();

		for (iter = mapData->begin(); iter != iterEnd; ++iter)
		{
			PATLASDATA pData = iter->second;

			int iLength = pData->strKey.length();
			char strKey[512] = {};
			strcpy_s(strKey, pData->strKey.c_str());

			fwrite(&iLength, 4, 1, pFile);
			fwrite(&strKey, iLength, 1, pFile);
			fwrite(&pData->wX, 2, 1, pFile);
			fwrite(&pData->wY, 2, 1, pFile);
			fwrite(&pData->wWidth, 2, 1, pFile);
			fwrite(&pData->wHeight, 2, 1, pFile);
			fwrite(&pData->iAtlasWidth, 4, 1, pFile);
			fwrite(&pData->iAtlasHeight, 4, 1, pFile);
		}
		fclose(pFile);
		return true;
	}
}

/*
작성 : 김은수
일자 : 2018. 07. 15
요약 : Insert Texture Metadata.
내용 : 임의의 Atlas Texture Metadata를 DataManager 클래스 내부 변수에 추가합니다.
반환 : strKey가 중복되는 경우 false를 반환합니다.
*/
bool CDataManager::InsertAtlasData(const string & strKey, unsigned short wStartX, unsigned short wStartY,
	unsigned short wWidth, unsigned short wHeight, unsigned int iTextureWidth, unsigned int iTextureHeight)
{
	unordered_map<string, PATLASDATA>::iterator iter = m_mapAtlasData.find(strKey);

	if (iter == m_mapAtlasData.end())
	{
		PATLASDATA pData = new ATLASDATA;

		pData->strKey = strKey;
		pData->wX = wStartX;
		pData->wY = wStartY;
		pData->wWidth = wWidth;
		pData->wHeight = wHeight;
		pData->iAtlasWidth = iTextureWidth;
		pData->iAtlasHeight = iTextureHeight;

		m_mapAtlasData.insert(make_pair(strKey, pData));

		return true;
	}
	else
	{
		return false;
	}
}

/*
작성 : 김은수
일자 : 2018. 07. 15
요약 : Remove Texture Metadata.
내용 : DataManager클래스 내의 Atlas Texture Metadata 중 strKey와 일치하는 데이터를 제거합니다.
반환 : 제거에 성공했을 경우 true를 반환, 그외의 모든 경우는 false를 반환합니다.
*/
bool CDataManager::RemoveAtlasData(const string & strKey)
{
	unordered_map<string, PATLASDATA>::iterator iter = m_mapAtlasData.find(strKey);

	if (iter == m_mapAtlasData.end())
	{
		return false;
	}
	else if (iter->second)
	{
		delete iter->second;
		m_mapAtlasData.erase(iter);
		return true;
	}
	else
	{
		return false;
	}
}

/*
작성 : 김은수
일자 : 2018. 07. 15
요약 : Get Texture Metadata.
내용 : DataManager클래스 내의 Atlas Texture Metadata 중 strKey와 일치하는 데이터를 검색합니다.
반환 : strKey가 존재하지 않는 경우 NULL을 반환합니다.
*/
PATLASDATA CDataManager::GetAtlasData(const string & strKey)
{
	unordered_map<string, PATLASDATA>::iterator iter = m_mapAtlasData.find(strKey);

	if (iter == m_mapAtlasData.end())
	{
		return NULL;
	}

	return iter->second;
}

bool CDataManager::Initialize()
{
	return true;
}
