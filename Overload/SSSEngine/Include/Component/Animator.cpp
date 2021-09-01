#include "Animator.h"
#include "../AnimationStateController.h"
#include "../AnimationState.h"
#include "../AnimationClip.h"
#include "../GameObject.h"
#include "../Component.h"
#include "SpriteRenderer.h"
#include "MeshRenderer.h"
#include "../Material.h"
#include "../Transform.h"
#include "../Device.h"
#include "../Shader.h"
#include "../ShaderManager.h"
#include "../Transition.h"
#include "../DataManager.h"
#include "../Texture.h"

SSS_USING

CAnimator::CAnimator():
	m_pSpriteRenderer(NULL),
	m_pMeshRenderer(NULL),
	m_bSyncScale(false),
	m_vScaleFactor(Vector3::One),
	m_vLocalScaleFactor(Vector3::One),
	m_pBoneTexture(NULL),
	m_pBoneTextureRV(NULL)
{
	SetTag("Animator");
}

CAnimator::~CAnimator()
{
	SAFE_DELETE(m_pAnimationController);
	//SAFE_RELEASE(m_pSpriteRenderer);
	//SAFE_RELEASE(m_pMeshRenderer);
	SAFE_RELEASE(m_pBoneTextureRV);
	SAFE_RELEASE(m_pBoneTexture);
}
void CAnimator::BlendKeyFrame(const vector<KEYFRAMEBUFFER>& vecBoneSrc, const vector<KEYFRAMEBUFFER>& vecBoneDest, float fSLerpPercent)
{

	CMesh* pMesh = m_pMeshRenderer->GetMesh();
	size_t iSize = pMesh->GetBoneCount();

	// 본 수만큼 반복한다.
	for (size_t i = 0; i < iSize; ++i)
	{
		// 키프레임이 없을 경우
		if (vecBoneDest[i].bEmpty || !vecBoneSrc[i].pKeyFrame || !vecBoneDest[i].pKeyFrame)
		{
			continue;
		}

		const PKEYFRAME pCurKey = vecBoneSrc[i].pKeyFrame;
		const PKEYFRAME pNextKey = vecBoneDest[i].pKeyFrame;

		Vector3 vBlendPos = pCurKey->vPos;
		Vector3 vBlendScale = pCurKey->vScale;
		Vector3 vBlendRot = pCurKey->vRot;

		// 현재 프레임의 시간을 얻어온다.
		double	 dFrameTime = pCurKey->dTime;
		XMVECTOR vS = XMVectorLerp(pCurKey->vScale.Convert(), pNextKey->vScale.Convert(), fSLerpPercent);
		XMVECTOR vT = XMVectorLerp(pCurKey->vPos.Convert(), pNextKey->vPos.Convert(), fSLerpPercent);
		XMVECTOR vR = XMQuaternionSlerp(pCurKey->vRot.Convert(), pNextKey->vRot.Convert(), fSLerpPercent);
		XMVECTOR vZero = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		Matrix	matBone = XMMatrixAffineTransformation(vS, vZero, vR, vT);

		

		size_t iRegisteredCount = m_vecRegisteredBoneTransform.size();
		if (iRegisteredCount > 0)
		{
			for (size_t j = 0; j < iRegisteredCount; ++j)
			{
				if (m_vecRegisteredBoneTransform[j].iBoneIndex == i)
				{
					m_vecRegisteredBoneTransform[j].pTransform->SetWorldMatrix(matBone * m_pTransform->GetWorldMatrix());
				}
			}
		}

		matBone = *pMesh->GetBone()->at(i)->matOffset * matBone;
		m_vecBoneMatrix[i] = matBone;
	}


	//size_t iRegisteredCount = m_vecRegisteredBoneTransform.size();
 	//for (size_t i = 0; i < iRegisteredCount; ++i)
	//{
	//	CTransform* pTransform = m_vecRegisteredBoneTransform[i].pTransform;
	//	int iIndex = m_vecRegisteredBoneTransform[i].iBoneIndex;
	//
	//	//Vector3 vLocalPosition = pTransform->GetLocalPosition();
	//	//Vector3 vLocalRotation = pTransform->GetLocalRotation();
	//	//Vector3 vLocalScale = pTransform->GetLocalScale(); 
	//	//XMVECTOR vZero = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	//	
	//	//Matrix matLocal = XMMatrixAffineTransformation(vLocalScale.Convert(), vLocalRotation.Convert(), vZero, vLocalPosition.Convert());		
	//	Matrix matWorld = m_pTransform->GetWorldMatrix() * m_vecBoneMatrix[iIndex];
	//	pTransform->SetWorldMatrix(matWorld);
	//}
}

