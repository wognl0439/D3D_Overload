#include "Terrain.h"
#include "../Mesh.h"
#include "../Material.h"
#include "../PathManager.h"
#include "../ResourcesManager.h"
#include "../ShaderManager.h"
#include "MeshRenderer.h"
#include "../Transform.h"
#include "../Texture.h"
#include "../Mathf.h"
#include "../NavigationMesh.h"
#include "../NavigationManager.h"
#include "../Debug.h"


SSS_USING

CTerrain::CTerrain() :
	m_pMaterial(NULL),
	m_pRenderer(NULL),
	m_pMesh(NULL),
	m_iSplatCount(0),
	m_fDetailLevel(1),
	m_fMaxHeight(30),
	m_fMinHeight(0),
	m_bNaviMesh(false)
{
}

CTerrain::CTerrain(const CTerrain & Terrain) :
	CComponent(Terrain)
{
	m_iSplatCount = Terrain.m_iSplatCount;
	m_fDetailLevel = Terrain.m_fDetailLevel;
	m_fMaxHeight = Terrain.m_fMaxHeight;
	m_fMinHeight = Terrain.m_fMinHeight;
}

CTerrain::~CTerrain()
{
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pRenderer);
}

Vector2 CTerrain::GetTerrainSize() const
{
	return Vector2(m_iWidth, m_iHeight);
}

void CTerrain::SetMouseUV(Vector2 vPos, float Radius)
{
	m_tCBuffer.vMousePosition = vPos;
	m_tCBuffer.fRadius = Radius / (m_iWidth * m_pTransform->GetWorldScale().x);
}

bool CTerrain::SaveTerrainData(const wchar_t* pTerrainFileName, const wchar_t* pMeshFileName, const string& strPathKey)
{
	string	strFullPath;
	const char*	pBasePath = GET_SINGLE(CPathManager)->FindPathMultiByte(strPathKey);

	if (pBasePath)
		strFullPath = pBasePath;

	char strPath[512] = {};
	WideCharToMultiByte(CP_ACP, 0, pTerrainFileName, -1, strPath, lstrlenW(pTerrainFileName), 0, 0);
	strFullPath += strPath;

	FILE* pFile = NULL;

	fopen_s(&pFile, strFullPath.c_str(), "wb");

	m_pMaterial->Save(pFile);

	fwrite(&m_fDetailLevel, sizeof(float), 1, pFile);
	fwrite(&m_iSplatCount, sizeof(int), 1, pFile);
	fwrite(&m_bNaviMesh, sizeof(bool), 1, pFile);
	fwrite(&m_iWidth, sizeof(int), 1, pFile);
	fwrite(&m_iHeight, sizeof(int), 1, pFile);

	if (!m_vecFaceNormal.empty())
		m_vecFaceNormal.clear();

	// 정점정보를 생성한다.
	vector<unsigned int>	vecIdx;

	// 인덱스를 구해준다.
	for (unsigned int i = 0; i < m_iHeight - 1; ++i)
	{
		for (unsigned int j = 0; j < m_iWidth - 1; ++j)
		{
			int	idx = i * m_iWidth + j;

			// 우상단 삼각형
			vecIdx.push_back(idx);
			vecIdx.push_back(idx + 1);
			vecIdx.push_back(idx + m_iWidth + 1);

			// 삼각형의 면법선을 구한다.
			Vector3	vEdge1 = m_vecNowVertex[idx + 1].vPos - m_vecNowVertex[idx].vPos;
			Vector3	vEdge2 = m_vecNowVertex[idx + m_iWidth + 1].vPos - m_vecNowVertex[idx].vPos;

			vEdge1 = vEdge1.Normalize();
			vEdge2 = vEdge2.Normalize();

			Vector3	vFaceNormal = vEdge1.Cross(vEdge2);

			m_vecFaceNormal.push_back(vFaceNormal.Normalize());

			// 좌하단 삼각형
			vecIdx.push_back(idx);
			vecIdx.push_back(idx + m_iWidth + 1);
			vecIdx.push_back(idx + m_iWidth);

			// 삼각형의 면법선을 구한다.
			vEdge1 = m_vecNowVertex[idx + m_iWidth + 1].vPos - m_vecNowVertex[idx].vPos;
			vEdge2 = m_vecNowVertex[idx + m_iWidth].vPos - m_vecNowVertex[idx].vPos;

			vEdge1 = vEdge1.Normalize();
			vEdge2 = vEdge2.Normalize();

			vFaceNormal = vEdge1.Cross(vEdge2);

			m_vecFaceNormal.push_back(vFaceNormal.Normalize());
		}
	}

	ComputeNormal(m_vecNowVertex, vecIdx);
	ComputeTangent(m_vecNowVertex, vecIdx);

	m_pMesh->UpdateVertexBuffer(&m_vecNowVertex[0]);
	/*-*/
	m_pMesh->Save(pMeshFileName, MESH_PATH);

	int Length = m_vecNowVertex.size();

	fwrite(&Length, sizeof(int), 1, pFile);
	fwrite(&m_vecNowVertex[0], sizeof(VERTEXBUMP), Length, pFile);


	if (pFile)
	{
		fclose(pFile);
		return true;
	}
	else
	{
		return false;
	}
}

