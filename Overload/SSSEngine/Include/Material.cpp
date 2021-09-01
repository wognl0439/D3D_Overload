#include "Material.h"
#include "../Texture.h"
#include "../Sampler.h"
#include "../ResourcesManager.h"
#include "../Device.h"
#include "../ShaderManager.h"
#include "Component/SpriteRenderer.h"
#include "Component/MeshRenderer.h"

SSS_USING

CMaterial::CMaterial()
{
	SetTag("Material");
	m_vecContainer.resize(1);
	m_vecContainer[0].vecSubset.resize(1);
}

CMaterial::CMaterial(const CMaterial & mtrl)
{
	for (size_t i = 0; i < mtrl.m_vecContainer.size(); ++i)
	{
		MATERIALCONTAINER	tContainer;

		for (size_t j = 0; j < mtrl.m_vecContainer[i].vecSubset.size(); ++j)
		{
			MATERIALSUBSET	tSubset;
			tSubset.tMaterial = mtrl.m_vecContainer[i].vecSubset[j].tMaterial;
			tSubset.pShaderResource = NULL;

			if (mtrl.m_vecContainer[i].vecSubset[j].pShaderResource)
			{
				tSubset.pShaderResource = new SHADERRESOURCE;
				*tSubset.pShaderResource = *mtrl.m_vecContainer[i].vecSubset[j].pShaderResource;

				if (tSubset.pShaderResource->pDiffuseTexture)
					tSubset.pShaderResource->pDiffuseTexture->AddRef();

				if (tSubset.pShaderResource->pNormalTexture)
					tSubset.pShaderResource->pNormalTexture->AddRef();

				if (tSubset.pShaderResource->pSpecularTexture)
					tSubset.pShaderResource->pSpecularTexture->AddRef();

				if (tSubset.pShaderResource->pSampler)
					tSubset.pShaderResource->pSampler->AddRef();
			}
			tContainer.vecSubset.push_back(tSubset);
		}

		m_vecContainer.push_back(tContainer);
	}
}

CMaterial::~CMaterial()
{
	for (size_t i = 0; i < m_vecContainer.size(); ++i)
	{
		for (size_t j = 0; j < m_vecContainer[i].vecSubset.size(); ++j)
		{
			if (m_vecContainer[i].vecSubset[j].pShaderResource)
			{
				SAFE_RELEASE(m_vecContainer[i].vecSubset[j].pShaderResource->pDiffuseTexture);
				SAFE_RELEASE(m_vecContainer[i].vecSubset[j].pShaderResource->pNormalTexture);
				SAFE_RELEASE(m_vecContainer[i].vecSubset[j].pShaderResource->pSpecularTexture);
				SAFE_RELEASE(m_vecContainer[i].vecSubset[j].pShaderResource->pAlphaTexture);
				SAFE_RELEASE(m_vecContainer[i].vecSubset[j].pShaderResource->pSampler);
				SAFE_DELETE(m_vecContainer[i].vecSubset[j].pShaderResource);
			}


			unordered_map<string, SHADERRESOURCE>::iterator	iter;
			unordered_map<string, SHADERRESOURCE>::iterator	iterEnd = m_vecContainer[i].vecSubset[j].mapMultiTexture.end();

			for (iter = m_vecContainer[i].vecSubset[j].mapMultiTexture.begin(); iter != iterEnd; ++iter)
			{
				SAFE_RELEASE(iter->second.pDiffuseTexture);
				SAFE_RELEASE(iter->second.pNormalTexture);
				SAFE_RELEASE(iter->second.pSpecularTexture);
				SAFE_RELEASE(iter->second.pAlphaTexture);
				SAFE_RELEASE(iter->second.pSampler);
			}
		}
	}
}


bool CMaterial::Initialize()
{
	return true;
}

CMaterial * CMaterial::Clone() const
{
	return new CMaterial(*this);
}

void CMaterial::SetMaterial(int iContainer, int iSubset)
{
	PMATERIALSUBSET	pSubset = &m_vecContainer[iContainer].vecSubset[iSubset];

	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(MATERIAL_CBUFFER,
		&m_vecContainer[iContainer].vecSubset[iSubset].tMaterial,
		CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);

	if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
	{
		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture)
			m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture->SetTexture(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iDiffuseTextureRegister);

		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture)
			m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture->SetTexture(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iNormalTextureRegister);

		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture)
			m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture->SetTexture(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iSpecularTextureRegister);

		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture)
			m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture->SetTexture(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iAlphaTextureRegister);

		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSampler)
			m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSampler->SetSampler(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iSamplerRegister);

	}

	unordered_map<string, SHADERRESOURCE>::iterator	iter;
	unordered_map<string, SHADERRESOURCE>::iterator	iterEnd = pSubset->mapMultiTexture.end();

	for (iter = pSubset->mapMultiTexture.begin(); iter != iterEnd; ++iter)
	{
		if (iter->second.pDiffuseTexture)
			iter->second.pDiffuseTexture->SetTexture(iter->second.iDiffuseTextureRegister);

		if (iter->second.pNormalTexture)
			iter->second.pNormalTexture->SetTexture(iter->second.iNormalTextureRegister);

		if (iter->second.pSpecularTexture)
			iter->second.pSpecularTexture->SetTexture(iter->second.iSpecularTextureRegister);

		if (iter->second.pAlphaTexture)
			iter->second.pAlphaTexture->SetTexture(iter->second.iAlphaTextureRegister);

		if (iter->second.pSampler)
			iter->second.pSampler->SetSampler(iter->second.iSamplerRegister);
	}
}

void CMaterial::ResetMaterial(int iContainer, int iSubset)
{
}



bool CMaterial::ChangeMaterialContainer(MATERIALCONTAINER * pSourceContainer, int iChangeContainerIndex)
{
	if (RemoveMaterialContainer(iChangeContainerIndex))
	{
		if (InsertMaterialContainer(pSourceContainer, iChangeContainerIndex))
		{
			return true;
		}
		return false;
	}
	return true;
}

