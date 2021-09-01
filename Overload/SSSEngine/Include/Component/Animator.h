#pragma once
#include "../Component.h"
#include "../Mesh.h"

SSS_BEGIN

typedef struct SSS_DLL _tagBoneTransform
{
	int iBoneIndex = -1;
	CTransform* pTransform = NULL;
}BONETRANSFORM, *PBONETRANSFORM;

class SSS_DLL CAnimator :
	public CComponent
{ 
private:
	friend class CGameObject;

private:
	CAnimator();
	~CAnimator();
	
private:
	class CAnimationStateController* m_pAnimationController;
	class CSpriteRenderer*	m_pSpriteRenderer;
	class CMeshRenderer*	m_pMeshRenderer;
	vector<class CTransform*> m_vecBoneTransform;
	vector<BONETRANSFORM> m_vecRegisteredBoneTransform;
	vector<Matrix> m_vecBoneMatrix;

	bool m_bAnimationStateTransitioned;
	bool m_bSyncScale;
	Vector3 m_vOriginScale;
	Vector3 m_vScaleFactor;
	Vector3 m_vLocalScaleFactor;

	vector<string> m_vecReservedBoneTransform;
	//3D
	ID3D11Texture2D*					m_pBoneTexture;
	ID3D11ShaderResourceView*	m_pBoneTextureRV;


	ANIMATIONCLIP2DINFO m_tClip2DInfo;

private:
	void BlendKeyFrame(const vector<KEYFRAMEBUFFER>& vecBoneSrc, const vector<KEYFRAMEBUFFER>& vecBoneDest, float fSLerpPercent = 0.5f);
	void BlendKeyFrame2D(const KEYFRAMEBUFFER2D& vecBoneSrc);
	bool SetBoneTransformFromReservation();

public:
	vector<BONETRANSFORM>* GetRegisteredBoneObject();
	void RegisterSocket(size_t iBoneIndex, class CTransform* pTransform);
	void RegisterSocket(const string& strBoneName, class CTransform* pTransform);
	int GetBoneIndex(const string& strBoneName);
	string GetBoneName(size_t iBoneIndex);
	Matrix GetBoneMatrix(size_t iBoneIndex) const;
	Matrix GetBoneMatrix(const string& strBoneName);

	vector<class CTransform*>* GetBoneTransform(); //const
	class CMeshRenderer* GetMeshRenderer() const;
	class CSpriteRenderer* GetSpriteRenderer() const;
	void SetTargetRenderer(class CSpriteRenderer* pRenderer);
	void SetTargetRenderer(class CMeshRenderer* pRenderer);
	const Vector3& GetLocalScaleFactor() const;
	const Vector3& GetFixedScaleFactor() const;
	const Vector3& GetOriginScale() const;
	void SetFixedScaleFactor(const Vector3& vFactor);
	void SetLocalScaleFactor(const Vector3& vFactor);
	bool GetSyncScale() const;
	void SetSyncScale(bool bUseSyncScale);
	void SetAnimationStateTransitioned();
	void SetAnimationController(class CAnimationStateController* pController);
	

public:
	void SetEntryState(const string& strStateName);
	class CAnimationState* CreateAnimationStateInCurrentController(const string& strStateName);
	CAnimationStateController* CreateAnimationController();
	class CAnimationStateController* GetAnimationController() const;

public:
	bool CreateBoneObject();
	bool CreateBoneTexture();
	ID3D11Texture2D* GetBoneTexture() const;
	ID3D11ShaderResourceView* GetBoneTextureResourceView() const;
	

public:
	void Reset() override;
	bool Initialize() override; 
	void Start() override;
	int Prerender(class CMeshRenderer* pRenderer) override;
	int Update(float fTime) override;
	
	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;



};

SSS_END
