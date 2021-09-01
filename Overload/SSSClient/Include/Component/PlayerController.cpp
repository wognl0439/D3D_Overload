#include "PlayerController.h"
#include "Transform.h"
#include "GameObject.h"
#include "Input.h"

#include "Component/DirectionalLight.h"
#include "Component/SpotLight.h"
#include "Component/PointLight.h"
#include "Collider.h"
#include "Scene.h"
#include "Layer.h"
#include "Component/PostProcessing.h"

#include "Debug.h"

#include "Component/Camera.h"
#include "Component/Animator.h"
#include "AnimationState.h"
#include "AnimationClip.h"
#include "Transition.h"

#include "Component/MeshRenderer.h"
#include "Component/SpriteRenderer.h"
#include "Component/Distortion.h"
#include "Component/Fire.h"
#include "Material.h"
#include "PlayerCamera.h"
#include "Mathf.h"
#include "CollisionManager.h"
#include "Collider.h"
#include "Component/TerrainCollider.h"
#include "Component/Trail.h"
#include "Component/SphereCollider.h"
#include "Attack.h"

#include "SoundManager.h"
#include "Device.h"
#include "EffectController.h"
#include "Component/Billboard.h"
#include "Core.h"
#include "Component/Particle.h"
#include "Component/RigidBody.h"
#include "Component/MeshCollider.h"
#include "Component/Decal.h"
#include "Component/Fire.h"

#include "PostOption.h"
#include "Component/BoxCollider.h"
#include "NPC_IDLE_AI.h"

CPlayerController::CPlayerController() :
	m_bPressed(false),
	m_fActiveTime(0.f),
	m_fRotateSpeed(90.f),
	m_bSoundPlay(false),
	m_bSkillEffect(false),
	m_bUseMeteo(false),
	m_bClickGround(false),
	m_vCursorClickPosition(Vector3::One),
	m_pSkillCircle(NULL)
{
	m_fMoveSpeed = 10.0f;
	m_fJumpHeight = 2.0f;
	m_fDamage = 2.0f;
	m_fDefence = 2.0f;
	m_fHP = 20.0f;
	m_fHPMax = 20.0f;
	m_fMP = 20.0f;
	m_fMPMax = 20.0f;
	m_fSP = 20.0f;
	m_fSPMax = 20.0f;

	m_fActiveTime = 0.f;
	m_fActiveMaxTime = 0.f;

	m_eStandardState = PLAYER_IDLE1;
	m_eBattleState = PLAYER_BATTLE_NONE;
	m_eDMGState = PLAYER_DAMAGED_NONE;
	for (int i = 0; i < 4; ++i)
	{
		if (m_pAttackTrail[i])
			m_pAttackTrail[i] = NULL;
	}

	m_pDummyForCameraWork = NULL;
	m_pStateController = NULL;
	m_pPlayerPointLight = NULL;

	m_bOnIdle = true;
}


CPlayerController::~CPlayerController()
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_pAttackTrail[i])
			SAFE_RELEASE(m_pAttackTrail[i]);
	}

	if (m_pSkillCircle)
		SAFE_RELEASE(m_pSkillCircle);
}

