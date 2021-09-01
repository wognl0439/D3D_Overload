#include "Mesh.h"
#include "Device.h"
#include "Material.h"
//Transform, Projection 이후 제거. Render 함수 수정
#include "Core.h"
#include "PathManager.h"
#include "ResourcesManager.h"

#include "../NavigationMesh.h"

SSS_USING

CMesh::CMesh():
	m_pMaterial(NULL)
{
	
}

CMesh::~CMesh()
{
	{		
		size_t iBoneSize = m_vecBone.size();

		for (size_t i = 0; i < iBoneSize; ++i)
		{
			SAFE_DELETE(m_vecBone[i]->matBone);
			SAFE_DELETE(m_vecBone[i]->matOffset);
			SAFE_DELETE(m_vecBone[i]);
		}
		m_vecBone.clear();
	}

	vector<PMESHCONTAINER>::iterator iter;
	vector<PMESHCONTAINER>::iterator iterEnd = m_vecMeshContainer.end();

	for (iter = m_vecMeshContainer.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE((*iter)->tVB.pVB);
		SAFE_DELETE_ARRAY((*iter)->tVB.pData);
	
		for (size_t i = 0; i < (*iter)->vecIB.size(); ++i)
		{
			SAFE_RELEASE((*iter)->vecIB[i].pIB);
			SAFE_DELETE_ARRAY((*iter)->vecIB[i].pData);
		}

		SAFE_DELETE((*iter));
	}

	SAFE_RELEASE(m_pMaterial);
	

	size_t iBoneSize = m_vecBone.size();

	for (size_t i = 0; i < iBoneSize; ++i)
	{
		SAFE_DELETE(m_vecBone[i]->matBone);
		SAFE_DELETE(m_vecBone[i]->matOffset);		
	}
	Safe_Delete_VecList(m_vecBone);


	size_t iBoneKeyFrameSize = m_vecAnimationKeyFrame.size();
	for (size_t i = 0; i < iBoneKeyFrameSize; ++i )
	{
		Safe_Delete_VecList(m_vecAnimationKeyFrame[i]->vecKeyFrame);
	}
	Safe_Delete_VecList(m_vecAnimationKeyFrame);

	m_vecMeshContainer.clear();	


	Safe_Delete_VecList(m_vecAnimationClipInfo);
}

void CMesh::SetEnableMeshContainer(int iContainer, bool bEnable)
{
	m_vecMeshContainer[iContainer]->bEnable = bEnable;
}

bool CMesh::GetEnableMeshContainer(int iContainer) const
{
	return m_vecMeshContainer[iContainer]->bEnable;
}

//Mesh가 ResourcesManager에 로드가 되어 있어야 함
bool CMesh::ChangeMeshContainer(const string & strMesh, int iContainer)
{
	CMesh* pMesh = GET_SINGLE(CResourcesManager)->FindMesh(strMesh);
	if (pMesh)
	{
		MATERIALCONTAINER* pMaterialContainer = pMesh->m_pMaterial->GetMaterialContainer(0);

		if (ChangeMeshContainer(pMesh, iContainer))
		{
			SAFE_RELEASE(pMesh);
			return true;
		}
	}
	//메시 검색에 실패함. ResourcesManager에 등록되어 있지 않은 메시임.
	return false;
}
//Mesh가 ResourcesManager에 로드가 되어 있어야 함
bool CMesh::ChangeMeshContainer(CMesh * pMesh, int iContainer)
{
	if (pMesh)
	{
		//기존 버퍼 해제
		SAFE_RELEASE(m_vecMeshContainer[iContainer]->tVB.pVB);
		SAFE_DELETE_ARRAY(m_vecMeshContainer[iContainer]->tVB.pData);

		for (size_t i = 0; i < m_vecMeshContainer[iContainer]->vecIB.size(); ++i)
		{
			SAFE_RELEASE(m_vecMeshContainer[iContainer]->vecIB[i].pIB);
			SAFE_DELETE_ARRAY(m_vecMeshContainer[iContainer]->vecIB[i].pData);
		}
		m_vecMeshContainer[iContainer]->vecIB.clear();

		//재할당
#pragma region Create Buffer
		{
			m_vecMeshContainer[iContainer] = new MESHCONTAINER;
			m_vecMeshContainer[iContainer]->tVB.iCount = pMesh->m_vecMeshContainer[0]->tVB.iCount;
			m_vecMeshContainer[iContainer]->tVB.iSize = pMesh->m_vecMeshContainer[0]->tVB.iSize;
			m_vecMeshContainer[iContainer]->tVB.eUsage = pMesh->m_vecMeshContainer[0]->tVB.eUsage;
			m_vecMeshContainer[iContainer]->tVB.ePrimitiveType = pMesh->m_vecMeshContainer[0]->tVB.ePrimitiveType;


			int iVBLength = pMesh->m_vecMeshContainer[0]->tVB.iSize * pMesh->m_vecMeshContainer[0]->tVB.iCount;
			m_vecMeshContainer[iContainer]->tVB.pData = new char[iVBLength];
			memcpy_s(m_vecMeshContainer[iContainer]->tVB.pData, iVBLength, pMesh->m_vecMeshContainer[0]->tVB.pData, iVBLength);

			D3D11_BUFFER_DESC	tVtxDesc = {};
			tVtxDesc.ByteWidth = m_vecMeshContainer[iContainer]->tVB.iCount * m_vecMeshContainer[iContainer]->tVB.iSize;
			tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			tVtxDesc.Usage = m_vecMeshContainer[iContainer]->tVB.eUsage;

			if (m_vecMeshContainer[iContainer]->tVB.eUsage == D3D11_USAGE_DYNAMIC)
				tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			D3D11_SUBRESOURCE_DATA	tSubRs = {};
			tSubRs.pSysMem = m_vecMeshContainer[iContainer]->tVB.pData;

			// 버퍼를 생성한다.
			DEVICE->CreateBuffer(&tVtxDesc, &tSubRs, &m_vecMeshContainer[iContainer]->tVB.pVB);

			size_t	iSubset = pMesh->m_vecMeshContainer[0]->vecIB.size();
			m_vecMeshContainer[iContainer]->vecIB.resize(iSubset);
			for (size_t i = 0; i < iSubset; ++i)
			{
				m_vecMeshContainer[iContainer]->vecIB[i].iCount = pMesh->m_vecMeshContainer[0]->vecIB[i].iCount;
				m_vecMeshContainer[iContainer]->vecIB[i].iSize = pMesh->m_vecMeshContainer[0]->vecIB[i].iSize;
				m_vecMeshContainer[iContainer]->vecIB[i].eUsage = pMesh->m_vecMeshContainer[0]->vecIB[i].eUsage;
				m_vecMeshContainer[iContainer]->vecIB[i].eFormat = pMesh->m_vecMeshContainer[0]->vecIB[i].eFormat;

				int iIBLength = pMesh->m_vecMeshContainer[0]->vecIB[i].iSize * pMesh->m_vecMeshContainer[0]->vecIB[i].iCount;
				m_vecMeshContainer[iContainer]->vecIB[i].pData = new char[iIBLength];
				memcpy_s(m_vecMeshContainer[iContainer]->vecIB[i].pData, iIBLength, pMesh->m_vecMeshContainer[0]->vecIB[i].pData, iIBLength);

				D3D11_BUFFER_DESC	tIdxDesc = {};
				tIdxDesc.ByteWidth = m_vecMeshContainer[iContainer]->vecIB[i].iCount * m_vecMeshContainer[iContainer]->vecIB[i].iSize;
				tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				tIdxDesc.Usage = m_vecMeshContainer[iContainer]->vecIB[i].eUsage;

				if (m_vecMeshContainer[iContainer]->vecIB[i].eUsage == D3D11_USAGE_DYNAMIC)
					tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

				D3D11_SUBRESOURCE_DATA	tSubRs = {};
				tSubRs.pSysMem = m_vecMeshContainer[iContainer]->vecIB[i].pData;

				// 버퍼를 생성한다.
				DEVICE->CreateBuffer(&tIdxDesc, &tSubRs, &m_vecMeshContainer[iContainer]->vecIB[i].pIB);
			}
		}
#pragma endregion


		MATERIALCONTAINER* pMaterialContainer = pMesh->m_pMaterial->GetMaterialContainer(0);
		
		if (m_pMaterial)
		{
			if (m_pMaterial->ChangeMaterialContainer(pMaterialContainer, iContainer))
			{
				return true;
			}
			else
			{
				//Material Container 교체에 실패함.
				//어짜피 터지므로 이유는 알아야 하니 assert 해줌. 와 넘나 친절한것
				assert(false);
				return false;
			}
		}
		else
		{
			// Mesh에 Material이 존재하지 않아서 Material이 교체되지 않음.
			return true;
		}
	}
	return false;
}

