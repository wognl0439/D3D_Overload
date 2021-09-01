#include "FbxLoader.h"

SSS_USING

CFbxLoader::CFbxLoader()	:
	m_pManager(NULL),
	m_pScene(NULL)
{
}

CFbxLoader::~CFbxLoader()
{
	for (size_t i = 0; i < m_vecMaterials.size(); ++i)
	{
		for (size_t j = 0; j < m_vecMaterials[i].size(); ++j)
		{
			SAFE_DELETE(m_vecMaterials[i][j]);
		}

		m_vecMaterials[i].clear();
	}

	m_vecMaterials.clear();

	Safe_Delete_VecList(m_vecMeshContainer);

	for (int i = 0; i < m_NameArr.Size(); ++i)
	{
		m_NameArr[i]->Clear();
		delete m_NameArr[i];
	}

	m_NameArr.Clear();

	Safe_Delete_VecList(m_vecClip);
	Safe_Delete_VecList(m_vecBones);

	m_pScene->Clear();
	m_pScene->Destroy();
	m_pManager->Destroy();
}

const vector<PFBXMESHCONTAINER>* CFbxLoader::GetMeshContainers() const
{
	return &m_vecMeshContainer;
}

const vector<vector<PFBXMATERIAL>>* CFbxLoader::GetMaterials() const
{
	return &m_vecMaterials;
}

const vector<PFBXBONE>* CFbxLoader::GetBones() const
{
	return &m_vecBones;
}

const vector<PFBXANIMATIONCLIP>* CFbxLoader::GetClips() const
{
	return &m_vecClip;
}

bool CFbxLoader::LoadFbx(const char * pFullPath, int iLoadType)
{
	// FbxManager ��ü�� �����Ѵ�.
	m_pManager = FbxManager::Create();

	// IOSetting�� �����Ѵ�.
	FbxIOSettings*	pIos = FbxIOSettings::Create(m_pManager, IOSROOT);

	// FbxManager�� �����Ѵ�.
	m_pManager->SetIOSettings(pIos);

	// FbxScene�� �����Ѵ�.
	m_pScene = FbxScene::Create(m_pManager, "");

	// FbxImporter �� �̿��ؼ� fbx ���Ϸκ��� �޽� ������ �о�´�.
	FbxImporter*	pImporter = FbxImporter::Create(m_pManager, "");

	// �ش� ��ο� �ִ� fbx ������ �о���� ������ ������.
	pImporter->Initialize(pFullPath, -1, m_pManager->GetIOSettings());

	// ������ ���� ������ FbxScene�� ��带 �����Ѵ�.
	pImporter->Import(m_pScene);

	if (m_pScene->GetGlobalSettings().GetAxisSystem() != FbxAxisSystem::Max)
		m_pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::Max);
	
	m_pScene->FillAnimStackNameArray(m_NameArr);

	if (iLoadType & FLT_ANIMATION)
	{
		LoadAnimationClip();

		// �� ������ �о�´�.
		LoadBone(m_pScene->GetRootNode());

		// Ŭ���� ������ �ִ� �������� �� ������ŭ resize ���ش�.
		// ���� Animation Clip���� �ϴ����̴�.
		for (size_t i = 0; i < m_vecClip.size(); ++i)
		{
			m_vecClip[i]->vecBoneKeyFrame.resize(m_vecBones.size());
		}
	}
	
	if(iLoadType & FLT_MESH)
		Triangulate(m_pScene->GetRootNode());

	LoadMesh(m_pScene->GetRootNode(), iLoadType);

	pImporter->Destroy();

	return true;
}