void CPlayerController::TransState(float fTime)
{
	if (m_eStandardState != PLAYER_STANDARD_NONE)
	{
		switch (m_eStandardState)
		{// switch 문의 여는 괄호입니다
		case PLAYER_EMOTION_1:
		case PLAYER_EMOTION_2:
		case PLAYER_EMOTION_3:
		case PLAYER_EMOTION_4:
		case PLAYER_EMOTION_5:
		case PLAYER_EMOTION_7:
		case PLAYER_EMOTION_8:
		case PLAYER_EMOTION_9:
		case PLAYER_EMOTION_10:
		case PLAYER_EMOTION_11:
		case PLAYER_EMOTION_12:
		case PLAYER_EMOTION_13:
		case PLAYER_EMOTION_14:
		case PLAYER_EMOTION_15:
		case PLAYER_EMOTION_16:
		case PLAYER_EMOTION_17:
		case PLAYER_EMOTION_18:
		case PLAYER_EMOTION_19:
		case PLAYER_EMOTION_20:
		{
			CAnimationState* pAnimationState = m_pStateController->GetCurrentState();
			if (pAnimationState)
			{
				CAnimationClip* pClip = pAnimationState->GetClip();
				if (pAnimationState->End())
				{
					m_eStandardState = PLAYER_IDLE1;
					m_fActiveTime = 0.f;
				}
			}
		}
		break;
		// Sit Down
		case PLAYER_EMOTION_6:
		{
			CAnimationState* pAnimationState = m_pStateController->GetCurrentState();
			if (pAnimationState)
			{
				CAnimationClip* pClip = pAnimationState->GetClip();
				if (pAnimationState->End())
				{
					m_eStandardState = PLAYER_EMOTION_21;
				}
			}
		}
		break;
		// Sitting
		case PLAYER_EMOTION_21:
		{
			if (GETKEY(KEY_W)
				|| GETKEY(KEY_A)
				|| GETKEY(KEY_S)
				|| GETKEY(KEY_D))
			{
				m_eStandardState = PLAYER_IDLE1;
				m_fActiveTime = 0.f;
			}
		}
		break;
		case PLAYER_IDLE1:
		case PLAYER_IDLE2:
		{
			CAnimationState* pAnimationState = m_pStateController->GetCurrentState();
			CAnimationClip* pClip = pAnimationState->GetClip();
			if (pClip)
			{
				string strClipName = pClip->GetClipName();
				if (strClipName != "Idle"
					&& strClipName != "Lobby_Idle_cb3")
					return;
			}
			m_fActiveMaxTime = 5.f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			if (fProgress >= 1.f)
			{
				if (m_eStandardState == PLAYER_IDLE1)
					m_eStandardState = PLAYER_IDLE2;
				else
					m_eStandardState = PLAYER_IDLE1;
				m_fActiveTime = 0.f;
			}
			
			// 장비 교체 부분
			if (GETKEYDOWN(KEY_Z))
			{
				ChangeEquipment(m_eEquipList.vecHELMET[m_eEquipList.iHELMETCount]);
				++m_eEquipList.iHELMETCount;
				if (m_eEquipList.vecHELMET.size() <= m_eEquipList.iHELMETCount)
					m_eEquipList.iHELMETCount = 0;
			}
			if (GETKEYDOWN(KEY_X))
			{
				ChangeEquipment(m_eEquipList.vecSHOULDER[m_eEquipList.iSHOULDERCount]);
				++m_eEquipList.iSHOULDERCount;
				if (m_eEquipList.vecSHOULDER.size() <= m_eEquipList.iSHOULDERCount)
					m_eEquipList.iSHOULDERCount = 0;
			}
			if (GETKEYDOWN(KEY_C))
			{
				ChangeEquipment(m_eEquipList.vecUPPER[m_eEquipList.iUPPERCount]);
				++m_eEquipList.iUPPERCount;
				if (m_eEquipList.vecUPPER.size() <= m_eEquipList.iUPPERCount)
					m_eEquipList.iUPPERCount = 0;
			}
			if (GETKEYDOWN(KEY_V))
			{
				ChangeEquipment(m_eEquipList.vecBELT[m_eEquipList.iBELTCount]);
				++m_eEquipList.iBELTCount;
				if (m_eEquipList.vecBELT.size() <= m_eEquipList.iBELTCount)
					m_eEquipList.iBELTCount = 0;
			}
			if (GETKEYDOWN(KEY_B))
			{
				ChangeEquipment(m_eEquipList.vecGLOVES[m_eEquipList.iGLOVESCount]);
				++m_eEquipList.iGLOVESCount;
				if (m_eEquipList.vecGLOVES.size() <= m_eEquipList.iGLOVESCount)
					m_eEquipList.iGLOVESCount = 0;
			}
			if (GETKEYDOWN(KEY_N))
			{
				ChangeEquipment(m_eEquipList.vecBOOTS[m_eEquipList.iBOOTSCount]);
				++m_eEquipList.iBOOTSCount;
				if (m_eEquipList.vecBOOTS.size() <= m_eEquipList.iBOOTSCount)
					m_eEquipList.iBOOTSCount = 0;
			}
			/*if (GETKEYDOWN(KEY_M))
			{
				ChangeEquipment(m_eEquipList.vecHair[m_eEquipList.iHairCount]);
				++m_eEquipList.iHairCount;
				if (m_eEquipList.vecHair.size() <= m_eEquipList.iHairCount)
					m_eEquipList.iHairCount = 0;
			}*/

			if (GETKEY(KEY_LEFT_CONTROL))
			{
				if (GETKEYDOWN(KEY_1))
				{
					m_eStandardState = PLAYER_EMOTION_1;
					m_fActiveTime = 0.f;
					return;
				}

				if (GETKEYDOWN(KEY_2))
				{
					m_eStandardState = PLAYER_EMOTION_2;
					m_fActiveTime = 0.f;
					return;
				}

				if (GETKEYDOWN(KEY_3))
				{
					m_eStandardState = PLAYER_EMOTION_3;
					m_fActiveTime = 0.f;
					return;
				}

				if (GETKEYDOWN(KEY_4))
				{
					m_eStandardState = PLAYER_EMOTION_4;
					m_fActiveTime = 0.f;
					return;
				}

				if (GETKEYDOWN(KEY_5))
				{
					m_eStandardState = PLAYER_EMOTION_5;
					m_fActiveTime = 0.f;
					return;
				}

				if (GETKEYDOWN(KEY_6))
				{
					m_eStandardState = PLAYER_EMOTION_6;
					m_fActiveTime = 0.f;
					return;
				}
				if (GETKEYDOWN(KEY_7))
				{
					m_eStandardState = PLAYER_EMOTION_7;
					m_fActiveTime = 0.f;
					return;
				}
				if (GETKEYDOWN(KEY_8))
				{
					m_eStandardState = PLAYER_EMOTION_8;
					m_fActiveTime = 0.f;
					return;
				}
				if (GETKEYDOWN(KEY_9))
				{
					m_eStandardState = PLAYER_EMOTION_9;
					m_fActiveTime = 0.f;
					return;
				}
				if (GETKEYDOWN(KEY_0))
				{
					m_eStandardState = PLAYER_EMOTION_10;
					m_fActiveTime = 0.f;
					return;
				}
				if (GETKEYDOWN(KEY_Q))
				{
					m_eStandardState = PLAYER_EMOTION_11;
					m_fActiveTime = 0.f;
					return;
				}
				if (GETKEYDOWN(KEY_W))
				{
					m_eStandardState = PLAYER_EMOTION_12;
					m_fActiveTime = 0.f;
					return;
				}
				if (GETKEYDOWN(KEY_E))
				{
					m_eStandardState = PLAYER_EMOTION_13;
					m_fActiveTime = 0.f;
					return;
				}
				if (GETKEYDOWN(KEY_R))
				{
					m_eStandardState = PLAYER_EMOTION_14;
					m_fActiveTime = 0.f;
					return;
				}
				if (GETKEYDOWN(KEY_T))
				{
					m_eStandardState = PLAYER_EMOTION_15;
					m_fActiveTime = 0.f;
					return;
				}
				if (GETKEYDOWN(KEY_Y))
				{
					m_eStandardState = PLAYER_EMOTION_16;
					m_fActiveTime = 0.f;
					return;
				}
				if (GETKEYDOWN(KEY_U))
				{
					m_eStandardState = PLAYER_EMOTION_17;
					m_fActiveTime = 0.f;
					return;
				}
				if (GETKEYDOWN(KEY_I))
				{
					m_eStandardState = PLAYER_EMOTION_18;
					m_fActiveTime = 0.f;
					return;
				}
				if (GETKEYDOWN(KEY_O))
				{
					m_eStandardState = PLAYER_EMOTION_19;
					m_fActiveTime = 0.f;
					return;
				}
				if (GETKEYDOWN(KEY_P))
				{
					m_eStandardState = PLAYER_EMOTION_20;
					m_fActiveTime = 0.f;
					return;
				}
			}

			if (GETKEY(KEY_W))
			{
				m_eStandardState = PLAYER_WALK_F;
				if (GETKEY(KEY_LEFT_SHIFT))
					m_eStandardState = PLAYER_RUN_F;
			}
			if (GETKEY(KEY_A))
			{
				m_eStandardState = PLAYER_ROT_L;
			}

			if (GETKEY(KEY_S))
			{
				m_eStandardState = PLAYER_WALK_B;
				if (GETKEY(KEY_LEFT_SHIFT))
					m_eStandardState = PLAYER_RUN_B;
			}
			if (GETKEY(KEY_D))
			{
				m_eStandardState = PLAYER_ROT_R;
			}

			bool bMouseVisible = GET_SINGLE(CInput)->GetCursorVisible();

			if (GETKEYDOWN(MOUSE_LEFT) && !bMouseVisible)
			{
				if (m_bUseMeteo)
				{
					m_bUseMeteo = false;
					m_pSkillCircle->Enable(false);

					m_eStandardState = PLAYER_STANDARD_NONE;
					m_eBattleState = PLAYER_SKILL_B1;
					m_fActiveTime = 0.f;
					m_bSoundPlay = true;
					m_bSkillEffect = true;

					Vector2 vCursorPosition = GET_SINGLE(CInput)->GetCursorPosition();
					CCamera* pCamera = m_pScene->GetMainCamera();
					CTransform* pCameraTransform = pCamera->GetTransform();
					RAYCASTHIT tHit;
					if (vCursorPosition.x > 0 && vCursorPosition.x < DEVICE_RESOLUTION.iWidth && vCursorPosition.y > 0 &&
						vCursorPosition.y < DEVICE_RESOLUTION.iHeight)
					{
						RAY tRay = pCamera->ScreenPointToRay(vCursorPosition);
						tHit = GET_SINGLE(CCollisionManager)->RayCast(tRay);

					}
					if (tHit.bResult)
					{
						m_vCursorClickPosition = tHit.vPoint;
					}
					SAFE_RELEASE(pCamera);
					SAFE_RELEASE(pCameraTransform);

					CSphereCollider* pPlayerCollision = m_pGameObject->GetComponent<CSphereCollider>();
					pPlayerCollision->Enable(false);
					SAFE_RELEASE(pPlayerCollision);
				}
				else
				{
					m_eStandardState = PLAYER_STANDARD_NONE;
					m_eBattleState = PLAYER_ATTACK_A1;
					m_fActiveTime = 0.f;
					m_bSoundPlay = true;

					m_iAttackPhase = 1;
				}
			}
			if (GETKEYDOWN(KEY_1) && !bMouseVisible)
			{
				m_eStandardState = PLAYER_STANDARD_NONE;
				m_eBattleState = PLAYER_ATTACK_B1;
				m_fActiveTime = 0.f;
				m_bSoundPlay = true;
				m_bSkillEffect = true;

				m_iAttackPhase = 1;
			}

			if (GETKEYDOWN(KEY_2) && !bMouseVisible)
			{
				m_eStandardState = PLAYER_STANDARD_NONE;
				m_eBattleState = PLAYER_SKILL_A1;
				m_fActiveTime = 0.f;
				m_bSoundPlay = true;

				GET_SINGLE(CSoundManager)->Play("Kick_Voice");

				// 모션블러킴
				CGameObject* pMainCameraObject = m_pScene->GetMainCameraObject();
				CPostProcessing* pPostProcessing = pMainCameraObject->GetComponent<CPostProcessing>();
				if (pPostProcessing)
				{
					uint32_t iFlags = pPostProcessing->GetPostProcessingFlags();
					if ((iFlags & PT_MOTION_BLUR) == false)
						pPostProcessing->ToggleFlags(PT_MOTION_BLUR);
					SAFE_RELEASE(pPostProcessing);
				}
				SAFE_RELEASE(pMainCameraObject);

				CSphereCollider* pPlayerCollision = m_pGameObject->GetComponent<CSphereCollider>();
				pPlayerCollision->Enable(false);
				SAFE_RELEASE(pPlayerCollision);
			}

			if (GETKEYDOWN(KEY_3) && !bMouseVisible)
			{
				if (!m_bUseMeteo)
				{
					m_bUseMeteo = true;

					if (!m_pSkillCircle)
					{
						m_pSkillCircle = CGameObject::CreateObject("SkillCircle", m_pLayer);
						CEffectController*	pSkillCircle = m_pSkillCircle->AddComponent<CEffectController>();
						pSkillCircle->SetLifeTime(1.0f, false);
						pSkillCircle->SetSkillCircle(true);
						SAFE_RELEASE(pSkillCircle);

						CTransform*	pSkillCircleTr = m_pSkillCircle->GetTransform();

						pSkillCircleTr->SetPivot(0.5f, 0.5f, 0.5f);
						pSkillCircleTr->SetWorldScale(30.f, 30.f, 30.f);

						CMeshRenderer*	pMeshRenderer = m_pSkillCircle->AddComponent<CMeshRenderer>("Effect_Fall_Position");
						pMeshRenderer->SetMesh(ENGINE_MESH_RECTANGLE_TEX, true);
						pMeshRenderer->SetRenderState(ALPHA_BLEND);
						pMeshRenderer->SetRenderState(CULLNONE);


						//Diffuse, Normal, Specular
						CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
						pMaterial->SetDiffuseTexture("FX_Circle_wind03", L"Decals/FX_Circle_wind03.tga");
						pMaterial->SetNormalTexture("FX_Circle_wind03_NRM", L"Decals/FX_Circle_wind03_NRM.tga");
						pMaterial->SetDiffuseColor(Vector4(0.2f, 1.0f, 0.2f, 1.0f));
						pMaterial->SetEmissiveColor(Vector4(0.2f, 1.0f, 0.2f, 0.8f));
						pMaterial->SetAmbientColor(Vector4(1.f, 1.f, 1.f, 0.1f));
						pMaterial->SetSpecularColor(Vector4(1.f, 1.f, 1.f, 3.f));

						SAFE_RELEASE(pMaterial);
						SAFE_RELEASE(pMeshRenderer);

						CDecal*	pDecal = m_pSkillCircle->AddComponent<CDecal>();
						SAFE_RELEASE(pDecal);

						SAFE_RELEASE(pSkillCircleTr);

						m_pSkillCircle->Enable(true);
					}
					else
					{
						m_pSkillCircle->Enable(true);
						CEffectController*	pSkillCircle = m_pSkillCircle->GetComponent<CEffectController>();
						pSkillCircle->SetSkillCircle(true);
						SAFE_RELEASE(pSkillCircle);
					}
					GET_SINGLE(CSoundManager)->Play("Skill_Ready");
				}
				else
				{
					m_bUseMeteo = false;
					m_pSkillCircle->Enable(false);
					CEffectController*	pSkillCircle = m_pSkillCircle->GetComponent<CEffectController>();
					pSkillCircle->SetSkillCircle(false);
					SAFE_RELEASE(pSkillCircle);
					GET_SINGLE(CSoundManager)->Play("Skill_Cancle");
				}
			}

			if (GETKEYDOWN(KEY_SPACE) && !bMouseVisible)
			{
				m_eStandardState = PLAYER_DODGE;
				m_fActiveTime = 0.f;

				CSphereCollider* pPlayerCollision = m_pGameObject->GetComponent<CSphereCollider>();
				pPlayerCollision->Enable(false);
				SAFE_RELEASE(pPlayerCollision);

				GET_SINGLE(CSoundManager)->Play("Kick_Voice");

				CTransform* pDummyTr = m_pDummyForCameraWork->GetTransform();
				pDummyTr->SetWorldPosition(m_pTransform->GetWorldPosition());
				pDummyTr->SetWorldRotation(m_pTransform->GetWorldRotation());
				SAFE_RELEASE(pDummyTr);

				CGameObject* pPlayerCameraObj = m_pLayer->FindGameObjectWithTag("Camera Gimbal Y");
				if (pPlayerCameraObj)
				{
					CPlayerCamera* pPlayerCamera = pPlayerCameraObj->GetComponent<CPlayerCamera>();
					pPlayerCamera->SetPlayer(m_pDummyForCameraWork);
					SAFE_RELEASE(pPlayerCamera);
					SAFE_RELEASE(pPlayerCameraObj);
				}
			}

			// 대화 
			if (GETKEYDOWN(KEY_RETURN))
			{
				
			}
		}
		break;

		case PLAYER_ROT_R:
		case PLAYER_ROT_L:
		case PLAYER_WALK_F:
		case PLAYER_WALK_B:
		case PLAYER_RUN_F:
		case PLAYER_RUN_B:
		{
			bool IsMove = false;
			float fSpeed = m_fMoveSpeed;
			m_fActiveTime = 0.f;
			if (GETKEY(KEY_W))
			{
				if (m_eStandardState == PLAYER_RUN_F)
					fSpeed *= 2.f;
				m_pTransform->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * fSpeed * fTime);
				m_eStandardState = PLAYER_WALK_F;
				if (GETKEY(KEY_LEFT_SHIFT))
					m_eStandardState = PLAYER_RUN_F;

				IsMove = true;
			}

			else if (GETKEY(KEY_S))
			{
				if (m_eStandardState == PLAYER_RUN_B)
					fSpeed *= 2.f;
				m_pTransform->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->back() * fSpeed* fTime);
				m_eStandardState = PLAYER_WALK_B;
				if (GETKEY(KEY_LEFT_SHIFT))
					m_eStandardState = PLAYER_RUN_B;

				IsMove = true;
			}

			if (GETKEY(KEY_D))
			{
				Vector3 vRotation = m_pTransform->GetWorldRotation();
				float fAngle = vRotation.y;
				fAngle += m_fRotateSpeed * D2R * fTime;
				m_pTransform->SetWorldRotation(vRotation.x, fAngle, vRotation.z);

				//CGameObject* pPlayerCameraObj = m_pLayer->FindGameObjectWithTag("Camera Gimbal Y");
				//if (pPlayerCameraObj)
				//{
				//	CPlayerCamera* pPlayerCamera = pPlayerCameraObj->GetComponent<CPlayerCamera>();
				//	CTransform* pCameraZimbalY = pPlayerCamera->GetTransform();
				//	float fRotY = pCameraZimbalY->GetWorldRotation().y;
				//	pCameraZimbalY->SetWorldRotationY(fRotY + m_fRotateSpeed * D2R * fTime);
				//	SAFE_RELEASE(pCameraZimbalY);
				//	SAFE_RELEASE(pPlayerCamera);
				//	SAFE_RELEASE(pPlayerCameraObj);
				//}
			}
			else if (GETKEY(KEY_A))
			{
				Vector3 vRotation = m_pTransform->GetWorldRotation();
				float fAngle = vRotation.y;
				fAngle -= m_fRotateSpeed * D2R * fTime;
				m_pTransform->SetWorldRotation(vRotation.x, fAngle, vRotation.z);

				//CGameObject* pPlayerCameraObj = m_pLayer->FindGameObjectWithTag("Camera Gimbal Y");
				//if (pPlayerCameraObj)
				//{
				//	CPlayerCamera* pPlayerCamera = pPlayerCameraObj->GetComponent<CPlayerCamera>();
				//	CTransform* pCameraZimbalY = pPlayerCamera->GetTransform();
				//	float fRotY = pCameraZimbalY->GetWorldRotation().y;
				//	pCameraZimbalY->SetWorldRotationY(fRotY - m_fRotateSpeed * D2R * fTime);
				//	SAFE_RELEASE(pCameraZimbalY);
				//	SAFE_RELEASE(pPlayerCamera);
				//	SAFE_RELEASE(pPlayerCameraObj);
				//}
			}

			bool bMouseVisible = GET_SINGLE(CInput)->GetCursorVisible();

			if (GETKEYDOWN(MOUSE_LEFT) && !bMouseVisible)
			{
				if (m_bUseMeteo)
				{
					m_bUseMeteo = false;
					m_pSkillCircle->Enable(false);

					m_eStandardState = PLAYER_STANDARD_NONE;
					m_eBattleState = PLAYER_SKILL_B1;
					m_fActiveTime = 0.f;
					m_bSoundPlay = true;
					m_bSkillEffect = true;

					Vector2 vCursorPosition = GET_SINGLE(CInput)->GetCursorPosition();
					CCamera* pCamera = m_pScene->GetMainCamera();
					CTransform* pCameraTransform = pCamera->GetTransform();
					RAYCASTHIT tHit;
					if (vCursorPosition.x > 0 && vCursorPosition.x < DEVICE_RESOLUTION.iWidth && vCursorPosition.y > 0 &&
						vCursorPosition.y < DEVICE_RESOLUTION.iHeight)
					{
						RAY tRay = pCamera->ScreenPointToRay(vCursorPosition);
						tHit = GET_SINGLE(CCollisionManager)->RayCast(tRay);

					}
					if (tHit.bResult)
					{
						m_vCursorClickPosition = tHit.vPoint;
					}
					SAFE_RELEASE(pCamera);
					SAFE_RELEASE(pCameraTransform);

					CSphereCollider* pPlayerCollision = m_pGameObject->GetComponent<CSphereCollider>();
					pPlayerCollision->Enable(false);
					SAFE_RELEASE(pPlayerCollision);
				}
				else
				{
					m_eStandardState = PLAYER_STANDARD_NONE;
					m_eBattleState = PLAYER_ATTACK_A1;
					m_fActiveTime = 0.f;
					m_bSoundPlay = true;

					m_iAttackPhase = 1;
				}
			}

			if (GETKEYDOWN(KEY_1) && !bMouseVisible)
			{
				IsMove = true;
				m_eStandardState = PLAYER_STANDARD_NONE;
				m_eBattleState = PLAYER_ATTACK_B1;
				m_fActiveTime = 0.f;
				m_bSoundPlay = true;
				m_bSkillEffect = true;

				m_iAttackPhase = 1;
			}

			if (GETKEYDOWN(KEY_2) && !bMouseVisible)
			{
				IsMove = true;
				m_eStandardState = PLAYER_STANDARD_NONE;
				m_eBattleState = PLAYER_SKILL_A1;
				m_fActiveTime = 0.f;

				// 모션블러킴
				CGameObject* pMainCameraObject = m_pScene->GetMainCameraObject();
				CPostProcessing* pPostProcessing = pMainCameraObject->GetComponent<CPostProcessing>();
				if (pPostProcessing)
				{
					uint32_t iFlags = pPostProcessing->GetPostProcessingFlags();
					if ((iFlags & PT_MOTION_BLUR) == false)
						pPostProcessing->ToggleFlags(PT_MOTION_BLUR);
					SAFE_RELEASE(pPostProcessing);
				}
				SAFE_RELEASE(pMainCameraObject);

				CSphereCollider* pPlayerCollision = m_pGameObject->GetComponent<CSphereCollider>();
				pPlayerCollision->Enable(false);
				SAFE_RELEASE(pPlayerCollision);
			}

			if (GETKEYDOWN(KEY_SPACE) && !bMouseVisible)
			{
				IsMove = true;
				m_eStandardState = PLAYER_DODGE;
				m_fActiveTime = 0.f;

				CSphereCollider* pPlayerCollision = m_pGameObject->GetComponent<CSphereCollider>();
				pPlayerCollision->Enable(false);
				SAFE_RELEASE(pPlayerCollision);

				GET_SINGLE(CSoundManager)->Play("Kick_Voice");

				CTransform* pDummyTr = m_pDummyForCameraWork->GetTransform();
				pDummyTr->SetWorldPosition(m_pTransform->GetWorldPosition());
				pDummyTr->SetWorldRotation(m_pTransform->GetWorldRotation());
				SAFE_RELEASE(pDummyTr);
				
				CGameObject* pPlayerCameraObj = m_pLayer->FindGameObjectWithTag("Camera Gimbal Y");
				if (pPlayerCameraObj)
				{
					CPlayerCamera* pPlayerCamera = pPlayerCameraObj->GetComponent<CPlayerCamera>();
					pPlayerCamera->SetPlayer(m_pDummyForCameraWork);
					SAFE_RELEASE(pPlayerCamera);
					SAFE_RELEASE(pPlayerCameraObj);
				}
			}

			if (!IsMove)
			{
				m_eStandardState = PLAYER_IDLE1;
				m_fActiveTime = 0.f;
			}
		}
		break;
		case PLAYER_DODGE:
		{
			m_fActiveMaxTime = 1.f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			if (fProgress >= 1.f)
			{
				m_pStateController->SetInteger("Standard", 0);

				m_fActiveTime = 0.f;
				m_eStandardState = PLAYER_IDLE1;
				m_eBattleState = PLAYER_BATTLE_NONE;

				CSphereCollider* pPlayerCollision = m_pGameObject->GetComponent<CSphereCollider>();
				pPlayerCollision->Enable(true);
				SAFE_RELEASE(pPlayerCollision);

				CGameObject* pPlayerCameraObj = m_pLayer->FindGameObjectWithTag("Camera Gimbal Y");
				if (pPlayerCameraObj)
				{
					CPlayerCamera* pPlayerCamera = pPlayerCameraObj->GetComponent<CPlayerCamera>();
					pPlayerCamera->SetPlayer(m_pGameObject);
					SAFE_RELEASE(pPlayerCamera);
					SAFE_RELEASE(pPlayerCameraObj);
				}
				
				Vector3 vPos = m_pTransform->GetWorldPosition();
				vPos += m_pTransform->back() * 14.2f;
				m_pTransform->SetWorldPosition(vPos);
			}
			else if (fProgress <= 0.3f)
			{
				CTransform* pDummyTr = m_pDummyForCameraWork->GetTransform();
				Vector3 vPos = pDummyTr->GetWorldPosition();
				vPos += m_pTransform->back() * 15.f * fTime * 3.3f;
				pDummyTr->SetWorldPosition(vPos);
				SAFE_RELEASE(pDummyTr);
			}
		}
		break;

		case PLAYER_DODGE_GETUP:
		{
			m_fActiveMaxTime = 1.f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			if (fProgress >= 1.f)
			{
				m_fActiveTime = 0.f;
				m_eStandardState = PLAYER_IDLE1;
				m_eBattleState = PLAYER_BATTLE_NONE;

				CSphereCollider* pPlayerCollision = m_pGameObject->GetComponent<CSphereCollider>();
				pPlayerCollision->Enable(true);
				SAFE_RELEASE(pPlayerCollision);
			}

			Vector3 vPos = m_pTransform->GetWorldPosition();
			vPos += m_pTransform->back() * 15.f * fTime;
			m_pTransform->SetWorldPosition(vPos);
		}
		break;

		}// switch 문의 닫는 괄호입니다
	}
	// 전투 조작
	else if (m_eBattleState != PLAYER_BATTLE_NONE)
	{
		switch (m_eBattleState)
		{ // switch 문의 여는 괄호입니다
		case PLAYER_ATTACK_A1:
		{
			m_fActiveMaxTime = 0.75f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;

			if (m_iAttackPhase == 1)
			{
				if (fProgress >= 0.1f)
				{
					CGameObject* pAttackObj = CGameObject::CreateObject("Attack", m_pLayer);
					CSphereCollider* pCollider = pAttackObj->AddComponent<CSphereCollider>();
					pCollider->SetRadius(3.f);
					SAFE_RELEASE(pCollider);
					CAttack* pAttack = pAttackObj->AddComponent<CAttack>();
					pAttack->SetValue(1);
					pAttack->SetAttribute(ATTACK_NORMAL);
					pAttack->SetDamagePoint(5.f);
					pAttack->SetActiveMaxTime(0.3f);
					SAFE_RELEASE(pAttack);
					CTransform* pTr = pAttackObj->GetTransform();
					pTr->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * 2.f);
					SAFE_RELEASE(pTr);
					SAFE_RELEASE(pAttackObj);

					++m_iAttackPhase;
				}
			}
			else if (m_iAttackPhase == 2)
			{
				if (fProgress >= 0.4f)
				{
					CGameObject* pAttackObj = CGameObject::CreateObject("Attack", m_pLayer);
					CSphereCollider* pCollider = pAttackObj->AddComponent<CSphereCollider>();
					pCollider->SetRadius(3.f);
					SAFE_RELEASE(pCollider);
					CAttack* pAttack = pAttackObj->AddComponent<CAttack>();
					pAttack->SetValue(1);
					pAttack->SetAttribute(ATTACK_NORMAL);
					pAttack->SetDamagePoint(5.f);
					pAttack->SetActiveMaxTime(0.5f);
					SAFE_RELEASE(pAttack);
					CTransform* pTr = pAttackObj->GetTransform();
					pTr->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * 2.f);
					SAFE_RELEASE(pTr);
					SAFE_RELEASE(pAttackObj);

					++m_iAttackPhase;
				}
			}

			if (fProgress >= 0.15f && m_bSoundPlay)
			{
				m_bSoundPlay = false;
				GET_SINGLE(CSoundManager)->Play("Attack001_SFX");
				GET_SINGLE(CSoundManager)->Play("Attack001_Voice");
			}
			else if (fProgress >= 0.8f)
			{
				if (GETKEYDOWN(MOUSE_LEFT))
				{
					m_fActiveTime = 0.f;
					m_eBattleState = PLAYER_ATTACK_A2;

					m_bSoundPlay = true;
					m_iAttackPhase = 1;
				}
				else if (fProgress >= 1.f)
				{
					m_fActiveTime = 0.f;
					m_eStandardState = PLAYER_IDLE1;
					m_eBattleState = PLAYER_BATTLE_NONE;
				}
			}
		}
		break;
		case PLAYER_ATTACK_A2:
		{
			m_fActiveMaxTime = 0.75f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			if (m_bSoundPlay)
			{
				m_bSoundPlay = false;
				GET_SINGLE(CSoundManager)->Play("Attack002_SFX");
				GET_SINGLE(CSoundManager)->Play("Attack002_Voice");
			}

			if (m_iAttackPhase == 1)
			{
				if (fProgress >= 0.1f)
				{
					CGameObject* pAttackObj = CGameObject::CreateObject("Attack", m_pLayer);
					CSphereCollider* pCollider = pAttackObj->AddComponent<CSphereCollider>();
					pCollider->SetRadius(3.f);
					SAFE_RELEASE(pCollider);
					CAttack* pAttack = pAttackObj->AddComponent<CAttack>();
					pAttack->SetValue(1);
					pAttack->SetAttribute(ATTACK_NORMAL);
					pAttack->SetDamagePoint(10.f);
					pAttack->SetActiveMaxTime(0.2f);
					SAFE_RELEASE(pAttack);
					CTransform* pTr = pAttackObj->GetTransform();
					pTr->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * 2.f);
					SAFE_RELEASE(pTr);
					SAFE_RELEASE(pAttackObj);

					++m_iAttackPhase;
				}
			}

			if (fProgress >= 0.8f)
			{
				if (GETKEYDOWN(MOUSE_LEFT))
				{
					m_fActiveTime = 0.f;
					m_eBattleState = PLAYER_ATTACK_A3;
					m_bSoundPlay = true;

					m_iAttackPhase = 1;
				}
				else if (fProgress >= 1.f)
				{
					m_fActiveTime = 0.f;
					m_eStandardState = PLAYER_IDLE1;
					m_eBattleState = PLAYER_BATTLE_NONE;
				}
			}

		}
		break;
		case PLAYER_ATTACK_A3:
		{
			m_fActiveMaxTime = 0.75f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;

			if (m_bSoundPlay)
			{
				m_bSoundPlay = false;
				GET_SINGLE(CSoundManager)->Play("Attack003_SFX");
				GET_SINGLE(CSoundManager)->Play("Attack003_Voice");
			}

			if (m_iAttackPhase == 1)
			{
				if (fProgress >= 0.2f)
				{
					CGameObject* pAttackObj = CGameObject::CreateObject("Attack", m_pLayer);
					CSphereCollider* pCollider = pAttackObj->AddComponent<CSphereCollider>();
					pCollider->SetRadius(3.f);
					SAFE_RELEASE(pCollider);
					CAttack* pAttack = pAttackObj->AddComponent<CAttack>();
					pAttack->SetValue(1);
					pAttack->SetAttribute(ATTACK_NORMAL);
					pAttack->SetDamagePoint(10.f);
					pAttack->SetActiveMaxTime(0.2f);
					SAFE_RELEASE(pAttack);
					CTransform* pTr = pAttackObj->GetTransform();
					pTr->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * 2.f);
					SAFE_RELEASE(pTr);
					SAFE_RELEASE(pAttackObj);

					++m_iAttackPhase;
				}
			}

			if (fProgress >= 0.8f)
			{
				if (GETKEYDOWN(MOUSE_LEFT))
				{
					m_fActiveTime = 0.f;
					m_eBattleState = PLAYER_SMASH_A1;
					m_bSoundPlay = true;

					m_iAttackPhase = 1;
				}
				else if (GETKEYDOWN(KEY_1))
				{
					m_fActiveTime = 0.f;
					m_eBattleState = PLAYER_SMASH_A2;
					m_bSoundPlay = true;
					m_bSkillEffect = true;

					m_iAttackPhase = 1;

					CTransform* pDummyTr = m_pDummyForCameraWork->GetTransform();
					pDummyTr->SetWorldPosition(m_pTransform->GetWorldPosition());
					pDummyTr->SetWorldRotation(m_pTransform->GetWorldRotation());
					SAFE_RELEASE(pDummyTr);

					CGameObject* pPlayerCameraObj = m_pLayer->FindGameObjectWithTag("Camera Gimbal Y");
					if (pPlayerCameraObj)
					{
					CPlayerCamera* pPlayerCamera = pPlayerCameraObj->GetComponent<CPlayerCamera>();
					pPlayerCamera->SetPlayer(m_pDummyForCameraWork);
					SAFE_RELEASE(pPlayerCamera);
					SAFE_RELEASE(pPlayerCameraObj);
					}

					CSphereCollider* pPlayerCollision = m_pGameObject->GetComponent<CSphereCollider>();
					pPlayerCollision->Enable(false);
					SAFE_RELEASE(pPlayerCollision);
				}
				else if (fProgress >= 1.f)
				{
					m_fActiveTime = 0.f;
					m_eStandardState = PLAYER_IDLE1;
					m_eBattleState = PLAYER_BATTLE_NONE;
				}
			}
		}
		break;
		case PLAYER_SMASH_A1:
		{
			m_fActiveMaxTime = 0.75f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			if (m_bSoundPlay)
			{
				m_bSoundPlay = false;
				GET_SINGLE(CSoundManager)->Play("Smash_SFX");
				GET_SINGLE(CSoundManager)->Play("Smash_Voice");
			}

			if (m_iAttackPhase == 1)
			{
				if (fProgress >= 0.2f)
				{
					CGameObject* pAttackObj = CGameObject::CreateObject("Attack", m_pLayer);
					CSphereCollider* pCollider = pAttackObj->AddComponent<CSphereCollider>();
					pCollider->SetRadius(3.f);
					SAFE_RELEASE(pCollider);
					CAttack* pAttack = pAttackObj->AddComponent<CAttack>();
					pAttack->SetValue(1);
					pAttack->SetAttribute(ATTACK_DOWN);
					pAttack->SetDamagePoint(30.f);
					pAttack->SetActiveMaxTime(0.2f);
					SAFE_RELEASE(pAttack);
					CTransform* pTr = pAttackObj->GetTransform();
					pTr->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * 2.f);
					SAFE_RELEASE(pTr);
					SAFE_RELEASE(pAttackObj);

					++m_iAttackPhase;
				}
			}

			if (fProgress >= 1.f)
			{
				m_fActiveTime = 0.f;
				m_eStandardState = PLAYER_IDLE1;
				m_eBattleState = PLAYER_BATTLE_NONE;
			}
		}
		break;
		case PLAYER_ATTACK_B1:
		{
			m_fActiveMaxTime = 0.75f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			if (m_bSoundPlay)
			{
				m_bSoundPlay = false;
				GET_SINGLE(CSoundManager)->Play("Smash_SFX");
				GET_SINGLE(CSoundManager)->Play("Smash_Voice");
			}

			if (m_iAttackPhase == 1)
			{
				if (fProgress >= 0.4f)
				{
					PlayerActiveAttackSphere(3.f, ATTACK_NORMAL, 20.f, 0.2f,
						m_pTransform->GetWorldPosition() + m_pTransform->forward() * 2.f);

					++m_iAttackPhase;
				}
			}

			if (fProgress >= 0.8f)
			{
				if (GETKEYDOWN(MOUSE_LEFT))
				{
					m_fActiveTime = 0.f;
					m_eBattleState = PLAYER_SMASH_B2;
					m_bSoundPlay = true;

					m_iAttackPhase = 1;
				}
				else if (fProgress >= 1.f)
				{
					m_fActiveTime = 0.f;
					m_eStandardState = PLAYER_IDLE1;
					m_eBattleState = PLAYER_BATTLE_NONE;

					{
						CLayer* pLayer = m_pGameObject->GetLayer();

						CGameObject* pTestObj = CGameObject::CreateObject("Test", pLayer);
						CTransform*	pTestTr = pTestObj->GetTransform();
						Vector3	vPosition = m_pTransform->GetWorldPosition();
						pTestTr->SetWorldPosition(vPosition.x, vPosition.y + 10.f, vPosition.z);
						pTestTr->SetWorldScale(0.5f, 0.5f, 0.5f);

						CSphereCollider* pTestCollision = pTestObj->AddComponent<CSphereCollider>();
						SAFE_RELEASE(pTestCollision);

						CRigidbody*	pRigidBody = pTestObj->AddComponent<CRigidbody>();
						pRigidBody->SetKinematic(false);
						pRigidBody->SetUseGravity(true);
						SAFE_RELEASE(pRigidBody);

						SAFE_RELEASE(pTestTr);
						SAFE_RELEASE(pTestObj);
						SAFE_RELEASE(pLayer);
					}
				}
			}

			else if (fProgress >= 0.3f && m_bSkillEffect)
			{
				m_bSkillEffect = false;
				
			}
		}
		break;
		case PLAYER_SMASH_B2:
		{
			m_fActiveMaxTime = 0.75f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			//if (fProgress >= 0.6f && m_bSoundPlay)
			//{
			//	m_bSoundPlay = false;
			//	GET_SINGLE(CSoundManager)->Play("Kick_SFX");
			//}
			//else if (fProgress >= 0.5f && fProgress < 0.6f &&!m_bSoundPlay)
			//{
			//	m_bSoundPlay = true;
			//}

			if (m_iAttackPhase == 1)
			{
				if (fProgress >= 0.3f)
				{
					PlayerActiveAttackSphere(3.f, ATTACK_DOWN, 30.f, 0.2f,
						m_pTransform->GetWorldPosition() + m_pTransform->forward() * 2.f);

					++m_iAttackPhase;
				}
			}

			if (fProgress >= 0.3f && m_bSoundPlay)
			{
				m_bSoundPlay = false;
				GET_SINGLE(CSoundManager)->Play("Kick_SFX");
				GET_SINGLE(CSoundManager)->Play("Kick_Voice");
			}
			if (fProgress >= 1.f)
			{
				m_fActiveTime = 0.f;
				m_eStandardState = PLAYER_IDLE1;
				m_eBattleState = PLAYER_BATTLE_NONE;
			}
		}
		break;
		case PLAYER_SKILL_A1:
		{
			m_fActiveMaxTime = 1.5f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;

			if (m_bSoundPlay && fProgress >= 0.2f)
			{
				m_bSoundPlay = false;
				GET_SINGLE(CSoundManager)->Play("BladeDance_Ready");
			}

			if (fProgress >= 1.f)
			{
				m_bSoundPlay = true;
				m_fActiveTime = 0.f;
				m_eBattleState = PLAYER_SKILL_A2;
				m_iAttackPhase = 2;
			}

		}
		break;
		case PLAYER_SKILL_A2:
		{
			m_fActiveMaxTime = 1.75f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;

			if (m_iAttackPhase == 2)
			{
				if (fProgress >= 0.1f)
				{
					PlayerActiveAttackSphere(4.f, ATTACK_NORMAL, 10.f, 0.1f,
						m_pTransform->GetWorldPosition() + m_pTransform->forward() * 3.f);
					++m_iAttackPhase;
				}
			}
			else if (m_iAttackPhase == 3)
			{
				if (fProgress >= 0.2f)
				{
					PlayerActiveAttackSphere(4.f, ATTACK_NORMAL, 10.f, 0.1f,
						m_pTransform->GetWorldPosition() + m_pTransform->forward() * 3.f);

					++m_iAttackPhase;
				}
			}
			else if (m_iAttackPhase == 4)
			{
				if (fProgress >= 0.22f)
				{
					PlayerActiveAttackSphere(4.f, ATTACK_NORMAL, 10.f, 0.1f,
						m_pTransform->GetWorldPosition() + m_pTransform->forward() * 3.f);

					++m_iAttackPhase;
				}
			}
			else if (m_iAttackPhase == 5)
			{
				if (fProgress >= 0.4f)
				{
					PlayerActiveAttackSphere(4.f, ATTACK_NORMAL, 10.f, 0.1f,
						m_pTransform->GetWorldPosition() + m_pTransform->forward() * 3.f);

					++m_iAttackPhase;
				}
			}
			else if (m_iAttackPhase == 6)
			{
				if (fProgress >= 0.42f)
				{
					PlayerActiveAttackSphere(4.f, ATTACK_NORMAL, 10.f, 0.1f,
						m_pTransform->GetWorldPosition() + m_pTransform->forward() * 3.f);

					++m_iAttackPhase;
				}
			}
			else if (m_iAttackPhase == 7)
			{
				if (fProgress >= 0.5f)
				{
					PlayerActiveAttackSphere(4.f, ATTACK_NORMAL, 10.f, 0.1f,
						m_pTransform->GetWorldPosition() + m_pTransform->forward() * 3.f);

					++m_iAttackPhase;
				}
			}
			else if (m_iAttackPhase == 8)
			{
				if (fProgress >= 0.5f)
				{
					PlayerActiveAttackSphere(4.f, ATTACK_NORMAL, 10.f, 0.1f,
						m_pTransform->GetWorldPosition() + m_pTransform->forward() * 3.f);

					++m_iAttackPhase;
				}
			}
			else if (m_iAttackPhase == 9)
			{
				if (fProgress >= 0.52f)
				{
					PlayerActiveAttackSphere(4.f, ATTACK_NORMAL, 10.f, 0.1f,
						m_pTransform->GetWorldPosition() + m_pTransform->forward() * 3.f);

					++m_iAttackPhase;
				}
			}
			else if (m_iAttackPhase == 10)
			{
				if (fProgress >= 0.8f)
				{
					PlayerActiveAttackSphere(4.f, ATTACK_DOWN, 30.f, 0.1f,
						m_pTransform->GetWorldPosition() + m_pTransform->forward() * 3.f);

					++m_iAttackPhase;
					GET_SINGLE(CSoundManager)->Play("Smash_SFX");
					GET_SINGLE(CSoundManager)->Play("Smash_Voice");
					GET_SINGLE(CSoundManager)->Play("BladeDance_LastHit");

					// Effect Lightning
					{
						Vector3	vAxisZ = m_pTransform->GetWorldAxis(AX_Z);
						Vector3 vAxisZDir = vAxisZ.Normalize();

						CLayer* pLayer = m_pGameObject->GetLayer();

						CGameObject* pSkillEffectObject = CGameObject::CreateObject("Lightning_Effect", pLayer);
						CTransform*	pEffectTransfrom = pSkillEffectObject->GetTransform();

						pEffectTransfrom->SetPivot(0.5f, 1.0f, 0.5f);
						pEffectTransfrom->SetWorldScale(10.f, 100.f, 1.f);

						Vector3	vObjTr = m_pTransform->GetWorldPosition();
						pEffectTransfrom->SetWorldPosition(vObjTr.x + vAxisZDir.x * 5.f, vObjTr.y, vObjTr.z + vAxisZDir.z * 5.f);

						CMeshRenderer*	pMeshRenderer = pSkillEffectObject->AddComponent<CMeshRenderer>("Effect_Lightning");
						pMeshRenderer->SetMesh(ENGINE_MESH_RECTANGLE_TEX, true);
						pMeshRenderer->SetRenderState(ALPHA_BLEND);
						pMeshRenderer->SetRenderState(CULLNONE);

						CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
						pMaterial->SetDiffuseTexture("FX_thunderbolt_01", L"Effects/FX_thunderbolt_01.tga");
						pMaterial->SetAlphaTexture("FX_thunderbolt_01", L"Effects/FX_thunderbolt_01.tga");
						pMaterial->SetAlphaColor(Vector4(0.f, 0.f, 0.f, 1.0f));
						pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
						pMaterial->SetEmissiveColor(Vector4(0.20f, 0.45f, 1.0f, 1.0f));
						SAFE_RELEASE(pMaterial);
						SAFE_RELEASE(pMeshRenderer);

						CEffectController*	pEffectController = pSkillEffectObject->AddComponent<CEffectController>("Effect_Lightning_Ctrl");

						pEffectController->SetLifeTime(0.1f);

						SAFE_RELEASE(pEffectController);

						CBillboard*	pBillboard = pSkillEffectObject->AddComponent<CBillboard>("Effect_Lightning_Billboard");
						SAFE_RELEASE(pBillboard);

						SAFE_RELEASE(pEffectTransfrom);
						SAFE_RELEASE(pSkillEffectObject);


						CGameObject* pSkillLightObject = CGameObject::CreateObject("Lightning_Effect", pLayer);
						CTransform*	pSkillLightTransform = pSkillLightObject->GetTransform();

						pSkillLightTransform->SetWorldPosition(vObjTr.x + vAxisZDir.x * 5.f, vObjTr.y, vObjTr.z + vAxisZDir.z * 5.f);

						CPointLight*	pLight = pSkillLightObject->AddComponent<CPointLight>();
						pLight->SetRange(500.f);
						pLight->SetLightColor(Vector4(0.5f, 0.5f, 0.5f, 0.5f), Vector4(0.10f, 0.22f, 0.6f, 1.0f), Vector4(0.20f, 0.45f, 1.0f, 1.0f));
						SAFE_RELEASE(pLight);

						pEffectController = pSkillLightObject->AddComponent<CEffectController>("Effect_Lightning_Ctrl");
						pEffectController->SetLifeTime(3.0f);
						pEffectController->SetLightRedution();
						SAFE_RELEASE(pEffectController);

						SAFE_RELEASE(pSkillLightTransform);
						SAFE_RELEASE(pSkillLightObject);

						SAFE_RELEASE(pLayer);

						// Effect Distortion
						{
							CLayer* pLayer = m_pGameObject->GetLayer();

							CGameObject* pSpriteObj = CGameObject::CreateObject("LightningDistortion", pLayer);
							//pSpriteObj->SetTransparency(true);
							CTransform*	pSpriteTr = pSpriteObj->GetTransform();

							pSpriteTr->SetWorldScale(0.f, 0.f, 1.f);

							Vector3	vObjTr = m_pTransform->GetWorldPosition();
							pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 2.5f, vObjTr.z);

							CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("LightningDistortion");
							pMeshRenderer->SetMesh(PRIMITIVE_MESH_SPHERE, true);
							pMeshRenderer->SetRenderState(ALPHA_BLEND);
							pMeshRenderer->SetRenderState(CULLNONE);
							pMeshRenderer->SetShader("DistortionAnimShader");
							pMeshRenderer->SetInputLayout(BUMP_ANIM_LAYOUT);

							float fMaxNormalScale;
							CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
							pMaterial->SetDiffuseTexture("distortion2.png", L"Effects/Distortion/distortion2.png");
							pMaterial->SetAlphaColor(Vector4(1.f, 1.f, 1.f, 1.0f));
							pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
							pMaterial->SetEmissiveColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
							fMaxNormalScale = pMaterial->GetNormalScale();
							SAFE_RELEASE(pMaterial);

							SAFE_RELEASE(pMeshRenderer);

							CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_LightningDistortion_Ctrl");

							pEffectController->SetLifeTime(1.0f);
							pEffectController->SetScaleInfo(1.0f, Vector3(0.f, 0.f, 0.f), Vector3(200.f, 200.f, 200.f), true);
							pEffectController->SetNormalScaleInfo(0.9f, fMaxNormalScale, 0.f);

							SAFE_RELEASE(pEffectController);

							CDistortion* pDistortion = pSpriteObj->AddComponent<CDistortion>("Effect_LightningDistortion");
							SAFE_RELEASE(pDistortion);

							SAFE_RELEASE(pSpriteTr);
							SAFE_RELEASE(pSpriteObj);
							SAFE_RELEASE(pLayer);
						}
						//

						// Effect Fall Position Decal
						{
							CLayer* pLayer = m_pGameObject->GetLayer();

							CGameObject* pSpriteObj = CGameObject::CreateObject("Effect_Fall_Position", pLayer);
							pSpriteObj->SetTransparency(true);
							CTransform*	pSpriteTr = pSpriteObj->GetTransform();

							//pSpriteTr->SetPivot(0.5f, 0.5f, 0.5f);
							pSpriteTr->SetWorldScale(15.f, 15.f, 15.f);

							Vector3	vObjTr = m_pTransform->GetWorldPosition();
							pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 2.5f, vObjTr.z);

							CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("Effect_Fall_Position");
							pMeshRenderer->SetMesh(ENGINE_MESH_RECTANGLE_TEX, true);
							pMeshRenderer->SetRenderState(ALPHA_BLEND);
							pMeshRenderer->SetRenderState(CULLNONE);

							CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
							pMaterial->SetDiffuseTexture("FX_Crack_001_TEX_Clamp_HKB", L"Decals/FX_Crack_001_TEX_Clamp_HKB.tga");
							pMaterial->SetNormalTexture("FX_CrackNormal_001_TEX_HKB", L"Decals/FX_CrackNormal_001_TEX_HKB.tga");
							//pMaterial->SetSpecularTexture("FX_Circle_Ring_Boom01", L"Effects/FX_Circle_Ring_Boom01.tga");
							//pMaterial->SetAlphaTexture("FX_Circle_Ring_Boom01", L"Effects/FX_Circle_Ring_Boom01.tga");
							//pMaterial->SetAlphaColor(Vector4(1.f, 1.f, 1.f, 1.0f));
							pMaterial->SetDiffuseColor(Vector4(1.0f, 0.20f, 0.20f, 1.0f));
							pMaterial->SetEmissiveColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
							SAFE_RELEASE(pMaterial);

							SAFE_RELEASE(pMeshRenderer);

							CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_Fall_Position_Ctrl");

							pEffectController->SetLifeTime(5.0f);
							//pEffectController->SetAlphaInfo(5.0f, 0.f, 1.0f);
							//pEffectController->SetScaleInfo(2.0f, Vector3(0.f, 0.f, 0.f), Vector3(30.f, 30.f, 30.f), true, true);
							//pEffectController->SetRotateInfo(1.f);

							SAFE_RELEASE(pEffectController);

							CDecal*	pDecal = pSpriteObj->AddComponent<CDecal>();
							SAFE_RELEASE(pDecal);

							SAFE_RELEASE(pSpriteTr);
							SAFE_RELEASE(pSpriteObj);
							SAFE_RELEASE(pLayer);
						}
					}
				}
			}

			if (fProgress >= 1.f)
			{
				m_fActiveTime = 0.f;
				m_eStandardState = PLAYER_IDLE1;
				m_eBattleState = PLAYER_BATTLE_NONE;

				// 모션블러 끔
				CGameObject* pMainCameraObject = m_pScene->GetMainCameraObject();
				CPostProcessing* pPostProcessing = pMainCameraObject->GetComponent<CPostProcessing>();
				if (pPostProcessing)
				{
					uint32_t iFlags = pPostProcessing->GetPostProcessingFlags();
					if (iFlags & PT_MOTION_BLUR)
						pPostProcessing->ToggleFlags(PT_MOTION_BLUR);
					SAFE_RELEASE(pPostProcessing);
				}
				SAFE_RELEASE(pMainCameraObject);

				CSphereCollider* pPlayerCollision = m_pGameObject->GetComponent<CSphereCollider>();
				pPlayerCollision->Enable(true);
				SAFE_RELEASE(pPlayerCollision);

				
			}

			if (m_bSoundPlay)
			{
				m_bSoundPlay = false;
				GET_SINGLE(CSoundManager)->Play("BladeDance");
			}
		}
		break;
		case PLAYER_SMASH_A2:
		{
			m_fActiveMaxTime = 1.5f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;

			if (fProgress >= 0.35f && m_bSoundPlay)
			{
				m_bSoundPlay = false;

				GET_SINGLE(CSoundManager)->Play("Skill001_SFX");
				GET_SINGLE(CSoundManager)->Play("Skill001_Voice");
			}

			else if (fProgress >= 1.f)
			{
				m_pStateController->SetInteger("Standard", 0);

				m_fActiveTime = 0.f;
				m_eStandardState = PLAYER_IDLE1;
				m_eBattleState = PLAYER_BATTLE_NONE;
				m_pTransform->SetWorldPosition(m_pTransform->GetWorldPosition() + m_pTransform->forward() * 5.f);

				CGameObject* pPlayerCameraObj = m_pLayer->FindGameObjectWithTag("Camera Gimbal Y");
				if (pPlayerCameraObj)
				{
					CPlayerCamera* pPlayerCamera = pPlayerCameraObj->GetComponent<CPlayerCamera>();
					pPlayerCamera->SetPlayer(m_pGameObject);
					SAFE_RELEASE(pPlayerCamera);
					SAFE_RELEASE(pPlayerCameraObj);
				}

				Vector3 vPos = m_pTransform->GetWorldPosition();
				vPos += m_pTransform->forward() * 5.f;
				m_pTransform->SetWorldPosition(vPos);

				CSphereCollider* pPlayerCollision = m_pGameObject->GetComponent<CSphereCollider>();
				pPlayerCollision->Enable(true);
				SAFE_RELEASE(pPlayerCollision);
			}
			if (fProgress >= 0.35f && m_bSkillEffect)
			{
				m_bSkillEffect = false;

				// Effect
				{
					CLayer* pLayer = m_pGameObject->GetLayer();

					CGameObject* pSpriteObj = CGameObject::CreateObject("Effect", pLayer);
					pSpriteObj->SetTransparency(true);
					CTransform*	pSpriteTr = pSpriteObj->GetTransform();

					//pSpriteTr->SetWorldScale(256.f, 256.0f, 1.0f);
					pSpriteTr->SetWorldPosition(m_pTransform->GetWorldPosition().x, m_pTransform->GetWorldPosition().y - 50.f, 1.0f);

					pSpriteTr->SetWorldScale(5.f, 5.f, 1.f);
					pSpriteTr->SetWorldPosition(m_pTransform->GetWorldPosition());
					//pSpriteTr->SetWorldRotation(m_pTransform->GetWorldRotation());

					CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("MeshEffect001");
					pMeshRenderer->SetMesh(ENGINE_MESH_RECTANGLE_TEX, true);
					pMeshRenderer->SetRenderState(ALPHA_BLEND);
					pMeshRenderer->SetRenderState(CULLNONE);

					CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
					pMaterial->SetDiffuseTexture("FX_Circle_Ring_Boom01", L"Effects/FX_Circle_Ring_Boom01.tga");
					pMaterial->SetAlphaTexture("FX_Circle_Ring_Boom01", L"Effects/FX_Circle_Ring_Boom01.tga");
					pMaterial->SetAlphaColor(Vector4(0.f, 0.f, 0.f, 1.0f));
					pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					SAFE_RELEASE(pMaterial);

					SAFE_RELEASE(pMeshRenderer);
					//CSpriteRenderer*	pSpriteRenderer = pSpriteObj->AddComponent<CSpriteRenderer>("Effect001");
					//pSpriteRenderer->SetShader(STANDARD_TEXTURE_SHADER);
					//pSpriteRenderer->SetDiffuseTexture("Effect", L"Effects\\FX_Impact_015_TEX_KKJ.tga");
					//pSpriteRenderer->SetImageInfo("Effect");
					//SAFE_RELEASE(pSpriteRenderer);

					CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect001_Ctrl");

					Vector3	vMin = { 0.f, 0.f, 0.f };
					Vector3	vMax = { 100.f, 100.f, 1.0f };
					pEffectController->SetLifeTime(0.15f);
					pEffectController->SetScaleInfo(0.15f, vMin, vMax, true);

					//pEffectController->RotationEffect(true, 5.f);
					SAFE_RELEASE(pEffectController);

					CBillboard*	pBillboard = pSpriteObj->AddComponent<CBillboard>();
					SAFE_RELEASE(pBillboard);

					SAFE_RELEASE(pSpriteTr);
					SAFE_RELEASE(pSpriteObj);
					SAFE_RELEASE(pLayer);

					PlayerActiveAttackSphere(6.f, ATTACK_NORMAL, 30.f, 0.25f,
						m_pTransform->GetWorldPosition() + m_pTransform->forward() * 5.f);
				}
			}

			if(fProgress <= 0.5f)
			{
				CTransform* pDummyTr = m_pDummyForCameraWork->GetTransform();
				Vector3 vPos = pDummyTr->GetWorldPosition();
				vPos += m_pTransform->forward() * 10.f * fTime;
				pDummyTr->SetWorldPosition(vPos);
				SAFE_RELEASE(pDummyTr);
			}
		}
		break;
		// 3번스킬 Start
		case PLAYER_SKILL_B1:
		{
			m_fActiveMaxTime = 0.5f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			if (fProgress >= 1.f)
			{
				m_fActiveTime = 0.f;
				m_eBattleState = PLAYER_SKILL_B2;

				// Effect MagicCircle1
				{
					CLayer* pLayer = m_pGameObject->GetLayer();

					CGameObject* pSpriteObj = CGameObject::CreateObject("Effect_MagicCircle001", pLayer);
					pSpriteObj->SetTransparency(true);
					CTransform*	pSpriteTr = pSpriteObj->GetTransform();

					pSpriteTr->SetPivot(0.5f, 0.5f, 0.5f);
					pSpriteTr->SetWorldScale(0.f, 0.f, 1.f);

					Vector3	vObjTr = m_pTransform->GetWorldPosition();
					pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 0.2f, vObjTr.z);

					CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("Effect_MagicCircle001");
					pMeshRenderer->SetMesh(ENGINE_MESH_RECTANGLE_TEX, true);
					pMeshRenderer->SetRenderState(ALPHA_BLEND);
					pMeshRenderer->SetRenderState(CULLNONE);

					CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
					pMaterial->SetDiffuseTexture("fx_NPCZONE_001_TEX_KKJ", L"Effects/fx_NPCZONE_001_TEX_KKJ.tga");
					pMaterial->SetAlphaTexture("fx_NPCZONE_001_TEX_KKJ", L"Effects/fx_NPCZONE_001_TEX_KKJ.tga");
					pMaterial->SetAlphaColor(Vector4(0.f, 0.f, 0.f, 1.0f));
					pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					pMaterial->SetEmissiveColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					SAFE_RELEASE(pMaterial);

					SAFE_RELEASE(pMeshRenderer);

					CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_MagicCircle001_Ctrl");
					pEffectController->SetLifeTime(3.7f);
					pEffectController->UseBurnEffect(1.0f);
					pEffectController->SetScaleInfo(1.0f, Vector3(0.f, 0.f, 1.f), Vector3(50.f, 50.f, 1.f), true);
					pEffectController->SetRotateInfo(0.25f);
					SAFE_RELEASE(pEffectController);

					SAFE_RELEASE(pSpriteTr);
					SAFE_RELEASE(pSpriteObj);


					// Effect Distortion3
					{
						CGameObject* pSpriteObj = CGameObject::CreateObject("MeteoEndDistortion3", pLayer);
						CTransform*	pSpriteTr = pSpriteObj->GetTransform();

						//pSpriteTr->SetPivot(0.5f, 0.5f, 0.5f);
						pSpriteTr->SetWorldScale(0.f, 0.f, 1.f);

						Vector3	vObjTr = m_pTransform->GetWorldPosition();
						pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 2.5f, vObjTr.z);

						CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("MeteoEndDistortion3");
						pMeshRenderer->SetMesh(PRIMITIVE_MESH_SPHERE, true);
						pMeshRenderer->SetRenderState(ALPHA_BLEND);
						pMeshRenderer->SetRenderState(CULLNONE);
						pMeshRenderer->SetShader("DistortionAnimShader");
						pMeshRenderer->SetInputLayout(BUMP_ANIM_LAYOUT);

						float fMaxNormalScale;
						CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
						pMaterial->SetDiffuseTexture("distortion2.png", L"Effects/Distortion/distortion2.png");
						pMaterial->SetAlphaColor(Vector4(1.f, 1.f, 1.f, 1.0f));
						pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
						pMaterial->SetEmissiveColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
						fMaxNormalScale = pMaterial->GetNormalScale();
						SAFE_RELEASE(pMaterial);

						SAFE_RELEASE(pMeshRenderer);

						CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_MeteoEndDistortion_Ctrl3");
						pEffectController->SetLifeTime(1.f);
						pEffectController->SetScaleInfo(1.f, Vector3(100.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), true);
						pEffectController->SetNormalScaleInfo(0.9f, fMaxNormalScale, 0.f);
						SAFE_RELEASE(pEffectController);

						CDistortion* pDistortion = pSpriteObj->AddComponent<CDistortion>("Effect_MeteoEndDistortion3");
						SAFE_RELEASE(pDistortion);

						SAFE_RELEASE(pSpriteTr);
						SAFE_RELEASE(pSpriteObj);
					}

					SAFE_RELEASE(pLayer);
				}

				{
					CLayer* pLayer = m_pGameObject->GetLayer();

					CGameObject* pParticleObj = CGameObject::CreateObject("TimeParticle001", pLayer);

					CTransform*	pParticleTr = pParticleObj->GetTransform();
					Vector3	vObjTr = m_pTransform->GetWorldPosition();
					pParticleTr->SetWorldPosition(vObjTr.x, vObjTr.y + 2.8f, vObjTr.z + 0.5f);
					pParticleTr->SetWorldScale(0.1f, 0.1f, 1.0f);

					CEffectController*	pEffectController = pParticleObj->AddComponent<CEffectController>("TimeParticle001_Ctrl");
					pEffectController->SetLifeTime(4.5f);
					pEffectController->SetRotateInfo(5.f);
					SAFE_RELEASE(pEffectController);

					CParticle*	pParticle = pParticleObj->AddComponent<CParticle>();

					pParticle->SetTexture("TimeParticle001", L"Effects\\FX_light_003_TEX_KJS.tga");
					pParticle->SetLoop(true);
					pParticle->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					pParticle->SetAlpha(1.0f, 1.0f);
					pParticle->SetEmissionShape(PES_SPHERE);
					pParticle->SetEmissiveColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					pParticle->SetForce(0, 20);
					pParticle->SetGravity(false);
					pParticle->SetLifetime(0.3f, 0.5f);
					pParticle->SetMaxCount(50);
					pParticle->SetRadius(0);
					pParticle->SetRateOverTime(100.0f);
					pParticle->SetSpeed(1.0f, 5.f);
					pParticle->SetSize(Vector2(5.f, 5.f), Vector2(7.f, 7.f));
					pParticle->SetStartDelay(0);

					SAFE_RELEASE(pParticle);
					SAFE_RELEASE(pParticleTr);
					SAFE_RELEASE(pParticleObj);
					SAFE_RELEASE(pLayer);
				}
			}
			else if (m_bSoundPlay)
			{
				m_bSoundPlay = false;
				GET_SINGLE(CSoundManager)->Play("Kick_Voice");
				GET_SINGLE(CSoundManager)->Play("TimeStop_SFX");
			}


		}
		break;
		// 3번스킬 Loop
		case PLAYER_SKILL_B2:
		{
			m_fActiveMaxTime = 4.3f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			if (fProgress >= 1.f)
			{
				m_fActiveTime = 0.f;
				m_eBattleState = PLAYER_SKILL_B3;
				m_bSkillEffect = true;
				m_bSoundPlay = true;

			}
			else if (m_bSkillEffect && fProgress >= 0.1f && fProgress <= 0.11f)
			{
				m_bSkillEffect = false;				

				// Effect MagicCircle2
				{		
					CLayer* pLayer = m_pGameObject->GetLayer();

					CGameObject* pSpriteObj = CGameObject::CreateObject("Effect_MagicCircle002", pLayer);
					pSpriteObj->SetTransparency(true);
					CTransform*	pSpriteTr = pSpriteObj->GetTransform();

					pSpriteTr->SetWorldScale(0.f, 0.f, 1.f);
					pSpriteTr->SetPivot(0.5f, 0.5f, 0.5f);

					Vector3	vObjTr = m_pTransform->GetWorldPosition();
					pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 3.f, vObjTr.z);

					CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("Effect_MagicCircle002");
					pMeshRenderer->SetMesh(ENGINE_MESH_RECTANGLE_TEX, true);
					pMeshRenderer->SetRenderState(ALPHA_BLEND);
					pMeshRenderer->SetRenderState(CULLNONE);

					CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
					pMaterial->SetDiffuseTexture("FX_Ashlha_MAgic_01_TEX_KKJ", L"Effects/FX_Ashlha_MAgic_01_TEX_KKJ.tga");
					pMaterial->SetAlphaTexture("FX_Ashlha_MAgic_01_TEX_KKJ", L"Effects/FX_Ashlha_MAgic_01_TEX_KKJ.tga");
					pMaterial->SetAlphaColor(Vector4(0.f, 0.f, 0.f, 1.0f));
					pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					pMaterial->SetEmissiveColor(Vector4(1.0f, 1.0f, 0.f, 10.0f));
					SAFE_RELEASE(pMaterial);

					SAFE_RELEASE(pMeshRenderer);

					CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_MagicCircle002_Ctrl");
					pEffectController->SetLifeTime(3.3f);
					pEffectController->UseBurnEffect(1.0f);
					pEffectController->SetScaleInfo(2.8f, Vector3(0.f, 0.f, 1.f), Vector3(30.f, 30.f, 1.f), true);
					pEffectController->SetRotateInfo(0.5f);
					SAFE_RELEASE(pEffectController);

					SAFE_RELEASE(pSpriteTr);
					SAFE_RELEASE(pSpriteObj);

					// Effect Distortion3
					{
						CGameObject* pSpriteObj = CGameObject::CreateObject("MeteoEndDistortion3", pLayer);
						CTransform*	pSpriteTr = pSpriteObj->GetTransform();

						//pSpriteTr->SetPivot(0.5f, 0.5f, 0.5f);
						pSpriteTr->SetWorldScale(0.f, 0.f, 1.f);

						Vector3	vObjTr = m_pTransform->GetWorldPosition();
						pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 2.5f, vObjTr.z);

						CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("MeteoEndDistortion3");
						pMeshRenderer->SetMesh(PRIMITIVE_MESH_SPHERE, true);
						pMeshRenderer->SetRenderState(ALPHA_BLEND);
						pMeshRenderer->SetRenderState(CULLNONE);
						pMeshRenderer->SetShader("DistortionAnimShader");
						pMeshRenderer->SetInputLayout(BUMP_ANIM_LAYOUT);

						float	fMaxNormalScale;
						CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
						pMaterial->SetDiffuseTexture("distortion2.png", L"Effects/Distortion/distortion2.png");
						pMaterial->SetAlphaColor(Vector4(1.f, 1.f, 1.f, 1.0f));
						pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
						pMaterial->SetEmissiveColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
						fMaxNormalScale = pMaterial->GetNormalScale();
						SAFE_RELEASE(pMaterial);

						SAFE_RELEASE(pMeshRenderer);

						CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_MeteoEndDistortion_Ctrl3");
						pEffectController->SetLifeTime(1.f);
						pEffectController->SetScaleInfo(1.f, Vector3(100.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), true);
						pEffectController->SetNormalScaleInfo(0.9f, fMaxNormalScale, 0.f);
						SAFE_RELEASE(pEffectController);

						CDistortion* pDistortion = pSpriteObj->AddComponent<CDistortion>("Effect_MeteoEndDistortion3");
						SAFE_RELEASE(pDistortion);

						SAFE_RELEASE(pSpriteTr);
						SAFE_RELEASE(pSpriteObj);
					}
					SAFE_RELEASE(pLayer);
				}
			}
			else if (!m_bSkillEffect && fProgress >= 0.12 && fProgress <= 0.13)
			{
				m_bSkillEffect = true;
			}

			else if (m_bSkillEffect && fProgress >= 0.13f && fProgress <= 0.14f)
			{
				m_bSkillEffect = false;
				// Effect Magicword001
				{
					CLayer* pLayer = m_pGameObject->GetLayer();

					CGameObject* pSpriteObj = CGameObject::CreateObject("Effect_Magicword001", pLayer);
					pSpriteObj->SetTransparency(true);
					CTransform*	pSpriteTr = pSpriteObj->GetTransform();

					pSpriteTr->SetPivot(0.5f, 0.5f, 0.5f);
					pSpriteTr->SetWorldScale(0.f, 0.f, 1.f);

					Vector3	vObjTr = m_pTransform->GetWorldPosition();
					pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 2.5f, vObjTr.z);

					CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("Effect_Magicword001");
					pMeshRenderer->SetMesh(ENGINE_MESH_RECTANGLE_TEX, true);
					pMeshRenderer->SetRenderState(ALPHA_BLEND);
					pMeshRenderer->SetRenderState(CULLNONE);

					CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
					pMaterial->SetDiffuseTexture("FX_mysticword_009_TEX_KJS", L"Effects/FX_mysticword_009_TEX_KJS.tga");
					pMaterial->SetAlphaTexture("FX_mysticword_009_TEX_KJS", L"Effects/FX_mysticword_009_TEX_KJS.tga");
					pMaterial->SetAlphaColor(Vector4(0.f, 0.f, 0.f, 1.0f));
					pMaterial->SetDiffuseColor(Vector4(1.0f, 0.15f, 0.15f, 1.0f));
					pMaterial->SetEmissiveColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					SAFE_RELEASE(pMaterial);
					SAFE_RELEASE(pMeshRenderer);

					CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_Magicword001_Ctrl");
					pEffectController->SetLifeTime(2.0f);
					pEffectController->UseBurnEffect(2.0f);
					pEffectController->SetScaleInfo(1.0f, Vector3(0.f, 0.f, 1.f), Vector3(15.f, 15.f, 1.f), true);
					pEffectController->SetRotateInfo(4.f);

					SAFE_RELEASE(pEffectController);

					CBillboard*	pBillboard = pSpriteObj->AddComponent<CBillboard>("Effect_Magicword001_Billboard");
					pBillboard->SetRotate(true);
					SAFE_RELEASE(pBillboard);

					SAFE_RELEASE(pSpriteTr);
					SAFE_RELEASE(pSpriteObj);


					//// Effect Distortion3
					//{
					//	CGameObject* pSpriteObj = CGameObject::CreateObject("MeteoEndDistortion3", pLayer);
					//	CTransform*	pSpriteTr = pSpriteObj->GetTransform();

					//	pSpriteTr->SetWorldScale(0.f, 0.f, 1.f);

					//	Vector3	vObjTr = m_pTransform->GetWorldPosition();
					//	pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 2.5f, vObjTr.z);

					//	CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("MeteoEndDistortion3");
					//	pMeshRenderer->SetMesh(PRIMITIVE_MESH_SPHERE, true);
					//	pMeshRenderer->SetRenderState(ALPHA_BLEND);
					//	pMeshRenderer->SetRenderState(CULLNONE);
					//	pMeshRenderer->SetShader("DistortionAnimShader");
					//	pMeshRenderer->SetInputLayout(BUMP_ANIM_LAYOUT);

					//	CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
					//	pMaterial->SetDiffuseTexture("distortion2.png", L"Effects/Distortion/distortion2.png");
					//	pMaterial->SetAlphaColor(Vector4(1.f, 1.f, 1.f, 1.0f));
					//	pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					//	pMaterial->SetEmissiveColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					//	SAFE_RELEASE(pMaterial);

					//	SAFE_RELEASE(pMeshRenderer);

					//	CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_MeteoEndDistortion_Ctrl3");

					//	pEffectController->SetLifeTime(1.f);
					//	pEffectController->SetScaleInfo(1.f, Vector3(100.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), true);
					//	pEffectController->SetRotateInfo(10.f);

					//	SAFE_RELEASE(pEffectController);

					//	CDistortion* pDistortion = pSpriteObj->AddComponent<CDistortion>("Effect_MeteoEndDistortion3");
					//	SAFE_RELEASE(pDistortion);

					//	SAFE_RELEASE(pSpriteTr);
					//	SAFE_RELEASE(pSpriteObj);
					//}

					SAFE_RELEASE(pLayer);
				}
			}

			else if (!m_bSkillEffect && fProgress >= 0.15 && fProgress <= 0.16)
			{
				m_bSkillEffect = true;
			}

			else if (m_bSkillEffect && fProgress >= 0.3f && fProgress <= 0.31f)
			{
				m_bSkillEffect = false;
				// Effect MagicCircle003
				{
					CLayer* pLayer = m_pGameObject->GetLayer();

					CGameObject* pSpriteObj = CGameObject::CreateObject("Effect_MagicCircle003", pLayer);
					pSpriteObj->SetTransparency(true);
					CTransform*	pSpriteTr = pSpriteObj->GetTransform();

					pSpriteTr->SetPivot(0.5f, 0.5f, 0.5f);
					pSpriteTr->SetWorldScale(0.f, 0.f, 1.f);
					pSpriteTr->SetWorldRotationX(90.f);

					Vector3	vObjTr = m_pTransform->GetWorldPosition();
					pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 7.0f, vObjTr.z);

					CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("Effect_MagicCircle003");
					pMeshRenderer->SetMesh(ENGINE_MESH_RECTANGLE_TEX, true);
					pMeshRenderer->SetRenderState(ALPHA_BLEND);
					pMeshRenderer->SetRenderState(CULLNONE);

					CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
					pMaterial->SetDiffuseTexture("FX_NewMagic_001_TEX_KKJ", L"Effects/FX_NewMagic_001_TEX_KKJ.tga");
					pMaterial->SetAlphaTexture("FX_NewMagic_001_TEX_KKJ", L"Effects/FX_NewMagic_001_TEX_KKJ.tga");
					pMaterial->SetAlphaColor(Vector4(0.f, 0.f, 0.f, 1.0f));
					pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					pMaterial->SetEmissiveColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

					SAFE_RELEASE(pMaterial);
					SAFE_RELEASE(pMeshRenderer);

					CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_MagicCircle003_Ctrl");
					pEffectController->SetLifeTime(2.4f);
					pEffectController->UseBurnEffect(1.0f);
					pEffectController->SetScaleInfo(1.0f, Vector3(0.f, 0.f, 1.f), Vector3(25.f, 25.f, 1.f), true);
					pEffectController->SetRotateInfo(0.5f);

					SAFE_RELEASE(pEffectController);

					SAFE_RELEASE(pSpriteTr);
					SAFE_RELEASE(pSpriteObj);


					// Effect Distortion3
					{
						CGameObject* pSpriteObj = CGameObject::CreateObject("MeteoEndDistortion3", pLayer);
						CTransform*	pSpriteTr = pSpriteObj->GetTransform();

						pSpriteTr->SetWorldScale(0.f, 0.f, 1.f);

						Vector3	vObjTr = m_pTransform->GetWorldPosition();
						pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 2.5f, vObjTr.z);

						CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("MeteoEndDistortion3");
						pMeshRenderer->SetMesh(PRIMITIVE_MESH_SPHERE, true);
						pMeshRenderer->SetRenderState(ALPHA_BLEND);
						pMeshRenderer->SetRenderState(CULLNONE);
						pMeshRenderer->SetShader("DistortionAnimShader");
						pMeshRenderer->SetInputLayout(BUMP_ANIM_LAYOUT);

						float fMaxNormalScale;
						CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
						pMaterial->SetDiffuseTexture("distortion2.png", L"Effects/Distortion/distortion2.png");
						pMaterial->SetAlphaColor(Vector4(1.f, 1.f, 1.f, 1.0f));
						pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
						pMaterial->SetEmissiveColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
						fMaxNormalScale = pMaterial->GetNormalScale();
						SAFE_RELEASE(pMaterial);

						SAFE_RELEASE(pMeshRenderer);

						CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_MeteoEndDistortion_Ctrl3");

						pEffectController->SetLifeTime(1.f);
						pEffectController->SetScaleInfo(1.f, Vector3(100.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), true);
						pEffectController->SetNormalScaleInfo(0.9f, fMaxNormalScale, 0.f);

						SAFE_RELEASE(pEffectController);

						CDistortion* pDistortion = pSpriteObj->AddComponent<CDistortion>("Effect_MeteoEndDistortion3");
						SAFE_RELEASE(pDistortion);

						SAFE_RELEASE(pSpriteTr);
						SAFE_RELEASE(pSpriteObj);
					}

					SAFE_RELEASE(pLayer);
				}
			}
		}
		break;
		// 3번스킬 End
		case PLAYER_SKILL_B3:
		{
			m_fActiveMaxTime = 0.5f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			if (fProgress >= 1.f)
			{
				CSphereCollider* pPlayerCollision = m_pGameObject->GetComponent<CSphereCollider>();
				pPlayerCollision->Enable(true);
				SAFE_RELEASE(pPlayerCollision);

				//Vector3	vAxisZ = m_pTransform->GetWorldAxis(AX_Z);
				//Vector3 vAxisZDir = vAxisZ.Normalize();

				m_fActiveTime = 0.f;
				m_eStandardState = PLAYER_IDLE1;
				m_eBattleState = PLAYER_BATTLE_NONE;

				CGameObject* pMeteoObject = CGameObject::Load(m_pScene, L"Prefab\\MeteoStone_LocalSet.SSSPrefab", DATA_PATH);
				CTransform*	pMeteoTransform = pMeteoObject->GetTransform();
				pMeteoTransform->SetPivot(0.5f, 0.5f, 0.5f);
				pMeteoTransform->SetWorldScale(0.5f, 0.5f, 0.5f);

				int iChildCount = pMeteoTransform->GetChildrenCount();
				if (iChildCount > 0)
				{
					vector<CTransform*>*	pMeteoChildren = pMeteoTransform->GetChildren();
					vector<CTransform*>::iterator	iter;
					vector<CTransform*>::iterator	iterEnd = pMeteoChildren->end();

					for (iter = pMeteoChildren->begin(); iter != iterEnd; ++iter)
					{
						CGameObject*	pMeteoChildObject = (*iter)->GetGameObject();

						if (pMeteoChildObject->HasComponent<CMeshCollider>())
						{
							CMeshCollider*	pMeteoCollider = pMeteoChildObject->GetComponent<CMeshCollider>();
							pMeteoCollider->SetRestitution(0.1f);
							SAFE_RELEASE(pMeteoCollider);
						}
						else
						{
							CMeshCollider*	pMeteoCollider = pMeteoChildObject->AddComponent<CMeshCollider>();
							pMeteoCollider->SetRestitution(0.1f);
							pMeteoCollider->BakeConvexMesh();
							SAFE_RELEASE(pMeteoCollider);
						}

						if (pMeteoChildObject->HasComponent<CRigidbody>())
						{
							CRigidbody*	pChildRigidbody = pMeteoChildObject->GetComponent<CRigidbody>();
							pChildRigidbody->SetKinematic(true);
							pChildRigidbody->SetUseGravity(false);
							pChildRigidbody->SetDrag(1.f);
							SAFE_RELEASE(pChildRigidbody)
						}
						else
						{
							CRigidbody*	pChildRigidbody = pMeteoChildObject->AddComponent<CRigidbody>();
							pChildRigidbody->SetKinematic(true);
							pChildRigidbody->SetUseGravity(false);
							pChildRigidbody->SetDrag(1.f);
							SAFE_RELEASE(pChildRigidbody)
						}

						CGameObject* pMeteoFire = CGameObject::Load(m_pScene, L"Prefab\\Effects\\RedFire_BoneDragon.SSSPrefab", DATA_PATH);
						CTransform*		pFireTransform = pMeteoFire->GetTransform();

						pFireTransform->SetParent(*iter);
						pFireTransform->SetLocalScale(70.f, 70.f, 1.f);
						pFireTransform->SetWorldScale(70.f, 70.f, 1.f);

						//CMeshRenderer*	pFireRenderer = pMeteoFire->GetComponent<CMeshRenderer>();
						//pFireRenderer->SetRenderState(DEPTH_DISABLE);
						//SAFE_RELEASE(pFireRenderer);

						CPointLight*	pFireLight = pMeteoFire->AddComponent<CPointLight>();
						pFireLight->SetRange(20.f);
						pFireLight->SetLightColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.6f, 0.1f, 0.1f, 1.0f), Vector4(0.6f, 0.1f, 0.1f, 1.0f));
						SAFE_RELEASE(pFireLight);


						CEffectController*	pEffectController = pMeteoFire->AddComponent<CEffectController>("Effect_MeteoFire_Ctrl3");

						pEffectController->SetLifeTime(1.f, false);
						pEffectController->UseMeteoFire(m_vCursorClickPosition);

						SAFE_RELEASE(pEffectController);

						SAFE_RELEASE(pFireTransform);
						SAFE_RELEASE(pMeteoFire);


						/*CGameObject*	pMeteoFire = CGameObject::CreateObject("MeteoFire", m_pLayer);
						CTransform*		pFireTransform = pMeteoFire->GetTransform();

						pFireTransform->SetLocalScale(10.f, 10.f, 1.f);
						pFireTransform->SetWorldScale(10.f, 10.f, 1.f);
						pFireTransform->SetParent(pMeteoTransform);

						CFire*	pFire = pMeteoFire->AddComponent<CFire>();
						SAFE_RELEASE(pFire);

						CBillboard* pBillboard = pMeteoFire->AddComponent<CBillboard>();
						SAFE_RELEASE(pBillboard);

						CMeshRenderer*	pFireRenderer = pMeteoFire->GetComponent<CMeshRenderer>();
						pFireRenderer->SetRenderState(DEPTH_DISABLE);

						CMaterial*	pMaterial = pFireRenderer->GetMaterial();
						pMaterial->SetDiffuseTexture("FX_Flame_001_Tex_CJH", L"Effects/Fire/FX_Flame_001_Tex_CJH.tga");
						pMaterial->SetNormalTexture("noise03", L"Effects/Fire/noise03.tga");
						pMaterial->SetAlphaTexture("FX_fireMask_01", L"Effects/Fire/FX_fireMask_01.tga");
												
						SAFE_RELEASE(pMaterial);
						SAFE_RELEASE(pFireRenderer);					

						SAFE_RELEASE(pFireTransform);
						SAFE_RELEASE(pMeteoFire);*/
					}
				}				

				CMeshRenderer*	pMeshRenderer = pMeteoObject->AddComponent<CMeshRenderer>("Effect_MeteoFire001");
				pMeshRenderer->SetMesh(PRIMITIVE_MESH_SPHERE, true);
				pMeshRenderer->SetRenderState(ALPHA_BLEND);
				pMeshRenderer->SetRenderState(CULLNONE);

				CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
				pMaterial->SetDiffuseTexture("FX_fire_005_TEX_KJS", L"Effects/fire/FX_fire_005_TEX_KJS.tga");
				pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 0.25f));
				pMaterial->SetEmissiveColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

				SAFE_RELEASE(pMaterial);
				SAFE_RELEASE(pMeshRenderer);

				CPointLight*	pMeteoLight = pMeteoObject->AddComponent<CPointLight>();
				pMeteoLight->SetRange(500.f);
				pMeteoLight->SetLightColor(Vector4(1.0f, 0.141f, 0.f, 1.0f), Vector4(1.0f, 0.141f, 0.f, 1.0f), Vector4(1.0f, 0.141f, 0.f, 1.0f));
				SAFE_RELEASE(pMeteoLight);

				CEffectController*	pMeteoController = pMeteoObject->AddComponent<CEffectController>("MeteoController");
				pMeteoController->SetLifeTime(10.f);
				Vector3 vStartPosition = Vector3(m_vCursorClickPosition.x, m_vCursorClickPosition.y + 1500.f,
					m_vCursorClickPosition.z);

				pMeteoController->SetMoveInfo(vStartPosition, m_vCursorClickPosition, 1.f, true);
				pMeteoController->SetUseMeteo();

				SAFE_RELEASE(pMeteoController);
				SAFE_RELEASE(pMeteoTransform);
				SAFE_RELEASE(pMeteoObject);
			}

			if (fProgress >= 0.3f && m_bSoundPlay)
			{
				m_bSoundPlay = false;

				GET_SINGLE(CSoundManager)->Play("Smash_Voice");
			}

			if (fProgress >= 0.7f && m_bSkillEffect)
			{
				m_bSkillEffect = false;
				// Effect MagicEnd
				{
					CLayer* pLayer = m_pGameObject->GetLayer();

					CGameObject* pSpriteObj = CGameObject::CreateObject("Effect_MagicEnd", pLayer);
					pSpriteObj->SetTransparency(true);
					CTransform*	pSpriteTr = pSpriteObj->GetTransform();

					pSpriteTr->SetPivot(0.5f, 0.5f, 0.5f);
					//pSpriteTr->SetWorldScale(50.f, 50.f, 1.f);
					pSpriteTr->SetWorldScale(0.f, 0.f, 1.f);
					//pSpriteTr->SetWorldRotationX(90.f);

					Vector3	vObjTr = m_pTransform->GetWorldPosition();
					pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 2.5f, vObjTr.z);

					CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("Effect_MagicEnd");
					pMeshRenderer->SetMesh(ENGINE_MESH_RECTANGLE_TEX, true);
					pMeshRenderer->SetRenderState(ALPHA_BLEND);
					pMeshRenderer->SetRenderState(CULLNONE);

					CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
					pMaterial->SetDiffuseTexture("FX_Hiteffect_002_TEX_KJS", L"Effects/FX_Hiteffect_002_TEX_KJS.tga");
					pMaterial->SetAlphaTexture("FX_Hiteffect_002_TEX_KJS", L"Effects/FX_Hiteffect_002_TEX_KJS.tga");
					pMaterial->SetAlphaColor(Vector4(1.f, 1.f, 1.f, 1.0f));
					pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					pMaterial->SetEmissiveColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					SAFE_RELEASE(pMaterial);

					SAFE_RELEASE(pMeshRenderer);

					CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_MagicEnd_Ctrl");

					pEffectController->SetLifeTime(0.5f);
					pEffectController->SetScaleInfo(0.5f, Vector3(0.f, 0.f, 1.f), Vector3(200.f, 200.f, 1.f), true);

					SAFE_RELEASE(pEffectController);

					CBillboard*	pBillboard = pSpriteObj->AddComponent<CBillboard>("Effect_MagicEnd_Billboard");
					SAFE_RELEASE(pBillboard);

					//CDistortion*	pDistortion = pSpriteObj->AddComponent<CDistortion>("Effect_MagicEndDistortion");
					//SAFE_RELEASE(pDistortion);

					SAFE_RELEASE(pSpriteTr);
					SAFE_RELEASE(pSpriteObj);
					SAFE_RELEASE(pLayer);
				}

				// Effect Distortion
				{
					CLayer* pLayer = m_pGameObject->GetLayer();

					CGameObject* pSpriteObj = CGameObject::CreateObject("MagicEndDistortion", pLayer);
					//pSpriteObj->SetTransparency(true);
					CTransform*	pSpriteTr = pSpriteObj->GetTransform();

					//pSpriteTr->SetPivot(0.5f, 0.5f, 0.5f);
					pSpriteTr->SetWorldScale(0.f, 0.f, 1.f);

					Vector3	vObjTr = m_pTransform->GetWorldPosition();
					pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 2.5f, vObjTr.z);

					CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("MagicEndDistortion");
					pMeshRenderer->SetMesh(PRIMITIVE_MESH_SPHERE, true);
					pMeshRenderer->SetRenderState(ALPHA_BLEND);
					pMeshRenderer->SetRenderState(CULLNONE);
					pMeshRenderer->SetShader("DistortionAnimShader");
					pMeshRenderer->SetInputLayout(BUMP_ANIM_LAYOUT);

					float fMaxNormalScale;
					CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
					pMaterial->SetDiffuseTexture("distortion2.png", L"Effects/Distortion/distortion2.png");
					pMaterial->SetAlphaColor(Vector4(1.f, 1.f, 1.f, 1.0f));
					pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					pMaterial->SetEmissiveColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					fMaxNormalScale = pMaterial->GetNormalScale();
					SAFE_RELEASE(pMaterial);

					SAFE_RELEASE(pMeshRenderer);

					CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_MagicEndDistortion_Ctrl");

					pEffectController->SetLifeTime(0.5f);
					pEffectController->SetScaleInfo(0.5f, Vector3(0.f, 0.f, 0.f), Vector3(200.f, 200.f, 200.f), true);
					pEffectController->SetNormalScaleInfo(0.45f, fMaxNormalScale, 0.f);

					SAFE_RELEASE(pEffectController);

					CDistortion* pDistortion = pSpriteObj->AddComponent<CDistortion>("Effect_MagicEndDistortion");
					SAFE_RELEASE(pDistortion);

					//CBillboard*	pBillboard = pSpriteObj->AddComponent<CBillboard>();
					//SAFE_RELEASE(pBillboard);

					SAFE_RELEASE(pSpriteTr);
					SAFE_RELEASE(pSpriteObj);
					SAFE_RELEASE(pLayer);
				}
			}
			else if (fProgress >= 0.21f && fProgress <= 0.3f && !m_bSkillEffect)
			{
				m_bSkillEffect = true;
			}
			else if (fProgress >= 0.1f && fProgress <= 0.2f && m_bSkillEffect)
			{
				m_bSkillEffect = false;
				// Effect Meteo Fall Position Circle
				{
					CLayer* pLayer = m_pGameObject->GetLayer();

					CGameObject* pSpriteObj = CGameObject::CreateObject("Effect_Fall_Position", pLayer);
					pSpriteObj->SetTransparency(true);
					CTransform*	pSpriteTr = pSpriteObj->GetTransform();

					pSpriteTr->SetPivot(0.5f, 0.5f, 0.5f);
					pSpriteTr->SetWorldScale(0.f, 0.f, 0.f);
					pSpriteTr->SetWorldPosition(m_vCursorClickPosition);

					CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("Effect_Fall_Position");
					pMeshRenderer->SetMesh(ENGINE_MESH_RECTANGLE_TEX, true);
					pMeshRenderer->SetRenderState(ALPHA_BLEND);
					pMeshRenderer->SetRenderState(CULLNONE);

					CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
					pMaterial->SetDiffuseTexture("FX_Circle_wind03", L"Effects/FX_Circle_wind03.tga");
					pMaterial->SetNormalTexture("FX_Circle_wind03_NRM", L"Effects/FX_Circle_wind03_NRM.tga");
					//pMaterial->SetSpecularTexture("FX_Circle_Ring_Boom01", L"Effects/FX_Circle_Ring_Boom01.tga");
					//pMaterial->SetAlphaTexture("FX_Circle_Ring_Boom01", L"Effects/FX_Circle_Ring_Boom01.tga");
					//pMaterial->SetAlphaColor(Vector4(1.f, 1.f, 1.f, 1.0f));
					pMaterial->SetDiffuseColor(Vector4(1.0f, 0.20f, 0.20f, 1.0f));
					pMaterial->SetEmissiveColor(Vector4(1.0f, 0.14f, 0.1f, 0.5f));
					SAFE_RELEASE(pMaterial);

					SAFE_RELEASE(pMeshRenderer);

					CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_Fall_Position_Ctrl");

					pEffectController->SetLifeTime(2.0f);
					pEffectController->SetScaleInfo(2.0f, Vector3(0.f, 0.f, 0.f), Vector3(30.f, 30.f, 30.f), true, true);
					//pEffectController->SetRotateInfo(1.f);

					SAFE_RELEASE(pEffectController);

					CDecal*	pDecal = pSpriteObj->AddComponent<CDecal>();
					SAFE_RELEASE(pDecal);

					SAFE_RELEASE(pSpriteTr);
					SAFE_RELEASE(pSpriteObj);
					SAFE_RELEASE(pLayer);
				}
			}
		}
		break;

		} // switch 문의 닫는 괄호입니다
	}
	else if (m_eDMGState != PLAYER_DAMAGED_NONE)
	{
		switch (m_eDMGState)
		{// switch문의 여는괄호입니다
			break;
		case PLAYER_DMG_FRONT:
		{
			m_fActiveMaxTime = 0.5f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			if (fProgress >= 1.f)
			{
				m_eDMGState = PLAYER_DAMAGED_NONE;
				m_eStandardState = PLAYER_IDLE1;
				m_fActiveTime = 0.f;
			}
		}
		break;
		case PLAYER_DMG_BACK:
		{
			m_fActiveMaxTime = 0.5f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			if (fProgress >= 1.f)
			{
				m_eDMGState = PLAYER_DAMAGED_NONE;
				m_eStandardState = PLAYER_IDLE1;
				m_fActiveTime = 0.f;
			}
		}
		break;
		case PLAYER_DOWN:
		{
			m_fActiveMaxTime = 1.f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			if (fProgress >= 1.f)
			{
				m_eDMGState = PLAYER_DOWN_LOOP;
				m_fActiveTime = 0.f;
			}
		}
		break;
		case PLAYER_DOWN_LOOP:
		{
			m_fActiveMaxTime = 1.f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			if (fProgress >= 1.f)
			{
				m_eDMGState = PLAYER_RECOVERY;
				m_fActiveTime = 0.f;
			}

			if (GETKEYDOWN(KEY_SPACE))
			{
				m_eStandardState = PLAYER_DODGE_GETUP;
				m_eDMGState = PLAYER_DAMAGED_NONE;
				m_eBattleState = PLAYER_BATTLE_NONE;
				m_fActiveTime = 0.f;

				CSphereCollider* pPlayerCollision = m_pGameObject->GetComponent<CSphereCollider>();
				pPlayerCollision->Enable(false);
				SAFE_RELEASE(pPlayerCollision);

				GET_SINGLE(CSoundManager)->Play("Kick_Voice");
			}
		}
		break;
		case PLAYER_DEADBODY:
		{

		}
		break;
		case PLAYER_RECOVERY:
		{
			m_fActiveMaxTime = 1.f;
			m_fActiveTime += fTime;
			float fProgress = m_fActiveTime / m_fActiveMaxTime;
			if (fProgress >= 1.f)
			{
				m_eDMGState = PLAYER_DAMAGED_NONE;
				m_eStandardState = PLAYER_IDLE1;
				m_fActiveTime = 0.f;
			}
		}
		case PLAYER_DIE:
		{

		}
		break;
		}// switch문의 닫는괄호입니다
	}

}

