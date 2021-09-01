#pragma once

#include "Macro.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "Flag.h"

//Math
#define PI		  3.14159265358979f
#define D2R	  0.01745329251994f
#define R2D	57.29577951308231f

// 충돌 구역용 해싱 함수
namespace std
{
	template<>
	struct hash<SSS::_tagVector2>
	{
		size_t operator()(const SSS::_tagVector2	& key) const
		{
			return hash_value<float>(key.x + key.y);
		}
	};


	template<>
	struct hash<SSS::_tagVector3>
	{
		size_t operator()(const SSS::_tagVector3	& key) const
		{
			return hash_value<float>(key.x + key.y + key.z);
		}
	};

}

SSS_BEGIN


typedef struct SSS_DLL _tagItemInfo
{
	uint64_t iID;
	string strName;
	string strMeshName; // 메시 컨테이너 키 이름

	uint64_t iPrice; // 살떄는 이가격, 팔떄는 2.75/10
	
	ITEM_TYPE eType = ITEM_TYPE::IT_TRADE;
	ITEM_GRADE eGrade = ITEM_GRADE::IG_NORMAL;
	ITEM_EQUIP_PART eEquipPart = ITEM_EQUIP_PART::IEP_NONE;

	float fHP = 0.0f;
	float fSP = 0.0f;
	float fMP = 0.0f;

	float fAttack = 0.0f;
	float fDefence = 0.0f;
	float fAttackSpeed = 0.0f;
	float fMoveSpeed = 0.0f;
	float fCriticalChange = 0.0f;
	float fCriticalDamage = 0.0f;
	float fDodge = 0.0f;

}ITEMINFO, *PITEMINFO;
typedef struct SSS_DLL _tagRay
{
	Vector3 vOrigin;
	Vector3 vDirection;
	Vector4 vColor;
	float fDistance;
}RAY, *PRAY;


typedef struct SSS_DLL _tagLine
{
	vector<Vector3> vecPoint;
	Vector4 vColor;
}LINE, *PLINE;



typedef struct SSS_DLL _tagAtlasData
{
	string strKey;
	unsigned short wX = 0;
	unsigned short wY = 0;
	unsigned short wWidth = 0;
	unsigned short wHeight = 0;
	unsigned int iAtlasWidth = 0;
	unsigned int iAtlasHeight = 0;
}ATLASDATA, *PATLASDATA;

//Shader
typedef struct SSS_DLL _tagVertexColor
{
	Vector3 vertexPosition;
	Vector4 vertexColor;

	_tagVertexColor()
	{}

	//Copy Constructor
	_tagVertexColor(const _tagVertexColor& vtx)
	{}

	//Init
	_tagVertexColor(float x, float y, float z, float r, float g, float b, float a) :
		vertexPosition(x, y, z),
		vertexColor(r, g, b, a)		
	{}
	//

}VERTEXCOLOR, *PVERTEXCOLOR;


//Shader
typedef struct SSS_DLL _tagVertexParticle
{
	Vector3 vPosition;
	Vector2 vScale;
	float fAlpha;
	uint32_t iEnabled;

	void Unuse()
	{
		vPosition = Vector3::Zero;
		vScale = Vector2::Zero;
		fAlpha = 0;
		iEnabled = 0;
	}
	_tagVertexParticle():
		iEnabled(0),
		fAlpha(0)
	{}

}VERTEXPARTICLE, *PVERTEXPARTICLE;


// Vertex Color Normal
typedef struct SSS_DLL _tagVertexColorNormal
{
	Vector3	vPos;
	Vector3	vNormal;
	Vector4	vColor;

	_tagVertexColorNormal()
	{
	}

	_tagVertexColorNormal(const _tagVertexColorNormal& vtx) :
		vPos(vtx.vPos),
		vNormal(vtx.vNormal),
		vColor(vtx.vColor)
	{
	}

	_tagVertexColorNormal(const Vector3& vP, const Vector3& vN, const Vector4& vC)
	{
		vPos = vP;
		vNormal = vN;
		vColor = vC;
	}

	_tagVertexColorNormal(float x, float y, float z, float nx, float ny, float nz,
		float r, float g, float b, float a) :
		vPos(x, y, z),
		vNormal(nx, ny, nz),
		vColor(r, g, b, a)
	{
	}
}VERTEXCOLORNORMAL, *PVERTEXCOLORNORMAL;


