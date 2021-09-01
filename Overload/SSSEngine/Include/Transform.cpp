#include "Transform.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Scene.h"
#include "Layer.h"
#include "Component.h"


SSS_USING

CTransform::CTransform():
	m_bUpdate(true),
	m_pParent(NULL),
	m_bDoNotUpdateInProcedural(false)
{
	// ========================= Local ========================
	m_matLocalScale = new Matrix;
	m_matLocalRotationX = new Matrix;
	m_matLocalRotationY = new Matrix;
	m_matLocalRotationZ = new Matrix;
	m_matLocalRotation = new Matrix;
	m_matLocalPosition = new Matrix;
	m_matLocal = new Matrix;
	// ========================= World ========================
	m_matWorldScale = new Matrix;
	m_matWorldRotationX = new Matrix;
	m_matWorldRotationY= new Matrix;
	m_matWorldRotationZ= new Matrix;
	m_matWorldRotation = new Matrix;
	m_matWorldPosition = new Matrix;
	m_matWorldRevolveX= new Matrix;
	m_matWorldRevolveY= new Matrix;
	m_matWorldRevolveZ= new Matrix;
	m_matWorldRevolve = new Matrix;
	m_matParent = new Matrix;
	m_matWorld = new Matrix;


}


CTransform::~CTransform()
{
	// ========================= Local ========================
	SAFE_DELETE(m_matLocalScale);
	SAFE_DELETE(m_matLocalRotationX);
	SAFE_DELETE(m_matLocalRotationY);
	SAFE_DELETE(m_matLocalRotationZ);
	SAFE_DELETE(m_matLocalRotation);
	SAFE_DELETE(m_matLocalPosition);
	SAFE_DELETE(m_matLocal);

	// ========================= World ========================
	SAFE_DELETE(m_matWorldScale);
	SAFE_DELETE(m_matWorldRotationX);
	SAFE_DELETE(m_matWorldRotationY);
	SAFE_DELETE(m_matWorldRotationZ);
	SAFE_DELETE(m_matWorldRotation);
	SAFE_DELETE(m_matWorldPosition);
	SAFE_DELETE(m_matWorldRevolveX);
	SAFE_DELETE(m_matWorldRevolveY);
	SAFE_DELETE(m_matWorldRevolveZ);
	SAFE_DELETE(m_matWorldRevolve);
	SAFE_DELETE(m_matParent);
	SAFE_DELETE(m_matWorld);

	if (m_pParent)
	{
		size_t iChildCount = m_pParent->m_vecChildren.size();

		vector<CTransform*>::iterator iter;
		vector<CTransform*>::iterator iterEnd = m_pParent->m_vecChildren.end();

		for (iter = m_pParent->m_vecChildren.begin(); iter != iterEnd; )
		{
			if ((*iter) == this)
			{
				iter = m_pParent->m_vecChildren.erase(iter);
				break;
			}
			else
			{
				++iter;
			}
		}
	}
		
	vector<CTransform*>::iterator iterChildren = m_vecChildren.begin();
	vector<CTransform*>::iterator iterChildrenEnd = m_vecChildren.end();

	for (;iterChildren != iterChildrenEnd;)
	{
		CGameObject* pChildObject = (*iterChildren)->GetGameObject();

		CLayer* pLayer = pChildObject->GetLayer();
		pLayer->RemoveGameObject(pChildObject);
		SAFE_RELEASE(pLayer);

		list<CComponent*>::const_iterator iter = pChildObject->GetAllComponents().begin();
		list<CComponent*>::const_iterator iterEnd = pChildObject->GetAllComponents().end();
		for (; iter != iterEnd;)
		{
			(*iter)->DestroyImmediate();

			pChildObject->GetAllComponents().erase(iter);

			iter = pChildObject->GetAllComponents().begin();
			iterEnd = pChildObject->GetAllComponents().end();
		}
		
		pChildObject->DestroyImmediate();

		iterChildren = m_vecChildren.begin();
		iterChildrenEnd = m_vecChildren.end();
	}
}

CTransform::CTransform(const CTransform & transform)
{
	*this = transform;

	// ========================= Local ========================
	m_matLocalScale = new Matrix;
	m_matLocalRotationX = new Matrix;
	m_matLocalRotationY = new Matrix;
	m_matLocalRotationZ = new Matrix;
	m_matLocalRotation = new Matrix;
	m_matLocalPosition = new Matrix;
	m_matLocal = new Matrix;


	*m_matLocalScale = *transform.m_matLocalScale;
	*m_matLocalRotationX = *transform.m_matLocalRotationX;
	*m_matLocalRotationY = *transform.m_matLocalRotationY;
	*m_matLocalRotationZ = *transform.m_matLocalRotationZ;
	*m_matLocalRotation = *transform.m_matLocalRotation;
	*m_matLocalPosition = *transform.m_matLocalPosition;
	*m_matLocal = *transform.m_matLocal;

	// ========================= World ========================
	m_matWorldScale = new Matrix;
	m_matWorldRotationX = new Matrix;
	m_matWorldRotationY = new Matrix;
	m_matWorldRotationZ = new Matrix;
	m_matWorldRotation = new Matrix;
	m_matWorldPosition = new Matrix;
	m_matWorldRevolveX = new Matrix;
	m_matWorldRevolveY = new Matrix;
	m_matWorldRevolveZ = new Matrix;
	m_matWorldRevolve = new Matrix;
	m_matParent = new Matrix;
	m_matWorld = new Matrix;

	*m_matWorldScale = *transform.m_matWorldScale;
	*m_matWorldRotationX = *transform.m_matWorldRotationX;
	*m_matWorldRotationY = *transform.m_matWorldRotationY;
	*m_matWorldRotationZ = *transform.m_matWorldRotationZ;
	*m_matWorldRotation = *transform.m_matWorldRotation;
	*m_matWorldPosition = *transform.m_matWorldPosition;
	*m_matWorldRevolveX = *transform.m_matWorldRevolveX;
	*m_matWorldRevolveY = *transform.m_matWorldRevolveY;
	*m_matWorldRevolveZ = *transform.m_matWorldRevolveZ;
	*m_matWorldRevolve = *transform.m_matWorldRevolve;
	*m_matParent = *transform.m_matParent;
	*m_matWorld = *transform.m_matWorld;

}