//Mesh가 ResourcesManager에 로드가 되어 있어야 함
bool CMesh::InsertMeshContainer(const string & strMesh, int iContainer)
{
	CMesh* pMesh = GET_SINGLE(CResourcesManager)->FindMesh(strMesh);
	if (pMesh)
	{
		if (InsertMeshContainer(pMesh, iContainer))
		{
			SAFE_RELEASE(pMesh);
			return true;
		}
	}
	return false;
}
//Mesh가 ResourcesManager에 로드가 되어 있어야 함
bool CMesh::InsertMeshContainer(CMesh * pMesh, int iContainer)
{
	//Container의 인덱스가 0보다 크고 Count + 보다 작거나 같을 때만 가능
	if (iContainer >= 0 && iContainer <= (int)m_vecMeshContainer.size())
	{
		PMESHCONTAINER pContainer = new MESHCONTAINER;
		m_vecMeshContainer.insert(m_vecMeshContainer.begin() + iContainer, pContainer);

#pragma region Create Buffer
		{
			pContainer = new MESHCONTAINER;
			pContainer->tVB.iCount = pMesh->m_vecMeshContainer[0]->tVB.iCount;
			pContainer->tVB.iSize = pMesh->m_vecMeshContainer[0]->tVB.iSize;
			pContainer->tVB.eUsage = pMesh->m_vecMeshContainer[0]->tVB.eUsage;
			pContainer->tVB.ePrimitiveType = pMesh->m_vecMeshContainer[0]->tVB.ePrimitiveType;


			int iVBLength = pMesh->m_vecMeshContainer[0]->tVB.iSize * pMesh->m_vecMeshContainer[0]->tVB.iCount;
			pContainer->tVB.pData = new char[iVBLength];
			memcpy_s(pContainer->tVB.pData, iVBLength, pMesh->m_vecMeshContainer[0]->tVB.pData, iVBLength);

			D3D11_BUFFER_DESC	tVtxDesc = {};
			tVtxDesc.ByteWidth = iVBLength;
			tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			tVtxDesc.Usage = pContainer->tVB.eUsage;

			if (pContainer->tVB.eUsage == D3D11_USAGE_DYNAMIC)
				tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			D3D11_SUBRESOURCE_DATA	tSubRs = {};
			tSubRs.pSysMem = pContainer->tVB.pData;

			// 버퍼를 생성한다.
			DEVICE->CreateBuffer(&tVtxDesc, &tSubRs, &pContainer->tVB.pVB);

			size_t	iSubset = pMesh->m_vecMeshContainer[0]->vecIB.size();
			pContainer->vecIB.resize(iSubset);
			for (size_t i = 0; i < iSubset; ++i)
			{
				pContainer->vecIB[i].iCount = pMesh->m_vecMeshContainer[0]->vecIB[i].iCount;
				pContainer->vecIB[i].iSize = pMesh->m_vecMeshContainer[0]->vecIB[i].iSize;
				pContainer->vecIB[i].eUsage = pMesh->m_vecMeshContainer[0]->vecIB[i].eUsage;
				pContainer->vecIB[i].eFormat = pMesh->m_vecMeshContainer[0]->vecIB[i].eFormat;

				int iIBLength = pMesh->m_vecMeshContainer[0]->vecIB[i].iSize * pMesh->m_vecMeshContainer[0]->vecIB[i].iCount;
				pContainer->vecIB[i].pData = new char[iIBLength];
				memcpy_s(pContainer->vecIB[i].pData, iIBLength, pMesh->m_vecMeshContainer[0]->vecIB[i].pData, iIBLength);

				D3D11_BUFFER_DESC	tIdxDesc = {};
				tIdxDesc.ByteWidth = pContainer->vecIB[i].iCount * pContainer->vecIB[i].iSize;
				tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				tIdxDesc.Usage = pContainer->vecIB[i].eUsage;

				if (pContainer->vecIB[i].eUsage == D3D11_USAGE_DYNAMIC)
					tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

				D3D11_SUBRESOURCE_DATA	tSubRs = {};
				tSubRs.pSysMem = pContainer->vecIB[i].pData;

				// 버퍼를 생성한다.
				DEVICE->CreateBuffer(&tIdxDesc, &tSubRs, &pContainer->vecIB[i].pIB);
			}
		}
#pragma endregion

		MATERIALCONTAINER* pMaterialContainer = pMesh->m_pMaterial->GetMaterialContainer(0);
		m_pMaterial->InsertMaterialContainer(pMaterialContainer, iContainer);

		return true;
	}

	return false;
}

bool CMesh::RemoveMeshContainer(int iContainer)
{
	//기존 버퍼 해제
	SAFE_RELEASE(m_vecMeshContainer[iContainer]->tVB.pVB);
	SAFE_DELETE_ARRAY(m_vecMeshContainer[iContainer]->tVB.pData);

	for (size_t i = 0; i < m_vecMeshContainer[iContainer]->vecIB.size(); ++i)
	{
		SAFE_RELEASE(m_vecMeshContainer[iContainer]->vecIB[i].pIB);
		SAFE_DELETE_ARRAY(m_vecMeshContainer[iContainer]->vecIB[i].pData);
	}
	m_vecMeshContainer[iContainer]->vecIB.clear();

	m_vecMeshContainer.erase(m_vecMeshContainer.begin() + iContainer);


	if (m_pMaterial)
	{
		m_pMaterial->RemoveMaterialContainer(iContainer);
	}

	return false;
}

Matrix * CMesh::GetLocalMatrix(int iContainer) const
{
	return &m_vecMeshContainer[iContainer]->matLocal;
}

bool CMesh::HasAnyKeyFrame() const
{
	size_t iBoneCount = m_vecAnimationKeyFrame.size();

	for (size_t i = 0; i < iBoneCount; ++i)
	{
		size_t iClipLength = m_vecAnimationKeyFrame[i]->vecKeyFrame.size();

		if (iClipLength != 0)
			return true;
	}

	return false;
}

