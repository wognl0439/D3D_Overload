#include "AnimationClip.h"
#include "AnimationState.h"
#include "Component\Animator.h"
#include "Component\MeshRenderer.h"
#include "FBXLoader.h"
#include "Mesh.h"
#include "Device.h"
#include "ResourcesManager.h"
#include "Texture.h"
#include "Material.h"
#include "Component\SpriteRenderer.h"
#include "DataManager.h"
#include "ShaderManager.h"
#include "Transform.h"
#include "FBXLoader.h"
#include "Mathf.h"

SSS_USING

CAnimationClip::CAnimationClip():
	m_pAnimationState(NULL),
	m_pAnimationController(NULL),
	m_pAnimator(NULL),
	m_fTime(0),
	m_iFrameRate(60),
	m_iTotalFrameCount(0),
	m_iCurrentFrameIndex(0),
	m_fProgress(0),
	m_bOnExit(false)
{
	m_tClipInfo.iEndFrame = 0;
	m_tClipInfo.iStartFrame = 0;
	m_tClipInfo.iLength = 0;
	m_tClipInfo.strName = "";	
}


CAnimationClip::CAnimationClip(const CAnimationClip & clip)
{
}

CAnimationClip::~CAnimationClip()
{
	size_t iBoneCount = m_vecKeyFrame.size();
	for (size_t i = 0; i < iBoneCount; ++i)
	{
		Safe_Delete_VecList(m_vecKeyFrame[i]->vecKeyFrame);
	}
	Safe_Delete_VecList(m_vecKeyFrame);
	Safe_Delete_VecList(m_vecKeyFrame2D);
}

void CAnimationClip::ComputeKeyFrame2D()
{
	size_t iFrameCount = m_vecKeyFrame2D.size();

	PANIMATIONCLIP2D pPrevClipInfo = new ANIMATIONCLIP2D;
	 
	pPrevClipInfo->pTexture = m_vecKeyFrame2D[0]->pTexture;
	pPrevClipInfo->strAtlasKey = m_vecKeyFrame2D[0]->strAtlasKey;

	for (size_t i = 0; i < iFrameCount; ++i)
	{
		if (m_vecKeyFrame2D[i]->pTexture == NULL)
		{
			m_vecKeyFrame2D[i]->pTexture = pPrevClipInfo->pTexture;
		}
		else
		{
			if (m_vecKeyFrame2D[i]->pTexture != pPrevClipInfo->pTexture)
			{
				pPrevClipInfo->pTexture = m_vecKeyFrame2D[i]->pTexture;
			}
		}


		if (m_vecKeyFrame2D[i]->strAtlasKey.empty())
		{
			m_vecKeyFrame2D[i]->strAtlasKey = pPrevClipInfo->strAtlasKey;
		}
		else
		{
			if (m_vecKeyFrame2D[i]->strAtlasKey != pPrevClipInfo->strAtlasKey)
			{
				pPrevClipInfo->strAtlasKey = m_vecKeyFrame2D[i]->strAtlasKey;
			}
		}	
	}

	delete pPrevClipInfo;
}

float CAnimationClip::GetIntervalToNextFrame()
{
	//한줄로 축약 가능
	float fUnitInterval = 1.0f / (float)m_iFrameRate;
	float fTimeForNextFrame = (float)GetNextKeyFrameIndex() * fUnitInterval;
	return fTimeForNextFrame - m_fTime;
}

float CAnimationClip::GetProgressToNextFrame()
{
	//한줄로 축약 가능
	float fUnitInterval = 1.0f / (float)m_iFrameRate;
	float fTimeForCurrentFrame = (float)GetKeyFrameIndex(m_fProgress) * fUnitInterval;	
	return CMathf::Clamp((m_fTime - fTimeForCurrentFrame) / fUnitInterval, 0.0f, 1.0f);
}

void CAnimationClip::SetProgressOffset(float fProgress)
{
	float fTotalTime = (float)m_tClipInfo.iLength / (float)m_iFrameRate;
	m_fProgress = fProgress;
	m_iCurrentFrameIndex = GetKeyFrameIndex(m_fProgress);
	m_fTime = fTotalTime * m_fProgress;
}