void CFbxLoader::Triangulate(FbxNode * pNode)
{
	// FbxNodeAttribute : FbxNode �Ӽ��� ��Ÿ���� �������̽��̴�.
	// ���κ��� ���� �� �ִ�.
	FbxNodeAttribute*	pAttr = pNode->GetNodeAttribute();

	if (pAttr && (pAttr->GetAttributeType() == FbxNodeAttribute::eMesh ||
		pAttr->GetAttributeType() == FbxNodeAttribute::eNurbs ||
		pAttr->GetAttributeType() == FbxNodeAttribute::eNurbsSurface))
	{
		FbxGeometryConverter	converter(m_pManager);

		converter.Triangulate(pAttr, true);
	}

	// ���������� �д´�.
	int	iMtrlCount = pNode->GetMaterialCount();

	if (iMtrlCount > 0)
	{
		vector<PFBXMATERIAL>	vecMtrl;

		m_vecMaterials.push_back(vecMtrl);

		for (int i = 0; i < iMtrlCount; ++i)
		{
			// FbxSurfaceMaterial : Fbx �� ������ �ִ� ���� ������
			// ����ִ�.
			FbxSurfaceMaterial*	pMtrl = pNode->GetMaterial(i);

			if (!pMtrl)
				continue;

			LoadMaterial(pMtrl);
		}
	}

	// ���� ����� �ڽĳ�� ���� ���´�.
	int	iChildCount = pNode->GetChildCount();

	for (int i = 0; i < iChildCount; ++i)
	{
		Triangulate(pNode->GetChild(i));
	}
}

void CFbxLoader::LoadMaterial(FbxSurfaceMaterial * pMtrl)
{
	// ���� ������ ������ ����ü�� �����Ѵ�.
	PFBXMATERIAL	pMtrlInfo = new FBXMATERIAL;

	// ���� ������ �����̳ʿ� ���� ������ �߰��Ѵ�.
	m_vecMaterials[m_vecMaterials.size() - 1].push_back(pMtrlInfo);

	// Diffuse ������ �о�´�.
	pMtrlInfo->vDif = GetMaterialColor(pMtrl, FbxSurfaceMaterial::sDiffuse,
		FbxSurfaceMaterial::sDiffuseFactor);

	pMtrlInfo->vAmb = GetMaterialColor(pMtrl, FbxSurfaceMaterial::sAmbient,
		FbxSurfaceMaterial::sAmbientFactor);

	pMtrlInfo->vSpc = GetMaterialColor(pMtrl, FbxSurfaceMaterial::sSpecular,
		FbxSurfaceMaterial::sSpecularFactor);

	pMtrlInfo->vEmv = GetMaterialColor(pMtrl, FbxSurfaceMaterial::sEmissive,
		FbxSurfaceMaterial::sEmissiveFactor);

	pMtrlInfo->fSpecularPower = GetMaterialFactor(pMtrl, FbxSurfaceMaterial::sSpecularFactor);
	pMtrlInfo->fShininess = GetMaterialFactor(pMtrl, FbxSurfaceMaterial::sShininess);
	pMtrlInfo->fTransparencyFactor = GetMaterialFactor(pMtrl, FbxSurfaceMaterial::sTransparencyFactor);

	//bool bResult = true;
	pMtrlInfo->strDifTex = GetMaterialTexture(pMtrl, FbxSurfaceMaterial::sDiffuse);

	/*FILE* pFile = NULL;
	fopen_s(&pFile, pMtrlInfo->strDifTex.c_str(), "rb");
	if (!pFile)
	{
		bResult &= false;
		fclose(pFile);
	}*/

	pMtrlInfo->strBumpTex = GetMaterialTexture(pMtrl, FbxSurfaceMaterial::sNormalMap);

	if (pMtrlInfo->strBumpTex.empty())
		pMtrlInfo->strBumpTex = GetMaterialTexture(pMtrl, FbxSurfaceMaterial::sBump);

	pMtrlInfo->strSpcTex = GetMaterialTexture(pMtrl, FbxSurfaceMaterial::sSpecular);


	pMtrlInfo->strAlphaTex = GetMaterialTexture(pMtrl, FbxSurfaceMaterial::sDisplacementColor);

	if (pMtrlInfo->strAlphaTex.empty())
		pMtrlInfo->strAlphaTex = GetMaterialTexture(pMtrl, FbxSurfaceMaterial::sVectorDisplacementColor);	
}

