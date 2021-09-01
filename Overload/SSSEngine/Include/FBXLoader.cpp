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
	// FbxManager 객체를 생성한다.
	m_pManager = FbxManager::Create();

	// IOSetting을 생성한다.
	FbxIOSettings*	pIos = FbxIOSettings::Create(m_pManager, IOSROOT);

	// FbxManager에 지정한다.
	m_pManager->SetIOSettings(pIos);

	// FbxScene을 생성한다.
	m_pScene = FbxScene::Create(m_pManager, "");

	// FbxImporter 를 이용해서 fbx 파일로부터 메쉬 정보를 읽어온다.
	FbxImporter*	pImporter = FbxImporter::Create(m_pManager, "");

	// 해당 경로에 있는 fbx 파일을 읽어오고 정보를 만들어낸다.
	pImporter->Initialize(pFullPath, -1, m_pManager->GetIOSettings());

	// 위에서 만들어낸 정보를 FbxScene에 노드를 구성한다.
	pImporter->Import(m_pScene);

	if (m_pScene->GetGlobalSettings().GetAxisSystem() != FbxAxisSystem::Max)
		m_pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::Max);
	
	m_pScene->FillAnimStackNameArray(m_NameArr);

	if (iLoadType & FLT_ANIMATION)
	{
		LoadAnimationClip();

		// 본 정보를 읽어온다.
		LoadBone(m_pScene->GetRootNode());

		// 클립이 가지고 있는 프레임을 본 개수만큼 resize 해준다.
		// 원래 Animation Clip에서 하던것이다.
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
	// FbxNodeAttribute : FbxNode 속성을 나타내는 인터페이스이다.
	// 노드로부터 얻어올 수 있다.
	FbxNodeAttribute*	pAttr = pNode->GetNodeAttribute();

	if (pAttr && (pAttr->GetAttributeType() == FbxNodeAttribute::eMesh ||
		pAttr->GetAttributeType() == FbxNodeAttribute::eNurbs ||
		pAttr->GetAttributeType() == FbxNodeAttribute::eNurbsSurface))
	{
		FbxGeometryConverter	converter(m_pManager);

		converter.Triangulate(pAttr, true);
	}

	// 재질정보를 읽는다.
	int	iMtrlCount = pNode->GetMaterialCount();

	if (iMtrlCount > 0)
	{
		vector<PFBXMATERIAL>	vecMtrl;

		m_vecMaterials.push_back(vecMtrl);

		for (int i = 0; i < iMtrlCount; ++i)
		{
			// FbxSurfaceMaterial : Fbx 가 가지고 있는 재질 정보를
			// 담고있다.
			FbxSurfaceMaterial*	pMtrl = pNode->GetMaterial(i);

			if (!pMtrl)
				continue;

			LoadMaterial(pMtrl);
		}
	}

	// 현재 노드의 자식노드 수를 얻어온다.
	int	iChildCount = pNode->GetChildCount();

	for (int i = 0; i < iChildCount; ++i)
	{
		Triangulate(pNode->GetChild(i));
	}
}