float CAnimationClip::GetProgressAtTime(float fTime) const
{
	float fTotalTime = (float)m_tClipInfo.iLength / (float)m_iFrameRate;
	float fProgress = 0;
	if (m_pAnimationState->IsLoop())
	{
		fProgress = fTime / fTotalTime;
		while (fProgress > 1.0f)
		{
			fProgress -= 1.0f;
		}
		return CMathf::Clamp(fProgress, 0.0f, 1.0f);
	}
	else
	{
		if (fTime <= fTotalTime)
		{
			return CMathf::Clamp(fTime / fTotalTime, 0.0f, 1.0f);
		}
		else
		{
			return 1.0f;
		}
	}
}

int CAnimationClip::GetCurrentKeyFrameIndex()
{
	return GetKeyFrameIndex(m_fProgress);
}

int CAnimationClip::GetNextKeyFrameIndex()
{
	float fUnitProgressPerFrame = (float)m_iCurrentFrameIndex / ((float)m_tClipInfo.iLength - 1);	
	return GetKeyFrameIndex(fUnitProgressPerFrame * (float)m_iCurrentFrameIndex);
}

int CAnimationClip::GetKeyFrameIndex(float fProgress)
{
	while (fProgress > 1.0f)
	{
		m_fProgress -= 1.0f;
	}

	return (int)(fProgress *((int)m_tClipInfo.iLength - 1));
}

/*
- m_bLoop가 True 일 경우 마지막 프레임을 초과하는 인덱스는 회전 한 값을 반환.
- m_bLoop가 False 일 경우 마지막 프레임을 초과하는 인덱스는 1.0f을 반환.
*/
float CAnimationClip::GetProgressAtIndex(int iIndex) const
{
	int idx = iIndex;
	if (m_pAnimationState->IsLoop())
	{
		while (idx > (int)m_tClipInfo.iLength - 1)
		{
			idx -= m_tClipInfo.iLength;
		}
		return CMathf::Clamp((float)idx / (float)((int)m_tClipInfo.iLength - 1), 0.0f, 1.0f);
	}
	else
	{
		if (idx > (int)m_tClipInfo.iLength - 1)
		{
			return 1.0f;
		}
		else
		{
			return CMathf::Clamp((float)idx / (float)((int)m_tClipInfo.iLength - 1), 0.0f, 1.0f);
		}
	}
}

vector<KEYFRAMEBUFFER>& CAnimationClip::GetNextKeyFrame()
{
	int iIndex = GetKeyFrameIndex(m_fProgress);
	++iIndex;
	
	return GetKeyFrame(GetProgressAtIndex(iIndex));
}

vector<KEYFRAMEBUFFER>& CAnimationClip::GetCurrentKeyFrame()
{
	return GetKeyFrame(m_fProgress);
}

vector<KEYFRAMEBUFFER>& CAnimationClip::GetKeyFrame(float fProgress)
{
	int iIndex = GetKeyFrameIndex(fProgress);

	m_vecKeyFrameBuffer.clear();

	size_t iBoneCount = m_vecKeyFrame.size();
	for (size_t i = 0; i < iBoneCount; ++i)
	{ 
		KEYFRAMEBUFFER tKeyFrame;
		tKeyFrame.bEmpty = false;
		tKeyFrame.iBoneIndex = m_vecKeyFrame[i]->iBoneIndex;
		if (iIndex >= 0 && m_vecKeyFrame[i]->vecKeyFrame.size() > iIndex)
		{
			tKeyFrame.pKeyFrame = m_vecKeyFrame[i]->vecKeyFrame[iIndex];
		}
		else
		{
			tKeyFrame.pKeyFrame = NULL;
		}
		m_vecKeyFrameBuffer.push_back(tKeyFrame);
	}

	//Animation2D 정보는 0번 인덱스에 넣는다.

	return m_vecKeyFrameBuffer;
}

KEYFRAMEBUFFER2D CAnimationClip::GetCurrentKeyFrame2D()
{
	KEYFRAMEBUFFER2D tInfo;

	int iIndex = GetCurrentKeyFrameIndex();

	if (iIndex >= 0 && iIndex < (int)m_vecKeyFrame2D.size())
	{
		tInfo.bEmpty = false;
		tInfo.pAtlasData = GET_SINGLE(CDataManager)->GetAtlasData(m_vecKeyFrame2D[iIndex]->strAtlasKey);
		tInfo.pTexture = m_vecKeyFrame2D[iIndex]->pTexture;
		if (!tInfo.pTexture && !tInfo.pAtlasData)
		{
			tInfo.bEmpty = true;
		}
	}

	return tInfo;
}