bool CTransform::DoNotUpdateInProcedural(bool bUse)
{
	m_bDoNotUpdateInProcedural = bUse;
	return m_bDoNotUpdateInProcedural;
}

bool CTransform::IsUpdateInProcedural() const
{
	return m_bDoNotUpdateInProcedural;
}

void CTransform::SetScene(CScene * pScene)
{
	m_pScene = pScene;
}

void CTransform::SetLayer(CLayer * pLayer)
{
	m_pLayer = pLayer;
}

void CTransform::SetUpdate(bool bUpdate)
{
	m_bUpdate = bUpdate;	

	if (m_bUpdate)
	{
		size_t iChildCount = m_vecChildren.size();

		for (size_t i = 0; i < iChildCount; ++i)
		{
			if (m_vecChildren[i] != this)
			{
				m_vecChildren[i]->SetUpdate(true);
			}
		}
	}
}

bool CTransform::Initialize()
{
	m_vLocalScale = Vector3::One;
	m_vLocalRotation = Vector3::Zero;
	m_vLocalPosition = Vector3::Zero;
	for (int i = 0; i < AX_END; ++i)
	{
		m_vLocalAxis[i] = Vector3::Axis[i];
	}

	m_vWorldScale = Vector3::One;
	m_vWorldRotation = Vector3::Zero;
	m_vWorldPosition = Vector3::Zero;
	m_vWorldRevolve = Vector3::Zero;
	for (int i = 0; i < AX_END; ++i)
	{
		m_vWorldAxis[i] = Vector3::Axis[i];
	}

	return true;
}

int CTransform::Update(float fTime)
{
	if (m_bUpdate)
	{
		*m_matLocal = *m_matLocalScale * *m_matLocalRotation * *m_matLocalPosition;
		*m_matWorld = *m_matWorldScale * *m_matWorldRotation * *m_matWorldPosition;// **m_matWorldRevolve * *m_matParent;

		ComputeAxis();

		m_bUpdate = false;

		if (!m_vecChildren.empty())
		{
			size_t iChildCount = m_vecChildren.size();
			for (size_t i = 0; i < iChildCount; ++i)
			{ 
				if (!m_vecChildren[i]->m_bDoNotUpdateInProcedural)
				{
					m_vecChildren[i]->UpdateChildTransform();
				}
			}
		}
	}

	m_vPrevPosition = m_vWorldPosition;
	return 0;
}

int CTransform::LateUpdate(float fTime)
{
	if (m_bUpdate)
	{
		*m_matLocal = *m_matLocalScale * *m_matLocalRotation * *m_matLocalPosition;
		*m_matWorld = *m_matWorldScale * *m_matWorldRotation * *m_matWorldPosition; // * *m_matWorldRevolve * *m_matParent;

		ComputeAxis();

		m_bUpdate = false;

		if (!m_vecChildren.empty())
		{
			size_t iChildCount = m_vecChildren.size();
			for (size_t i = 0; i < iChildCount; ++i)
			{
				if (!m_vecChildren[i]->m_bDoNotUpdateInProcedural)
				{
					m_vecChildren[i]->UpdateChildTransform();
				}
			}
		}
	}

	m_vPrevPosition = m_vWorldPosition;
	return 0;
}

CTransform * CTransform::Clone() const
{
	return new CTransform(*this);
}

void * CTransform::operator new(size_t iSize)
{
	return (CTransform*)_aligned_malloc(iSize, 16);
}

void CTransform::operator delete(void * p)
{
	_aligned_free(p);
}


void CTransform::SetLocalScale(const Vector3 & vScale)
{
	m_vLocalScale = vScale;
	*m_matLocalScale = XMMatrixScaling(m_vLocalScale.x, m_vLocalScale.y, m_vLocalScale.z);
	SetUpdate(true);
}

void CTransform::SetLocalScale(float x, float y, float z)
{
	Vector3 vScale(x, y, z);
	m_vLocalScale = vScale;
	*m_matLocalScale = XMMatrixScaling(m_vLocalScale.x, m_vLocalScale.y, m_vLocalScale.z);	
	SetUpdate(true);
}

void CTransform::SetLocalScale(float fScale[3])
{
	Vector3 vScale = fScale;
	m_vLocalScale = vScale;
	*m_matLocalScale = XMMatrixScaling(m_vLocalScale.x, m_vLocalScale.y, m_vLocalScale.z);
	SetUpdate(true);
}