Vector4 CFbxLoader::GetMaterialColor(FbxSurfaceMaterial * pMtrl, 
	const char * pPropertyName, const char * pPropertyFactorName)
{
	FbxDouble3	vResult(0, 0, 0);
	double		dFactor = 0;

	// ���ڷ� ���� �������� �ش� �̸��� ���� ������Ƽ�� �ִ����� �Ǵ��Ѵ���
	// ���´�. ���������� �ش� �̸��� ������Ƽ ���Ͱ� �ִ����� �Ǵ��Ѵ���
	// ���´�.
	FbxProperty	tProperty = pMtrl->FindProperty(pPropertyName);
	FbxProperty	tPropertyFactor = pMtrl->FindProperty(pPropertyFactorName);

	// ��ȿ���� üũ�Ѵ�.
	if (tProperty.IsValid() && tPropertyFactor.IsValid())
	{
		vResult = tProperty.Get<FbxDouble3>();
		dFactor = tPropertyFactor.Get<FbxDouble>();

		// Factor�� 1�� �ƴ� ��� ������ ���������� ����Ͽ�
		// ���� vResult �� ��� �����ش�.
		if (dFactor != 1)
		{
			vResult[0] *= dFactor;
			vResult[1] *= dFactor;
			vResult[2] *= dFactor;
		}
	}

	return Vector4((float)vResult[0], (float)vResult[1], (float)vResult[2], (float)dFactor);
}

double CFbxLoader::GetMaterialFactor(FbxSurfaceMaterial * pMtrl,
	const char * pPropertyName)
{
	FbxProperty	tProperty = pMtrl->FindProperty(pPropertyName);
	double	dFactor = 0.0;

	if (tProperty.IsValid())
		dFactor = tProperty.Get<FbxDouble>();

	return dFactor;
}

string CFbxLoader::GetMaterialTexture(FbxSurfaceMaterial * pMtrl, 
	const char * pPropertyName)
{
	FbxProperty	tProperty = pMtrl->FindProperty(pPropertyName);

	string	str = "";

	if (tProperty.IsValid())
	{
		// FbxFileTexture �� Ÿ���� ��� �ִ����� ������ �Լ��̴�.
		int	iTexCount = tProperty.GetSrcObjectCount<FbxFileTexture>();

		if (iTexCount > 0)
		{
			FbxFileTexture*	pFileTex = tProperty.GetSrcObject<FbxFileTexture>(0);

			if (pFileTex)
				str = pFileTex->GetFileName();
		}
	}

	return str;
}

void CFbxLoader::LoadMesh(FbxNode * pNode, int iLoadType)
{
	FbxNodeAttribute*	pAttr = pNode->GetNodeAttribute();

	if (pAttr && pAttr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxMesh*	pMesh = pNode->GetMesh();

		if (pMesh)
		{			
			LoadMesh(pMesh, iLoadType);
		}
	}

	int	iChildCount = pNode->GetChildCount();

	for (int i = 0; i < iChildCount; ++i)
	{
		LoadMesh(pNode->GetChild(i), iLoadType);
	}
}

