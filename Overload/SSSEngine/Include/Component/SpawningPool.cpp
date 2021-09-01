#include "SpawningPool.h"
#include "TerrainCollider.h"
#include "Camera.h"
#include "../Transform.h"
#include "../PathManager.h"
#include "../Scene.h"
#include "../Engine.h"
#include "../CollisionManager.h"
#include "../ResourcesManager.h"
#include "../ShaderManager.h"
#include "../Device.h"
#include "../Core.h"
#include "../RenderManager.h"
#include "../Mesh.h"
#include "../Shader.h"
#include "../Sampler.h"
#include "../RenderState.h"

SSS_USING

void CSpawningPool::AddPool(const string & strKey)
{
	unordered_map<string, POBJECTPOOL>::iterator iter = m_mapPool.find(strKey);
	if (iter == m_mapPool.end())
	{
		OBJECTPOOL* pPool = new OBJECTPOOL;
		pPool->strKey = strKey;
		m_mapPool.insert(make_pair(strKey, pPool));
	}
}

void CSpawningPool::AddRange(const string & strTargetPool, const Vector2 & vXZPlaneCenter, float fRadius, const Vector4& vDebugColor)
{
	unordered_map<string, POBJECTPOOL>::iterator iter = m_mapPool.find(strTargetPool);

	if (iter != m_mapPool.end())
	{
		iter->second->vecRangeInfo.push_back(Vector3(vXZPlaneCenter.x, vXZPlaneCenter.y, fRadius));
		iter->second->vecRangeColor.push_back(vDebugColor);
	}
}

void CSpawningPool::SetPrefab(const string & strTargetPool, const string& strPrefabRelativePath, const string& strPrefabPathKey)
{
	unordered_map<string, POBJECTPOOL>::iterator iter = m_mapPool.find(strTargetPool);

	if (iter != m_mapPool.end())
	{
		if (strPrefabPathKey == NO_USE_PATH || strPrefabPathKey.empty())
		{
			char* pRelativePath = NULL;
			char* pPathKey = NULL;
			GET_SINGLE(CPathManager)->ExtractRelativeDirectoryFromFullPath(strPrefabRelativePath.c_str(), &pRelativePath, &pPathKey);

			string strRelativePath(pRelativePath);
			iter->second->wstrPrefabRelativePath = wstring(strRelativePath.begin(), strRelativePath.end());
			iter->second->strPrefabPathKey = pPathKey;
		}
		else
		{
			string strRelativePath(strPrefabRelativePath);
			iter->second->wstrPrefabRelativePath = wstring(strRelativePath.begin(), strRelativePath.end());
			iter->second->strPrefabPathKey = strPrefabPathKey;
		}
	}
}

void CSpawningPool::SetInterval(const string & strTargetPool, float fInterval)
{
	unordered_map<string, POBJECTPOOL>::iterator iter = m_mapPool.find(strTargetPool);

	if (iter != m_mapPool.end())
	{
		iter->second->fInterval = fInterval;
	}
}

void CSpawningPool::SetMaxSpawnCount(const string & strTargetPool, uint32_t iMax)
{
	unordered_map<string, POBJECTPOOL>::iterator iter = m_mapPool.find(strTargetPool);

	if (iter != m_mapPool.end())
	{
		iter->second->iMaxCount = iMax;
	}
}

bool CSpawningPool::Initialize()
{
	return true;
}

void CSpawningPool::Start()
{
	unordered_map<string, POBJECTPOOL>::iterator iter;
	unordered_map<string, POBJECTPOOL>::iterator iterEnd = m_mapPool.end();
	for (iter = m_mapPool.begin(); iter != iterEnd; ++iter)
	{
		iter->second->fTimer = iter->second->fInterval;
	}
}