void CMesh::AddAnimationClip(const string& strClipName, size_t iStartFrame, size_t iEndFrame)
{
	PANIMATIONCLIP pClip = new ANIMATIONCLIP;
	pClip->strName = strClipName;
	pClip->iStartFrame = iStartFrame;
	pClip->iEndFrame = iEndFrame;
	pClip->iLength = iEndFrame - iStartFrame + 1;

	m_vecAnimationClipInfo.push_back(pClip);
}

void CMesh::RemoveAnimationClip(const string & strClipName)
{
	vector<PANIMATIONCLIP>::iterator iter;
	vector<PANIMATIONCLIP>::iterator iterEnd = m_vecAnimationClipInfo.end();
	for (iter = m_vecAnimationClipInfo.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->strName == strClipName)
		{
			m_vecAnimationClipInfo.erase(iter);
			return;
		}
	}
}

void CMesh::RemoveAnimationClip(size_t iIndex)
{
	size_t iSize = m_vecAnimationClipInfo.size();

	if (iIndex >= 0 && iIndex < iSize)
	{
		vector<PANIMATIONCLIP>::iterator iter = m_vecAnimationClipInfo.begin() + iIndex;
		m_vecAnimationClipInfo.erase(iter);
	}
}

PANIMATIONCLIP CMesh::GetAnimationClipInfo(const string & strClipName)
{
	size_t iSize = m_vecAnimationClipInfo.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		if (m_vecAnimationClipInfo[i]->strName == strClipName)
		{
			return m_vecAnimationClipInfo[i];
		}
	}

	return NULL;
}

PANIMATIONCLIP CMesh::GetAnimationClipInfo(size_t iIndex)
{
	size_t iSize = m_vecAnimationClipInfo.size();

	if (iIndex >= 0 && iIndex < iSize)
	{
		return m_vecAnimationClipInfo[iIndex];
	}

	return NULL;
}

vector<PANIMATIONCLIP>* CMesh::GetAnimationClipInfo()
{
	return &m_vecAnimationClipInfo;
}

void CMesh::SetMaterial(CMaterial * pMaterial)
{
	SAFE_RELEASE(m_pMaterial);
	if (pMaterial)
	{
		pMaterial->AddRef();
		m_pMaterial = pMaterial;
	}
}

CMaterial * CMesh::GetMaterial() const
{
	if (m_pMaterial)
	{
		m_pMaterial->AddRef();
	}
	return m_pMaterial;
}

void CMesh::GetMeshPosition(vector<Vector3>* vecPos, int iContainer)
{
	// 정점 수만큼 반복한다.
	char*	pVtx = (char*)m_vecMeshContainer[iContainer]->tVB.pData;
	for (int i = 0; i < m_vecMeshContainer[iContainer]->tVB.iCount; ++i)
	{
		int	iSize = m_vecMeshContainer[iContainer]->tVB.iSize;

		Vector3	vPos;
		memcpy(&vPos, pVtx + (i * iSize), sizeof(Vector3));

		vecPos->push_back(vPos);
	}
}

void CMesh::GetMeshIndex(vector<int>* vecIndex, int iContainer, int iSubset)
{
	char*	pIdx = (char*)m_vecMeshContainer[iContainer]->vecIB[iSubset].pData;

	for (int i = 0; i < m_vecMeshContainer[iContainer]->vecIB[iSubset].iCount; ++i)
	{
		switch (m_vecMeshContainer[iContainer]->vecIB[iSubset].iSize)
		{
		case 2:
		{
			short	sIdx;
			memcpy(&sIdx, pIdx + (i * 2), 2);
			vecIndex->push_back(sIdx);
		}
		break;
		case 4:
		{
			int	iIdx;
			memcpy(&iIdx, pIdx + (i * 4), 4);
			vecIndex->push_back(iIdx);
		}
		break;
		}
	}
}

void CMesh::CreateNaviMeshGridInfo(CNavigationMesh * pNaviMesh, const Vector3 & vScale, const Vector3 & vLandPos, int iRectCount)
{
	for (size_t i = 0; i < m_vecMeshContainer.size(); ++i)
	{
		vector<Vector3>	vecPos;

		GetMeshPosition(&vecPos, i);

		// 메쉬에서 인덱스 정보를 얻어온다.
		for (size_t j = 0; j < m_vecMeshContainer[i]->vecIB.size(); ++j)
		{
			vector<int>		vecIndex;
			GetMeshIndex(&vecIndex, i, j);


			for (size_t k = 0; k < vecIndex.size(); k += 3)
			{
				Vector3	vPos[3];
				vPos[0] = vecPos[vecIndex[k]] * vScale + vLandPos;
				vPos[1] = vecPos[vecIndex[k + 1]] * vScale + vLandPos;
				vPos[2] = vecPos[vecIndex[k + 2]] * vScale + vLandPos;

				pNaviMesh->AddCell(vPos);
			}

			// 격자맵일 경우 규칙이 정해져있기때문에 그 규칙에 의해서 인접정보를 만들어낸다.
			//pNaviMesh->CreateGridMapAdj(128);
			vecIndex.clear();
		}

		vecPos.clear();
	}

	pNaviMesh->CreateGridMapAdj(iRectCount);
}

void CMesh::CreateNaviMeshInfo(CNavigationMesh * pNaviMesh, const Vector3 & vScale, const Vector3 & vLandPos)
{
	for (size_t i = 0; i < m_vecMeshContainer.size(); ++i)
	{
		vector<Vector3>	vecPos;

		GetMeshPosition(&vecPos, i);

		// 메쉬에서 인덱스 정보를 얻어온다.
		for (size_t j = 0; j < m_vecMeshContainer[i]->vecIB.size(); ++j)
		{
			vector<int>		vecIndex;
			GetMeshIndex(&vecIndex, i, j);


			for (size_t k = 0; k < vecIndex.size(); k += 3)
			{
				Vector3	vPos[3];
				vPos[0] = vecPos[vecIndex[k]] * vScale + vLandPos;
				vPos[1] = vecPos[vecIndex[k + 1]] * vScale + vLandPos;
				vPos[2] = vecPos[vecIndex[k + 2]] * vScale + vLandPos;

				pNaviMesh->AddCell(vPos);
			}

			// 격자맵일 경우 규칙이 정해져있기때문에 그 규칙에 의해서 인접정보를 만들어낸다.
			//pNaviMesh->CreateGridMapAdj(128);
			vecIndex.clear();
		}

		vecPos.clear();
	}

	pNaviMesh->CreateAdj();
}

const vector<PBONEKEYFRAME>* CMesh::GetAnimationClip() const
{
	return &m_vecAnimationKeyFrame;
}

size_t CMesh::GetAnimationLength()
{
	size_t iMax = 0;
	size_t iCount = m_vecAnimationKeyFrame.size();
	for (size_t i = 0; i < iCount; ++i)
	{
		size_t iSize = m_vecAnimationKeyFrame[i]->vecKeyFrame.size();
		if (iSize > iMax)
		{
			iMax = iSize;
		}
	}
	return iMax;
}


const vector<PBONE>* CMesh::GetBone() const
{
	return &m_vecBone;
}

size_t CMesh::GetBoneCount() const
{
	return m_vecBone.size();
}