KEYFRAMEBUFFER2D CAnimationClip::GetNextKeyFrame2D()
{
	KEYFRAMEBUFFER2D tInfo;

	int iIndex = GetNextKeyFrameIndex();

	if (iIndex >= 0 && iIndex < (int)m_vecKeyFrame2D.size())
	{
		tInfo.bEmpty = false;
		tInfo.pAtlasData = GET_SINGLE(CDataManager)->GetAtlasData(m_vecKeyFrame2D[iIndex]->strAtlasKey);
		tInfo.pTexture = m_vecKeyFrame2D[iIndex]->pTexture;
		if (!tInfo.pTexture && !tInfo.pAtlasData)
		{
			tInfo.bEmpty = true;
		}
	}

	return tInfo;
}

size_t CAnimationClip::GetFrameRate() const
{
	return m_iFrameRate;
}

float CAnimationClip::GetProgress() const
{
	return m_fProgress;
}

const string & CAnimationClip::GetClipName() const
{
	return m_tClipInfo.strName;
}

size_t CAnimationClip::GetClipLength() const
{
	return m_iTotalFrameCount;
}

void CAnimationClip::SetKeyFrame(size_t iFrameIndex, const string & strAtlasMetadataKey, const string & strTextureKey, wchar_t * pFileName, const string & strPathKey)
{
	CMeshRenderer* pMeshRenderer = m_pAnimator->GetMeshRenderer();
	if (pMeshRenderer)
	{
		pMeshRenderer->SetAnimationCondition(AC_2D);
	}

	CSpriteRenderer* pSpriteRenderer = m_pAnimator->GetSpriteRenderer();
	if (pSpriteRenderer)
	{
		pSpriteRenderer->SetAnimationCondition(AC_2D);
	}

	size_t iFrameCount = m_vecKeyFrame2D.size();

	if ((int)iFrameIndex > ((int)iFrameCount) - 1)
	{
		for (size_t i = iFrameCount; i <= iFrameIndex; ++i)
		{
			PANIMATIONCLIP2D pClip = new ANIMATIONCLIP2D;
			pClip->pTexture = NULL;
			pClip->strAtlasKey = "";
			m_vecKeyFrame2D.push_back(pClip);
		}
	}

	ComputeKeyFrame2D();
	CTexture* pTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strTextureKey, pFileName, strPathKey);
	if (pTexture)
	{
		m_vecKeyFrame2D[iFrameIndex]->pTexture = pTexture;
	}
	else
	{
		m_vecKeyFrame2D[iFrameIndex]->pTexture = NULL;
	}
	SAFE_RELEASE(pTexture);

	if (!strAtlasMetadataKey.empty())
	{
		PATLASDATA pData = GET_SINGLE(CDataManager)->GetAtlasData(strAtlasMetadataKey);
		if (pData)
		{
			m_vecKeyFrame2D[iFrameIndex]->strAtlasKey = strAtlasMetadataKey;
		}
		else
		{
			pData = NULL;
		}
	}
	else
	{
		m_vecKeyFrame2D[iFrameIndex]->strAtlasKey = "";
	}

	ComputeKeyFrame2D();

	if (m_vecKeyFrame.size() > m_vecKeyFrame2D.size())
	{
		m_tClipInfo.iLength = m_vecKeyFrame.size();
		m_tClipInfo.iEndFrame = m_vecKeyFrame.size() - 1;
	}
	else
	{
		m_tClipInfo.iLength = m_vecKeyFrame2D.size();
		m_tClipInfo.iEndFrame = m_vecKeyFrame2D.size() - 1;
	}
}