void CTransform::SetLocalRotation(const Vector3 & vRotation)
{
	m_vLocalRotation = vRotation;
	*m_matLocalRotationX = XMMatrixRotationX(vRotation.x);
	*m_matLocalRotationY = XMMatrixRotationY(vRotation.y);
	*m_matLocalRotationZ = XMMatrixRotationZ(vRotation.z);

	*m_matLocalRotation = *m_matLocalRotationX * *m_matLocalRotationY * *m_matLocalRotationZ;

	SetUpdate(true);
}

void CTransform::SetLocalRotation(float x, float y, float z)
{
	m_vLocalRotation = Vector3(x, y, z);
	*m_matLocalRotationX = XMMatrixRotationX(m_vLocalRotation.x);
	*m_matLocalRotationY = XMMatrixRotationY(m_vLocalRotation.y);
	*m_matLocalRotationZ = XMMatrixRotationZ(m_vLocalRotation.z);

	*m_matLocalRotation = *m_matLocalRotationX * *m_matLocalRotationY * *m_matLocalRotationZ;

	SetUpdate(true);
}

void CTransform::SetLocalRotation(float fRotation[3])
{
	m_vLocalRotation = Vector3(fRotation);
	*m_matLocalRotationX = XMMatrixRotationX(m_vLocalRotation.x);
	*m_matLocalRotationY = XMMatrixRotationY(m_vLocalRotation.y);
	*m_matLocalRotationZ = XMMatrixRotationZ(m_vLocalRotation.z);

	*m_matLocalRotation = *m_matLocalRotationX * *m_matLocalRotationY * *m_matLocalRotationZ;

	SetUpdate(true);
}

void CTransform::SetLocalRotationX(float x)
{
	m_vLocalRotation.x = x;
	*m_matLocalRotationX = XMMatrixRotationX(m_vLocalRotation.x);
	*m_matLocalRotation = *m_matLocalRotationX * *m_matLocalRotationY * *m_matLocalRotationZ;

	SetUpdate(true);
}

void CTransform::SetLocalRotationY(float y)
{
	m_vLocalRotation.y = y;
	*m_matLocalRotationY = XMMatrixRotationY(m_vLocalRotation.y);
	*m_matLocalRotation = *m_matLocalRotationX * *m_matLocalRotationY * *m_matLocalRotationZ;
	
	SetUpdate(true);
}

void CTransform::SetLocalRotationZ(float z)
{
	m_vLocalRotation.z = z;

	*m_matLocalRotationZ = XMMatrixRotationZ(m_vLocalRotation.z);
	*m_matLocalRotation = *m_matLocalRotationX * *m_matLocalRotationY * *m_matLocalRotationZ;

	SetUpdate(true);
}

void CTransform::SetLocalPosition(const Vector3 & vPos)
{
	m_vLocalPosition = vPos;
	*m_matLocalPosition = XMMatrixTranslation(m_vLocalPosition.x, m_vLocalPosition.y, m_vLocalPosition.z);
	SetUpdate(true);
}

void CTransform::SetLocalPosition(float x, float y, float z)
{
	m_vLocalPosition = Vector3(x, y, z);
	*m_matLocalPosition = XMMatrixTranslation(m_vLocalPosition.x, m_vLocalPosition.y, m_vLocalPosition.z);
	SetUpdate(true);
}

void CTransform::SetLocalPosition(float fPos[3])
{
	m_vLocalPosition = Vector3(fPos);
	*m_matLocalPosition = XMMatrixTranslation(m_vLocalPosition.x, m_vLocalPosition.y, m_vLocalPosition.z);
	SetUpdate(true);
}


void CTransform::ComputeAxis()
{
	for (int i = 0; i < 3; ++i)
	{
		//축과 행렬을 곱한다.
		m_vLocalAxis[i] = Vector3::Axis[i].TransformNormal(*m_matLocalRotation).Normalize();
		m_vWorldAxis[i] = Vector3::Axis[i].TransformNormal(*m_matWorldRotation).Normalize();
	}
	m_vLocalAxis[AX_XR] = m_vLocalAxis[AX_X] * -1.0f;
	m_vLocalAxis[AX_YR] = m_vLocalAxis[AX_Y] * -1.0f;
	m_vLocalAxis[AX_ZR] = m_vLocalAxis[AX_Z] * -1.0f;	
	m_vWorldAxis[AX_XR] = m_vWorldAxis[AX_X] * -1.0f;
	m_vWorldAxis[AX_YR] = m_vWorldAxis[AX_Y] * -1.0f;
	m_vWorldAxis[AX_ZR] = m_vWorldAxis[AX_Z] * -1.0f;
}

Vector3 CTransform::GetAngleFromRotationMatrix(const Matrix & mat)
{
	float x, y, z;

	if (mat.fMat[2][0] < 1.0f)
	{
		if (mat.fMat[2][0] > -1.0f)
		{
			y = asin(mat.fMat[2][0]);
			z = atan2(-mat.fMat[1][0], mat.fMat[0][0]);
			x = atan2(-mat.fMat[2][1], mat.fMat[2][2]);
		}
		else
		{
			y = PI / 2.0f;
			z = -atan2(-mat.fMat[1][2], mat.fMat[1][1]);
			x = 0;
		}
	}	
	else
	{
		y = -PI / 2.0f;
		z = atan2(-mat.fMat[1][2], mat.fMat[1][1]);
		x = 0;
	}
	return Vector3(x, y, z);
}

