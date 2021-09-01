
struct VS_INPUT_POS
{
	float3	vPos	: POSITION;
};

struct VS_INPUT_COLOR
{
	float3	vPos	: POSITION;
	float4	vColor	: COLOR;
};
struct VS_INPUT_COLOR_NORMAL
{
	float3	vPos	: POSITION;
	float3	vNormal : NORMAL;
	float4	vColor	: COLOR;
};

struct VS_INPUT_TEX_NORMAL
{
	float3	vPos	: POSITION;
	float3	vNormal : NORMAL;
	float2	vUV : TEXCOORD0;
};

struct VS_INPUT_BUMP
{
	float3	vPos	: POSITION;
	float3	vNormal : NORMAL;
	float2	vUV : TEXCOORD;
	float3	vTangent : TANGENT;
	float3	vBinormal : BINORMAL;
};

struct VS_OUTPUT_COLOR
{
	// Semantic ���� �տ� SV_ �� �ٰԵǸ� �� ������ System Value��
	// �Ǿ ���� ������ �Ұ����ϴ�. ������ ��¿����θ� �����
	// ���������� ������ �ȴ�.
	float4	vPos	: SV_POSITION;
	float4	vColor	: COLOR;
	float3	vLocalPosition : POSITION;
};

struct VS_OUTPUT_COLOR_NORMAL
{
	float4	vPos					: SV_POSITION;
	float4	vColor				: COLOR;
	float3	vLocalPos			: POSITION;
	float3	vNormal				: NORMAL;
	float3	vViewPos			: POSITION1;
	float4	vProjectionPos	: POSITION2;
};

struct VS_OUTPUT_TEX_NORMAL
{
	float4	vPos					: SV_POSITION;
	float2	vUV					: TEXCOORD0;
	float3	vLocalPos			: POSITION;
	float3	vNormal				: NORMAL;
	float3	vViewPos			: POSITION1;
	float4	vProjectionPos	: POSITION2;
};

struct VS_INPUT_TEX
{
	float3	vPos		: POSITION;
	float2	vUV		: TEXCOORD0;
};

struct VS_INPUT_TEX_SIZE
{
	float3	vPos		: POSITION;
	//float4	vSize		: POSITION1;
	float2	vUV		: TEXCOORD0;
};


struct VS_OUTPUT_POS
{
	float4	vPos	: SV_POSITION;
	float4	vProjPos	: POSITION2;
};

struct VS_OUTPUT_TEX
{
	float4	vPos	: SV_POSITION;
	float2	vUV		: TEXCOORD0;
	float4	vProjPos	: POSITION;
};

struct VS_OUTPUT_BUMP
{
	float4	vPos	: SV_POSITION;
	float3	vNormal	: NORMAL;
	float2	vUV	: TEXCOORD;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float3	vViewPos	: POSITION1;
	float4	vProjPos	: POSITION2;
};

struct VS_INPUT_TEX_NORMAL_ANIM
{
	float3	vPos	: POSITION;
	float3	vNormal	: NORMAL;
	float2	vUV		: TEXCOORD;
	float4	vWeights	: BLENDWEIGHTS;
	float4	vIndices	: BLENDINDICES;
};

struct VS_INPUT_BUMP_ANIM
{
	float3	vPos	: POSITION;
	float3	vNormal	: NORMAL;
	float2	vUV		: TEXCOORD;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float4	vWeights	: BLENDWEIGHTS;
	float4	vIndices	: BLENDINDICES;
};


struct PS_OUTPUT_SINGLE
{
	float4	vTarget	: SV_TARGET;
};

struct PS_OUTPUT
{
	float4	vTarget0	: SV_TARGET;
	float4	vTarget1	: SV_TARGET1;
	float4	vTarget2	: SV_TARGET2;
	float4	vTarget3	: SV_TARGET3;
	float4	vTarget4	: SV_TARGET4;
	float4	vTarget5	: SV_TARGET5;
};

// Transform Constant Buffer
cbuffer Transform	: register(b0)
{
	matrix	g_matWorld;
	matrix	g_matView;
	matrix	g_matProj;
	matrix	g_matWV;
	matrix	g_matWVP;
	matrix	g_matVP;
	matrix	g_matWP;
	matrix	g_matInvProj;
	matrix	g_matInvVP;
	matrix	g_matInvWV;
	matrix	g_matInvWVP;
			
	matrix	g_matShadowView;
	matrix	g_matShadowProj;
	matrix	g_matShadowVP;
	matrix	g_matShadowWVP;

	float3	g_vPivot;
	int			g_iRenderMode;
	float3	g_vMeshLength;
	int			g_iHasAnimation;
}


