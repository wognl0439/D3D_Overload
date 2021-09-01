#include "Share.fx"

// Collider CBuffer
cbuffer Collider	: register(b11)
{
	int			g_bUI;
	float3	g_vEmpty;
	float4	g_vColliderColor;
}
#define COLLIDER_UI 1

VS_OUTPUT_COLOR ColliderVS(VS_INPUT_COLOR input)
{
	VS_OUTPUT_COLOR output = (VS_OUTPUT_COLOR)0;

	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;

	if (g_bUI == COLLIDER_UI)
	{
		output.vPos = mul(float4(vPos, 1.f), g_matWP);
	}
	else
	{
		output.vPos = mul(float4(vPos, 1.f), g_matWVP);
	}
	output.vColor = g_vColliderColor;
	output.vLocalPosition = input.vPos;

	return output;
}

PS_OUTPUT_SINGLE ColliderPS(VS_OUTPUT_COLOR input)
{
	PS_OUTPUT_SINGLE output = (PS_OUTPUT_SINGLE)0;

	output.vTarget = input.vColor;

	return output;
}