// Vertex Tex
typedef struct SSS_DLL _tagVertexTex
{
	Vector3	vPos;
	Vector2	vUV;

	_tagVertexTex()
	{
	}

	_tagVertexTex(const _tagVertexTex& vtx) :
		vPos(vtx.vPos),
		vUV(vtx.vUV)
	{
	}

	_tagVertexTex(float x, float y, float z, float u, float v) :
		vPos(x, y, z),
		vUV(u, v)
	{
	}
}VERTEXTEX, *PVERTEXTEX;


typedef struct SSS_DLL _tagVertexTexNormal
{
	Vector3	vPos;
	Vector3	vNormal;
	Vector2 vUV;

	_tagVertexTexNormal()
	{
	}

	_tagVertexTexNormal(const _tagVertexTexNormal& vtx) :
		vPos(vtx.vPos),
		vNormal(vtx.vNormal),
		vUV(vtx.vUV)
	{
	}

	_tagVertexTexNormal(const Vector3& vP, const Vector3& vN ,const Vector2& vU )
	{
		vPos = vP;
		vNormal = vN;
		vUV = vU;
	}

	_tagVertexTexNormal(float x, float y, float z, float nx, float ny, float nz, float u, float v) :
		vPos(x, y, z),
		vUV(u, v),
		vNormal(nx, ny, nz)
	{
	}
}VERTEXTEXNORMAL, *PVERTEXTEXNORMAL;


// Vertex Bump
typedef struct SSS_DLL _tagVertexBump
{
	Vector3	vPos;
	Vector3	vNormal;
	Vector2	vUV;
	Vector3	vTangent;
	Vector3	vBinormal;

	_tagVertexBump()
	{
	}

	_tagVertexBump(const _tagVertexBump& vtx) :
		vPos(vtx.vPos),
		vNormal(vtx.vNormal),
		vUV(vtx.vUV),
		vTangent(vtx.vTangent),
		vBinormal(vtx.vBinormal)
	{
	}
}VERTEXBUMP, *PVERTEXBUMP;


// Vertex Tex Normal Anim
typedef struct SSS_DLL _tagVertexTexNormalAnim
{
	Vector3	vPos;
	Vector3	vNormal;
	Vector2	vUV;
	Vector4	vWeight;
	Vector4	vIndex;

	_tagVertexTexNormalAnim()
	{
	}

	_tagVertexTexNormalAnim(const _tagVertexTexNormalAnim& vtx) :
		vPos(vtx.vPos),
		vNormal(vtx.vNormal),
		vUV(vtx.vUV),
		vWeight(vtx.vWeight),
		vIndex(vtx.vIndex)
	{
	}
}VERTEXTEXNORMALANIM, *PVERTEXTEXNORMALANIM;

// Vertex Bump
typedef struct SSS_DLL _tagVertexBumpAnim
{
	Vector3	vPos;
	Vector3	vNormal;
	Vector2	vUV;
	Vector3	vTangent;
	Vector3	vBinormal;
	Vector4	vWeight;
	Vector4	vIndex;

	_tagVertexBumpAnim()
	{
	}

	_tagVertexBumpAnim(const _tagVertexBumpAnim& vtx) :
		vPos(vtx.vPos),
		vNormal(vtx.vNormal),
		vUV(vtx.vUV),
		vTangent(vtx.vTangent),
		vBinormal(vtx.vBinormal),
		vWeight(vtx.vWeight),
		vIndex(vtx.vIndex)
	{
	}
}VERTEXBUMPANIM, *PVERTEXBUMPANIM;





