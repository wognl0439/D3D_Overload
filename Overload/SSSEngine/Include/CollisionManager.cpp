#include "CollisionManager.h"
#include "Core.h"
#include "Layer.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Component\UICollider.h"
#include "Collider.h"
#include "GameObject.h"
#include "Input.h"
#include "Component\Camera.h"

#include "Device.h"

#include "Transform.h"
#include "Debug.h"

#define MAX_NUM_ACTOR_SHAPES 128
SSS_USING

DEFINITION_SINGLE(CCollisionManager)

CCollisionManager::CCollisionManager() :
	m_pCoveredButtonCollider(NULL),
	m_pCoveredCollider(NULL),
	m_fFixedTimeStep(1.0f / 60.0f),
	m_vGravity(0.0f, -42.1f, 0.0f)
{
	m_pFoundation = NULL;
	m_pPhysics = NULL;
	m_pPVD = NULL;
	m_pCooking = NULL;
}


CCollisionManager::~CCollisionManager()
{
	if (m_pCooking)
		m_pCooking->release();


	size_t iRemainCount = m_pPhysics->getNbConvexMeshes();
	iRemainCount = m_pPhysics->getNbConvexMeshes();
	iRemainCount = m_pPhysics->getNbHeightFields();
	iRemainCount = m_pPhysics->getNbMaterials();
	iRemainCount = m_pPhysics->getNbScenes();
	iRemainCount = m_pPhysics->getNbShapes();
	iRemainCount = m_pPhysics->getNbTriangleMeshes();
	iRemainCount = m_pPhysics->getNbClothFabrics();

	if (m_pPhysics)
		m_pPhysics->release();

	if (m_pPVD)
	{
		if (m_pPVD->isConnected())
		{
			PxPvdTransport* transport = m_pPVD->getTransport();
			m_pPVD->release();
			transport->release();
		}
		else
		{
			m_pPVD->release();
		}
	}



	if (m_pFoundation)
		m_pFoundation->release();

}


void CCollisionManager::onConstraintBreak(PxConstraintInfo *, PxU32)
{
}

void CCollisionManager::onWake(PxActor **, PxU32)
{
}

void CCollisionManager::onSleep(PxActor **, PxU32)
{
}