//내부에서 모든 리소스에 대해 Ref 올림
bool CMaterial::InsertMaterialContainer(MATERIALCONTAINER * pSourceContainer, int iInsertContainerIndex)
{
	MATERIALCONTAINER tContainer = {};

	size_t iSubsetSize = pSourceContainer->vecSubset.size();

	for (size_t i = 0; i < iSubsetSize; ++i)
	{
		//깊은 복사를 한다.
		MATERIALSUBSET tTargetSubset = pSourceContainer->vecSubset[i];
		MATERIALSUBSET tSubset = {};
		tSubset.tMaterial = tTargetSubset.tMaterial; //내부에서 복사생성자 호출함

		if (pSourceContainer->vecSubset[i].pShaderResource)
		{
			tSubset.pShaderResource = new SHADERRESOURCE;
			*tSubset.pShaderResource = *pSourceContainer->vecSubset[i].pShaderResource;

			if (tSubset.pShaderResource->pDiffuseTexture)
				tSubset.pShaderResource->pDiffuseTexture->AddRef();

			if (tSubset.pShaderResource->pNormalTexture)
				tSubset.pShaderResource->pNormalTexture->AddRef();

			if (tSubset.pShaderResource->pSpecularTexture)
				tSubset.pShaderResource->pSpecularTexture->AddRef();

			if (tSubset.pShaderResource->pAlphaTexture)
				tSubset.pShaderResource->pAlphaTexture->AddRef();

			if (tSubset.pShaderResource->pSampler)
				tSubset.pShaderResource->pSampler->AddRef();
		}

		unordered_map<string, SHADERRESOURCE>::iterator iter;
		unordered_map<string, SHADERRESOURCE>::iterator iterEnd = tTargetSubset.mapMultiTexture.end();

		for (iter = tTargetSubset.mapMultiTexture.begin(); iter != iterEnd; ++iter)
		{
			SHADERRESOURCE tShaderResources = {};
			tShaderResources = iter->second;

			if (tShaderResources.pDiffuseTexture)
				tShaderResources.pDiffuseTexture->AddRef();

			if (tShaderResources.pNormalTexture)
				tShaderResources.pNormalTexture->AddRef();

			if (tShaderResources.pSpecularTexture)
				tShaderResources.pSpecularTexture->AddRef();

			if (tShaderResources.pAlphaTexture)
				tShaderResources.pAlphaTexture->AddRef();

			if (tShaderResources.pSampler)
				tShaderResources.pSampler->AddRef();

			tSubset.mapMultiTexture.insert(make_pair(iter->first, tShaderResources));
		}

		tContainer.vecSubset.push_back(tSubset);
	}	

	m_vecContainer.insert(m_vecContainer.begin() + iInsertContainerIndex, tContainer);
	return true;
}

//절대 단독으로 사용하지 말것. 
//MeshContainer와 인덱스가 일치하지 않으면 텍스쳐를 제대로 읽어오지 않음.
//MeshContainer와 갯수가 맞지 않으면 터짐
bool CMaterial::RemoveMaterialContainer(int iContainerIndex)
{
	size_t iSubsetCount = m_vecContainer[iContainerIndex].vecSubset.size();
	
	for (size_t i = 0; i < iSubsetCount; ++i)
	{
		if (m_vecContainer[iContainerIndex].vecSubset[i].pShaderResource)
		{
			SAFE_RELEASE(m_vecContainer[iContainerIndex].vecSubset[i].pShaderResource->pDiffuseTexture);
			SAFE_RELEASE(m_vecContainer[iContainerIndex].vecSubset[i].pShaderResource->pNormalTexture);
			SAFE_RELEASE(m_vecContainer[iContainerIndex].vecSubset[i].pShaderResource->pSpecularTexture);
			SAFE_RELEASE(m_vecContainer[iContainerIndex].vecSubset[i].pShaderResource->pAlphaTexture);
			SAFE_RELEASE(m_vecContainer[iContainerIndex].vecSubset[i].pShaderResource->pSampler);
			SAFE_DELETE(m_vecContainer[iContainerIndex].vecSubset[i].pShaderResource);
		}

		unordered_map<string, SHADERRESOURCE>::iterator	iter;
		unordered_map<string, SHADERRESOURCE>::iterator	iterEnd = m_vecContainer[iContainerIndex].vecSubset[i].mapMultiTexture.end();

		for (iter = m_vecContainer[iContainerIndex].vecSubset[i].mapMultiTexture.begin(); iter != iterEnd; ++iter)
		{
			SAFE_RELEASE(iter->second.pDiffuseTexture);
			SAFE_RELEASE(iter->second.pNormalTexture);
			SAFE_RELEASE(iter->second.pSpecularTexture);
			SAFE_RELEASE(iter->second.pAlphaTexture);
			SAFE_RELEASE(iter->second.pSampler);
		}
	}

	m_vecContainer.erase(m_vecContainer.begin() + iContainerIndex);
	return true;
}

MATERIALCONTAINER * CMaterial::GetMaterialContainer(int iContainer)
{
	if (iContainer < 0 || iContainer >= (int)m_vecContainer.size())
		return NULL;

	return &m_vecContainer[iContainer];
}

CTexture * CMaterial::GetDiffuseTexture(int iContainer, int iSubset) const
{
	if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
	{
		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture)
			m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture->AddRef();

		return m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture;
	}

	return NULL;
}

CTexture * CMaterial::GetNormalTexture(int iContainer, int iSubset) const
{
	if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
	{
		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture)
			m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture->AddRef();

		return m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture;
	}
	return NULL;
}

CTexture * CMaterial::GetSpecularTexture(int iContainer, int iSubset) const
{
	if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
	{
		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture)
			m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture->AddRef();

		return m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture;
	}
	return NULL;
}

CTexture * CMaterial::GetAlphaTexture(int iContainer, int iSubset) const
{
	if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
	{
		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture)
			m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture->AddRef();

		return m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture;
	}
	return NULL;
}

CTexture * CMaterial::GetAlphaMultiTexture(const string & strMultiTexKey, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture.empty())
	{
		return m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pAlphaTexture;
	}

	return NULL;
}

