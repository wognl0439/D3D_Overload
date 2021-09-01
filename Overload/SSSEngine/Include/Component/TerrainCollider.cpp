#include "TerrainCollider.h"
#include "../Texture.h"
#include "../CollisionManager.h"
#include "../ResourcesManager.h"
#include "../Transform.h"
#include "../Debug.h"
#include "../ShaderManager.h"
#include "Terrain.h"


SSS_USING

CTerrainCollider::CTerrainCollider():
	m_pHeightTexture(NULL),
	m_fMinHeight(0.0f),
	m_fMaxHeight(30.0f),
	m_vLocalScale(Vector3::One)
{
	m_fRow = 1.0f;
	m_fColumn = 1.0f;

	SetTag("Terrain Collider");

	m_pShader = GET_SINGLE(CShaderManager)->FindShader(COLLIDER_SHADER);
	m_pMesh = NULL;
	m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_COLOR_LAYOUT);

	SetRenderState(WIREFRAME);
}


CTerrainCollider::~CTerrainCollider()
{
	SAFE_RELEASE(m_pHeightTexture);
}


void CTerrainCollider::CreateActor(bool bStatic)
{
}

void CTerrainCollider::SetPhysicsTransform()
{
	if (m_pActor)
	{
		Vector3 vPosition = m_pTransform->GetWorldPosition();
		Vector4 vQuaternion = m_pTransform->GetWorldRotation().ToQuaternion();
		PxTransform pxTransform(vPosition.x, vPosition.y, vPosition.z, PxQuat(vQuaternion.x, vQuaternion.y, vQuaternion.z, vQuaternion.w));
		m_pActor->setGlobalPose(pxTransform, true);
	}
}

CTexture * CTerrainCollider::GetTexture() const
{
	return m_pHeightTexture;
}

Vector2 CTerrainCollider::GetHeightRange() const
{
	return Vector2(m_fMinHeight, m_fMaxHeight);
}

Vector3 CTerrainCollider::GetVolume() const
{
	return m_vLocalScale;
}

void CTerrainCollider::SetHeight(float fMin, float fMax)
{
	m_fMinHeight = fMin;
	m_fMaxHeight = fMax;
}


void CTerrainCollider::SetHeight(const Vector2 & vMinMax)
{
	m_fMinHeight = vMinMax.x;
	m_fMaxHeight = vMinMax.y;
}

void CTerrainCollider::SetVolume(const Vector2& vMinMax)
{
	if (m_pActor)
	{
		if (m_tGeometry.eType == CGT_TERRAIN)
		{
			//Terrain은 복수의 충돌체를 보유할 수 없다.
			if (m_tGeometry.vecResourceData.size() == 1)
			{
				size_t iShapeCount = m_vecShapes.size();
				for (size_t i = 0; i < iShapeCount; ++i)
				{
					m_pActor->detachShape(*m_vecShapes[i], false);
				}

				Vector3 vGeneralScale = m_pTransform->GetWorldScale();
				Vector3 vPosition = m_pTransform->GetWorldPosition();

				float fHeightScale = vMinMax.y - vMinMax.x;
				fHeightScale = vGeneralScale.y * (1.0f / 255.0f) * fHeightScale;
				float fRowScale = vGeneralScale.x;
				float fColumnScale = vGeneralScale.z;

				PxMeshScale tScale(PxVec3(fRowScale, fHeightScale, fColumnScale));
				size_t iGeometryCount = m_tGeometry.vecResourceData.size();

				RemoveShapes();
				for (size_t i = 0; i < iGeometryCount; ++i)
				{
					PxHeightFieldGeometry pGeometry(
						(PxHeightField*)m_tGeometry.vecResourceData[i], 
						PxMeshGeometryFlags(),
						tScale.scale.y, tScale.scale.x, tScale.scale.z);
					
					PxShape* pShape = GET_SINGLE(CCollisionManager)->GetPhysics()->createShape(pGeometry, *m_pPhysicsMaterial, true);
					m_pActor->attachShape(*pShape);
					m_vecShapes.push_back(pShape);
					pShape->release();
				}


				Vector2 vHorizontalPivot(fRowScale * m_fRow / 2.0f, fColumnScale * m_fColumn / 2.0f);

				PxTransform pxTransform = m_pActor->getGlobalPose();
				pxTransform.p.x = vPosition.x - vHorizontalPivot.x;
				pxTransform.p.y = vPosition.y + vMinMax.x;
				pxTransform.p.z = vPosition.z + vHorizontalPivot.y;
				m_pActor->setGlobalPose(pxTransform);
			}
		}
	}
}