void CMesh::SetClipInfo(PFBXANIMATIONCLIP pClip)
{
	ANIMATIONCLIP tInfo = {};

	tInfo.strName = pClip->strName;

	// FBXANIMATIONCLIP에 있는 starttime 과 endtime 을 이용하여 keyframe 을 얻어온다.
	tInfo.iStartFrame = pClip->tStart.GetFrameCount(pClip->eTimeMode);
	tInfo.iEndFrame = pClip->tEnd.GetFrameCount(pClip->eTimeMode);
	tInfo.iLength = tInfo.iEndFrame - tInfo.iStartFrame;

	// 키 프레임 수만큼 반복하며 각각의 프레임을 보간할 행렬 정보를 위치, 크기, 회전정보로
	// 뽑아온다.
	for (size_t i = 0; i < pClip->vecBoneKeyFrame.size(); ++i)
	{
		PBONEKEYFRAME	pBoneKeyFrame = new BONEKEYFRAME;

		pBoneKeyFrame->iBoneIndex = pClip->vecBoneKeyFrame[i].iBoneIndex;

		m_vecAnimationKeyFrame.push_back(pBoneKeyFrame);

		// 아래부터 키프레임 정보를 저장한다.
		pBoneKeyFrame->vecKeyFrame.reserve(pClip->vecBoneKeyFrame[i].vecKeyFrame.size());

		for (size_t j = 0; j < pClip->vecBoneKeyFrame[i].vecKeyFrame.size(); ++j)
		{
			PKEYFRAME	pKeyFrame = new KEYFRAME;

			pKeyFrame->dTime = pClip->vecBoneKeyFrame[i].vecKeyFrame[j].dTime;

			// 현재 본의 키 프레임에 해당하는 행렬 정보를 얻어온다.
			FbxAMatrix mat = pClip->vecBoneKeyFrame[i].vecKeyFrame[j].matTransform;
			FbxVector4	vPos, vScale;
			FbxQuaternion	qRot;

			// 행렬로부터 위치, 크기, 회전 정보를 얻어온다.
			vPos = mat.GetT();
			vScale = mat.GetS();
			qRot = mat.GetQ();

			pKeyFrame->vScale = Vector3((float)vScale.mData[0], (float)vScale.mData[1], (float)vScale.mData[2]);
			pKeyFrame->vPos = Vector3((float)vPos.mData[0], (float)vPos.mData[1], (float)vPos.mData[2]);
			pKeyFrame->vRot = Vector4((float)qRot.mData[0], (float)qRot.mData[1], (float)qRot.mData[2], (float)qRot.mData[3]);
		
			pBoneKeyFrame->vecKeyFrame.push_back(pKeyFrame);
		}
	}
}



void CMesh::SetClipInfo(PBONEKEYFRAME pClip)
{
}

void CMesh::SetClipInfo(wchar_t * pFileName, const string & strPathKey)
{
}

float CMesh::GetRadius() const
{
	return m_fRadius;
}

Vector3 CMesh::GetMin() const
{
	return m_vMin;
}

Vector3 CMesh::GetMax() const
{
	return m_vMax;
}

Vector3 CMesh::GetLength() const
{
	return m_vLength;
}

Vector3 CMesh::GetCenter() const
{
	return m_vCenter;
}

const string & CMesh::GetKey() const
{
	return m_strKey;
}

const string & CMesh::GetDirectory() const
{
	return m_strDirectory;
}

const string& CMesh::GetShaderKey() const
{
	return m_strShaderKey;
}

const string& CMesh::GetLayoutKey() const
{
	return m_strLayoutKey;
}

const string & CMesh::GetPathKey() const
{
	return m_strPathKey;
}

size_t CMesh::GetContainerCount() const
{
	return m_vecMeshContainer.size();
}

size_t CMesh::GetSubsetCount(size_t iContainerIndex) const
{
	return m_vecMeshContainer[iContainerIndex]->vecIB.size();
}

size_t CMesh::GetVertexCount() const
{
	size_t iContainerCount = m_vecMeshContainer.size();

	size_t iCount = 0;
	for (size_t i = 0; i < iContainerCount; ++i)
	{
		iCount += m_vecMeshContainer[i]->tVB.iCount;
	}
	return iCount;
}

size_t CMesh::GetVertexCount(size_t iContainer) const
{
	return m_vecMeshContainer[iContainer]->tVB.iCount;
}

bool CMesh::CreateMesh(const string & strKey, const string& strShaderKey, const string& strInputLayoutKey, UINT iVertexCount, UINT iVertexSize, D3D11_USAGE eVertexUsage,
	void * pVertexData, D3D11_PRIMITIVE_TOPOLOGY ePrimitiveType, UINT iIndexCount, UINT iIndexSize,
	D3D11_USAGE eIndexUsage, DXGI_FORMAT eIndexFormat, void * pIndexData)
{
	m_strShaderKey = strShaderKey;
	m_strLayoutKey = strInputLayoutKey;

	SetTag(strKey);

	PMESHCONTAINER pContainer = new MESHCONTAINER;

	m_vecMeshContainer.push_back(pContainer);

	if (!CreateVertexBuffer(iVertexCount, iVertexSize, eVertexUsage, pVertexData,ePrimitiveType, pContainer))	
	{
		return false;
	}

	if (!CreateIndexBuffer(iIndexCount, iIndexSize, eIndexUsage, eIndexFormat, pIndexData, pContainer))
	{
		return false;
	}


	return true;
}

void CMesh::Render(int iContainer, int iSubset)
{
	UINT	iOffset = 0;
	PMESHCONTAINER	pContainer = m_vecMeshContainer[iContainer];
	
	// 도형 위상구조를 설정한다.
	DEVICE_CONTEXT->IASetPrimitiveTopology(pContainer->tVB.ePrimitiveType);
	DEVICE_CONTEXT->IASetVertexBuffers(0, 1, &pContainer->tVB.pVB, &pContainer->tVB.iSize, &iOffset);

	if (pContainer->vecIB.empty())
	{
		DEVICE_CONTEXT->Draw(pContainer->tVB.iCount, 0);
	}
	else
	{
		DEVICE_CONTEXT->IASetIndexBuffer(pContainer->vecIB[iSubset].pIB, pContainer->vecIB[iSubset].eFormat, 0);
		DEVICE_CONTEXT->DrawIndexed(pContainer->vecIB[iSubset].iCount, 0, 0);	
	}
}

void CMesh::Render()
{
	for (size_t i = 0; i < m_vecMeshContainer.size(); ++i)
	{
		UINT	iOffset = 0;
		PMESHCONTAINER	pContainer = m_vecMeshContainer[i];


		// 도형 위상구조를 설정한다.
		DEVICE_CONTEXT->IASetPrimitiveTopology(pContainer->tVB.ePrimitiveType);
		DEVICE_CONTEXT->IASetVertexBuffers(0, 1, &pContainer->tVB.pVB, &pContainer->tVB.iSize, &iOffset);
		
		if (pContainer->vecIB.empty())
		{
			DEVICE_CONTEXT->Draw(pContainer->tVB.iCount, 0);
		}
		else
		{
			for (size_t j = 0; j < pContainer->vecIB.size(); ++j)
			{
				DEVICE_CONTEXT->IASetIndexBuffer(	pContainer->vecIB[j].pIB, pContainer->vecIB[j].eFormat,	0);
				DEVICE_CONTEXT->DrawIndexed(pContainer->vecIB[j].iCount, 0, 0);
			}
		}
	}
}