const Vector4 & CMaterial::GetDiffuseColor(int iContainer, int iSubset) const
{
	return m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vDiffuse;
}

const Vector4 & CMaterial::GetAmbientColor(int iContainer, int iSubset) const
{
	return m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAmbient;
}

const Vector4 & CMaterial::GetSpecularColor(int iContainer, int iSubset) const
{
	return m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vSpecular	;
}

const Vector4 & CMaterial::GetEmissiveColor(int iContainer, int iSubset) const
{
	return m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vEmissive;
}

const Vector4 & CMaterial::GetAlphaColor(int iContainer, int iSubset) const
{
	return m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAlpha;
}

int CMaterial::GetDiffuseTextureRegister(int iContainer, int iSubset) const
{
	if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
	{
		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture)
			return m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iDiffuseTextureRegister;
	}
	return -1;
}

int CMaterial::GetNormalTextureRegister(int iContainer, int iSubset) const
{
	if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
	{
		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture)
			return m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iNormalTextureRegister;
	}
	return -1;
}

int CMaterial::GetSpecularTextureRegister(int iContainer, int iSubset) const
{
	if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
	{
		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture)
			return m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iSpecularTextureRegister;
	}
	return -1;
}

int CMaterial::GetAlphaTextureRegister(int iContainer, int iSubset) const
{
	if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
	{
		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture)
			return m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iAlphaTextureRegister;
	}
	return -1;
}

void CMaterial::SetDiffuseColor(const Vector4 & vColor, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vDiffuse = vColor;
}

void CMaterial::SetAmbientColor(const Vector4 & vColor, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAmbient = vColor;
}

void CMaterial::SetSpecularColor(const Vector4 & vColor, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vSpecular = vColor;
}

void CMaterial::SetEmissiveColor(const Vector4 & vColor, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vEmissive = vColor;
}

void CMaterial::SetAlphaColor(const Vector4 & vColor, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAlpha = vColor;
}

size_t CMaterial::GetContainerCount() const
{
	return m_vecContainer.size();
}

size_t CMaterial::GetSubsetCount(int iContainer)
{
	return m_vecContainer[iContainer].vecSubset.size();
}

size_t CMaterial::GetMultiTextureCount(int iContainer, int iSubset)
{
	return m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture.size();
}

const unordered_map<string, SHADERRESOURCE>* CMaterial::GetMultiTexture(int iContainer, int iSubset) const
{
	return &m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture;
}

CSampler * CMaterial::GetSampler(int iContainer, int iSubset)
{
	if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
	{
		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSampler)
			m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSampler->AddRef();

		return m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSampler;
	}
	return NULL;
}

int CMaterial::GetSamplerRegister(int iContainer, int iSubset)
{
	if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
	{
		if (m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSampler)
			return m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iSamplerRegister;
	}
	return -1;
}

void CMaterial::ClearContainer()
{
	for (size_t i = 0; i < m_vecContainer.size(); ++i)
	{
		for (size_t j = 0; j < m_vecContainer[i].vecSubset.size(); ++j)
		{
			if (m_vecContainer[i].vecSubset[j].pShaderResource)
			{
				SAFE_RELEASE(m_vecContainer[i].vecSubset[j].pShaderResource->pDiffuseTexture);
				SAFE_RELEASE(m_vecContainer[i].vecSubset[j].pShaderResource->pNormalTexture);
				SAFE_RELEASE(m_vecContainer[i].vecSubset[j].pShaderResource->pSpecularTexture);
				SAFE_RELEASE(m_vecContainer[i].vecSubset[j].pShaderResource->pSampler);
				SAFE_DELETE(m_vecContainer[i].vecSubset[j].pShaderResource);
			}
		}
	}

	m_vecContainer.clear();	
}

void CMaterial::AddContainer()
{
	MATERIALCONTAINER	tContainer;
	m_vecContainer.push_back(tContainer);
}

void CMaterial::ResizeContainer(size_t iSize)
{
	for (int i = 0; i < iSize; ++i)
	{
		MATERIALCONTAINER	tContainer;
		m_vecContainer.push_back(tContainer);
	}
}

void CMaterial::ResizeSubset(size_t iSize, int iContainer)
{
	for (int i = 0; i < iSize; ++i)
	{
		MATERIALSUBSET	tSubset;
		m_vecContainer[iContainer].vecSubset.push_back(tSubset);
	}
} 

void CMaterial::AddSubset(int iContainer)
{
	MATERIALSUBSET	tSubset;
	m_vecContainer[iContainer].vecSubset.push_back(tSubset);
}

void CMaterial::SetMaterial(const Vector4 & vDiffuse, const Vector4 & vAmbient, const Vector4 & vSpecular, float fSpecularPower, const Vector4 & vEmissive, int iContainer, int iSubset)
{
	//????
	float	fSpecularTex = m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAmbient.w;

	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vDiffuse = vDiffuse;
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAmbient = vAmbient;
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vSpecular = vSpecular;
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vEmissive = vEmissive;
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vSpecular.w = fSpecularPower;
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAmbient.w  = fSpecularTex;
}

void CMaterial::SetDiffuseTexture(const string & strKey, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture = GET_SINGLE(CResourcesManager)->FindTexture(strKey);
}

void CMaterial::SetDiffuseTexture(CTexture * pTexture, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture);

	if (pTexture)
		pTexture->AddRef();

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture = pTexture;
}

void CMaterial::SetDiffuseTexture(const string & strKey, const wchar_t * pFileName, int iContainer, int iSubset, const string & strPathKey)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, pFileName, strPathKey);
}

void CMaterial::SetDiffuseTextureFromFullPath(const string & strKey, const wchar_t * pFullPath, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, pFullPath);
}

void CMaterial::SetDiffuseTexture(const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset, const string & strPathKey)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, vecFileName, strPathKey);
}

void CMaterial::SetDiffuseTextureFromFullPath(const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pDiffuseTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, vecFileName);
}

void CMaterial::SetDiffuseTextureRegister(int iRegister, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iDiffuseTextureRegister = iRegister;
}