void CTerrainCollider::LoadTerrain(float fMaxHeight, const Vector3& vScale, const string& strKey, const string& strFileName, const string& strPathKey)
{
	wstring wstrPath(strFileName.begin(), strFileName.end());
	SAFE_RELEASE(m_pHeightTexture);
	m_pHeightTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, wstrPath.c_str(), strPathKey);
	PxPhysics* pPhysics = GET_SINGLE(CCollisionManager)->GetPhysics();

	Vector3 vPosition = m_pTransform->GetWorldPosition();

	if (m_pHeightTexture)
	{
		PxU16 iColumns = PxU16(m_pHeightTexture->GetImage(0)->GetMetadata().width);
		PxU16 iRows = PxU16(m_pHeightTexture->GetImage(0)->GetMetadata().height);

		size_t iTextureStride = m_pHeightTexture->GetImage(0)->GetPixelsSize() / (iColumns * iRows);

		PxHeightFieldDesc heightFieldDesc;
		heightFieldDesc.nbColumns = iColumns;
		heightFieldDesc.nbRows = iRows;

		//Sample 은 Terrain을 만드는 각 버텍스(ControlPoint)이다. 
		//1개의 Sample은 32비트.
		//Sample에 높이는 16비트 (Short)
		//각 Sample Material 당 8비트 (1비트는 Flag 비트, 7비트는 Data 비트)이다.
		//Tesselation Flag는 삼각형이 생성될 방향을 의미한다. 1일경우 좌하/우상단에 상각형을 만들고, 0일경우 좌상/우하단에 삼각형을 만든다.
		vector<PxHeightFieldSample> vecSamples;
		uint8_t* pPixelOrigin = m_pHeightTexture->GetImage(0)->GetPixels();



		for (size_t y = 0; y < iColumns; ++y)
		{
			for (size_t x = 0; x < iRows; ++x)
			{
				uint8_t* pOffset = pPixelOrigin + iTextureStride * (y * iColumns + x);
				uint8_t iValue = *pOffset;

				PxHeightFieldSample tSample;		
				tSample.height = iValue;
				tSample.setTessFlag(); // 
				vecSamples.push_back(tSample);
			}
		}

		heightFieldDesc.samples.data = &vecSamples[0];
		heightFieldDesc.samples.stride = sizeof(PxU32);
		heightFieldDesc.convexEdgeThreshold = 0;
		heightFieldDesc.thickness = -50.0f;

		PxHeightField* pHeightField = GET_SINGLE(CCollisionManager)->GetCooking()->createHeightField(heightFieldDesc, pPhysics->getPhysicsInsertionCallback());
		
		if (pHeightField)
		{
			m_fRow = (float)iRows;
			m_fColumn = (float)iColumns;
		}
		else
		{
			m_fRow = 1.0f;
			m_fColumn = 1.0f;
		}

		//Primitive 의 경우 충돌체의 포인터를 가지고 있지 않도록 한다.
		m_tGeometry.Reset();
		m_tGeometry.eType = CGT_TERRAIN;
		m_tGeometry.vecResourceData.push_back(pHeightField);

				
		//Pivot 이 될 Transform 설정	
		Vector4 vQuat = Vector3(0.0f, PI / 2.0f, 0.0f).ToQuaternion();
		PxQuat vQuaternion(vQuat.x, vQuat.y, vQuat.z, vQuat.w);
		//PxTransform pose(PxVec3(vPosition.x - (iRows / 2.0f) , vPosition.y, vPosition.z - (iColumns / 2.0f)), vQuaternion);
		PxTransform pose(PxVec3(vPosition.x - (iRows / 2.0f), vPosition.y, vPosition.z + (iColumns / 2.0f)), vQuaternion);
		m_pActor = pPhysics->createRigidStatic(pose);
		m_pActor->setName("Terrain");
		if (!m_pActor)
		{
			GET_SINGLE(CDebug)->Log(L"Terrain Collider Static Actor 생성에 실패하였습니다.");
		}

		m_vLocalScale.x = 1.0f;
		m_vLocalScale.y = 1.0f;
		m_vLocalScale.z = 1.0f;

		RemoveShapes();
		//로컬 Scale과 PxHeightField는 별개이므로 임시로 원본의 크기로 설정되도록 해준다.
		PxShape* pShape = GET_SINGLE(CCollisionManager)->GetPhysics()->createShape(PxHeightFieldGeometry(pHeightField, PxMeshGeometryFlags(), (1.0f / 255.0f) * fMaxHeight, 1.0f, 1.0f), *m_pPhysicsMaterial, true);

		if (!pShape)
		{
			GET_SINGLE(CDebug)->Log(L"Terrain Collider Shape 생성에 실패하였습니다.");
		}
		pShape->setFlag(PxShapeFlag::ePARTICLE_DRAIN, false);
		pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);

		m_pActor->attachShape(*pShape);
		m_vecShapes.push_back(pShape);
		pShape->release();
		GET_SINGLE(CCollisionManager)->AddCollider(this); 
		m_pGameObject->SetStatic(true);
	}
	else
	{
		GET_SINGLE(CDebug)->Log(L"높이 텍스쳐 로드에 실패했습니다. Terrain Collider Mesh를 생성할 수 없습니다.");
	}	
}