void CAnimationClip::SetKeyFrame(size_t iFrameIndex, const string & strAtlasMetadataKey, const string & strTextureKey)
{
	CMeshRenderer* pMeshRenderer = m_pAnimator->GetMeshRenderer();
	if (pMeshRenderer)
	{
		pMeshRenderer->SetAnimationCondition(AC_2D);
	}

	CSpriteRenderer* pSpriteRenderer = m_pAnimator->GetSpriteRenderer();
	if (pSpriteRenderer)
	{
		pSpriteRenderer->SetAnimationCondition(AC_2D);
	}

	size_t iFrameCount = m_vecKeyFrame2D.size();

	if ((int)iFrameIndex > ((int)iFrameCount) - 1)
	{
		for (size_t i = iFrameCount; i < iFrameIndex; ++i)
		{
			PANIMATIONCLIP2D pClip = new ANIMATIONCLIP2D;
			pClip->pTexture = NULL;
			pClip->strAtlasKey = "";
			m_vecKeyFrame2D.push_back(pClip);
		}
	}

	ComputeKeyFrame2D();
	CTexture* pTexture = GET_SINGLE(CResourcesManager)->FindTexture(strTextureKey);
	if (pTexture)
	{
		m_vecKeyFrame2D[iFrameIndex]->pTexture = pTexture;
	}
	else
	{
		m_vecKeyFrame2D[iFrameIndex]->pTexture = NULL;
	}
	SAFE_RELEASE(pTexture);

	if (!strAtlasMetadataKey.empty())
	{
		PATLASDATA pData = GET_SINGLE(CDataManager)->GetAtlasData(strAtlasMetadataKey);
		if (pData)
		{
			m_vecKeyFrame2D[iFrameIndex]->strAtlasKey = strAtlasMetadataKey;
		}
		else
		{
			pData = NULL;
		}
	}
	else
	{
		m_vecKeyFrame2D[iFrameIndex]->strAtlasKey = "";
	}

	ComputeKeyFrame2D();

	if (m_vecKeyFrame.size() > m_vecKeyFrame2D.size())
	{
		m_tClipInfo.iLength = m_vecKeyFrame.size();
		m_tClipInfo.iEndFrame = m_vecKeyFrame.size() - 1;
	}
	else
	{
		m_tClipInfo.iLength = m_vecKeyFrame2D.size();
		m_tClipInfo.iEndFrame = m_vecKeyFrame2D.size() - 1;
	}
}

void CAnimationClip::SetKeyFrame(size_t iFrameIndex, const string & strAtlasMetadataKey)
{
	CMeshRenderer* pMeshRenderer = m_pAnimator->GetMeshRenderer();
	if (pMeshRenderer)
	{
		pMeshRenderer->SetAnimationCondition(AC_2D);
	}

	CSpriteRenderer* pSpriteRenderer = m_pAnimator->GetSpriteRenderer();
	if (pSpriteRenderer)
	{
		pSpriteRenderer->SetAnimationCondition(AC_2D);
	}


	size_t iFrameCount = m_vecKeyFrame2D.size();

	if ((int)iFrameIndex > ((int)iFrameCount) - 1)
	{
		for (size_t i = iFrameCount; i < iFrameIndex; ++i)
		{
			PANIMATIONCLIP2D pClip = new ANIMATIONCLIP2D;
			pClip->pTexture = NULL;
			pClip->strAtlasKey = "";
			m_vecKeyFrame2D.push_back(pClip);
		}
	}

	ComputeKeyFrame2D();

	if (iFrameIndex == 0)
	{
		m_vecKeyFrame2D[iFrameIndex]->pTexture = NULL;
	}
	else
	{

		CTexture* pTexture = m_vecKeyFrame2D[iFrameIndex - 1]->pTexture;

		if (pTexture)
		{
			PATLASDATA pData = GET_SINGLE(CDataManager)->GetAtlasData(strAtlasMetadataKey);
			if (pData)
			{
				m_vecKeyFrame2D[iFrameIndex]->strAtlasKey = strAtlasMetadataKey;
			}
			else
			{
				pData = NULL;
			}
		}
		else
		{
			m_vecKeyFrame2D[iFrameIndex]->pTexture = NULL;
		}

		SAFE_RELEASE(pTexture);

		if (!strAtlasMetadataKey.empty())
		{
			m_vecKeyFrame2D[iFrameIndex]->strAtlasKey = strAtlasMetadataKey;
		}
		else
		{
			m_vecKeyFrame2D[iFrameIndex]->strAtlasKey = "";
		}
	}
	ComputeKeyFrame2D();

	if (m_vecKeyFrame.size() > m_vecKeyFrame2D.size())
	{
		m_tClipInfo.iLength = m_vecKeyFrame.size();
		m_tClipInfo.iEndFrame = m_vecKeyFrame.size() - 1;
	}
	else
	{
		m_tClipInfo.iLength = m_vecKeyFrame2D.size();
		m_tClipInfo.iEndFrame = m_vecKeyFrame2D.size() - 1;
	}
}

