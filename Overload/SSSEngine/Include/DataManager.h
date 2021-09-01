#pragma once
#include "Engine.h"
#include "Mesh.h"
SSS_BEGIN

/*
작성 : 김은수
일자 : 2018. 07. 15
요약 : Data Manager
내용 : 게임 내에서 쓰일 Metadata 전체를 관리하는 싱글턴 클래스 입니다.
비고 : Resource 파일은 여기서 관리 안함. 
*/

typedef struct _tagPSAChunkHeader
{
	char strChunkID[20];
	uint32_t iTypeFlag;
	uint32_t iDataSize;
	uint32_t iDataCount;

	bool Load(FILE* pFile)
	{
		if (pFile)
		{
			fread(&strChunkID, sizeof(char), 20, pFile);
			fread(&iTypeFlag, sizeof(uint32_t), 1, pFile);
			fread(&iDataSize, sizeof(uint32_t), 1, pFile);
			fread(&iDataCount, sizeof(uint32_t), 1, pFile);

			return true;
		}
		return false;
	}

}PSAChunkHeader, *PPSAChunkHeader;

typedef struct _tagPSABone
{
	char strBoneName[64];
	uint32_t iFlags;
	uint32_t iNumChildren;
	int32_t iParentIndex;
	Vector3 vPosition;
	Vector3 vRotation;
	Vector3 vScale;
	double fLength;

	bool Load(FILE* pFile)
	{
		if (pFile)
		{
			fread(strBoneName, sizeof(char), 64, pFile);
			fread(&iFlags, sizeof(uint32_t), 1, pFile);
			fread(&iNumChildren, sizeof(uint32_t), 1, pFile);
			fread(&iParentIndex, sizeof(int32_t), 1, pFile);

			fread(&vPosition, sizeof(Vector3), 1, pFile);
			fread(&vRotation, sizeof(Vector3), 1, pFile);
			fread(&vScale, sizeof(Vector3), 1, pFile);
			fread(&fLength, sizeof(double), 1, pFile);
			return true;
		}

		return false;
	}
}PSABONE, *PPSABONE;


typedef struct _tagPSAClipInfo
{
	char strClipName[64];
	char strGroupName[64];
	int32_t iTotalBones;
	int32_t iRootInclud;
	int32_t iCompressedType;
	int32_t iQuotum;
	float fReduction;
	float fTrackTime;
	float fAnimRate;
	int32_t iStartBone;
	int32_t iFirstRawFrame;
	int32_t iNumRawFrames;

	uint32_t iStartFrameIndex;
	uint32_t iEndFrameIndex;

	bool Load(FILE* pFile)
	{
		if (pFile)
		{
			fread(strClipName, sizeof(char), 64, pFile);
			fread(strGroupName, sizeof(char), 64, pFile);

			fread(&iTotalBones, sizeof(int32_t), 1, pFile);
			fread(&iRootInclud, sizeof(int32_t), 1, pFile);
			fread(&iCompressedType, sizeof(int32_t), 1, pFile);
			fread(&iQuotum, sizeof(int32_t), 1, pFile);
			fread(&fReduction, sizeof(float), 1, pFile);
			fread(&fTrackTime, sizeof(float), 1, pFile);
			fread(&fAnimRate, sizeof(float), 1, pFile);

			fread(&iStartBone, sizeof(int32_t), 1, pFile);
			fread(&iFirstRawFrame, sizeof(int32_t), 1, pFile);
			fread(&iNumRawFrames, sizeof(int32_t), 1, pFile);

			iStartFrameIndex = iFirstRawFrame;
			iEndFrameIndex = iStartFrameIndex + iNumRawFrames - 1;
			return true;
		}
		return false;
	}
}PSACLIPINFO, *PPSACLIPINFO;


typedef SSS_DLL struct _tagPlayerData
{
	uint32_t iTransitionMapFrom;
	uint32_t iTransitionMapTo;
	uint32_t iEventProgress;
	uint32_t iQuestProgress;
	uint64_t iOptionFlag;

	string strRelativeFileDirectory;
	string strFilePathKey;
}PLAYERDATA, *PPLAYERDATA;


class SSS_DLL CDataManager
{
	DECLARE_SINGLE(CDataManager)

private:
	friend class CCore;
	unordered_map<string , PATLASDATA> m_mapAtlasData;

private:
	class CGameObject* m_pCurrentGameSystem;
	class CGameObject* m_pCameraGimbal;
	class CGameObject* m_pPlayer;
	PLAYERDATA m_tPlayerData;

public:
	class CGameObject* GetCameraGimbal() const;
	class CGameObject* GetPlayer() const;
	
	
	void SetPlayerData(PLAYERDATA tData);
	void SetGameSystem(class CGameObject* pGameObject);
	void LoadPlayer(class CScene* pScene = NULL);

	PLAYERDATA GetPlayerData() const;
	class CGameObject* GetGameSystem() const;

	bool LoadPSAClipInfo(__out vector<PANIMATIONCLIP>* vecClipInfo, bool bReplaceAll, const wchar_t* pFileName, const string& strPathKey = DATA_PATH);
	bool LoadAtlasData(const wchar_t* pFileName, const string& strPathKey = DATA_PATH);
	bool SaveAtlasData(const wchar_t* pFileName, const string& strPathKey = DATA_PATH) const;
	bool SaveAtlasData(unordered_map<string, PATLASDATA>* mapData, const wchar_t* pFileName, const string& strPathKey = DATA_PATH) const;
	bool InsertAtlasData(const string& strKey, unsigned short wStartX, unsigned short wStartY, unsigned short wWidth, unsigned short wHeight, unsigned int iTextureWidth, unsigned int iTextureHeight);
	bool RemoveAtlasData(const string& strKey);
	PATLASDATA GetAtlasData(const string& strKey);

private:
	bool Initialize();
};

SSS_END
