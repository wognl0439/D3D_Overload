#pragma once
#include "Engine.h"



//PhysX
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "LowLevelDEBUG_x64")
#pragma comment(lib, "LowLevelAABBDEBUG_x64")
#pragma comment(lib, "LowLevelClothDEBUG_x64")
#pragma comment(lib, "LowLevelDynamicsDEBUG_x64")
#pragma comment(lib, "LowLevelParticlesDEBUG_x64")
#pragma comment(lib, "PhysX3DEBUG_x64")
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x64")
#pragma comment(lib, "PhysX3CommonDEBUG_x64")
#pragma comment(lib, "PhysX3CookingDEBUG_x64")
#pragma comment(lib, "PhysX3ExtensionsDEBUG_x64")
#pragma comment(lib, "PhysX3VehicleDEBUG_x64")
#pragma comment(lib, "SceneQueryDEBUG_x64")
#pragma comment(lib, "PsFastXmlDEBUG_x64")
#pragma comment(lib, "PxFoundationDEBUG_x64")
#pragma comment(lib, "PxPvdSDKDEBUG_x64")
#pragma comment(lib, "PxTaskDEBUG_x64")
#pragma comment(lib, "SimulationControllerDEBUG_x64")
#else
#pragma comment(lib, "LowLevel_x64")
#pragma comment(lib, "LowLevelAABB_x64")
#pragma comment(lib, "LowLevelCloth_x64")
#pragma comment(lib, "LowLevelDynamics_x64")
#pragma comment(lib, "LowLevelParticles_x64")
#pragma comment(lib, "PhysX3_x64")
#pragma comment(lib, "PhysX3CharacterKinematic_x64")
#pragma comment(lib, "PhysX3Common_x64")
#pragma comment(lib, "PhysX3Cooking_x64")
#pragma comment(lib, "PhysX3Extensions_x64")
#pragma comment(lib, "PhysX3Vehicle_x64")
#pragma comment(lib, "SceneQuery_x64")
#pragma comment(lib, "PsFastXml_x64")
#pragma comment(lib, "PxFoundation_x64")
#pragma comment(lib, "PxPvdSDK_x64")
#pragma comment(lib, "PxTask_x64")
#pragma comment(lib, "SimulationController_x64")
#endif // _DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib, "LowLevelDEBUG_x86")
#pragma comment(lib, "LowLevelAABBDEBUG_x86")
#pragma comment(lib, "LowLevelClothDEBUG_x86")
#pragma comment(lib, "LowLevelDynamicsDEBUG_x86")
#pragma comment(lib, "LowLevelParticlesDEBUG_x86")
#pragma comment(lib, "PhysX3DEBUG_x86")
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x86")
#pragma comment(lib, "PhysX3CommonDEBUG_x86")
#pragma comment(lib, "PhysX3CookingDEBUG_x86")
#pragma comment(lib, "PhysX3ExtensionsDEBUG_x86")
#pragma comment(lib, "PhysX3VehicleDEBUG_x86")
#pragma comment(lib, "SceneQueryDEBUG_x86")
#pragma comment(lib, "PsFastXmlDEBUG_x86")
#pragma comment(lib, "PxFoundationDEBUG_x86")
#pragma comment(lib, "PxPvdSDKDEBUG_x86")
#pragma comment(lib, "PxTaskDEBUG_x86")
#pragma comment(lib, "SimulationControllerDEBUG_x86")
#else
#pragma comment(lib, "LowLevel_x86")
#pragma comment(lib, "LowLevelAABB_x86")
#pragma comment(lib, "LowLevelCloth_x86")
#pragma comment(lib, "LowLevelDynamics_x86")
#pragma comment(lib, "LowLevelParticles_x86")
#pragma comment(lib, "PhysX3_x86")
#pragma comment(lib, "PhysX3CharacterKinematic_x86")
#pragma comment(lib, "PhysX3Common_x86")
#pragma comment(lib, "PhysX3Cooking_x86")
#pragma comment(lib, "PhysX3Extensions_x86")
#pragma comment(lib, "PhysX3Vehicle_x86")
#pragma comment(lib, "SceneQuery_x86")
#pragma comment(lib, "PsFastXml_x86")
#pragma comment(lib, "PxFoundation_x86")
#pragma comment(lib, "PxPvdSDK_x86")
#pragma comment(lib, "PxTask_x86")
#pragma comment(lib, "SimulationController_x86")
#endif // _DEBUG
#endif