void CAnimator::BlendKeyFrame2D(const KEYFRAMEBUFFER2D& vecBoneSrc)
{
	if (m_pMeshRenderer)
	{
		CMaterial* pMaterial = m_pMeshRenderer->GetMaterial();
		CTexture* pTexture = pMaterial->GetDiffuseTexture();

		if (!vecBoneSrc.bEmpty)
		{
			if (pTexture != vecBoneSrc.pTexture)
			{
				pMaterial->SetDiffuseTexture(vecBoneSrc.pTexture);
			}
		}	

		SAFE_RELEASE(pTexture);
		SAFE_RELEASE(pMaterial);
	}

	if (m_pSpriteRenderer)
	{
		CMaterial* pMaterial = m_pSpriteRenderer->GetMaterial();
		CTexture* pTexture = pMaterial->GetDiffuseTexture();

		if (!vecBoneSrc.bEmpty)
		{
			if (pTexture != vecBoneSrc.pTexture)
			{
				pMaterial->SetDiffuseTexture(vecBoneSrc.pTexture);
			}
		}

		SAFE_RELEASE(pTexture);
		SAFE_RELEASE(pMaterial);
	}

	if (vecBoneSrc.pAtlasData)
	{
		m_tClip2DInfo.fX = vecBoneSrc.pAtlasData->wX;
		m_tClip2DInfo.fY = vecBoneSrc.pAtlasData->wY;
		m_tClip2DInfo.fWidth = vecBoneSrc.pAtlasData->wWidth;
		m_tClip2DInfo.fHeght = vecBoneSrc.pAtlasData->wHeight;
		m_tClip2DInfo.fAtlasWidth = vecBoneSrc.pAtlasData->iAtlasWidth;
		m_tClip2DInfo.fAtlasHeight = vecBoneSrc.pAtlasData->iAtlasHeight;
	}
}

bool CAnimator::SetBoneTransformFromReservation()
{
	m_vecBoneTransform.clear();

	size_t iReservationCount = m_vecReservedBoneTransform.size();
	for (size_t i = 0; i < iReservationCount; ++i)
	{
		m_vecBoneTransform.push_back(m_pTransform->GetChild(m_vecReservedBoneTransform[i]));
	}

	m_vecReservedBoneTransform.clear();

	return true;
}

vector<BONETRANSFORM>* CAnimator::GetRegisteredBoneObject()
{
	return &m_vecRegisteredBoneTransform;
}

void CAnimator::RegisterSocket(size_t iBoneIndex, CTransform * pTransform)
{
	if (m_pMeshRenderer && pTransform)
	{
		CMesh* pMesh = m_pMeshRenderer->GetMesh();
		if (pMesh)
		{
			if (iBoneIndex >= 0 && iBoneIndex < pMesh->GetBoneCount())
			{
				BONETRANSFORM tTransform;
				tTransform.iBoneIndex = iBoneIndex;
				tTransform.pTransform = pTransform;

				m_vecRegisteredBoneTransform.push_back(tTransform);
			}
		}
	}
	
}

void CAnimator::RegisterSocket(const string & strBoneName, CTransform * pTransform)
{
	size_t iBoneIndex = GetBoneIndex(strBoneName);
	RegisterSocket(iBoneIndex, pTransform);
}

