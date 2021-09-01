#include "Share.fx"

SamplerState		g_LinearSampler	: register(s1);
Texture2D			g_LoadingTexture	: register(t0);

cbuffer	Loading	: register(b11)
{
	float g_fLoadingProgress;
	float3 g_vLoadingPadding;
}



VS_OUTPUT_TEX LoadingVS(VS_INPUT_TEX input)
{
	VS_OUTPUT_TEX	output = (VS_OUTPUT_TEX)0;
	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;
	output.vPos = mul(float4(vPos, 1.f), g_matWP);
	output.vUV = input.vUV;
	return output;
}

PS_OUTPUT_SINGLE LoadingPS(VS_OUTPUT_TEX input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;


	float4	vMaskColor = (float4) 0;
	float4	vColor = (float4)0;
	//Masking Texture와 Background Texture는 동일함.
	
	float2 vUV = (float2)0;
	vUV = input.vUV;
	vUV.y = input.vUV.y * 0.5f + 0.5f; //Mask의 Offset
	vMaskColor = g_LoadingTexture.Sample(g_LinearSampler, vUV);

	//마스킹
	if (vMaskColor.a == 0.0f)
		clip(-1);

	if (1.0f - input.vUV.y >= g_fLoadingProgress)
	{
		vColor = vMaskColor;
	}
	else
	{
		vUV.y = vUV.y - 0.5f;
		vColor = g_LoadingTexture.Sample(g_LinearSampler, vUV);
	}
	
	output.vTarget = vColor * g_vMaterialDiffuse;

	return output;
}
