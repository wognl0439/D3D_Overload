#include "Texture.h"
#include "Device.h"
#include "PathManager.h"

SSS_USING


CTexture::CTexture() :
	m_pRSView(NULL)
{
}

CTexture::~CTexture()
{
	SAFE_RELEASE(m_pRSView);
	Safe_Delete_VecList(m_vecScratchImage);
}

string CTexture::GetKey() const
{
	return m_strKey;
}

string CTexture::GetPathKey() const
{
	return m_strPathKey;
}

wstring CTexture::GetRelativePath() const
{
	return m_strRelativePath;
}

vector<wstring> CTexture::GetvecFileName() const
{
	return m_vecFileName;
}

void CTexture::SetKey(const string & strKey)
{
	m_strKey = strKey;
}

void CTexture::SetPathKey(const string & strPathKey)
{
	m_strPathKey = strPathKey;
}

void CTexture::SetRelativePath(const wstring & strPath)
{
	m_strRelativePath = strPath;
}

void CTexture::SetvecFileName(const vector<wstring>& vecFileName)
{
	m_vecFileName.clear();
	m_vecFileName = vecFileName;
}

ScratchImage * CTexture::GetImage(size_t idx) const
{
	return m_vecScratchImage[idx];
}

Vector2 CTexture::GetTextureSize(size_t idx)
{
	Vector2 vReturn;
	vReturn.x = m_vecScratchImage[idx]->GetImage(0, 0, 0)->width;
	vReturn.y = m_vecScratchImage[idx]->GetImage(0, 0, 0)->height;

	return vReturn;
}

bool CTexture::LoadTexture(const string & strKey, const wchar_t * pFileName, const string & strPathKey)
{
	wstring	strPath;

	m_strRelativePath = pFileName;

	if (strPathKey != NO_USE_PATH)
	{
		strPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);
	}

	strPath += pFileName;

	return LoadTextureFromFullPath(strKey, strPath.c_str());
}

bool CTexture::LoadTexture(const string & strKey, const vector<wstring>& vecFileName, const string & strPathKey)
{
	wstring	strPath;

	if (strPathKey != NO_USE_PATH)
	{
		strPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);
	}

	for (size_t i = 0; i < vecFileName.size(); ++i)
	{
		wstring strFullPath = strPath;
		strFullPath += vecFileName[i];
		m_strKey = strKey;

		m_vecFileName.push_back(vecFileName[i]);
		m_strPathKey = strPathKey;

		ScratchImage*	pImage = new ScratchImage;

		// 이미지 로딩을 위한 확장자 포맷을 구분한다.
		wchar_t	strExt[_MAX_EXT] = {};

		_wsplitpath_s(strFullPath.c_str(), NULL, 0, NULL, 0, NULL, 0, strExt, _MAX_EXT);

		char	strExt1[_MAX_EXT] = {};

		WideCharToMultiByte(CP_ACP, 0, strExt, -1, strExt1, lstrlenW(strExt), 0, 0);

		_strupr_s(strExt1);

		if (strcmp(strExt1, ".DDS") == 0)
		{
			if (FAILED(LoadFromDDSFile(strFullPath.c_str(), DDS_FLAGS_NONE, NULL, *pImage)))
			{
				assert(false);
				return false;
			}
		}

		else if (strcmp(strExt1, ".TGA") == 0)
		{
			if (FAILED(LoadFromTGAFile(strFullPath.c_str(), NULL, *pImage)))
			{
				assert(false);
				return false;
			}
		}

		else
		{
			if (FAILED(LoadFromWICFile(strFullPath.c_str(), WIC_FLAGS_NONE, NULL, *pImage)))
			{
				assert(false);
				return false;
			}
		}

		m_vecScratchImage.push_back(pImage);
	}

	return CreateShaderResourceArrayView();
}

