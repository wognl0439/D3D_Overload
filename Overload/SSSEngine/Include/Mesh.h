#pragma once
#include "Ref.h"
#include "FBXLoader.h"

SSS_BEGIN
typedef struct SSS_DLL _tagKeyFrame
{
	double	dTime;
	Vector3	vPos;
	Vector3	vScale;
	Vector4	vRot;
}KEYFRAME, *PKEYFRAME;

typedef	struct SSS_DLL _tagKeyFrameBuffer
{
	bool bEmpty;
	int iBoneIndex;
	PKEYFRAME pKeyFrame;

	_tagKeyFrameBuffer() :
		bEmpty(true),
		iBoneIndex(-1),
		pKeyFrame(NULL)
	{}
}KEYFRAMEBUFFER, *PKEYFRAMEBUFFER;


typedef	struct SSS_DLL _tagKeyFrameBuffer2D
{
	bool bEmpty;
	class CTexture* pTexture;
	PATLASDATA pAtlasData;

	_tagKeyFrameBuffer2D() :
		bEmpty(true),
		pAtlasData(NULL),
		pTexture(NULL)
	{}
}KEYFRAMEBUFFER2D, *PKEYFRAMEBUFFER2D;

typedef struct SSS_DLL _tagBoneKeyFrame
{
	int		iBoneIndex;
	vector<PKEYFRAME>	vecKeyFrame;
}BONEKEYFRAME, *PBONEKEYFRAME;

typedef struct SSS_DLL _tagBone
{
	string		strName;
	int			iDepth;
	int			iParentIndex;
	PMatrix		matOffset;
	PMatrix		matBone;

	_tagBone()
	{}

}BONE, *PBONE;

typedef struct SSS_DLL _tagVertexBuffer
{
	unsigned int	iCount;
	unsigned int	iSize;
	D3D11_USAGE		eUsage;
	// 도형 위상구조. 정점버퍼에 있는 정점을 어떤 도형으로 그려낼지
	// 결정하는 기준이 된다.
	D3D11_PRIMITIVE_TOPOLOGY	ePrimitiveType;
	void*			pData;
	ID3D11Buffer*	pVB;
}VERTEXBUFFER, *PVERTEXBUFFER;

typedef struct SSS_DLL _tagIndexBuffer
{
	unsigned int		iCount;
	unsigned int		iSize;
	D3D11_USAGE	eUsage;
	DXGI_FORMAT	eFormat;
	void*					pData;
	ID3D11Buffer*	pIB;
}INDEXBUFFER, *PINDEXBUFFER;

//각각의 컨테이너는 여러개의 서브셋을 가질 수 있다.
typedef struct SSS_DLL _tagMeshContainer
{
	bool bEnable = true;
	VERTEXBUFFER	tVB;
	vector<INDEXBUFFER> vecIB;
	Matrix matLocal;
	string strContainerName;
}MESHCONTAINER, *PMESHCONTAINER;

typedef struct SSS_DLL _tagAnimationClip
{
	string		strName;
	size_t		iStartFrame;
	size_t		iEndFrame;
	size_t		iLength;

	_tagAnimationClip() :
		strName(""),
		iStartFrame(0),
		iEndFrame(0)
	{
	}
}ANIMATIONCLIP, *PANIMATIONCLIP;

