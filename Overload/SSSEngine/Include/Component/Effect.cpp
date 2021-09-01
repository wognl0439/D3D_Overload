#include "Effect.h"
#include "MeshRenderer.h"
#include "../ShaderManager.h"
#include "../ResourcesManager.h"
#include "../Material.h"

SSS_USING


CEffect::CEffect():
	m_bBackward(false)
{
}


CEffect::~CEffect()
{
}

void CEffect::ResetTimer()
{
	m_fTimer = 0.0f;
}

void CEffect::SetForward()
{
	m_bBackward = false;
}

void CEffect::SetBackward()
{
	m_bBackward = true;
}

void CEffect::SetFlags(unsigned int iFlags)
{
	m_tEffectCBuffer.iFlag = iFlags;

	if (m_tEffectCBuffer.iFlag & VF_BURN)
	{
		CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();
		if(pRenderer)
		{
			CMaterial* pMaterial = pRenderer->GetMaterial();
			if (pMaterial)
			{
				pMaterial->SetDiffuseMultiTexture("BurnNoiseTex", "NoiseTexture.png", L"Effects/Burn/Noise.png");
				pMaterial->SetDiffuseMultiTextureRegister("BurnNoiseTex", 9);
				SAFE_RELEASE(pMaterial);
			}
			SAFE_RELEASE(pRenderer);
		}
		else
		{
			CMeshRenderer* pMeshRenderer = m_pGameObject->AddComponent<CMeshRenderer>();

			CMaterial* pMaterial = pMeshRenderer->GetMaterial();
			if (pMaterial)
			{
				pMaterial->SetDiffuseMultiTexture("BurnNoiseTex", "NoiseTexture.png", L"Effects/Burn/Noise.png");
				pMaterial->SetDiffuseMultiTextureRegister("BurnNoiseTex", 9);
				SAFE_RELEASE(pMaterial);
			}

			SAFE_RELEASE(pRenderer);
		}
	}
}

uint32_t CEffect::GetFlags() const
{
	return m_tEffectCBuffer.iFlag;
}

void CEffect::SetLimitTime(float fTime)
{
	m_fTimeLimit = fTime;
}

float CEffect::GetLimitTime() const
{
	return m_fTimeLimit;
}

Vector4 CEffect::GetBorderColor() const
{
	return m_tEffectCBuffer.vFXBurnColor;
}

float CEffect::GetBorderRangePercentage() const
{
	return m_tEffectCBuffer.fFXBurnBorderRangePercentage;
}

void CEffect::SetLimColor(Vector3 & vColor)
{
	m_tEffectCBuffer.vFXLimColor = vColor;
}

void CEffect::SetLimPower(float fPower)
{
	m_tEffectCBuffer.vFXLimPower = fPower;
}

Vector3 CEffect::GetLimColor() const
{
	return m_tEffectCBuffer.vFXLimColor;
}

float CEffect::GetLimPower() const
{
	return m_tEffectCBuffer.vFXLimPower;
}

float CEffect::GetProgress() const
{
	return m_fProgress;
}

void CEffect::SetBorderColor(const Vector4 & vColor)
{
	m_tEffectCBuffer.vFXBurnColor = vColor;
}

void CEffect::SetBorderRangePercentage(float fPercentage)
{
	m_tEffectCBuffer.fFXBurnBorderRangePercentage = fPercentage;
}

void CEffect::SetTexture(const string & strKey, const wchar_t * pFileName)
{
	CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();
	if (pRenderer)
	{
		CMaterial* pMaterial = pRenderer->GetMaterial();
		pMaterial->SetDiffuseMultiTexture("BurnNoiseTex", strKey, pFileName);
		pMaterial->SetDiffuseMultiTextureRegister("BurnNoiseTex", 9);

		SAFE_RELEASE(pMaterial);
		SAFE_RELEASE(pRenderer);
	}
}

bool CEffect::Initialize()
{
	m_tEffectCBuffer = {};

	m_tEffectCBuffer.fFXBurnProgress = 0.f;
	m_tEffectCBuffer.fFXBurnBorderRangePercentage = 0.f;
	m_tEffectCBuffer.vFXBurnPadding = Vector4::clear;
	m_tEffectCBuffer.vFXBurnColor = Vector4::clear;

	m_fTimer = 0.f;
	m_fTimeLimit = 5.f;

	m_tEffectCBuffer.iFlag = VF_LIMLIGHT;

	m_tEffectCBuffer.vFXLimColor = Vector3(1.f, 1.f, 1.f);
	m_tEffectCBuffer.vFXLimPower = 3.f;

	return true;
}

int CEffect::Update(float fTime)
{
	if (m_tEffectCBuffer.iFlag & VF_BURN)
	{
		m_fTimer += fTime;
		if (m_bBackward)
		{
			m_fProgress = 1.0f - (m_fTimer / m_fTimeLimit);
			if (m_fProgress <= 0.0f)
			{
				m_fProgress = 0.0f;
			}

			m_tEffectCBuffer.fFXBurnProgress = m_fProgress;
		}
		else
		{
			m_fProgress = m_fTimer / m_fTimeLimit;
			if (m_fProgress >= 1.0f)
			{
				m_fProgress = 1.0f;
			}

			m_tEffectCBuffer.fFXBurnProgress = m_fProgress;
		}
	}

	return 0;
}

int CEffect::Prerender(CMeshRenderer * pRenderer)
{
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(Effect_CBUFFER, &m_tEffectCBuffer, CBT_PIXEL);

	return 0;
}

int CEffect::Postrender(CMeshRenderer * pRenderer)
{
	EFFECT tNULL = {};
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(Effect_CBUFFER, &tNULL, CBT_PIXEL);

	return 0;
}

bool CEffect::Save(FILE * pFile)
{
	fwrite(&m_tEffectCBuffer, sizeof(EFFECT), 1, pFile);
	fwrite(&m_fTimer, sizeof(float), 1, pFile);
	fwrite(&m_fTimeLimit, sizeof(float), 1, pFile);
	fwrite(&m_fProgress, sizeof(float), 1, pFile);

	return true;
}

bool CEffect::Load(FILE * pFile)
{
	fread(&m_tEffectCBuffer, sizeof(EFFECT), 1, pFile);
	fread(&m_fTimer, sizeof(float), 1, pFile);
	fread(&m_fTimeLimit, sizeof(float), 1, pFile);
	fread(&m_fProgress, sizeof(float), 1, pFile);

	return true;
}