bool CTexture::LoadTextureFromFullPath(const string & strKey, const wchar_t * pFullPath)
{
	ScratchImage*	pImage = new ScratchImage;

	// 이미지 로딩을 위한 확장자 포맷을 구분한다.
	wchar_t	strExt[_MAX_EXT] = {};

	_wsplitpath_s(pFullPath, NULL, 0, NULL, 0, NULL, 0, strExt, _MAX_EXT);

	char	strExt1[_MAX_EXT] = {};

	WideCharToMultiByte(CP_ACP, 0, strExt, -1, strExt1, lstrlenW(strExt), 0, 0);

	_strupr_s(strExt1);

	if (strcmp(strExt1, ".DDS") == 0)
	{
		if (FAILED(LoadFromDDSFile(pFullPath, DDS_FLAGS_NONE, NULL, *pImage)))
		{
			assert(false);
			return false;
		}
	}

	else if (strcmp(strExt1, ".TGA") == 0)
	{
		if (FAILED(LoadFromTGAFile(pFullPath, NULL, *pImage)))
		{
			assert(false);
			return false;
		}
	}

	else
	{
		if (FAILED(LoadFromWICFile(pFullPath, WIC_FLAGS_NONE, NULL, *pImage)))
		{
			assert(false);
			return false;
		}
	}



	char pFullPathA[512] = {}; 
	WideCharToMultiByte(CP_ACP, 0, pFullPath, -1, pFullPathA, lstrlenW(pFullPath), NULL, NULL);

	char* pRelativePath = NULL ;
	char* pPathKey = NULL;
	GET_SINGLE(CPathManager)->ExtractRelativeDirectoryFromFullPath(pFullPathA, &pRelativePath, &pPathKey);
	 
	string strRelativePath(pRelativePath);
	m_strKey = strKey;
	m_strPathKey = string(pPathKey);
	m_strRelativePath = wstring(strRelativePath.begin(), strRelativePath.end());


	m_vecScratchImage.push_back(pImage);

	return CreateShaderResourceView();
}

bool CTexture::LoadTextureFromFullPath(const string & strKey, const vector<wstring>& vecFileName)
{
	for (size_t i = 0; i < vecFileName.size(); ++i)
	{
		m_strKey = strKey;
		
		ScratchImage*	pImage = new ScratchImage;

		// 이미지 로딩을 위한 확장자 포맷을 구분한다.
		wchar_t	strExt[_MAX_EXT] = {};

		_wsplitpath_s(vecFileName[i].c_str(), NULL, 0, NULL, 0, NULL, 0, strExt, _MAX_EXT);

		char	strExt1[_MAX_EXT] = {};

		WideCharToMultiByte(CP_ACP, 0, strExt, -1, strExt1, lstrlenW(strExt), 0, 0);

		_strupr_s(strExt1);

		if (strcmp(strExt1, ".DDS") == 0)
		{
			if (FAILED(LoadFromDDSFile(vecFileName[i].c_str(), DDS_FLAGS_NONE, NULL, *pImage)))
			{
				assert(false);
				return false;
			}
		}

		else if (strcmp(strExt1, ".TGA") == 0)
		{
			if (FAILED(LoadFromTGAFile(vecFileName[i].c_str(), NULL, *pImage)))
			{
				assert(false);
				return false;
			}
		}

		else
		{
			if (FAILED(LoadFromWICFile(vecFileName[i].c_str(), WIC_FLAGS_NONE, NULL, *pImage)))
			{
				assert(false);
				return false;
			}
		}

		m_vecScratchImage.push_back(pImage);
	}

	return CreateShaderResourceArrayView();
}

bool CTexture::CreateShaderResourceView()
{
	if (FAILED(DirectX::CreateShaderResourceView(DEVICE, m_vecScratchImage[0]->GetImages(),
		m_vecScratchImage[0]->GetImageCount(),
		m_vecScratchImage[0]->GetMetadata(),
		&m_pRSView)))
		return false;

	return true;
}

