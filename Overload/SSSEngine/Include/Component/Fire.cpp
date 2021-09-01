#include "Fire.h"
#include "MeshRenderer.h"
#include "../GameObject.h"
#include "../ShaderManager.h"
#include "../Timer.h"
#include "../TimerManager.h"
#include "../ResourcesManager.h"
#include "../Sampler.h"
#include "Billboard.h"
#include "../Material.h"
#include "../Transform.h"
#include "Animator.h"

SSS_USING

CFire::CFire()
{
	memset(&m_tCBuffer, 0, sizeof(FIRECBUFFER));
	m_fActiveTime = 0.f;
	m_fFireSpeed = 0.1f;
	m_fActiveMaxTime = 1.f;
	m_bOnceDestroy = false;
}


CFire::~CFire()
{
}

void CFire::SetScale(const Vector3 & vScale)
{
	m_tCBuffer.vScales = vScale;
}

void CFire::SetSpeed(const Vector3 & vSpeed)
{
	m_vScrollSpeed = vSpeed;
}

void CFire::SetDistortion1(const Vector2 & vDistord)
{
	m_tCBuffer.vDistortion1 = vDistord;
}

void CFire::SetDistortion2(const Vector2 & vDistord)
{
	m_tCBuffer.vDistortion2 = vDistord;
}

void CFire::SetDistortion3(const Vector2 & vDistord)
{
	m_tCBuffer.vDistortion3 = vDistord;
}

void CFire::SetDistortionScale(float fScale)
{
	m_tCBuffer.fDistortionScale = fScale;
}

void CFire::SetBias(float fBias)
{
	m_tCBuffer.fDistortionBias = fBias;
}

void CFire::SetOffsetY(float fOffsetY)
{
	m_tCBuffer.fOffsetY = fOffsetY;
}

float CFire::GetDistortionScale() const
{
	return m_tCBuffer.fDistortionScale;
}

bool CFire::GetDestroyOption() const
{
	return m_bOnceDestroy;
}

void CFire::SetDestroyOption(bool bOption)
{
	m_bOnceDestroy = bOption;
}

float CFire::GetActiveMaxTime() const
{
	return m_fActiveMaxTime;
}

void CFire::SetActiveMaxTime(float fActiveMaxTime)
{
	m_fActiveMaxTime = fActiveMaxTime;
}