void CAnimationClip::SetFrameRate(size_t iFrameRate)
{
	m_iFrameRate = iFrameRate;
}

void CAnimationClip::SetFrameRate(FbxTime::EMode eMode)
{
	switch (eMode)
	{
	case fbxsdk::FbxTime::eFrames120:
		m_iFrameRate = 120;
		break;
	case fbxsdk::FbxTime::eFrames100:
		m_iFrameRate = 100;
		break;
	case fbxsdk::FbxTime::eFrames60:
		m_iFrameRate = 60;
		break;
	case fbxsdk::FbxTime::eFrames50:
		m_iFrameRate = 50;
		break;
	case fbxsdk::FbxTime::eFrames48:
		m_iFrameRate = 48;
		break;
	case fbxsdk::FbxTime::eFrames30:
		m_iFrameRate = 30;
		break;
	default:
		m_iFrameRate = 60;
		break;
	}
}

void CAnimationClip::SetKeyFrame(PANIMATIONCLIP pClipInfo)
{
	ClearClip();

	if (m_pAnimator->GetMeshRenderer())
	{
		CMesh* pMesh = m_pAnimator->GetMeshRenderer()->GetMesh();
		if (pMesh)
		{
			size_t iBoneCount = pMesh->GetBoneCount();

			m_tClipInfo.iStartFrame = pClipInfo->iStartFrame;
			m_tClipInfo.iEndFrame = pClipInfo->iEndFrame;
			m_tClipInfo.iLength = pClipInfo->iLength;
			m_tClipInfo.strName = pClipInfo->strName;

			vector<PBONEKEYFRAME>::const_iterator iter;
			vector<PBONEKEYFRAME>::const_iterator iterEnd = pMesh->GetAnimationClip()->end();

			//모든 Bone의 인수로 들어온 범위의 값만 추려서 보관한다.
			for (iter = pMesh->GetAnimationClip()->begin(); iter != iterEnd; ++iter)
			{
				if ((*iter)->vecKeyFrame.size() == 0)
				{
					PBONEKEYFRAME pClip = new BONEKEYFRAME;
					pClip->iBoneIndex = 0;
					m_vecKeyFrame.push_back(pClip);
					continue;
				}

				PBONEKEYFRAME pClip = new BONEKEYFRAME;
				pClip->iBoneIndex = (*iter)->iBoneIndex;

				for (size_t i = m_tClipInfo.iStartFrame; i <= m_tClipInfo.iEndFrame; ++i)
				{
					//Start 또는 End 의 인덱스 위치가 유효하지 않은 경우 자동으로 에러 밷어줌
					PKEYFRAME pKeyFrame = new KEYFRAME;
					pKeyFrame->dTime = (*iter)->vecKeyFrame[i]->dTime;
					pKeyFrame->vPos = (*iter)->vecKeyFrame[i]->vPos;
					pKeyFrame->vRot = (*iter)->vecKeyFrame[i]->vRot;
					pKeyFrame->vScale = (*iter)->vecKeyFrame[i]->vScale;
					pClip->vecKeyFrame.push_back(pKeyFrame);
				}

				m_vecKeyFrame.push_back(pClip);
			}
		}
	}

	for (size_t i = 0; i < m_tClipInfo.iLength; ++i)
	{
		PANIMATIONCLIP2D pClip2D = new ANIMATIONCLIP2D;
		pClip2D->pTexture = NULL;
		m_vecKeyFrame2D.push_back(pClip2D);
	}
}