bool CTexture::CreateShaderResourceArrayView()
{
	vector<ID3D11Texture2D*>	vecTexture;

	for (size_t i = 0; i < m_vecScratchImage.size(); ++i)
	{
		ID3D11Texture2D*	pTexture = NULL;

		if (FAILED(DirectX::CreateTextureEx(DEVICE, m_vecScratchImage[i]->GetImages(),
			m_vecScratchImage[i]->GetImageCount(),
			m_vecScratchImage[i]->GetMetadata(),
			D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ, 0, FALSE,
			(ID3D11Resource**)&pTexture)))
		{
			assert(false);
			return false;
		}

		vecTexture.push_back(pTexture);
	}

	D3D11_TEXTURE2D_DESC	tTexDesc = {};
	vecTexture[0]->GetDesc(&tTexDesc);

	// Array Texture를 만들기 위한 Desc 구조체를 생성한다.
	D3D11_TEXTURE2D_DESC	tArrayDesc = {};
	tArrayDesc.Width = tTexDesc.Width;
	tArrayDesc.Height = tTexDesc.Height;
	tArrayDesc.MipLevels = tTexDesc.MipLevels;
	tArrayDesc.ArraySize = vecTexture.size();
	tArrayDesc.Format = tTexDesc.Format;
	tArrayDesc.SampleDesc.Count = 1;
	tArrayDesc.SampleDesc.Quality = 0;
	tArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	tArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// 위에서 설정한 정보를 이용해서 비어있는 Array Texture를 생성한다.
	ID3D11Texture2D*	pArrayTexture = NULL;
	if (FAILED(DEVICE->CreateTexture2D(&tArrayDesc, NULL, &pArrayTexture)))
	{
		assert(false);
		return false;
	}

	// Array Texture의 픽셀정보를 불러온 텍스쳐의 픽셀정보로 채워준다.
	for (size_t i = 0; i < vecTexture.size(); ++i)
	{
		// 각각의 텍스쳐들을 밉맵 수준만큼 반복한다.
		for (size_t mip = 0; mip < tTexDesc.MipLevels; ++mip)
		{
			// 텍스처를 Map을 걸어준다.
			DEVICE_ENTER_CRITICAL_SECTION

			D3D11_MAPPED_SUBRESOURCE	tMap = {};
			DEVICE_CONTEXT->Map(vecTexture[i], mip, D3D11_MAP_READ, 0, &tMap);

			DEVICE_CONTEXT->UpdateSubresource(pArrayTexture,
				D3D11CalcSubresource(mip, i, tTexDesc.MipLevels),
				NULL, tMap.pData, tMap.RowPitch, tMap.DepthPitch);

			DEVICE_CONTEXT->Unmap(vecTexture[i], mip);

			DEVICE_LEAVE_CRITICAL_SECTION

		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC	tViewDesc = {};
	tViewDesc.Format = tArrayDesc.Format;
	tViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	tViewDesc.Texture2DArray.MostDetailedMip = 0;
	tViewDesc.Texture2DArray.MipLevels = tArrayDesc.MipLevels;
	tViewDesc.Texture2DArray.FirstArraySlice = 0;
	tViewDesc.Texture2DArray.ArraySize = vecTexture.size();

	if (FAILED(DEVICE->CreateShaderResourceView(pArrayTexture, &tViewDesc, &m_pRSView)))
	{
		assert(false);
		return false;
	}

	SAFE_RELEASE(pArrayTexture);

	Safe_Release_VecList(vecTexture);

	return true;
}

void CTexture::SetTexture(int iRegister)
{
	DEVICE_CONTEXT->PSSetShaderResources(iRegister, 1, &m_pRSView);
}

void CTexture::SavePNG(const wchar_t * pFileName, const string & strPathKey, size_t iMipIndex)
{
	//m_vecScratchImage[iMipIndex]->GetImage(0,0,0)->pixels
	//SaveToWICFile(&m_vecScratchImage[iMipIndex], WIC_FLAGS_NONE,)
}

void CTexture::SavePNG(Image img, const wchar_t * pFileName, const string & strPathKey, size_t iMipIndex)
{
	wstring strFullPath;

	if (!strPathKey.empty() && strPathKey != NO_USE_PATH)
	{
		strFullPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);
	}

	SaveToWICFile(img, WIC_FLAGS_NONE, GetWICCodec(WIC_CODEC_PNG), strFullPath.c_str());
}

void CTexture::SaveBMP(Image img, const wchar_t * pFileName, const string & strPathKey, size_t iMipIndex)
{
	wstring strFullPath;

	if (!strPathKey.empty() && strPathKey != NO_USE_PATH)
	{
		strFullPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);
	}

	strFullPath += pFileName;

	HRESULT tResult = SaveToWICFile(img, WIC_FLAGS_NONE, GetWICCodec(WIC_CODEC_BMP), strFullPath.c_str());
}

void CTexture::LoadTexture(Image * img, int Count)
{
	SAFE_DELETE(m_vecScratchImage[Count]);
	SAFE_RELEASE(m_pRSView);
	ScratchImage* NewImg = new ScratchImage;
	NewImg->InitializeArrayFromImages(img, 1);
	m_vecScratchImage[Count] = NewImg;
	CreateShaderResourceArrayView();
}