void CFbxLoader::LoadMesh(FbxMesh * pMesh, int iLoadType)
{
	// MeshContainer�� ����� �����̳ʺ� ������ �����̳�&����� ��
	// �ε��� ������ ���ͼ� �޽��� �����ؾ� �Ѵ�.
	PFBXMESHCONTAINER	pContainer = new FBXMESHCONTAINER;

	pContainer->bBump = false;


	FbxMatrix matTransform = GetTransform(pMesh->GetNode());
	Matrix matConvertedTransform 
		(
			matTransform.Get(0, 0), matTransform.Get(0, 1), matTransform.Get(0, 2), matTransform.Get(0, 3),
			matTransform.Get(1, 0), matTransform.Get(1, 1), matTransform.Get(1, 2), matTransform.Get(1, 3),
			matTransform.Get(2, 0), matTransform.Get(2, 1), matTransform.Get(2, 2), matTransform.Get(2, 3),
			matTransform.Get(3, 0), matTransform.Get(3, 1), matTransform.Get(3, 2), matTransform.Get(3, 3)
		);


	Matrix	matAxisConverter (
		1.0, 0.0, 0.0, 0.0 
		, 0.0, 0.0, 1.0, 0.0 
		, 0.0, 1.0, 0.0, 0.0 
		, 0.0, 0.0, 0.0, 1.0 );

	pContainer->matTransform = matAxisConverter * matConvertedTransform * matAxisConverter;

	m_vecMeshContainer.push_back(pContainer);
	if (iLoadType & FLT_MESH)
	{
		// ControlPoint �� ��ġ������ ��� �ִ� �迭�̴�.
		// �� �迭�� ������ �� ������ ������ �ȴ�.
		int	iVtxCount = pMesh->GetControlPointsCount();

		// ���������� FbxVector4Ÿ���� �迭�� �����ϰ� �ֱ� ������ �迭�� 
		// �����ּҸ� ���´�.
		FbxVector4*	pVtxPos = pMesh->GetControlPoints();		
		


		// �����̳ʰ� ������ �ִ� ���� �������� ��������ŭ resize ���ش�.
		pContainer->vecPos.resize(iVtxCount);
		pContainer->vecNormal.resize(iVtxCount);
		pContainer->vecUV.resize(iVtxCount);
		pContainer->vecTangent.resize(iVtxCount);
		pContainer->vecBinormal.resize(iVtxCount);

		for (int i = 0; i < iVtxCount; ++i)
		{
			// y�� z���� �ٲ�� �ֱ� ������ �������ش�.
			pContainer->vecPos[i].x = pVtxPos[i].mData[0] ;
			pContainer->vecPos[i].y = pVtxPos[i].mData[2] ;
			pContainer->vecPos[i].z = pVtxPos[i].mData[1] ;
		}

		// ������ ���� ���´�.
		int	iPolygonCount = pMesh->GetPolygonCount();

		UINT	iVtxID = 0;

		// ���� ���� ���´�.
		int	iMtrlCount = pMesh->GetNode()->GetMaterialCount();

		if (iMtrlCount > 0)
		{
			// ���� ���� �� ����� ���̱� ������ ���� ����ŭ resize �Ѵ�.
			pContainer->vecIndices.resize(iMtrlCount);
		}
		else
		{
			pContainer->vecIndices.resize(1);
		}

		// ���� ������ ���´�.
		FbxGeometryElementMaterial*	pMaterial = pMesh->GetElementMaterial();
		int iCount = pMesh->GetElementMaterialCount();
		// �ﰢ�� ����ŭ �ݺ��Ѵ�.
		for (int i = 0; i < iPolygonCount; ++i)
		{
			// �� �������� �����ϴ� ������ ���� ���´�.
			// �ﰢ���� ��� 3���� ���´�.
			int	iPolygonSize = pMesh->GetPolygonSize(i);

			int	iIdx[3] = {};

			for (int j = 0; j < iPolygonSize; ++j)
			{
				// ���� �ﰢ���� �����ϰ� �ִ� ���ؽ����� �������� �ε�����
				// ���Ѵ�.
				int	iControlIndex = pMesh->GetPolygonVertex(i, j);

				iIdx[j] = iControlIndex;

				LoadNormal(pMesh, pContainer, iVtxID, iControlIndex);

				LoadUV(pMesh, pContainer, pMesh->GetTextureUVIndex(i, j),	iControlIndex);

				LoadTangent(pMesh, pContainer, iVtxID, iControlIndex);

				LoadBinormal(pMesh, pContainer, iVtxID, iControlIndex);

				++iVtxID;
			}

			if (pMaterial)
			{
				if (pMaterial->GetIndexArray().GetCount() > 0)
				{
					int	iMtrlID = pMaterial->GetIndexArray().GetAt(i);

					pContainer->vecIndices[iMtrlID].push_back(iIdx[0]);
					pContainer->vecIndices[iMtrlID].push_back(iIdx[2]);
					pContainer->vecIndices[iMtrlID].push_back(iIdx[1]);
				}
			}
			else
			{
				pContainer->vecIndices[0].push_back(iIdx[0]);
				pContainer->vecIndices[0].push_back(iIdx[2]);
				pContainer->vecIndices[0].push_back(iIdx[1]);
			}
		}
	}

	LoadAnimation(pMesh, pContainer, iLoadType);
}

void CFbxLoader::LoadNormal(FbxMesh * pMesh, PFBXMESHCONTAINER pContainer, 
	int iVtxID, int iControlIndex)
{
	// �޽��κ��� ElementNormal ������ ���´�.
	FbxGeometryElementNormal*	pNormal = pMesh->GetElementNormal();

	int	iNormalIndex = iVtxID;

	// MappingMode�� ReferenceMode�� ���� �ε����� ����� ������ �޶�����.
	if (pNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (pNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
			iNormalIndex = pNormal->GetIndexArray().GetAt(iVtxID);
			break;
		}
	}

	else if (pNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		switch (pNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			iNormalIndex = iControlIndex;
			break;
		case FbxGeometryElement::eIndexToDirect:
			iNormalIndex = pNormal->GetIndexArray().GetAt(iControlIndex);
			break;
		}
	}

	FbxVector4	vNormal = pNormal->GetDirectArray().GetAt(iNormalIndex);

	pContainer->vecNormal[iControlIndex].x = vNormal.mData[0];
	pContainer->vecNormal[iControlIndex].y = vNormal.mData[2];
	pContainer->vecNormal[iControlIndex].z = vNormal.mData[1];
}

