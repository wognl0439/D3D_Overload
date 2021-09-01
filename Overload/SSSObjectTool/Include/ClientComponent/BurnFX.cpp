#include "BurnFX.h"
#include "Component/MeshRenderer.h"
#include "Material.h"
#include "Engine.h"
#include "ResourcesManager.h"
#include "ShaderManager.h"

CBurnFX::CBurnFX():
	m_fTimer(0.0f), 
	m_fTimeLimit(5.0f)
{
}

CBurnFX::~CBurnFX()
{
}

void CBurnFX::SetBorderColor(const Vector4 & vColor)
{
	m_tRendererCBuffer.vFXBurnColor = vColor;
}

void CBurnFX::SetBorderRangePercentage(float fPercentage)
{
	m_tRendererCBuffer.fFXBurnBorderRangePercentage = fPercentage;
}

void CBurnFX::SetTexture(const string & strKey, const wchar_t * pFileName)
{
	CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();

	CMaterial* pMaterial = pRenderer->GetMaterial();
	pMaterial->SetDiffuseMultiTexture("BurnNoiseTex", strKey, pFileName);
	pMaterial->SetDiffuseMultiTextureRegister("BurnNoiseTex", 9);

	SAFE_RELEASE(pMaterial);
	SAFE_RELEASE(pRenderer);
}

bool CBurnFX::Initialize()
{
	memset(&m_tRendererCBuffer, 0, sizeof(FXBURN));

	//CBuffer는 10번 Register
	//Texture는 9번 Register
	CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();
	pRenderer->SetShader("BurnShader");

	CMaterial* pMaterial = pRenderer->GetMaterial();
	pMaterial->SetDiffuseMultiTexture("BurnNoiseTex", "NoiseTexture.png", L"Noise.png");
	pMaterial->SetDiffuseMultiTextureRegister("BurnNoiseTex", 9);

	SAFE_RELEASE(pMaterial);
	SAFE_RELEASE(pRenderer);

	return true;
}

int CBurnFX::Update(float fTime)
{
	m_fTimer += fTime;

	m_fProgress = m_fTimer / m_fTimeLimit;
	if (m_fProgress >= 1.0f)
	{
		m_fProgress = 0.0f;
		m_fTimer = 0.0f;
	}

	m_tRendererCBuffer.fFXBurnProgress = m_fProgress;

	return 0;
}

void CBurnFX::Render(float fTime)
{
}

int CBurnFX::Prerender(CMeshRenderer * pRenderer)
{
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(FX_BURN_CBUFFER, &m_tRendererCBuffer, CBT_VERTEX | CBT_PIXEL);

	return 0;
}

bool CBurnFX::Save(FILE * pFile)
{
	fwrite(&m_tRendererCBuffer, sizeof(FXBURN), 1, pFile);
	fwrite(&m_fTimer, sizeof(float), 1, pFile);
	fwrite(&m_fTimeLimit, sizeof(float), 1, pFile);
	fwrite(&m_fProgress, sizeof(float), 1, pFile);

	return true;
}

bool CBurnFX::Load(FILE * pFile)
{
	fread(&m_tRendererCBuffer, sizeof(FXBURN), 1, pFile);
	fread(&m_fTimer, sizeof(float), 1, pFile);
	fread(&m_fTimeLimit, sizeof(float), 1, pFile);
	fread(&m_fProgress, sizeof(float), 1, pFile);

	return true;
}