void CMesh::UpdateVertexBuffer(void * pVtxData, size_t iSize)
{
	for (size_t i = 0; i < m_vecMeshContainer.size(); ++i)
	{
		int	iCount = m_vecMeshContainer[i]->tVB.iCount;

		if (iSize > 0)
			iCount = iSize;

		int	 iSize = m_vecMeshContainer[i]->tVB.iSize * iCount;

		memcpy(m_vecMeshContainer[i]->tVB.pData, pVtxData, iSize);

		switch (m_vecMeshContainer[i]->tVB.eUsage)
		{
		case D3D11_USAGE_DEFAULT:
			DEVICE_CONTEXT->UpdateSubresource(m_vecMeshContainer[i]->tVB.pVB,
				0, NULL, pVtxData, 0, 0);
			break;
		case D3D11_USAGE_DYNAMIC:
		case D3D11_USAGE_STAGING:
		{
			D3D11_MAPPED_SUBRESOURCE	tMap = {};
			DEVICE_CONTEXT->Map(m_vecMeshContainer[i]->tVB.pVB, 0,
				D3D11_MAP_WRITE_DISCARD, 0, &tMap);

			memcpy(tMap.pData, pVtxData, iSize);

			DEVICE_CONTEXT->Unmap(m_vecMeshContainer[i]->tVB.pVB, 0);
		}
		break;
		}
	}
}

void CMesh::MoveVertexBuffer(const Vector3 & vMove)
{
	for (size_t i = 0; i < m_vecMeshContainer.size(); ++i)
	{
		int	iSize = m_vecMeshContainer[i]->tVB.iSize *
			m_vecMeshContainer[i]->tVB.iCount;

		char*	pData = (char*)m_vecMeshContainer[i]->tVB.pData;

		for (int j = 0; j < m_vecMeshContainer[i]->tVB.iCount; ++j)
		{
			PVector3	pPos = (PVector3)(pData + (j * m_vecMeshContainer[i]->tVB.iSize));

			(*pPos) += vMove;
		}

		switch (m_vecMeshContainer[i]->tVB.eUsage)
		{
		case D3D11_USAGE_DEFAULT:
			DEVICE_CONTEXT->UpdateSubresource(m_vecMeshContainer[i]->tVB.pVB,
				0, NULL, m_vecMeshContainer[i]->tVB.pData, 0, 0);
			break;
		case D3D11_USAGE_DYNAMIC:
		case D3D11_USAGE_STAGING:
		{
			D3D11_MAPPED_SUBRESOURCE	tMap = {};
			DEVICE_CONTEXT->Map(m_vecMeshContainer[i]->tVB.pVB, 0,
				D3D11_MAP_WRITE_DISCARD, 0, &tMap);

			memcpy(tMap.pData, m_vecMeshContainer[i]->tVB.pData,
				iSize);

			DEVICE_CONTEXT->Unmap(m_vecMeshContainer[i]->tVB.pVB, 0);
		}
		break;
		}
	}
}

//Full Path 사용할 경우 PathKey 에 NO_USE_PATH 사용할 것
void CMesh::Save(const wchar_t * pFileName, const string & strPathKey)
{
	wstring strFullPathW;

	if (!strPathKey.empty() && strPathKey != NO_USE_PATH)
	{
		strFullPathW = GET_SINGLE(CPathManager)->FindPath(strPathKey);
	}
	
	strFullPathW += pFileName;
	
	string strFullPathA(strFullPathW.begin(), strFullPathW.end());

	FILE* pFile = NULL;

	fopen_s(&pFile, strFullPathA.c_str(), "wb");

	if (pFile)
	{
		//기존에 Radius 값 계산 제대로 안되던 부분 때문에 다시계산함.
		m_fRadius = m_vLength.Max();

		fwrite(&m_vMin, sizeof(Vector3), 1, pFile);
		fwrite(&m_vMax, sizeof(Vector3), 1, pFile);
		fwrite(&m_vCenter, sizeof(Vector3), 1, pFile);
		fwrite(&m_vLength, sizeof(Vector3), 1, pFile);
		fwrite(&m_fRadius, sizeof(float), 1, pFile);


		//Shader와 Layout은 반드시 Material이 없을 수 없기 때문에, Material이 가진 정보를 사용하도록 한다.		
		size_t	iSize = m_vecMeshContainer.size();
		fwrite(&iSize, sizeof(size_t), 1, pFile);
		for (size_t i = 0; i < iSize; ++i)
		{
			fwrite(&m_vecMeshContainer[i]->tVB.iCount, sizeof(UINT), 1, pFile);
			fwrite(&m_vecMeshContainer[i]->tVB.iSize, sizeof(UINT), 1, pFile);
			fwrite(&m_vecMeshContainer[i]->tVB.eUsage, sizeof(D3D11_USAGE), 1, pFile);
			fwrite(&m_vecMeshContainer[i]->tVB.ePrimitiveType, sizeof(D3D11_PRIMITIVE_TOPOLOGY), 1, pFile);
			fwrite(m_vecMeshContainer[i]->tVB.pData, m_vecMeshContainer[i]->tVB.iSize, m_vecMeshContainer[i]->tVB.iCount, pFile);

			size_t	iSubset = m_vecMeshContainer[i]->vecIB.size();
			fwrite(&iSubset, sizeof(size_t), 1, pFile);
			for (size_t j = 0; j < iSubset; ++j)
			{
				fwrite(&m_vecMeshContainer[i]->vecIB[j].iCount, sizeof(UINT), 1, pFile);
				fwrite(&m_vecMeshContainer[i]->vecIB[j].iSize, sizeof(UINT), 1, pFile);
				fwrite(&m_vecMeshContainer[i]->vecIB[j].eUsage, sizeof(D3D11_USAGE), 1, pFile);
				fwrite(&m_vecMeshContainer[i]->vecIB[j].eFormat, sizeof(DXGI_FORMAT), 1, pFile);
				fwrite(m_vecMeshContainer[i]->vecIB[j].pData, m_vecMeshContainer[i]->vecIB[j].iSize, m_vecMeshContainer[i]->vecIB[j].iCount, pFile);
			}
		}

		//Save Bone Data 
		bool bHasBone = false;
		if (m_vecBone.size() != 0)
		{
			bHasBone = true;
		}
		fwrite(&bHasBone, sizeof(bool), 1, pFile);

		if (bHasBone)
		{
			size_t iBoneCount = m_vecBone.size();
			fwrite(&iBoneCount, sizeof(size_t), 1, pFile);

			for (size_t i = 0; i < iBoneCount; ++i)
			{
				size_t iBoneNameLength = m_vecBone[i]->strName.length();
				fwrite(&iBoneNameLength, sizeof(size_t), 1, pFile);
				fwrite(m_vecBone[i]->strName.c_str(), sizeof(char), iBoneNameLength, pFile);
				fwrite(&m_vecBone[i]->iDepth, sizeof(int), 1, pFile);
				fwrite(&m_vecBone[i]->iParentIndex, sizeof(int), 1, pFile);
				fwrite(m_vecBone[i]->matOffset, sizeof(Matrix), 1, pFile);
				fwrite(m_vecBone[i]->matBone, sizeof(Matrix), 1, pFile);				 
				//Transform 은 해당 오브젝트가 가진 컴포넌트들만 알면 됨.
				//Mesh는 Resource이므로 위와 같은 정보는 리소스 범위에 포함되지 않으므로 저장하지 않음.
				//해당 정보들은 오브젝트 수준에서 저장하도록 함.
			}
		}


		bool bHasAnimationKeyFrame = HasAnyKeyFrame();
		fwrite(&bHasAnimationKeyFrame, sizeof(bool), 1, pFile);

		if (bHasAnimationKeyFrame)
		{
			size_t iBoneCount = m_vecAnimationKeyFrame.size();
			fwrite(&iBoneCount, sizeof(size_t), 1, pFile);
			for (size_t i = 0; i < iBoneCount; ++i)
			{
				size_t iBoneIndex = m_vecAnimationKeyFrame[i]->iBoneIndex;
				size_t iKeyFrameCount = m_vecAnimationKeyFrame[i]->vecKeyFrame.size();
				fwrite(&iBoneIndex, sizeof(size_t), 1, pFile);
				fwrite(&iKeyFrameCount, sizeof(size_t), 1, pFile);

				for (size_t j = 0; j < iKeyFrameCount; ++j)
				{
					fwrite(&m_vecAnimationKeyFrame[i]->vecKeyFrame[j]->dTime, sizeof(double), 1, pFile);
					fwrite(&m_vecAnimationKeyFrame[i]->vecKeyFrame[j]->vPos, sizeof(Vector3), 1, pFile);
					fwrite(&m_vecAnimationKeyFrame[i]->vecKeyFrame[j]->vRot, sizeof(Vector4), 1, pFile);
					fwrite(&m_vecAnimationKeyFrame[i]->vecKeyFrame[j]->vScale, sizeof(Vector3), 1, pFile);
				}
			}
		}


		bool bHasClipInfo = m_vecAnimationClipInfo.size() != 0;
		fwrite(&bHasClipInfo, sizeof(bool), 1, pFile);
		if (bHasClipInfo)
		{
			size_t iClipCount = m_vecAnimationClipInfo.size();
			fwrite(&iClipCount, sizeof(size_t), 1, pFile);
			for (size_t i = 0; i < iClipCount; ++i)
			{
				size_t iClipNameLength = m_vecAnimationClipInfo[i]->strName.length();
				fwrite(&iClipNameLength, sizeof(size_t), 1, pFile);
				fwrite(m_vecAnimationClipInfo[i]->strName.c_str(), sizeof(char), iClipNameLength, pFile);

				fwrite(&m_vecAnimationClipInfo[i]->iStartFrame, sizeof(size_t), 1, pFile);
				fwrite(&m_vecAnimationClipInfo[i]->iEndFrame, sizeof(size_t), 1, pFile);
				fwrite(&m_vecAnimationClipInfo[i]->iLength, sizeof(size_t), 1, pFile);
			}
		}

		fclose(pFile);
	}
}

