#include "Share.fx"

#define LUMINANCE_FACTOR float4(0.27f, 0.67f, 0.06f, 0.0f)
#define DELTA 0.00000001f

SamplerState	samLinear : register(s12);
Texture2D		 g_Input0 : register(t0);
Texture2D		 g_Input1 : register(t1);

static const float2 vPos[4] =
{
	float2(-1.0f, 1.0f),
	float2(1.0f, 1.0f),
	float2(-1.0f, -1.0f),
	float2(1.0f, -1.0f),
};

static const float2 vUV[4] =
{
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f),
	float2(0.0f, 1.0f),
	float2(1.0f, 1.0f),
};

cbuffer PostCBuffer : register(b8)
{
	uint mipLevel0 = 0;
	uint mipLevel1 = 1;
	uint mip_empty0;
	uint mip_empty1;
};

VS_OUTPUT_TEX DrawFullScreenQuad(uint iVertexID : SV_VertexID)
{
	VS_OUTPUT_TEX output = (VS_OUTPUT_TEX)0;

	output.vPos = float4(vPos[iVertexID], 0.0f, 1.0f);
	output.vUV = vUV[iVertexID];

	return output;
}

float CalcLuminance(VS_OUTPUT_TEX input) : SV_Target
{
	return log(dot(g_Input0.Sample(samLinear, input.vUV).rgb, LUMINANCE_FACTOR) + DELTA);
}

float AvgLuminance(VS_OUTPUT_TEX input) : SV_Target
{
	return  0.08f / (exp(g_Input0.SampleLevel(samLinear, float2(0.5, 0.5), 9).r) - DELTA);
}

float PrevAvgLuminance() : SV_Target
{
	return g_Input0.Load(float3(0, 0, 0)).r;
}

float4 Bright(VS_OUTPUT_TEX input) : SV_Target
{
	return max(g_Input0.SampleLevel(samLinear, input.vUV, 0) - 0.5f, (float4)0.0f);
}

float4 Add(VS_OUTPUT_TEX input) : SV_Target
{
	return 0.5f * (g_Input0.SampleLevel(samLinear, input.vUV, mipLevel0) + g_Input1.SampleLevel(samLinear, input.vUV, mipLevel1));
}

static const float g_BlurWeights[] =
{
	0.004815026f,
	0.028716039f,
	0.102818575f,
	0.221024189f,
	0.28525234f,
	0.221024189f,
	0.102818575f,
	0.028716039f,
	0.004815026f
};

float4 BlurHorizontal(VS_OUTPUT_TEX input) : SV_TARGET
{
	const float2 OFFSETS[] =
	{ 
		float2(-4, 0), float2(-3, 0), float2(-2, 0),
		float2(-1, 0), float2(0, 0), float2(1, 0),
		float2(2, 0), float2(3, 0), float2(4, 0) 
	};

	float4 output = g_Input0.Load(float3(input.vPos.xy + OFFSETS[0], mipLevel0)) * g_BlurWeights[0];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[1], mipLevel0)) * g_BlurWeights[1];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[2], mipLevel0)) * g_BlurWeights[2];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[3], mipLevel0)) * g_BlurWeights[3];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[4], mipLevel0)) * g_BlurWeights[4];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[5], mipLevel0)) * g_BlurWeights[5];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[6], mipLevel0)) * g_BlurWeights[6];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[7], mipLevel0)) * g_BlurWeights[7];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[8], mipLevel0)) * g_BlurWeights[8];
	
	return output;
}


float4 BlurVertical(VS_OUTPUT_TEX input) : SV_TARGET
{
	const float2 OFFSETS[] =
	{ 
		float2(0, -4), float2(0, -3), float2(0, -2),
		float2(0, -1), float2(0, 0), float2(0, 1),
		float2(0, 2), float2(0, 3), float2(0, 4) 
	};

	float4 output = g_Input0.Load(float3(input.vPos.xy + OFFSETS[0], mipLevel0)) * g_BlurWeights[0];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[1], mipLevel0)) * g_BlurWeights[1];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[2], mipLevel0)) * g_BlurWeights[2];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[3], mipLevel0)) * g_BlurWeights[3];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[4], mipLevel0)) * g_BlurWeights[4];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[5], mipLevel0)) * g_BlurWeights[5];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[6], mipLevel0)) * g_BlurWeights[6];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[7], mipLevel0)) * g_BlurWeights[7];
	output += g_Input0.Load(float3(input.vPos.xy + OFFSETS[8], mipLevel0)) * g_BlurWeights[8];
	
	return output;
}