void CPlayerController::PlayerActiveAttackSphere(float fRadius, ATTACK_ATTRIBUTE eAttribute, float fDamage, float fMaxActiveTime, Vector3 vPosition)
{
	CGameObject* pAttackObj = CGameObject::CreateObject("Attack", m_pLayer);
	CSphereCollider* pCollider = pAttackObj->AddComponent<CSphereCollider>();
	pCollider->SetRadius(fRadius);
	SAFE_RELEASE(pCollider);
	CAttack* pAttack = pAttackObj->AddComponent<CAttack>();
	pAttack->SetValue(1);
	pAttack->SetAttribute(eAttribute);
	pAttack->SetDamagePoint(fDamage);
	pAttack->SetActiveMaxTime(fMaxActiveTime);
	SAFE_RELEASE(pAttack);
	CTransform* pTr = pAttackObj->GetTransform();
	pTr->SetWorldPosition(vPosition);
	SAFE_RELEASE(pTr);
	SAFE_RELEASE(pAttackObj);
}

bool CPlayerController::IsIdleState() const
{
	return m_bOnIdle;
}

int CPlayerController::GetItemIndex(const string & strItemName)
{
	unordered_map<string, ITEMINFO>::iterator iter = m_mapEquipItemInfo.find(strItemName);

	if (iter != m_mapEquipItemInfo.end())
	{
		return iter->second.iID;
	}

	return -1;
}