int CAnimator::GetBoneIndex(const string & strBoneName)
{
	if (m_pMeshRenderer)
	{
		CMesh* pMesh = m_pMeshRenderer->GetMesh();
		if (pMesh)
		{
			size_t iBoneCount = pMesh->GetBoneCount();
			for (size_t i = 0; i < iBoneCount; ++i)
			{
				if (pMesh->GetBone()->at(i)->strName == strBoneName)
				{
					return i;
				}
			}
		}
	}
	return -1;
}

string CAnimator::GetBoneName(size_t iBoneIndex)
{
	if (m_pMeshRenderer)
	{
		CMesh* pMesh = m_pMeshRenderer->GetMesh();
		if (pMesh)
		{
			if (iBoneIndex >= 0 && iBoneIndex < pMesh->GetBoneCount())
			{
				return pMesh->GetBone()->at(iBoneIndex)->strName;
			}
		}
	}
	return string();
}

Matrix CAnimator::GetBoneMatrix(size_t iBoneIndex) const
{
	if (iBoneIndex < m_vecBoneMatrix.size())
	{
		if (m_pMeshRenderer)
		{
			CMesh* pMesh = m_pMeshRenderer->GetMesh();
			Matrix matOffset = *pMesh->GetBone()->at(iBoneIndex)->matOffset;
			return matOffset.Inverse() * m_vecBoneMatrix[iBoneIndex].Convert() * m_pTransform->GetWorldMatrix();
		}
		else
		{
			assert(false);
		}
	}
	else
	{
		//Out of Range;
		assert(false);
	}
}

Matrix CAnimator::GetBoneMatrix(const string & strBoneName)
{
	int iIndex = GetBoneIndex(strBoneName);
	if (iIndex > -1)
	{
		return GetBoneMatrix(iIndex);
	}
	else
	{
		assert(false);
	}
}

vector<class CTransform*>* CAnimator::GetBoneTransform()
{
	return &m_vecBoneTransform;
}
CMeshRenderer * CAnimator::GetMeshRenderer() const
{
	return m_pMeshRenderer;
}
CSpriteRenderer * CAnimator::GetSpriteRenderer() const
{
	return m_pSpriteRenderer;
}
void CAnimator::SetTargetRenderer(CSpriteRenderer * pRenderer)
{
	m_pSpriteRenderer = pRenderer;
}
void CAnimator::SetTargetRenderer(CMeshRenderer * pRenderer)
{
	m_pMeshRenderer = pRenderer;
}
const Vector3 & CAnimator::GetLocalScaleFactor() const
{
	return m_vLocalScaleFactor;
}
const Vector3& CAnimator::GetFixedScaleFactor() const
{
	return m_vScaleFactor;
}
const Vector3& CAnimator::GetOriginScale() const
{
	return m_vOriginScale;
}
void CAnimator::SetFixedScaleFactor(const Vector3& vFactor)
{
	m_vScaleFactor = vFactor;
}
void CAnimator::SetLocalScaleFactor(const Vector3 & vFactor)
{
	m_vLocalScaleFactor = vFactor;
}
bool CAnimator::GetSyncScale() const
{
	return m_bSyncScale;
}
void CAnimator::SetSyncScale(bool bUseSyncScale)
{
	m_bSyncScale = bUseSyncScale;
}

void CAnimator::SetAnimationStateTransitioned()
{
	m_bAnimationStateTransitioned = true;
}
void CAnimator::SetAnimationController(CAnimationStateController * pController)
{
	m_pAnimationController = pController;
	pController->m_pAnimator = this;
}

void CAnimator::SetEntryState(const string & strStateName)
{
	if (m_pAnimationController)
	{
		m_pAnimationController->SetEntryState(strStateName);
	}
}

class CAnimationState* CAnimator::CreateAnimationStateInCurrentController(const string & strStateName)
{
	if (m_pAnimationController)
	{
		return m_pAnimationController->CreateAnimationState(strStateName);
	}	

	return NULL;
}