void CAnimationClip::SetKeyFrame(const string & strClipName)
{
	CMeshRenderer* pMeshRenderer = m_pAnimator->GetMeshRenderer();
	if (pMeshRenderer)
	{
		pMeshRenderer->SetAnimationCondition(AC_3D);
	}


	if (m_pAnimator->GetMeshRenderer())
	{
		CMesh* pMesh = m_pAnimator->GetMeshRenderer()->GetMesh();
		if (pMesh)
		{	
			PANIMATIONCLIP pClipInfo	= pMesh->GetAnimationClipInfo(strClipName);

			if (!pClipInfo)
				return;
	
			ClearClip();

			m_tClipInfo.strName = pClipInfo->strName;
			m_tClipInfo.iStartFrame = pClipInfo->iStartFrame;
			m_tClipInfo.iEndFrame = pClipInfo->iEndFrame;
			m_tClipInfo.iLength = pClipInfo->iLength;

			size_t iBoneCount = pMesh->GetBoneCount();

			vector<PBONEKEYFRAME>::const_iterator iter;
			vector<PBONEKEYFRAME>::const_iterator iterEnd = pMesh->GetAnimationClip()->end();

			//모든 Bone의 인수로 들어온 범위의 값만 추려서 보관한다.

			for (iter = pMesh->GetAnimationClip()->begin(); iter != iterEnd; ++iter)
			{
				if ((*iter)->vecKeyFrame.size() == 0)
				{
					PBONEKEYFRAME pClip = new BONEKEYFRAME;
					pClip->iBoneIndex = 0;
					m_vecKeyFrame.push_back(pClip);
					continue;
				}

				PBONEKEYFRAME pClip = new BONEKEYFRAME;
				pClip->iBoneIndex = (*iter)->iBoneIndex;

				for (size_t i = m_tClipInfo.iStartFrame; i <= m_tClipInfo.iEndFrame; ++i)
				{
					//Start 또는 End 의 인덱스 위치가 유효하지 않은 경우 자동으로 에러 밷어줌
					PKEYFRAME pKeyFrame = new KEYFRAME;
					pKeyFrame->dTime = (*iter)->vecKeyFrame[i]->dTime;
					pKeyFrame->vPos = (*iter)->vecKeyFrame[i]->vPos;
					pKeyFrame->vRot = (*iter)->vecKeyFrame[i]->vRot;
					pKeyFrame->vScale = (*iter)->vecKeyFrame[i]->vScale;
					pClip->vecKeyFrame.push_back(pKeyFrame);
				}
				m_vecKeyFrame.push_back(pClip);
			}
		}
	}

	for (size_t i = 0; i < m_tClipInfo.iLength; ++i)
	{
		PANIMATIONCLIP2D pClip2D = new ANIMATIONCLIP2D;
		pClip2D->pTexture = NULL;
		m_vecKeyFrame2D.push_back(pClip2D);
	}
}

void CAnimationClip::SetKeyFrame(size_t iClipIndexFromMesh)
{
	CMeshRenderer* pMeshRenderer = m_pAnimator->GetMeshRenderer();
	if (pMeshRenderer)
	{
		pMeshRenderer->SetAnimationCondition(AC_3D);
	}

	if (m_pAnimator->GetMeshRenderer())
	{
		CMesh* pMesh = m_pAnimator->GetMeshRenderer()->GetMesh();
		if (pMesh)
		{
			PANIMATIONCLIP pClipInfo = pMesh->GetAnimationClipInfo(iClipIndexFromMesh);

			if (!pClipInfo)
				return;

			ClearClip();

			size_t iBoneCount = pMesh->GetBoneCount();

			m_tClipInfo.iStartFrame = pClipInfo->iStartFrame;
			m_tClipInfo.iEndFrame = pClipInfo->iEndFrame;
			m_tClipInfo.iLength = pClipInfo->iLength;
			m_tClipInfo.strName = pClipInfo->strName;

			vector<PBONEKEYFRAME>::const_iterator iter;
			vector<PBONEKEYFRAME>::const_iterator iterEnd = pMesh->GetAnimationClip()->end();

			//모든 Bone의 인수로 들어온 범위의 값만 추려서 보관한다.
			for (iter = pMesh->GetAnimationClip()->begin(); iter != iterEnd; ++iter)
			{
				if ((*iter)->vecKeyFrame.size() == 0)
				{
					PBONEKEYFRAME pClip = new BONEKEYFRAME;
					pClip->iBoneIndex = 0;
					m_vecKeyFrame.push_back(pClip);
					continue;
				}

				PBONEKEYFRAME pClip = new BONEKEYFRAME;
				pClip->iBoneIndex = (*iter)->iBoneIndex;

				for (size_t i = m_tClipInfo.iStartFrame; i <= m_tClipInfo.iEndFrame; ++i)
				{
					//Start 또는 End 의 인덱스 위치가 유효하지 않은 경우 자동으로 에러 밷어줌
					PKEYFRAME pKeyFrame = new KEYFRAME;
					pKeyFrame->dTime = (*iter)->vecKeyFrame[i]->dTime;
					pKeyFrame->vPos = (*iter)->vecKeyFrame[i]->vPos;
					pKeyFrame->vRot = (*iter)->vecKeyFrame[i]->vRot;
					pKeyFrame->vScale = (*iter)->vecKeyFrame[i]->vScale;
					pClip->vecKeyFrame.push_back(pKeyFrame);
				}

				m_vecKeyFrame.push_back(pClip);
			}
		}
	}

	for (size_t i = 0; i < m_tClipInfo.iLength; ++i)
	{
		PANIMATIONCLIP2D pClip2D = new ANIMATIONCLIP2D;
		pClip2D->pTexture = NULL;
		m_vecKeyFrame2D.push_back(pClip2D);
	}
}