void CMaterial::SetNormalTexture(const string & strKey, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture = GET_SINGLE(CResourcesManager)->FindTexture(strKey);
}

void CMaterial::SetNormalTexture(CTexture * pTexture, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture);

	if (pTexture)
		pTexture->AddRef();

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture = pTexture;
}

void CMaterial::SetNormalTexture(const string & strKey, const wchar_t * pFileName, int iContainer, int iSubset, const string & strPathKey)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, pFileName, strPathKey);
}

void CMaterial::SetNormalTextureFromFullPath(const string & strKey, const wchar_t * pFullPath, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, pFullPath);
}

void CMaterial::SetNormalTexture(const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset, const string & strPathKey)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, vecFileName, strPathKey);
}

void CMaterial::SetNormalTextureFromFullPath(const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pNormalTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, vecFileName);
}

void CMaterial::SetNormalTextureRegister(int iRegister, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iNormalTextureRegister = iRegister;
}


void CMaterial::SetSpecularTexture(const string & strKey, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture = GET_SINGLE(CResourcesManager)->FindTexture(strKey);

	// Specular Texture 셋팅할 경우 사용한다고 Shdaer 에게 알려주기위해..
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAmbient.w = 1.f;
}

void CMaterial::SetSpecularTexture(CTexture * pTexture, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture);

	if (pTexture)
		pTexture->AddRef();

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture = pTexture;

	// Specular Texture 셋팅할 경우 사용한다고 Shdaer 에게 알려주기위해..
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAmbient.w = 1.f;
}

void CMaterial::SetSpecularTexture(const string & strKey, const wchar_t * pFileName, int iContainer, int iSubset, const string & strPathKey)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, pFileName, strPathKey);

	// Specular Texture 셋팅할 경우 사용한다고 Shdaer 에게 알려주기위해..
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAmbient.w = 1.f;
}

void CMaterial::SetSpecularTextureFromFullPath(const string & strKey, const wchar_t * pFullPath, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, pFullPath);

	// Specular Texture 셋팅할 경우 사용한다고 Shdaer 에게 알려주기위해..
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAmbient.w = 1.f;
}

void CMaterial::SetSpecularTexture(const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset, const string & strPathKey)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, vecFileName, strPathKey);

	// Specular Texture 셋팅할 경우 사용한다고 Shdaer 에게 알려주기위해..
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAmbient.w = 1.f;
}

void CMaterial::SetSpecularTextureFromFullPath(const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSpecularTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, vecFileName);

	// Specular Texture 셋팅할 경우 사용한다고 Shdaer 에게 알려주기위해..
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAmbient.w = 1.f;
}

void CMaterial::SetSpecularTextureRegister(int iRegister, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iSpecularTextureRegister = iRegister;
}

void CMaterial::ChangeAlphaTexture(int Count, const string & strMultiTexKey, const wchar_t * pFileName, int iContainer, int iSubset, const string & strPathKey)
{
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pAlphaTexture->m_vecFileName[Count] = pFileName;
}

void CMaterial::SetNormalScale(float fScale, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.fNormalScale = fScale;
}

float CMaterial::GetNormalScale(int iContainer, int iSubset) const
{
	return m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.fNormalScale;
}

void CMaterial::SetAlphaTexture(const string & strKey, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture = GET_SINGLE(CResourcesManager)->FindTexture(strKey);

	// Alpha Texture 셋팅할 경우 사용한다고 Shdaer 에게 알려주기위해..
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAlpha.w = 1.f;
}

void CMaterial::SetAlphaTexture(CTexture * pTexture, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture);

	if (pTexture)
		pTexture->AddRef();

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture = pTexture;

	// Alpha Texture 셋팅할 경우 사용한다고 Shdaer 에게 알려주기위해..
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAlpha.w = 1.f;
}

void CMaterial::SetAlphaTexture(const string & strKey, const wchar_t * pFileName, int iContainer, int iSubset, const string & strPathKey)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, pFileName, strPathKey);

	// Alpha Texture 셋팅할 경우 사용한다고 Shdaer 에게 알려주기위해..
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAlpha.w = 1.f;
}

void CMaterial::SetAlphaTextureFromFullPath(const string & strKey, const wchar_t * pFullPath, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, pFullPath);

	// Alpha Texture 셋팅할 경우 사용한다고 Shdaer 에게 알려주기위해..
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAlpha.w = 1.f;
}

void CMaterial::SetAlphaTexture(const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset, const string & strPathKey)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, vecFileName, strPathKey);

	// Alpha Texture 셋팅할 경우 사용한다고 Shdaer 에게 알려주기위해..
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAlpha.w = 1.f;
}

void CMaterial::SetAlphaTextureFromFullPath(const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pAlphaTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, vecFileName);

	// Alpha Texture 셋팅할 경우 사용한다고 Shdaer 에게 알려주기위해..
	m_vecContainer[iContainer].vecSubset[iSubset].tMaterial.vAlpha.w = 1.f;
}

void CMaterial::SetAlphaTextureRegister(int iRegister, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iAlphaTextureRegister = iRegister;
}

void CMaterial::SetDiffuseMultiTexture(const string & strMultiTexKey, const string & strKey, const wchar_t * pFileName, int iContainer, int iSubset, const string & strPathKey)
{	 
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pDiffuseTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pDiffuseTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, pFileName, strPathKey);
}

void CMaterial::SetDiffuseMultiTextureFromFullPath(const string & strMultiTexKey, const string & strKey, const wchar_t * pFullPath, int iContainer, int iSubset)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pDiffuseTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pDiffuseTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, pFullPath);
}

void CMaterial::SetDiffuseMultiTexture(const string & strMultiTexKey, const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset, const string & strPathKey)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pDiffuseTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pDiffuseTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, vecFileName, strPathKey);
}

void CMaterial::SetDiffuseMultiTextureFromFullPath(const string & strMultiTexKey, const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pDiffuseTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pDiffuseTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, vecFileName);
}