void CCollisionManager::onContact(const PxContactPairHeader & pairHeader, const PxContactPair * pairs, PxU32 nbPairs)
{
	//모든 충돌 쌍을 돌면서 이벤트 날려줌
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		// Enter => PxPairFlag::eNOTIFY_TOUCH_FOUND
		// Exit => PxPairFlag::eNOTIFY_TOUCH_LOST
		// Stay => PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		const PxContactPair& cp = pairs[i];
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			CCollider* pColliderA = GetCollider(pairHeader.actors[0]);
			CCollider* pColliderB = GetCollider(pairHeader.actors[1]);

			if (pColliderA->IsEnable() && pColliderB->IsEnable())
			{
				CScene* pScene = pColliderA->GetScene();

				CGameObject* pGameObjectA = pColliderA->GetGameObject();
				CGameObject* pGameObjectB = pColliderB->GetGameObject();

				CLayer* pLayerA = pColliderA->GetLayer();
				CLayer* pLayerB = pColliderB->GetLayer();


				if (pScene->GetLayerCollision(pLayerA, pLayerB))
				{
					CGameObject* pRootA = pGameObjectA->GetRoot();
					CGameObject* pRootB = pGameObjectB->GetRoot();

					pRootA->CollisionEnterAll(pColliderA, pColliderB, 0.0f);
					pRootB->CollisionEnterAll(pColliderB, pColliderA, 0.0f);

					SAFE_RELEASE(pGameObjectA);
					SAFE_RELEASE(pGameObjectB);
					SAFE_RELEASE(pLayerA);
					SAFE_RELEASE(pLayerB);
					SAFE_RELEASE(pScene);
				}
				else
				{
					SAFE_RELEASE(pGameObjectA);
					SAFE_RELEASE(pGameObjectB);
					SAFE_RELEASE(pLayerA);
					SAFE_RELEASE(pLayerB);
					SAFE_RELEASE(pScene);
					continue;
				}

			}
			else
			{
				continue;
			}
		}
		else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			CCollider* pColliderA = GetCollider(pairHeader.actors[0]);
			CCollider* pColliderB = GetCollider(pairHeader.actors[1]);

			if (pColliderA->IsEnable() && pColliderB->IsEnable())
			{
				CScene* pScene = pColliderA->GetScene();

				CGameObject* pGameObjectA = pColliderA->GetGameObject();
				CGameObject* pGameObjectB = pColliderB->GetGameObject();

				CLayer* pLayerA = pColliderA->GetLayer();
				CLayer* pLayerB = pColliderB->GetLayer();
				
				if (pScene->GetLayerCollision(pLayerA, pLayerB))
				{
					CGameObject* pRootA = pGameObjectA->GetRoot();
					CGameObject* pRootB = pGameObjectB->GetRoot();

					pRootA->CollisionStayAll(pColliderA, pColliderB, 0.0f);
					pRootB->CollisionStayAll(pColliderB, pColliderA, 0.0f);

					SAFE_RELEASE(pGameObjectA);
					SAFE_RELEASE(pGameObjectB);
					SAFE_RELEASE(pLayerA);
					SAFE_RELEASE(pLayerB);
					SAFE_RELEASE(pScene);
				}
				else
				{
					SAFE_RELEASE(pGameObjectA);
					SAFE_RELEASE(pGameObjectB);
					SAFE_RELEASE(pLayerA);
					SAFE_RELEASE(pLayerB);
					SAFE_RELEASE(pScene);
					continue;
				}

			}
			else
			{
				continue;
			}
		}
		else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			CCollider* pColliderA = GetCollider(pairHeader.actors[0]);
			CCollider* pColliderB = GetCollider(pairHeader.actors[1]);

			if (pColliderA->IsEnable() && pColliderB->IsEnable())
			{
				CScene* pScene = pColliderA->GetScene();

				CGameObject* pGameObjectA = pColliderA->GetGameObject();
				CGameObject* pGameObjectB = pColliderB->GetGameObject();

				CLayer* pLayerA = pColliderA->GetLayer();
				CLayer* pLayerB = pColliderB->GetLayer();


				if (pScene->GetLayerCollision(pLayerA, pLayerB))
				{
					CGameObject* pRootA = pGameObjectA->GetRoot();
					CGameObject* pRootB = pGameObjectB->GetRoot();

					pRootA->CollisionExitAll(pColliderA, pColliderB, 0.0f);
					pRootB->CollisionExitAll(pColliderB, pColliderA, 0.0f);

					SAFE_RELEASE(pGameObjectA);
					SAFE_RELEASE(pGameObjectB);
					SAFE_RELEASE(pLayerA);
					SAFE_RELEASE(pLayerB);
					SAFE_RELEASE(pScene);
				}
				else
				{
					SAFE_RELEASE(pGameObjectA);
					SAFE_RELEASE(pGameObjectB);
					SAFE_RELEASE(pLayerA);
					SAFE_RELEASE(pLayerB);
					SAFE_RELEASE(pScene);
					continue;
				}

			}
			else
			{
				continue;
			}
		}
	}
}

