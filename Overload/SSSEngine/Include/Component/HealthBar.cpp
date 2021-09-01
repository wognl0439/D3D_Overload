#include "Camera.h"
#include "HealthBar.h"
#include "SpriteRenderer.h"
#include "MeshRenderer.h"
#include "../Mathf.h"
#include "../Scene.h"
#include "../Material.h"
#include "../Transform.h"
#include "../ShaderManager.h"
#include "../Device.h"

SSS_USING


CHealthBar::CHealthBar():
	m_fAlphaTimer(0.0f)
	, m_fDamagedTimer(0.0f)
	, m_fVisibleDistanceThreshold(0.0f)
	, m_pBaseObject(NULL)
	, m_bCurrentAggressive(false)
	, m_bPreviousAggressive(false)
	, m_eFadingPhase(HB_FADE_NONE)
	, m_eState(HBS_STABLE)
	, m_fFadingTimeThreshold(0.15f)
	, m_bCurrentInsideScreen(false)
	, m_bPreviousInsideScreen(false)
	, m_bCurrentInsideRange(false)
	, m_bPreviousInsideRange(false)
{
	memset(&m_tConstantBuffer, 0, sizeof(HEALTHBARCBUFFER));	
}


CHealthBar::~CHealthBar()
{
}

void CHealthBar::ComputeProgressColor()
{
	float fProgress = m_tConstantBuffer.fHealthPercentage;

	m_tConstantBuffer.vColor = Vector4::red;
	//m_tConstantBuffer.vColor.x = CMathf::Clamp(CMathf::Lerp(2.0f, 0.0f, fProgress), 0.0f, 1.0f);
	//m_tConstantBuffer.vColor.y = CMathf::Clamp(CMathf::Lerp(0.0f, 2.0f, fProgress), 0.0f, 1.0f);
	//m_tConstantBuffer.vColor.z = 0.0f;
	//m_tConstantBuffer.vColor.w = 1.0f;
}

void CHealthBar::SetForegroundTexture(const string & strKey, const wchar_t * pFileName, const string & strPathKey)
{
	CSpriteRenderer* pRenderer = GetComponent<CSpriteRenderer>();
	CMaterial* pMaterial = pRenderer->GetMaterial();
	pMaterial->SetNormalTexture(strKey, pFileName, 0, 0, strPathKey);
	pMaterial->SetSampler(SAMPLER_LINEAR);
	SAFE_RELEASE(pMaterial);
	SAFE_RELEASE(pRenderer);
}

void CHealthBar::SetBackgroundTexture(const string & strKey, const wchar_t * pFileName, const string & strPathKey)
{
	CSpriteRenderer* pRenderer = GetComponent<CSpriteRenderer>();
	CMaterial* pMaterial = pRenderer->GetMaterial();
	pMaterial->SetDiffuseTexture(strKey, pFileName, 0, 0, strPathKey);
	pMaterial->SetSampler(SAMPLER_LINEAR);
	SAFE_RELEASE(pMaterial);
	SAFE_RELEASE(pRenderer);
}

void CHealthBar::SetOffset(const Vector3 vOffset)
{
	m_vOffset = vOffset;
}

void CHealthBar::SetOffset(float fX, float fY, float fZ)
{
	m_vOffset.x = fX;
	m_vOffset.y = fY;
	m_vOffset.z = fZ;
}

void CHealthBar::SetVisibleDistanceThreshold(float fDistance)
{
	m_fVisibleDistanceThreshold = fDistance;
}

void CHealthBar::SetPlayerObject(CGameObject * pGameObject)
{
	m_pPlayerObject = pGameObject;
}

void CHealthBar::SetBaseObject(CGameObject * pGameObject)
{
	m_pBaseObject = pGameObject;
}