bool CTerrain::LoadTerrainData(const wchar_t* pTerrainFileName, const wchar_t* pMeshFileName, const string& strPathKey)
{
	string	strFullPath;
	const char*	pBasePath = GET_SINGLE(CPathManager)->FindPathMultiByte(strPathKey);

	if (pBasePath)
		strFullPath = pBasePath;

	char strPath[512] = {};
	WideCharToMultiByte(CP_ACP, 0, pTerrainFileName, -1, strPath, lstrlenW(pTerrainFileName), 0, 0);
	strFullPath += strPath;

	FILE* pFile = NULL;
	fopen_s(&pFile, strFullPath.c_str(), "rb");

	if (!pFile)
		return false;

	m_pMaterial->Load(pFile);

	fread(&m_fDetailLevel, sizeof(float), 1, pFile);
	m_tCBuffer.fDetailLevel = m_fDetailLevel;

	fread(&m_iSplatCount, sizeof(int), 1, pFile);
	m_tCBuffer.iSplatCount = m_iSplatCount;
	m_tCBuffer.fRadius = 0.f;
	m_tCBuffer.vMousePosition = Vector2::Zero;

	fread(&m_bNaviMesh, sizeof(bool), 1, pFile);
	fread(&m_iWidth, sizeof(int), 1, pFile);
	fread(&m_iHeight, sizeof(int), 1, pFile);

	int indexCount = 0;
	fread(&indexCount, sizeof(int), 1, pFile);

	if (!m_vecNowVertex.empty())
		m_vecNowVertex.clear();

	m_vecNowVertex.resize(indexCount);
	fread(&m_vecNowVertex[0], sizeof(VERTEXBUMP), indexCount, pFile);

	if (m_pMesh)
		SAFE_RELEASE(m_pMesh)

	m_pMesh = GET_SINGLE(CResourcesManager)->LoadSSSMesh("", pMeshFileName);

	if (m_bNaviMesh)
	{
		CNavigationMesh*	pNaviMesh = GET_SINGLE(CNavigationManager)->CreateNavigationMesh("Terrain");
		CTransform* pTr = m_pGameObject->GetTransform();
		m_pMesh->CreateNaviMeshGridInfo(pNaviMesh, pTr->GetWorldScale(), pTr->GetWorldPosition(), 128);
		SAFE_RELEASE(pTr);
	}

	m_pRenderer->SetMesh(m_pMesh);
	m_pRenderer->SetShader(TERRAIN_SHADER);
	m_pRenderer->SetInputLayout(BUMP_LAYOUT);

	m_pMaterial->SetSamplerMultiTextureRegister("Splat", 10, SAMPLER_LINEAR);

	if (pFile)
	{
		fclose(pFile);
		return true;
	}
	else
	{
		return false;
	}
}

void CTerrain::SetBaseDiffuseTexture(const string & strKey, const wchar_t * pFileName, const string & strPathKey)
{
	m_pMaterial->SetDiffuseTexture(strKey, pFileName, 0, 0, strPathKey);
	//m_pMaterial->SetDiffuseTextureRegister(0);
}

void CTerrain::SetBaseNormalTexture(const string & strKey, const wchar_t * pFileName, const string & strPathKey)
{
	m_pMaterial->SetNormalTexture(strKey, pFileName, 0, 0, strPathKey);
	//m_pMaterial->SetNormalTextureRegister(1);
}

void CTerrain::SetBaseSpecularTexture(const string & strKey, const wchar_t * pFileName, const string & strPathKey)
{
	m_pMaterial->SetSpecularTexture(strKey, pFileName, 0, 0, strPathKey);
	//m_pMaterial->SetSpecularTextureRegister(2);
}