void CCollisionManager::onTrigger(PxTriggerPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxTriggerPair& cp = pairs[i];

		PxRigidActor* pActor = cp.triggerActor;
		PxRigidActor* pTarget = cp.otherActor;

		if (cp.status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			string strTargetName = pTarget->getName();
			string strBaseName = pActor->getName();

			CCollider* pColliderA = GetCollider(pActor);
			CCollider* pColliderB = GetCollider(pTarget);

			if (!pColliderA || !pColliderB)
				continue;
			

			if (pColliderA->IsEnable() && pColliderB->IsEnable())
			{
				CScene* pScene = pColliderA->GetScene();

				CGameObject* pGameObjectA = pColliderA->GetGameObject();
				CGameObject* pGameObjectB = pColliderB->GetGameObject();

				CLayer* pLayerA = pColliderA->GetLayer();
				CLayer* pLayerB = pColliderB->GetLayer();


				if (pScene->GetLayerCollision(pLayerA, pLayerB))
				{
					CGameObject* pRootA = pGameObjectA->GetRoot();
					CGameObject* pRootB = pGameObjectB->GetRoot();

					pRootA->CollisionEnterAll(pColliderA, pColliderB, 0.0f);
					pRootB->CollisionEnterAll(pColliderB, pColliderA, 0.0f);

					SAFE_RELEASE(pGameObjectA);
					SAFE_RELEASE(pGameObjectB);
					SAFE_RELEASE(pLayerA);
					SAFE_RELEASE(pLayerB);
					SAFE_RELEASE(pScene);
				}
				else
				{
					SAFE_RELEASE(pGameObjectA);
					SAFE_RELEASE(pGameObjectB);
					SAFE_RELEASE(pLayerA);
					SAFE_RELEASE(pLayerB);
					SAFE_RELEASE(pScene);
					continue;
				}

			}
			else
			{
				continue;
			}
		}
		else if (cp.status == PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			CCollider* pColliderA = GetCollider(pActor);
			CCollider* pColliderB = GetCollider(pTarget);

			if (!pColliderA || !pColliderB)
				continue;

			if (pColliderA->IsEnable() && pColliderB->IsEnable())
			{
				CScene* pScene = pColliderA->GetScene();

				CGameObject* pGameObjectA = pColliderA->GetGameObject();
				CGameObject* pGameObjectB = pColliderB->GetGameObject();

				CLayer* pLayerA = pColliderA->GetLayer();
				CLayer* pLayerB = pColliderB->GetLayer();


				if (pScene->GetLayerCollision(pLayerA, pLayerB))
				{
					CGameObject* pRootA = pGameObjectA->GetRoot();
					CGameObject* pRootB = pGameObjectB->GetRoot();

					pRootA->CollisionExitAll(pColliderA, pColliderB, 0.0f);
					pRootB->CollisionExitAll(pColliderB, pColliderA, 0.0f);

					SAFE_RELEASE(pGameObjectA);
					SAFE_RELEASE(pGameObjectB);
					SAFE_RELEASE(pLayerA);
					SAFE_RELEASE(pLayerB);
					SAFE_RELEASE(pScene);
				}
				else
				{
					SAFE_RELEASE(pGameObjectA);
					SAFE_RELEASE(pGameObjectB);
					SAFE_RELEASE(pLayerA);
					SAFE_RELEASE(pLayerB);
					SAFE_RELEASE(pScene);
					continue;
				}

			}
			else
			{
				continue;
			}
		}
	}
}

void CCollisionManager::onAdvance(const PxRigidBody * const *, const PxTransform *, const PxU32)
{
}

PxCooking * CCollisionManager::GetCooking() const
{
	return m_pCooking;
}

PxFoundation * CCollisionManager::GetFoundation() const
{
	return m_pFoundation;
}

PxPhysics * CCollisionManager::GetPhysics() const
{
	return m_pPhysics;
}

PxPvd * CCollisionManager::GetPVD() const
{
	return m_pPVD;
}

void CCollisionManager::AddCollider(CCollider * pCollider)
{
	if (pCollider)
	{
		CScene* pScene = pCollider->GetScene();
		pScene->GetPhysicsScene()->addActor(*pCollider->m_pActor);
		m_mapCollider.insert(make_pair(pCollider->m_pActor, pCollider));	
		SAFE_RELEASE(pScene);
	}
}

void CCollisionManager::RemoveCollider(CCollider * pCollider)
{
	if (pCollider)
	{
		unordered_map<PxRigidActor*, CCollider*>::iterator iter = m_mapCollider.find(pCollider->m_pActor);
		if (iter != m_mapCollider.end())
		{
			//CScene* pScene = pCollider->GetScene();
			//pScene->GetPhysicsScene()->removeActor(*iter->first);
			m_mapCollider.erase(iter);
			//SAFE_RELEASE(pScene);
		}
	}
}

CCollider * CCollisionManager::GetCollider(PxRigidActor * pActor)
{
	unordered_map<PxRigidActor*, CCollider*>::iterator iter = m_mapCollider.find(pActor);
	if (iter != m_mapCollider.end())
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}