//만약에 strKey가 string::empty와 같으면 내부에서 pFileName을 기반으로 키를 생성함.
//strPathKey를 사용하지 않는다면 NO_USE_PATH 넣으면 pFileName을 FullPath로 처리하도록 함.
CMesh* CMesh::Load(const string & strKey, const wchar_t * pFileName, const string & strPathKey)
{
	wstring strFullPathW;

	if (strPathKey != NO_USE_PATH)
	{
		strFullPathW = GET_SINGLE(CPathManager)->FindPath(strPathKey);
	}
	
	strFullPathW += pFileName;

	string strFullPathA(strFullPathW.begin(), strFullPathW.end());

	
	string strIntrinsicKey;
	string strIntrinsicRelativeDirectory;
	string strIntrinsicPathKey;

	char* pRelativePath = NULL;
	char* pPathKey = NULL;
	GET_SINGLE(CPathManager)->ExtractRelativeDirectoryFromFullPath(strFullPathA.c_str(), &pRelativePath, &pPathKey);

	strIntrinsicRelativeDirectory = pRelativePath;

	if (strKey.empty())
	{
		strIntrinsicKey = pRelativePath;
		if (strPathKey == NO_USE_PATH || strPathKey.empty())
		{
			strIntrinsicPathKey = pPathKey;
		}
		else
		{
			strIntrinsicPathKey = strPathKey;
		}
	}
	else
	{
		strIntrinsicKey = strKey;
		if (strPathKey == NO_USE_PATH || strPathKey.empty())
		{
			strIntrinsicPathKey = pPathKey;
		}
		else
		{
			strIntrinsicPathKey = strPathKey;
		}
	}

	FILE* pFile = NULL;

	fopen_s(&pFile, strFullPathA.c_str(), "rb");

	if (pFile)
	{
		m_strKey = strIntrinsicKey;
		m_strPathKey = strIntrinsicPathKey;
		m_strDirectory = strIntrinsicKey;


		fread(&m_vMin, sizeof(Vector3), 1, pFile);
		fread(&m_vMax, sizeof(Vector3), 1, pFile);
		fread(&m_vCenter, sizeof(Vector3), 1, pFile);
		fread(&m_vLength, sizeof(Vector3), 1, pFile);
		fread(&m_fRadius, sizeof(float), 1, pFile);

		m_fRadius = m_vLength.Max();

		//Shader와 Layout은 반드시 Material이 없을 수 없기 때문에, Material이 가진 정보를 사용하도록 한다.		
		size_t	iSize = 0;
		fread(&iSize, sizeof(size_t), 1, pFile);
		for (size_t i = 0; i < iSize; ++i)
		{
			PMESHCONTAINER	pContainer = new MESHCONTAINER;
			m_vecMeshContainer.push_back(pContainer);

			fread(&pContainer->tVB.iCount, sizeof(UINT), 1, pFile);
			fread(&pContainer->tVB.iSize, sizeof(UINT), 1, pFile);
			fread(&pContainer->tVB.eUsage, sizeof(D3D11_USAGE), 1, pFile);
			fread(&pContainer->tVB.ePrimitiveType, sizeof(D3D11_PRIMITIVE_TOPOLOGY), 1, pFile);
			pContainer->tVB.pData = new char[pContainer->tVB.iSize * pContainer->tVB.iCount];
			fread(pContainer->tVB.pData, pContainer->tVB.iSize, pContainer->tVB.iCount, pFile);

			D3D11_BUFFER_DESC	tVtxDesc = {};
			tVtxDesc.ByteWidth = pContainer->tVB.iCount * pContainer->tVB.iSize;
			tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			tVtxDesc.Usage = pContainer->tVB.eUsage;

			if (pContainer->tVB.eUsage == D3D11_USAGE_DYNAMIC)
				tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			D3D11_SUBRESOURCE_DATA	tSubRs = {};
			tSubRs.pSysMem = pContainer->tVB.pData;

			// 버퍼를 생성한다.
			DEVICE->CreateBuffer(&tVtxDesc, &tSubRs, &pContainer->tVB.pVB);

			size_t	iSubset = 0;
			fread(&iSubset, sizeof(size_t), 1, pFile);
			pContainer->vecIB.resize(iSubset);
			for (size_t j = 0; j < iSubset; ++j)
			{
				// 인덱스버퍼 저장
				fread(&pContainer->vecIB[j].iCount, sizeof(UINT), 1, pFile);
				fread(&pContainer->vecIB[j].iSize, sizeof(UINT), 1, pFile);
				fread(&pContainer->vecIB[j].eUsage, sizeof(D3D11_USAGE), 1, pFile);
				fread(&pContainer->vecIB[j].eFormat, sizeof(DXGI_FORMAT), 1, pFile);
				pContainer->vecIB[j].pData = new char[pContainer->vecIB[j].iSize * pContainer->vecIB[j].iCount];
				fread(pContainer->vecIB[j].pData, pContainer->vecIB[j].iSize, pContainer->vecIB[j].iCount, pFile);

				D3D11_BUFFER_DESC	tIdxDesc = {};
				tIdxDesc.ByteWidth = pContainer->vecIB[j].iCount * pContainer->vecIB[j].iSize;
				tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				tIdxDesc.Usage = pContainer->vecIB[j].eUsage;

				if (pContainer->vecIB[j].eUsage == D3D11_USAGE_DYNAMIC)
					tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

				D3D11_SUBRESOURCE_DATA	tSubRs = {};
				tSubRs.pSysMem = pContainer->vecIB[j].pData;

				// 버퍼를 생성한다.
				DEVICE->CreateBuffer(&tIdxDesc, &tSubRs, &pContainer->vecIB[j].pIB);
			}
		}

		//Load Bone Data 
		bool bHasBone = false;
		fread(&bHasBone, sizeof(bool), 1, pFile);
		if (bHasBone)
		{
			size_t iBoneCount = 0;
			fread(&iBoneCount, sizeof(size_t), 1, pFile);
			for (size_t i = 0; i < iBoneCount; ++i)
			{
				PBONE pBone = new BONE;
				pBone->matBone = new Matrix;
				pBone->matOffset = new Matrix;
				size_t iBoneNameLength = 0;
				char strBoneName[256] = {};
				fread(&iBoneNameLength, sizeof(size_t), 1, pFile);
				fread(&strBoneName[0], sizeof(char), iBoneNameLength, pFile);
				fread(&pBone->iDepth, sizeof(int), 1, pFile);
				fread(&pBone->iParentIndex, sizeof(int), 1, pFile);
				fread(pBone->matOffset, sizeof(Matrix), 1, pFile);
				fread(pBone->matBone, sizeof(Matrix), 1, pFile);
				
				pBone->strName = strBoneName;

				m_vecBone.push_back(pBone);
			}
		}


		bool bHasAnimationKeyFrame = false;
		fread(&bHasAnimationKeyFrame, sizeof(bool), 1, pFile);

		if (bHasAnimationKeyFrame)
		{
			size_t iBoneCount = 0;
			fread(&iBoneCount, sizeof(size_t), 1, pFile);

			for (size_t i = 0; i < iBoneCount; ++i)
			{
				PBONEKEYFRAME pBoneKeyFrame = new BONEKEYFRAME;
				size_t iKeyFrameCount = 0;

				fread(&pBoneKeyFrame->iBoneIndex, sizeof(size_t), 1, pFile);
				fread(&iKeyFrameCount, sizeof(size_t), 1, pFile);

				for (size_t j = 0; j < iKeyFrameCount; ++j)
				{
					PKEYFRAME pKeyFrame = new KEYFRAME;

					fread(&pKeyFrame->dTime, sizeof(double), 1, pFile);
					fread(&pKeyFrame->vPos, sizeof(Vector3), 1, pFile);
					fread(&pKeyFrame->vRot, sizeof(Vector4), 1, pFile);
					fread(&pKeyFrame->vScale, sizeof(Vector3), 1, pFile);

					pBoneKeyFrame->vecKeyFrame.push_back(pKeyFrame);
				}
				m_vecAnimationKeyFrame.push_back(pBoneKeyFrame);
			}
		}
		

		bool bHasClipInfo = false;
		fread(&bHasClipInfo, sizeof(bool), 1, pFile);

		if (bHasClipInfo)
		{
			size_t iClipCount = m_vecAnimationClipInfo.size();
			fread(&iClipCount, sizeof(size_t), 1, pFile);

			for (size_t i = 0; i < iClipCount; ++i)
			{
				PANIMATIONCLIP pClipInfo = new ANIMATIONCLIP;

				size_t iClipNameLength = 0;
				char strClipName[256] = {};
				fread(&iClipNameLength, sizeof(size_t), 1, pFile);
				fread(&strClipName[0], sizeof(char), iClipNameLength, pFile);
				fread(&pClipInfo->iStartFrame, sizeof(size_t), 1, pFile);
				fread(&pClipInfo->iEndFrame, sizeof(size_t), 1, pFile);
				fread(&pClipInfo->iLength, sizeof(size_t), 1, pFile);
				pClipInfo->strName = strClipName;

				m_vecAnimationClipInfo.push_back(pClipInfo);
			}
		}

		fclose(pFile);
	}

	return this;
}