cbuffer	PostProcess	: register(b8)
{
	uint		g_iPostProcessingFlags;
	uint		g_iPostProcessingDeviceWidth;
	uint		g_iPostProcessingDeviceHeight;
	float		g_fPostProcessingHDRAverageLuminance;

	float		g_fPostProcessingHDRExposure;
	float		g_fPostProcessingHDRWhiteThreshold;	
	float		g_fPostProcessingFogMinDistance;
	float		g_fPostProcessingFogMaxDistance;
	
	float4		g_vPostProcessingFogColor;

	float		g_fPostProcessingDepthOfFieldMinRange;
	float		g_fPostProcessingDepthOfFieldMaxRange;
	float		g_fPostProcessingDepthOfFieldMaxSigma;
	float		g_fPadding;

	int			g_iMotionBlurMode;
	float3		g_vPostProcessEmpty;
	matrix		g_matPreView;
	matrix		g_matPreViewProj;
}

// Animation 2D Constant Buffer
cbuffer	Animation2D	: register(b9)
{
	float		g_fAnimX;
	float		g_fAnimY;
	float		g_fAnimWidth;
	float		g_fAnimHeght;
	float		g_fAnimAtlasWidth;
	float		g_fAnimAtlasHeight;
	float2	g_vAnimPadding;
}

cbuffer Outline : register(b12)
{
	float	g_fFXOutlineSize;
	float3	g_vFXOutlineOffset;
	float4	g_vFXOutlineColor;
}

cbuffer Effect : register(b10)
{
	uint	g_iEffectFlag;
	float3	g_vEffectPadding;
	// Burn
	float g_fFXBurnProgress;
	float g_fFXBurnBorderRangePercentage;
	float2 g_vFXBurnPadding;
	float4 g_vFXBurnColor;
	// LimLight
	float3  g_vFXLimColor;
	float	g_vFXLimPower;
}

// Material Constant Buffer
cbuffer Material	: register(b1)
{
	float4	g_vMaterialDiffuse;
	float4	g_vMaterialAmbient;
	float4	g_vMaterialSpecular;
	float4	g_vMaterialEmissive;
	float4	g_vMaterialAlpha;
	float		g_fNormalScale;
	float3		vMaterialEmpty;
}


cbuffer Light	: register(b2)
{
	float4		g_vLightDiffuse;
	float4		g_vLightAmbient;
	float4		g_vLightSpecular;
	int				g_iLightType;
	float3		g_vLightDirection;
	float3		g_vLightPosition;
	float			g_fLightRange;
	float			g_fLightInAngle;
	float			g_fLightOutAngle;
	float2		g_vLightEmpty;
}

Texture2D		g_DiffuseTexture : register(t0);
Texture2D		g_NormalMapTexture : register(t1);
Texture2D		g_SpecularMapTexture : register(t2);
Texture2D		g_AlphaMapTexture : register(t3);
Texture2D		g_NoiseTexture : register(t9);

SamplerState	g_DiffuseSampler : register(s0);
Texture2DArray	g_FrameAnimationTexture: register(t9);

Texture2D		g_BoneTexture	: register(t7);
#define	ANIMATION_NONE		0
#define	ANIMATION_2D			1
#define ANIMATION_3D			2
#define ANIMATION_BOTH		3
#define ANIMATION_ATLAS		16

/*Light*/
#define	LT_DIR		0
#define	LT_POINT	1
#define	LT_SPOT	2

struct _tagLightInfo
{
	float4	vDiffuse;
	float4	vAmbient;
	float4	vSpecular;
	float4	vEmissive;
};


float ConvertColor(float4 vColor)
{
	uint4	vColor1 = (uint4)0;
	vColor1.r = uint(vColor.r * 255);
	vColor1.g = uint(vColor.g * 255);
	vColor1.b = uint(vColor.b * 255);
	vColor1.a = uint(vColor.a * 255);

	uint	iColor = 0;
	iColor = (uint)(vColor1.a * 255);
	iColor = (iColor << 8) | vColor1.r;
	iColor = (iColor << 8) | vColor1.g;
	iColor = (iColor << 8) | vColor1.b;

	return asfloat(iColor);
}

float4 ConvertColor(float fColor)
{
	float4	vColor;
	uint	iColor = asuint(fColor);
	vColor.b = (iColor & 0x000000ff) / 255.f;
	vColor.g = ((iColor >> 8) & 0x000000ff) / 255.f;
	vColor.r = ((iColor >> 16) & 0x000000ff) / 255.f;
	vColor.a = ((iColor >> 24) & 0x000000ff) / 255.f;

	return vColor;
}