CGameObject* CTransform::GetGameObject() const
{


	m_pGameObject->AddRef();
	return m_pGameObject;
}

void CTransform::SetParent(const string & strTag)
{
	//기존의 부모로 설정 되어진 오브젝트에서는 지워줘야 함.
	if (m_pParent)
	{
		vector<CTransform*>::iterator iter;
		vector<CTransform*>::iterator iterEnd = m_pParent->m_vecChildren.end();

		for (iter = m_pParent->m_vecChildren.begin(); iter != iterEnd; ++iter)
		{
			if (*iter == this)
			{
				//(*iter)->Release();
				m_pParent->m_vecChildren.erase(iter);
				break;
			}
		}
	}

	CGameObject* pParent = m_pLayer->FindGameObjectWithTag(strTag);
	if (pParent)
	{
		m_pParent = pParent->GetTransform();
		m_pParent->Release();
		SAFE_RELEASE(pParent);
		SetLocalMatrix(*m_matWorld * Matrix::Inverse(m_pParent->GetWorldMatrix()));
	}
	else
	{
		//단, 부모가 존재하지 않는 경우에는 로컬의 개념이 소거되므로
		SetLocalMatrix(Matrix::IdentityMatrix());
	}
}

void CTransform::SetParent(CTransform * pParent)
{	
	if (m_pParent == pParent)
	{
		return;
	}

	if (m_pParent)
	{
		vector<CTransform*>::iterator iter;
		vector<CTransform*>::iterator iterEnd = m_pParent->m_vecChildren.end();

		for (iter = m_pParent->m_vecChildren.begin(); iter != iterEnd; ++iter)
		{
			if (*iter == this)
			{
				m_pParent->m_vecChildren.erase(iter);
				m_pParent = NULL;
				break;
			}
		}
	}

	//pParent->AddRef();
	m_pParent = pParent;
	if (m_pParent)
	{
		pParent->AddChild(this);		
		SetLocalMatrix(*m_matWorld * Matrix::Inverse(pParent->GetWorldMatrix()));
	}
	else
	{
		//단, 부모가 존재하지 않는 경우에는 로컬의 개념이 소거되므로
		SetLocalMatrix(Matrix::IdentityMatrix());
	}
}

bool CTransform::IsChild(CTransform * pTarget) const
{
	for (size_t i = 0; i < m_vecChildren.size(); ++i)
	{
		if (m_vecChildren[i] == pTarget)
		{
			return true;
		}
	}
	return false;
}

bool CTransform::IsRoot() const
{
	return m_pParent == NULL;
}

void CTransform::AddChild(const string & strTag)
{
	CGameObject* pTarget = m_pLayer->FindGameObjectWithTag(strTag);
	CTransform* pTargetTransform = pTarget->GetTransform();

	//GetTransform에서 RefCount 올려주고 있으므로 따로 AddRef 하지 않는다.
	if (!IsChild(pTargetTransform))
	{
		//기존에 자식으로 있지 않은 Transform만 추가함.
		m_vecChildren.push_back(pTargetTransform);
		pTargetTransform->SetParent(this);
		SAFE_RELEASE(pTargetTransform);
	}
	SAFE_RELEASE(pTarget);
}

void CTransform::AddChild(CTransform * pChild)
{
	if (!IsChild(pChild))
	{
		m_vecChildren.push_back(pChild);
		pChild->SetParent(this);
	}
}

CTransform * CTransform::GetChild(const string & strChildName) const
{
	size_t iChildCount = m_vecChildren.size();
	for (size_t i = 0; i < iChildCount; ++i)
	{
		CGameObject* pChild = m_vecChildren[i]->GetGameObject();
		if (pChild->GetTag() == strChildName)
		{
			SAFE_RELEASE(pChild);
			return m_vecChildren[i];
		}
		else
		{
			CTransform* pFind = m_vecChildren[i]->GetChild(strChildName);
			if (pFind)
			{
				SAFE_RELEASE(pChild);
				return pFind;
			}
		}
		SAFE_RELEASE(pChild);
	}
	return NULL;
}

CTransform * CTransform::GetParent() const
{
	return m_pParent;
}

CTransform * CTransform::GetChild(int idx) const
{
	return m_vecChildren[idx];
}

size_t CTransform::GetChildrenCount() const
{
	return m_vecChildren.size();
}

vector<CTransform*>* CTransform::GetChildren()
{
	return &m_vecChildren;
}

void CTransform::UpdateChildTransform()
{
	//위치 변환의 경우에는 순서가 딱히 상관 없지만, 회전과 크기가 변경되므로, C * P 순서를 지키도록 한다.
	*m_matParent = *m_pParent->m_matWorld;

	if (m_bUpdate)
	{
		SetLocalMatrix(*m_matLocalScale * *m_matLocalRotation * *m_matLocalPosition);
		SetWorldMatrix(*m_matLocal * *m_matParent);
		ComputeAxis();

		m_bUpdate = false;
	}

	//자식이 있으면 업데이트 해준다.
	if (!m_vecChildren.empty())
	{
		for (size_t i = 0; i < m_vecChildren.size(); ++i)
		{
			if (!m_vecChildren[i]->m_bDoNotUpdateInProcedural)
			{
				m_vecChildren[i]->UpdateChildTransform();
			}
		}
	}

	m_vPrevPosition = m_vWorldPosition;
}

