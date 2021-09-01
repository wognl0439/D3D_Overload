#pragma once
#include "../Component.h"

SSS_BEGIN

enum PARTICLE_EMISSION_SHAPE
{
	PES_SPHERE,
	PES_CONE
};

class SSS_DLL CParticle :
	public CComponent
{
private:
	friend class CGameObject;

private:
	typedef struct _tagParticleInfo
	{
		bool			bEnabled;
		Vector3		vPos;
		Vector3		vDir;
		Vector3		vScale;
		Vector3		vForce;
		float		fSpeed;
		float		fTime;
		float		fLifetime;
		float		fAlpha;
		
		void Reset()
		{
			bEnabled = true;
			vPos = Vector3::Zero;
			vDir = Vector3::Zero;
			vScale = Vector3::One;
			vForce = Vector3::Zero;
			fSpeed = 0;
			fTime = 0;
			fLifetime = 0;
			fAlpha = 0;			
		}
		
		void Unuse()
		{
			Reset();
			bEnabled = false;
		}

		_tagParticleInfo() :
			fSpeed(0),
			fTime(0),
			fLifetime(0),
			fAlpha(0),
			bEnabled(false)
		{}
	}ParticleInfo, *PParticleInfo;

public:
	CParticle();
	~CParticle();

public:
	bool m_bOnEmission;
	vector<ParticleInfo> m_vecParticleInfo;
	vector<VERTEXPARTICLE> m_vecVertex;
	PARTICLE_EMISSION_SHAPE m_eEmissionShape;

	float			m_fEnableTimeLapse;

	//Physics
	bool			m_bLoop;
	bool			m_bRigidBody;
	bool			m_bGravity;
	float			m_fLocalTimeScale;
	Vector2		m_vMassRange;
	Vector2		m_vForceRange;
	//Properties
	float			m_fRateOverTime; // 초당 생성 개수
	size_t		m_iMaxCount;
	size_t		m_iCountFromStart;
	size_t		m_iCurrentCount;
	Vector2		m_vMinSize;
	Vector2		m_vMaxSize;
	Vector2		m_vSpeedRange;
	Vector2		m_vLifetimeRange;
	Vector2		m_vAlphaRange;
	float			m_fAngle; // Angle 안쪽으로 랜덤한 위치에 생성
	float			m_fRadius;
	float			m_fStartDelay; // 컴포넌트 추가 직후 딜레이 (s)
	

	// Texture와 Diffuse Color를 저장해둔 후에 이것으로 출력한다.
	class CShader*			m_pShader;
	class CMesh*				m_pMesh;
	ID3D11InputLayout*	m_pLayout;
	class CTexture*			m_pTexture;
	class CSampler*			m_pSampler;
	class CRenderState*	m_pAlphaBlend;

	Vector4						m_vDiffuse;
	Vector4						m_vEmissive;


private:
	int GetEmptyIndex() const;
	void UpdateTransform();
	static bool SortParticle(VERTEXPARTICLE & tSrc, VERTEXPARTICLE & tDest);

public:
	void SetRateOverTime(float fRateOverTime);
	void SetTimeScale(float fTimeScale);
	void SetStartDelay(float fDelay);
	void SetMaxCount(size_t iMax);
	void SetEmissionShape(PARTICLE_EMISSION_SHAPE eShape);
	void SetGravity(bool bUse);
	void SetRigidbody(bool bUse);
	void SetLoop(bool bLoop);
	void SetMass(float fMin, float fMax);
	void SetForce(float fMin, float fMax);
	void SetSize(const Vector2& vMin, const Vector2& vMax);
	void SetSpeed(float fMin, float fMax);
	void SetLifetime(float fMin, float fMax);
	void SetAlpha(float fMin, float fMax);
	void SetAngle(float fAngle);
	void SetRadius(float fRadius);
	void SetTexture(const string& strKey, const wchar_t* pFileName, const string& strPathKey = TEXTURE_PATH);
	void SetDiffuseColor(const Vector4& vColor);
	void SetEmissiveColor(const Vector4& vColor);


public:
	bool Initialize() override;
	int Update(float fTime) override;
	void Render(float fTime) override;
};

SSS_END