void CPlayerController::ChangeEquipment(const string & strOnEquipItemName)
{
	ITEMINFO tItem = GetEquipItemInfo(strOnEquipItemName);

	unordered_map<string, ITEMINFO>::iterator iter = m_mapEquipItemInfo.begin();
	unordered_map<string, ITEMINFO>::iterator iterEnd = m_mapEquipItemInfo.end();

	//CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();
	//
	CGameObject* pChild = m_pTransform->GetChild(0)->GetGameObject();
	CMeshRenderer* pRenderer = pChild->GetComponent<CMeshRenderer>();
	SAFE_RELEASE(pChild);
	//
	for (; iter != iterEnd; ++iter)
	{
		int iIndex = iter->second.iID;
		if (iIndex > 0)
		{
			if (iter->second.eEquipPart == tItem.eEquipPart)
			{
				//같은 부위 전부 끔
				pRenderer->SetEnableMeshContainer(iter->second.iID, false);
			}
		}
	}

	//들어온 아이 켬
	pRenderer->SetEnableMeshContainer(tItem.iID, true);

	SAFE_RELEASE(pRenderer);
}

ITEMINFO CPlayerController::GetEquipItemInfo(const string & strItemName)
{
	unordered_map<string, ITEMINFO>::iterator iter = m_mapEquipItemInfo.find(strItemName);
	if (iter != m_mapEquipItemInfo.end())
	{
		return iter->second;
	}
	else
	{
		return ITEMINFO();
	}
}

