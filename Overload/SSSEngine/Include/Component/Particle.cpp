#include "Particle.h"
#include "../Shader.h"
#include "../ShaderManager.h"
#include "../RenderManager.h"
#include "../ResourcesManager.h"
#include "../RenderState.h"
#include "../Texture.h"
#include "../Mesh.h"
#include "../Transform.h"
#include "Camera.h"
#include "../Scene.h"
#include "../SceneManager.h"
#include "../Sampler.h"
#include "../Device.h"

SSS_USING

CParticle::CParticle():
	m_pMesh(NULL),
	m_pLayout(NULL),
	m_pTexture(NULL),
	m_pSampler(NULL),
	m_pAlphaBlend(NULL),
	m_eEmissionShape(PES_CONE)
{
	m_bOnEmission = false;
	m_fEnableTimeLapse = 0;
	m_bLoop = true;
	m_bRigidBody = false;
	m_bGravity = false;
		
	m_iCountFromStart = 0;
	m_iMaxCount = 0;
	m_iCurrentCount = 0;
}


CParticle::~CParticle()
{
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pAlphaBlend);
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pSampler);
}

bool CParticle::Initialize()
{
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(ENGINE_MESH_PARTICLE);
	m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(PARTICLE_LAYOUT);
	m_pAlphaBlend = GET_SINGLE(CRenderManager)->FindRenderState(ALPHA_BLEND);
	m_pShader = GET_SINGLE(CShaderManager)->FindShader("Particle");
	m_pSampler = GET_SINGLE(CResourcesManager)->FindSampler(SAMPLER_LINEAR);
	

	if (!m_pMesh || !m_pLayout || !m_pAlphaBlend)
		return false;

	if (m_pMesh)
	{
		size_t iVertexCount = m_pMesh->GetVertexCount();		
		m_vecParticleInfo.resize(iVertexCount);
		m_vecVertex.resize(iVertexCount);
		m_iMaxCount = iVertexCount;
	}

	if (m_pAlphaBlend)
	{
		m_pGameObject->SetTransparency(true);
	}

	return true;
}

