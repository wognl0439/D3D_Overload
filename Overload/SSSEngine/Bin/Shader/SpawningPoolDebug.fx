#include "Share.fx"

cbuffer	SpawningPool	: register(b11)
{
	float4 g_vSpawningPoolRangeColor;
}

VS_OUTPUT_COLOR_NORMAL SpawningPoolDebugVS(VS_INPUT_COLOR_NORMAL input)
{
	VS_OUTPUT_COLOR_NORMAL output = (VS_OUTPUT_COLOR_NORMAL)0;
	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;
	output.vPos = mul(float4(vPos, 1.f), g_matWVP);	
	output.vColor = g_vSpawningPoolRangeColor;
	output.vLocalPos = input.vPos;

	//float4	vPos					: SV_POSITION;
	//float4	vColor				: COLOR;
	//float3	vLocalPos			: POSITION;
	//float3	vNormal				: NORMAL;
	//float3	vViewPos			: POSITION1;
	//float4	vProjectionPos	: POSITION2;
	return output;
}

PS_OUTPUT_SINGLE SpawningPoolDebugPS(VS_OUTPUT_COLOR_NORMAL input)
{
	PS_OUTPUT_SINGLE output = (PS_OUTPUT_SINGLE)0;

	output.vTarget = g_vSpawningPoolRangeColor;

	return output;
}