bool CPlayerController::Initialize()
{
	ITEMINFO tInfo;
	tInfo.eType = IT_EQUIPMENT;
	tInfo.eEquipPart = IEP_HAIR;
	tInfo.eGrade = IG_LEGENDARY;
	tInfo.iPrice = 58000000;

	tInfo.iID = 11;
	tInfo.strName = "HAIR004_DEFAULT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	tInfo.iID = 0;
	tInfo.strName = "HAIR113";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	tInfo.iID = 10;
	tInfo.strName = "HAIR109";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	tInfo.iID = 12;
	tInfo.strName = "HAIR007";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	tInfo.iID = 13;
	tInfo.strName = "HAIR101";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	tInfo.iID = 1;
	tInfo.eEquipPart = IEP_NONE;
	tInfo.strName = "FACE";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));
	tInfo.iID = 2;
	tInfo.strName = "EAR";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	m_eEquipList.vecHair.push_back("HAIR004_DEFAULT");
	m_eEquipList.vecHair.push_back("HAIR113");
	m_eEquipList.vecHair.push_back("HAIR109");
	m_eEquipList.vecHair.push_back("HAIR007");
	m_eEquipList.vecHair.push_back("HAIR101");

#pragma region Upper
	tInfo.iID = 52;
	tInfo.eEquipPart = IEP_UPPER;
	tInfo.strName = "UW0001_UPPER_DEFAULT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 41;
	tInfo.strName = "RB0203_UPPER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 34;
	tInfo.strName = "MS0102_UPPER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 42;
	tInfo.strName = "MD2009_UPPER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 27;
	tInfo.strName = "LT0201_UPPER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 20;
	tInfo.strName = "HV3001_UPPER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 9;
	tInfo.strName = "EC0011_UPPER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 41;
	tInfo.strName = "RB0203_UPPER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	m_eEquipList.vecUPPER.push_back("UW0001_UPPER_DEFAULT");
	m_eEquipList.vecUPPER.push_back("RB0203_UPPER");
	m_eEquipList.vecUPPER.push_back("MS0102_UPPER");
	m_eEquipList.vecUPPER.push_back("MD2009_UPPER");
	m_eEquipList.vecUPPER.push_back("LT0201_UPPER");
	m_eEquipList.vecUPPER.push_back("HV3001_UPPER");
	m_eEquipList.vecUPPER.push_back("EC0011_UPPER");
