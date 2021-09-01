#include "stdafx.h"
#include "EditScene.h"
#include "Component/Camera.h"
#include "Transform.h"
#include "Layer.h"
#include "SceneManager.h"
#include "Input.h"
#include "Component.h"
#include "GameObject.h"
#include "SoundManager.h"
#include "Component/Text.h"
#include "ClientComponent\Chat.h"

CEditScene::CEditScene()
{
}


CEditScene::~CEditScene()
{
}

bool CEditScene::Initialize()
{
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_BGM, "TestBGM0", true, "UseSound/Battle1_Loop.ogg", SOUND_PATH);
	GET_SINGLE(CSoundManager)->LoadSound(SOUND_BGM, "TestBGM1", true, "UseSound/BloodyBattle_Full.ogg", SOUND_PATH);
	//GET_SINGLE(CSoundManager)->Volume(SOUND_BGM, 0.1f);
	//GET_SINGLE(CSoundManager)->Play("TestBGM");
	//CGameObject* pCamera = this->GetMainCameraObject();
	//SAFE_RELEASE(pCamera);

	return true;
}