int CSpawningPool::Update(float fTime)
{
	unordered_map<string, POBJECTPOOL>::iterator iter;
	unordered_map<string, POBJECTPOOL>::iterator iterEnd = m_mapPool.end();

	for (iter = m_mapPool.begin(); iter != iterEnd; ++iter)
	{
		size_t iObjectCount = iter->second->vecGameObjects.size();
		//SpawnCount :: Enabled인 오브젝트 갯수
		//LoadCount :: ObjectCount;
		size_t iEnableCount = 0;
		for (size_t i = 0; i < iObjectCount; ++i)
		{
			if (iter->second->vecGameObjects[i]->IsEnable())
			{
				++iEnableCount;
			}
		}

		iter->second->iCurrentLoadedCount = iObjectCount;
		iter->second->iCurrentLoadedCount = iEnableCount;
	}
	
	for (iter = m_mapPool.begin(); iter != iterEnd; ++iter)
	{
		iter->second->fTimer += fTime;
		if (iter->second->fInterval <= iter->second->fTimer)
		{
			iter->second->fTimer -= iter->second->fInterval;

			size_t iAvailableCount = iter->second->vecGameObjects.size();
			size_t iMaxCount = iter->second->iMaxCount;
			size_t iCurrentCount = iter->second->iCurrentSpawnCount;
			size_t iSpawnCount = iMaxCount - iCurrentCount;
			for (size_t i = 0; i < iSpawnCount; ++i)
			{
				size_t iCircleIndex = rand() % iter->second->vecRangeInfo.size();
				Vector2 vCenter = Vector2(iter->second->vecRangeInfo[iCircleIndex].x, iter->second->vecRangeInfo[iCircleIndex].y);
				float fRange = iter->second->vecRangeInfo[iCircleIndex].z;
				Vector2 vPosition = Vector2::RandomPointInsideCircle(vCenter, fRange, 0.0f);

				RAY tRay = {};
				tRay.vOrigin = Vector3(vPosition.x, 10000.0f, vPosition.y);
				tRay.vDirection = Vector3(0.0f, -1.0f, 0.0f);
				tRay.fDistance = 20000.0f;
				LINECASTHIT tHit = GET_SINGLE(CCollisionManager)->LineCast(tRay);
				
				Vector3 vSpawnPosition;
				for (size_t i = 0; i < tHit.iCount; ++i)
				{
					if (tHit.vecHits[i].bResult)
					{
						if (tHit.vecHits[i].pCollider->IsComponent<CTerrainCollider>())
						{
							vSpawnPosition = tHit.vecHits[i].vPoint;
						}
					}
				}

				//생성 / 로드 / 사용
				if (iAvailableCount < iMaxCount)
				{
					if (vSpawnPosition.x != 0.f && vSpawnPosition.z != 0.f)
					{
						CGameObject* pGameObject = CGameObject::Load(m_pScene, iter->second->wstrPrefabRelativePath.c_str(), iter->second->strPrefabPathKey.c_str());
						iter->second->vecGameObjects.push_back(pGameObject);
						CTransform* pTransform = pGameObject->GetTransform();
						pTransform->SetWorldPosition(vSpawnPosition);
						SAFE_RELEASE(pTransform);
						SAFE_RELEASE(pGameObject);

						iAvailableCount = iter->second->vecGameObjects.size();
						iter->second->iCurrentLoadedCount++;
						iter->second->iCurrentSpawnCount++;
					}
				}
				else
				{
					//로드는 전부 되어 있음.

					//동작하는 오브젝트의 갯수가 부족할 때
					if (iter->second->iCurrentSpawnCount < iMaxCount)
					{
						for (size_t j = 0; j < iAvailableCount; ++j)
						{
							if (!iter->second->vecGameObjects[j]->IsEnable())
							{
								iter->second->vecGameObjects[j]->Reset();
								CTransform* pTransform = iter->second->vecGameObjects[j]->GetTransform();
								pTransform->SetWorldPosition(vSpawnPosition);
								SAFE_RELEASE(pTransform);
								iter->second->vecGameObjects[j]->Enable(true);
								iter->second->iCurrentSpawnCount++;
								break;
							}
						}
					}
				}
			}
		}
	}

	return 0;
}

void CSpawningPool::RenderDebug(float fTime)
{
	//모든 풀을 돌면서 영역을 그려줌.
	
	unordered_map<string, POBJECTPOOL>::iterator iter ;
	unordered_map<string, POBJECTPOOL>::iterator iterEnd = m_mapPool.end();
	
	for (iter = m_mapPool.begin(); iter != iterEnd; ++iter)
	{
		size_t iRangeCount = iter->second->vecRangeInfo.size();
		for (size_t i = 0; i < iRangeCount; ++i)
		{
			Vector3 vInfo = iter->second->vecRangeInfo[i];
			GET_SINGLE(CShaderManager)->UpdateConstantBuffer(SPAWNINGPOOL_CBUFFER, &iter->second->vecRangeColor[i], CBT_PIXEL);
			DrawRange(Vector3(vInfo.x, 0.0f, vInfo.y), vInfo.z, fTime);
		}
	}
}