void CFbxLoader::LoadMaterial(FbxSurfaceMaterial * pMtrl)
{
	// 재질 정보를 저장할 구조체를 생성한다.
	PFBXMATERIAL	pMtrlInfo = new FBXMATERIAL;

	// 가장 마지막 컨테이너에 재질 정보를 추가한다.
	m_vecMaterials[m_vecMaterials.size() - 1].push_back(pMtrlInfo);

	// Diffuse 정보를 읽어온다.
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

	// 인자로 들어온 재질에서 해당 이름을 가진 프로퍼티가 있는지를 판단한다음
	// 얻어온다. 마찬가지로 해당 이름의 프로퍼티 팩터가 있는지를 판단한다음
	// 얻어온다.
	FbxProperty	tProperty = pMtrl->FindProperty(pPropertyName);
	FbxProperty	tPropertyFactor = pMtrl->FindProperty(pPropertyFactorName);

	// 유효한지 체크한다.
	if (tProperty.IsValid() && tPropertyFactor.IsValid())
	{
		vResult = tProperty.Get<FbxDouble3>();
		dFactor = tPropertyFactor.Get<FbxDouble>();

		// Factor가 1이 아닐 경우 일종의 비율값으로 사용하여
		// 얻어온 vResult 에 모두 곱해준다.
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
		// FbxFileTexture 이 타입이 몇개가 있는지를 얻어오는 함수이다.
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
	// MeshContainer를 만들고 컨테이너별 정점과 컨테이너&서브셋 별
	// 인덱스 정보를 얻어와서 메쉬를 구성해야 한다.
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
		// ControlPoint 는 위치정보를 담고 있는 배열이다.
		// 이 배열의 개수는 곧 정점의 개수가 된다.
		int	iVtxCount = pMesh->GetControlPointsCount();

		// 내부적으로 FbxVector4타입의 배열로 저장하고 있기 때문에 배열의 
		// 시작주소를 얻어온다.
		FbxVector4*	pVtxPos = pMesh->GetControlPoints();		
		


		// 컨테이너가 가지고 있는 정점 정보들을 정점수만큼 resize 해준다.
		pContainer->vecPos.resize(iVtxCount);
		pContainer->vecNormal.resize(iVtxCount);
		pContainer->vecUV.resize(iVtxCount);
		pContainer->vecTangent.resize(iVtxCount);
		pContainer->vecBinormal.resize(iVtxCount);

		for (int i = 0; i < iVtxCount; ++i)
		{
			// y와 z축이 바뀌어 있기 때문에 변경해준다.
			pContainer->vecPos[i].x = pVtxPos[i].mData[0] ;
			pContainer->vecPos[i].y = pVtxPos[i].mData[2] ;
			pContainer->vecPos[i].z = pVtxPos[i].mData[1] ;
		}

		// 폴리곤 수를 얻어온다.
		int	iPolygonCount = pMesh->GetPolygonCount();

		UINT	iVtxID = 0;

		// 재질 수를 얻어온다.
		int	iMtrlCount = pMesh->GetNode()->GetMaterialCount();

		if (iMtrlCount > 0)
		{
			// 재질 수는 곧 서브셋 수이기 때문에 재질 수만큼 resize 한다.
			pContainer->vecIndices.resize(iMtrlCount);
		}
		else
		{
			pContainer->vecIndices.resize(1);
		}

		// 재질 정보를 얻어온다.
		FbxGeometryElementMaterial*	pMaterial = pMesh->GetElementMaterial();
		int iCount = pMesh->GetElementMaterialCount();
		// 삼각형 수만큼 반복한다.
		for (int i = 0; i < iPolygonCount; ++i)
		{
			// 이 폴리곤을 구성하는 정점의 수를 얻어온다.
			// 삼각형일 경우 3개를 얻어온다.
			int	iPolygonSize = pMesh->GetPolygonSize(i);

			int	iIdx[3] = {};

			for (int j = 0; j < iPolygonSize; ++j)
			{
				// 현재 삼각형을 구성하고 있는 버텍스정보 내에서의 인덱스를
				// 구한다.
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
	// 메쉬로부터 ElementNormal 정보를 얻어온다.
	FbxGeometryElementNormal*	pNormal = pMesh->GetElementNormal();

	int	iNormalIndex = iVtxID;

	// MappingMode와 ReferenceMode에 따라서 인덱스로 사용할 정보가 달라진다.
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
		// m_NameArr에 저장된 이름으로 Scene으로부터 FbxAnimStack 객체를 얻어온다.
		FbxAnimStack*	pAnimStack = m_pScene->FindMember<FbxAnimStack>(m_NameArr[i]->Buffer());

		if (!pAnimStack)
			continue;

		PFBXANIMATIONCLIP	pClip = new FBXANIMATIONCLIP;

		pClip->strName = pAnimStack->GetName();
		FbxTakeInfo*	pTake = m_pScene->GetTakeInfo(pClip->strName.c_str());

		pClip->tStart = pTake->mLocalTimeSpan.GetStart();
		pClip->tEnd = pTake->mLocalTimeSpan.GetStop();
		// GetFrameCount 함수를 호출하고  time모드를 넣어주면 시간을 프레임으로
		// 변환해준다. 몇프레임 짜리 애니메이션 인지를 구해준다.
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

	// 메쉬의 정점 수를 얻어온다.
	int	iCPCount = pMesh->GetControlPointsCount();

	// 정점의 가중치 정보와 본인덱스 정보는 정점 수만큼
	// 만들어져야 한다.
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
			// Cluster : 관절을 의미한다.
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
					//^^^편입^^^^
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

		// map의 특징 : 키를 이용해 인덱스처럼 접근할 경우 해당 키가 없다면
		// 만들어준다.
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

		// 전체 프레임 수만큼 반복한다.
		for (FbxLongLong j = Start; j <= End; ++j)
		{
			FbxTime	tTime = {};
			FBXKEYFRAME	tKeyFrame = {};

			// 현재 프레임에 해당하는 FbxTime을 만들어낸다.
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
			// 가중치 값에 따라 내림차순 정렬한다.
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