#include "PxPhysicsAPI.h"
//누락
#include "extensions/PxRepXSimpleType.h"
#include "extensions/PxSceneQueryExt.h"
#include "extensions/PxRigidActorExt.h"
#include "extensions/PxRaycastCCD.h"
#include "extensions/PxMassProperties.h"
#include "extensions/PxDefaultSimulationFilterShader.h"
#include "extensions/PxDefaultErrorCallback.h"
#include "extensions/PxDefaultCpuDispatcher.h"
#include "extensions/PxDefaultAllocator.h"
#include "extensions/PxD6Joint.h"
#include "extensions/PxConstraintExt.h"
#include "extensions/PxCollectionExt.h"
#include "extensions/PxClothTetherCooker.h"
#include "extensions/PxClothMeshQuadifier.h"
#include "extensions/PxClothMeshDesc.h"
#include "extensions/PxClothFabricCooker.h"
#include "extensions/PxBroadPhaseExt.h"
#include "extensions/PxBinaryConverter.h"

using namespace physx;

SSS_BEGIN

enum FRUSTUM_PLANE
{
	FP_LEFT,
	FP_RIGHT,
	FP_TOP,
	FP_BOTTOM,
	FP_NEAR,
	FP_FAR,
	FP_END
};

typedef struct SSS_DLL _tagFrustum
{
	Vector3 vPos[8];
	Vector4 vPlane[FP_END];

	_tagFrustum()
	{
		vPos[0] = Vector3(-1.f, 1.f, 0.f);
		vPos[1] = Vector3(1.f, 1.f, 0.f);
		vPos[2] = Vector3(-1.f, -1.f, 0.f);
		vPos[3] = Vector3(1.f, -1.f, 0.f);
		vPos[4] = Vector3(-1.f, 1.f, 1.f);
		vPos[5] = Vector3(1.f, 1.f, 1.f);
		vPos[6] = Vector3(-1.f, -1.f, 1.f);
		vPos[7] = Vector3(1.f, -1.f, 1.f);
	}
}FRUSTUM, *PFRUSTUM;

typedef struct SSS_DLL _tagRayCastHit
{
	bool bResult;
	class CCollider* pCollider;
	Vector3 vPoint;
	Vector3 vNormal;
	float fDistamce;

	_tagRayCastHit():bResult(false), pCollider(NULL), fDistamce(0.0f)
	{}

}RAYCASTHIT, *PRAYCASTHIT;


typedef struct SSS_DLL _tagLineCastHit
{
	size_t iCount;
	vector<RAYCASTHIT> vecHits;

	_tagLineCastHit():iCount(0)
	{}

}LINECASTHIT, *PLINECASTHIT;

class SSS_DLL CCollisionManager:
	public PxSimulationEventCallback
{
	DECLARE_SINGLE(CCollisionManager)	

	void	onConstraintBreak(PxConstraintInfo*, PxU32) override;
	void	onWake(PxActor**, PxU32) override;
	void	onSleep(PxActor**, PxU32) override;
	void	onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
	void	onTrigger(PxTriggerPair* pairs, PxU32 nbPairs) override;
	void	onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32) override;
	
public:
	//Scene으로 이전
	PxDefaultAllocator				m_Allocator;
	PxDefaultErrorCallback			m_ErrorCallback;
	PxFoundation*					m_pFoundation;
	PxPhysics*							m_pPhysics;
	//PxDefaultCpuDispatcher*		m_pDispatcher;
	//PxGpuDispatcher*				m_pGPUDispatcher;
	//PxCudaContextManager*		m_pCudaContextManager;
	PxPvd*								m_pPVD;
	PxCooking*							m_pCooking;

private:
	float m_fFixedTimeStep;
	Vector3 m_vGravity;

public:
	PxCooking* GetCooking() const;
	PxFoundation* GetFoundation() const;
	PxPhysics* GetPhysics() const;
	PxPvd* GetPVD() const;


	void AddCollider(class CCollider* pCollider = NULL);
	//Collider를 PhysicsScene에서 빼줌.
	//해제하지는 않음.
	void RemoveCollider(class CCollider* pCollider);
	CCollider* GetCollider(class PxRigidActor* pActor);
	void SetGravity(const Vector3& vGravity);
	Vector3 GetGravity() const;

	float GetFixedTimeStep() const;
	void SetFixedTimeStep(float fTimeStep);
	void Reset();

private:
	class CUICollider* m_pCoveredButtonCollider;
	class CCollider* m_pCoveredCollider;
	unordered_map<class PxRigidActor*, class CCollider*> m_mapCollider;


public:

	RAYCASTHIT RayCast(const RAY& tRay);
	LINECASTHIT LineCast(const RAY& tRay);
	void ClearCoveredUI();
	void ClearCoveredCollider();
	class CUICollider* GetCoveredUI() const;
	class CCollider* GetCoveredCollider() const;
	
public:
	bool Initialize();
	int UpdateCollision(float fTime);

};


SSS_END