#include "MeshRenderer.h"
#include "../Mesh.h"
#include "../ResourcesManager.h"
#include "../Shader.h"
#include "../ShaderManager.h"
#include "../Device.h"
#include "Camera.h"
#include "../Scene.h"
#include "../Transform.h"
#include "../Material.h"
#include "../RenderManager.h"
#include "../RenderState.h"
#include "../DataManager.h"
#include "../Texture.h"
#include "../Debug.h"

SSS_USING

CMeshRenderer::CMeshRenderer() :
	m_pMesh(NULL),
	m_pShader(NULL),
	m_pInputLayout(NULL),
	m_pMaterial(NULL),
	m_bMotionBlur(false),
	m_iHasAnimation(AC_NONE),
	m_bUseMeshLocal(false)
{
	memset(m_pRenderState, 0, sizeof(CRenderState*) * RS_END);
}

CMeshRenderer::CMeshRenderer(const CMeshRenderer & component)
{
	*this = component;

	if (m_pShader)
		m_pShader->AddRef();

	if (m_pMesh)
		m_pMesh->AddRef();


	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->AddRef();
	}

	// 만약 재질 정보가 있을 경우 재질 정보를 얻어온다.
	//m_pMaterial = GetComponent<CMaterial>();
}


CMeshRenderer::~CMeshRenderer()
{
	for (int i = 0; i < RS_END; ++i)
	{
		SAFE_RELEASE(m_pRenderState[i]);
	}
	

	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pMesh);
}

bool CMeshRenderer::GetEnableMeshContainer(int iContainer) const
{
	return m_pMesh->GetEnableMeshContainer(iContainer);
}

void CMeshRenderer::SetEnableMeshContainer(int iContainer, bool bEnable)
{
	m_pMesh->SetEnableMeshContainer(iContainer, bEnable);
}

void CMeshRenderer::SetAnimationCondition(int32_t iCondition)
{
	m_iHasAnimation |= iCondition;
}

int32_t CMeshRenderer::GetAnimationCondition() const
{
	return m_iHasAnimation;
}

CMaterial * CMeshRenderer::GetMaterial() const
{
	if (m_pMaterial)
		m_pMaterial->AddRef();

	return m_pMaterial;
}

void CMeshRenderer::SetMaterial(CMaterial * pMaterial)
{
	SAFE_RELEASE(m_pMaterial);

	pMaterial->AddRef();
	m_pMaterial = pMaterial;
}

bool CMeshRenderer::Initialize()
{
	// 만약 재질 정보가 있을 경우 재질 정보를 얻어온다.
	//m_pMaterial = GetComponent<CMaterial>();
	return true;
}

void CMeshRenderer::Render(float fTime)
{
	UpdateTransform();
	UpdatePrerender();
	
	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->SetState();
	}
	
	//재질이 없다면 그릴 수 없으므로 걸러준다.
	//보통은 기본 재질이 들어가도록 한다.
	if (m_pMaterial && m_pMesh)
	{
		for (size_t i = 0; i < m_pMaterial->GetContainerCount(); ++i)
		{
			for (int j = 0; j < m_pMaterial->GetSubsetCount(i); ++j)
			{
				if (m_pMesh->GetEnableMeshContainer(i))
				{
					m_pMaterial->SetMaterial(i, j);

					m_pShader->SetShader();

					DEVICE_CONTEXT->IASetInputLayout(m_pInputLayout);

					m_pMesh->Render(i, j);

					m_pMaterial->ResetMaterial(i, j);
				}
			}
		}
	}
	else
	{
		wstring strLog(m_pGameObject->GetTag().begin(), m_pGameObject->GetTag().end());
		strLog += L" 오브젝트의 CMeshRenderer 컴포넌트가 Material이 존재하지 않습니다.\n";
		GET_SINGLE(CDebug)->Log(strLog);
	}

	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->ResetState();
	}
	
	UpdatePostrender();
}

CMeshRenderer * CMeshRenderer::Clone() const
{
	return new CMeshRenderer(*this);
}