float2 ComputeAnimation2DUV(float2 _vUV)
{
	float2	vUV = (float2)0;	

	if (g_iHasAnimation & ANIMATION_2D || g_iHasAnimation & ANIMATION_ATLAS)
	{
		//�ȼ� ������ �����Ѵ�.
		vUV.x = floor(_vUV.x * g_fAnimAtlasWidth);
		vUV.y = floor(_vUV.y * g_fAnimAtlasHeight);

		if (vUV.x < 0.0f)
		{
			vUV.x = 0.0f;
		}
		if (vUV.y < 0.0f)
		{
			vUV.y = 0.0f;
		}
		vUV.x = vUV.x + 0.5f;
		vUV.y = vUV.y + 0.5f;

		vUV.x = vUV.x / g_fAnimAtlasWidth;
		vUV.y = vUV.y / g_fAnimAtlasHeight;

		vUV.x = (float)(g_fAnimX + vUV.x * g_fAnimWidth) / (float)g_fAnimAtlasWidth;
		vUV.y = (float)(g_fAnimY + vUV.y * g_fAnimHeght) / (float)g_fAnimAtlasHeight;

	}
	else
		vUV = _vUV;

	return vUV;
}

float Lerp(float fBegin, float fEnd, float fRate)
{
	return (1 - fRate) * fBegin + fRate * fEnd;
}

_tagLightInfo ComputeLight(float3 vViewPos, float3 vViewNormal)
{
	_tagLightInfo	tInfo = (_tagLightInfo)0;

	float3	vLightDir = (float3)0;
	float	fAtt = 1.f;
	float fAttenuationDescendRate;

	//Directional Light
	if (g_iLightType == 0)
	{
		// ���� ������ �� �������� ��ȯ�Ѵ�.
		vLightDir = mul(float4(g_vLightDirection, 0.f), g_matView).xyz;
		vLightDir = -normalize(vLightDir);
	}
	//Point Light
	else if (g_iLightType == 1)
	{
		// ���� ��ġ�� �� �������� ��ȯ�Ѵ�.
		float3	vLightViewPos = mul(float4(g_vLightPosition, 1.f), g_matView).xyz;

		// �������� ������ ���ϴ� ���͸� �����.
		vLightDir = vLightViewPos - vViewPos;

		// ����� �������� �Ÿ��� ���Ѵ�.
		float	fDist = length(vLightDir);

		vLightDir = -normalize(vLightDir);

		if (fDist > g_fLightRange)
			fAtt = 0.f;

		else
			fAtt = 1.f - fDist / g_fLightRange;
	}
	//Spot Light
	else if (g_iLightType == 2)
	{
		// ���� ������ �� �������� ��ȯ�Ѵ�.
		vLightDir = mul(float4(g_vLightDirection, 0.f), g_matView).xyz;
		vLightDir = normalize(vLightDir);

		//���� ��ġ�� �� �������� ��ȯ�Ѵ�.
		float3	vLightViewPos = mul(float4(g_vLightPosition, 1.f), g_matView).xyz;

		// ������ġ���� ������ġ�� ���ϴ� ���͸� �����ش�.
		float3 vLightDirectionToTarget = vViewPos - vLightViewPos;
		vLightDirectionToTarget = normalize(vLightDirectionToTarget);

		float	fViewAngle = dot(vLightDir, vLightDirectionToTarget);

		// �ٱ��� ������ ����� ���� ���� �ȿ� ���ٴ� ���̴�.
		if (fViewAngle < g_fLightOutAngle)
		{
			fAtt = 0.f;
		}
		// �� �ȿ� ���� ��쿡�� ������ ������ ���ش�.
		else
		{
			float	fDistance = distance(vLightViewPos, vViewPos);
			float fDot = dot(-vLightDir, vViewNormal);

			//������ ���� ����� ���� ���Ϳ��� ���� ����� ����̸� ���� ���� �� ���� ��ġ�̴�.
			//= ������ ��������� ������� ���� ���Ϳ��� ���� ����� �����̸� ���� ���� �� ���� ��ġ�̴�.
			if (fDot > 0)
			{
				float fDistanceAttenuation = max(0, 1.f - fDistance / g_fLightRange);
				
				if (fDistance <= g_fLightRange)
				{
					//InAngle ����
					if (fViewAngle >= g_fLightInAngle)
					{
						fAtt = 1.0f;
					}
					//InAngle �ٱ��� �̸鼭 OutAngle����
					else
					{
						//���� ������ �������� ��Ī�Ǿ��� cos������ ������ �ǹǷ�, ������ 0 ���� PI�����̸�,
						//������ Ŭ ���� ���� �۾�����. ���� cos�Լ��� ���ǿ��� InAngle�� �� ũ�� �ǹǷ�,
						//InAngle���� OutAngle�� ���༭ �и� ������ش�.
						//���ڵ� ���� ��ġ�� InAngle�� OutAngle ���̿� �ִٸ� ������ ����� �׻� InAngle���� �����Ƿ�
						//InAngle���� ������ ����� ���ش�.
						fAtt = 1.0f - ((g_fLightInAngle - fViewAngle) / (g_fLightInAngle - g_fLightOutAngle));
					}
				}
				//LightRange �ٱ���
				else
				{
					fAtt = 0.0f;
				}
				//�Ÿ��� ���� ����
				fAtt *= fDistanceAttenuation * fDistanceAttenuation;
			}
			//���� ���� �� ����
			else
			{
				float fDistanceAttenuation = max(0, 1.f - fDistance / g_fLightRange);		

				fAtt = 1.0f - min(1, -fDot);

				if (fDistance <= g_fLightRange)
				{
					if (fViewAngle < g_fLightInAngle)
					{
						fAtt *= (1.0f - ((g_fLightInAngle - fViewAngle) / (g_fLightInAngle - g_fLightOutAngle)));
					}
				}

				fAtt *= fDistanceAttenuation * fDistanceAttenuation;
			}
		}

		vLightDir = -vLightDir;
	}
				
	tInfo.vDiffuse = g_vLightDiffuse * g_vMaterialDiffuse * max(0, dot(vLightDir, vViewNormal)) * fAtt;
	tInfo.vAmbient = g_vLightAmbient * g_vMaterialAmbient* fAtt;

	float3	vView = -normalize(vViewPos);
	float3	vR = 2.f * vViewNormal * dot(vViewNormal, vLightDir) - vLightDir;
	vR = normalize(vR);

	tInfo.vSpecular = (g_vLightSpecular * g_vMaterialSpecular * pow(max(0, dot(vView, vR)), g_vMaterialSpecular.w) * fAtt);

	return tInfo;
}


