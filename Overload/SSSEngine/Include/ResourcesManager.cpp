#include "ResourcesManager.h"
#include "Mesh.h"
#include "Core.h"
#include "Sampler.h"
#include "Texture.h"
#include "PathManager.h"
#include "FBXLoader.h"
#include "Material.h"

SSS_USING

DEFINITION_SINGLE(CResourcesManager)

CResourcesManager::CResourcesManager()
{
}


CResourcesManager::~CResourcesManager()
{
	Safe_Release_Map(m_mapMesh);
	Safe_Release_Map(m_mapTexture);
	Safe_Release_Map(m_mapSampler);
}


bool CResourcesManager::CreateCone(const string & strKey, const string & strShaderKey, const string & strInputLayout, UINT iSides, float fAngle, const Vector3& vPivotOffset)
{
	//높이는 항상 1이 되도록 하고, Pivot은 설정해 줄 수 있도록 한다. 기본은 바닥이 Pivot이다.
	vector<Vector3> vecVerticesPosition;
	vector<Vector3> vecNormals;

	vector<VERTEXCOLORNORMAL> vecVertices;
	vector<UINT> vecIndices;

	//Angle에따른 바닥면의 버텍스 위치를 계산한다.
	//높이가 항상 1이기 때문에 Tangent 값이 반지름이 된다.
	float fDistanceFromCenter = tanf(fAngle);

	//바닥쪽에 (0,0,0)을 중심으로 하는 원위에 위치를 잡아준다.
	if (iSides < 3)
		return false;

	float fBottomAngle = 0.0f;
	float fIncrease = (2.0f * PI) / (float)iSides;

	vecVerticesPosition.push_back(Vector3(0.0f, 1.0f, 0.0f));
	for (UINT i = 0; i < iSides; ++i)
	{
		vecVerticesPosition.push_back(Vector3(cosf(fBottomAngle), 0.0f, sinf(fBottomAngle)) * fDistanceFromCenter);
		fBottomAngle += fIncrease;
	}

	//기둥의 법선을 만든다.
	vecNormals.push_back(Vector3(0.0f, 1.0f, 0.0f));

	size_t iSize = vecVerticesPosition.size();
	for (size_t i = 2; i < iSize; ++i)
	{
		Vector3 vEdge1 = (vecVerticesPosition[i - 1] - vecVerticesPosition[0]).Normalize();
		Vector3 vEdge2 = (vecVerticesPosition[i] - vecVerticesPosition[0]).Normalize();
		Vector3 vFaceNormal = vEdge1.Cross(vEdge2).Normalize();
		vecNormals.push_back(-vFaceNormal);
	}
	{
		Vector3 vEdge1 = (vecVerticesPosition[iSize - 1] - vecVerticesPosition[0]).Normalize();
		Vector3 vEdge2 = (vecVerticesPosition[1] - vecVerticesPosition[0]).Normalize();
		Vector3 vFaceNormal = vEdge1.Cross(vEdge2).Normalize();		
		vecNormals.push_back(-vFaceNormal);
	}
	
	//Side Face
	// 0번은 위쪽이기떄문에 1번부터
	{
		VERTEXCOLORNORMAL tVertex;
		tVertex.vColor = Vector4::white;
		tVertex.vNormal = vecNormals[0];
		tVertex.vPos = vecVerticesPosition[0];
		vecVertices.push_back(tVertex);
	}
	for (size_t i = 1; i <= iSides; ++i)
	{
		VERTEXCOLORNORMAL tVertex;
		tVertex.vColor = Vector4::white;
		if (i == 1)
		{
			tVertex.vNormal = (vecNormals[1] + vecNormals[vecNormals.size() - 1]).Normalize();
		}
		else
		{
			tVertex.vNormal = (vecNormals[i] + vecNormals[i - 1]).Normalize();
		}
		tVertex.vPos = vecVerticesPosition[i];
		vecVertices.push_back(tVertex);
	}

	//Side
	{
		vecIndices.push_back(0);
		vecIndices.push_back(1);
		vecIndices.push_back(iSides);
	}
	for (UINT i = 0; i < iSides - 1; ++i)
	{
		vecIndices.push_back(0);
		vecIndices.push_back(i + 2);
		vecIndices.push_back(i + 1);
	}

	//바닥면 법선 및 조립
	//Vertex는 바닥면 중앙에 하나 더 만들어주도록 한다.
	Vector3 vBottomNormal(0.0f, -1.0f, 0.0f);

	//바닥면 버텍스 생성
	size_t iOffsetIndex = vecVerticesPosition.size();
	vecVerticesPosition.push_back(Vector3(0.0f, 0.0f, 0.0f));
	
	{
		VERTEXCOLORNORMAL tVertex;
		tVertex.vColor = Vector4::white;
		tVertex.vNormal = vBottomNormal;
		tVertex.vPos = vecVerticesPosition[iOffsetIndex];
		vecVertices.push_back(tVertex);
	}
	for (size_t i = 0; i < iSides; ++i)
	{		
		VERTEXCOLORNORMAL tVertex;
		tVertex.vColor = Vector4::white;
		tVertex.vNormal = vBottomNormal;
		tVertex.vPos = vecVerticesPosition[i + 1];
		vecVertices.push_back(tVertex);				
	}

	//바닥면 인덱스
	{
		vecIndices.push_back(iOffsetIndex);
		vecIndices.push_back(iOffsetIndex + iSides);
		vecIndices.push_back(iOffsetIndex + 1);
	}
	for (UINT i = 0; i < iSides - 1; ++i)
	{
		vecIndices.push_back(iOffsetIndex);
		vecIndices.push_back(iOffsetIndex+ i + 1);
		vecIndices.push_back(iOffsetIndex+ i + 2);
	}

	CMesh*	pMesh = CreateMesh(strKey,
		strShaderKey, strInputLayout,
		vecVertices.size(), sizeof(VERTEXCOLORNORMAL), D3D11_USAGE_DEFAULT,
		&vecVertices[0], D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		vecIndices.size(), 4, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT,
		&vecIndices[0]);

	SAFE_RELEASE(pMesh);
}

CMesh * CResourcesManager::CreateSpherePos(const string & strKey, float fRadius, unsigned int iSubDivision)
{
	CMesh*	pMesh = FindMesh(strKey);

	if (pMesh)
		return pMesh;

	pMesh = new CMesh;

	if (!pMesh->CreateSpherePos(strKey, fRadius, iSubDivision))
	{
		SAFE_RELEASE(pMesh);
		return NULL;
	}

	pMesh->AddRef();

	m_mapMesh.insert(make_pair(strKey, pMesh));

	return pMesh;
}