void CTerrain::AddSplatDiffuseTexture(const string & strMultiTexKey, const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset, const string & strPathKey)
{
	m_iSplatCount = vecFileName.size();
	m_tCBuffer.iSplatCount = vecFileName.size();

	m_pMaterial->SetDiffuseMultiTexture(strMultiTexKey, strKey, vecFileName, iContainer, iSubset);
	m_pMaterial->SetDiffuseMultiTextureRegister(strMultiTexKey, 6);
}

void CTerrain::AddSplatNormalTexture(const string & strMultiTexKey, const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset, const string & strPathKey)
{
	m_pMaterial->SetNormalMultiTexture(strMultiTexKey, strKey, vecFileName, iContainer, iSubset);
	m_pMaterial->SetNormalMultiTextureRegister(strMultiTexKey, 7);
}

void CTerrain::AddSplatSpecularTexture(const string & strMultiTexKey, const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset, const string & strPathKey)
{
	m_pMaterial->SetSpecularMultiTexture(strMultiTexKey, strKey, vecFileName, iContainer, iSubset);
	m_pMaterial->SetSpecularMultiTextureRegister(strMultiTexKey, 8);
}

void CTerrain::AddSplatAlphaTexture(const string & strMultiTexKey, const string & strKey, const vector<wstring>& vecFileName, int iContainer, int iSubset, const string & strPathKey)
{
	m_pMaterial->SetAlphaMultiTexture(strMultiTexKey, strKey, vecFileName, iContainer, iSubset);
	m_pMaterial->SetAlphaMultiTextureRegister(strMultiTexKey, 9);
}

void CTerrain::SetSampler()
{
	m_pMaterial->SetSamplerMultiTextureRegister("Splat", 10, SAMPLER_LINEAR);
}

bool CTerrain::LoadHeightMap(const string & strKey, float fDetailLevel, const char * pFileName, const string & strPathKey)//const wchar_t * pFileName)
{
	if (!m_pRenderer)
		return false;

	m_fDetailLevel = fDetailLevel;
	m_tCBuffer.fDetailLevel = fDetailLevel;

	string strFileName = pFileName;
	wstring wstrFileName(strFileName.begin(), strFileName.end());
	CTexture* pTexture = GET_SINGLE(CResourcesManager)->LoadTexture(strKey, wstrFileName.c_str(), strPathKey);

	if (pTexture)
	{
		m_iWidth = pTexture->GetImage(0)->GetMetadata().width;
		m_iHeight = pTexture->GetImage(0)->GetMetadata().height;

		uint32_t iStride = pTexture->GetImage(0)->GetPixelsSize() / (m_iWidth * m_iHeight);

		m_vecPos.clear();
		// 정점정보를 생성한다.
		vector<VERTEXBUMP>	vecVtx;
		vector<unsigned int>	vecIdx;
		float fHorizontalCenter = m_iWidth / 2.0f;
		float fVerticalCenter = m_iHeight / 2.0f; 

		uint8_t* pPixelOrigin = pTexture->GetImage(0)->GetPixels();

		for (unsigned int y = 0; y < m_iHeight; ++y)
		{
			for (unsigned int x = 0; x < m_iWidth; ++x)
			{
				VERTEXBUMP	tVtx = {};

				uint8_t* pOffset = pPixelOrigin + iStride * (y * m_iHeight + x);
				uint8_t iValue = *pOffset;

				float fHeightRate = iValue * (1.0f / 255.0f);

				tVtx.vPos = Vector3(x - fHorizontalCenter, fHeightRate * (m_fMaxHeight - m_fMinHeight), m_iHeight - 1 - y - fVerticalCenter);
				tVtx.vPos.y += m_fMinHeight;
				m_vecPos.push_back(tVtx.vPos);

				tVtx.vUV = Vector2(x / (float)(m_iWidth - 1), y / (float)(m_iHeight - 1));
				vecVtx.push_back(tVtx);
			}
		}
		SAFE_RELEASE(pTexture);

		if (!m_vecFaceNormal.empty())
			m_vecFaceNormal.clear();

		// 인덱스를 구해준다.
		for (unsigned int i = 0; i < m_iHeight - 1; ++i)
		{
			for (unsigned int j = 0; j < m_iWidth - 1; ++j)
			{
				int idx = i * m_iWidth + j;

				// 우상단 삼각형
				vecIdx.push_back(idx);
				vecIdx.push_back(idx + 1);
				vecIdx.push_back(idx + m_iWidth + 1);

				// 삼각형의 면법선을 구한다.
				Vector3	vEdge1 = m_vecPos[idx + 1] - m_vecPos[idx];
				Vector3	vEdge2 = m_vecPos[idx + m_iWidth + 1] - m_vecPos[idx];

				vEdge1 = vEdge1.Normalize();
				vEdge2 = vEdge2.Normalize();

				Vector3	vFaceNormal = vEdge1.Cross(vEdge2);

				m_vecFaceNormal.push_back(vFaceNormal.Normalize());

				// 좌하단 삼각형
				vecIdx.push_back(idx);
				vecIdx.push_back(idx + m_iWidth + 1);
				vecIdx.push_back(idx + m_iWidth);

				// 삼각형의 면법선을 구한다.
				vEdge1 = m_vecPos[idx + m_iWidth + 1] - m_vecPos[idx];
				vEdge2 = m_vecPos[idx + m_iWidth] - m_vecPos[idx];

				vEdge1 = vEdge1.Normalize();
				vEdge2 = vEdge2.Normalize();

				vFaceNormal = vEdge1.Cross(vEdge2);

				m_vecFaceNormal.push_back(vFaceNormal.Normalize());
			}
		}

		ComputeNormal(vecVtx, vecIdx);
		ComputeTangent(vecVtx, vecIdx);

		// 만들어준 정보를 이용해서 메쉬를 만든다.

		if (m_pMesh)
		{
			SAFE_RELEASE(m_pMesh);
			//터레인생성이 처음이 아니라면 똑같은 이름에 높이값이 다른 메쉬가 등록되어있을 수 있다. 
			//기존의 이름으로 등록되어있는 메쉬가 있을 경우 지워준다.
		}

		//if(!m_vecNowVertex.empty())
		m_vecNowVertex.clear();
		m_vecNowVertex = vecVtx;

		m_pMesh = GET_SINGLE(CResourcesManager)->CreateMesh(strKey, TERRAIN_SHADER,
			BUMP_LAYOUT, vecVtx.size(), sizeof(VERTEXBUMP), D3D11_USAGE_DYNAMIC,
			&vecVtx[0], D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			vecIdx.size(), sizeof(UINT), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT,
			&vecIdx[0]);

		if (m_pMesh)
		{
			m_pRenderer->SetMesh(m_pMesh);
			//SAFE_RELEASE(m_pMaterial);
			//m_pMaterial = m_pRenderer->GetMaterial();
			return true;
		}
		else
		{
			//SAFE_RELEASE(m_pMaterial);
			return false;
		}
	}
	else
	{
		return false;
	}
}