struct _tagSkinning
{
	float3	vPos;
	float3	vNormal;
	float3	vTangent;
	float3	vBinormal;
};

matrix GetBoneMatrix(int idx)
{
	matrix	matBone =
	{
		g_BoneTexture.Load(int3(idx * 4, 0, 0)),
		g_BoneTexture.Load(int3(idx * 4 + 1, 0, 0)),
		g_BoneTexture.Load(int3(idx * 4 + 2, 0, 0)),
		g_BoneTexture.Load(int3(idx * 4 + 3, 0, 0))
	};

	return matBone;
}

_tagSkinning Skinning(float3 vPos, float3 vNormal, float3 vTangent,
	float3 vBinormal, float4 vWeights, float4 vIndices)
{
	_tagSkinning	tSkinning = (_tagSkinning)0;

	float	fWeights[4];
	fWeights[0] = vWeights.x;
	fWeights[1] = vWeights.y;
	fWeights[2] = vWeights.z;
	fWeights[3] = 1.f - vWeights.x - vWeights.y - vWeights.z;

	for (int i = 0; i < 4; ++i)
	{
		matrix	matBone = GetBoneMatrix((int)vIndices[i]);

		tSkinning.vPos += fWeights[i] * mul(float4(vPos, 1.f), matBone).xyz;
		tSkinning.vNormal += fWeights[i] * mul(float4(vNormal, 0.f), matBone).xyz;
		tSkinning.vTangent += fWeights[i] * mul(float4(vTangent, 0.f), matBone).xyz;
		tSkinning.vBinormal += fWeights[i] * mul(float4(vBinormal, 0.f), matBone).xyz;
	}

	tSkinning.vNormal = normalize(tSkinning.vNormal);
	tSkinning.vTangent = normalize(tSkinning.vTangent);
	tSkinning.vBinormal = normalize(tSkinning.vBinormal);

	return tSkinning;
}

_tagSkinning Skinning(float3 vPos, float3 vNormal, float4 vWeights,
	float4 vIndices)
{
	_tagSkinning	tSkinning = (_tagSkinning)0;

	float	fWeights[4];
	fWeights[0] = vWeights.x;
	fWeights[1] = vWeights.y;
	fWeights[2] = vWeights.z;
	fWeights[3] = 1.f - vWeights.x - vWeights.y - vWeights.z;

	for (int i = 0; i < 4; ++i)
	{
		matrix	matBone = GetBoneMatrix((int)vIndices[i]);

		tSkinning.vPos += fWeights[i] * mul(float4(vPos, 1.f), matBone).xyz;
		tSkinning.vNormal += fWeights[i] * mul(float4(vNormal, 0.f), matBone).xyz;
	}

	tSkinning.vNormal = normalize(tSkinning.vNormal);

	return tSkinning;
}

float4 ComputeRimLight(float3 vOriginColor, float3 vVertexNormal, float3 vPosition)
{
	float3 vRimColor = g_vFXLimColor;
	float4 vResult = 0;
	float3 vDirection = normalize(-vPosition);
	float fRim = 0;
	fRim = max(0.f, dot(vVertexNormal, vDirection));
	fRim = pow(1.0f - fRim, g_vFXLimPower);
	vResult.rgb = ((1.0f - fRim) * vOriginColor.rgb) + (fRim * vRimColor);
	vResult.a = 1.0f;
	return vResult;
}