bool CMesh::CreateVertexBuffer(UINT iVertexCount, UINT iVertexSize, D3D11_USAGE eVertexUsage,
	void * pVertexData, D3D11_PRIMITIVE_TOPOLOGY ePrimitiveType, PMESHCONTAINER pContainer)
{

     	char*	pVertices = (char*)pVertexData;

	for (int i = 0; i < iVertexCount; ++i)
	{
		PVector3	pPos = (PVector3)(pVertices + (iVertexSize * i));

		if (m_vMin.x > pPos->x)
			m_vMin.x = pPos->x;

		if (m_vMin.y > pPos->y)
			m_vMin.y = pPos->y;

		if (m_vMin.z > pPos->z)
			m_vMin.z = pPos->z;

		if (m_vMax.x < pPos->x)
			m_vMax.x = pPos->x;

		if (m_vMax.y < pPos->y)
			m_vMax.y = pPos->y;

		if (m_vMax.z < pPos->z)
			m_vMax.z = pPos->z;
	}

	m_vLength = m_vMax - m_vMin;
	m_vCenter = (m_vMin + m_vMax) / 2.0f;
	m_fRadius = m_vLength.Max() / 2.0f;



	pContainer->tVB.ePrimitiveType = ePrimitiveType;
	pContainer->tVB.eUsage = eVertexUsage;
	pContainer->tVB.iCount = iVertexCount;
	pContainer->tVB.iSize = iVertexSize;
	pContainer->tVB.pData = new char[iVertexCount * iVertexSize];

	//Copy Vertex Data to Dynamicaly allocated Storage
	memcpy(pContainer->tVB.pData, pVertexData, iVertexCount * iVertexSize);

	D3D11_BUFFER_DESC tDescription = {};

	tDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDescription.ByteWidth = iVertexCount * iVertexSize;
	tDescription.Usage = eVertexUsage;

	//버텍스의 용도가 Dynamic 이면 CPU에서 버텍스 정보 변경 가능함.
	//분리한 이유는, 메모리 자원을 비디오카드 메모리에 복사해주기 떄문에, 메모리가 분리되어 있어서 다른 접근 방식이 필요함
	if (eVertexUsage == D3D11_USAGE_DYNAMIC)
	{
		tDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	/*실제 버텍스의 정보를 메모리에 얹음*/
	D3D11_SUBRESOURCE_DATA tSubResource = {};
	tSubResource.pSysMem = pContainer->tVB.pData;



	if (FAILED(DEVICE->CreateBuffer(&tDescription, &tSubResource, &pContainer->tVB.pVB)))
	{
		return false;
	}
	return true;
}

bool CMesh::CreateIndexBuffer(UINT iIndexCount, UINT iIndexSize, D3D11_USAGE eIndexUsage,
	DXGI_FORMAT eIndexFomat, void * pIndexData, PMESHCONTAINER pContainer)
{

	if (iIndexCount == 0)
		return true;

	INDEXBUFFER tIndexBuffer = {};

	tIndexBuffer.eFormat = eIndexFomat;
	tIndexBuffer.eUsage = eIndexUsage;
	tIndexBuffer.iCount = iIndexCount;
	tIndexBuffer.iSize = iIndexSize;
	tIndexBuffer.pData = new char[iIndexCount * iIndexSize];

	memcpy(tIndexBuffer.pData, pIndexData, iIndexCount * iIndexSize);


	D3D11_BUFFER_DESC tDescription = {};
	tDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tDescription.ByteWidth = iIndexCount * iIndexSize;
	tDescription.Usage = eIndexUsage;

	if (tDescription.Usage == D3D11_USAGE_DYNAMIC)
	{
		tDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	D3D11_SUBRESOURCE_DATA pSubResource = {};
	pSubResource.pSysMem = tIndexBuffer.pData;


	if (FAILED(DEVICE->CreateBuffer(&tDescription, &pSubResource,
		&tIndexBuffer.pIB)))
		return false;

	pContainer->vecIB.push_back(tIndexBuffer);

	return true;
}

bool CMesh::CreateSpherePos(const string & strKey, float fRadius, unsigned int iSubDivision)
{
	m_strShaderKey = STANDARD_TEXTURE_SHADER;
	m_strLayoutKey = POS_UV_LAYOUT;
	SetTag(strKey);

	// Put a cap on the number of subdivisions.
	iSubDivision = min(iSubDivision, 5u);

	// Approximate a sphere by tessellating an icosahedron.
	const float X = 0.525731f;
	const float Z = 0.850651f;

	Vector3 pos[12] =
	{
		Vector3(-X, 0.0f, Z),  Vector3(X, 0.0f, Z),
		Vector3(-X, 0.0f, -Z), Vector3(X, 0.0f, -Z),
		Vector3(0.0f, Z, X),   Vector3(0.0f, Z, -X),
		Vector3(0.0f, -Z, X),  Vector3(0.0f, -Z, -X),
		Vector3(Z, X, 0.0f),   Vector3(-Z, X, 0.0f),
		Vector3(Z, -X, 0.0f),  Vector3(-Z, -X, 0.0f)
	};

	DWORD k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	vector<VERTEXTEX>	vecVertices;
	vector<DWORD>		vecIndices;

	vecVertices.resize(12);
	vecIndices.resize(60);

	for (UINT i = 0; i < 12; ++i)
		vecVertices[i].vPos = pos[i];

	for (UINT i = 0; i < 60; ++i)
		vecIndices[i] = k[i];

	for (UINT i = 0; i < iSubDivision; ++i)
		Subdivide(vecVertices, vecIndices);

	// Project vertices onto sphere and scale.
	vector<Vector3>	vecPos;
	vecPos.resize(vecVertices.size());
	for (UINT i = 0; i < vecVertices.size(); ++i)
	{
		// Project onto unit sphere.
		Vector3	vN = vecVertices[i].vPos.Normalize();

		// Project onto sphere.
		Vector3 p = fRadius * vN;

		vecVertices[i].vPos = p;
		// Normal이 있을 경우 따로 저장한다.

		// Derive texture coordinates from spherical coordinates.
		float theta = AngleFromXY(
			vecVertices[i].vPos.x,
			vecVertices[i].vPos.z);

		float phi = acosf(vecVertices[i].vPos.y / fRadius);

		vecVertices[i].vUV.x = theta / XM_2PI;
		vecVertices[i].vUV.y = phi / XM_PI;

		vecPos[i] = vecVertices[i].vPos;
	}

	// 버텍스버퍼와 인덱스버퍼를 만든다.
	PMESHCONTAINER	pContainer = new MESHCONTAINER;

	m_vecMeshContainer.push_back(pContainer);

	if (!CreateVertexBuffer(vecPos.size(), sizeof(Vector3),
		D3D11_USAGE_DEFAULT, &vecPos[0], D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		pContainer))
		return false;

	if (!CreateIndexBuffer(vecIndices.size(), sizeof(DWORD), D3D11_USAGE_DEFAULT,
		DXGI_FORMAT_R32_UINT, &vecIndices[0], pContainer))
		return false;

	return true;
}

void CMesh::Subdivide(vector<VERTEXTEX>& vecVertices, vector<DWORD>& vecIndices)
{
	// Save a copy of the input geometry.
	vector<VERTEXTEX>	vecCopyVertex = vecVertices;
	vector<DWORD>	vecCopyIndex = vecIndices;


	vecVertices.resize(0);
	vecIndices.resize(0);

	//       v1
	//       *
	//      / \
			//     /   \
	//  m0*-----*m1
//   / \   / \
	//  /   \ /   \
	// *-----*-----*
// v0    m2     v2

	UINT numTris = vecCopyIndex.size() / 3;
	for (UINT i = 0; i < numTris; ++i)
	{
		VERTEXTEX v0 = vecCopyVertex[vecCopyIndex[i * 3 + 0]];
		VERTEXTEX v1 = vecCopyVertex[vecCopyIndex[i * 3 + 1]];
		VERTEXTEX v2 = vecCopyVertex[vecCopyIndex[i * 3 + 2]];

		//
		// Generate the midpoints.
		//

		VERTEXTEX m0, m1, m2;

		// For subdivision, we just care about the position component.  We derive the other
		// vertex components in CreateGeosphere.

		m0.vPos = Vector3(
			0.5f*(v0.vPos.x + v1.vPos.x),
			0.5f*(v0.vPos.y + v1.vPos.y),
			0.5f*(v0.vPos.z + v1.vPos.z));

		m1.vPos = Vector3(
			0.5f*(v1.vPos.x + v2.vPos.x),
			0.5f*(v1.vPos.y + v2.vPos.y),
			0.5f*(v1.vPos.z + v2.vPos.z));

		m2.vPos = Vector3(
			0.5f*(v0.vPos.x + v2.vPos.x),
			0.5f*(v0.vPos.y + v2.vPos.y),
			0.5f*(v0.vPos.z + v2.vPos.z));

		//
		// Add new geometry.
		//

		vecVertices.push_back(v0); // 0
		vecVertices.push_back(v1); // 1
		vecVertices.push_back(v2); // 2
		vecVertices.push_back(m0); // 3
		vecVertices.push_back(m1); // 4
		vecVertices.push_back(m2); // 5

		vecIndices.push_back(i * 6 + 0);
		vecIndices.push_back(i * 6 + 3);
		vecIndices.push_back(i * 6 + 5);

		vecIndices.push_back(i * 6 + 3);
		vecIndices.push_back(i * 6 + 4);
		vecIndices.push_back(i * 6 + 5);

		vecIndices.push_back(i * 6 + 5);
		vecIndices.push_back(i * 6 + 4);
		vecIndices.push_back(i * 6 + 2);

		vecIndices.push_back(i * 6 + 3);
		vecIndices.push_back(i * 6 + 1);
		vecIndices.push_back(i * 6 + 4);
	}
}

float CMesh::AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	// Quadrant I or IV
	if (x >= 0.0f)
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f * PI; // in [0, 2*pi).
	}

	// Quadrant II or III
	else
		theta = atanf(y / x) + PI; // in [0, 2*pi).

	return theta;
}