void CTerrain::SaveHeightMapForBMP(const wchar_t * pFileName)
{
	CTexture* pTest = m_pMaterial->GetAlphaMultiTexture("Splat");

	ScratchImage* pImageTest = pTest->GetImage(0);
	uint8_t* memory = pImageTest->GetPixels();
	TexMetadata Meta = pImageTest->GetMetadata();

	vector<UCHAR> vecMemory;
	vecMemory.resize(m_iHeight * m_iWidth * 4);


	for (int i = 0; i < m_iHeight * m_iWidth; ++i)
	{
		float fY = m_vecNowVertex[i].vPos.y;
		fY -= m_fMinHeight;
		fY *= 255 / (m_fMaxHeight - m_fMinHeight);
				
		fY = CMathf::Clamp(fY, 0, 255);
	
		vecMemory[i * 4] = fY;
		vecMemory[i * 4 + 1] = fY;
		vecMemory[i * 4 + 2] = fY;
		fY = 255;
		vecMemory[i * 4 + 3] = fY;

		/*vecMemory[i * 4] = static_cast<unsigned char>(fY);
		vecMemory[i * 4 + 1] = static_cast<unsigned char>(fY);
		vecMemory[i * 4 + 2] = static_cast<unsigned char>(fY);
		fY = 255;
		vecMemory[i * 4 + 3] = static_cast<unsigned char>(fY);*/

	}

	Image img;
	img.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	img.height = m_iHeight;
	img.width = m_iWidth;
	img.slicePitch = m_iHeight * m_iWidth * 4;
	img.rowPitch = m_iWidth * 4;
	img.pixels = &vecMemory[0];


	pTest->SaveBMP(img, pFileName, NO_USE_PATH);
}