bool CFire::Initialize()
{
	CMeshRenderer* pRenderer = GetComponent<CMeshRenderer>();

	// Render Group 지정
	m_pGameObject->SetTransparency(true);

	// MeshRenderer 가 없을경우 자동으로 추가후 셋팅
	if (!pRenderer)
	{
		pRenderer = m_pGameObject->AddComponent<CMeshRenderer>();

		CBillboard* pBillboard = GetComponent<CBillboard>();
		// 현재 이 컴포넌트에 Billboard 기능이 있을경우 거기에 맞춰서 셋팅한당
		if (pBillboard)
		{
			pRenderer->SetMesh(ENGINE_MESH_BILLBOARD, true);
			pRenderer->SetShader("FireBillBoardShader");
			pRenderer->SetInputLayout(POS_UV_LAYOUT);
			pRenderer->SetRenderState(ALPHA_BLEND);
			pRenderer->SetRenderState(CULLNONE);

			SAFE_RELEASE(pBillboard);
		}
		else
		{
			pRenderer->SetMesh("FireMesh", true);
			pRenderer->SetShader("FireShader");
			pRenderer->SetInputLayout(POS_UV_LAYOUT);

			pRenderer->SetRenderState(ALPHA_BLEND);
			pRenderer->SetRenderState(CULLNONE);
		}

		CMaterial* pMaterial = pRenderer->GetMaterial();
		// Sampler WRAP 
		pMaterial->SetSampler("LINEAR_WRAP");
		// Fire Tex (원본 불의 Diffuse Texture 입니다)
		// Normal Texture에는 왜곡을 적용할 Noise Tex 가 들어갑니다
		// Alpha Texture에는 왜곡된 이미지를 걸러내줄 Alpha Texure 가 들어갑니다
		SAFE_RELEASE(pMaterial);
		SAFE_RELEASE(pRenderer);
	}
	// 있으면 그냥 가져온것으로 셋팅
	else
	{
		CBillboard* pBillboard = GetComponent<CBillboard>();
		// 현재 이 컴포넌트에 Billboard 기능이 있을경우 거기에 맞춰서 셋팅한당
		if (pBillboard)
		{
			pRenderer->SetMesh(ENGINE_MESH_BILLBOARD);
			pRenderer->SetShader("FireBillBoardShader");
			pRenderer->SetInputLayout(POS_UV_LAYOUT);
			pRenderer->SetRenderState(ALPHA_BLEND);
			pRenderer->SetRenderState(CULLNONE);
			SAFE_RELEASE(pBillboard);
		}
		else
		{
			pRenderer->SetMesh("FireMesh");
			pRenderer->SetShader("FireShader");
			pRenderer->SetInputLayout(POS_UV_LAYOUT);

			pRenderer->SetRenderState(ALPHA_BLEND);
			pRenderer->SetRenderState(CULLNONE);
		}

		CMaterial* pMaterial = pRenderer->GetMaterial();
		// Sampler WRAP 
		pMaterial->SetSampler("LINEAR_WRAP");
		// Fire Tex (원본 불의 Diffuse Texture 입니다)
		// Normal Texture에는 왜곡을 적용할 Noise Tex 가 들어갑니다
		// Alpha Texture에는 왜곡된 이미지를 걸러내줄 Alpha Texure 가 들어갑니다
		SAFE_RELEASE(pMaterial);

		SAFE_RELEASE(pRenderer);
	}


	m_fActiveTime = 0.f;
	m_tCBuffer.fFrameTime = 0.f;

	m_vScrollSpeed = Vector3(1.3f, 2.1f, 2.3f);
	m_tCBuffer.vScrollSpeeds = Vector3(1.3f, 2.1f, 2.3f);
	m_tCBuffer.vScales = Vector3(0.1f, 0.2f, 0.3f);

	m_tCBuffer.vDistortion1 = Vector2(0.1f, 0.2f);
	m_tCBuffer.vDistortion2 = Vector2(0.1f, 0.3f);
	m_tCBuffer.vDistortion3 = Vector2(0.1f, 0.1f);
	m_tCBuffer.fDistortionScale = 0.8f;

	m_tCBuffer.fDistortionBias = 0.f;
	m_tCBuffer.fOffsetY = 0.f;

	return true;
}

int CFire::LateUpdate(float fTime)
{
	m_fActiveTime += fTime;
	float fProgress = m_fActiveTime / m_fActiveMaxTime;
	if (fProgress >= 1.f)
	{
		if (m_bOnceDestroy)
			m_pGameObject->Destroy(true);
		m_fActiveTime = 0.0f;
	}

	Vector3 vScrollSpeed = m_vScrollSpeed * fProgress * m_fFireSpeed;
	m_tCBuffer.vScrollSpeeds.x = vScrollSpeed.x;
	m_tCBuffer.vScrollSpeeds.y = vScrollSpeed.y;
	m_tCBuffer.vScrollSpeeds.z = vScrollSpeed.z;

	int iRand = rand() % 10;
	m_tCBuffer.fFrameTime = (float)iRand * 0.01f;

	return 0;
}

int CFire::Prerender(CMeshRenderer * pRenderer)
{
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer("FireCBuffer", &m_tCBuffer, CBT_VERTEX | CBT_GEOMETRY | CBT_PIXEL);

	// Distortion 용 Clamp 샘플러 
	CSampler* pSampler = GET_SINGLE(CResourcesManager)->FindSampler("LINEAR_CLAMP");
	pSampler->SetSampler(1);
	SAFE_RELEASE(pSampler);

	return 0;
}

bool CFire::Save(FILE * pFile)
{
	fwrite(&m_tCBuffer, sizeof(FIRECBUFFER), 1, pFile);
	fwrite(&m_fFireSpeed, sizeof(float), 1, pFile);

	return true;
}

bool CFire::Load(FILE * pFile)
{
	fread(&m_tCBuffer, sizeof(FIRECBUFFER), 1, pFile);
	fread(&m_fFireSpeed, sizeof(float), 1, pFile);

	return true;
}
