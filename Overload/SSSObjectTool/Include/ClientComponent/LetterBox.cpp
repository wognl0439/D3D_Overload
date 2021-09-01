#include "LetterBox.h"
#include "Transform.h"
#include "Component/SpriteRenderer.h"
#include "Core.h"
#include "Device.h"
#include "Layer.h"
#include "Scene.h"
#include "Material.h"

CLetterBox::CLetterBox()	:
	pBlackBoad_High(NULL),
	pBlackBoad_Low(NULL),
	bVisible(false),
	fVisibleTime(0.f),
	bStart(false)
{
}

CLetterBox::~CLetterBox()
{
	if(pBlackBoad_High)
		SAFE_RELEASE(pBlackBoad_High);
	if (pBlackBoad_Low)
		SAFE_RELEASE(pBlackBoad_Low);
}

bool CLetterBox::Initialize()
{
	return true;
}

void CLetterBox::Start()
{
	pBlackBoad_High = CGameObject::CreateObject("BlackBoad", m_pLayer);

	CSpriteRenderer* pSpriteRenderer = pBlackBoad_High->AddComponent<CSpriteRenderer>();
	pSpriteRenderer->SetDefaultMaterial();
	CMaterial* pMtl = pSpriteRenderer->GetMaterial();
	pMtl->SetDiffuseColor(Vector4(1.f,1.f,1.f,0.f));
	SAFE_RELEASE(pMtl);
	SAFE_RELEASE(pSpriteRenderer);

	CTransform* pTransform = pBlackBoad_High->GetTransform();
	m_pTransform->SetPivot(0.5f, 0.5f, 1.f);
	m_pTransform->SetWorldScale(DEVICE_RESOLUTION.iWidth, 100.f, 1.f);
	m_pTransform->SetWorldPosition(DEVICE_RESOLUTION.iWidth * 0.5f, 48.f, 0.f);
	SAFE_RELEASE(pTransform);

	pBlackBoad_Low = CGameObject::CreateObject("BlackBoad", m_pLayer);

	pSpriteRenderer = pBlackBoad_Low->AddComponent<CSpriteRenderer>();
	pSpriteRenderer->SetDefaultMaterial();
	pMtl = pSpriteRenderer->GetMaterial();
	pMtl->SetDiffuseColor(Vector4(1.f, 1.f, 1.f, 0.f));
	SAFE_RELEASE(pMtl);
	SAFE_RELEASE(pSpriteRenderer);

	pTransform = pBlackBoad_Low->GetTransform();
	m_pTransform->SetPivot(0.5f, 0.5f, 1.f);
	m_pTransform->SetWorldScale(DEVICE_RESOLUTION.iWidth, 100.f, 1.f);
	m_pTransform->SetWorldPosition(DEVICE_RESOLUTION.iWidth * 0.5f, DEVICE_RESOLUTION.iHeight - 48.f, 0.f);
	SAFE_RELEASE(pTransform);

	SetVisibleLetterBox(true);
}

int CLetterBox::Update(float fTime)
{
	if (bStart)
		SlowUpdateLetterBox(fTime);

	return 0;
}

void CLetterBox::SetVisibleLetterBox(bool visible)
{
	bVisible = visible;
	bStart = true;
	
	if (bVisible)
		fVisibleTime = 0.f;
	else
		fVisibleTime = 255.f;
}

void CLetterBox::SlowUpdateLetterBox(float fTime)
{
	CSpriteRenderer* pSpriteRenderer = pBlackBoad_High->GetComponent<CSpriteRenderer>();
	CMaterial* pMtl = pSpriteRenderer->GetMaterial();
	Vector4 vColor = pMtl->GetDiffuseColor();

	if (bVisible)
	{
		if (fVisibleTime < 255.f)
			fVisibleTime += fTime;
		else
			bStart = false;
	}
	else
	{
		if (fVisibleTime > 0.f)
			fVisibleTime -= fTime;
		else
			bStart = false;
	}
	
	vColor.z = fVisibleTime / 255.f;

	pMtl->SetDiffuseColor(vColor);
	SAFE_RELEASE(pMtl);
	SAFE_RELEASE(pSpriteRenderer);

	pSpriteRenderer = pBlackBoad_Low->GetComponent<CSpriteRenderer>();
	pMtl = pSpriteRenderer->GetMaterial();

	pMtl->SetDiffuseColor(vColor);
	SAFE_RELEASE(pMtl);
	SAFE_RELEASE(pSpriteRenderer);
}