int CParticle::Update(float fTime)
{
	Vector3 vPosition = m_pTransform->GetWorldPosition();
	Vector3 vForward = m_pTransform->forward();
	Vector3 vRight = m_pTransform->right();
	Vector3 vUp = m_pTransform->up();

	m_fEnableTimeLapse += fTime;
	bool	bStartFlag = false;
	bool	bEndFlag = false;

	if (!m_bOnEmission)
	{
		if (m_fStartDelay - m_fEnableTimeLapse <= 0.0f)
		{
			m_fEnableTimeLapse = 0;
			m_bOnEmission = true;
		}
	}

	else
	{
		bStartFlag = true;
		float fGenerationRate = 1.0f / m_fRateOverTime;
		if (m_bLoop)
		{
			if (m_iCurrentCount < m_iMaxCount)
			{
				if (m_fEnableTimeLapse >= fGenerationRate)
				{
					++m_iCurrentCount;
					m_fEnableTimeLapse -= fGenerationRate;
					//Create
					int index = GetEmptyIndex();

					PParticleInfo pParticle = &m_vecParticleInfo[index];
					pParticle->bEnabled = true;
					pParticle->fAlpha = RandomRange(m_vAlphaRange.x, m_vAlphaRange.y);
					pParticle->fLifetime = RandomRange(m_vLifetimeRange.x, m_vLifetimeRange.y);
					pParticle->vPos = vPosition + Vector3::RandomDirection() * (RandomRange(-1.0f, 1.0f) * m_fRadius);
					pParticle->vScale = Vector2(RandomRange(m_vMinSize.x, m_vMaxSize.x), RandomRange(m_vMinSize.y, m_vMaxSize.y));


					if (m_bRigidBody)
					{
						//Set Force
					}
					else
					{
						pParticle->fSpeed = RandomRange(m_vSpeedRange.x, m_vSpeedRange.y);
						pParticle->fTime = 0;

						if (m_eEmissionShape == PES_CONE)
						{
							//원 평면상의 랜덤한 위치를 대상으로 생성 지점에서의 상대적인 방향벡터를 구한다.
							float fFarRadius = (10000.0f / tanf(m_fAngle * D2R));
							Vector3 vTargetPosition = (vUp * 10000.0f) + vRight * (RandomRange(-1.0f, 1.0f) * fFarRadius);
							pParticle->vDir = (vTargetPosition - pParticle->vPos).Normalize();
						}
						else if (m_eEmissionShape == PES_SPHERE)
						{
							pParticle->vDir = Vector3::RandomDirection();
						}
						else
						{
							pParticle->vDir = Vector3::RandomDirection();
						}
					}
				}
			}
		}
		else
		{
			if (m_iCountFromStart < m_iMaxCount)
			{
				if (m_fEnableTimeLapse >= fGenerationRate)
				{
					++m_iCountFromStart;
					m_fEnableTimeLapse -= fGenerationRate;

					//Create
					int index = GetEmptyIndex();
					PParticleInfo pParticle = &m_vecParticleInfo[index];
					pParticle->bEnabled = true;
					pParticle->fAlpha = RandomRange(m_vAlphaRange.x, m_vAlphaRange.y);
					pParticle->fLifetime = RandomRange(m_vLifetimeRange.x, m_vLifetimeRange.y);
					pParticle->vPos = vPosition + vRight * (RandomRange(-1.0f, 1.0f) * m_fRadius);
					pParticle->vScale = Vector2(RandomRange(m_vMinSize.x, m_vMaxSize.x), RandomRange(m_vMinSize.y, m_vMaxSize.y));

					if (m_bRigidBody)
					{
						//Set Force
					}
					else
					{
						pParticle->fSpeed = RandomRange(m_vSpeedRange.x, m_vSpeedRange.y);
						pParticle->fTime = 0;

						if (m_eEmissionShape == PES_CONE)
						{
							//원 평면상의 랜덤한 위치를 대상으로 생성 지점에서의 상대적인 방향벡터를 구한다.
							float fFarRadius = (10000.0f / tanf(m_fAngle * D2R));
							Vector3 vTargetPosition = (vUp * 10000.0f) + vRight * (RandomRange(-1.0f, 1.0f) * fFarRadius);
							pParticle->vDir = (vTargetPosition - pParticle->vPos).Normalize();
						}
						else if (m_eEmissionShape == PES_SPHERE)
						{
							pParticle->vDir = Vector3::RandomDirection();
						}
						else
						{
							pParticle->vDir = Vector3::RandomDirection();
						}
					}
				}
			}
			else
			{
				bEndFlag = true;
			}
		}
		
	}
	
	size_t iParticleCount = m_vecParticleInfo.size();
	m_vecVertex.clear();
	m_vecVertex.resize(m_vecParticleInfo.size());

	size_t iIndex = 0;
	bool bEnd = false;
	for (size_t i = 0; i < iParticleCount; ++i)
	{
		if (m_vecParticleInfo[i].bEnabled)
		{
			m_vecParticleInfo[i].fTime += fTime;

			if (m_vecParticleInfo[i].fTime >= m_vecParticleInfo[i].fLifetime)
			{
				m_vecParticleInfo[i].Unuse();
				--m_iCurrentCount;
				continue;
			}


			m_vecParticleInfo[i].vPos += m_vecParticleInfo[i].vDir * m_vecParticleInfo[i].fSpeed * fTime;

			VERTEXPARTICLE tVertex = {};
			tVertex.iEnabled = 1;
			tVertex.vPosition = m_vecParticleInfo[i].vPos;
			tVertex.vScale = Vector2(m_vecParticleInfo[i].vScale.x, m_vecParticleInfo[i].vScale.y);
			tVertex.fAlpha = m_vecParticleInfo[i].fAlpha;

			m_vecVertex[iIndex] = tVertex;
			++iIndex;
		}

		bEnd |= m_vecParticleInfo[i].bEnabled;
	}	

	if (!bEnd && bStartFlag && !m_bLoop && bEndFlag)
	{
		m_pGameObject->Destroy(true);
	}

	sort(m_vecVertex.begin(), m_vecVertex.end(), CParticle::SortParticle);

	return 0;
}