void CMaterial::SetNormalMultiTexture(const string & strMultiTexKey, const string & strKey, const wchar_t * pFileName, int iContainer, int iSubset, const string & strPathKey)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pNormalTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pNormalTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, pFileName, strPathKey);
}

void CMaterial::SetNormalMultiTextureFromFullPath(const string & strMultiTexKey, const string & strKey, const wchar_t * pFullPath, int iContainer, int iSubset)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pNormalTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pNormalTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, pFullPath);
}

void CMaterial::SetNormalMultiTexture(const string & strMultiTexKey, const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset, const string & strPathKey)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pNormalTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pNormalTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, vecFileName, strPathKey);
}

void CMaterial::SetNormalMultiTextureFromFullPath(const string & strMultiTexKey, const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pNormalTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pNormalTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, vecFileName);
}


void CMaterial::SetSpecularMultiTexture(const string & strMultiTexKey, const string & strKey, const wchar_t * pFileName, int iContainer, int iSubset, const string & strPathKey)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pSpecularTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pSpecularTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, pFileName, strPathKey);
}

void CMaterial::SetSpecularMultiTextureFromFullPath(const string & strMultiTexKey, const string & strKey, const wchar_t * pFullPath, int iContainer, int iSubset)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pSpecularTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pSpecularTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, pFullPath);
}

void CMaterial::SetSpecularMultiTexture(const string & strMultiTexKey, const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset, const string & strPathKey)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pSpecularTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pSpecularTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, vecFileName, strPathKey);
}

void CMaterial::SetSpecularMultiTextureFromFullPath(const string & strMultiTexKey, const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pSpecularTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pSpecularTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, vecFileName);
}

void CMaterial::SetAlphaMultiTexture(const string & strMultiTexKey, const string & strKey, const wchar_t * pFileName, int iContainer, int iSubset, const string & strPathKey)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pAlphaTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pAlphaTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, pFileName, strPathKey);
}

void CMaterial::SetAlphaMultiTextureFromFullPath(const string & strMultiTexKey, const string & strKey, const wchar_t * pFullPath, int iContainer, int iSubset)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pAlphaTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pAlphaTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, pFullPath);
}

void CMaterial::SetAlphaMultiTexture(const string & strMultiTexKey, const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset, const string & strPathKey)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pAlphaTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pAlphaTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, vecFileName, strPathKey);
}

void CMaterial::SetAlphaMultiTextureFromFullPath(const string & strMultiTexKey, const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pAlphaTexture);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pAlphaTexture = GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strKey, vecFileName);
}



void CMaterial::SetDiffuseMultiTextureRegister(const string & strMultiTexKey, int iRegister, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].iDiffuseTextureRegister = iRegister;
}

void CMaterial::SetNormalMultiTextureRegister(const string & strMultiTexKey, int iRegister, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].iNormalTextureRegister = iRegister;
}

void CMaterial::SetSpecularMultiTextureRegister(const string & strMultiTexKey, int iRegister, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].iSpecularTextureRegister = iRegister;
}

void CMaterial::SetAlphaMultiTextureRegister(const string & strMultiTexKey, int iRegister, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].iAlphaTextureRegister = iRegister;
}


void CMaterial::SetMultiTextureSampler(const string & strMultiTexKey, const string & strSamplerKey, int iContainer, int iSubset)
{
	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pSampler);
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pSampler = GET_SINGLE(CResourcesManager)->FindSampler(strSamplerKey);
}

void CMaterial::SetMultiTextureSamplerRegister(const string & strMultiTexKey, int iSamplerRegister, int iContainer, int iSubset)
{
	if (m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pSampler)
	{
		m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].iSamplerRegister = iSamplerRegister;
	}
	else
	{
		//Sampler 설정 안하냐
	}
}

void CMaterial::SetSampler(const string & strKey, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSampler);

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSampler = GET_SINGLE(CResourcesManager)->FindSampler(strKey);
}

void CMaterial::SetSampler(CSampler * pSampler, int iContainer, int iSubset)
{
	if (!m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource)
		m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource = new SHADERRESOURCE;

	SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSampler);

	if (pSampler)
		pSampler->AddRef();

	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->pSampler = pSampler;
}

void CMaterial::SetSamplerRegister(int iRegister, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].pShaderResource->iSamplerRegister = iRegister;
}