Vector3 CTransform::GetPivot() const
{
	return m_vPivot;
}

Vector3 CTransform::GetLocalScale() const
{
	return m_vLocalScale;
}

Vector3 CTransform::GetLocalRotation() const
{
	return m_vLocalRotation;
}

Vector3 CTransform::GetLocalPosition() const
{
	return m_vLocalPosition;
}

Matrix CTransform::GetLocalMatrix() const
{
	return *m_matLocal;
}

void CTransform::SetLocalMatrix(const Matrix & mat)
{
	Vector3 vPosition;
	Vector3 vRotation;
	Vector3 vScale;

	mat.Decompose(mat, &vPosition, &vRotation, &vScale);
	SetLocalPosition(vPosition);
	SetLocalRotation(vRotation);
	SetLocalScale(vScale);

	*m_matLocal = mat;

	m_bUpdate = false;
}

void CTransform::SetWorldMatrix(const Matrix & mat)
{
	Vector3 vPosition;
	Vector3 vRotation;
	Vector3 vScale;

	mat.Decompose(mat, &vPosition, &vRotation, &vScale);
	SetWorldPosition(vPosition);
	SetWorldRotation(vRotation);
	SetWorldScale(vScale);

	*m_matWorld = mat;
	
	if (m_pParent)
	{
		SetLocalMatrix(*m_matWorld * Matrix::Inverse(m_pParent->GetWorldMatrix()));
	}

	m_bUpdate = false;
}

void CTransform::SetPivot(float x, float y, float z)
{
	m_vPivot = Vector3(x, y, z);
}

void CTransform::SetPivot(const Vector3 & vPoint)
{
	m_vPivot = vPoint;
}

// ========================= World ========================


void CTransform::SetWorldScale(const Vector3 & vScale)
{
	m_vWorldScale = vScale;
	*m_matWorldScale = XMMatrixScaling(m_vWorldScale.x, m_vWorldScale.y, m_vWorldScale.z);
	SetUpdate(true);
}

void CTransform::SetWorldScale(float x, float y, float z)
{
	m_vWorldScale = Vector3(x, y, z);
	*m_matWorldScale = XMMatrixScaling(m_vWorldScale.x, m_vWorldScale.y,	m_vWorldScale.z);
	SetUpdate(true);
}

void CTransform::SetWorldScale(float fScale[3])
{
	m_vWorldScale = Vector3(fScale);
	*m_matWorldScale = XMMatrixScaling(m_vWorldScale.x, m_vWorldScale.y, m_vWorldScale.z);
	SetUpdate(true);
}

void CTransform::SetWorldRotation(const Vector3 & vRotation)
{
	m_vWorldRotation = vRotation;
	*m_matWorldRotationX = XMMatrixRotationX(m_vWorldRotation.x);
	*m_matWorldRotationY = XMMatrixRotationY(m_vWorldRotation.y);
	*m_matWorldRotationZ = XMMatrixRotationZ(m_vWorldRotation.z);
	*m_matWorldRotation = *m_matWorldRotationX * *m_matWorldRotationY * *m_matWorldRotationZ;

	ComputeAxis();

	SetUpdate(true);
}

void CTransform::SetWorldRotation(float x, float y, float z)
{
	m_vWorldRotation = Vector3(x , y, z);
	*m_matWorldRotationX = XMMatrixRotationX(m_vWorldRotation.x);
	*m_matWorldRotationY = XMMatrixRotationY(m_vWorldRotation.y);
	*m_matWorldRotationZ = XMMatrixRotationZ(m_vWorldRotation.z);
	*m_matWorldRotation = *m_matWorldRotationX * *m_matWorldRotationY * *m_matWorldRotationZ;
	SetUpdate(true);
}

void CTransform::SetWorldRotation(float fRotation[3])
{
	m_vWorldRotation = Vector3(fRotation);
	*m_matWorldRotationX = XMMatrixRotationX(m_vWorldRotation.x);
	*m_matWorldRotationY = XMMatrixRotationY(m_vWorldRotation.y);
	*m_matWorldRotationZ = XMMatrixRotationZ(m_vWorldRotation.z);
	*m_matWorldRotation = *m_matWorldRotationX * *m_matWorldRotationY * *m_matWorldRotationZ;
	SetUpdate(true);
}

void CTransform::SetWorldRotationX(float x)
{
	m_vWorldRotation.x = x;
	*m_matWorldRotationX = XMMatrixRotationX(m_vWorldRotation.x);
	*m_matWorldRotation = *m_matWorldRotationX * *m_matWorldRotationY * *m_matWorldRotationZ;
	SetUpdate(true);
}

void CTransform::SetWorldRotationY(float y)
{
	m_vWorldRotation.y = y;
	*m_matWorldRotationY = XMMatrixRotationY(m_vWorldRotation.y);
	*m_matWorldRotation = *m_matWorldRotationX * *m_matWorldRotationY * *m_matWorldRotationZ;
	SetUpdate(true);
}