void CAnimationClip::SetKeyFrame(const string& strClipName, size_t iStartFrame, size_t iEndFrame)
{
	CMeshRenderer* pMeshRenderer = m_pAnimator->GetMeshRenderer();
	if (pMeshRenderer)
	{
		pMeshRenderer->SetAnimationCondition(AC_3D);
	}

	m_tClipInfo.strName = strClipName;
	m_tClipInfo.iStartFrame = iStartFrame;
	m_tClipInfo.iEndFrame = iEndFrame;
	m_tClipInfo.iLength = iEndFrame - iStartFrame + 1;
	
	if (m_pAnimator->GetMeshRenderer())
	{
		CMesh* pMesh = m_pAnimator->GetMeshRenderer()->GetMesh();
		if (pMesh)
		{
			size_t iBoneCount = pMesh->GetBoneCount();

			vector<PBONEKEYFRAME>::const_iterator iter;
			vector<PBONEKEYFRAME>::const_iterator iterEnd = pMesh->GetAnimationClip()->end();

			//모든 Bone의 인수로 들어온 범위의 값만 추려서 보관한다.

			for (iter = pMesh->GetAnimationClip()->begin(); iter != iterEnd; ++iter)
			{
				if ((*iter)->vecKeyFrame.size() == 0)
				{
					PBONEKEYFRAME pClip = new BONEKEYFRAME;
					pClip->iBoneIndex = 0;
					m_vecKeyFrame.push_back(pClip);
					continue;
				}

				PBONEKEYFRAME pClip = new BONEKEYFRAME;
				pClip->iBoneIndex = (*iter)->iBoneIndex;

				for (size_t i = iStartFrame; i <= iEndFrame; ++i)
				{
					//Start 또는 End 의 인덱스 위치가 유효하지 않은 경우 자동으로 에러 밷어줌
					PKEYFRAME pKeyFrame = new KEYFRAME;
					pKeyFrame->dTime = (*iter)->vecKeyFrame[i]->dTime;
					pKeyFrame->vPos = (*iter)->vecKeyFrame[i]->vPos;
					pKeyFrame->vRot = (*iter)->vecKeyFrame[i]->vRot;
					pKeyFrame->vScale = (*iter)->vecKeyFrame[i]->vScale;
					pClip->vecKeyFrame.push_back(pKeyFrame);
				}
				m_vecKeyFrame.push_back(pClip);
			}
		}
	}
	


	for (size_t i = 0; i < m_tClipInfo.iLength; ++i)
	{
		PANIMATIONCLIP2D pClip2D = new ANIMATIONCLIP2D;
		pClip2D->pTexture = NULL;
		m_vecKeyFrame2D.push_back(pClip2D);
	}
}


void CAnimationClip::SetClipName(const string & strName)
{
	m_tClipInfo.strName = strName;
}