bool CMaterial::Save(FILE * pFile)
{
	size_t iContainerCount = GetContainerCount();
	fwrite(&iContainerCount, sizeof(size_t), 1, pFile);
	for (size_t i = 0; i < iContainerCount; ++i)
	{
		size_t iSubsetCount = GetSubsetCount(i);
		fwrite(&iSubsetCount, sizeof(size_t), 1, pFile);

		for (size_t j = 0; j < iSubsetCount; ++j)
		{
			//Save MATERIAL Data
			Vector4 vDiffuse = GetDiffuseColor(i, j);
			Vector4 vAmbient = GetAmbientColor(i, j);
			Vector4 vSpecular = GetSpecularColor(i, j);
			Vector4 vEmissive = GetEmissiveColor(i, j);
			Vector4 vAlpha = GetAlphaColor(i, j);
			fwrite(&vDiffuse, sizeof(Vector4), 1, pFile);
			fwrite(&vAmbient, sizeof(Vector4), 1, pFile);
			fwrite(&vSpecular, sizeof(Vector4), 1, pFile);
			fwrite(&vEmissive, sizeof(Vector4), 1, pFile);
			fwrite(&vAlpha, sizeof(Vector4), 1, pFile);

			//Save SHADERRESOURCE Data
			CTexture* pDiffuseTexture = GetDiffuseTexture(i, j);
			CTexture* pSpecularTexture = GetSpecularTexture(i, j);
			CTexture* pNormalTexture = GetNormalTexture(i, j);
			CTexture* pAlphaTexture = GetAlphaTexture(i, j);
			CSampler* pSampler = GetSampler(i, j);

			bool bHasResources = pDiffuseTexture != NULL;
			fwrite(&bHasResources, sizeof(bool), 1, pFile);
			if (bHasResources)
			{
				TextureSave(pDiffuseTexture, pFile);
				int iDiffuseTextureRegister = GetDiffuseTextureRegister(i, j);
				fwrite(&iDiffuseTextureRegister, sizeof(int), 1, pFile);
			}

			bHasResources = pNormalTexture != NULL;
			fwrite(&bHasResources, sizeof(bool), 1, pFile);
			if (bHasResources)
			{
				TextureSave(pNormalTexture, pFile);
				int iNormalTextureRegister = GetNormalTextureRegister(i, j);
				fwrite(&iNormalTextureRegister, sizeof(int), 1, pFile);
			}

			bHasResources = pSpecularTexture != NULL;
			fwrite(&bHasResources, sizeof(bool), 1, pFile);
			if (bHasResources)
			{
				TextureSave(pSpecularTexture, pFile);
				int iSpecularTextureRegister = GetSpecularTextureRegister(i, j);
				fwrite(&iSpecularTextureRegister, sizeof(int), 1, pFile);
			}

			bHasResources = pAlphaTexture != NULL;
			fwrite(&bHasResources, sizeof(bool), 1, pFile);
			if (bHasResources)
			{
				TextureSave(pAlphaTexture, pFile);
				int iAlphaTextureRegister = GetAlphaTextureRegister(i, j);
				fwrite(&iAlphaTextureRegister, sizeof(int), 1, pFile);
			}

			bHasResources = pSampler != NULL;
			fwrite(&bHasResources, sizeof(bool), 1, pFile);
			if (bHasResources)
			{
				string strSamplerKey = pSampler->GetKey();
				size_t iSamplerKeyLength = strSamplerKey.length();
				int iSamplerRegister = GetSamplerRegister(i, j);
				fwrite(&iSamplerKeyLength, sizeof(size_t), 1, pFile);
				fwrite(strSamplerKey.c_str(), sizeof(char), iSamplerKeyLength, pFile);
				fwrite(&iSamplerRegister, sizeof(int), 1, pFile);
			}


			//Save MultiTexture Data
			size_t iMultiTextureCount = GetMultiTextureCount(i, j);
			fwrite(&iMultiTextureCount, sizeof(size_t), 1, pFile);

			unordered_map<string, SHADERRESOURCE>::const_iterator iter;
			unordered_map<string, SHADERRESOURCE>::const_iterator iterEnd = GetMultiTexture(i, j)->end();
			for (iter = GetMultiTexture(i, j)->begin(); iter != iterEnd; ++iter)
			{
				string strMultiTextureKey = iter->first;
				size_t iMultiTextureKeyLength = strMultiTextureKey.length();
				fwrite(&iMultiTextureKeyLength, sizeof(size_t), 1, pFile);
				fwrite(strMultiTextureKey.c_str(), sizeof(char), iMultiTextureKeyLength, pFile);

				bHasResources = iter->second.pDiffuseTexture != NULL;
				fwrite(&bHasResources, sizeof(bool), 1, pFile);
				if (bHasResources)
				{
					TextureMultiSave(iter->second.pDiffuseTexture, pFile);
					int iRegister = iter->second.iDiffuseTextureRegister;
					fwrite(&iRegister, sizeof(int), 1, pFile);
				}

				bHasResources = iter->second.pNormalTexture != NULL;
				fwrite(&bHasResources, sizeof(bool), 1, pFile);
				if (bHasResources)
				{
					TextureMultiSave(iter->second.pNormalTexture, pFile);
					int iRegister = iter->second.iNormalTextureRegister;
					fwrite(&iRegister, sizeof(int), 1, pFile);
				}

				bHasResources = iter->second.pSpecularTexture != NULL;
				fwrite(&bHasResources, sizeof(bool), 1, pFile);
				if (bHasResources)
				{
					TextureMultiSave(iter->second.pSpecularTexture, pFile);
					int iRegister = iter->second.iSpecularTextureRegister;
					fwrite(&iRegister, sizeof(int), 1, pFile);
				}
				
				bHasResources = iter->second.pAlphaTexture != NULL;
				fwrite(&bHasResources, sizeof(bool), 1, pFile);
				if (bHasResources)
				{
					TextureMultiSave(iter->second.pAlphaTexture, pFile);
					int iRegister = iter->second.iAlphaTextureRegister;
					fwrite(&iRegister, sizeof(int), 1, pFile);
				}

				bHasResources = iter->second.pSampler != NULL;
				fwrite(&bHasResources, sizeof(bool), 1, pFile);
				if (bHasResources)
				{
					string strSamplerKey = iter->second.pSampler->GetKey();
					size_t iSamplerKeyLength = strSamplerKey.length();
					int iSamplerRegister = iter->second.iSamplerRegister;
					fwrite(&iSamplerKeyLength, sizeof(size_t), 1, pFile);
					fwrite(strSamplerKey.c_str(), sizeof(char), iSamplerKeyLength, pFile);
					fwrite(&iSamplerRegister, sizeof(int), 1, pFile);
				}
			}
			SAFE_RELEASE(pDiffuseTexture);
			SAFE_RELEASE(pSpecularTexture);
			SAFE_RELEASE(pNormalTexture);
			SAFE_RELEASE(pAlphaTexture);
			SAFE_RELEASE(pSampler);
		}
	}
	return true;
}

void CMaterial::SetSamplerMultiTextureRegister(const string & strMultiTexKey, int iSamplerRegister, const string & strSamplerKey, int iContainer, int iSubset)
{
	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].iSamplerRegister = iSamplerRegister;

	if (m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pSampler)
		SAFE_RELEASE(m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pSampler);

	m_vecContainer[iContainer].vecSubset[iSubset].mapMultiTexture[strMultiTexKey].pSampler = GET_SINGLE(CResourcesManager)->FindSampler(strSamplerKey);
}