void CCollisionManager::SetGravity(const Vector3 & vGravity)
{
	m_vGravity = vGravity;
	CScene* pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
	
	if (pScene)
	{
		pScene->GetPhysicsScene()->setGravity(PxVec3(m_vGravity.x, m_vGravity.y, m_vGravity.z));
		SAFE_RELEASE(pScene);
	}	
}

Vector3 CCollisionManager::GetGravity() const
{
	return m_vGravity;
}

float CCollisionManager::GetFixedTimeStep() const
{
	return m_fFixedTimeStep;
}

void CCollisionManager::SetFixedTimeStep(float fTimeStep)
{
	m_fFixedTimeStep = fTimeStep;
}

void CCollisionManager::Reset()
{	
}

RAYCASTHIT CCollisionManager::RayCast(const RAY & tRay)
{
	//대상의 Shape가 SceneQuery에 등록된 모양이어야 캐스팅이 가능하다.
	//PxShapeFlag::eSCENE_QUERY_SHAPE

	PxHitFlags iFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eDISTANCE | PxHitFlag::eMESH_ANY;
	PxRaycastBuffer tHit;
	PxVec3 vOrigin(tRay.vOrigin.x, tRay.vOrigin.y, tRay.vOrigin.z);
	PxVec3 vDirection(tRay.vDirection.x, tRay.vDirection.y, tRay.vDirection.z);
	CScene* pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
	PxScene* pPhysicsScene = pScene->GetPhysicsScene();

	bool bStatus = pScene->GetPhysicsScene()->raycast(vOrigin, vDirection, tRay.fDistance, tHit, iFlags);
	SAFE_RELEASE(pScene);
	RAYCASTHIT tResult;
	tResult.bResult = bStatus;
	
	if (bStatus)
	{
		PxVec3 vPosition = tHit.block.position;
		PxVec3 vNormal = tHit.block.normal;
		tResult.pCollider = GetCollider(tHit.block.actor);
		tResult.vPoint = Vector3(vPosition.x, vPosition.y, vPosition.z);
		tResult.vNormal = Vector3(vNormal.x, vNormal.y, vNormal.z);
		tResult.fDistamce = (float)tHit.block.distance;
	}	

	return tResult;
}

LINECASTHIT CCollisionManager::LineCast(const RAY & tRay)
{
	LINECASTHIT tLineCastHit;

	PxHitFlags iFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eDISTANCE | PxHitFlag::eMESH_ANY | PxHitFlag::eMESH_MULTIPLE;
	PxRaycastHit tHit[256] = {};
	PxRaycastBuffer tHitBuffer(tHit, 256);

	PxVec3 vOrigin(tRay.vOrigin.x, tRay.vOrigin.y, tRay.vOrigin.z);
	PxVec3 vDirection(tRay.vDirection.x, tRay.vDirection.y, tRay.vDirection.z);
	CScene* pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
	bool bStatus = pScene->GetPhysicsScene()->raycast(vOrigin, vDirection, tRay.fDistance, tHitBuffer, iFlags);
	SAFE_RELEASE(pScene);

	size_t iCount = tHitBuffer.getNbAnyHits();

	for (size_t i = 0; i < iCount; ++i)
	{
		PxRaycastHit HitIndexed = tHitBuffer.getAnyHit(i);

		RAYCASTHIT tResult;
		tResult.bResult = bStatus;
		if (bStatus)
		{
			PxVec3 vPosition = HitIndexed.position;
			PxVec3 vNormal = HitIndexed.normal;
			tResult.pCollider = GetCollider(HitIndexed.actor);
			tResult.vPoint = Vector3(vPosition.x, vPosition.y, vPosition.z);
			tResult.vNormal = Vector3(vNormal.x, vNormal.y, vNormal.z);
			tResult.fDistamce = (float)HitIndexed.distance;
		} 

		tLineCastHit.vecHits.push_back(tResult);
	}
	tLineCastHit.iCount = tLineCastHit.vecHits.size();

	return tLineCastHit;
}

void CCollisionManager::ClearCoveredUI()
{
	m_pCoveredButtonCollider = NULL;
}

void CCollisionManager::ClearCoveredCollider()
{
	m_pCoveredCollider = NULL;
}

CUICollider * CCollisionManager::GetCoveredUI() const
{
	return m_pCoveredButtonCollider;
}