void CFbxLoader::LoadUV(FbxMesh * pMesh, PFBXMESHCONTAINER pContainer,
	int iUVID, int iControlIndex)
{
	int iCount = pMesh->GetElementUVCount();
	FbxGeometryElementUV*	pUV = pMesh->GetElementUV(0);

	

	if (!pUV)
		return;

	int	 iUVIndex = iUVID;

	if (pUV->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		switch (pUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			iUVIndex = iControlIndex;
			break;
		case FbxGeometryElement::eIndexToDirect:
			iUVIndex = pUV->GetIndexArray().GetAt(iControlIndex);
			break;
		}
	}

	else if (pUV->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (pUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			//iUVIndex = pUV->GetIndexArray().GetAt(iUVIndex);
		}
		break;
		default:
			break; // other reference modes not shown here!
		}
	}

	FbxVector2	vUV = pUV->GetDirectArray().GetAt(iUVIndex);

	if (vUV.mData[0] < 0.0f)
	{
		while (vUV.mData[0] < 0.0f)
		{
			vUV.mData[0] += 1.0f;
		}
	}
	if (vUV.mData[1] < 0.0f)
	{
		while (vUV.mData[1] < 0.0f)
		{
			vUV.mData[1] += 1.0f;
		}
	}
	if (vUV.mData[0] > 1.0f)
	{
		while (vUV.mData[0] > 1.0f)
		{
			vUV.mData[0] -= 1.0f;
		}
	}
	if (vUV.mData[1] > 1.0f)
	{
		while (vUV.mData[1] > 1.0f)
		{
			vUV.mData[1] -= 1.0f;
		}
	}



	pContainer->vecUV[iControlIndex].x = vUV.mData[0];// -(int)vUV.mData[0];
	pContainer->vecUV[iControlIndex].y = 1.0f - vUV.mData[1];//(vUV.mData[1] - (int)vUV.mData[1]);
}

void CFbxLoader::LoadTangent(FbxMesh * pMesh, PFBXMESHCONTAINER pContainer, int iVtxID, int iControlIndex)
{
	FbxGeometryElementTangent*	pTangent = pMesh->GetElementTangent();

	if (!pTangent)
		return;

	pContainer->bBump = true;

	int	iTangentIndex = iVtxID;

	if (pTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (pTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
			iTangentIndex = pTangent->GetIndexArray().GetAt(iVtxID);
			break;
		}
	}

	else if (pTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		switch (pTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			iTangentIndex = iControlIndex;
			break;
		case FbxGeometryElement::eIndexToDirect:
			iTangentIndex = pTangent->GetIndexArray().GetAt(iControlIndex);
			break;
		}
	}

	FbxVector4	vTangent = pTangent->GetDirectArray().GetAt(iTangentIndex);

	pContainer->vecTangent[iControlIndex].x = vTangent.mData[0];
	pContainer->vecTangent[iControlIndex].y = vTangent.mData[2];
	pContainer->vecTangent[iControlIndex].z = vTangent.mData[1];
}

void CFbxLoader::LoadBinormal(FbxMesh * pMesh, PFBXMESHCONTAINER pContainer, int iVtxID, int iControlIndex)
{
	FbxGeometryElementBinormal*	pBinormal = pMesh->GetElementBinormal();

	if (!pBinormal)
		return;

	pContainer->bBump = true;

	int	iBinormalIndex = iVtxID;

	if (pBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (pBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
			iBinormalIndex = pBinormal->GetIndexArray().GetAt(iVtxID);
			break;
		}
	}

	else if (pBinormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		switch (pBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			iBinormalIndex = iControlIndex;
			break;
		case FbxGeometryElement::eIndexToDirect:
			iBinormalIndex = pBinormal->GetIndexArray().GetAt(iControlIndex);
			break;
		}
	}

	FbxVector4	vBinormal = pBinormal->GetDirectArray().GetAt(iBinormalIndex);

	pContainer->vecBinormal[iControlIndex].x = vBinormal.mData[0];
	pContainer->vecBinormal[iControlIndex].y = vBinormal.mData[2];
	pContainer->vecBinormal[iControlIndex].z = vBinormal.mData[1];
}