bool CMaterial::Load(FILE * pFile)
{
	ClearContainer();

	size_t iContainerCount = 0;
	fread(&iContainerCount, sizeof(size_t), 1, pFile);

	ResizeContainer(iContainerCount);

	for (size_t i = 0; i < iContainerCount; ++i)
	{
		size_t iSubsetCount = 0;
		fread(&iSubsetCount, sizeof(size_t), 1, pFile);

		ResizeSubset(iSubsetCount, i);

		for (size_t j = 0; j < iSubsetCount; ++j)
		{
			//Save MATERIAL Data
			Vector4 vDiffuse;
			Vector4 vAmbient;
			Vector4 vSpecular;
			Vector4 vEmissive;
			Vector4 vAlpha;
			fread(&vDiffuse, sizeof(Vector4), 1, pFile);
			fread(&vAmbient, sizeof(Vector4), 1, pFile);
			fread(&vSpecular, sizeof(Vector4), 1, pFile);
			fread(&vEmissive, sizeof(Vector4), 1, pFile);
			fread(&vAlpha, sizeof(Vector4), 1, pFile);

			m_vecContainer[i].vecSubset[j].tMaterial.vDiffuse = vDiffuse;
			m_vecContainer[i].vecSubset[j].tMaterial.vAmbient = vAmbient;
			m_vecContainer[i].vecSubset[j].tMaterial.vSpecular = vSpecular;
			m_vecContainer[i].vecSubset[j].tMaterial.vEmissive = vEmissive;
			m_vecContainer[i].vecSubset[j].tMaterial.vAlpha = vAlpha;

			bool bHasResources = false;
			fread(&bHasResources, sizeof(bool), 1, pFile);
			if (bHasResources)
			{
				CTexture* pDiffuseTexture = TextureLoad(pFile);

				int iDiffuseTextureRegister = 0;
				fread(&iDiffuseTextureRegister, sizeof(int), 1, pFile);
				SetDiffuseTexture(pDiffuseTexture, i, j);
				SetDiffuseTextureRegister(iDiffuseTextureRegister, i, j);

				SAFE_RELEASE(pDiffuseTexture);

			}

			bHasResources = false;
			fread(&bHasResources, sizeof(bool), 1, pFile);
			if (bHasResources)
			{
				CTexture* pNormalTexture = TextureLoad(pFile);
				int iNormalTextureRegister = 0;
				fread(&iNormalTextureRegister, sizeof(int), 1, pFile);
				SetNormalTexture(pNormalTexture, i, j);
				SetNormalTextureRegister(iNormalTextureRegister, i, j);

				SAFE_RELEASE(pNormalTexture);
			}

			bHasResources = false;
			fread(&bHasResources, sizeof(bool), 1, pFile);
			if (bHasResources)
			{
				CTexture* pSpecularTexture = TextureLoad(pFile);
				int iSpecularTextureRegister = 0;
				fread(&iSpecularTextureRegister, sizeof(int), 1, pFile);
				SetSpecularTexture(pSpecularTexture, i, j);
				SetSpecularTextureRegister(iSpecularTextureRegister, i, j);

				SAFE_RELEASE(pSpecularTexture);
			}
			
			bHasResources = false;
			fread(&bHasResources, sizeof(bool), 1, pFile);
			if (bHasResources)
			{
				CTexture* pAlphaTexture = TextureLoad(pFile);
				int iAlphaTextureRegister = 0;
				fread(&iAlphaTextureRegister, sizeof(int), 1, pFile);
				SetAlphaTexture(pAlphaTexture, i, j);
				SetAlphaTextureRegister(iAlphaTextureRegister, i, j);

				SAFE_RELEASE(pAlphaTexture);
			}

			bHasResources = false;
			fread(&bHasResources, sizeof(bool), 1, pFile);
			if (bHasResources)
			{
				char strSamplerKey[256] = {};
				size_t iSamplerKeyLength = 0;
				int iSamplerRegister = 0;
				fread(&iSamplerKeyLength, sizeof(size_t), 1, pFile);
				fread(strSamplerKey, sizeof(char), iSamplerKeyLength, pFile);
				fread(&iSamplerRegister, sizeof(int), 1, pFile);

				SetSampler(strSamplerKey, i, j);
				SetSamplerRegister(iSamplerRegister, i, j);
			}

			//Save MultiTexture Data
			size_t iMultiTextureCount = 0;
			fread(&iMultiTextureCount, sizeof(size_t), 1, pFile);

			for (size_t iter = 0; iter < iMultiTextureCount; ++iter)
			{
				char strMultiTextureKey[256] = {};
				size_t iMultiTextureKeyLength = 0;
				fread(&iMultiTextureKeyLength, sizeof(size_t), 1, pFile);
				fread(strMultiTextureKey, sizeof(char), iMultiTextureKeyLength, pFile);

				bHasResources = false;
				fread(&bHasResources, sizeof(bool), 1, pFile);
				if (bHasResources)
				{
					CTexture* pDiffuseTexture = TextureMultiLoad(pFile);
					int iDiffuseTextureRegister = 0;
					fread(&iDiffuseTextureRegister, sizeof(int), 1, pFile);

					SetDiffuseMultiTexture(strMultiTextureKey, pDiffuseTexture->m_strKey, pDiffuseTexture->m_strRelativePath.c_str(), i, j, pDiffuseTexture->m_strPathKey);
					SetDiffuseMultiTextureRegister(strMultiTextureKey, iDiffuseTextureRegister, i, j);

					SAFE_RELEASE(pDiffuseTexture);
				}

				bHasResources = false;
				fread(&bHasResources, sizeof(bool), 1, pFile);
				if (bHasResources)
				{
					CTexture* pNormalTexture = TextureMultiLoad(pFile);
					int iNormalTextureRegister = 0;
					fread(&iNormalTextureRegister, sizeof(int), 1, pFile);

					SetNormalMultiTexture(strMultiTextureKey, pNormalTexture->m_strKey, pNormalTexture->m_strRelativePath.c_str(), i, j, pNormalTexture->m_strPathKey);
					SetNormalMultiTextureRegister(strMultiTextureKey, iNormalTextureRegister, i, j);

					SAFE_RELEASE(pNormalTexture);
				}

				bHasResources = false;
				fread(&bHasResources, sizeof(bool), 1, pFile);
				if (bHasResources)
				{
					CTexture* pSpcularTexture = TextureMultiLoad(pFile);
					int iSpecularTextureRegister = 0;
					fread(&iSpecularTextureRegister, sizeof(int), 1, pFile);

					SetSpecularMultiTexture(strMultiTextureKey, pSpcularTexture->m_strKey, pSpcularTexture->m_strRelativePath.c_str(), i, j, pSpcularTexture->m_strPathKey);
					SetSpecularMultiTextureRegister(strMultiTextureKey, iSpecularTextureRegister, i, j);

					SAFE_RELEASE(pSpcularTexture);
				}


				bHasResources = false;
				fread(&bHasResources, sizeof(bool), 1, pFile);
				if (bHasResources)
				{
					CTexture* pAlphaTexture = TextureMultiLoad(pFile);
					int iAlphaTextureRegister = 0;
					fread(&iAlphaTextureRegister, sizeof(int), 1, pFile);

					SetAlphaMultiTexture(strMultiTextureKey, pAlphaTexture->m_strKey, pAlphaTexture->m_strRelativePath.c_str(), i, j, pAlphaTexture->m_strPathKey);
					SetAlphaMultiTextureRegister(strMultiTextureKey, iAlphaTextureRegister, i, j);

					SAFE_RELEASE(pAlphaTexture);
				}


				bHasResources = false;
				fread(&bHasResources, sizeof(bool), 1, pFile);
				if (bHasResources)
				{
					char strSamplerKey[256] = {};
					size_t iSamplerKeyLength = 0;
					int iSamplerRegister = 0;
					fread(&iSamplerKeyLength, sizeof(size_t), 1, pFile);
					fread(&strSamplerKey, sizeof(char), iSamplerKeyLength, pFile);
					fread(&iSamplerRegister, sizeof(int), 1, pFile);

					SetMultiTextureSampler(strMultiTextureKey, strSamplerKey, i, j);
					SetMultiTextureSamplerRegister(strMultiTextureKey, iSamplerRegister, i, j);
				}
			}
		}
	}
	return true;
}

