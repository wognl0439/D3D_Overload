#include "EffectController.h"
#include "Engine.h"
#include "Transform.h"
#include "GameObject.h"
#include "Mathf.h"
#include "Component/MeshRenderer.h"
#include "Material.h"
#include "Component/SphereCollider.h"
#include "Component/RigidBody.h"
#include "SoundManager.h"
#include "Component/Effect.h"
#include "Component/MeshCollider.h"
#include "Component/Effect.h"
#include "Component/BoxCollider.h"
#include "Component/Billboard.h"
#include "Component/Distortion.h"
#include "Layer.h"
#include "Component/PointLight.h"

#include "PlayerCamera.h"
#include "Component/Camera.h"
#include "Input.h"
#include "Scene.h"
#include "Device.h"
#include "Component/Decal.h"
#include "Attack.h"
#include "DataManager.h"

CEffectController::CEffectController() :
	m_fLifeTime(0.0f)
	, m_bUseScaling(false)
	, m_fScalingTimeThreshold(0.0f)
	, m_bRotate(false)
	, m_fRotatePerSecond(0.0f)
	, m_vMinSize(Vector3::One)
	, m_vMaxSize(Vector3::One)
	, m_fProgress(0.0f)
	, m_fTimer(0.0f)
	, m_bUseAlpha(false)
	, m_fAlphaTime(0.f)
	, m_fMinAlpha(0.f)
	, m_fMaxAlpha(0.f)
	, m_bBlink(false)
	, m_fAlphaBlink(0.f)
	, m_fAcceleration(3.2f)
	, m_bMove(false)
	, m_vMoveDir(Vector3::One)
	, m_bUseArrive(false)
	, m_vArrivePosition(Vector3::One)
	, m_bMeteo(false)
	, m_bReScale(false)
	, m_bUseLimLight(false)
	, m_vLimLightColor(Vector3::One)
	, m_fLimLightTime(0.f)
	, m_bUseDestroy(true)
	, m_bUseBurnEffect(false)
	, m_bDestroy(true)
	, m_fBurnTime(0.f)
	, m_bUseLight(false)
	, m_bUseSkill(false)
	, m_bUseMeteoFire(false)
	, m_bUseNoramlScale(false)
	, m_fNormalScalingTimeThreshold(0.f)
{
}

CEffectController::~CEffectController()
{
}

bool CEffectController::Initialize()
{
	return true;
}