void CTerrain::SaveAlphaTexture(int Count, const wstring& pFileName)
{
	CTexture* pTest = m_pMaterial->GetAlphaMultiTexture("Splat");

	ScratchImage* pImageTest = pTest->GetImage(Count);
	uint8_t* memory = pImageTest->GetPixels();
	TexMetadata Meta = pImageTest->GetMetadata();

	vector<UCHAR> vecMemory;

	vecMemory.resize(pImageTest->GetPixelsSize());
	memcpy(&vecMemory[0], memory, sizeof(UCHAR) * vecMemory.size());

	Image img;
	img.format = pImageTest->GetImage(0, 0, 0)->format;
	img.height = pImageTest->GetImage(0, 0, 0)->height;
	img.width = pImageTest->GetImage(0, 0, 0)->width;
	img.slicePitch = pImageTest->GetImage(0, 0, 0)->slicePitch;
	img.rowPitch = pImageTest->GetImage(0, 0, 0)->rowPitch;
	img.pixels = &vecMemory[0];

	char* pRelativePath = {};
	char* pPathKey = {};
	GET_SINGLE(CPathManager)->ExtractRelativeDirectoryFromFullPath(string(pFileName.begin(), pFileName.end()).c_str(), &pRelativePath, &pPathKey);

	if (pPathKey != TEXTURE_PATH)
	{
		GET_SINGLE(CDebug)->Log(L"Texture Path에다가만 저장 할 수 있습니다.");
	}
	else
	{
		string strTemp(pRelativePath);
		wstring wstrRelativePath(strTemp.begin(), strTemp.end());
		pTest->SaveBMP(img, wstrRelativePath.c_str(), TEXTURE_PATH);

		m_pMaterial->ChangeAlphaTexture(Count, "Splat", wstrRelativePath.c_str());
	}
}

void CTerrain::LoadAlphaTexture(int Count, const wstring& pFileName)
{
	m_pMaterial->SetAlphaMultiTexture("Splat", "SplatAlpha", pFileName.c_str());
}

void CTerrain::BrushEditHeight(float BrushPower, int BrushDir, float fRadius, const Vector2 & vPosition, const Vector2 & vCenterPosition)
{
	Vector2 vCenter = vPosition;
	vCenter.x *= m_iWidth;
	vCenter.y *= m_iHeight;

	int xCount = vCenter.x / 1;
	int yCount = vCenter.y / 1;
	int rCount = fRadius / m_pTransform->GetWorldScale().x;

	int yMin = max(yCount - rCount, 0);
	int yMax = min(yCount + rCount, m_iHeight);

	int xMin = max(xCount - rCount, 0);
	int xMax = min(xCount + rCount, m_iWidth);

	for (int y = yMin; y < yMax; ++y)
	{
		for (int x = xMin; x < xMax; ++x)
		{
			int index = y * m_iHeight + x;

			Vector2 Length;
			Length.x = m_vecNowVertex[index].vPos.x * m_pTransform->GetWorldScale().x;
			Length.y = m_vecNowVertex[index].vPos.z * m_pTransform->GetWorldScale().z;

			float fDist = (Length - vCenterPosition).Length();

			if (fDist <= fRadius)
			{
				if (m_fMaxHeight >= m_vecNowVertex[index].vPos.y)
				{
					float Add = GET_SINGLE(CMathf)->GaussianDistribution(0, fRadius - fDist, fRadius);

					m_vecNowVertex[index].vPos.y += Add * BrushDir * BrushPower;

					if (m_fMinHeight > m_vecNowVertex[index].vPos.y)
					{
						m_vecNowVertex[index].vPos.y = m_fMinHeight / m_pTransform->GetWorldScale().y;
					}
				}
				else if (m_fMaxHeight < m_vecNowVertex[index].vPos.y)
				{
					m_vecNowVertex[index].vPos.y = m_fMaxHeight / m_pTransform->GetWorldScale().y;;
				}
			}
		}
	}

	m_pMesh->UpdateVertexBuffer(&m_vecNowVertex[0]);

	m_pRenderer->SetMesh(m_pMesh);
}