// 상수 버퍼 사용시 주의사항 : 반드시 메모리 사이즈를 16바이트 기준으로
// 맞춰서 사용해야 한다. 예를 들어 24바이트 일 경우 반드시 8바이트를
// 더 선언해서 맞춰주어야 한다.
typedef struct SSS_DLL _tagTransformConstantBuffer
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProjection;
	Matrix	matWV;
	Matrix	matWVP;
	Matrix	matVP;
	Matrix   matWP;
	Matrix	matInvProjection;
	Matrix	matInvVP;
	Matrix	matInvWV;
	Matrix	matInvWVP;

	Matrix	matShadowView;
	Matrix	matShadowProj;
	Matrix	matShadowVP;
	Matrix	matShadowWVP;

	Vector3 vPivot;
	int			iRenderMode;
	Vector3 vLength;
	int			iHasAnimation;
	
}TRANSFORMCBUFFER, *PTRANSFORMCBUFFER;

typedef struct SSS_DLL _tagColliderConstantBuffer
{
	int bUI;
	Vector3 vEmpty;
	Vector4 vColor;
}COLLIDERCBUFFER, *PCOLLIDERCBUFFER;


typedef struct SSS_DLL _tagPostProcessConstantBuffer
{
	unsigned int iFlags;
	unsigned int iDeviceWidth;
	unsigned int iDeviceHeight;
	float		fHDRAverageLuminance;

	float		fHDRExposure;
	float		fHDRWhiteThreshold;
	float fFogMinDistance;
	float fFogMaxDistance;

	Vector4 vFogColor;
	float		fDepthOfFieldMinRange;
	float		fDepthOfFieldMaxRange;
	float		fDepthOfFieldMaxSigma;
	float		fPadding;

	int			iMotionBlurMode;
	Vector3		vEmpty;
	Matrix		matPreView;
	Matrix		matPreViewProj;
}POSTPROCESSCBUFFER, *PPOSTPROCESSCBUFFER;


typedef struct SSS_DLL _tagFXBurn
{
	float fFXBurnProgress;
	float fFXBurnBorderRangePercentage;
	Vector2 vFXBurnPadding;
	Vector4 vFXBurnColor;
}FXBURN, *PFXBURN;

typedef struct SSS_DLL _tagEffect
{
	unsigned int iFlag;
	Vector3		vEmpty;

	// Burn Effect
	float fFXBurnProgress;
	float fFXBurnBorderRangePercentage;
	Vector2 vFXBurnPadding;
	Vector4 vFXBurnColor;

	// LimLight Effect
	Vector3 vFXLimColor;
	float	vFXLimPower;

}EFFECT, *PEFFECT;

typedef struct SSS_DLL _tagFXOutline
{
	float fFXOutlineSize;
	Vector3 vFXOutlineOffset;
	Vector4 vFXOutlineColor;
}FXOUTLINE, *PFXOUTLINE;

typedef struct SSS_DLL _tagMaterial
{
	Vector4		vDiffuse;
	Vector4		vAmbient;
	Vector4		vSpecular;
	Vector4		vEmissive;
	Vector4		vAlpha;
	float		fNormalScale;
	Vector3		vEmpty;

}MATERIAL, *PMATERIAL;

// Animation 2D Constant Buffer
//typedef struct SSS_DLL _tagAnimation2DCBuffer
//{
//	int		iType;
//	int		iFrameX;
//	int		iFrameY;
//	int		iStartX;
//	int		iStartY;
//	int		iLengthX;
//	int		iLengthY;
//	int		iMaxX;
//	int		iMaxY;
//	Vector3	vEmpty;
//}ANIMATION2DCBUFFER, *PANIMATION2DCBUFFER;

// Custom Animation Info
//Unused
//typedef struct SSS_DLL _tagCustomAnimationInfo
//{
//	int		iStartX;
//	int		iStartY;
//	int		iSizeX;
//	int		iSizeY;
//	int		iMaxSizeX;
//	int		iMaxSizeY;
//}CUSTOMANIMATIONINFO, *PCUSTOMANIMATIONINFO;