int CEffectController::Update(float fTime)
{
	m_fTimer += fTime;
	m_fProgress = m_fTimer / m_fLifeTime;

	if (m_bUseScaling)
	{
		CTransform*	pTransform = m_pGameObject->GetTransform();
		float fScaleFactor = m_fProgress + ((m_fLifeTime - m_fScalingTimeThreshold) / m_fLifeTime);
		if (fScaleFactor <= 1.0f)
		{
			Vector3 vScale = fScaleFactor * (m_vMaxSize - m_vMinSize) + m_vMinSize;
			pTransform->SetWorldScale(vScale);
		}
		else
		{
			if (m_bReScale)
				fScaleFactor = 0;
			else
				m_bUseScaling = false;
		}
		SAFE_RELEASE(pTransform);
	}

	if (m_bUseNoramlScale)
	{
		CMeshRenderer* pRenderer = m_pGameObject->GetComponent<CMeshRenderer>();
		if (pRenderer)
		{
			CMaterial*	pMaterial = pRenderer->GetMaterial();
			if(pMaterial)
			{
				float fScaleFactor = m_fProgress + ((m_fLifeTime - m_fNormalScalingTimeThreshold) / m_fLifeTime);
				if (fScaleFactor <= 1.0f)
				{
					float fScale = fScaleFactor * (m_fMaxNormalScale - m_fMinNormalScale) + m_fMinNormalScale;
					pMaterial->SetNormalScale(fScale);
				}
				SAFE_RELEASE(pMaterial);
			}
			SAFE_RELEASE(pRenderer);
		}
	}

	if (m_bRotate)
	{
		float fLimitAngle = m_fRotatePerSecond * 2.0f * PI * m_fLifeTime;
		float fAngle = CMathf::Lerp(0.0f, fLimitAngle, m_fProgress);

		CTransform*	pTransform = m_pGameObject->GetTransform();
		Vector3	vRotate(0.0f, 0.0f, fAngle);
		vRotate = vRotate.TransformNormal(pTransform->GetWorldMatrix());
		vRotate.x = PI / 2.0f;
		pTransform->SetWorldRotation(vRotate);
		SAFE_RELEASE(pTransform);
	}

	if (m_bUseAlpha)
	{
		m_fAlphaBlink += fTime;

		CMeshRenderer*	pRenderer = m_pGameObject->GetComponent<CMeshRenderer>();
		CMaterial*	pMaterial = pRenderer->GetMaterial();
		Vector4	vDiffuseColor = pMaterial->GetDiffuseColor();

		if (m_bBlink)
		{			
			float fBlinkAlpha = (sinf(PI*(m_fAlphaBlink - 0.5f)) + 1) * 0.5f;

			vDiffuseColor.w = fBlinkAlpha;
			pMaterial->SetDiffuseColor(vDiffuseColor);
		}
		else
		{
			float fAlpha = CMathf::Lerp(0.0f, m_fMaxAlpha, m_fProgress);
			vDiffuseColor.w = fAlpha;
			pMaterial->SetDiffuseColor(vDiffuseColor);
		}

		if (m_fAlphaBlink >= m_fAlphaTime)
			m_bUseAlpha = false;
		SAFE_RELEASE(pMaterial);
		SAFE_RELEASE(pRenderer);
	}

	if (m_bMove)
	{
		m_fAcceleration += fTime;
		//m_vMoveDir += m_fAcceleration;

		Vector3 vPosition = m_pTransform->GetWorldPosition();
		vPosition += m_vMoveDir * m_fAcceleration;
		m_pTransform->SetWorldPosition(vPosition);

		if (m_bUseArrive)
			CheckArrive();
	}

	if (m_bUseMeteoFire)
	{
		if (m_bUseArrive)
			CheckArrive();
	}

	if (m_bUseLimLight)
	{
		m_fLimLightTime -= fTime;
		if (m_fLimLightTime <= 0.f)
		{
			CEffect*	pEffect = m_pGameObject->GetComponent<CEffect>();
			pEffect->SetLimColor(Vector3(1.f, 1.f, 1.f));
			pEffect->SetLimPower(3.f);
			SAFE_RELEASE(pEffect);

			//Destroy(true);
		}
	}

	if (m_bUseLight)
	{
		if (m_pGameObject->HasComponent<CPointLight>())
		{
			CPointLight*	pPointLight = m_pGameObject->GetComponent<CPointLight>();
			float fLightReduction = pPointLight->GetRange() * fTime * m_fLifeTime;
			float fRange = pPointLight->GetRange() - fLightReduction;
			pPointLight->SetRange(fRange);
			SAFE_RELEASE(pPointLight)
		}
	}

	if (m_bUseSkill)
	{
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
			if (m_pGameObject)
			{
				CTransform*	pSkillTransform = m_pGameObject->GetTransform();
				pSkillTransform->SetWorldPosition(tHit.vPoint);
				SAFE_RELEASE(pSkillTransform);
			}
		}
		SAFE_RELEASE(pCamera);
		SAFE_RELEASE(pCameraTransform);
	}

	if (m_fProgress >= 1.0f && m_bDestroy)
	{
		if (m_bUseBurnEffect)
		{
			m_bUseBurnEffect = false;
			m_fProgress = 0;
			m_fTimer = 0;
			m_fLifeTime = m_fBurnTime - 0.1f;
			CEffect*	pBurnEffect = m_pGameObject->GetComponent<CEffect>();
			if (pBurnEffect)
			{
				pBurnEffect->SetFlags(VF_BURN);
				pBurnEffect->SetLimitTime(m_fBurnTime);
				SAFE_RELEASE(pBurnEffect);
			}
			else
			{
				CEffect*	pBurnEffect = m_pGameObject->AddComponent<CEffect>();
				pBurnEffect->SetFlags(VF_BURN);
				pBurnEffect->SetLimitTime(m_fBurnTime);
				SAFE_RELEASE(pBurnEffect);
			}
		}

		else if (m_bUseDestroy && !m_bUseBurnEffect)
		{
			m_pGameObject->Destroy(true);
		}		
	}

	return 0;
}

