#include "Tile.h"
#include "../ShaderManager.h"
#include "../RenderManager.h"
#include "../RenderState.h"
#include "../Shader.h"
#include "../ResourcesManager.h"
#include "../Mesh.h"
#include "../Scene.h"
#include "Camera.h"
#include "../Device.h"

SSS_USING

CTile::CTile()
{
	m_eType = TT_RC;
	m_eTileFlag = TF_NORMAL;
	SetTag("Tile");

	m_fG = -1.f;
	m_fH = -1.f;
	m_fTotal = -1.f;
	m_iParentIndex = -1;
	m_iIndex = -1;
	m_iIndexX = -1;
	m_iIndexY = -1;
	m_eAStarType = TAT_NONE;

	m_pShader = GET_SINGLE(CShaderManager)->FindShader(COLLIDER_SHADER);
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(ENGINE_MESH_FRAME_ISO_COLOR);
	m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_COLOR_LAYOUT);

	if (GET_SINGLE(CRenderManager)->GetRenderMode() == RM_2D)
	{
		m_pDepthDisable = GET_SINGLE(CRenderManager)->FindRenderState(DEPTH_DISABLE);
	}

	m_vDebugColor = Vector4::green;
}


CTile::~CTile()
{
	SAFE_RELEASE(m_pDepthDisable);
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pMesh);
}

TILE_ASTARLIST_TYPE CTile::GetAStarType() const
{
	return m_eAStarType;
}

Vector3 CTile::GetPos() const
{
	return m_vPos;
}

Vector3 CTile::GetScale() const
{
	return m_vScale;
}

int CTile::GetIndexX() const
{
	return m_iIndexX;
}

int CTile::GetIndexY() const
{
	return m_iIndexY;
}

int CTile::GetIndex() const
{
	return m_iIndex;
}

int CTile::GetParentIndex() const
{
	return m_iParentIndex;
}

float CTile::GetG() const
{
	return m_fG;
}

float CTile::GetH() const
{
	return m_fH;
}

float CTile::GetTotal() const
{
	return m_fTotal;
}

TILE_FLAG CTile::GetTileFlag() const
{
	return m_eTileFlag;
}

TILE_TYPE CTile::GetTileType() const
{
	return m_eType;
}

void CTile::SetAStarType(TILE_ASTARLIST_TYPE eType)
{
	m_eAStarType = eType;
}

void CTile::SetIndexX(int iIndexX)
{
	m_iIndexX = iIndexX;
}

void CTile::SetIndexY(int iIndexY)
{
	m_iIndexY = iIndexY;
}

void CTile::SetIndex(int iIndex)
{
	m_iIndex = iIndex;
}

void CTile::SetParentIndex(int iParentIndex)
{
	m_iParentIndex = iParentIndex;
}

void CTile::SetG(float fG)
{
	m_fG = fG;
}

void CTile::SetH(float fH)
{
	m_fH = fH;
}

void CTile::SetTotal(float fTotal)
{
	m_fTotal = fTotal;
}

void CTile::SetTileFlag(TILE_FLAG eFlag)
{
	m_eTileFlag = eFlag;
}

void CTile::SetColor(const Vector4 & vColor)
{
	m_vDebugColor = vColor;
}

void CTile::SetPos(const Vector3 & vPos)
{
	m_vPos = vPos;
}

void CTile::SetScale(const Vector3 & vScale)
{
	m_vScale = vScale;
}

void CTile::ComputeTotal()
{
	m_fTotal = m_fG + m_fH;
}

bool CTile::Initialize()
{
	return true;
}

int CTile::Update(float fTime)
{
	return 0;
}

int CTile::LateUpdate(float fTime)
{
	return 0;
}

int CTile::Collision(float fTime)
{
	return 0;
}

void CTile::Render(float fTime)
{
	switch (m_eTileFlag)
	{
	case TF_NORMAL:
		m_vDebugColor = Vector4::green;
		break;
	case TF_NOMOVE:
		m_vDebugColor = Vector4::red;
		break;
	case TF_OBJECT:
		m_vDebugColor = Vector4::yellow;
		break;
	case TF_DEBUG_PATH:
		m_vDebugColor = Vector4::hotpink;
		break;
	}

	TRANSFORMCBUFFER	tBuffer = {};

	if (m_vScale.z == 0.f)
		m_vScale.z = 1.f;

	Matrix	matScale, matPos, matWorld;
	matScale = Matrix::Scaling(m_vScale);
	matPos = Matrix::Translation(m_vPos);
	matWorld = matScale * matPos;

	CCamera*	pCamera = m_pScene->GetMainCamera();

	tBuffer.matWorld = matWorld;
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProjection = pCamera->GetProjectionMatrix();
	tBuffer.matWV = matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
	tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
	tBuffer.matWP = matWorld * tBuffer.matProjection;
	tBuffer.vPivot = Vector3::Zero;
	tBuffer.vLength = m_pMesh->GetLength();

	SAFE_RELEASE(pCamera);

	tBuffer.matWorld = tBuffer.matWorld.Transpose();
	tBuffer.matView = tBuffer.matView.Transpose();
	tBuffer.matProjection = tBuffer.matProjection.Transpose();
	tBuffer.matWV = tBuffer.matWV.Transpose();
	tBuffer.matWVP = tBuffer.matWVP.Transpose();
	tBuffer.matVP = tBuffer.matVP.Transpose();
	tBuffer.matWP = tBuffer.matWP.Transpose();

	// Transform 정보를 Shader 상수버퍼로 넘겨준다.
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(TRANSFORM_CBUFFER,
		&tBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);

	COLLIDERCBUFFER	tCollCBuffer = {};

	tCollCBuffer.vColor = m_vDebugColor;

	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(COLLIDER_CBUFFER,
		&tCollCBuffer, CBT_VERTEX);

	if (m_pDepthDisable)
		m_pDepthDisable->SetState();

	DEVICE_CONTEXT->IASetInputLayout(m_pLayout);

	m_pShader->SetShader();

	m_pMesh->Render();

	if (m_pDepthDisable)
		m_pDepthDisable->ResetState();
}

bool CTile::Save(FILE * pFile)
{
	fwrite(&m_eType, sizeof(TILE_TYPE), 1, pFile);
	fwrite(&m_vPos, sizeof(Vector3), 1, pFile);
	fwrite(&m_vScale, sizeof(Vector3), 1, pFile);
	fwrite(&m_eTileFlag, sizeof(TILE_FLAG), 1, pFile);
	fwrite(&m_iIndexX, sizeof(int), 1, pFile);
	fwrite(&m_iIndexY, sizeof(int), 1, pFile);
	fwrite(&m_iIndex, sizeof(int), 1, pFile);
	return true;
}

bool CTile::Load(FILE * pFile)
{
	m_strTag = "Tile";

	fread(&m_eType, sizeof(TILE_TYPE), 1, pFile);
	fread(&m_vPos, sizeof(Vector3), 1, pFile);
	fread(&m_vScale, sizeof(Vector3), 1, pFile);
	fread(&m_eTileFlag, sizeof(TILE_FLAG), 1, pFile);
	fread(&m_iIndexX, sizeof(int), 1, pFile);
	fread(&m_iIndexY, sizeof(int), 1, pFile);
	fread(&m_iIndex, sizeof(int), 1, pFile);
	return true;
}