void CTerrainCollider::LoadTerrain(const string & strKey, const string & strFileName, const string & strPathKey)
{
	LoadTerrain(m_fMaxHeight, m_vLocalScale, strKey, strFileName, strPathKey);
}


void CTerrainCollider::Start()
{
	CTerrain* pTerrain = GetComponent<CTerrain>();
	if(pTerrain)
	{
		m_fMinHeight = pTerrain->GetMinHeight();
		m_fMaxHeight = pTerrain->GetMaxHeight();	
		SAFE_RELEASE(pTerrain);
	}
	
	SetVolume(Vector2(m_fMinHeight, m_fMaxHeight));
}

bool CTerrainCollider::Initialize()
{
	CCollider::Initialize();
	return true;
}

int CTerrainCollider::LateUpdate(float fTime)
{
	return 0;
}

void CTerrainCollider::RenderDebug(float fDeltaTime)
{
}

int CTerrainCollider::OnCollisionEnter(CCollider * pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

int CTerrainCollider::OnCollisionStay(CCollider * pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

int CTerrainCollider::OnCollisionExit(CCollider * pThis, CCollider * pTarget, float fTime)
{
	return 0;
}

void CTerrainCollider::OnMouseEnter(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
}

void CTerrainCollider::OnMouseExit(CCollider * pTarget, const Vector2 & vPosition, const Vector3 & vIntersectPosition)
{
}

bool CTerrainCollider::Save(FILE * pFile)
{
	float fFriction = m_pPhysicsMaterial->getStaticFriction();
	float fDynamicFriction = m_pPhysicsMaterial->getDynamicFriction();
	float fRestitution = m_pPhysicsMaterial->getRestitution();

	fwrite(&fFriction, sizeof(float), 1, pFile);
	fwrite(&fDynamicFriction, sizeof(float), 1, pFile);
	fwrite(&fRestitution, sizeof(float), 1, pFile);

	fwrite(&m_fMaxHeight, sizeof(float), 1, pFile);
	fwrite(&m_vLocalScale, sizeof(Vector3), 1, pFile);

	bool bHasHeightTexture = false;
	if (m_pHeightTexture)
	{
		bHasHeightTexture = true;
	}
	fwrite(&bHasHeightTexture, sizeof(bool), 1, pFile);

	if (bHasHeightTexture)
	{
		string strKey = m_pHeightTexture->GetKey();
		string strPathKey = m_pHeightTexture->GetPathKey();
		wstring strRelativePath = m_pHeightTexture->GetRelativePath();

		size_t iLength = strKey.length();
		fwrite(&iLength, sizeof(size_t), 1, pFile);
		fwrite(strKey.c_str(), sizeof(char), iLength, pFile);

		iLength = strPathKey.length();
		fwrite(&iLength, sizeof(size_t), 1, pFile);
		fwrite(strPathKey.c_str(), sizeof(char), iLength, pFile);

		iLength = strRelativePath.length();
		fwrite(&iLength, sizeof(size_t), 1, pFile);
		fwrite(strRelativePath.c_str(), sizeof(wchar_t), iLength, pFile);
	}
	

	return true;
}

bool CTerrainCollider::Load(FILE * pFile)
{
	float fFriction = 0;
	float fDynamicFriction = 0;
	float fRestitution = 0;

	fread(&fFriction, sizeof(float), 1, pFile);
	fread(&fDynamicFriction, sizeof(float), 1, pFile);
	fread(&fRestitution, sizeof(float), 1, pFile);

	fread(&m_fMaxHeight, sizeof(float), 1, pFile);
	fread(&m_vLocalScale, sizeof(Vector3), 1, pFile);

	bool bHasHeightTexture = false;
	fread(&bHasHeightTexture, sizeof(bool), 1, pFile);
	
	if (bHasHeightTexture)
	{
		char strKey[256] = {};
		char strPathKey[256] = {};
		wchar_t strRelativePath[512] = {};

		size_t iLength = 0;
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(strKey, sizeof(char), iLength, pFile);

		iLength = 0;
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(strPathKey, sizeof(char), iLength, pFile);

		iLength = 0;
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(strRelativePath, sizeof(wchar_t), iLength, pFile);
		
		m_pHeightTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, strRelativePath, strPathKey);

		if (!m_pHeightTexture)
		{
			wstring strObjectName(m_pGameObject->GetTag().begin(), m_pGameObject->GetTag().end());
			GET_SINGLE(CDebug)->Log(strObjectName + L"오브젝트가 가진 Terrain Collider 컴포넌트의 HeightMap Texture 로드에 실패했습니다. 경로를 확인해주세요.\n");
		}
		else
		{
			wstring wstrTemp(strRelativePath);
			string strTemp(wstrTemp.begin(), wstrTemp.end());
			LoadTerrain(m_fMaxHeight, m_vLocalScale, strKey, strTemp.c_str(), strPathKey);
		}
	}
	
	return true;
}
