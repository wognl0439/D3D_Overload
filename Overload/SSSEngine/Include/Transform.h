#pragma once
#include "Ref.h"

SSS_BEGIN

class SSS_DLL CTransform :
	public CRef
{
private:
	friend class CSceneManager;
	friend class CGameObject;

private:
	CTransform();
	CTransform(const CTransform& transform);
	~CTransform();
	
private:
	class CScene*	m_pScene;
	class CLayer*	m_pLayer;
	bool			m_bDoNotUpdateInProcedural;
	bool			m_bUpdate; 

public:
	bool DoNotUpdateInProcedural(bool bUse = true);
	bool IsUpdateInProcedural() const;
	void SetScene(class CScene* pScene);
	void SetLayer(class CLayer* pLayer);
	void SetUpdate(bool bUpdate);

public:
	bool Initialize();
	int Update(float fTime);
	int LateUpdate(float fTime);
	CTransform* Clone()	const;

public:
	void* operator new(size_t iSize);
	void operator delete(void* p);


private:
	//======================= Hierarchy Data =====================
	CGameObject* m_pGameObject;
	CTransform* m_pParent;
	vector<CTransform*> m_vecChildren;	

	//========================= Identity =======================
	Vector3		m_vPivot;

	// ========================= Local ========================
	Vector3		m_vLocalScale;
	Vector3		m_vLocalRotation;
	Vector3		m_vLocalPosition;
	Vector3		m_vLocalAxis[AX_END];
	// ========================= World ========================
	Vector3		m_vWorldScale;
	Vector3		m_vWorldRotation;
	Vector3		m_vWorldPosition;
	Vector3		m_vWorldRevolve;
	Vector3		m_vWorldAxis[AX_END];	
	Vector3		m_vPrevPosition;
private:
	// ========================= Local ========================
	PMatrix		m_matLocalScale;
	PMatrix		m_matLocalRotationX;
	PMatrix		m_matLocalRotationY;
	PMatrix		m_matLocalRotationZ;
	PMatrix		m_matLocalRotation;
	PMatrix		m_matLocalPosition;
	PMatrix		m_matLocal;
	// ========================= World ========================
	PMatrix		m_matWorldScale;
	PMatrix		m_matWorldRotationX;
	PMatrix		m_matWorldRotationY;
	PMatrix		m_matWorldRotationZ;
	PMatrix		m_matWorldRotation;
	PMatrix		m_matWorldPosition;
	PMatrix		m_matWorldRevolveX;
	PMatrix		m_matWorldRevolveY;
	PMatrix		m_matWorldRevolveZ;
	PMatrix		m_matWorldRevolve;
	PMatrix		m_matParent;
	PMatrix		m_matWorld;


private:
	void ComputeAxis();

public:
	static Vector3 GetAngleFromRotationMatrix(const Matrix& mat);


public:
	class CGameObject* GetGameObject() const;
	void SetParent(const string& strTag);
	void SetParent(CTransform* pParent);
	bool IsChild(class CTransform* pTarget) const;
	bool IsRoot() const;
	void AddChild(const string& strTag);
	void AddChild(CTransform* pChild);
	CTransform* GetChild(const string& strChildName) const;
	CTransform* GetParent() const;
	CTransform* GetChild(int idx) const;
	size_t GetChildrenCount() const;
	vector<CTransform*>* GetChildren();
	void UpdateChildTransform();


public:
	Vector3 GetPivot() const;
	Vector3 GetLocalScale()	const;
	Vector3 GetLocalRotation()	const;
	Vector3 GetLocalPosition()	const;
	Matrix GetLocalMatrix()	const;
	void SetLocalMatrix(const Matrix& mat);
	void SetWorldMatrix(const Matrix& mat);

public:
	void SetPivot(float x, float y, float z);
	void SetPivot(const Vector3& vPoint);

public:
	void SetLocalScale(const Vector3& vScale);
	void SetLocalScale(float x, float y, float z);
	void SetLocalScale(float fScale[3]);
	void SetLocalRotation(const Vector3& vRotation);
	void SetLocalRotation(float x, float y, float z);
	void SetLocalRotation(float fRotation[3]);
	void SetLocalRotationX(float x);
	void SetLocalRotationY(float y);
	void SetLocalRotationZ(float z);
	void SetLocalPosition(const Vector3& vPos);
	void SetLocalPosition(float x, float y, float z);
	void SetLocalPosition(float fPos[3]);
	Vector3 GetLocalAxis(AXIS eAxis)	const;
	const Vector3* GetLocalAxis()	const;

public:
	void SetWorldScale(const Matrix& matScale);
	void SetWorldPosition(const Matrix& matPosition);
	void SetWorldRotation(const Matrix& matRotation);
	void SetWorldScale(const Vector3& vScale);
	void SetWorldScale(float x, float y, float z);
	void SetWorldScale(float fScale[3]);
	void SetWorldRotation(const Vector3& vRotationDegree);
	void SetWorldRotation(float x, float y, float z);
	void SetWorldRotation(float fRotation[3]);
	void SetWorldRotationX(float x);
	void SetWorldRotationY(float y);
	void SetWorldRotationZ(float z);
	void SetWorldPosition(const Vector3& vPos);
	void SetWorldPosition(float x, float y, float z);
	void SetWorldPosition(float fPos[3]);
	void SetWorldRevolve(const Vector3& vRotation);
	void SetWorldRevolve(float x, float y, float z);
	void SetWorldRevolve(float fRotation[3]);
	void SetWorldRevolveX(float x);
	void SetWorldRevolveY(float y);
	void SetWorldRevolveZ(float z);

public:
	Vector3 GetWorldScale()	const;
	Vector3 GetWorldRotation()	const;
	Vector3 GetWorldPosition()	const;
	Vector3 GetWorldRevolve()	const;
	Matrix& GetWorldMatrix()	const;
	Matrix& GetWorldPositionMatrix() const;
	Matrix& GetWorldRotationMatrix() const;
	Matrix& GetWorldScaleMatrix() const;
	Matrix& GetLocalPositionMatrix() const;
	Matrix& GetLocalRotationMatrix() const;
	Matrix& GetLocalScaleMatrix() const;
	
	Vector3 GetWorldAxis(AXIS eAxis)	const;
	Vector3 GetDeltaPosition() const;
	
	const Vector3* GetWorldAxis()	const;
	

	void Rotate(AXIS eAxis, float fAngle);
	void RotateAround(const Vector3 vPoint, const Vector3 Axis, float fAngle);
	void LookAt(const Vector3& vTargetPosition);
	void LookAt(class CGameObject* pTargetObject);
	void LookAt(CTransform* pTargetTransform);
	void LookRotation(const Vector3& vTargetPoint, float fRotationSpeed, float fRate);

	float AngularDistance(const Vector3& vTargetPosition);
public:
	Vector3 forward();
	Vector3 back();
	Vector3 right();
	Vector3 left();
	Vector3 up();
	Vector3 down();


public:
	void Save(FILE* pFile);
	void Load(FILE* pFile);
};

SSS_END