void CParticle::Render(float fTime)
{
	UpdateTransform();

	BILLBOARDCBUFFER	tBuffer = {};
	tBuffer.vCenter = m_pTransform->GetWorldPosition();
	tBuffer.fSizeX = m_pTransform->GetWorldScale().x;
	tBuffer.fSizeY = m_pTransform->GetWorldScale().y;

	CTransform*	pCameraTr = m_pScene->GetMainCameraTransform();
	tBuffer.vAxisX = pCameraTr->right();
	tBuffer.vAxisY = pCameraTr->up();

	SAFE_RELEASE(pCameraTr);

	m_pAlphaBlend->SetState();

	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(BILLBOARD_CBUFFER, &tBuffer, CBT_VERTEX | CBT_GEOMETRY);

	m_pTexture->SetTexture(0);
	m_pSampler->SetSampler(0);

	m_pMesh->UpdateVertexBuffer(&m_vecVertex[0], m_vecVertex.size());

	m_pShader->SetShader();

	DEVICE_CONTEXT->IASetInputLayout(m_pLayout);

	m_pMesh->Render();

	m_pAlphaBlend->ResetState();
}

void CParticle::SetMass(float fMin, float fMax)
{
	m_vMassRange.x = fMin;
	m_vMassRange.y = fMax;
}

void CParticle::SetForce(float fMin, float fMax)
{
	m_vForceRange.x = fMin;
	m_vForceRange.y = fMax;
}

int CParticle::GetEmptyIndex() const
{
	size_t iSize = m_vecParticleInfo.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		if (!m_vecParticleInfo[i].bEnabled)
		{
			return i;
		}
	}

	return -1;
}

void CParticle::UpdateTransform()
{
	TRANSFORMCBUFFER	tBuffer;

	// Camera를 얻어온다.
	CCamera*	pCamera = m_pScene->GetMainCamera();

	tBuffer.matWorld = m_pTransform->GetLocalMatrix() * m_pTransform->GetWorldMatrix();
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProjection = pCamera->GetPerspectiveProjectionMatrix();
	tBuffer.matWV = tBuffer.matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
	tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
	tBuffer.matWP = tBuffer.matWorld * tBuffer.matProjection;
	tBuffer.matInvProjection = Matrix::Inverse(tBuffer.matProjection);
	tBuffer.matInvVP = Matrix::Inverse(tBuffer.matVP);
	tBuffer.matInvWVP = Matrix::Inverse(tBuffer.matWVP);
	tBuffer.vPivot = m_pTransform->GetPivot();
	tBuffer.iRenderMode = GET_SINGLE(CRenderManager)->GetRenderMode() ? 1 : 0;
	tBuffer.vLength = m_pMesh->GetLength();

	//tBuffer.matShadowView = pCamera->GetShadowViewMatrix();
	//tBuffer.matShadowProj = pCamera->GetShadowProjMatrix();
	//tBuffer.matShadowVP = tBuffer.matShadowView * tBuffer.matShadowProj;
	//tBuffer.matShadowWVP = tBuffer.matWorld * tBuffer.matShadowVP;

	SAFE_RELEASE(pCamera);

	tBuffer.matWorld = tBuffer.matWorld.Transpose();
	tBuffer.matView = tBuffer.matView.Transpose();
	tBuffer.matProjection = tBuffer.matProjection.Transpose();
	tBuffer.matWV = tBuffer.matWV.Transpose();
	tBuffer.matWVP = tBuffer.matWVP.Transpose();
	tBuffer.matVP = tBuffer.matVP.Transpose();
	tBuffer.matWP = tBuffer.matWP.Transpose();
	tBuffer.matInvProjection = tBuffer.matInvProjection.Transpose();
	tBuffer.matInvVP = tBuffer.matInvVP.Transpose();
	tBuffer.matInvWVP = tBuffer.matInvWVP.Transpose();

	//tBuffer.matShadowView = tBuffer.matShadowView.Transpose();
	//tBuffer.matShadowProj = tBuffer.matShadowProj.Transpose();
	//tBuffer.matShadowVP = tBuffer.matShadowVP.Transpose();
	//tBuffer.matShadowWVP = tBuffer.matShadowWVP.Transpose();

	// Transform 정보를 Shader 상수버퍼로 넘겨준다.
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(TRANSFORM_CBUFFER, &tBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);
}