void CHealthBar::UpdateHealthBarInfo(bool bIsAggressive, float fCurrentHP, float fMaxHP, float fDeltaTime)
{
	m_bPreviousAggressive = m_bCurrentAggressive;
	m_bCurrentAggressive = bIsAggressive;
	m_bPreviousInsideScreen = m_bCurrentInsideScreen;
	m_bPreviousInsideRange = m_bCurrentInsideRange;

	if (!m_pBaseObject)
		return;

	Vector2 vResultPosition;
	{
		CTransform* pRenderTransform = m_pBaseObject->GetTransform();
		Vector3 vRenderPosition = pRenderTransform->GetWorldPosition();
		SAFE_RELEASE(pRenderTransform);

		CCamera* pCamera = m_pScene->GetMainCamera();
		vResultPosition = pCamera->WorldToScreenPoint(vRenderPosition + m_vOffset);
		SAFE_RELEASE(pCamera);

		if (vResultPosition == Vector2::Infinity)
		{
			m_bCurrentInsideScreen = false;
		}
		else
		{
			m_bCurrentInsideScreen = true;			
		}
	}
		

	//Base 와의 거리가 VisibleThresholdRange의 영역 내일 경우면서 Base Object에게 어그로가 튄 경우. 
	//영역 내 이면서 어그로가 안튀고, 체력이 100%일 경우는 표기하지 않음.
	//또는 피격을 당한 직후, 지정된 시간이 경과되지 않았을 경우(ex; 피격 후 무조건적으로 5초는 데미지를 보여줌)
	//생성/소멸 시 알파값 Fading

	if (m_pBaseObject)
	{
		CTransform* pBaseTransform = m_pBaseObject->GetTransform();
		CTransform* pPlayerTransform = m_pPlayerObject->GetTransform();
		Vector3 vPlayerPosition = pPlayerTransform->GetWorldPosition();
		Vector3 vBasePosition = pBaseTransform->GetWorldPosition();
		SAFE_RELEASE(pBaseTransform);
		SAFE_RELEASE(pPlayerTransform);

		float fDistance = (vPlayerPosition - vBasePosition).Length();
		m_bCurrentInsideRange = fDistance <= m_fVisibleDistanceThreshold;
		bool bEnter = !m_bPreviousAggressive && m_bCurrentAggressive;
		bool bStay = m_bPreviousAggressive && m_bCurrentAggressive;
		bool bExit = m_bPreviousAggressive && !m_bCurrentAggressive;
		float fHealth = fCurrentHP / fMaxHP;

		//월드 공간 상의 위치가 범위 내에 있는가
		bool bEnterRange = !m_bPreviousInsideRange && m_bCurrentInsideRange;
		bool bStayRange = m_bPreviousInsideRange && m_bCurrentInsideRange;
		bool bExitRange = m_bPreviousInsideRange && !m_bCurrentInsideRange;

		//화면 공간 상의 위치는 최종적으로 관여시키도록 한다.
		bool bEnterScreen = !m_bPreviousInsideScreen && m_bCurrentInsideScreen;
		bool bStayScreen = m_bPreviousInsideScreen && m_bCurrentInsideScreen;
		bool bExitScreen = m_bPreviousInsideScreen && !m_bCurrentInsideScreen;
		
		//순서는 Aggresive => Inside Range => Inside Screen 순서대로 체크
		if (bEnter)
		{
			if (bEnterRange)
			{
				//어그로 끌림과 동시에 범위 진입 시 보이기 시작 
				m_fAlphaTimer = 0.0f;
				m_fDamagedTimer = 0.0f;
				m_eState = HBS_START_VISIBLE; 
				m_eFadingPhase = HB_FADE_IN;
			}
			else if (bStayRange)
			{
				//어그로 끌리기 시작했을 때에 범위내에 존재할 경우 보이기 시작
				m_fAlphaTimer = 0.0f;
				m_fDamagedTimer = 0.0f;
				m_eState = HBS_START_VISIBLE;
				m_eFadingPhase = HB_FADE_IN;
			}
			else if (bExitRange)
			{
				//어그로 끌리기 시작했을 때에 범위밖에 존재할 경우 보이지 않음
				m_fAlphaTimer = 0.0f;
				m_fDamagedTimer = 0.0f;
				m_eState = HBS_EXIT_VISIBLE;
				m_eFadingPhase = HB_FADE_OUT;
			}
		}
		else if (bStay)
		{
			if (bEnterRange)
			{
				//전투 중 범위 내에 진입
				m_fAlphaTimer = 0.0f;
				m_fDamagedTimer = 0.0f;
				m_eState = HBS_START_VISIBLE;
				m_eFadingPhase = HB_FADE_IN;
			}
			else if (bStayRange)
			{
				//전투 중 범위 내에 이미 존재
				if (m_eFadingPhase == HB_FADE_NONE)
				{
					m_eState = HBS_STAY_VISIBLE;					
				}
			}
			else if (bExitRange)
			{
				//전투 중 범위 내에서 벗어남
				m_fAlphaTimer = 0.0f;
				m_fDamagedTimer = 0.0f;
				m_eState = HBS_EXIT_VISIBLE;
				m_eFadingPhase = HB_FADE_OUT;
			}
		}
		else if (bExit)
		{
			//전투 해제시에는 상태에 상관없이 끄도록 함.
			m_fAlphaTimer = 0.0f;
			m_fDamagedTimer = 0.0f;
			m_eState = HBS_EXIT_VISIBLE;
			m_eFadingPhase = HB_FADE_OUT;
		}		
		else
		{
			//출력 안함
			m_tConstantBuffer.fAlpha = 0.0f;
			m_tConstantBuffer.fHealthPercentage = fCurrentHP / fMaxHP; 
			ComputeProgressColor();
			return;
		}
	}


	
	if (m_eFadingPhase == HB_FADE_IN)
	{
		if(m_bCurrentInsideScreen)
			m_pTransform->SetWorldPosition(vResultPosition.x, vResultPosition.y, 0.0f);

		m_fAlphaTimer += fDeltaTime;
		m_fDamagedTimer += fDeltaTime;

		float fProgress = CMathf::Clamp(m_fAlphaTimer / m_fFadingTimeThreshold, 0.0f, 1.0f);

		if (fProgress >= 1.0f)
		{
			if (m_bCurrentAggressive)
			{
				m_eState = HBS_STAY_VISIBLE;
				m_eFadingPhase = HB_FADE_NONE;
			}
			else
			{
				m_eState = HBS_EXIT_VISIBLE;
				m_eFadingPhase = HB_FADE_OUT;
			}
		}
		else
		{
			m_tConstantBuffer.fAlpha = fProgress;
			m_tConstantBuffer.fHealthPercentage = fCurrentHP / fMaxHP;
		}
	}

	if (m_eFadingPhase == HB_FADE_NONE)
	{
		if (m_bCurrentInsideScreen)
			m_pTransform->SetWorldPosition(vResultPosition.x, vResultPosition.y, 0.0f);
		m_tConstantBuffer.fAlpha = 1.0f;
		m_tConstantBuffer.fHealthPercentage = fCurrentHP / fMaxHP;
	}

	if (m_eFadingPhase == HB_FADE_OUT)
	{
		if (m_bCurrentInsideScreen)
			m_pTransform->SetWorldPosition(vResultPosition.x, vResultPosition.y, 0.0f);

		m_fAlphaTimer += fDeltaTime;
		m_fDamagedTimer += fDeltaTime;

		float fProgress = CMathf::Clamp(1.0f - (m_fAlphaTimer / m_fFadingTimeThreshold), 0.0f, 1.0f);

		if (fProgress <= 0.0f)
			m_eState = HBS_STABLE;
		else
		{
			m_tConstantBuffer.fAlpha = fProgress;
			m_tConstantBuffer.fHealthPercentage = fCurrentHP / fMaxHP;
		}
	}

	ComputeProgressColor();		
	
}