bool CMaterial::TextureSave(CTexture * pTexture, FILE * pFile)
{
	size_t iKeyLength = pTexture->m_strKey.length();
	size_t iRelativePathLength = pTexture->m_strRelativePath.length();
	size_t iPathKeyLength = pTexture->m_strPathKey.length();

	fwrite(&iKeyLength, sizeof(size_t), 1, pFile);
	fwrite(pTexture->m_strKey.c_str(), sizeof(char), iKeyLength, pFile);

	fwrite(&iRelativePathLength, sizeof(size_t), 1, pFile);
	fwrite(pTexture->m_strRelativePath.c_str(), sizeof(wchar_t), iRelativePathLength, pFile);

	fwrite(&iPathKeyLength, sizeof(size_t), 1, pFile);
	fwrite(pTexture->m_strPathKey.c_str(), sizeof(char), iPathKeyLength, pFile);

	return true;
}

bool CMaterial::TextureMultiSave(CTexture * pTexture, FILE * pFile)
{
	size_t iKeyLength = pTexture->m_strKey.length();
	size_t iPathKeyLength = pTexture->m_strPathKey.length();

	fwrite(&iKeyLength, sizeof(size_t), 1, pFile);
	fwrite(pTexture->m_strKey.c_str(), sizeof(char), iKeyLength, pFile);

	size_t iArraySize = pTexture->m_vecFileName.size();
	fwrite(&iArraySize, sizeof(size_t), 1, pFile);
	for (size_t i = 0; i < pTexture->m_vecFileName.size(); ++i)
	{
		size_t iRelativePathLength = pTexture->m_vecFileName[i].length();
		fwrite(&iRelativePathLength, sizeof(size_t), 1, pFile);
		fwrite(pTexture->m_vecFileName[i].c_str(), sizeof(wchar_t), iRelativePathLength, pFile);
	}

	fwrite(&iPathKeyLength, sizeof(size_t), 1, pFile);
	fwrite(pTexture->m_strPathKey.c_str(), sizeof(char), iPathKeyLength, pFile);

	return true;
}

CTexture * CMaterial::TextureLoad(FILE * pFile)
{
	size_t iKeyLength = 0;
	size_t iRelativePathLength = 0;
	size_t iPathKeyLength = 0;

	char strKey[256] = {};
	wchar_t strRelativePath[512] = {};
	char strPathKey[256] = {};

	fread(&iKeyLength, sizeof(size_t), 1, pFile);
	fread(&strKey[0], sizeof(char), iKeyLength, pFile);

	fread(&iRelativePathLength, sizeof(size_t), 1, pFile);
	fread(&strRelativePath[0], sizeof(wchar_t), iRelativePathLength, pFile);

	fread(&iPathKeyLength, sizeof(size_t), 1, pFile);
	fread(&strPathKey[0], sizeof(char), iPathKeyLength, pFile);

	return  GET_SINGLE(CResourcesManager)->LoadTexture(strKey, strRelativePath, strPathKey);
}

CTexture * CMaterial::TextureMultiLoad(FILE * pFile)
{
	size_t iKeyLength = 0;
	size_t iRelativePathLength = 0;
	size_t iPathKeyLength = 0;

	char strKey[256] = {};
	vector<wstring> vecRelativePath;
	char strPathKey[256] = {};

	fread(&iKeyLength, sizeof(size_t), 1, pFile);
	fread(&strKey[0], sizeof(char), iKeyLength, pFile);

	size_t iArraySize = 0;
	fread(&iArraySize, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iArraySize; ++i)
	{
		size_t iRelativePathLength = 0;
		fread(&iRelativePathLength, sizeof(size_t), 1, pFile);
		wchar_t strBuffer[256] = {};
		fread(&strBuffer, sizeof(wchar_t), iRelativePathLength, pFile);
		vecRelativePath.push_back(strBuffer);
	}

	fread(&iPathKeyLength, sizeof(size_t), 1, pFile);
	fread(&strPathKey[0], sizeof(char), iPathKeyLength, pFile);

	return GET_SINGLE(CResourcesManager)->LoadTexture(strKey, vecRelativePath, strPathKey);
}