void CTerrain::BrushEditSplat(float BrushPower, int BrushDir, float fRadius, Vector2 vPosition, Vector2 vCenterPosition, int Count)
{
	CTexture* pTest = m_pMaterial->GetAlphaMultiTexture("Splat");

	ScratchImage* pImageTest = pTest->GetImage(Count);
	uint8_t* memory = pImageTest->GetPixels();
	TexMetadata Meta = pImageTest->GetMetadata();

	vector<UCHAR> vecMemory;

	vecMemory.resize(pImageTest->GetPixelsSize());
	memcpy(&vecMemory[0], memory, sizeof(UCHAR) * vecMemory.size());

	Vector2 vCenter = vPosition;
	vCenter.x *= Meta.width;
	vCenter.y *= Meta.height;

	float NowRadius = fRadius * Meta.width / (m_iWidth * m_pTransform->GetWorldScale().x);

	int xCount = vCenter.x / 1;
	int yCount = vCenter.y / 1;
	int rCount = NowRadius / 1;

	int yMin = max(yCount - rCount, 0);
	int yMax = min(yCount + rCount, Meta.height);

	int xMin = max(xCount - rCount, 0);
	int xMax = min(xCount + rCount, Meta.width);


	Vector2 Length;

	for (int y = yMin; y < yMax; ++y)
	{
		Length.y = y;

		for (int x = xMin; x < xMax; ++x)
		{
			int index = y * Meta.height + x;
			index *= 4;

			Length.x = x;

			float fDist = (vCenter - Length).Length();

			if (fDist <= NowRadius)
			{
				if (BrushDir == 1)
				{
					float Add = GET_SINGLE(CMathf)->GaussianDistribution(0, NowRadius - fDist, NowRadius);
					Add *= 10.f * BrushPower;


					if (vecMemory[index] + Add <= 255.f)
					{
						vecMemory[index] += Add;
						vecMemory[index + 1] += Add;
						vecMemory[index + 2] += Add;
					}
				}
				else
				{
					vecMemory[index] = 0;
					vecMemory[index + 1] = 0;
					vecMemory[index + 2] = 0;
				}
			}
		}
	}

	Image img;
	img.format = pImageTest->GetImage(0, 0, 0)->format;
	img.height = pImageTest->GetImage(0, 0, 0)->height;
	img.width = pImageTest->GetImage(0, 0, 0)->width;
	img.slicePitch = pImageTest->GetImage(0, 0, 0)->slicePitch;
	img.rowPitch = pImageTest->GetImage(0, 0, 0)->rowPitch;
	img.pixels = &vecMemory[0];

	pTest->LoadTexture(&img, Count);

	//pTest->SavePNG(img, L"Alpha.PNG", TEXTURE_PATH);
}

Vector3 CTerrain::GetClickPosition(Vector2 & vCenterPosition)
{
	Vector2 vCenter = vCenterPosition;
	vCenter.x *= m_iWidth;
	vCenter.y *= m_iHeight;

	int xCount = vCenter.x / 1;
	int yCount = vCenter.y / 1;

	int index = yCount * m_iHeight + xCount;

	return m_vecNowVertex[index].vPos;
}

Vector2 CTerrain::GetUVFromWorldPosition(const Vector3 & vPosition)
{
	Vector3 vObjectPosition = m_pTransform->GetWorldPosition();
	Vector3 vLocalPositionFromInput = vObjectPosition + Vector3(m_iWidth * m_pTransform->GetWorldScale().x * 0.5f, 0.0f, m_iHeight * m_pTransform->GetWorldScale().z * 0.5f) - vPosition;

	vLocalPositionFromInput.x /= m_iWidth * m_pTransform->GetWorldScale().x;
	vLocalPositionFromInput.z /= m_iHeight * m_pTransform->GetWorldScale().z;

	vLocalPositionFromInput.x = 1 - vLocalPositionFromInput.x;

	return Vector2(vLocalPositionFromInput.x, vLocalPositionFromInput.z);
}

float CTerrain::GetMaxHeight() const
{
	return m_fMaxHeight;
}

void CTerrain::SetMaxHeight(float fHeight)
{
	m_fMaxHeight = fHeight;
}

float CTerrain::GetMinHeight() const
{
	return m_fMinHeight;
}

void CTerrain::SetMinHeight(float fHeight)
{
	m_fMinHeight = fHeight;
}

void CTerrain::SetSplatCount(int iCount)
{
	m_iSplatCount = iCount;
	m_tCBuffer.iSplatCount = iCount;
}

int CTerrain::GetSplatCount() const
{
	return m_iSplatCount;
}

void CTerrain::SetDetailLevel(int DetailLevel)
{
	m_fDetailLevel = DetailLevel;
	m_tCBuffer.fDetailLevel = DetailLevel;
}

int CTerrain::GetDetailLevel() const
{
	return m_fDetailLevel;
}

void CTerrain::SetNavigationMesh(bool bNavi)
{
	m_bNaviMesh = bNavi;
}

bool CTerrain::GetNavigationMesh() const
{
	return m_bNaviMesh;
}