void CEffectController::SetScaleInfo(float fScalingTime, Vector3 vMinSize, Vector3 vMaxSize, bool bStay, bool bReScale)
{
	m_bUseScaling = true;
	m_bReScale = bReScale;
	m_fScalingTimeThreshold = fScalingTime;
	m_vMinSize = vMinSize;
	m_vMaxSize = vMaxSize;
}

void CEffectController::SetAlphaInfo(float fAlphaTime, float fMinAlpha, float fMaxAlpha, bool bBlink)
{
	m_bUseAlpha = true;
	m_fAlphaTime = fAlphaTime;
	m_fMinAlpha = fMinAlpha;
	m_fMaxAlpha = fMaxAlpha;
	m_bBlink = bBlink;
}

void CEffectController::SetRotateInfo(float fRotatePerSecond)
{
	m_bRotate = true;
	m_fRotatePerSecond = fRotatePerSecond;
}

void CEffectController::SetMoveInfo(Vector3 vStartPosition, Vector3 vEndPosition, float fDefaultSpeed, bool bUseArrive)
{
	m_bMove = true;
	m_bUseArrive = bUseArrive;
	m_vMoveDir = (vEndPosition - vStartPosition).Normalize();
	m_vMoveDir *= fDefaultSpeed;

	m_pTransform->SetWorldPosition(vStartPosition);

	if (bUseArrive)
		m_vArrivePosition = vEndPosition;
}