void CFbxLoader::LoadAnimationClip()
{
	int	iCount = m_NameArr.GetCount();

	if (iCount <= 0)
		return;

	FbxTime::EMode	eTimeMode = m_pScene->GetGlobalSettings().GetTimeMode();

	for (int i = 0; i < iCount; ++i)
	{
		// m_NameArr�� ����� �̸����� Scene���κ��� FbxAnimStack ��ü�� ���´�.
		FbxAnimStack*	pAnimStack = m_pScene->FindMember<FbxAnimStack>(m_NameArr[i]->Buffer());

		if (!pAnimStack)
			continue;

		PFBXANIMATIONCLIP	pClip = new FBXANIMATIONCLIP;

		pClip->strName = pAnimStack->GetName();
		FbxTakeInfo*	pTake = m_pScene->GetTakeInfo(pClip->strName.c_str());

		pClip->tStart = pTake->mLocalTimeSpan.GetStart();
		pClip->tEnd = pTake->mLocalTimeSpan.GetStop();
		// GetFrameCount �Լ��� ȣ���ϰ�  time��带 �־��ָ� �ð��� ����������
		// ��ȯ���ش�. �������� ¥�� �ִϸ��̼� ������ �����ش�.
		pClip->lTimeLength = pClip->tEnd.GetFrameCount(eTimeMode) - pClip->tStart.GetFrameCount(eTimeMode);
		pClip->eTimeMode = eTimeMode;

		m_vecClip.push_back(pClip);
	}
}

void CFbxLoader::LoadBone(FbxNode * pNode)
{
	int	iChildCount = pNode->GetChildCount();

	for (int i = 0; i < iChildCount; ++i)
	{
		LoadBoneRecursive(pNode->GetChild(i), 0, 0, -1);
	}
}

void CFbxLoader::LoadBoneRecursive(FbxNode * pNode, int iDepth, int iIndex, int iParent)
{
	FbxNodeAttribute*	pAttr = pNode->GetNodeAttribute();
	

	if (pAttr && pAttr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{		
		
		PFBXBONE	pBone = new FBXBONE;
		
		pBone->strName = pNode->GetName();
		pBone->iDepth = iDepth;
		pBone->iParentIndex = iParent;

		m_vecBones.push_back(pBone);
	}

	int iChildCount = pNode->GetChildCount();

	for (int i = 0; i < iChildCount; ++i)
	{
		LoadBoneRecursive(pNode->GetChild(i), iDepth + 1, m_vecBones.size(), iIndex);
	}
}

void CFbxLoader::LoadAnimation(FbxMesh * pMesh, PFBXMESHCONTAINER pContainer, int iLoadType)
{
	int	iSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);

	fbxsdk::FbxDeformer::EDeformerType eType;

	int iDeformers = pMesh->GetDeformerCount(FbxDeformer::eUnknown);
	//iDeformers = pMesh->GetDeformerCount(FbxDeformer::eBlendShape);
	//iDeformers = pMesh->GetDeformerCount(FbxDeformer::eVertexCache);


	if (iSkinCount <= 0)
		return;

	// �޽��� ���� ���� ���´�.
	int	iCPCount = pMesh->GetControlPointsCount();

	// ������ ����ġ ������ ���ε��� ������ ���� ����ŭ
	// ��������� �Ѵ�.
	pContainer->vecBlendWeight.resize(iCPCount);
	pContainer->vecBlendIndex.resize(iCPCount);

	pContainer->bAnimation = true;
	FbxAMatrix matTransform = GetTransform(pMesh->GetNode());


	for (int i = 0; i < iSkinCount; ++i)
	{
		FbxSkin*	pSkin = (FbxSkin*)pMesh->GetDeformer(i, FbxDeformer::eSkin);
		
		if (!pSkin)
			continue;

		FbxSkin::EType	eSkinningType = pSkin->GetSkinningType();

		if (eSkinningType == FbxSkin::eRigid || eSkinningType == FbxSkin::eLinear)
		{  
			// Cluster : ������ �ǹ��Ѵ�.
			int	iClusterCount = pSkin->GetClusterCount();

			for (int j = 0; j < iClusterCount; ++j)
			{
				FbxCluster*	pCluster = pSkin->GetCluster(j);

				if (!pCluster->GetLink())
					continue;

				string	strBoneName = pCluster->GetLink()->GetName();
				
				int	iBoneIndex = FindBoneFromName(strBoneName);

				if (iLoadType & FLT_MESH)
					LoadWeightAndIndex(pCluster, iBoneIndex, pContainer);

				if (iLoadType & FLT_ANIMATION)
				{
					LoadOffsetMatrix(pCluster, matTransform, iBoneIndex, pContainer);
					//^^^����^^^^
					//m_vecBones[iBoneIndex]->matBone = matTransform;

					LoadTimeTransform(pMesh->GetNode(), pCluster, matTransform, iBoneIndex);
				}
			}
		}
	}

	if (iLoadType & FLT_MESH)
		ChangeWeightAndIndices(pContainer);
}