void CResourcesManager::SetMeshDirectoryDescriptions(CMesh * pTargetMesh, const string& strKey, const string & strFullPath)
{
	string strIntrinsicKey;
	string strIntrinsicRelativeDirectory;
	string strIntrinsicPathKey;

	char* pRelativePath = NULL;
	char* pPathKey = NULL;
	GET_SINGLE(CPathManager)->ExtractRelativeDirectoryFromFullPath(strFullPath.c_str(), &pRelativePath, &pPathKey);

	strIntrinsicRelativeDirectory = pRelativePath;

	if (strKey.empty())
	{
		strIntrinsicKey = pRelativePath;
		strIntrinsicPathKey = pPathKey;		
	}
	else
	{
		strIntrinsicKey = strKey;
		strIntrinsicPathKey = pPathKey;
	}

	pTargetMesh->m_strKey = strIntrinsicKey;
	pTargetMesh->m_strDirectory = strIntrinsicRelativeDirectory;
	pTargetMesh->m_strPathKey = strIntrinsicPathKey;
}

bool CResourcesManager::Initialize()
{

	//Rectangle
	{
		VERTEXCOLOR tRectangle[4] =
		{
			VERTEXCOLOR(-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f),
			VERTEXCOLOR(0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f),
			VERTEXCOLOR(-0.5f, 0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f),
			VERTEXCOLOR(0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f)
		};

		//UINT	iRectangleIdx[2][3] = {{0, 2, 1}, { 1, 2, 3} };

		CMesh*	pMesh = CreateMesh(
			ENGINE_MESH_RECTANGLE_COLOR,
			STANDARD_COLOR_SHADER,
			POS_COLOR_LAYOUT,
			4, sizeof(VERTEXCOLOR), D3D11_USAGE_DYNAMIC,
			tRectangle, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		SAFE_RELEASE(pMesh);
	}

	//Orthography Rectangle
	{
		VERTEXCOLOR tRectangle[4] =
		{
			VERTEXCOLOR(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f),
			VERTEXCOLOR(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f),
			VERTEXCOLOR(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f),
			VERTEXCOLOR(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f)
		};
		
		UINT	iRectangleIdx[2][3] = {{ 0, 1, 2 },{ 1, 3, 2 } };

		CMesh*	pMesh = CreateMesh(ENGINE_MESH_ORTHO_RECTANGLE_COLOR,
			STANDARD_COLOR_SHADER, POS_COLOR_LAYOUT, 4, sizeof(VERTEXCOLOR), D3D11_USAGE_DYNAMIC,
			tRectangle, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6, 4, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT,
			iRectangleIdx);


		SAFE_RELEASE(pMesh);
	}
	

	//Debug Rectangle Frame
	{
		VERTEXCOLOR	tFrameRectColor[5] =
		{
			VERTEXCOLOR(0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f),
			VERTEXCOLOR(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f),
			VERTEXCOLOR(1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f),
			VERTEXCOLOR(0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f),
			VERTEXCOLOR(0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f)
		};

		CMesh* pMesh = CreateMesh(ENGINE_MESH_FRAME_RECT_COLOR,
			STANDARD_COLOR_SHADER, POS_COLOR_LAYOUT, 5, sizeof(VERTEXCOLOR),
			D3D11_USAGE_DEFAULT,	tFrameRectColor, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		SAFE_RELEASE(pMesh);
	}

	{
		VERTEXCOLOR	tFrameRectColor[2] =
		{
			VERTEXCOLOR(0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f),
			VERTEXCOLOR(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f),
		};

		CMesh* pMesh = CreateMesh(ENGINE_MESH_DEBUG_RAY,
			COLLIDER_SHADER, POS_COLOR_LAYOUT, 2, sizeof(VERTEXCOLOR),
			D3D11_USAGE_DEFAULT, tFrameRectColor, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		SAFE_RELEASE(pMesh);
	}

	{
		Vector3	vPos;

		CMesh* pMesh = CreateMesh(ENGINE_MESH_BILLBOARD,"Billboard", POS_LAYOUT,
			1, sizeof(Vector3), D3D11_USAGE_DEFAULT,
			&vPos, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		SAFE_RELEASE(pMesh);
	}


	//Particle
	{
		VERTEXPARTICLE vVertices[PARTICLE_MAX] = {};

		CMesh* pMesh = CreateMesh(ENGINE_MESH_PARTICLE, "Particle", PARTICLE_LAYOUT,
			PARTICLE_MAX, sizeof(VERTEXPARTICLE), D3D11_USAGE_DYNAMIC, &vVertices[0], D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		SAFE_RELEASE(pMesh);
	}

	// Trail
	{
		VERTEXTEX vVertextex[20] = {};

		CMesh* pMesh = CreateMesh("EngineTrailMesh", "TrailShader", POS_UV_LAYOUT,
			20, sizeof(VERTEXTEX), D3D11_USAGE_DYNAMIC, &vVertextex[0], D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		SAFE_RELEASE(pMesh);
	}
	
	{

		VERTEXTEX	tPerspectiveRectTex[4] =
		{
			VERTEXTEX(0.f, 0.f, 0.f, 0.f, 1.f),
			VERTEXTEX(1.f, 0.f, 0.f, 1.f, 1.f),
			VERTEXTEX(0.f, 1.f, 0.f, 0.f, 0.f),
			VERTEXTEX(1.f, 1.f, 0.f, 1.f, 0.f)
		};
		
		UINT	iRectIdx[2][3] = { { 0, 3, 1 },{ 0, 2, 3 } };

		CMesh* pMesh = CreateMesh(ENGINE_MESH_RECTANGLE_TEX,
			STANDARD_TEXTURE_SHADER, POS_UV_LAYOUT,
			4, sizeof(VERTEXTEX), D3D11_USAGE_DEFAULT,
			tPerspectiveRectTex, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			6, 4, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT,
			iRectIdx);

		SAFE_RELEASE(pMesh);
	}


	{

		VERTEXTEX	tOrthoRectTex[4] =
		{
			VERTEXTEX(0.f, 0.f, 0.f, 0.f, 0.f),
			VERTEXTEX(1.f, 0.f, 0.f, 1.f, 0.f),
			VERTEXTEX(0.f, 1.f, 0.f, 0.f, 1.f),
			VERTEXTEX(1.f, 1.f, 0.f, 1.f, 1.f)
		};


		UINT	iRectIdx[2][3] = { { 0, 1, 3 }, {0, 3, 2 } };

		CMesh* pMesh = CreateMesh(ENGINE_MESH_ORTHORECT_TEX,
			UI_TEXTURE_SHADER, POS_UV_LAYOUT,
			4, sizeof(VERTEXTEX), D3D11_USAGE_DEFAULT,
			tOrthoRectTex, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			6, 4, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT,
			iRectIdx);

		SAFE_RELEASE(pMesh);
	}	

	// Point Volume Light 용 메쉬
	{
		CMesh* pMesh = CreateSpherePos("PosSphere", 0.5f, 10);
		SAFE_RELEASE(pMesh);
	}

	// Fire Mesh
	{
		UINT	iFireIdx[6] = { 0, 1, 2, 3, 4, 5 };

		VERTEXTEX	tFireColor[6] =
		{
			VERTEXTEX(0.f, 1.0f, 0.0f, 0.0f, 0.0f),
			VERTEXTEX(1.0f, 1.0f, 0.0f, 1.0f, 0.0f),
			VERTEXTEX(0.f, 0.f, 0.0f, 0.0f, 1.0f),
			VERTEXTEX(0.f, 0.f, 0.0f, 0.0f, 1.0f),
			VERTEXTEX(1.0f, 1.0f, 0.0f,1.0f, 0.0f),
			VERTEXTEX(1.0f, 0.f, 0.0f, 1.0f, 1.0f)
		};

		CMesh* pMesh = CreateMesh("FireMesh",
			"FireShader", POS_UV_LAYOUT,
			6, sizeof(VERTEXTEX), D3D11_USAGE_DEFAULT,
			tFireColor, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			6, 6, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT,
			iFireIdx);

		SAFE_RELEASE(pMesh);
	}

	// Water Mesh
	{
		UINT iRectIdx[6] = { 0, 1, 3, 0, 3, 2 };

		VERTEXBUMP	VertexBump[4];

		VertexBump[0].vPos = Vector3(0.f, 0.f, 1.f);
		VertexBump[0].vNormal = Vector3(0.f, 1.f, 0.f);
		VertexBump[0].vUV = Vector2(0.f, 0.f);

		VertexBump[1].vPos = Vector3(1.f, 0.f, 1.f);
		VertexBump[1].vNormal = Vector3(0.f, 1.f, 0.f);
		VertexBump[1].vUV = Vector2(0.f, 1.f);

		VertexBump[2].vPos = Vector3(0.f, 0.f, 0.f);
		VertexBump[2].vNormal = Vector3(0.f, 1.f, 0.f);
		VertexBump[2].vUV = Vector2(1.f, 0.f);

		VertexBump[3].vPos = Vector3(1.f, 0.f, 0.f);
		VertexBump[3].vNormal = Vector3(0.f, 1.f, 0.f);
		VertexBump[3].vUV = Vector2(1.f, 1.f);

		for (int i = 0; i < 2; ++i)
		{
			int	idx0 = iRectIdx[i * 3];
			int	idx1 = iRectIdx[i * 3 + 1];
			int	idx2 = iRectIdx[i * 3 + 2];

			float	fVtx1[3], fVtx2[3];
			fVtx1[0] = VertexBump[idx1].vPos.x - VertexBump[idx0].vPos.x;
			fVtx1[1] = VertexBump[idx1].vPos.y - VertexBump[idx0].vPos.y;
			fVtx1[2] = VertexBump[idx1].vPos.z - VertexBump[idx0].vPos.z;

			fVtx2[0] = VertexBump[idx2].vPos.x - VertexBump[idx0].vPos.x;
			fVtx2[1] = VertexBump[idx2].vPos.y - VertexBump[idx0].vPos.y;
			fVtx2[2] = VertexBump[idx2].vPos.z - VertexBump[idx0].vPos.z;

			float	ftu[2], ftv[2];
			ftu[0] = VertexBump[idx1].vUV.x - VertexBump[idx0].vUV.x;
			ftv[0] = VertexBump[idx1].vUV.y - VertexBump[idx0].vUV.y;

			ftu[1] = VertexBump[idx2].vUV.x - VertexBump[idx0].vUV.x;
			ftv[1] = VertexBump[idx2].vUV.y - VertexBump[idx0].vUV.y;

			float	fDen = 1.f / (ftu[0] * ftv[1] - ftu[1] * ftv[0]);

			Vector3	vTangent;
			vTangent.x = (ftv[1] * fVtx1[0] - ftv[0] * fVtx2[0]) * fDen;
			vTangent.y = (ftv[1] * fVtx1[1] - ftv[0] * fVtx2[1]) * fDen;
			vTangent.z = (ftv[1] * fVtx1[2] - ftv[0] * fVtx2[2]) * fDen;

			vTangent = vTangent.Normalize();

			VertexBump[idx0].vTangent = vTangent;
			VertexBump[idx1].vTangent = vTangent;
			VertexBump[idx2].vTangent = vTangent;

			Vector3 vBinormal[3];
			VertexBump[idx0].vBinormal = VertexBump[idx0].vNormal.Cross(vTangent).Normalize();
			VertexBump[idx1].vBinormal = VertexBump[idx1].vNormal.Cross(vTangent).Normalize();
			VertexBump[idx2].vBinormal = VertexBump[idx2].vNormal.Cross(vTangent).Normalize();
		}

		CMesh* pMesh = GET_SINGLE(CResourcesManager)->CreateMesh("WaterMesh",
			"WaterShader", BUMP_LAYOUT,
			4, sizeof(VERTEXBUMP), D3D11_USAGE_DEFAULT,
			&VertexBump, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			6, sizeof(unsigned int), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT,
			&iRectIdx);

		SAFE_RELEASE(pMesh);
	}

	// Decal Mesh
	{
		Vector3	vCube[8] =
		{
			Vector3(-0.5f, 0.5f, -0.5f),
			Vector3(0.5f, 0.5f, -0.5f),
			Vector3(-0.5f, -0.5f, -0.5f),
			Vector3(0.5f, -0.5f, -0.5f),
			Vector3(-0.5f, 0.5f, 0.5f),
			Vector3(0.5f, 0.5f, 0.5f),
			Vector3(-0.5f, -0.5f, 0.5f),
			Vector3(0.5f, -0.5f, 0.5f)
		};

		UINT	iCubeIdx[36] = { 0, 1, 3, 0, 3, 2, 1, 5, 7, 1, 7, 3,
			4, 0, 2, 4, 2, 6, 5, 4, 6, 5, 6, 7,
			4, 5, 1, 4, 1, 0, 2, 3, 7, 2, 7, 6 };

		CMesh* pMesh = CreateMesh("EngineDecalMesh",
			"DecalShader", POS_LAYOUT,
			8, sizeof(Vector3), D3D11_USAGE_DEFAULT,
			vCube, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 36, 4,
			D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, iCubeIdx);

		SAFE_RELEASE(pMesh);
	}

	CSampler*	pSampler = CreateSampler(SAMPLER_LINEAR);
	SAFE_RELEASE(pSampler);

	pSampler = CreateSampler(SAMPLER_POINT, D3D11_FILTER_MIN_MAG_MIP_POINT);
	SAFE_RELEASE(pSampler);

	pSampler = CreateSampler("POINT_WRAP", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP);

	SAFE_RELEASE(pSampler);

	pSampler = CreateSampler("LINEAR_WRAP", D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP);

	SAFE_RELEASE(pSampler);

	pSampler = CreateSampler("LINEAR_CLAMP", D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP);

	SAFE_RELEASE(pSampler);

	pSampler = CreateSampler("POINT_MIRROR", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_MIRROR,
		D3D11_TEXTURE_ADDRESS_MIRROR, D3D11_TEXTURE_ADDRESS_MIRROR);

	SAFE_RELEASE(pSampler);

	pSampler = CreateSampler("LINEAR_MIRROR", D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MIRROR,
		D3D11_TEXTURE_ADDRESS_MIRROR, D3D11_TEXTURE_ADDRESS_MIRROR);

	SAFE_RELEASE(pSampler);

	pSampler = CreateSampler("HDRSampler", D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP,
		0.0f, 16, D3D11_COMPARISON_NEVER, Vector4::clear, FLT_MIN, FLT_MAX);

	SAFE_RELEASE(pSampler);

	CreateCone("Cone", STANDARD_COLOR_NORMAL_SHADER, POS_COLOR_NORMAL_LAYOUT, 100, 30.0f * D2R);

	CMesh* pMesh = LoadMesh(GIZMO_MESH_ARROW_X, L"Primitive\\ArrowX.obj", MESH_PATH, false);
	SAFE_RELEASE(pMesh);
	pMesh = LoadMesh(GIZMO_MESH_ARROW_Y, L"Primitive\\ArrowY.obj", MESH_PATH, false);
	SAFE_RELEASE(pMesh);
	pMesh = LoadMesh(GIZMO_MESH_ARROW_Z, L"Primitive\\ArrowZ.obj", MESH_PATH, false);
	SAFE_RELEASE(pMesh);
	pMesh = LoadOBJ(false, PRIMITIVE_MESH_CIRCLE, L"Primitive\\Circle.obj");
	SAFE_RELEASE(pMesh);
	pMesh = LoadOBJ(false, PRIMITIVE_MESH_PYRAMID, L"Primitive\\Pyramid.obj");
	SAFE_RELEASE(pMesh);
	pMesh = LoadOBJ(false, PRIMITIVE_MESH_CYLINDER, L"Primitive\\Cylinder.obj");
	SAFE_RELEASE(pMesh); 
	pMesh = LoadOBJ(false, PRIMITIVE_MESH_BOX, L"Primitive\\Box.obj");
	SAFE_RELEASE(pMesh);
	pMesh = LoadOBJ(false, PRIMITIVE_MESH_CUBE, L"Primitive\\Cube.obj");
	SAFE_RELEASE(pMesh);
	pMesh = LoadOBJ(false, PRIMITIVE_MESH_SPHERE, L"Primitive\\Sphere.obj");
	SAFE_RELEASE(pMesh);
	pMesh = LoadOBJ(true, MESH_SKYBOX, L"Primitive\\Sphere.obj");
	SAFE_RELEASE(pMesh);
	pMesh = LoadOBJ(false, PRIMITIVE_MESH_CAPSULE, L"Primitive\\Capsule.obj");
	SAFE_RELEASE(pMesh);
	pMesh = LoadMesh(PRIMITIVE_MESH_DOUGHNUT, L"Primitive\\Doughnut.FBX", MESH_PATH, false);
	SAFE_RELEASE(pMesh);
	
	return true;
}


CMesh * CResourcesManager::CreateMesh(
	const string & strKey, 
	const string& strShaderKey,
	const string& strInputLayoutKey,
	 UINT iVtxCount, UINT iVtxSize,
	D3D11_USAGE eVtxUsage, void * pVtxData, D3D11_PRIMITIVE_TOPOLOGY ePrimitiveType,
	UINT iIdxCount, UINT iIdxSize, D3D11_USAGE eIdxUsage, DXGI_FORMAT eIdxFmt, void * pIdxData)
{
	CMesh*	pMesh = FindMesh(strKey);

	if (pMesh)
		return pMesh;

	pMesh = new CMesh;

	if (!pMesh->CreateMesh(strKey, strShaderKey, strInputLayoutKey,  iVtxCount, iVtxSize, eVtxUsage,
		pVtxData, ePrimitiveType, iIdxCount, iIdxSize, eIdxUsage,
		eIdxFmt, pIdxData))
	{
		SAFE_RELEASE(pMesh);
		return NULL;
	}

	pMesh->AddRef();

	pMesh->m_strKey = strKey;

	m_mapMesh.insert(make_pair(strKey, pMesh));

	return pMesh;
}
CMesh * CResourcesManager::LoadMesh(const string & strKey, const wchar_t * pFileName, const string & strPathKey, bool bUseTexture)
{
	wstring strFullPathW;
	string strFullPathA;
	string strIntrinsicKey;
	string strIntrinsicRelativeDirectory;
	string strIntrinsicPathKey;

	if (!strPathKey.empty() && strPathKey != NO_USE_PATH)
	{
		strFullPathW = GET_SINGLE(CPathManager)->FindPath(strPathKey);
	}

	strFullPathW += pFileName;
	strFullPathA = string(strFullPathW.begin(), strFullPathW.end());
	char* pKey = NULL;
	char* pPathKey = NULL;
	GET_SINGLE(CPathManager)->ExtractRelativeDirectoryFromFullPath(strFullPathA.c_str(), &pKey, &pPathKey);

	if (strKey.empty())
	{
		strIntrinsicKey = pKey;
		if (strPathKey.empty() || strPathKey == NO_USE_PATH)
		{
			strIntrinsicPathKey = pPathKey;
			strIntrinsicRelativeDirectory = pKey;
		}
		else
		{
			wstring strFileNameW(pFileName);
			strIntrinsicPathKey = strPathKey;
			strIntrinsicRelativeDirectory = string(strFileNameW.begin(), strFileNameW.end());
		}
	}
	else
	{
		strIntrinsicKey = strKey;
		if (strPathKey.empty() || strPathKey == NO_USE_PATH)
		{
			strIntrinsicPathKey = pPathKey;
			strIntrinsicRelativeDirectory = pKey;
		}
		else
		{
			wstring strFileNameW(pFileName);
			strIntrinsicPathKey = strPathKey;
			strIntrinsicRelativeDirectory = string(strFileNameW.begin(), strFileNameW.end());
		}
	}

	CMesh* pMesh = FindMesh(strIntrinsicKey);

	if (pMesh)
	{
		return pMesh;
	}

	// 이미지 로딩을 위한 확장자 포맷을 구분한다.
	char	strExt[_MAX_EXT] = {};
	_splitpath_s(strFullPathA.c_str(), 0, 0, 0, 0, 0, 0, strExt, _MAX_EXT);
	_strupr_s(strExt);

	if (strcmp(strExt, ".OBJ") == 0)
	{
		
		return LoadOBJ(bUseTexture, strIntrinsicKey, wstring(strIntrinsicRelativeDirectory.begin(), strIntrinsicRelativeDirectory.end()).c_str(), strIntrinsicPathKey);
	}

	else if (strcmp(strExt, ".FBX") == 0)
	{		
		return LoadFBX(strIntrinsicKey, wstring(strIntrinsicRelativeDirectory.begin(), strIntrinsicRelativeDirectory.end()).c_str(), strIntrinsicPathKey);
	}

	else if (strcmp(strExt, ".SSSMESH") == 0)
	{
		return LoadSSSMesh(strIntrinsicKey, wstring(strIntrinsicRelativeDirectory.begin(), strIntrinsicRelativeDirectory.end()).c_str(), strIntrinsicPathKey);
	}
}

CMesh * CResourcesManager::LoadFBX(const string & strKey, const wchar_t * pFileName, const string & strPathKey)
{
	wstring strFullPathW;

	if (strPathKey != NO_USE_PATH)
	{
		strFullPathW = GET_SINGLE(CPathManager)->FindPath(strPathKey);
	}

	strFullPathW += pFileName;

	string strFullPathA(strFullPathW.begin(), strFullPathW.end());
	
	CFbxLoader Loader;

	if (!Loader.LoadFbx(strFullPathA.c_str()))
	{
		return NULL;
	}

	CMesh* pMesh = ConvertFbx(strKey, &Loader);

	SetMeshDirectoryDescriptions(pMesh, strKey, strFullPathA);

	return pMesh;
}


CMesh * CResourcesManager::LoadOBJ(bool bUseTexture, const string& strKey, const wchar_t* pFileName, const string& strPathKey)
{
	wstring	strPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);
	strPath += pFileName;

	char strFullPath[512] = {};
	WideCharToMultiByte(CP_ACP, 0, strPath.c_str(), -1, strFullPath, lstrlenW(strPath.c_str()), 0, 0);

	vector<Vector3> VerticesPositionContainer;
	vector<Vector2> VerticesUVContainer;
	vector<Vector3> VerticesNormalContainer;
	
	vector<VERTEXTEXNORMAL> Vertices;
	vector<VERTEXCOLORNORMAL> ColoredVertices;
	vector<UINT> Indices;
	
	FILE* pFile = NULL;
	
	fopen_s(&pFile, strFullPath, "rt");

	if (!pFile)
	{
		return false;
	}
	else
	{
		char pLine[512] = {};
		while (true)
		{
			fgets(pLine, 512, pFile);
			int iLength = strlen(pLine);
			if (feof(pFile))
			{
				break;
			}

			if (pLine[0] == '\n' || pLine[0] == '#' || pLine[0] == 's' || pLine[0] == 'g')
				continue;

			if (pLine[0] == 'v' && pLine[1] != 't' && pLine[1] != 'n')
			{
				//Vertex의 위치들을 저장해둔다.
				Vector3 position;
				char* pContext = NULL;
				char* value = strtok_s(&pLine[1], " ", &pContext);
				position.x = stof(value);
				value = strtok_s(NULL, " ", &pContext);
				position.y = stof(value);
				value = strtok_s(NULL, " ", &pContext);
				position.z = stof(value) * -1.0f;

				VerticesPositionContainer.push_back(position);
			}
			else if (pLine[0] == 'v' && pLine[1] == 't' && pLine[1] != 'n')
			{
				Vector2 uv;
				char* pContext = NULL;
				char* value = strtok_s(&pLine[2], " ", &pContext);
				uv.x = stof(value);
				value = strtok_s(NULL, " ", &pContext);
				uv.y = 1.0f - stof(value);
				VerticesUVContainer.push_back(uv);
			}
			else if (pLine[0] == 'v' && pLine[1] != 't' && pLine[1] == 'n')
			{
				Vector3 normal;
				char* pContext = NULL;
				char* value = strtok_s(&pLine[2], " ", &pContext);
				normal.x = stof(value);
				value = strtok_s(NULL, " ", &pContext);
				normal.y = stof(value);
				value = strtok_s(NULL, " ", &pContext);
				normal.z = stof(value) * -1.0f;

				VerticesNormalContainer.push_back(normal);
			}
			else if (pLine[0] == 'f')
			{
				UINT PositionIndex[3];
				UINT TextureIndex[3];
				UINT NormalIndex[3];

				char* pContext = NULL;
				char* value = strtok_s(&pLine[2], "/", &pContext);
				PositionIndex[0] = stoi(value) - 1;
				value = strtok_s(NULL, "/", &pContext);
				TextureIndex[0] = stoi(value) - 1;
				value = strtok_s(NULL, " ", &pContext);
				NormalIndex[0] = stoi(value) - 1;

				//2
				value = strtok_s(NULL, "/", &pContext);
				PositionIndex[1] = stoi(value) - 1;
				value = strtok_s(NULL, "/", &pContext);
				TextureIndex[1] = stoi(value) - 1;
				value = strtok_s(NULL, " ", &pContext);
				NormalIndex[1] = stoi(value) - 1;
				//3
				value = strtok_s(NULL, "/", &pContext);
				PositionIndex[2] = stoi(value) - 1;
				value = strtok_s(NULL, "/", &pContext);
				TextureIndex[2] = stoi(value) - 1;
				value = strtok_s(NULL, " ", &pContext);
				NormalIndex[2] = stoi(value) - 1;

				
				//Face 조립
				for (int i = 0; i < 3; ++i)
				{
					if (bUseTexture)
					{
						VERTEXTEXNORMAL tVertex;
						tVertex.vPos = VerticesPositionContainer[PositionIndex[2 - i]];
						tVertex.vUV = VerticesUVContainer[TextureIndex[2 - i]];
						tVertex.vNormal = VerticesNormalContainer[NormalIndex[2 - i]];
						Vertices.push_back(tVertex);
						Indices.push_back(Indices.size());
					}
					else
					{
						Vector4 vRandomColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

						VERTEXCOLORNORMAL tVertex; 
						tVertex.vPos = VerticesPositionContainer[PositionIndex[2 - i]];
						tVertex.vNormal = VerticesNormalContainer[NormalIndex[2 - i]];
						tVertex.vColor = vRandomColor;
						ColoredVertices.push_back(tVertex);
						Indices.push_back(Indices.size());
					}
				}				
			}
		}

		fclose(pFile);
	}
	
	if (bUseTexture)
	{
		CMesh* pMesh = CreateMesh(strKey,
			STANDARD_TEXTURE_NORMAL_SHADER, POS_UV_NORMAL_LAYOUT,
			Vertices.size(), sizeof(VERTEXTEXNORMAL), D3D11_USAGE_DEFAULT,
			&Vertices[0], D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			Indices.size(), sizeof(unsigned int), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT,
			&Indices[0]);


		SetMeshDirectoryDescriptions(pMesh, strKey, strFullPath);

		return pMesh;
	}
	else
	{
		CMesh* pMesh = CreateMesh(strKey,
			STANDARD_COLOR_NORMAL_SHADER, POS_COLOR_NORMAL_LAYOUT,
			ColoredVertices.size(), sizeof(VERTEXCOLORNORMAL), D3D11_USAGE_DEFAULT,
			&ColoredVertices[0], D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			Indices.size(), sizeof(unsigned int), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT,
			&Indices[0]);

		SetMeshDirectoryDescriptions(pMesh, strKey, strFullPath);
		return pMesh;
	}
}

/*LoadSSSMesh() dose not contain exception*/
CMesh * CResourcesManager::LoadSSSMesh(const string & strKey, const wchar_t * pFileName, const string & strPathKey)
{
	CMesh* pMesh = new CMesh;

	pMesh->Load(strKey, pFileName, strPathKey);

	m_mapMesh.insert(make_pair(strKey, pMesh));

	pMesh->AddRef();
	return pMesh;
}

/*ConvertFbx() dose not increasing Reference Count*/
CMesh* CResourcesManager::ConvertFbx(const string& strKey, CFbxLoader * pLoader)
{
	CMesh* pMesh = new CMesh;

	const vector<PFBXMESHCONTAINER>*	pvecContainer = pLoader->GetMeshContainers();
	const vector<vector<PFBXMATERIAL>>*	pvecMaterials = pLoader->GetMaterials();

	vector<PFBXMESHCONTAINER>::const_iterator	iter;
	vector<PFBXMESHCONTAINER>::const_iterator	iterEnd = pvecContainer->end();

	vector<bool>	vecEmptyIndex;
	int iContainerIndex = 0;
	
	for (iter = pvecContainer->begin(); iter != iterEnd; ++iter)
	{
		PMESHCONTAINER	pContainer = new MESHCONTAINER;

		pMesh->m_vecMeshContainer.push_back(pContainer);
		pContainer->matLocal = (*iter)->matTransform;
		pContainer->strContainerName = pLoader->m_pScene->GetGeometry(iContainerIndex)->GetInitialName();
		int	iVtxSize = 0;

		// 범프가 있을 경우
		if ((*iter)->bBump)
		{
			if ((*iter)->bAnimation)
			{
				pMesh->m_strShaderKey = STANDARD_BUMP_ANIM_SHADER;
				pMesh->m_strLayoutKey = BUMP_ANIM_LAYOUT;
				iVtxSize = sizeof(VERTEXBUMPANIM);

				vector<VERTEXBUMPANIM>	vecVtx;

				for (size_t i = 0; i < (*iter)->vecPos.size(); ++i)
				{
					VERTEXBUMPANIM	tVtx = {};

					tVtx.vPos = (*iter)->vecPos[i];
					tVtx.vNormal = (*iter)->vecNormal[i];
					tVtx.vUV = (*iter)->vecUV[i];
					tVtx.vTangent = (*iter)->vecTangent[i];
					tVtx.vBinormal = (*iter)->vecBinormal[i];
					tVtx.vWeight = (*iter)->vecBlendWeight[i];
					tVtx.vIndex = (*iter)->vecBlendIndex[i];

					vecVtx.push_back(tVtx);
				}

				if (!pMesh->CreateVertexBuffer(vecVtx.size(), iVtxSize,
					D3D11_USAGE_DEFAULT, &vecVtx[0],
					D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pContainer))
					return false;
			}

			else
			{
				pMesh->m_strShaderKey = STANDARD_BUMP_SHADER;
				pMesh->m_strLayoutKey = BUMP_LAYOUT;
				iVtxSize = sizeof(VERTEXBUMP);

				vector<VERTEXBUMP>	vecVtx;

				for (size_t i = 0; i < (*iter)->vecPos.size(); ++i)
				{
					VERTEXBUMP	tVtx = {};

					tVtx.vPos = (*iter)->vecPos[i];
					tVtx.vNormal = (*iter)->vecNormal[i];
					tVtx.vUV = (*iter)->vecUV[i];
					tVtx.vTangent = (*iter)->vecTangent[i];
					tVtx.vBinormal = (*iter)->vecBinormal[i];

					tVtx.vUV.x -= (int)tVtx.vUV.x;
					tVtx.vUV.y -= (int)tVtx.vUV.y;

					vecVtx.push_back(tVtx);
				}

				if (!pMesh->CreateVertexBuffer(vecVtx.size(),
					iVtxSize, D3D11_USAGE_DEFAULT, &vecVtx[0],
					D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pContainer))
					return false;
			}
		}

		// 범프가 없을 경우
		else
		{
			if ((*iter)->bAnimation)
			{
				pMesh->m_strShaderKey = STANDARD_TEXTURE_NORMAL_ANIM_SHADER;
				pMesh->m_strLayoutKey = POS_TEXTURE_NORMAL_ANIM_LAYOUT;
				iVtxSize = sizeof(VERTEXTEXNORMALANIM);

				vector<VERTEXTEXNORMALANIM>	vecVtx;

				for (size_t i = 0; i < (*iter)->vecPos.size(); ++i)
				{
					VERTEXTEXNORMALANIM	tVtx = {};

					tVtx.vPos = (*iter)->vecPos[i];
					tVtx.vNormal = (*iter)->vecNormal[i];
					tVtx.vUV = (*iter)->vecUV[i];

					tVtx.vWeight = (*iter)->vecBlendWeight[i];
					tVtx.vIndex = (*iter)->vecBlendIndex[i];

					vecVtx.push_back(tVtx);
				}

				if (!pMesh->CreateVertexBuffer(vecVtx.size(),
					iVtxSize, D3D11_USAGE_DEFAULT, &vecVtx[0],
					D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pContainer))
					return false;
			}

			else
			{
				pMesh->m_strShaderKey = STANDARD_TEXTURE_NORMAL_SHADER;
				pMesh->m_strLayoutKey = POS_UV_NORMAL_LAYOUT;
				iVtxSize = sizeof(VERTEXTEXNORMAL);

				vector<VERTEXTEXNORMAL>	vecVtx;

				for (size_t i = 0; i < (*iter)->vecPos.size(); ++i)
				{
					VERTEXTEXNORMAL	tVtx = {};

					tVtx.vPos = (*iter)->vecPos[i];
					tVtx.vNormal = (*iter)->vecNormal[i];
					tVtx.vUV = (*iter)->vecUV[i];

					tVtx.vUV.x -= (int)tVtx.vUV.x;
					tVtx.vUV.y -= (int)tVtx.vUV.y;

					vecVtx.push_back(tVtx);
				}

				if (!pMesh->CreateVertexBuffer(vecVtx.size(),
					iVtxSize, D3D11_USAGE_DEFAULT, &vecVtx[0],
					D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pContainer))
					return false;
			}
		}

		// 인덱스버퍼 생성
		for (size_t i = 0; i < (*iter)->vecIndices.size(); ++i)
		{
			if ((*iter)->vecIndices[i].empty())
			{
				vecEmptyIndex.push_back(false);
				continue;
			}

			vecEmptyIndex.push_back(true);

			if (!pMesh->CreateIndexBuffer((*iter)->vecIndices[i].size(), 4,
				D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT,
				&(*iter)->vecIndices[i][0], pContainer))
				return false;
		}

		++iContainerIndex;
	}

	// 재질 정보를 읽어온다.
	const vector<vector<PFBXMATERIAL>>*	pMaterials = pLoader->GetMaterials();

	vector<vector<PFBXMATERIAL>>::const_iterator	iterM;
	vector<vector<PFBXMATERIAL>>::const_iterator	iterMEnd = pMaterials->end();

	if (!pMaterials->empty())
	{
		pMesh->m_pMaterial = new CMaterial;

		if (!pMesh->m_pMaterial->Initialize())
		{
			SAFE_RELEASE(pMesh->m_pMaterial);
			return NULL;
		}

		pMesh->m_pMaterial->ClearContainer();
	}

	int	iContainer = 0;
	for (iterM = pMaterials->begin(); iterM != iterMEnd; ++iterM, ++iContainer)
	{
		// 재질의 컨테이너를 추가한다.
		pMesh->m_pMaterial->AddContainer();

		for (size_t i = 0; i < (*iterM).size(); ++i)
		{
			// 인덱스 버퍼가 비어있을 경우에는 재질을 추가하지 않는다.
			if (!vecEmptyIndex[i])
				continue;

			// 재질의 서브셋을 추가한다.
			pMesh->m_pMaterial->AddSubset(iContainer);

			// 재질 정보를 얻어온다.
			PFBXMATERIAL	pMtrl = (*iterM)[i];

			pMesh->m_pMaterial->SetMaterial(pMtrl->vDif, pMtrl->vAmb, pMtrl->vSpc, pMtrl->fShininess, pMtrl->vEmv, iContainer, i);

			// 이름을 불러온다.
			char	strName[MAX_PATH] = {};
			_splitpath_s(pMtrl->strDifTex.c_str(), NULL, 0, NULL, 0, strName, MAX_PATH, NULL, 0);
			wchar_t	strPath[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, pMtrl->strDifTex.c_str(), -1, strPath, pMtrl->strDifTex.length());

			pMesh->m_pMaterial->SetSampler(SAMPLER_LINEAR, iContainer, i);
			pMesh->m_pMaterial->SetSamplerRegister(0, iContainer, i);

			FILE* pFile = NULL;
			fopen_s(&pFile, pMtrl->strDifTex.c_str(), "rb");
			if (pFile)
			{
				fclose(pFile);
				pFile = NULL;
				pMesh->m_pMaterial->SetDiffuseTextureFromFullPath(strName, strPath, iContainer, i);
			}
			

			if (!pMtrl->strBumpTex.empty())
			{
				memset(strName, 0, MAX_PATH);
				_splitpath_s(pMtrl->strBumpTex.c_str(), NULL, 0, NULL, 0, strName, MAX_PATH, NULL, 0);
				memset(strPath, 0, sizeof(wchar_t) * MAX_PATH);
				MultiByteToWideChar(CP_ACP, 0, pMtrl->strBumpTex.c_str(), -1, strPath, pMtrl->strBumpTex.length());

				FILE* pFile = NULL;
				fopen_s(&pFile, pMtrl->strBumpTex.c_str(), "rb");
				if (pFile)
				{
					fclose(pFile);
					pFile = NULL;
					pMesh->m_pMaterial->SetNormalTextureFromFullPath(strName, strPath, iContainer, i);
				}
			}

			if (!pMtrl->strSpcTex.empty())
			{
				memset(strName, 0, MAX_PATH);
				_splitpath_s(pMtrl->strSpcTex.c_str(), NULL, 0, NULL, 0,
					strName, MAX_PATH, NULL, 0);

				memset(strPath, 0, sizeof(wchar_t) * MAX_PATH);

				MultiByteToWideChar(CP_ACP, 0, pMtrl->strSpcTex.c_str(),
					-1, strPath, pMtrl->strSpcTex.length());

				FILE* pFile = NULL;
				fopen_s(&pFile, pMtrl->strSpcTex.c_str(), "rb");
				if (pFile)
				{
					fclose(pFile);
					pFile = NULL;
					pMesh->m_pMaterial->SetSpecularTextureFromFullPath(strName, strPath, iContainer, i);
				}
			}

			if (!pMtrl->strAlphaTex.empty())
			{
				memset(strName, 0, MAX_PATH);
				_splitpath_s(pMtrl->strAlphaTex.c_str(), NULL, 0, NULL, 0, strName, MAX_PATH, NULL, 0);
				memset(strPath, 0, sizeof(wchar_t) * MAX_PATH);
				MultiByteToWideChar(CP_ACP, 0, pMtrl->strAlphaTex.c_str(), -1, strPath, pMtrl->strAlphaTex.length());

				FILE* pFile = NULL;
				fopen_s(&pFile, pMtrl->strAlphaTex.c_str(), "rb");
				if (pFile)
				{
					fclose(pFile);
					pFile = NULL;
					pMesh->m_pMaterial->SetAlphaTextureFromFullPath(strName, strPath, iContainer, i);
				}
			}
		}
	}

	// 텍스쳐가 저장된 폴더명을 키로 변경한다.
	char	strFullName[MAX_PATH] = {};
	if (pMaterials->size() > 0)
	{
		iterM = pMaterials->begin();
		strcpy_s(strFullName, (*iterM)[0]->strDifTex.c_str());
	}

	int	iLength = strlen(strFullName);
	for (int i = iLength - 1; i >= 0; --i)
	{
		if (strFullName[i] == '\\' || strFullName[i] == '/')
		{
			memset(strFullName + (i + 1), 0, sizeof(char) * (iLength - (i + 1)));
			strFullName[i] = '\\';
			break;
		}
	}

	char	strChange[MAX_PATH] = {};
	strcpy_s(strChange, strFullName);
	iLength = strlen(strChange);
	for (int i = iLength - 2; i >= 0; --i)
	{
		if (strChange[i] == '\\' || strChange[i] == '/')
		{
			memset(strChange + (i + 1), 0, sizeof(char) * (iLength - (i + 1)));
			break;
		}
	}

	strcat_s(strChange, pMesh->m_strTag.c_str());
	strcat_s(strChange, "\\");

	MoveFileA(strFullName, strChange);

	// Mesh\\ 까지의 경로를 제거한다.
	iLength = strlen(strChange);
	for (int i = iLength - 2; i >= 0; --i)
	{
		char	cText[5] = {};
		memcpy(cText, &strChange[i - 4], 4);
		_strupr_s(cText);

		if (strcmp(cText, "MESH") == 0)
		{
			memset(strChange, 0, sizeof(char) * (i + 1));
			memcpy(strChange, &strChange[i + 1], sizeof(char) * (iLength - (i + 1)));
			memset(strChange + (i + 1), 0, sizeof(char) * (iLength - (i + 1)));
			break;
		}
	}

	pMesh->m_vLength = pMesh->m_vMax - pMesh->m_vMin;
	pMesh->m_vCenter = (pMesh->m_vMax + pMesh->m_vMin) / 2.f;
	pMesh->m_fRadius = pMesh->m_vLength.Length() / 2.f;

	// 애니메이션 처리
	const vector<PFBXBONE>*	pvecBone = pLoader->GetBones();

	if (pvecBone->empty())
	{
		m_mapMesh.insert(make_pair(strKey, pMesh));
		pMesh->AddRef();
		return pMesh;
	}

	//// 본 수만큼 반복한다.
	vector<PFBXBONE>::const_iterator	iterB;
	vector<PFBXBONE>::const_iterator	iterBEnd = pvecBone->end();

	for (iterB = pvecBone->begin(); iterB != iterBEnd; ++iterB)
	{
		PBONE	pBone = new BONE;

		pBone->strName = (*iterB)->strName;
		pBone->iDepth = (*iterB)->iDepth;
		pBone->iParentIndex = (*iterB)->iParentIndex;

		float	fMat[4][4];

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				fMat[i][j] = (*iterB)->matOffset.mData[i].mData[j];
			}
		}

		pBone->matOffset = new Matrix;
		*pBone->matOffset = fMat;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				fMat[i][j] = (*iterB)->matBone.mData[i].mData[j];
			}
		}

		pBone->matBone = new Matrix;
		*pBone->matBone = fMat;

		pMesh->m_vecBone.push_back(pBone);
	}

	// 애니메이션 클립을 추가한다.
	const vector<PFBXANIMATIONCLIP>* pvecClip = pLoader->GetClips();

	// 클립을 읽어온다.
	vector<PFBXANIMATIONCLIP>::const_iterator	iterC;
	vector<PFBXANIMATIONCLIP>::const_iterator	iterCEnd = pvecClip->end();

	for (iterC = pvecClip->begin(); iterC != iterCEnd; ++iterC)
	{
		pMesh->SetClipInfo(*iterC);
	}

	m_mapMesh.insert(make_pair(strKey, pMesh));

	pMesh->AddRef();
	return pMesh;
}
CMesh * CResourcesManager::FindMesh(const string & strKey)
{
	unordered_map<string, class CMesh*>::iterator	iter = m_mapMesh.find(strKey);

	if (iter == m_mapMesh.end())
		return NULL;

	iter->second->AddRef();

	return iter->second;
}

CTexture * CResourcesManager::LoadTexture(const string & strKey, const wchar_t * pFileName, const string & strPathKey)
{
	CTexture*	pTexture = FindTexture(strKey);

	if (pTexture)
		return pTexture;

	pTexture = new CTexture;

	if (!pTexture->LoadTexture(strKey, pFileName, strPathKey))
	{
		SAFE_RELEASE(pTexture);
		return NULL;
	}

	pTexture->AddRef();

	m_mapTexture.insert(make_pair(strKey, pTexture));

	return pTexture;
}

CTexture * CResourcesManager::LoadTexture(const string & strKey, const vector<wstring>& vecFileName, const string & strPathKey)
{
	CTexture*	pTexture = FindTexture(strKey);

	if (pTexture)
		return pTexture;

	pTexture = new CTexture;

	if (!pTexture->LoadTexture(strKey, vecFileName, strPathKey))
	{
		SAFE_RELEASE(pTexture);
		return NULL;
	}

	pTexture->AddRef();

	m_mapTexture.insert(make_pair(strKey, pTexture));

	return pTexture;
}

CTexture * CResourcesManager::LoadTextureFromFullPath(const string & strKey, const wchar_t * pFullPath)
{
	CTexture*	pTexture = FindTexture(strKey);

	if (pTexture)
		return pTexture;

	pTexture = new CTexture;

	if (!pTexture->LoadTextureFromFullPath(strKey, pFullPath))
	{
		SAFE_RELEASE(pTexture);
		return NULL;
	}

	pTexture->AddRef();

	m_mapTexture.insert(make_pair(strKey, pTexture));

	return pTexture;
}

CTexture * CResourcesManager::LoadTextureFromFullPath(const string & strKey, const vector<wstring>& vecFullPath)
{
	CTexture*	pTexture = FindTexture(strKey);

	if (pTexture)
		return pTexture;

	pTexture = new CTexture;

	if (!pTexture->LoadTextureFromFullPath(strKey, vecFullPath))
	{
		SAFE_RELEASE(pTexture);
		return NULL;
	}

	pTexture->AddRef();

	m_mapTexture.insert(make_pair(strKey, pTexture));

	return pTexture;
}

CTexture * CResourcesManager::FindTexture(const string & strKey)
{
	unordered_map<string, class CTexture*>::iterator	iter = m_mapTexture.find(strKey);

	if (iter == m_mapTexture.end())
		return NULL;

	iter->second->AddRef();

	return iter->second;
}

CSampler * CResourcesManager::CreateSampler(const string & strKey, D3D11_FILTER eFilter, D3D11_TEXTURE_ADDRESS_MODE eU, D3D11_TEXTURE_ADDRESS_MODE eV, D3D11_TEXTURE_ADDRESS_MODE eW, float fMipLODBias, UINT iMaxAnisotropy, D3D11_COMPARISON_FUNC eFunc, const Vector4 & vBorderColor, float fMinLOD, float fMaxLOD)
{
	CSampler*	pSampler = FindSampler(strKey);

	if (pSampler)
		return pSampler;

	pSampler = new CSampler;

	if (!pSampler->CreateSampler(strKey, eFilter, eU, eV, eW, fMipLODBias, iMaxAnisotropy,
		eFunc, vBorderColor, fMinLOD, fMaxLOD))
	{
		SAFE_RELEASE(pSampler);
		return NULL;
	}

	pSampler->AddRef();
	pSampler->m_strKey = strKey;
	m_mapSampler.insert(make_pair(strKey, pSampler));

	return pSampler;
}

CSampler * CResourcesManager::FindSampler(const string & strKey)
{
	unordered_map<string, class CSampler*>::iterator	iter = m_mapSampler.find(strKey);

	if (iter == m_mapSampler.end())
		return NULL;

	iter->second->AddRef();

	return iter->second;
}