#pragma endregion

#pragma region SHOULDER


	tInfo.iID = 8;
	tInfo.eEquipPart = IEP_SHOULDER;
	tInfo.strName = "RB2007_SHOULDER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 40;
	tInfo.strName = "RB0203_SHOULDER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 33;
	tInfo.strName = "MS0102_SHOULDER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 48;
	tInfo.strName = "MD2009_SHOULDER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 26;
	tInfo.strName = "LT0201_SHOULDER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 19;
	tInfo.strName = "HV3001_SHOULDER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	m_eEquipList.vecSHOULDER.push_back("RB2007_SHOULDER");
	m_eEquipList.vecSHOULDER.push_back("RB0203_SHOULDER");
	m_eEquipList.vecSHOULDER.push_back("MS0102_SHOULDER");
	m_eEquipList.vecSHOULDER.push_back("MD2009_SHOULDER");
	m_eEquipList.vecSHOULDER.push_back("LT0201_SHOULDER");
	m_eEquipList.vecSHOULDER.push_back("HV3001_SHOULDER");

#pragma endregion

#pragma region LOWER
	tInfo.iID = 51;
	tInfo.eEquipPart = IEP_LOWER;
	tInfo.strName = "UW0001_LOWER_DEFAULT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 39;
	tInfo.strName = "RB0203_LOWER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 32;
	tInfo.strName = "MS0102_LOWER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 47;
	tInfo.strName = "MD2009_LOWER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 25;
	tInfo.strName = "LT0201_LOWER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 18;
	tInfo.strName = "HV3001_LOWER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 6;
	tInfo.strName = "MD0008_LOWER";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	m_eEquipList.vecLOWER.push_back("UW0001_LOWER_DEFAULT");
	m_eEquipList.vecLOWER.push_back("RB0203_LOWER");
	m_eEquipList.vecLOWER.push_back("MS0102_LOWER");
	m_eEquipList.vecLOWER.push_back("MD2009_LOWER");
	m_eEquipList.vecLOWER.push_back("LT0201_LOWER");
	m_eEquipList.vecLOWER.push_back("HV3001_LOWER");
	m_eEquipList.vecLOWER.push_back("MD0008_LOWER");