typedef struct SSS_DLL _tagBillboardCBuffer
{
	Vector3		vCenter;
	float			fSizeX;
	Vector3		vAxisX;
	float			fSizeY;
	Vector3		vAxisY;
	float			fPadding;
}BILLBOARDCBUFFER, *PBILLBOARDCBUFFER;

typedef struct SSS_DLL _tagHealthBarCBuffer
{
	//HealthBar Info
	float			fAlpha;
	float			fHealthPercentage;
	Vector2		vPadding;
	Vector4		vColor;
}HEALTHBARCBUFFER, *PHEALTHBARCBUFFER;

typedef struct SSS_DLL _tagTerrainCBuffer
{
	float	fDetailLevel;
	int		iSplatCount;
	Vector2	vMousePosition;
	float	fRadius;
	Vector3 vEmpty3;

}TERRAINCBUFFER, *PTERRAINCBUFFER;

// Renderer Constant Buffer
typedef struct SSS_DLL _tagRendererCBuffer
{
	int		iTransfer;
	int		iSize;
	void*	pData;
}RENDERERCBUFFER, *PRENDERERCBUFFER;


typedef struct SSS_DLL _tagLightCBuffer
{
	Vector4		vDiffuse;
	Vector4		vAmbient;
	Vector4		vSpecular;
	int				iType;
	Vector3		vDirection;
	Vector3		vPosition;
	float			fRange;
	float			fInAngle;
	float			fOutAngle;
	Vector2		vEmpty;
}LIGHTCBUFFER, *PLIGHTCBUFFER;

typedef struct SSS_DLL _tagShadowRendering
{
	int		iShadowEnable;
	float	fShadowBias;
	Vector2	vShadowResolution;
	Vector3	vShadowLightPos;
	float	vEmpty;

	_tagShadowRendering() :
		fShadowBias(0.00001f)
	{
	}
}SHADOWRENDERING, *PSHADOWRENDERING;

typedef struct SSS_DLL _tagFireCBuffer
{
	float	fFrameTime;
	Vector3 vScrollSpeeds;
	Vector3 vScales;
	float	fOffsetY;
	Vector2 vDistortion1;
	Vector2 vDistortion2;
	Vector2 vDistortion3;
	float   fDistortionScale;
	float   fDistortionBias;
}FIRECBUFFER, *PFIRECBUFFER;

typedef struct SSS_DLL _tagWaterCBuffer
{
	int		 iCameraPos;
	float	 fWaterSpeed;
	Vector2  vWaterEmpty;
}WATERCBUFFER, *PWATERCBUFFER;

typedef struct SSS_DLL _tagLoadingCBuffer
{
	float fProgress;
	Vector3 vPadding;
}LOADINGCBUFFER, *PLOADINGCBUFFER;

typedef struct SSS_DLL _tagTrailCBuffer
{
	Vector3	vTrailCurrentPos;
	float	fTrailScale;
	Vector3	vTrailAxisX;
	int		iTrailSize;
	Vector3	vTrailAxisY;
	float	fTrailEmpty;
	Vector4	vTrailVelocity[10];
}TRAILCBUFFER, *PTRAILCBUFFER;

typedef struct SSS_DLL _tagMipLevelCBuffer
{
	UINT mipLevel0;
	UINT mipLevel1;
	UINT empty0;
	UINT empty1;
}MIPLEVELCBUFFER, *PMIPLEVELCBUFFER;

typedef struct SSS_DLL _tagAnimationClip2DInfo
{
	float		fX;
	float		fY;
	float		fWidth;
	float		fHeght;
	float		fAtlasWidth;
	float		fAtlasHeight;
	Vector2	vPadding;

	_tagAnimationClip2DInfo() :
		fX(0),
		fY(0),
		fWidth(0),
		fHeght(0),
		fAtlasWidth(0),
		fAtlasHeight(0)
	{
	};
}ANIMATIONCLIP2DINFO, *PANIMATIONCLIP2DINFO;

SSS_END