void CEffectController::CheckArrive()
{
	Vector3 vObjectPosition = m_pTransform->GetWorldPosition();

	if (m_bUseMeteoFire)
	{
		if (vObjectPosition.y <= m_vArrivePosition.y + 20.f)
		{
			m_bMove = false;
			m_bUseArrive = false;
			m_bUseMeteoFire = false;

			SetLifeTime(6.f);
			SetScaleInfo(6.f, Vector3(100.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), true);
		}
	}

	else if (m_bMeteo)
	{
		if (vObjectPosition.y <= m_vArrivePosition.y + 5.f)
		{
			m_bMove = false;
			m_bUseArrive = false;
			m_bMeteo = false;

			CLayer* pLayer = m_pGameObject->GetLayer();


			//	Shake Camera
			{
				CGameObject*	pPlayerCameraGimbal = GET_SINGLE(CDataManager)->GetCameraGimbal();
				if (pPlayerCameraGimbal)
				{
					CPlayerCamera*	pPlayerCamera = pPlayerCameraGimbal->GetComponent<CPlayerCamera>();
					if (pPlayerCamera)
					{
						pPlayerCamera->StartShake(2.0f, 15.f, 3.0f);
						SAFE_RELEASE(pPlayerCamera);
					}
				}
			}

			// Damage Collider
			{
				CGameObject* pAttackObj = CGameObject::CreateObject("Attack", m_pLayer);
				CSphereCollider* pCollider = pAttackObj->AddComponent<CSphereCollider>();
				pCollider->SetRadius(50.f);
				SAFE_RELEASE(pCollider);
				CAttack* pAttack = pAttackObj->AddComponent<CAttack>();
				pAttack->SetValue(1);
				pAttack->SetAttribute(ATTACK_DOWN);
				pAttack->SetDamagePoint(200.f);
				pAttack->SetActiveMaxTime(0.1f);
				SAFE_RELEASE(pAttack);
				CTransform* pTr = pAttackObj->GetTransform();
				pTr->SetWorldPosition(m_vArrivePosition);
				SAFE_RELEASE(pTr);
				SAFE_RELEASE(pAttackObj);
			}

			// Effect Distortion3
			//{
			//	CGameObject* pSpriteObj = CGameObject::CreateObject("MeteoEndDistortion3", pLayer);
			//	CTransform*	pSpriteTr = pSpriteObj->GetTransform();
			//
			//	pSpriteTr->SetPivot(0.1f, 0.1f, 0.1f);
			//	pSpriteTr->SetWorldScale(0.f, 0.f, 1.f);
			//
			//	Vector3	vObjTr = m_pTransform->GetWorldPosition();
			//	pSpriteTr->SetWorldPosition(vObjTr.x, vObjTr.y + 2.5f, vObjTr.z);
			//
			//	CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("MeteoEndDistortion3");
			//	pMeshRenderer->SetMesh(PRIMITIVE_MESH_SPHERE, true);
			//	pMeshRenderer->SetRenderState(ALPHA_BLEND);
			//	pMeshRenderer->SetRenderState(CULLNONE);
			//	pMeshRenderer->SetShader("DistortionAnimShader");
			//	pMeshRenderer->SetInputLayout(BUMP_ANIM_LAYOUT);
			//
			//	float fMaxNormalScale;
			//	CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
			//	pMaterial->SetDiffuseTexture("distortion2.png", L"Effects/Distortion/distortion2.png");
			//	pMaterial->SetAlphaColor(Vector4(1.f, 1.f, 1.f, 1.0f));
			//	pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			//	pMaterial->SetEmissiveColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			//	fMaxNormalScale = pMaterial->GetNormalScale();
			//	SAFE_RELEASE(pMaterial);
			//
			//	SAFE_RELEASE(pMeshRenderer);
			//
			//	CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_MeteoEndDistortion_Ctrl3");
			//
			//	pEffectController->SetLifeTime(3.f);
			//	pEffectController->SetScaleInfo(3.f, Vector3(10.f, 10.f, 10.f), Vector3(1000.f, 1000.f, 1000.f), true);
			//	pEffectController->SetRotateInfo(100.f);
			//	pEffectController->SetNormalScaleInfo(2.9f, fMaxNormalScale, 0.f);
			//
			//	SAFE_RELEASE(pEffectController);
			//
			//	CDistortion* pDistortion = pSpriteObj->AddComponent<CDistortion>("Effect_MeteoEndDistortion3");
			//	SAFE_RELEASE(pDistortion);
			//
			//	SAFE_RELEASE(pSpriteTr);
			//	SAFE_RELEASE(pSpriteObj);
			//}


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
			
				pEffectController->SetLifeTime(2.f);
				pEffectController->SetScaleInfo(2.f, Vector3(-50.f, -50.f, -50.f), Vector3(200.f, 200.f, 200.f), true);
				pEffectController->SetNormalScaleInfo(1.9f, fMaxNormalScale, 0.f);

				SAFE_RELEASE(pEffectController);

				CDistortion* pDistortion = pSpriteObj->AddComponent<CDistortion>("Effect_MeteoEndDistortion3");
				SAFE_RELEASE(pDistortion);
			
				SAFE_RELEASE(pSpriteTr);
				SAFE_RELEASE(pSpriteObj);
			}



			// Meteo Fall Position Decal
			{
				CGameObject* pSpriteObj = CGameObject::CreateObject("Effect_Fall_Position", pLayer);
				pSpriteObj->SetTransparency(true);
				CTransform*	pSpriteTr = pSpriteObj->GetTransform();

				pSpriteTr->SetPivot(0.5f, 0.5f, 0.5f);
				pSpriteTr->SetWorldScale(50.f, 20.f, 50.f);
				pSpriteTr->SetWorldPosition(m_vArrivePosition);

				CMeshRenderer*	pMeshRenderer = pSpriteObj->AddComponent<CMeshRenderer>("Effect_Fall_Position");
				pMeshRenderer->SetMesh(ENGINE_MESH_RECTANGLE_TEX, true);
				pMeshRenderer->SetRenderState(ALPHA_BLEND);
				pMeshRenderer->SetRenderState(CULLNONE);

				CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
				pMaterial->SetDiffuseTexture("FX_Crack_006_TEX_HKB", L"Decals/FX_Crack_006_TEX_HKB.tga");
				pMaterial->SetNormalTexture("FX_CrackNormal_007_TEX_HKB", L"Decals/FX_CrackNormal_007_TEX_HKB.tga");	
				//pMaterial->SetAlphaTexture("FX_Crater_007_Meteo", L"Decals/FX_Crater_007_Meteo.tga");

				//pMaterial->SetDiffuseColor(Vector4(1.0f, 0.20f, 0.20f, 1.0f));
				pMaterial->SetEmissiveColor(Vector4(1.0f, 0.20f, 0.20f, 1.0f));
				SAFE_RELEASE(pMaterial);

				SAFE_RELEASE(pMeshRenderer);

				CEffectController*	pEffectController = pSpriteObj->AddComponent<CEffectController>("Effect_Fall_Position_Ctrl");

				pEffectController->SetLifeTime(5.0f);
				//pEffectController->SetScaleInfo(2.0f, Vector3(0.f, 0.f, 0.f), Vector3(30.f, 30.f, 30.f), true, true);
				//pEffectController->SetAlphaInfo(5.0f, 0.f, 1.0f);
				//pEffectController->SetRotateInfo(1.f);

				SAFE_RELEASE(pEffectController);

				CDecal*	pDecal = pSpriteObj->AddComponent<CDecal>();
				SAFE_RELEASE(pDecal);

				SAFE_RELEASE(pSpriteTr);
				SAFE_RELEASE(pSpriteObj);
			}

			SAFE_RELEASE(pLayer);

			CEffectController*	pMeteoEffectCtrl = m_pGameObject->GetComponent<CEffectController>();
			pMeteoEffectCtrl->SetLifeTime(8.f);
			pMeteoEffectCtrl->SetLightRedution();
			SAFE_RELEASE(pMeteoEffectCtrl);

			// Meteo Child Control
			int iChildCount = m_pTransform->GetChildrenCount();
			if (iChildCount > 0)
			{
				vector<CTransform*>*	pMeteoChildren = m_pTransform->GetChildren();
				vector<CTransform*>::iterator	iter;
				vector<CTransform*>::iterator	iterEnd = pMeteoChildren->end();
			
				for (iter = pMeteoChildren->begin(); iter != iterEnd; ++iter)
				{
					CGameObject*	pMeteoChildObject = (*iter)->GetGameObject();
								
					if (pMeteoChildObject->HasComponent<CRigidbody>())
					{
						CRigidbody*	pChildRigidbody = pMeteoChildObject->GetComponent<CRigidbody>();
						pChildRigidbody->SetKinematic(false);
						pChildRigidbody->SetUseGravity(true);
						SAFE_RELEASE(pChildRigidbody)
					}
					else
					{
						CRigidbody*	pChildRigidbody = pMeteoChildObject->AddComponent<CRigidbody>();
						pChildRigidbody->SetKinematic(false);
						pChildRigidbody->SetUseGravity(true);
						SAFE_RELEASE(pChildRigidbody)
					}

					if (pMeteoChildObject->HasComponent<CEffectController>())
					{
						CEffectController*	pMeteoEffectCtrl = pMeteoChildObject->GetComponent<CEffectController>();
						pMeteoEffectCtrl->SetLifeTime(4.f);
						pMeteoEffectCtrl->UseBurnEffect(4.0f);
						SAFE_RELEASE(pMeteoEffectCtrl);
					}
					else
					{
						CEffectController*	pMeteoEffectCtrl = pMeteoChildObject->AddComponent<CEffectController>();
						pMeteoEffectCtrl->SetLifeTime(4.f);
						pMeteoEffectCtrl->UseBurnEffect(4.0f);
						SAFE_RELEASE(pMeteoEffectCtrl);
					}
			
					SAFE_RELEASE(pMeteoChildObject);
				}
			}

			GET_SINGLE(CSoundManager)->Stop(SOUND_SFX);
			GET_SINGLE(CSoundManager)->Play("MeteoStone");
			GET_SINGLE(CSoundManager)->Play("MeteoBomb");
		}
	}
}