CAnimationStateController* CAnimator::CreateAnimationController()
{
	CAnimationStateController* pController = new CAnimationStateController(this);
	SetAnimationController(pController);

	return pController;
}

CAnimationStateController * CAnimator::GetAnimationController() const
{
	return m_pAnimationController;
}

bool CAnimator::CreateBoneObject()
{
	if (m_pMeshRenderer)
	{
		if (m_pMeshRenderer->GetMesh())
		{
			if (m_pMeshRenderer->GetMesh()->GetBoneCount() > 0)
			{
				size_t iBoneCount = m_pMeshRenderer->GetMesh()->GetBoneCount();
				for (size_t i = 0; i < iBoneCount; ++i)
				{
					PBONE pBone = m_pMeshRenderer->GetMesh()->GetBone()->at(i);

					string strName = pBone->strName;
					Matrix matBone = *pBone->matBone;
				
					CGameObject* pBoneObject = CGameObject::CreateObject(strName, m_pLayer);
					//CMeshRenderer* pRenderer = pBoneObject->AddComponent<CMeshRenderer>();
					//pRenderer->SetMesh(PRIMITIVE_MESH_SPHERE, true);
					//SAFE_RELEASE(pRenderer);

					CTransform* pBoneTransform = pBoneObject->GetTransform();
					m_vecBoneTransform.push_back(pBoneTransform);
					pBoneTransform->SetParent(m_pTransform);
					m_vecBoneTransform[i]->SetWorldMatrix(matBone * m_pTransform->GetWorldMatrix());
					pBoneTransform->DoNotUpdateInProcedural(); 
					SAFE_RELEASE(pBoneTransform);
					SAFE_RELEASE(pBoneObject);
				}

				size_t iBoneObjectCount = m_vecBoneTransform.size();
				for (size_t i = 0; i < iBoneObjectCount; ++i)
				{
					PBONE pBone = m_pMeshRenderer->GetMesh()->GetBone()->at(i);
					CTransform* pTransform = m_vecBoneTransform[i];
					if (pBone->iParentIndex >= 0)
					{
						if (i != pBone->iParentIndex)
						{
							CTransform* pParentTransform = m_vecBoneTransform[pBone->iParentIndex];
							pTransform->SetParent(pParentTransform);
						}
					}
				}
			}
		}
	}

	return true;
}

bool CAnimator::CreateBoneTexture()
{
	size_t iBoneCount = 0;

	if (!m_pMeshRenderer)
		return false;
	
	if (!m_pMeshRenderer->GetMesh())
		return false;
	
	iBoneCount = m_pMeshRenderer->GetMesh()->GetBone()->size();
	m_vecBoneMatrix.resize(iBoneCount);
	
	//if (m_pMeshRenderer->GetMesh()->GetBone()->size() > 0)
	//{
		SAFE_RELEASE(m_pBoneTexture);
		D3D11_TEXTURE2D_DESC	tDesc = {};
		tDesc.ArraySize = 1;
		tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tDesc.Usage = D3D11_USAGE_DYNAMIC;
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		tDesc.Height = 2;
		tDesc.Width = (iBoneCount + 1) * 4;
		tDesc.MipLevels = 1;
		tDesc.SampleDesc.Quality = 0;
		tDesc.SampleDesc.Count = 1;

		if (FAILED(DEVICE->CreateTexture2D(&tDesc, NULL, &m_pBoneTexture)))
			return false;

		if (FAILED(DEVICE->CreateShaderResourceView(m_pBoneTexture, NULL, &m_pBoneTextureRV)))
			return false;

		return true;
	//}	

	return false;
}

ID3D11Texture2D * CAnimator::GetBoneTexture() const
{
	return m_pBoneTexture;
}

ID3D11ShaderResourceView * CAnimator::GetBoneTextureResourceView() const
{
	return m_pBoneTextureRV;
}

void CAnimator::Reset()
{
	m_pAnimationController->Reset();
}