void CTerrain::ComputeNormal(vector<VERTEXBUMP>& vecVtx, const vector<UINT>& vecIdx)
{
	for (size_t i = 0; i < m_vecFaceNormal.size(); ++i)
	{
		int	idx0 = vecIdx[i * 3];
		int	idx1 = vecIdx[i * 3 + 1];
		int	idx2 = vecIdx[i * 3 + 2];

		vecVtx[idx0].vNormal += m_vecFaceNormal[i];
		vecVtx[idx1].vNormal += m_vecFaceNormal[i];
		vecVtx[idx2].vNormal += m_vecFaceNormal[i];
	}

	for (size_t i = 0; i < vecVtx.size(); ++i)
	{
		vecVtx[i].vNormal = vecVtx[i].vNormal.Normalize();
	}
}

void CTerrain::ComputeTangent(vector<VERTEXBUMP>& vecVtx, const vector<UINT>& vecIdx)
{
	// 탄젠트 벡터 구함.
	for (size_t i = 0; i < m_vecFaceNormal.size(); ++i)
	{
		int	idx0 = vecIdx[i * 3];
		int	idx1 = vecIdx[i * 3 + 1];
		int	idx2 = vecIdx[i * 3 + 2];

		float	fVtx1[3], fVtx2[3];
		fVtx1[0] = vecVtx[idx1].vPos.x - vecVtx[idx0].vPos.x;
		fVtx1[1] = vecVtx[idx1].vPos.y - vecVtx[idx0].vPos.y;
		fVtx1[2] = vecVtx[idx1].vPos.z - vecVtx[idx0].vPos.z;

		fVtx2[0] = vecVtx[idx2].vPos.x - vecVtx[idx0].vPos.x;
		fVtx2[1] = vecVtx[idx2].vPos.y - vecVtx[idx0].vPos.y;
		fVtx2[2] = vecVtx[idx2].vPos.z - vecVtx[idx0].vPos.z;

		float	ftu[2], ftv[2];
		ftu[0] = vecVtx[idx1].vUV.x - vecVtx[idx0].vUV.x;
		ftv[0] = vecVtx[idx1].vUV.y - vecVtx[idx0].vUV.y;

		ftu[1] = vecVtx[idx2].vUV.x - vecVtx[idx0].vUV.x;
		ftv[1] = vecVtx[idx2].vUV.y - vecVtx[idx0].vUV.y;

		float	fDen = 1.f / (ftu[0] * ftv[1] - ftu[1] * ftv[0]);

		Vector3	vTangent;
		vTangent.x = (ftv[1] * fVtx1[0] - ftv[0] * fVtx2[0]) * fDen;
		vTangent.y = (ftv[1] * fVtx1[1] - ftv[0] * fVtx2[1]) * fDen;
		vTangent.z = (ftv[1] * fVtx1[2] - ftv[0] * fVtx2[2]) * fDen;

		vTangent = vTangent.Normalize();

		vecVtx[idx0].vTangent = vTangent;
		vecVtx[idx1].vTangent = vTangent;
		vecVtx[idx2].vTangent = vTangent;

		vecVtx[idx0].vBinormal = vecVtx[idx0].vNormal.Cross(vTangent).Normalize();
		vecVtx[idx1].vBinormal = vecVtx[idx1].vNormal.Cross(vTangent).Normalize();
		vecVtx[idx2].vBinormal = vecVtx[idx2].vNormal.Cross(vTangent).Normalize();
	}
}

void CTerrain::Start()
{
}

int CTerrain::Prerender(CMeshRenderer * pRenderer)
{
	GET_SINGLE(CShaderManager)->CreateConstantBuffer("Terrain", sizeof(TERRAINCBUFFER), CBT_VERTEX | CBT_PIXEL);
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer("Terrain", &m_tCBuffer, CBT_VERTEX | CBT_PIXEL);
	return 0;
}

bool CTerrain::Initialize()
{
	CMeshRenderer* pRenderer = m_pGameObject->GetComponent<CMeshRenderer>();

	if (!pRenderer)
	{
		m_pRenderer = m_pGameObject->AddComponent<CMeshRenderer>();
	}
	else
	{
		m_pRenderer = pRenderer;
	}

	CMaterial* pMaterial = m_pRenderer->GetMaterial();

	if (pMaterial)
	{
		m_pMaterial = pMaterial;
	}
	else
	{
		pMaterial = new CMaterial;
		m_pRenderer->SetMaterial(pMaterial);
		m_pMaterial = pMaterial;
		SAFE_RELEASE(pMaterial);
	}

	return true;
}