void CTransform::SetWorldRotationZ(float z)
{
	m_vWorldRotation.z = z;
	*m_matWorldRotationZ = XMMatrixRotationZ(m_vWorldRotation.z);
	*m_matWorldRotation = *m_matWorldRotationX * *m_matWorldRotationY * *m_matWorldRotationZ;
	SetUpdate(true);
}

void CTransform::SetWorldPosition(const Vector3 & vPos)
{
	m_vWorldPosition = vPos;
	*m_matWorldPosition = XMMatrixTranslation(m_vWorldPosition.x, m_vWorldPosition.y, m_vWorldPosition.z);
	SetUpdate(true);
}

void CTransform::SetWorldPosition(float x, float y, float z)
{
	m_vWorldPosition = Vector3(x, y, z);
	*m_matWorldPosition = XMMatrixTranslation(m_vWorldPosition.x, m_vWorldPosition.y, m_vWorldPosition.z);
	SetUpdate(true);
}

void CTransform::SetWorldPosition(float fPos[3])
{
	m_vWorldPosition = Vector3(fPos);
	*m_matWorldPosition = XMMatrixTranslation(m_vWorldPosition.x, m_vWorldPosition.y, m_vWorldPosition.z);
	SetUpdate(true);
}

void CTransform::SetWorldRevolve(const Vector3 & vRotation)
{
	m_vWorldRevolve = vRotation ;

	*m_matWorldRevolveX = XMMatrixRotationX(m_vWorldRevolve.x);
	*m_matWorldRevolveY = XMMatrixRotationY(m_vWorldRevolve.y);
	*m_matWorldRevolveZ = XMMatrixRotationZ(m_vWorldRevolve.z);
	*m_matWorldRevolve = *m_matWorldRevolveX * *m_matWorldRevolveY * *m_matWorldRevolveZ;

	SetUpdate(true);
}

void CTransform::SetWorldRevolve(float x, float y, float z)
{
	m_vWorldRevolve = Vector3(x, y, z);

	*m_matWorldRevolveX = XMMatrixRotationX(m_vWorldRevolve.x);
	*m_matWorldRevolveY = XMMatrixRotationY(m_vWorldRevolve.y);
	*m_matWorldRevolveZ = XMMatrixRotationZ(m_vWorldRevolve.z);
	*m_matWorldRevolve = *m_matWorldRevolveX * *m_matWorldRevolveY * *m_matWorldRevolveZ;

	SetUpdate(true);
}

void CTransform::SetWorldRevolve(float fRotation[3])
{
	m_vWorldRevolve = Vector3(fRotation);

	*m_matWorldRevolveX = XMMatrixRotationX(m_vWorldRevolve.x);
	*m_matWorldRevolveY = XMMatrixRotationY(m_vWorldRevolve.y);
	*m_matWorldRevolveZ = XMMatrixRotationZ(m_vWorldRevolve.z);
	*m_matWorldRevolve = *m_matWorldRevolveX * *m_matWorldRevolveY * *m_matWorldRevolveZ;

	SetUpdate(true);
}

void CTransform::SetWorldRevolveX(float x)
{
	m_vWorldRevolve.x = x;
	*m_matWorldRevolveX = XMMatrixRotationX(m_vWorldRevolve.x);
	*m_matWorldRevolve = *m_matWorldRevolveX * *m_matWorldRevolveY * *m_matWorldRevolveZ;

	SetUpdate(true);
}

void CTransform::SetWorldRevolveY(float y)
{
	m_vWorldRevolve.y = y;
	*m_matWorldRevolveY = XMMatrixRotationY(m_vWorldRevolve.y);
	*m_matWorldRevolve = *m_matWorldRevolveY * *m_matWorldRevolveY * *m_matWorldRevolveZ;

	SetUpdate(true);
}

void CTransform::SetWorldRevolveZ(float z)
{
	m_vWorldRevolve.z = z;
	*m_matWorldRevolveZ = XMMatrixRotationZ(m_vWorldRevolve.z);
	*m_matWorldRevolve = *m_matWorldRevolveX * *m_matWorldRevolveY * *m_matWorldRevolveZ;

	SetUpdate(true);
}



Matrix& CTransform::GetWorldMatrix() const
{
	return *m_matWorld;
}

Matrix & CTransform::GetWorldPositionMatrix() const
{
	return *m_matWorldPosition;
}

Matrix& CTransform::GetWorldRotationMatrix() const
{
	return *m_matWorldRotation;
}

Matrix & CTransform::GetWorldScaleMatrix() const
{
	return *m_matWorldScale;
}


Matrix & CTransform::GetLocalPositionMatrix() const
{
	return *m_matLocalPosition;
}

Matrix & CTransform::GetLocalRotationMatrix() const
{
	return *m_matLocalRotation;
}

Matrix & CTransform::GetLocalScaleMatrix() const
{
	return *m_matLocalScale;
}

Vector3 CTransform::GetWorldScale() const
{
	return m_vWorldScale;
}

Vector3 CTransform::GetWorldRotation() const
{
	return m_vWorldRotation;
}

Vector3 CTransform::GetWorldPosition() const
{
	return m_vWorldPosition;
}

Vector3 CTransform::GetWorldRevolve() const
{
	return m_vWorldRevolve ;
}