CCollider * CCollisionManager::GetCoveredCollider() const
{
	return m_pCoveredCollider;
}


bool CCollisionManager::Initialize()
{
	char* strCallBackIP = "127.0.0.1";
	//Foundation은 프로세스 당 하나만 생성이 가능하다.

	m_pFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, m_Allocator, m_ErrorCallback);
	m_pPVD = PxCreatePvd(*m_pFoundation);
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPVD);

	PxCookingParams params(m_pPhysics->getTolerancesScale());
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
	params.buildGPUData = true; //Enable GRB data being produced in cooking.
	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, params);
	if (!m_pCooking)
		GET_SINGLE(CDebug)->Log(L"CollisionManager::Initialize Cooking 생성에 실패했습니다.");

	return true;
}

int CCollisionManager::UpdateCollision(float fTime)
{	
	//Start시 시간을 기록 함
	//
	CScene* pCurrentScene = GET_SINGLE(CSceneManager)->GetCurrentScene();

#pragma region Update Kinematic Collider Transformation
	{
		unordered_map<PxRigidActor*, CCollider*>::iterator iter;
		unordered_map<PxRigidActor*, CCollider*>::iterator iterEnd = m_mapCollider.end();

		for (iter = m_mapCollider.begin(); iter != iterEnd; ++iter)
		{
			if (iter->first->is<PxRigidDynamic>())
			{				
				if (((PxRigidDynamic*)iter->first)->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
				{
					CCollider* pCollider = iter->second;
					CTransform* pTransform = pCollider->GetTransform();
					Vector3 vColliderWorldPosition = pTransform->GetWorldPosition();

					Vector4 vQuaternion = pTransform->GetWorldRotation().ToQuaternion();
					PxQuat pxQuaternion = PxQuat(vQuaternion.x, vQuaternion.y, vQuaternion.z, vQuaternion.w);
					PxTransform pxTransform(vColliderWorldPosition.x, vColliderWorldPosition.y, vColliderWorldPosition.z, pxQuaternion);
					iter->first->setGlobalPose(pxTransform, false);
										
					SAFE_RELEASE(pTransform);
				}
			}
		}
	}
#pragma endregion
	
	if (pCurrentScene && GET_SINGLE(CSceneManager)->GetSceneState() == SS_PLAY)
	{
		PxScene* pPhysicsScene = pCurrentScene->GetPhysicsScene();
		pPhysicsScene->simulate(fTime);

		if (pPhysicsScene->fetchResults(true))
		{
			PxU32 iActorCount = pPhysicsScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
			if (iActorCount > 0)
			{
				vector<PxRigidActor*> vecActors(iActorCount);
				pPhysicsScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&vecActors[0]), iActorCount);
								
				PxShape* pShape = NULL;

				for (PxU32 i = 0; i<iActorCount; i++)
				{
					const PxU32 iShapeCount = vecActors[i]->getNbShapes();
					if (iShapeCount < 0)
					{
						vecActors[i]->getShapes(&pShape, 1);
					}
					bool bSleeping = vecActors[i]->is<PxRigidDynamic>() ? vecActors[i]->is<PxRigidDynamic>()->isSleeping() : false;
					
					//Update NonKinematic Transformation
					
					if (vecActors[i]->is<PxRigidDynamic>())
					{
						if (!(((PxRigidDynamic*)vecActors[i])->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC))
						{
							PxVec3 tPosition = vecActors[i]->getGlobalPose().p;
							PxQuat tQuaternion = vecActors[i]->getGlobalPose().q;
							Vector4 vQuaternion(tQuaternion.x, tQuaternion.y, tQuaternion.z, tQuaternion.w);

							CCollider* pCollider = GetCollider(vecActors[i]);
							CTransform* pTransform = pCollider->GetTransform();
							pTransform->SetWorldPosition(tPosition.x, tPosition.y, tPosition.z);
							pTransform->SetWorldRotation(Quaternion::EulerAngles(vQuaternion));
							SAFE_RELEASE(pTransform);
						}						
					}
				}
			}
		}		
	}

	SAFE_RELEASE(pCurrentScene);

	// false로 주고 Core에서 
	//while(!m_pScene->fetchResults(false))
	//{
	//		Logic()
	//} 
	//이후 받아온 값으로 Transform 세팅
	//
	
	CScene* pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();

	Vector2 vCursorPosition = GET_SINGLE(CInput)->GetCursorPosition();
	bool bCollisionWithUI = false;
	bool bCursorDown = GETKEYDOWN(MOUSE_LEFT) || GETKEYDOWN(MOUSE_RIGHT);
	bool bCursorPressed = GETKEY(MOUSE_LEFT) || GETKEY(MOUSE_RIGHT);
	bool bCursorUp = GETKEYUP(MOUSE_LEFT) || GETKEYUP(MOUSE_RIGHT);
		