bool CHealthBar::Initialize()
{
	CSpriteRenderer* pRenderer = GetComponent<CSpriteRenderer>();
	if (!pRenderer)
	{
		pRenderer = m_pGameObject->AddComponent<CSpriteRenderer>();
	}

	m_pTransform->SetWorldScale(120.0f, 20.0f, 0.0f);
	m_pTransform->SetPivot(0.5f, 0.5f, 0.5f);

	pRenderer->SetShader(HEALTHBAR_SHADER);
	pRenderer->SetRenderState(ALPHA_BLEND);
	pRenderer->SetZOrder(5000);	

	SAFE_RELEASE(pRenderer);

	SetBackgroundTexture("HealthBarBackground", L"UI\\HealthBarBackground.png", TEXTURE_PATH);
	SetForegroundTexture("HealthBarForeground", L"UI\\HealthBarForeground.png", TEXTURE_PATH);
	return true;
}

void CHealthBar::Start()
{
}

int CHealthBar::LateUpdate(float fTime)
{
	if (!m_pBaseObject)
	{
		m_eState = HBS_EXIT_VISIBLE;
		UpdateHealthBarInfo(false, 0, 1.0f, fTime);

		float fProgress = CMathf::Clamp(1.0f - (m_fAlphaTimer / m_fFadingTimeThreshold), 0.0f, 1.0f);
		if (fProgress == 0.0f)
			m_pGameObject->Destroy(true);
	}
	return 0;
}

int CHealthBar::Prerender(CMeshRenderer * pRenderer)
{	
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(HEALTHBAR_CBUFFER, &m_tConstantBuffer, CBT_VERTEX | CBT_GEOMETRY | CBT_PIXEL);

	return 0;
}