Vector3 CTransform::GetLocalAxis(AXIS eAxis) const
{
	return m_vLocalAxis[eAxis];
}

const Vector3 * CTransform::GetLocalAxis() const
{
	return m_vLocalAxis;
}

void CTransform::SetWorldScale(const Matrix & matScale)
{
	*m_matWorldScale = matScale;

	m_vWorldScale.x = (*m_matWorldScale).fMat[0][0];
	m_vWorldScale.y = (*m_matWorldScale).fMat[1][1];
	m_vWorldScale.z = (*m_matWorldScale).fMat[2][2];

	SetUpdate(true);
}

void CTransform::SetWorldPosition(const Matrix & matPosition)
{
	*m_matWorldPosition = matPosition;

	m_vWorldPosition.x = (*m_matWorldPosition).fMat[3][0];
	m_vWorldPosition.y = (*m_matWorldPosition).fMat[3][1];
	m_vWorldPosition.z = (*m_matWorldPosition).fMat[3][2];

	SetUpdate(true);
}

void CTransform::SetWorldRotation(const Matrix & matRotation)
{
	*m_matWorldRotation = matRotation;

	m_vWorldRotation = GetAngleFromRotationMatrix(*m_matWorldRotation);

	SetUpdate(true);
}

Vector3 CTransform::GetWorldAxis(AXIS eAxis) const
{
	return m_vWorldAxis[eAxis];
}

Vector3 CTransform::GetDeltaPosition() const
{
	return m_vWorldPosition - m_vPrevPosition;
}

const Vector3 * CTransform::GetWorldAxis() const
{
	return m_vWorldAxis;
}



void CTransform::Rotate(AXIS eAxis, float fAngle)
{
	switch (eAxis)
	{
	case AX_X:
		m_vWorldRotation.x += fAngle;
		*m_matWorldRotationX = XMMatrixRotationX(m_vWorldRotation.x);
		break;
	case AX_Y:
		m_vWorldRotation.y += fAngle;
		*m_matWorldRotationY = XMMatrixRotationY(m_vWorldRotation.y);
		break;
	case AX_Z:
		m_vWorldRotation.z += fAngle;
		*m_matWorldRotationZ = XMMatrixRotationZ(m_vWorldRotation.z);
		break;
	}

	*m_matWorldRotation = *m_matWorldRotationX * *m_matWorldRotationY * *m_matWorldRotationZ;

	SetUpdate(true);
}

//vPoint 를 중심으로 하며, 임의의 축 Axis를 기준으로 fAngle 만큼 회전한 위치로 회전한다.
void CTransform::RotateAround(const Vector3 vPoint, const Vector3 Axis, float fAngle)
{
	float fDistance = (m_vWorldPosition - vPoint).Length();
	Vector2 angle2 = Vector2(vPoint.x, vPoint.y);
	Vector2 pos2 = Vector2(m_vWorldPosition.x, m_vWorldPosition.y);
	Vector2 dir = Vector2::AngleToDirection(fAngle);
	Vector3 pos = vPoint + dir * fDistance;
	SetWorldPosition(pos);
}

void CTransform::LookAt(const Vector3 & vTargetPosition)
{
	// 바라봐야 할 방향을 구해준다.
	Vector3	vDirection = (vTargetPosition - m_vWorldPosition).Normalize();

	Vector3	vAxis = Vector3::Axis[AX_Z];

	// 기준축과 바라봐야 할 방향간의 각도를 구해준다.
	float	fAngle = vDirection.Dot(vAxis);
	fAngle = acosf(fAngle);

	// 회전을 할 축을 구해준다.
	Vector3	vRotAxis = vAxis.Cross(vDirection);

	// 기준 축과 바라봐야할 방향이 서로 평행할 경우
	if (vRotAxis == Vector3(0.f, 0.f, 0.f))
	{
		vRotAxis = Vector3::Axis[AX_Z];
	}

	*m_matWorldRotation = XMMatrixRotationAxis(vRotAxis.Convert(), fAngle);
	
	m_vWorldRotation = GetAngleFromRotationMatrix(*m_matWorldRotation);


	SetUpdate(true);
}

void CTransform::LookAt(CGameObject * pTargetObject)
{
	CTransform* pTransform = pTargetObject->GetTransform();

	LookAt(pTransform->GetWorldPosition());

	SAFE_RELEASE(pTransform);
}

void CTransform::LookAt(CTransform * pTargetTransform)
{
	
	LookAt(pTargetTransform->GetWorldPosition());

}

void CTransform::LookRotation(const Vector3 & vTargetPoint, float fRotateSpeed, float fRate)
{
	Vector3	vDirection = (vTargetPoint - m_vWorldPosition).Normalize();
	Vector3 vLerpDirection = XMVectorLerp((XMVECTOR)m_vWorldAxis[AX_Z], (XMVECTOR)vDirection, fRate);	
	LookAt(m_vWorldPosition + vLerpDirection);
}

//Return Degree
float CTransform::AngularDistance(const Vector3 & vTargetPosition)
{
	return m_vWorldPosition.Angle(vTargetPosition) * R2D;
}


Vector3 CTransform::forward()
{
	return m_vWorldAxis[AX_Z];
}

Vector3 CTransform::back()
{
	return m_vWorldAxis[AX_ZR];
}

Vector3 CTransform::right()
{
	return m_vWorldAxis[AX_X];
}