void CParticle::SetRateOverTime(float fRateOverTime)
{
	m_fRateOverTime = fRateOverTime;
}

void CParticle::SetTimeScale(float fTimeScale)
{
	m_fLocalTimeScale = fTimeScale;
}

void CParticle::SetStartDelay(float fDelay)
{
	m_fStartDelay = fDelay;
}

void CParticle::SetMaxCount(size_t iMax)
{
	if (iMax > PARTICLE_MAX)
		m_iMaxCount = PARTICLE_MAX;

	else
		m_iMaxCount = iMax;
}

void CParticle::SetEmissionShape(PARTICLE_EMISSION_SHAPE eShape)
{
	m_eEmissionShape = eShape;
}

void CParticle::SetGravity(bool bUse)
{
	m_bGravity = bUse;
}

void CParticle::SetRigidbody(bool bUse)
{
	m_bRigidBody = bUse;	
}

void CParticle::SetLoop(bool bLoop)
{
	m_bLoop = bLoop;
}

void CParticle::SetSize(const Vector2 & vMin, const Vector2 & vMax)
{
	m_vMinSize = vMin;
	m_vMaxSize = vMax;
}

void CParticle::SetSpeed(float fMin, float fMax)
{
	m_vSpeedRange.x = fMin;
	m_vSpeedRange.y = fMax;
}

void CParticle::SetLifetime(float fMin, float fMax)
{
	m_vLifetimeRange.x = fMin;
	m_vLifetimeRange.y = fMax;
}

void CParticle::SetAlpha(float fMin, float fMax)
{
	m_vAlphaRange.x = fMin;
	m_vAlphaRange.y = fMax;
}

void CParticle::SetAngle(float fAngle)
{
	m_fAngle = fAngle;
}

void CParticle::SetRadius(float fRadius)
{
	m_fRadius = fRadius;
}


void CParticle::SetTexture(const string & strKey, const wchar_t * pFileName, const string & strPathKey)
{

	SAFE_RELEASE(m_pTexture);

	m_pTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, pFileName, strPathKey);

	if (!m_pTexture)
	{
		//Texture가 없음.
	}
}

void CParticle::SetDiffuseColor(const Vector4 & vColor)
{
	m_vDiffuse = vColor;
}

void CParticle::SetEmissiveColor(const Vector4 & vColor)
{
	m_vEmissive = vColor;
}

bool CParticle::SortParticle(VERTEXPARTICLE & tSrc, VERTEXPARTICLE & tDest)
{
	if (tSrc.iEnabled == 0 && tDest.iEnabled == 0)
	{
		tSrc.Unuse();
		tDest.Unuse();
		return false;
	}
	else if (tSrc.iEnabled != 0 && tDest.iEnabled == 0)
	{
		tDest.Unuse();
		return false;
	}
	else if (tSrc.iEnabled == 0 && tDest.iEnabled != 0)
	{
		tSrc.Unuse();
		return true;
	}
	else
	{
		CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
		CCamera*	pCamera = pScene->GetMainCamera();
		Matrix	matView = pCamera->GetViewMatrix();

		SAFE_RELEASE(pCamera);
		SAFE_RELEASE(pScene);

		// 정점 위치들을 뷰공간으로 변환한다.
		Vector3	vSrc = tSrc.vPosition.TransformCoord(matView);
		Vector3	vDest = tDest.vPosition.TransformCoord(matView);
		return vSrc.z > vDest.z;
	}

	

}