#pragma endregion

#pragma region HELMET

	tInfo.iID = 38;
	tInfo.eEquipPart = IEP_HELMET;
	tInfo.strName = "RB0203_HELMET";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 31;
	tInfo.strName = "MS0102_HELMET";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 46;
	tInfo.strName = "MD2009_HELMET";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 24;
	tInfo.strName = "LT0201_HELMET";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 17;
	tInfo.strName = "HV3001_HELMET";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 3;
	tInfo.strName = "EC0016_HELMET";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	m_eEquipList.vecHELMET.push_back("RB0203_HELMET");
	m_eEquipList.vecHELMET.push_back("MS0102_HELMET");
	m_eEquipList.vecHELMET.push_back("MD2009_HELMET");
	m_eEquipList.vecHELMET.push_back("LT0201_HELMET");
	m_eEquipList.vecHELMET.push_back("HV3001_HELMET");
	m_eEquipList.vecHELMET.push_back("EC0016_HELMET");
#pragma endregion

#pragma region GLOVES

	tInfo.iID = 50;
	tInfo.eEquipPart = IEP_GLOVES;
	tInfo.strName = "UW0001_GLOVES_DEFAULT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 37;
	tInfo.strName = "RB0203_GLOVES";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 30;
	tInfo.strName = "MS0102_GLOVES";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 45;
	tInfo.strName = "MD2009_GLOVES";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 23;
	tInfo.strName = "LT0201_GLOVES";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 16;
	tInfo.strName = "HV3001_GLOVES";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 5;
	tInfo.strName = "HV0101_GLOVES";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	m_eEquipList.vecGLOVES.push_back("UW0001_GLOVES_DEFAULT");
	m_eEquipList.vecGLOVES.push_back("RB0203_GLOVES");
	m_eEquipList.vecGLOVES.push_back("MS0102_GLOVES");
	m_eEquipList.vecGLOVES.push_back("MD2009_GLOVES");
	m_eEquipList.vecGLOVES.push_back("LT0201_GLOVES");
	m_eEquipList.vecGLOVES.push_back("HV3001_GLOVES");
	m_eEquipList.vecGLOVES.push_back("HV0101_GLOVES");
#pragma endregion

#pragma region BOOTS

	tInfo.iID = 49;
	tInfo.eEquipPart = IEP_BOOTS;
	tInfo.strName = "UW0001_BOOTS_DEFAULT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 36;
	tInfo.strName = "RB0203_BOOTS";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 29;
	tInfo.strName = "MS0102_BOOTS";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 44;
	tInfo.strName = "MD2009_BOOTS";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 22;
	tInfo.strName = "LT0201_BOOTS";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 15;
	tInfo.strName = "HV3001_BOOTS";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 4;
	tInfo.strName = "HV0101_BOOTS";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	m_eEquipList.vecBOOTS.push_back("UW0001_BOOTS_DEFAULT");
	m_eEquipList.vecBOOTS.push_back("RB0203_BOOTS");
	m_eEquipList.vecBOOTS.push_back("MS0102_BOOTS");
	m_eEquipList.vecBOOTS.push_back("MD2009_BOOTS");
	m_eEquipList.vecBOOTS.push_back("LT0201_BOOTS");
	m_eEquipList.vecBOOTS.push_back("HV3001_BOOTS");
	m_eEquipList.vecBOOTS.push_back("HV0101_BOOTS");
#pragma endregion

#pragma region BELT

	tInfo.iID = 7;
	tInfo.eEquipPart = IEP_BELT;
	tInfo.strName = "RB1003_BELT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 35;
	tInfo.strName = "RB0203_BELT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 28;
	tInfo.strName = "MS0102_BELT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 43;
	tInfo.strName = "MD2009_BELT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 21;
	tInfo.strName = "LT0201_BELT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	tInfo.iID = 14;
	tInfo.strName = "HV3001_BELT";
	m_mapEquipItemInfo.insert(make_pair(tInfo.strName, tInfo));

	m_eEquipList.vecBELT.push_back("RB1003_BELT");
	m_eEquipList.vecBELT.push_back("RB0203_BELT");
	m_eEquipList.vecBELT.push_back("MS0102_BELT");
	m_eEquipList.vecBELT.push_back("MD2009_BELT");
	m_eEquipList.vecBELT.push_back("LT0201_BELT");
	m_eEquipList.vecBELT.push_back("HV3001_BELT");
#pragma endregion
	return true;
}

void CPlayerController::Start()
{
	CLayer* pUILayer = m_pScene->FindLayer("UI");
	CGameObject* pPostObj = CGameObject::CreateObject("PostOption", pUILayer);
	CPostOption* pPost = pPostObj->AddComponent<CPostOption>();
	SAFE_RELEASE(pPost);
	SAFE_RELEASE(pPostObj);
	SAFE_RELEASE(pUILayer);

	CGameObject* pPlayerCore = m_pTransform->GetChild(0)->GetGameObject();
	CMeshRenderer* pRenderer = pPlayerCore->GetComponent<CMeshRenderer>();
	size_t iContainerCount = pRenderer->GetContainerCount();

	for (size_t i = 0; i < iContainerCount; ++i)
	{
		pRenderer->SetEnableMeshContainer(i, false);
	}

	pRenderer->SetEnableMeshContainer(GetItemIndex("FACE"), true);
	pRenderer->SetEnableMeshContainer(GetItemIndex("EAR"), true);
	pRenderer->SetEnableMeshContainer(GetItemIndex("UW0001_BOOTS_DEFAULT"), true);
	pRenderer->SetEnableMeshContainer(GetItemIndex("UW0001_GLOVES_DEFAULT"), true);
	pRenderer->SetEnableMeshContainer(GetItemIndex("UW0001_LOWER_DEFAULT"), true);
	pRenderer->SetEnableMeshContainer(GetItemIndex("HAIR004_DEFAULT"), true);
	pRenderer->SetEnableMeshContainer(GetItemIndex("UW0001_UPPER_DEFAULT"), true);

	SAFE_RELEASE(pRenderer);
	SAFE_RELEASE(pPlayerCore);

	ChangeEquipment("EC0011_UPPER");
	ChangeEquipment("LT0201_LOWER");
	ChangeEquipment("MS0102_HELMET");
	ChangeEquipment("MD2009_GLOVES");
	ChangeEquipment("MS0102_BOOTS");
	ChangeEquipment("HAIR113");

	// Point Light..
	CGameObject* pPlayerPointLight = CGameObject::CreateObject("PlayerLight", m_pLayer);
	CPointLight* pPointLight = pPlayerPointLight->AddComponent<CPointLight>();
	pPointLight->SetLightColor(Vector4(1.f, 1.f, 1.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f), Vector4(0.f, 0.f, 0.f, 0.f));
	pPointLight->SetRange(7.5f);
	SAFE_RELEASE(pPointLight);
	CTransform* pPointLightTransform = pPlayerPointLight->GetTransform();

	// 무기 생성
	wstring strWeaponPath = L"Player\\Wepon\\Weapon01.SSSPrefab";
	CGameObject* pWeapon = CGameObject::Load(m_pScene, strWeaponPath.c_str(), MESH_PATH);
	CTransform* pWeaponTransform = pWeapon->GetTransform();
	pWeaponTransform->SetLocalScale(0.1f, 0.1f, 0.1f);

	CGameObject* pWeapon2 = CGameObject::Load(m_pScene, strWeaponPath.c_str(), MESH_PATH);
	CTransform* pWeaponTransform2 = pWeapon2->GetTransform();
	pWeaponTransform2->SetLocalScale(0.1f, 0.1f, 0.1f);

	CGameObject* pMeshCore = m_pTransform->GetChild(0)->GetGameObject();
	CAnimator* pAnimator = pMeshCore->GetComponent<CAnimator>();
	pAnimator->RegisterSocket("Bip01-L-Finger4", pWeaponTransform);
	pAnimator->RegisterSocket("Bip01-R-Finger4", pWeaponTransform2);
	pAnimator->RegisterSocket("Bip01", pPointLightTransform);
	SAFE_RELEASE(pAnimator);
	SAFE_RELEASE(pMeshCore);

	m_pAttackTrail[0] = pWeaponTransform->GetChild(0)->GetGameObject();
	m_pAttackTrail[1] = pWeaponTransform->GetChild(1)->GetGameObject();
	m_pAttackTrail[2] = pWeaponTransform2->GetChild(0)->GetGameObject();
	m_pAttackTrail[3] = pWeaponTransform2->GetChild(1)->GetGameObject();

	SAFE_RELEASE(pWeaponTransform);
	SAFE_RELEASE(pWeaponTransform2);

	SAFE_RELEASE(pWeapon);
	SAFE_RELEASE(pWeapon2);

	SAFE_RELEASE(pPointLightTransform);
	m_pPlayerPointLight = pPlayerPointLight;
	SAFE_RELEASE(pPlayerPointLight);

	// Player 본체 충돌체
	CSphereCollider* pPlayerCollision = m_pGameObject->AddComponent<CSphereCollider>();
	pPlayerCollision->SetRadius(3.f);
	pPlayerCollision->Enable(true);
	SAFE_RELEASE(pPlayerCollision);

	// Player Camera 이동용
	m_pDummyForCameraWork = CGameObject::CreateObject("Dummy", m_pLayer);
	m_pDummyForCameraWork->Release();

	// 애니메이션컨트롤러 받아놓..
	{
		CGameObject* pMeshCore = m_pTransform->GetChild(0)->GetGameObject();
		CAnimator* pAnimator = pMeshCore->GetComponent<CAnimator>();
		SAFE_RELEASE(pMeshCore);
		if (pAnimator)
		{
			CAnimationStateController* pController = pAnimator->GetAnimationController();
			if (pController)
			{
				// 많이 쓸꺼라 멤버변수로 받아놓..
				m_pStateController = pController;
			}

			SAFE_RELEASE(pAnimator);
		}
	}

	/*{
		CGameObject* pMeshCore = m_pTransform->GetChild(0)->GetGameObject();

		CDistortion* pDistortion = pMeshCore->AddComponent<CDistortion>();
		SAFE_RELEASE(pDistortion);

		CMeshRenderer* pMeshRenderer = pMeshCore->GetComponent<CMeshRenderer>();
		pMeshRenderer->SetShader("DistortionAnimShader");
		pMeshRenderer->SetInputLayout(BUMP_ANIM_LAYOUT);
		SAFE_RELEASE(pMeshRenderer);

		
		SAFE_RELEASE(pMeshCore);
	}*/
}