bool CAnimator::Initialize()
{
	CTransform* pTransform = m_pGameObject->GetTransform();
	m_vOriginScale = pTransform->GetWorldScale();
	SAFE_RELEASE(pTransform);

	CMeshRenderer* pMeshRenderer = m_pGameObject->GetComponent<CMeshRenderer>();
	CSpriteRenderer* pSpriteRenderer = m_pGameObject->GetComponent<CSpriteRenderer>();
	if (pMeshRenderer)
	{
		m_pMeshRenderer = pMeshRenderer;
		if (m_pMeshRenderer->GetShader()->GetKey() == STANDARD_BUMP_SHADER)
		{
			m_pMeshRenderer->SetShader(STANDARD_BUMP_ANIM_SHADER);
		}
		else if (m_pMeshRenderer->GetShader()->GetKey() == STANDARD_TEXTURE_NORMAL_SHADER)
		{
			m_pMeshRenderer->SetShader(STANDARD_TEXTURE_NORMAL_ANIM_SHADER);
		}
		CreateBoneTexture();
	}

	if (pSpriteRenderer)
	{
		m_pSpriteRenderer = pSpriteRenderer;
	}

	SAFE_RELEASE(pMeshRenderer);
	SAFE_RELEASE(pSpriteRenderer);
	
	return true;
}

void CAnimator::Start()
{
	m_pAnimationController->Start();

	size_t iBoneTransformCount = m_vecBoneTransform.size();
	for (size_t i = 0; i < iBoneTransformCount; ++i)
	{
		CGameObject* pGameObject = m_vecBoneTransform[i]->GetGameObject();
		size_t iComponentCount = pGameObject->GetComponentCount();
		SAFE_RELEASE(pGameObject);

		if (iComponentCount > 0)
		{
			BONETRANSFORM tTransform;
			tTransform.iBoneIndex = i;
			tTransform.pTransform = m_vecBoneTransform[i];
			m_vecRegisteredBoneTransform.push_back(tTransform);
		}
	}

	for (size_t i = 0; i < iBoneTransformCount; ++i)
	{
		m_vecBoneTransform[i]->SetParent(NULL);
	}

	vector<CTransform*>::iterator iter;
	vector<CTransform*>::iterator iterEnd = m_vecBoneTransform.end();
	for (iter = m_vecBoneTransform.begin() ;iter != iterEnd;)
	{
		CGameObject* pGameObject =(*iter)->GetGameObject();
		size_t iComponentCount = pGameObject->GetComponentCount();
		SAFE_RELEASE(pGameObject);

		if (iComponentCount == 0)
		{
			pGameObject->DestroyImmediate();
			iter = m_vecBoneTransform.erase(iter);
			iterEnd = m_vecBoneTransform.end();
		}
		else
		{
			++iter;
		}
	}
}

int CAnimator::Prerender(CMeshRenderer * pRenderer)
{
	if (m_pMeshRenderer)
	{
		D3D11_MAPPED_SUBRESOURCE	tMap = {};
		DEVICE_CONTEXT->Map(m_pBoneTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &tMap);
		if (m_vecBoneMatrix.size() > 0)
		{
			memcpy(tMap.pData, &m_vecBoneMatrix[0], sizeof(Matrix) * m_vecBoneMatrix.size());
		}
		DEVICE_CONTEXT->Unmap(m_pBoneTexture, 0);


		DEVICE_CONTEXT->VSSetShaderResources(7, 1, &m_pBoneTextureRV); 

		if (m_pMeshRenderer->GetAnimationCondition() & AC_2D)
		{
			GET_SINGLE(CShaderManager)->UpdateConstantBuffer("Animation2D", &m_tClip2DInfo, CBT_PIXEL);
		}
	}

	if (m_pSpriteRenderer)
	{
		if (m_pSpriteRenderer->GetAnimationCondition() & AC_2D)
		{
			GET_SINGLE(CShaderManager)->UpdateConstantBuffer("Animation2D", &m_tClip2DInfo, CBT_PIXEL);
		}
	}
	
	return 0;
}