#pragma region UI Collision
	{
		list<CUICollider*>::const_iterator iter;
		list<CUICollider*>::const_iterator iterEnd = pScene->GetButtonColliderList()->end();

		for (iter = pScene->GetButtonColliderList()->begin(); iter != iterEnd; ++iter)
		{
			CGameObject* pButton = (*iter)->GetGameObject();
			bool bEnable = pButton->IsEnable();
			SAFE_RELEASE(pButton);

			if ((*iter)->IsEnable() && bEnable)
			{
				bool bPrevCollision = (*iter)->GetCollision();
				bool bCurrentCollision = (*iter)->CheckCollision(vCursorPosition);

				if (!bPrevCollision && bCurrentCollision)
				{
					bCollisionWithUI = true;

					if (m_pCoveredButtonCollider)
					{
						CGameObject* pCoveredButton = m_pCoveredButtonCollider->GetGameObject();
						pCoveredButton->OnMouseExit(NULL, vCursorPosition, Vector3::Zero);
						m_pCoveredButtonCollider->SetCollision(false);						
						SAFE_RELEASE(pCoveredButton);

						CGameObject* pTargetButton = (*iter)->GetGameObject();
						pTargetButton->OnMouseEnter(NULL, vCursorPosition, Vector3::Zero);
						(*iter)->SetCollision(true);
						m_pCoveredButtonCollider = (*iter);
						SAFE_RELEASE(pTargetButton);
					}
					else
					{
						CGameObject* pTargetButton = (*iter)->GetGameObject();
						pTargetButton->OnMouseEnter(NULL, vCursorPosition, Vector3::Zero);
						(*iter)->SetCollision(true);
						m_pCoveredButtonCollider = (*iter);
						SAFE_RELEASE(pTargetButton);
					}
				}
				else if (bPrevCollision && bCurrentCollision)
				{
					bCollisionWithUI = true;

					CGameObject* pTargetButton = (*iter)->GetGameObject();
					pTargetButton->OnMouseStay(NULL, vCursorPosition, Vector3::Zero, fTime);

					if (bCursorDown)
					{
						pTargetButton->OnMouseDown(NULL, vCursorPosition, Vector3::Zero);
					}
					else if (bCursorPressed)
					{
						pTargetButton->OnMouseDrag(NULL, vCursorPosition, Vector3::Zero, fTime);
					}
					else if (bCursorUp)
					{
						pTargetButton->OnMouseUp(NULL, vCursorPosition, Vector3::Zero);
					}
					SAFE_RELEASE(pTargetButton);
				}
				else if (bPrevCollision && !bCurrentCollision)
				{
					bCollisionWithUI = true;

					CGameObject* pTargetButton = (*iter)->GetGameObject();
					pTargetButton->OnMouseExit(NULL, vCursorPosition, Vector3::Zero);
					(*iter)->SetCollision(false);
					m_pCoveredButtonCollider = NULL;
					SAFE_RELEASE(pTargetButton);
				}
				else
				{
					continue;
				}
			}
		}
	}
#pragma endregion

	CTransform* pCameraTransform = pScene->GetMainCameraTransform();
	Vector3 vCameraPosition = pCameraTransform->GetWorldPosition();
	SAFE_RELEASE(pCameraTransform);