void CMeshRenderer::UpdateTransform()
{
	TRANSFORMCBUFFER	tBuffer;

	// Camera를 얻어온다.
	CCamera*	pCamera = m_pScene->GetMainCamera();

	tBuffer.matWorld = m_pTransform->GetWorldMatrix();
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProjection = pCamera->GetPerspectiveProjectionMatrix();
	tBuffer.matWV = tBuffer.matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
	tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
	tBuffer.matWP = tBuffer.matWorld * tBuffer.matProjection;
	tBuffer.matInvProjection = Matrix::Inverse(tBuffer.matProjection);
	tBuffer.matInvVP = Matrix::Inverse(tBuffer.matVP);
	tBuffer.matInvWV = Matrix::Inverse(tBuffer.matWV);
	tBuffer.matInvWVP = Matrix::Inverse(tBuffer.matWVP);
	
	tBuffer.matShadowView = pCamera->GetShadowViewMatrix();
	tBuffer.matShadowProj = pCamera->GetShadowProjMatrix();
	tBuffer.matShadowVP = tBuffer.matShadowView * tBuffer.matShadowProj;
	tBuffer.matShadowWVP = tBuffer.matWorld * tBuffer.matShadowVP;

	tBuffer.vPivot = m_pTransform->GetPivot();
	tBuffer.iRenderMode = GET_SINGLE(CRenderManager)->GetRenderMode() ? 1 : 0;

	if (m_pMesh)
	{
		tBuffer.vLength = m_pMesh->GetLength();
	}
	else
	{
		tBuffer.vLength = Vector3::Zero;
	}
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
	tBuffer.matInvWV = tBuffer.matInvWV.Transpose();
	tBuffer.matInvWVP = tBuffer.matInvWVP.Transpose();
	tBuffer.matShadowView = tBuffer.matShadowView.Transpose();
	tBuffer.matShadowProj= tBuffer.matShadowProj.Transpose();
	tBuffer.matShadowVP = tBuffer.matShadowVP.Transpose();
	tBuffer.matShadowWVP = tBuffer.matShadowWVP.Transpose();

	tBuffer.iHasAnimation = m_iHasAnimation;

	// Transform 정보를 Shader 상수버퍼로 넘겨준다.
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(TRANSFORM_CBUFFER,
		&tBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);
}

void CMeshRenderer::UpdatePrerender()
{
	list<CComponent*> ComponentList = GetAllComponents();
	list<CComponent*>::iterator iter;
	list<CComponent*>::iterator iterEnd = ComponentList.end();

	for (iter = ComponentList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->Prerender(this	);
	}
}

void CMeshRenderer::UpdatePostrender()
{
	list<CComponent*> ComponentList = GetAllComponents();
	list<CComponent*>::iterator iter;
	list<CComponent*>::iterator iterEnd = ComponentList.end();

	for (iter = ComponentList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->Postrender(this);
	}
}

void CMeshRenderer::SetDefaultMaterial()
{
	if (!m_pMaterial)
	{
		m_pMaterial = new CMaterial;
	}
}

size_t CMeshRenderer::GetContainerCount() const
{
	return m_pMesh->GetContainerCount();
}