bool CTerrain::Save(FILE * pFile)
{
	if (!pFile)
		return false;

	fwrite(&m_fDetailLevel, sizeof(float), 1, pFile);
	fwrite(&m_iSplatCount, sizeof(int), 1, pFile);
	fwrite(&m_bNaviMesh, sizeof(bool), 1, pFile);
	fwrite(&m_iWidth, sizeof(int), 1, pFile);
	fwrite(&m_iHeight, sizeof(int), 1, pFile);
	fwrite(&m_fMaxHeight, sizeof(float), 1, pFile);
	fwrite(&m_fMinHeight, sizeof(float), 1, pFile);

	int Length = m_vecNowVertex.size();
	fwrite(&Length, sizeof(int), 1, pFile);
	fwrite(&m_vecNowVertex[0], sizeof(VERTEXBUMP), Length, pFile);

	return true;
}

bool CTerrain::Load(FILE * pFile)
{
	if (!pFile)
		return false;

	fread(&m_fDetailLevel, sizeof(float), 1, pFile);
	m_tCBuffer.fDetailLevel = m_fDetailLevel;

	fread(&m_iSplatCount, sizeof(int), 1, pFile);
	m_tCBuffer.iSplatCount = m_iSplatCount;
	m_tCBuffer.fRadius = 0.f;
	m_tCBuffer.vMousePosition = Vector2::Zero;
	fread(&m_bNaviMesh, sizeof(bool), 1, pFile);
	fread(&m_iWidth, sizeof(int), 1, pFile);
	fread(&m_iHeight, sizeof(int), 1, pFile);
	fread(&m_fMaxHeight, sizeof(float), 1, pFile);
	fread(&m_fMinHeight, sizeof(float), 1, pFile);

	int Length = m_vecNowVertex.size();
	fread(&Length, sizeof(int), 1, pFile);

	if (!m_vecNowVertex.empty())
		m_vecNowVertex.clear();

	m_vecNowVertex.resize(Length);
	fread(&m_vecNowVertex[0], sizeof(VERTEXBUMP), Length, pFile);

	if (m_pMesh)
		SAFE_RELEASE(m_pMesh);

	m_pMesh = m_pRenderer->GetMesh();
	m_pMesh->AddRef();

	if (m_bNaviMesh)
	{
		CNavigationMesh*	pNaviMesh = GET_SINGLE(CNavigationManager)->CreateNavigationMesh("Terrain");
		CTransform* pTr = m_pGameObject->GetTransform();
		m_pMesh->CreateNaviMeshGridInfo(pNaviMesh, pTr->GetWorldScale(), pTr->GetWorldPosition(), 128);
		SAFE_RELEASE(pTr);
	}

	return true;
}

bool CTerrain::UpdateNormal()
{
	if (!m_vecFaceNormal.empty())
		m_vecFaceNormal.clear();

	// 정점정보를 생성한다.
	vector<unsigned int>	vecIdx;

	// 인덱스를 구해준다.
	for (unsigned int i = 0; i < m_iHeight - 1; ++i)
	{
		for (unsigned int j = 0; j < m_iWidth - 1; ++j)
		{
			int	idx = i * m_iWidth + j;

			// 우상단 삼각형
			vecIdx.push_back(idx);
			vecIdx.push_back(idx + 1);
			vecIdx.push_back(idx + m_iWidth + 1);

			// 삼각형의 면법선을 구한다.
			Vector3	vEdge1 = m_vecNowVertex[idx + 1].vPos - m_vecNowVertex[idx].vPos;
			Vector3	vEdge2 = m_vecNowVertex[idx + m_iWidth + 1].vPos - m_vecNowVertex[idx].vPos;

			vEdge1 = vEdge1.Normalize();
			vEdge2 = vEdge2.Normalize();

			Vector3	vFaceNormal = vEdge1.Cross(vEdge2);

			m_vecFaceNormal.push_back(vFaceNormal.Normalize());

			// 좌하단 삼각형
			vecIdx.push_back(idx);
			vecIdx.push_back(idx + m_iWidth + 1);
			vecIdx.push_back(idx + m_iWidth);

			// 삼각형의 면법선을 구한다.
			vEdge1 = m_vecNowVertex[idx + m_iWidth + 1].vPos - m_vecNowVertex[idx].vPos;
			vEdge2 = m_vecNowVertex[idx + m_iWidth].vPos - m_vecNowVertex[idx].vPos;

			vEdge1 = vEdge1.Normalize();
			vEdge2 = vEdge2.Normalize();

			vFaceNormal = vEdge1.Cross(vEdge2);

			m_vecFaceNormal.push_back(vFaceNormal.Normalize());
		}
	}

	ComputeNormal(m_vecNowVertex, vecIdx);
	ComputeTangent(m_vecNowVertex, vecIdx);

	m_pMesh->UpdateVertexBuffer(&m_vecNowVertex[0]);

	return true;
}