FbxAMatrix CFbxLoader::GetTransform(FbxNode * pNode)
{
	const FbxVector4	vT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4	vR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4	vS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(vT, vR, vS);
}

int CFbxLoader::FindBoneFromName(const string & strName)
{
	for (size_t i = 0; i < m_vecBones.size(); ++i)
	{
		if (m_vecBones[i]->strName == strName)
			return i;
	}

	return -1;
}

void CFbxLoader::LoadWeightAndIndex(FbxCluster * pCluster, int iBoneIndex, PFBXMESHCONTAINER pContainer)
{
	int	iControlIndicesCount = pCluster->GetControlPointIndicesCount();

	for (int i = 0; i < iControlIndicesCount; ++i)
	{
		FBXWEIGHT	tWeight = {};

		tWeight.iIndex = iBoneIndex;
		tWeight.dWeight = pCluster->GetControlPointWeights()[i];

		int	iClusterIndex = pCluster->GetControlPointIndices()[i];

		// map�� Ư¡ : Ű�� �̿��� �ε���ó�� ������ ��� �ش� Ű�� ���ٸ�
		// ������ش�.
		pContainer->mapWeights[iClusterIndex].push_back(tWeight);
	}
}

void CFbxLoader::LoadOffsetMatrix(FbxCluster * pCluster, const FbxAMatrix & matTransform, int iBoneIndex, PFBXMESHCONTAINER pContainer)
{
	FbxAMatrix matCluster;
	FbxAMatrix matClusterLink;
	
	pCluster->GetTransformMatrix(matCluster);
	pCluster->GetTransformLinkMatrix(matClusterLink);
	
	FbxVector4	v1 = { 1.0, 0.0, 0.0, 0.0 };
	FbxVector4	v2 = { 0.0, 0.0, 1.0, 0.0 }; // { 0.0, 1.0, 0.0, 0.0 };
	FbxVector4	v3 = { 0.0, 1.0, 0.0, 0.0 }; // { 0.0, 0.0, 1.0, 0.0 }; 
	FbxVector4	v4 = { 0.0, 0.0, 0.0, 1.0 };

	FbxAMatrix	matReflect;
	matReflect.mData[0] = v1;
	matReflect.mData[1] = v2;
	matReflect.mData[2] = v3;
	matReflect.mData[3] = v4;

	/*
	1 0 0 0   1 2 3 4   1 0 0 0
	0 0 1 0   5 6 7 8   0 0 1 0
	0 1 0 0 * 9 0 1 2 * 0 1 0 0
	0 0 0 1   3 4 5 6   0 0 0 1

	1 2 3 4   1 0 0 0
	9 0 1 2   0 0 1 0
	5 6 7 8 * 0 1 0 0
	3 4 5 6   0 0 0 1

	1 3 2 4
	9 1 0 2
	5 7 6 8
	3 5 4 6
	*/
	FbxAMatrix matOffset;
	FbxVector4 vPosition = matClusterLink.GetT();
	FbxVector4 vRotation = matClusterLink.GetR();
	FbxVector4 vScale = matClusterLink.GetS();

	vRotation[0] *= D2R;
	vRotation[1] *= D2R;
	vRotation[2] *= D2R;

	matOffset = matClusterLink.Inverse() * matCluster * matTransform;
	matOffset = matReflect * matOffset * matReflect;
	
	m_vecBones[iBoneIndex]->matOffset = matOffset;
	m_vecBones[iBoneIndex]->matBone = FbxAMatrix(vPosition, vRotation, vScale) * matTransform;
}