int CAnimator::Update(float fTime)
{	
	if (!m_pAnimationController)
		return 0;
		
	m_pAnimationController->Update(fTime);
	
	vector<KEYFRAMEBUFFER> pCurrentKeyFrame;
	vector<KEYFRAMEBUFFER> pTargetKeyFrame;
	KEYFRAMEBUFFER2D tKeyFrameBuffer2D;

	float fLerpingFactor = 0.5f;
	//Animtion은 두 가지 상태만 존재 할 수 있다.
	//Transition 중일 때와, Transition중이 아닐 때.
	CAnimationState* pCurrentState = m_pAnimationController->GetCurrentState();
	if (pCurrentState)
	{
		if (pCurrentState == m_pAnimationController->GetAnimationState("Entry State"))
		{
			return 0;
		}

		if (pCurrentState->m_bOnTransition)
		{
			CAnimationState* pTargetState = pCurrentState->m_pReservedTransition->m_pTargetState;
			fLerpingFactor = pCurrentState->m_pReservedTransition->GetTransitionProgress();

			//Transition 중 일 때에는 예약된 Transition의 설정에 따라 해당 State의 키 프레임을 가져오도록 한다.
			float fExitTime = pCurrentState->m_pReservedTransition->m_fExitTime;
			float fExitDuration = pCurrentState->m_pReservedTransition->m_fExitDuration;

			CAnimationClip* pCurrentClip = pCurrentState->GetClip();
			CAnimationClip* pTargetClip = pTargetState->GetClip();
			pCurrentKeyFrame = pCurrentClip->GetKeyFrame(pCurrentState->m_pReservedTransition->m_fExitTime);
			pTargetKeyFrame = pTargetClip->GetKeyFrame(pTargetClip->GetProgressAtTime(fExitDuration));
			BlendKeyFrame(pCurrentKeyFrame, pTargetKeyFrame, fLerpingFactor);

			tKeyFrameBuffer2D = pCurrentState->GetClip()->GetCurrentKeyFrame2D();
			BlendKeyFrame2D(tKeyFrameBuffer2D);
		}
		else
		{
			pCurrentKeyFrame = pCurrentState->GetClip()->GetCurrentKeyFrame();
			pTargetKeyFrame = pCurrentState->GetClip()->GetNextKeyFrame();
			BlendKeyFrame(pCurrentKeyFrame, pTargetKeyFrame, pCurrentState->GetClip()->GetProgressToNextFrame());

			tKeyFrameBuffer2D = pCurrentState->GetClip()->GetCurrentKeyFrame2D();
			BlendKeyFrame2D(tKeyFrameBuffer2D);
		}
	}

	return 0;
}

bool CAnimator::Save(FILE * pFile)
{
	size_t iBoneTransformCount = m_vecBoneTransform.size();

	fwrite(&iBoneTransformCount, sizeof(size_t), 1, pFile);
	for (size_t i = 0; i < iBoneTransformCount; ++i)
	{
		CGameObject* pGameObject = m_vecBoneTransform[i]->GetGameObject();
		string strBoneTransformObjectName = pGameObject->GetTag();
		SAFE_RELEASE(pGameObject);

		size_t iLength = strBoneTransformObjectName.length();
		fwrite(&iLength, sizeof(size_t), 1, pFile);
		fwrite(strBoneTransformObjectName.c_str(), sizeof(char), iLength, pFile);
	}


	m_pAnimationController->Save(pFile);

	return true;
}

bool CAnimator::Load(FILE * pFile)
{ 
	size_t iBoneTransformCount = 0;
	fread(&iBoneTransformCount, sizeof(size_t), 1, pFile);
	for (size_t i = 0; i < iBoneTransformCount; ++i)
	{
		size_t iLength = 0;
		char strReservation[256] = {};
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(strReservation, sizeof(char), iLength, pFile);
		m_vecReservedBoneTransform.push_back(strReservation);
	}

	m_pAnimationController = new CAnimationStateController(this);
	m_pAnimationController->Load(pFile);
	return true;
}