int CPlayerController::Update(float fTime)
{
	CTransform* pBip01Transform = m_pPlayerPointLight->GetTransform();
	Vector3 vLayPosition = pBip01Transform->GetWorldPosition();
	Vector3 vPlayerPosition = m_pTransform->GetWorldPosition();
	SAFE_RELEASE(pBip01Transform);

	RAY tRay;
	tRay.vOrigin = vLayPosition + Vector3(0.0f, 1.5f, 0.0f);
	tRay.vDirection = Vector3(0.0f, -1.0f, 0.0f);
	tRay.fDistance = 100000.0f;

	LINECASTHIT tHit = GET_SINGLE(CCollisionManager)->LineCast(tRay);

	float fResult = -10000.f;
	for (int i = 0; i < tHit.iCount; ++i)
	{
		if (tHit.vecHits[i].bResult)
		{
			if (tHit.vecHits[i].pCollider->IsComponent<CTerrainCollider>())
				if(fResult < tHit.vecHits[i].vPoint.y)
					fResult = tHit.vecHits[i].vPoint.y;

			if (tHit.vecHits[i].pCollider->IsComponent<CMeshCollider>())
			{
				CRigidbody* pRigidbody = tHit.vecHits[i].pCollider->GetComponent<CRigidbody>();
				if (pRigidbody == NULL)
				{
					if (fResult < tHit.vecHits[i].vPoint.y)
						fResult = tHit.vecHits[i].vPoint.y;
				}
				SAFE_RELEASE(pRigidbody);
			}

			if (tHit.vecHits[i].pCollider->IsComponent<CBoxCollider>())
				if (fResult < tHit.vecHits[i].vPoint.y)
					fResult = tHit.vecHits[i].vPoint.y;
		}
	}

	if(fResult != -10000.f)
		m_pTransform->SetWorldPosition(vPlayerPosition.x, fResult, vPlayerPosition.z);

	CGameObject* pPlayerCore = m_pTransform->GetChild(0)->GetGameObject();
	CAnimator* pAnimator = pPlayerCore->GetComponent<CAnimator>();
	SAFE_RELEASE(pPlayerCore);
	if (pAnimator)
	{
		CAnimationStateController* pController = pAnimator->GetAnimationController();
		if (pController)
		{
			// 기본움직임 관련
			switch (m_eStandardState)
			{ // switch문의 여는 괄호입니다
			case PLAYER_STANDARD_NONE:
			{
				pController->SetInteger("Standard", -1);
			}
			break;
			case PLAYER_IDLE1:
			{
				pController->SetInteger("Standard", 0);
			}
			break;
			case PLAYER_IDLE2:
			{
				pController->SetInteger("Standard", 1);
			}
			break;
			case PLAYER_ROT_R:
			{
				//pController->SetInteger("Standard", 2);
			}
			break;
			case PLAYER_ROT_L:
			{
				//pController->SetInteger("Standard", 3);
			}
			break;
			case PLAYER_WALK_F:
			{
				pController->SetInteger("Standard", 4);
			}
			break;
			case PLAYER_WALK_B:
			{
				pController->SetInteger("Standard", 5);
			}
			break;
			case PLAYER_RUN_F:
			{
				pController->SetInteger("Standard", 6);
			}
			break;
			case PLAYER_RUN_B:
			{
				pController->SetInteger("Standard", 7);
			}
			break;
			case PLAYER_EMOTION_1:
			{
				pController->SetInteger("Standard", 100);
			}
			break;
			case PLAYER_EMOTION_2:
			{
				pController->SetInteger("Standard", 101);
			}
			break;
			case PLAYER_EMOTION_3:
			{
				pController->SetInteger("Standard", 102);
			}
			break;
			case PLAYER_EMOTION_4:
			{
				pController->SetInteger("Standard", 103);
			}
			break;
			case PLAYER_EMOTION_5:
			{
				pController->SetInteger("Standard", 104);
			}
			break;
			case PLAYER_EMOTION_6:
			{
				pController->SetInteger("Standard", 105);
			}
			break;
			case PLAYER_EMOTION_7:
			{
				pController->SetInteger("Standard", 106);
			}
			break;
			case PLAYER_EMOTION_8:
			{
				pController->SetInteger("Standard", 107);
			}
			break;
			case PLAYER_EMOTION_9:
			{
				pController->SetInteger("Standard", 108);
			}
			break;
			case PLAYER_EMOTION_10:
			{
				pController->SetInteger("Standard", 109);
			}
			break;
			case PLAYER_EMOTION_11:
			{
				pController->SetInteger("Standard", 110);
			}
			break;
			case PLAYER_EMOTION_12:
			{
				pController->SetInteger("Standard", 111);
			}
			break;
			case PLAYER_EMOTION_13:
			{
				pController->SetInteger("Standard", 112);
			}
			break;
			case PLAYER_EMOTION_14:
			{
				pController->SetInteger("Standard", 113);
			}
			break;
			case PLAYER_EMOTION_15:
			{
				pController->SetInteger("Standard", 114);
			}
			break;
			case PLAYER_EMOTION_16:
			{
				pController->SetInteger("Standard", 115);
			}
			break;
			case PLAYER_EMOTION_17:
			{
				pController->SetInteger("Standard", 116);
			}
			break;
			case PLAYER_EMOTION_18:
			{
				pController->SetInteger("Standard", 117);
			}
			break;
			case PLAYER_EMOTION_19:
			{
				pController->SetInteger("Standard", 118);
			}
			break;
			case PLAYER_EMOTION_20:
			{
				pController->SetInteger("Standard", 119);
			}
			case PLAYER_EMOTION_21:
			{
				pController->SetInteger("Standard", 120);
			}
			break;
			case PLAYER_DODGE:
			{
				pController->SetInteger("Standard", 10);
			}
			break;
			case PLAYER_DODGE_GETUP:
			{
				pController->SetInteger("Standard", 11);
			}
			break;

			}// switch문의 닫는괄호입니다

			 // 전투관련
			switch (m_eBattleState)
			{ // switch문의 여는 괄호입니다
			case PLAYER_BATTLE_NONE:
			{
				pController->SetInteger("Battle", 0);
			}
			break;
			case PLAYER_ATTACK_A1:
			{
				pController->SetInteger("Battle", 1);
			}
			break;
			case PLAYER_ATTACK_A2:
			{
				pController->SetInteger("Battle", 2);
			}
			break;
			case PLAYER_ATTACK_A3:
			{
				pController->SetInteger("Battle", 3);
			}
			break;
			case PLAYER_SMASH_A1:
			{
				pController->SetInteger("Battle", 4);
			}
			break;
			case PLAYER_ATTACK_B1:
			{
				pController->SetInteger("Battle", 5);
			}
			break;
			case PLAYER_SMASH_B2:
			{
				pController->SetInteger("Battle", 6);
			}
			break;
			case PLAYER_SKILL_A1:
			{
				pController->SetInteger("Battle", 7);
			}
			break;
			case PLAYER_SKILL_A2:
			{
				pController->SetInteger("Battle", 8);
			}
			break;
			case PLAYER_SMASH_A2:
			{
				pController->SetInteger("Battle", 9);
			}
			break;
			case PLAYER_SKILL_B1:
			{
				pController->SetInteger("Battle", 10);
			}
			break;
			case PLAYER_SKILL_B2:
			{
				pController->SetInteger("Battle", 11);
			}
			break;
			case PLAYER_SKILL_B3:
			{
				pController->SetInteger("Battle", 12);
			}
			break;

			}// switch문의 닫는괄호입니다

			switch (m_eDMGState)
			{// switch문의 여는괄호입니다
			case PLAYER_DAMAGED_NONE:
			{
				pController->SetInteger("Damaged", -1);
			}
			break;
			case PLAYER_DMG_FRONT:
			{
				pController->SetInteger("Damaged", 0);
			}
			break;
			case PLAYER_DMG_BACK:
			{
				pController->SetInteger("Damaged", 1);
			}
			break;
			case PLAYER_DOWN:
			{
				pController->SetInteger("Damaged", 2);
			}
			break;
			case PLAYER_DOWN_LOOP:
			{
				pController->SetInteger("Damaged", 3);
			}
			break;
			case PLAYER_DEADBODY:
			{
				pController->SetInteger("Damaged", 4);
			}
			break;
			case PLAYER_RECOVERY:
			{
				pController->SetInteger("Damaged", 5);
			}
			break;
			case PLAYER_DIE:
			{
				pController->SetInteger("Damaged", 6);
			}
			break;
			case PLAYER_PASSOUT:
			{
				pController->SetInteger("Damaged", 7);
			}
			break;
			}// switch문의 닫는괄호입니다
		}

	}
	SAFE_RELEASE(pAnimator);

	if (m_eStandardState == PLAYER_IDLE1
		|| m_eStandardState == PLAYER_IDLE2
		|| m_eStandardState == PLAYER_ROT_R
		|| m_eStandardState == PLAYER_ROT_L

		|| m_eStandardState == PLAYER_EMOTION_1
		|| m_eStandardState == PLAYER_EMOTION_2
		|| m_eStandardState == PLAYER_EMOTION_3
		|| m_eStandardState == PLAYER_EMOTION_4
		|| m_eStandardState == PLAYER_EMOTION_5
		|| m_eStandardState == PLAYER_EMOTION_6
		|| m_eStandardState == PLAYER_EMOTION_6_LOOP
		|| m_eStandardState == PLAYER_EMOTION_7
		|| m_eStandardState == PLAYER_EMOTION_8
		|| m_eStandardState == PLAYER_EMOTION_9
		|| m_eStandardState == PLAYER_EMOTION_10
		|| m_eStandardState == PLAYER_EMOTION_11
		|| m_eStandardState == PLAYER_EMOTION_12
		|| m_eStandardState == PLAYER_EMOTION_13
		|| m_eStandardState == PLAYER_EMOTION_14
		|| m_eStandardState == PLAYER_EMOTION_15
		|| m_eStandardState == PLAYER_EMOTION_16
		|| m_eStandardState == PLAYER_EMOTION_17
		|| m_eStandardState == PLAYER_EMOTION_18
		|| m_eStandardState == PLAYER_EMOTION_19
		|| m_eStandardState == PLAYER_EMOTION_20
		|| m_eStandardState == PLAYER_EMOTION_21
		)
		m_bOnIdle = true;
	else
		m_bOnIdle = false;


	if (m_eBattleState == PLAYER_BATTLE_NONE)
	{
		for (int i = 0; i < 4; ++i)
		{
			CMeshRenderer* pRend = m_pAttackTrail[i]->GetComponent<CMeshRenderer>();
			pRend->Enable(false);
			SAFE_RELEASE(pRend);
			CTrail* pTrail = m_pAttackTrail[i]->GetComponent<CTrail>();
			pTrail->SetPositionInAllList(m_pTransform->GetWorldPosition());
			SAFE_RELEASE(pTrail);
			m_iAttackPhase = 0;
		}

		m_iAttackPhase = 0;
	}
	else
	{
		for (int i = 0; i < 4; ++i)
		{
			CMeshRenderer* pRend = m_pAttackTrail[i]->GetComponent<CMeshRenderer>();
			pRend->Enable(true);
			SAFE_RELEASE(pRend);
		}
	}

	TransState(fTime);

	if (m_eStandardState == PLAYER_WALK_F
		|| m_eStandardState == PLAYER_RUN_F
		|| m_eStandardState == PLAYER_WALK_B
		|| m_eStandardState == PLAYER_RUN_B)
	{
		vLayPosition.y += 0.5f;
		tRay.vOrigin = vLayPosition;
		if (m_eStandardState == PLAYER_WALK_F
			|| m_eStandardState == PLAYER_RUN_F)
			tRay.vDirection = m_pTransform->forward();
		if (m_eStandardState == PLAYER_WALK_B
			|| m_eStandardState == PLAYER_RUN_B)
			tRay.vDirection = m_pTransform->back();

		tRay.fDistance = 2.0f;
		tHit = GET_SINGLE(CCollisionManager)->LineCast(tRay);

		for (int i = 0; i < tHit.iCount; ++i)
		{
			if (tHit.vecHits[i].bResult)
			{
				if (tHit.vecHits[i].pCollider->IsComponent<CMeshCollider>())
				{
					CRigidbody* pRigidbody = tHit.vecHits[i].pCollider->GetComponent<CRigidbody>();
					if (pRigidbody == NULL)
					{
						
						Vector3 vFinalPosition = m_pTransform->GetWorldPosition();
						vFinalPosition.x = tHit.vecHits[i].vPoint.x;
						vFinalPosition.z = tHit.vecHits[i].vPoint.z;
						vFinalPosition -= tRay.vDirection * tHit.vecHits[i].fDistamce;
						m_pTransform->SetWorldPosition(vFinalPosition);
					}
					SAFE_RELEASE(pRigidbody);
				}

				if (tHit.vecHits[i].pCollider->IsComponent<CBoxCollider>())
				{
					Vector3 vFinalPosition = m_pTransform->GetWorldPosition();
					vFinalPosition.x = tHit.vecHits[i].vPoint.x;
					vFinalPosition.z = tHit.vecHits[i].vPoint.z;
					vFinalPosition -= tRay.vDirection * tHit.vecHits[i].fDistamce;
					m_pTransform->SetWorldPosition(vFinalPosition);
				}
			}
		}
	}
	
	return 0;
}

int CPlayerController::OnCollisionEnter(CCollider * pThis, CCollider * pTarget, float fTime)
{
	CAttack* pAttack = pTarget->GetComponent<CAttack>();
	if (pAttack)
	{
		// -1 값을 가진 충돌체만 플레이어를 공격할수 있따..
		if (pAttack->GetValue() != -1)
		{
			SAFE_RELEASE(pAttack);
			return 0;
		}

		if (m_eDMGState == PLAYER_DAMAGED_NONE)
		{
			CGameObject* pPlayerCore = m_pTransform->GetChild(0)->GetGameObject();
			CAnimator* pAnimator = pPlayerCore->GetComponent<CAnimator>();
			SAFE_RELEASE(pPlayerCore);
			if (pAnimator)
			{
				CAnimationStateController* pController = pAnimator->GetAnimationController();
				if (pController)
				{
					pController->SetInteger("Standard", -1);
					pController->SetInteger("Battle", -1);

					switch (pAttack->GetAttribute())
					{
					case ATTACK_NORMAL:
					{
						pController->SetInteger("Damaged", 1);
						m_eDMGState = PLAYER_DMG_BACK;
						GET_SINGLE(CSoundManager)->Play("Damage_Voice");
					}
					break;
					case ATTACK_DOWN:
					{
						pController->SetInteger("Damaged", 2);
						m_eDMGState = PLAYER_DOWN;
						GET_SINGLE(CSoundManager)->Play("Down_Voice");
					}
					break;
					case ATTACK_STUN:
					{
						// 
					}
					break;
					case ATTACK_FREEZE:
					{
						// 추웡
					}
					break;
					}
				}
				m_fActiveTime = 0.f;
				m_eStandardState = PLAYER_STANDARD_NONE;
				m_eBattleState = PLAYER_BATTLE_NONE;
				SAFE_RELEASE(pAnimator);
			}
		}

		float fDamage = pAttack->GetDamagePoint();
		m_fHP -= fDamage;

		SAFE_RELEASE(pAttack);
		// 한번 맞았으면 타겟충돌체를 지워주도록 합니다..
		//pTarget->Destroy(true);
	}

	return 0;
}

int CPlayerController::OnCollisionExit(CCollider * pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

bool CPlayerController::Save(FILE* pFile)
{
	return true;
}

bool CPlayerController::Load(FILE* pFile)
{
	return true;
}