void CAnimationClip::ClearClip()
{
	m_tClipInfo.iStartFrame = 0;
	m_tClipInfo.iEndFrame = 0;
	m_tClipInfo.iLength = 0;
	m_tClipInfo.strName = "";

	size_t iBoneCount = m_vecKeyFrame.size();
	for (size_t i = 0; i < iBoneCount; ++i)
	{
		Safe_Delete_VecList(m_vecKeyFrame[i]->vecKeyFrame);
	}
	Safe_Delete_VecList(m_vecKeyFrame);

}

void CAnimationClip::ResetClip()
{
	m_fTime = 0;
	m_iCurrentFrameIndex = 0;
	m_fProgress = 0;
}

PKEYFRAME CAnimationClip::GetKeyFrame(int iBone, int iFrame) const
{
	assert(iBone >= 0 && iBone < m_vecKeyFrame.size());
	assert(iFrame >= 0 && iFrame < m_vecKeyFrame[iBone]->vecKeyFrame.size());	
	return m_vecKeyFrame[iBone]->vecKeyFrame[iFrame];
}

bool CAnimationClip::IsEmptyFrame(int iBone) const
{
	return m_vecKeyFrame[iBone]->vecKeyFrame.empty();
}

bool CAnimationClip::Update(float fTime)
{	
	m_fTime += fTime * m_pAnimationState->GetSpeed();

	float fTotalTime = (float)m_tClipInfo.iLength / (float)m_iFrameRate;
	if (m_pAnimationState->IsLoop())
	{
		while (m_fTime > fTotalTime)
		{
			m_fProgress -= 1.0f;
			m_fTime -= fTotalTime;
		}
	}
	else
	{
		if (m_fTime > fTotalTime)
		{
			m_fProgress = 1.0f;
			m_fTime = fTotalTime;
		}
	}
	

	m_fProgress = GetProgressAtTime(m_fTime);
	m_iCurrentFrameIndex = GetKeyFrameIndex(m_fProgress);
	

	return m_fProgress >= 1.0f;
}

void CAnimationClip::Save(FILE * pFile)
{
	fwrite(&m_iFrameRate, sizeof(size_t), 1, pFile);
	fwrite(&m_iTotalFrameCount, sizeof(size_t), 1, pFile);
	
	size_t iClipNameLength = m_tClipInfo.strName.length();
	fwrite(&iClipNameLength, sizeof(size_t), 1, pFile);
	fwrite(m_tClipInfo.strName.c_str(), sizeof(char), iClipNameLength, pFile);
	fwrite(&m_tClipInfo.iStartFrame, sizeof(size_t), 1, pFile);
	fwrite(&m_tClipInfo.iEndFrame, sizeof(size_t), 1, pFile);
	fwrite(&m_tClipInfo.iLength, sizeof(size_t), 1, pFile);
	
	//현재 Editor에서 2D Clip 편집이 지원되지 않기 때문에 저장 안함
	////vector 크기는 clipinfo.ilength에 있기 때문에 따로 저장하지 않음.
	//size_t iKeyFrameCount = m_vecKeyFrame2D.size();
	//for (size_t i = 0; i < iKeyFrameCount; ++i)
	//{

	//	m_vecKeyFrame2D[i]->pTexture->GetKey();
	//	m_vecKeyFrame2D[i]->pTexture->GetPathKey();
	//	m_vecKeyFrame2D[i]->pTexture->GetRelativePath();
	//	m_vecKeyFrame2D[i]->strAtlasKey;
	//}	
}

void CAnimationClip::Load(FILE * pFile)
{
	char strClipName[256] = {};
	fread(&m_iFrameRate, sizeof(size_t), 1, pFile);
	fread(&m_iTotalFrameCount, sizeof(size_t), 1, pFile);
	
	size_t iClipNameLength = 0;
	fread(&iClipNameLength, sizeof(size_t), 1, pFile);
	fread(strClipName, sizeof(char), iClipNameLength, pFile);
	fread(&m_tClipInfo.iStartFrame, sizeof(size_t), 1, pFile);
	fread(&m_tClipInfo.iEndFrame, sizeof(size_t), 1, pFile);
	fread(&m_tClipInfo.iLength, sizeof(size_t), 1, pFile);
	m_tClipInfo.strName = strClipName;

	SetKeyFrame(m_tClipInfo.strName, m_tClipInfo.iStartFrame, m_tClipInfo.iEndFrame);
}