void CEffectController::SetUseMeteo()
{
	m_bMeteo = true;
}

void CEffectController::SetLimLightInfo(float fLimLightTime, Vector3 vLimLightColor, float fLimLightPower, bool bUseDestroy)
{
	m_bUseLimLight = true;
	m_fLimLightTime = fLimLightTime;
	m_bUseDestroy = bUseDestroy;

	if (!m_pGameObject->HasComponent<CEffect>())
	{
		CEffect*	pEffect = m_pGameObject->AddComponent<CEffect>();
		pEffect->SetLimColor(vLimLightColor);
		pEffect->SetLimPower(fLimLightPower);
		SAFE_RELEASE(pEffect);
	}
	else
	{
		CEffect*	pEffect = m_pGameObject->GetComponent<CEffect>();
		pEffect->SetLimColor(vLimLightColor);
		pEffect->SetLimPower(fLimLightPower);
		SAFE_RELEASE(pEffect);
	}
}

void CEffectController::SetLifeTime(float fLifeTime, bool bDestroy)
{
	m_fLifeTime = fLifeTime;
	m_fTimer = 0.f;
	m_fProgress = 0.f;
	m_bDestroy = bDestroy;
}

void CEffectController::UseBurnEffect(float fBurnTime)
{
	m_bUseBurnEffect = true;
	m_fBurnTime = fBurnTime;
}