Vector3 CTransform::left()
{
	return m_vWorldAxis[AX_XR];
}

Vector3 CTransform::up()
{
	return m_vWorldAxis[AX_Y];
}

Vector3 CTransform::down()
{
	return m_vWorldAxis[AX_YR];
}

void CTransform::Save(FILE * pFile)
{
	fwrite(&m_vPivot, sizeof(Vector3), 1, pFile);
	fwrite(&m_vLocalScale, sizeof(Vector3), 1, pFile);
	fwrite(&m_vLocalRotation, sizeof(Vector3), 1, pFile);
	fwrite(&m_vLocalPosition, sizeof(Vector3), 1, pFile);
	fwrite(&m_vLocalAxis[0], sizeof(Vector3), 3, pFile);
	fwrite(&m_vWorldScale, sizeof(Vector3), 1, pFile);
	fwrite(&m_vWorldRotation, sizeof(Vector3), 1, pFile);
	fwrite(&m_vWorldPosition, sizeof(Vector3), 1, pFile);
	fwrite(&m_vWorldRevolve, sizeof(Vector3), 1, pFile);
	fwrite(&m_vWorldAxis[0], sizeof(Vector3), 3, pFile);
	fwrite(m_matLocalScale, sizeof(Matrix), 1, pFile);
	fwrite(m_matLocalRotationX, sizeof(Matrix), 1, pFile);
	fwrite(m_matLocalRotationY, sizeof(Matrix), 1, pFile);
	fwrite(m_matLocalRotationZ, sizeof(Matrix), 1, pFile);
	fwrite(m_matLocalRotation, sizeof(Matrix), 1, pFile);
	fwrite(m_matLocalPosition, sizeof(Matrix), 1, pFile);
	fwrite(m_matLocal, sizeof(Matrix), 1, pFile);
	fwrite(m_matWorldScale, sizeof(Matrix), 1, pFile);
	fwrite(m_matWorldRotationX, sizeof(Matrix), 1, pFile);
	fwrite(m_matWorldRotationY, sizeof(Matrix), 1, pFile);
	fwrite(m_matWorldRotationZ, sizeof(Matrix), 1, pFile);
	fwrite(m_matWorldRotation, sizeof(Matrix), 1, pFile);
	fwrite(m_matWorldPosition, sizeof(Matrix), 1, pFile);
	fwrite(m_matWorldRevolveX, sizeof(Matrix), 1, pFile);
	fwrite(m_matWorldRevolveY, sizeof(Matrix), 1, pFile);
	fwrite(m_matWorldRevolveZ, sizeof(Matrix), 1, pFile);
	fwrite(m_matWorldRevolve, sizeof(Matrix), 1, pFile);
	fwrite(m_matParent, sizeof(Matrix), 1, pFile);
	fwrite(m_matWorld, sizeof(Matrix), 1, pFile);
}

void CTransform::Load(FILE * pFile)
{
	fread(&m_vPivot, sizeof(Vector3), 1, pFile);
	fread(&m_vLocalScale, sizeof(Vector3), 1, pFile);
	fread(&m_vLocalRotation, sizeof(Vector3), 1, pFile);
	fread(&m_vLocalPosition, sizeof(Vector3), 1, pFile);
	fread(&m_vLocalAxis[0], sizeof(Vector3), 3, pFile);
	fread(&m_vWorldScale, sizeof(Vector3), 1, pFile);
	fread(&m_vWorldRotation, sizeof(Vector3), 1, pFile);
	fread(&m_vWorldPosition, sizeof(Vector3), 1, pFile);
	fread(&m_vWorldRevolve, sizeof(Vector3), 1, pFile);
	fread(&m_vWorldAxis[0], sizeof(Vector3), 3, pFile);
	fread(m_matLocalScale, sizeof(Matrix), 1, pFile);
	fread(m_matLocalRotationX, sizeof(Matrix), 1, pFile);
	fread(m_matLocalRotationY, sizeof(Matrix), 1, pFile);
	fread(m_matLocalRotationZ, sizeof(Matrix), 1, pFile);
	fread(m_matLocalRotation, sizeof(Matrix), 1, pFile);
	fread(m_matLocalPosition, sizeof(Matrix), 1, pFile);
	fread(m_matLocal, sizeof(Matrix), 1, pFile);
	fread(m_matWorldScale, sizeof(Matrix), 1, pFile);
	fread(m_matWorldRotationX, sizeof(Matrix), 1, pFile);
	fread(m_matWorldRotationY, sizeof(Matrix), 1, pFile);
	fread(m_matWorldRotationZ, sizeof(Matrix), 1, pFile);
	fread(m_matWorldRotation, sizeof(Matrix), 1, pFile);
	fread(m_matWorldPosition, sizeof(Matrix), 1, pFile);
	fread(m_matWorldRevolveX, sizeof(Matrix), 1, pFile);
	fread(m_matWorldRevolveY, sizeof(Matrix), 1, pFile);
	fread(m_matWorldRevolveZ, sizeof(Matrix), 1, pFile);
	fread(m_matWorldRevolve, sizeof(Matrix), 1, pFile);
	fread(m_matParent, sizeof(Matrix), 1, pFile);
	fread(m_matWorld, sizeof(Matrix), 1, pFile);
}