#pragma region Ray Picking
{
	if (!bCollisionWithUI)
	{
		CCamera* pCamera = pScene->GetMainCamera();
		CTransform* pCameraTransform = pCamera->GetTransform();
		//Vector3 vCameraPosition = pCameraTransform->GetWorldPosition();

		if (vCursorPosition.x > 0 && vCursorPosition.x < DEVICE_RESOLUTION.iWidth && vCursorPosition.y > 0 &&
			vCursorPosition.y < DEVICE_RESOLUTION.iHeight)
		{
			RAY tRay = pCamera->ScreenPointToRay(vCursorPosition);
			RAYCASTHIT tHit = RayCast(tRay);
			
			if (tHit.bResult)
			{
				CGameObject* pTargetObject = tHit.pCollider->GetGameObject();
				bool bEnable = pTargetObject->IsEnable();

				if (bEnable && tHit.pCollider->IsEnable())
				{
					if (m_pCoveredCollider == tHit.pCollider)
					{
						pTargetObject->OnMouseStay(tHit.pCollider, vCursorPosition, tHit.vPoint, fTime);

						if (bCursorDown)
						{
							pTargetObject->OnMouseDown(tHit.pCollider, vCursorPosition, tHit.vPoint);
						}
						else if (bCursorPressed)
						{
							pTargetObject->OnMouseDrag(tHit.pCollider, vCursorPosition, tHit.vPoint, fTime);
						}
						else if (bCursorUp)
						{
							pTargetObject->OnMouseUp(tHit.pCollider, vCursorPosition, tHit.vPoint);
						}
					}
					else if (m_pCoveredCollider != tHit.pCollider && m_pCoveredCollider != NULL)
					{
						m_pCoveredCollider->OnMouseExit(m_pCoveredCollider, vCursorPosition, tHit.vPoint);
						pTargetObject->OnMouseEnter(tHit.pCollider, vCursorPosition, tHit.vPoint);

						m_pCoveredCollider = tHit.pCollider;

						if (bCursorDown)
						{
							pTargetObject->OnMouseDown(tHit.pCollider, vCursorPosition, tHit.vPoint);
						}
						else if (bCursorPressed)
						{
							pTargetObject->OnMouseDrag(tHit.pCollider, vCursorPosition, tHit.vPoint, fTime);
						}
						else if (bCursorUp)
						{
							pTargetObject->OnMouseUp(tHit.pCollider, vCursorPosition, tHit.vPoint);
						}
					}
					else if (m_pCoveredCollider == NULL && tHit.pCollider)
					{
						m_pCoveredCollider = tHit.pCollider;

						pTargetObject->OnMouseEnter(tHit.pCollider, vCursorPosition, tHit.vPoint);

						if (bCursorDown)
						{
							pTargetObject->OnMouseDown(tHit.pCollider, vCursorPosition, tHit.vPoint);
						}
						else if (bCursorPressed)
						{
							pTargetObject->OnMouseDrag(tHit.pCollider, vCursorPosition, tHit.vPoint, fTime);
						}
						else if (bCursorUp)
						{
							pTargetObject->OnMouseUp(tHit.pCollider, vCursorPosition, tHit.vPoint);
						}
					}
				}

				SAFE_RELEASE(pTargetObject);
			}
			else
			{
				if (m_pCoveredCollider)
				{
					CGameObject* pTargetObject = m_pCoveredCollider->GetGameObject();
					pTargetObject->OnMouseExit(m_pCoveredCollider, vCursorPosition, tHit.vPoint);

					if (bCursorDown)
					{
						pTargetObject->OnMouseDown(m_pCoveredCollider, vCursorPosition, tHit.vPoint);
					}
					else if (bCursorPressed)
					{
						pTargetObject->OnMouseDrag(m_pCoveredCollider, vCursorPosition, tHit.vPoint, fTime);
					}
					else if (bCursorUp)
					{
						pTargetObject->OnMouseUp(m_pCoveredCollider, vCursorPosition, tHit.vPoint);
					}

					SAFE_RELEASE(pTargetObject);
					m_pCoveredCollider = NULL;
				}
			}
		}

		SAFE_RELEASE(pCameraTransform);
		SAFE_RELEASE(pCamera);
	}
}
#pragma endregion




#pragma region 3D
{

}
#pragma endregion

	SAFE_RELEASE(pScene);
	
	return 0;
}