void CEffectController::SetSkillCircle(bool bUseSkill)
{
	m_bUseSkill = bUseSkill;
}

void CEffectController::SetLightRedution()
{
	m_bUseLight = true;
}

void CEffectController::UseDamageEffect()
{
	CTransform*	pEffectTransform = m_pGameObject->GetTransform();
	Vector3 vCurrentPosition = pEffectTransform->GetWorldPosition();
	//Vector3 vRandomDirection = Vector3::RandomDirection();
	//float fRandomRange = CMathf::RandomRange(0.9f, 1.1f);
	
	CTransform*	pTransform = m_pGameObject->GetTransform();
	Vector3 vRotation = pTransform->GetWorldRotation();
	vRotation.z = RandomRange(-PI, PI);
	pTransform->SetWorldRotation(vRotation);
	SAFE_RELEASE(pTransform);

	SAFE_RELEASE(pEffectTransform);
}

void CEffectController::UseMeteoFire(Vector3 vArrivePosition, bool bUseArrive)
{
	m_bUseMeteoFire = true;
	m_bUseArrive = bUseArrive;
	m_vArrivePosition = vArrivePosition;
}

void CEffectController::SetNormalScaleInfo(float fScalingTime, float fMinScale, float fMaxScale)
{
	m_bUseNoramlScale = true;
	m_fNormalScalingTimeThreshold = fScalingTime;
	m_fMinNormalScale = fMinScale;
	m_fMaxNormalScale = fMaxScale;
}

bool CEffectController::Save(FILE * pFile)
{
	fwrite(&m_fLifeTime, sizeof(float), 1, pFile);
	fwrite(&m_bUseScaling, sizeof(bool), 1, pFile);
	fwrite(&m_fScalingTimeThreshold, sizeof(float), 1, pFile);
	fwrite(&m_bRotate, sizeof(bool), 1, pFile);
	fwrite(&m_fRotatePerSecond, sizeof(float), 1, pFile);
	fwrite(&m_vMinSize, sizeof(Vector3), 1, pFile);
	fwrite(&m_vMaxSize, sizeof(Vector3), 1, pFile);

	return true;
}

bool CEffectController::Load(FILE * pFile)
{
	fread(&m_fLifeTime, sizeof(float), 1, pFile);
	fread(&m_bUseScaling, sizeof(bool), 1, pFile);
	fread(&m_fScalingTimeThreshold, sizeof(float), 1, pFile);
	fread(&m_bRotate, sizeof(bool), 1, pFile);
	fread(&m_fRotatePerSecond, sizeof(float), 1, pFile);
	fread(&m_vMinSize, sizeof(Vector3), 1, pFile);
	fread(&m_vMaxSize, sizeof(Vector3), 1, pFile);

	return true;
}