void CSpawningPool::DrawRange(const Vector3& vCenter, float fRadius, float fTime)
{
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		CCamera*	pCamera = m_pScene->GetMainCamera();
		CTransform* pCameraTransform = pCamera->GetTransform();
		float vDistance = (vCenter - pCameraTransform->GetWorldPosition()).Length();

		TRANSFORMCBUFFER	tBuffer = {};
		
		//영벡터
		Matrix	matScale, matPos, matWorld;
		matScale = Matrix::Scaling(Vector3(fRadius * 2, 1.0f, fRadius * 2));
		matPos = Matrix::Translation(vCenter * vDistance);
		matWorld = matScale * matPos;
	

	
		tBuffer.matWorld = matWorld;
		tBuffer.matView = pCamera->GetViewMatrix();
		tBuffer.matProjection = pCamera->GetPerspectiveProjectionMatrix();
		tBuffer.matWV = matWorld * tBuffer.matView;
		tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
		tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
		tBuffer.matWP = tBuffer.matWorld * tBuffer.matProjection;
		tBuffer.vPivot = Vector3::Zero;
		tBuffer.vLength = m_pMesh->GetLength();
	
		SAFE_RELEASE(pCamera);
	
		tBuffer.matWorld = tBuffer.matWorld.Transpose();
		tBuffer.matView = tBuffer.matView.Transpose();
		tBuffer.matProjection = tBuffer.matProjection.Transpose();
		tBuffer.matWV = tBuffer.matWV.Transpose();
		tBuffer.matWVP = tBuffer.matWVP.Transpose();
		tBuffer.matVP = tBuffer.matVP.Transpose();
		tBuffer.matWP = tBuffer.matWP.Transpose();
	
		// Transform 정보를 Shader 상수버퍼로 넘겨준다.
		GET_SINGLE(CShaderManager)->UpdateConstantBuffer(TRANSFORM_CBUFFER, &tBuffer, SSS::CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);
	
	
		for (int i = 0; i < RS_END; ++i)
		{
			if (m_pRenderState[i])
				m_pRenderState[i]->SetState();
		}
	
	
		DEVICE_CONTEXT->IASetInputLayout(m_pLayout);
		m_pShader->SetShader();
		if (m_pSampler)
		{
			m_pSampler->SetSampler(0);
		}
	
		if (m_pMesh)
		{
			m_pMesh->Render();
		}
	
		for (int i = 0; i < RS_END; ++i)
		{
			if (m_pRenderState[i])
				m_pRenderState[i]->ResetState();
		}
	}
}

CSpawningPool::CSpawningPool()
{
	m_pShader = GET_SINGLE(CShaderManager)->FindShader(SPAWNINGPOOL_SHADER);
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(PRIMITIVE_MESH_CYLINDER);
	m_pSampler = GET_SINGLE(CResourcesManager)->FindSampler(SAMPLER_LINEAR);
	m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_COLOR_NORMAL_LAYOUT);
	m_pRenderState[RS_BLEND] = GET_SINGLE(CRenderManager)->FindRenderState(ALPHA_BLEND);
	m_pRenderState[RS_RASTERIZER] = GET_SINGLE(CRenderManager)->FindRenderState(CULLBACK);
	m_pRenderState[RS_DEPTHSTENCIL] = GET_SINGLE(CRenderManager)->FindRenderState(DEPTH_DISABLE);	
}

CSpawningPool::~CSpawningPool()
{
	unordered_map<string, POBJECTPOOL>::iterator iter;
	unordered_map<string, POBJECTPOOL>::iterator iterEnd = m_mapPool.end();

	for (iter = m_mapPool.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);		
	}
	m_mapPool.clear();
	
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pSampler);
	SAFE_RELEASE(m_pRenderState[RS_BLEND]);
	SAFE_RELEASE(m_pRenderState[RS_RASTERIZER]);
	SAFE_RELEASE(m_pRenderState[RS_DEPTHSTENCIL]);
}

bool CSpawningPool::Save(FILE * pFile)
{
	return true;
}

bool CSpawningPool::Load(FILE * pFile)
{
	return true;
}