void CFbxLoader::LoadTimeTransform(FbxNode * pNode, FbxCluster * pCluster, const FbxAMatrix & matTransform, int iBoneIndex)
{
	FbxVector4	v1 = { 1.0, 0.0, 0.0, 0.0 };
	FbxVector4	v2 = { 0.0, 0.0, 1.0, 0.0 };
	FbxVector4	v3 = { 0.0, 1.0, 0.0, 0.0 };
	FbxVector4	v4 = { 0.0, 0.0, 0.0, 1.0 };

	FbxAMatrix	matReflect;
	matReflect.mData[0] = v1;
	matReflect.mData[1] = v2;
	matReflect.mData[2] = v3;
	matReflect.mData[3] = v4;

	for (size_t i = 0; i < m_vecClip.size(); ++i)
	{
		FbxLongLong	Start = m_vecClip[i]->tStart.GetFrameCount(m_vecClip[i]->eTimeMode);
		FbxLongLong	End = m_vecClip[i]->tEnd.GetFrameCount(m_vecClip[i]->eTimeMode);

		m_vecClip[i]->vecBoneKeyFrame[iBoneIndex].iBoneIndex = iBoneIndex;

		// ��ü ������ ����ŭ �ݺ��Ѵ�.
		for (FbxLongLong j = Start; j <= End; ++j)
		{
			FbxTime	tTime = {};
			FBXKEYFRAME	tKeyFrame = {};

			// ���� �����ӿ� �ش��ϴ� FbxTime�� ������.
			tTime.SetFrame(j, m_vecClip[i]->eTimeMode);

			// EvaluateGlobalTransform

			FbxAMatrix	matOffset = pNode->EvaluateGlobalTransform(tTime) * matTransform;			
			FbxAMatrix	matCur = matOffset.Inverse() * pCluster->GetLink()->EvaluateGlobalTransform(tTime);
			matCur = matReflect * matCur * matReflect;

			tKeyFrame.dTime = tTime.GetSecondDouble();
			tKeyFrame.matTransform = matCur;
			m_vecClip[i]->vecBoneKeyFrame[iBoneIndex].vecKeyFrame.push_back(tKeyFrame);
		}
	}
}

void CFbxLoader::ChangeWeightAndIndices(PFBXMESHCONTAINER pContainer)
{
	unordered_map<int, vector<FBXWEIGHT>>::iterator	iter;
	unordered_map<int, vector<FBXWEIGHT>>::iterator	iterEnd = pContainer->mapWeights.end();

	for (iter = pContainer->mapWeights.begin(); iter != iterEnd; ++iter)
	{
		if (iter->second.size() > 4)
		{
			// ����ġ ���� ���� �������� �����Ѵ�.
			sort(iter->second.begin(), iter->second.end(), [](const FBXWEIGHT& lhs, const FBXWEIGHT& rhs)
			{
				return lhs.dWeight > rhs.dWeight;
			});

			double	dSum = 0.0;

			for (int i = 0; i < 4; ++i)
			{
				dSum += iter->second[i].dWeight;
			}

			double	dInterpolate = 1.f - dSum;

			vector<FBXWEIGHT>::iterator	iterErase = iter->second.begin() + 4;

			iter->second.erase(iterErase, iter->second.end());
			iter->second[0].dWeight += dInterpolate;
		}

		float	fWeight[4] = {};
		int		iIndex[4] = {};

		for (int i = 0; i < iter->second.size(); ++i)
		{
			fWeight[i] = iter->second[i].dWeight;
			iIndex[i] = iter->second[i].iIndex;
		}

		Vector4	vWeight = fWeight;
		Vector4	vIndex = iIndex;

		pContainer->vecBlendWeight[iter->first] = vWeight;
		pContainer->vecBlendIndex[iter->first] = vIndex;
	}
}