bool CMeshRenderer::ChangeMeshContainer(const string & strMesh, int iContainer)
{
	if (m_pMesh)
	{
		if (m_pMesh->ChangeMeshContainer(strMesh, iContainer))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CMeshRenderer::ChangeMeshContainer(CMesh * pMesh, int iContainer)
{	
	if (m_pMesh)
	{
		CMaterial* pMaterial = m_pMesh->GetMaterial();
		bool bResult = true;
		if (!m_pMesh->ChangeMeshContainer(pMesh, iContainer))
		{
			bResult &= false;
		}
		
		
		if (!pMaterial)
		{
			CMaterial* pTargetMaterial = pMesh->GetMaterial();
			if (pTargetMaterial)
			{
				MATERIALCONTAINER* pTargetMaterialContainer = pTargetMaterial->GetMaterialContainer(0);			
				if (!m_pMaterial->ChangeMaterialContainer(pTargetMaterialContainer, iContainer))
				{
					bResult &= false;
				}
			}
			SAFE_RELEASE(pTargetMaterial);
		}
		SAFE_RELEASE(pMaterial);

		return bResult;
	}
	else
	{
		return false;
	}
}

bool CMeshRenderer::InsertMeshContainer(const string & strMesh, int iContainer)
{
	if (m_pMesh)
	{
		if (m_pMesh->InsertMeshContainer(strMesh, iContainer))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CMeshRenderer::InsertMeshContainer(CMesh * pMesh, int iContainer)
{
	if (m_pMesh)
	{
		if (m_pMesh->InsertMeshContainer(pMesh, iContainer))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CMeshRenderer::RemoveMeshContainer(int iContainer)
{
	if (m_pMesh)
	{
		if (m_pMesh->RemoveMeshContainer(iContainer))
		{
			if (m_pMaterial->RemoveMaterialContainer(iContainer))
			{
				return true;
			}
			
			return false;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void CMeshRenderer::SetLocalTransformFromMesh(int iContainer)
{
	if (m_pMesh)
	{
		if (m_pGameObject)
		{
			if (m_pTransform)
			{
				if (m_pTransform->GetParent())
				{
					m_pTransform->SetLocalMatrix(*m_pMesh->GetLocalMatrix(iContainer));
				}
			}
		}
	}
}

ID3D11InputLayout * CMeshRenderer::GetInputLayout() const
{
	return m_pInputLayout;
}

CMesh * CMeshRenderer::GetMesh() const
{
	return m_pMesh;
}

CShader * CMeshRenderer::GetShader() const
{
	return m_pShader;
}

void CMeshRenderer::SetMesh(CMesh * pMesh, bool bOverrideMaterial)
{
	if (!pMesh)
	{
		return;
	}
	
	SAFE_RELEASE(m_pMesh);
	pMesh->AddRef();
	m_pMesh = pMesh;

	CMaterial* pMaterial = m_pMesh->GetMaterial();
	
	if (bOverrideMaterial)
	{
		if (pMaterial)
		{
			//기존 재질 해제
			SAFE_RELEASE(m_pMaterial);
			//RefCount 줄이지 않도록 함.
			m_pMaterial = pMaterial;
		}
		else
		{
			//재질이 없을 경우 기본 재질로 설정 해 준다.
			SAFE_RELEASE(m_pMaterial);
			SetDefaultMaterial();
		}
	}


	if (!m_pMesh->GetShaderKey().empty())
	{
		SetShader(m_pMesh->GetShaderKey());
	}

	if (!m_pMesh->GetLayoutKey().empty())
	{
		SetInputLayout(m_pMesh->GetLayoutKey());
	}
}
void CMeshRenderer::SetMesh(const string & strKey, bool bOverrideMaterial)
{
	SAFE_RELEASE(m_pMesh);
	//Find함수에서 Ref 올림
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(strKey);
	
	if (m_pMesh)
	{
		if (bOverrideMaterial)
		{
			CMaterial* pMaterial = m_pMesh->GetMaterial();
			if (pMaterial)
			{
				//기존 재질 해제
				SAFE_RELEASE(m_pMaterial);
				//RefCount 줄이지 않도록 함.
				m_pMaterial = pMaterial;
			}
			else
			{
				//재질이 없을 경우 기본 재질로 설정 해 준다.
				SetDefaultMaterial();
			}
		}

		if (!m_pMesh->GetShaderKey().empty())
		{
			SetShader(m_pMesh->GetShaderKey());
		}
	
		if (!m_pMesh->GetLayoutKey().empty())
		{
			SetInputLayout(m_pMesh->GetLayoutKey());
		}
	}
	
}

void CMeshRenderer::SetShader(CShader * pShader)
{
	SAFE_RELEASE(m_pShader);
	pShader->AddRef();
	m_pShader = pShader;
}

void CMeshRenderer::SetShader(const string & strKey)
{
	SAFE_RELEASE(m_pShader);
	m_pShader = GET_SINGLE(CShaderManager)->FindShader(strKey);
}

void CMeshRenderer::SetInputLayout(ID3D11InputLayout * pInputLayout)
{
	m_pInputLayout = pInputLayout;
}

void CMeshRenderer::SetInputLayout(const string & strKey)
{
	m_pInputLayout = GET_SINGLE(CShaderManager)->FindInputLayout(strKey);
}

void CMeshRenderer::SetRenderState(const string & strKey)
{
	CRenderState*	pRenderState = GET_SINGLE(CRenderManager)->FindRenderState(strKey);

	if (!pRenderState)
		return;

	SAFE_RELEASE(m_pRenderState[pRenderState->GetRenderStateType()]);

	m_pRenderState[pRenderState->GetRenderStateType()] = pRenderState;

	if (pRenderState->GetRenderStateType() == RS_BLEND || pRenderState->GetKey() == WIREFRAME)
	{
		m_pGameObject->SetTransparency(true);
	}

}

void CMeshRenderer::RenderShadowMap(float fTime)
{
	CCamera* pCamera = m_pScene->GetMainCamera();
	bool bUseShadow = pCamera->GetShadowState();
	SAFE_RELEASE(pCamera);

	if (!bUseShadow)
		return;

	UpdateTransform();

	// 재질정보를 설정한다.
	for (size_t i = 0; i < m_pMaterial->GetContainerCount(); ++i)
	{
		for (int j = 0; j < m_pMaterial->GetSubsetCount(i); ++j)
		{
			m_pMaterial->SetMaterial(i, j);

			DEVICE_CONTEXT->IASetInputLayout(m_pInputLayout);

			// 현재 자신에게 등록된 쉐이더가 애니메이션을 사용하는지 검사합니당..
			if (m_pShader->GetKey() == STANDARD_BUMP_ANIM_SHADER)
			{
				CShader* pShader = GET_SINGLE(CShaderManager)->FindShader("ShadowBumpAnimShader");
				pShader->SetShader();
				SAFE_RELEASE(pShader);
			}
			else if (m_pShader->GetKey() == STANDARD_TEXTURE_NORMAL_ANIM_SHADER)
			{
				CShader* pShader = GET_SINGLE(CShaderManager)->FindShader("ShadowTexNormalAnimShader");
				pShader->SetShader();
				SAFE_RELEASE(pShader);
			}
			// 애니메이션을 사용하지않는 친구라면 이쪽을 사용합니당...
			else
			{
				CShader* pShader = GET_SINGLE(CShaderManager)->FindShader("ShadowShader");
				pShader->SetShader();
				SAFE_RELEASE(pShader);
			}

			m_pMesh->Render(i, j);

			m_pMaterial->ResetMaterial(i, j);
		}
	}
}

void CMeshRenderer::RenderMotionBlurMap()
{
	if (!m_bMotionBlur)
		return;

	UpdateTransform();

	// 재질정보를 설정한다.
	for (size_t i = 0; i < m_pMaterial->GetContainerCount(); ++i)
	{
		for (int j = 0; j < m_pMaterial->GetSubsetCount(i); ++j)
		{
			DEVICE_CONTEXT->IASetInputLayout(m_pInputLayout);

			CShader* pShader = GET_SINGLE(CShaderManager)->FindShader("MotionBlurShader");
			pShader->SetShader();
			SAFE_RELEASE(pShader);

			m_pMesh->Render(i, j);
		}
	}
}

void CMeshRenderer::SetMotionBlur(bool bSwitch)
{
	m_bMotionBlur = bSwitch;
}

bool CMeshRenderer::Save(FILE * pFile)
{
	string strPathKey = m_pMesh->GetPathKey();
	size_t iPathKeyLength = strPathKey.length();
	string strKey = m_pMesh->GetKey();
	size_t iKeyLength = strKey.length();
	string strDirectory = m_pMesh->GetDirectory();
	size_t iDirectoryLength = strDirectory.length();

	fwrite(&iPathKeyLength, sizeof(size_t), 1, pFile);
	fwrite(strPathKey.c_str(), sizeof(char), iPathKeyLength, pFile);
	fwrite(&iKeyLength, sizeof(size_t), 1, pFile);
	fwrite(strKey.c_str(), sizeof(char), iKeyLength, pFile);
	fwrite(&iDirectoryLength, sizeof(size_t), 1, pFile);
	fwrite(strDirectory.c_str(), sizeof(char), iDirectoryLength, pFile);
	
	string strShaderKey = m_pShader->GetKey();
	size_t iShaderKeyLength = strShaderKey.length();
	string strInputLayoutKey = GET_SINGLE(CShaderManager)->FindInputLayoutKey(m_pInputLayout);	
	size_t iInputLayoutKeyLength = strInputLayoutKey.length();

	fwrite(&iShaderKeyLength, sizeof(size_t), 1, pFile);
	fwrite(strShaderKey.c_str(), sizeof(char), iShaderKeyLength, pFile);
	fwrite(&iInputLayoutKeyLength, sizeof(size_t), 1, pFile);
	fwrite(strInputLayoutKey.c_str(), sizeof(char), iInputLayoutKeyLength, pFile);
	

	for (size_t i = 0; i < 3; ++i)
	{
		bool bEnableRenderState = m_pRenderState[i] != NULL;
		fwrite(&bEnableRenderState, sizeof(bool), 1, pFile);

		if (bEnableRenderState)
		{
			string strRenderStateKey = m_pRenderState[i]->GetKey();
			size_t iRenderStateKeyLength = strRenderStateKey.length();
			if (bEnableRenderState)
			{
				fwrite(&iRenderStateKeyLength, sizeof(size_t), 1, pFile);
				fwrite(strRenderStateKey.c_str(), sizeof(char), iRenderStateKeyLength, pFile);
			}
		}
	}

//Load Material
	m_pMaterial->Save(pFile);



	return true;
}

bool CMeshRenderer::Load(FILE * pFile)
{
	//Clear Contained Properties

	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pShader);

	for (size_t i = 0; i < RS_END; ++i)
	{
		SAFE_RELEASE(m_pRenderState[i]);
	}

	//Load Mesh Data
	char strPathKey[256] = {};
	size_t iPathKeyLength = 0;
	char strKey[256] = {};
	size_t iKeyLength = 0;
	char strDirectory[512] = {};
	size_t iDirectoryLength = 0;

	fread(&iPathKeyLength, sizeof(size_t), 1, pFile);
	fread(&strPathKey[0], sizeof(char), iPathKeyLength, pFile);
	fread(&iKeyLength, sizeof(size_t), 1, pFile);
	fread(&strKey[0], sizeof(char), iKeyLength, pFile);
	fread(&iDirectoryLength, sizeof(size_t), 1, pFile);
	fread(&strDirectory[0], sizeof(char), iDirectoryLength, pFile);

	char strShaderKey[256] = {};
	size_t iShaderKeyLength = 0;
	char strInputLayoutKey[256] = {};
	size_t iInputLayoutKeyLength = 0;

	fread(&iShaderKeyLength, sizeof(size_t), 1, pFile);
	fread(&strShaderKey[0], sizeof(char), iShaderKeyLength, pFile);
	fread(&iInputLayoutKeyLength, sizeof(size_t), 1, pFile);
	fread(&strInputLayoutKey[0], sizeof(char), iInputLayoutKeyLength, pFile);

	CMesh* pMesh = GET_SINGLE(CResourcesManager)->FindMesh(strKey);
	if (!pMesh)
	{
		string pPath(strDirectory);
		wchar_t pPathW[512] = {};

		MultiByteToWideChar(CP_ACP, 0, pPath.c_str(), -1, pPathW, pPath.length());

		wstring strPathW(pPathW);
		CMesh* pMesh = GET_SINGLE(CResourcesManager)->LoadMesh(strKey, strPathW.c_str(), strPathKey, false);
		if (pMesh)
		{
			SetMesh(pMesh);
			SAFE_RELEASE(pMesh);
		}
		else
		{
			//Mesh가 로드에 실패했습니다.
			assert(false);
			return false;
		}
	}
	else
	{
		SetMesh(pMesh);
		SAFE_RELEASE(pMesh);
	}
	
//Load Input Layout
		SetInputLayout(strInputLayoutKey);
		if (!m_pInputLayout)
		{
			//Input Layout이 생성되어 있지 않습니다.
			assert(false);
			return false;
		}


//LoadShader
		SetShader(strShaderKey);
		if (!m_pShader)
		{
			return false;
		}

//Load Render State
	for (size_t i = 0; i < 3; ++i)
	{
		bool bEnableRenderState = 0;
		size_t iRenderStateKeyLength = 0;
		char strRenderStateKey[256] = {};
	
		fread(&bEnableRenderState, sizeof(bool), 1, pFile);
		if (bEnableRenderState)
		{
			fread(&iRenderStateKeyLength, sizeof(size_t), 1, pFile);
			fread(strRenderStateKey, sizeof(char), iRenderStateKeyLength, pFile);
			SetRenderState(strRenderStateKey);
		}
	}

//Load Material
	SAFE_RELEASE(m_pMaterial);
	m_pMaterial = new CMaterial;
	m_pMaterial->Load(pFile);


	return true;
}