class SSS_DLL CMesh :
	public CRef
{
private:
	friend class CDebug;
	friend class CResourcesManager;
	friend class CMeshCollider;

private:
	vector<PMESHCONTAINER> m_vecMeshContainer;
	vector<PBONE> m_vecBone;
	vector<PBONEKEYFRAME> m_vecAnimationKeyFrame;
	vector<PANIMATIONCLIP> m_vecAnimationClipInfo;

	Vector3					m_vMin;
	Vector3					m_vMax;
	Vector3					m_vCenter;
	Vector3					m_vLength;
	float						m_fRadius;
	string					m_strShaderKey;
	string					m_strLayoutKey;
	string					m_strKey;
	string					m_strPathKey;
	string					m_strDirectory;
	class CMaterial*	m_pMaterial;


private:
	CMesh();
	~CMesh();

public:
	void SetEnableMeshContainer(int iContainer, bool bEnable);
	bool GetEnableMeshContainer(int iContainer) const;
	bool ChangeMeshContainer(const string& strMesh, int iContainer);
	bool ChangeMeshContainer(CMesh* pMesh, int iContainer);
	//해당 인덱스의 뒤쪽에 삽입됨
	bool InsertMeshContainer(const string& strMesh, int iContainer);
	//해당 인덱스의 뒤쪽에 삽입됨
	bool InsertMeshContainer(CMesh* pMesh, int iContainer);
	bool RemoveMeshContainer(int iContainer = 0);
	
	Matrix* GetLocalMatrix(int iContainer = 0) const;
	bool HasAnyKeyFrame() const;
	void AddAnimationClip(const string& strClipName, size_t iStartFrame, size_t iEndFrame);
	void RemoveAnimationClip(const string& strClipName);
	void RemoveAnimationClip(size_t iIndex);
	PANIMATIONCLIP GetAnimationClipInfo(const string& strClipName);
	PANIMATIONCLIP GetAnimationClipInfo(size_t iIndex);
	vector<PANIMATIONCLIP>* GetAnimationClipInfo();	 
	const vector<PBONEKEYFRAME>* GetAnimationClip() const;
	size_t GetAnimationLength();
	const vector<PBONE>* GetBone() const;
	size_t GetBoneCount() const;
	void SetClipInfo(PFBXANIMATIONCLIP pClip);
	void SetClipInfo(PBONEKEYFRAME pClip);
	void SetClipInfo(wchar_t* pFileName, const string& strPathKey);

	float GetRadius() const;
	Vector3 GetMin() const;
	Vector3 GetMax() const;
	Vector3 GetLength() const;
	Vector3 GetCenter() const;
	const string& GetKey() const;
	const string& GetDirectory() const;
	const string& GetPathKey() const;
	const string& GetShaderKey()	const;
	const string& GetLayoutKey()	const;

	size_t GetContainerCount() const;
	size_t GetSubsetCount(size_t iContainerIndex) const;
	size_t GetVertexCount() const;
	size_t GetVertexCount(size_t iContainer) const;

	void SetMaterial(class CMaterial* pMaterial);
	class CMaterial* GetMaterial() const;

	void GetMeshPosition(vector<Vector3>* vecPos, int iContainer = 0);
	void GetMeshIndex(vector<int>* vecIndex, int iContainer = 0, int iSubset = 0);
	void CreateNaviMeshGridInfo(class CNavigationMesh* pNaviMesh, const Vector3& vScale,
		const Vector3& vLandPos, int iRectCount);
	void CreateNaviMeshInfo(class CNavigationMesh* pNaviMesh, const Vector3& vScale,
		const Vector3& vLandPos);

public:	
	bool CreateMesh(const string& strKey, const string& strShaderKey, const string& strInputLayoutKey, UINT iVtxCount,
		UINT iVtxSize, D3D11_USAGE eVtxUsage, void* pVtxData,
		D3D11_PRIMITIVE_TOPOLOGY ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		UINT iIdxCount = 0, UINT iIdxSize = 0,
		D3D11_USAGE eIdxUsage = D3D11_USAGE_DEFAULT,
		DXGI_FORMAT eIdxFmt = DXGI_FORMAT_R32_UINT,
		void* pIdxData = NULL);

	//출력
	void Render(int iContainer, int iSubset);
	void Render();
	void UpdateVertexBuffer(void* pVtxData, size_t iSize = 0);
	void MoveVertexBuffer(const Vector3& vMove);

	//Full Path 사용할 경우 PathKey 에 NO_USE_PATH 사용할 것
	void Save(const wchar_t* pFileName, const string& strPathKey);
	CMesh* Load(const string& strKey, const wchar_t* pFileName, const string& strPathKey);


private:
	//Create Vertex Buffer
	bool CreateVertexBuffer(UINT iVtxCount, UINT iVtxSize, D3D11_USAGE eVtxUsage, void* pVtxData, D3D11_PRIMITIVE_TOPOLOGY ePrimitiveType, PMESHCONTAINER pContainer);
	//Create Index Buffer
	bool CreateIndexBuffer(UINT iIdxCount, UINT iIdxSize, D3D11_USAGE eIdxUsage, DXGI_FORMAT eIdxFmt, void* pIdxData, PMESHCONTAINER pContainer);
	
	// Create Sphere Mesh
	bool CreateSpherePos(const string& strKey, float fRadius,
		unsigned int iSubDivision);
	void Subdivide(vector<VERTEXTEX>& vecVertices,
		vector<DWORD>& vecIndices);
	float AngleFromXY(float x, float y);
};

SSS_END
