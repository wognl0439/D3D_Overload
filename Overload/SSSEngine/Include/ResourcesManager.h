#pragma once

#include "Engine.h"

SSS_BEGIN

class SSS_DLL CResourcesManager
{
	DECLARE_SINGLE(CResourcesManager)


private:
	unordered_map<string, class CMesh*>		m_mapMesh;
	unordered_map<string, class CTexture*>		m_mapTexture;
	unordered_map<string, class CSampler*>	m_mapSampler;

private:
	bool CreateCone(const string & strKey, const string & strShaderKey, const string & strInputLayout, UINT iSides, float fAngle, const Vector3& vPivotOffset = Vector3::Zero);
	class CMesh* CreateSpherePos(const string& strKey, float fRadius,
		unsigned int iSubDivision);
	void SetMeshDirectoryDescriptions(class CMesh* pTargetMesh, const string& strKey, const string& strFullPath);
public:
	bool Initialize();

	class CMesh* CreateMesh(const string& strKey, 
		const string& strShaderKey,
		const string& strInputLayoutKey,
		UINT iVtxCount,		
		UINT iVtxSize, D3D11_USAGE eVtxUsage, void* pVtxData,
		D3D11_PRIMITIVE_TOPOLOGY ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		UINT iIdxCount = 0, UINT iIdxSize = 0,
		D3D11_USAGE eIdxUsage = D3D11_USAGE_DEFAULT,
		DXGI_FORMAT eIdxFmt = DXGI_FORMAT_R32_UINT,
		void* pIdxData = NULL);

	class CMesh* LoadMesh(const string& strKey, const wchar_t* pFileName, const string& strPathKey = MESH_PATH, bool bUseTexture = false) ;
	class CMesh* LoadFBX(const string& strKey, const wchar_t* pFileName, const string& strPathKey = MESH_PATH);
	class CMesh* LoadOBJ(bool bUseTexture, const string& strKey, const wchar_t* pFileName, const string& strPathKey = MESH_PATH);
	class CMesh* LoadSSSMesh(const string& strKey, const wchar_t* pFileName, const string& strPathKey = MESH_PATH);
	class CMesh* ConvertFbx(const string& strKey, class CFbxLoader* pLoader);

	class CMesh* FindMesh(const string& strKey);

	class CTexture* LoadTexture(const string& strKey, const wchar_t* pFileName, 	const string& strPathKey = TEXTURE_PATH);
	class CTexture* LoadTexture(const string& strKey, const vector<wstring>& vecFileName, const string& strPathKey = TEXTURE_PATH);
	class CTexture* LoadTextureFromFullPath(const string& strKey, const wchar_t* pFullPath);
	class CTexture* LoadTextureFromFullPath(const string& strKey, const vector<wstring>& vecFullPath);

	class CTexture* FindTexture(const string& strKey);

	class CSampler* CreateSampler(const string& strKey,
		D3D11_FILTER eFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_MODE eU = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE eV = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE eW = D3D11_TEXTURE_ADDRESS_WRAP,
		float fMipLODBias = 0.f,
		UINT iMaxAnisotropy = 1,
		D3D11_COMPARISON_FUNC eFunc = D3D11_COMPARISON_NEVER,
		const Vector4& vBorderColor = Vector4(0.f, 0.f, 0.f, 0.f),
		float fMinLOD = -FLT_MAX,
		float fMaxLOD = FLT_MAX);
	class CSampler* FindSampler(const string& strKey);


};

SSS_END	
