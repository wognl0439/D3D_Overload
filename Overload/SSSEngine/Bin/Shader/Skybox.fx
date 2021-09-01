#include "Share.fx"

TextureCube	g_CubeTex	: register(t10);


VS_OUTPUT_POS SkyVS(VS_INPUT_TEX_NORMAL input)
{
	VS_OUTPUT_POS	output = (VS_OUTPUT_POS)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP).xyww;
	output.vProjPos = float4(input.vPos, 1.f);

	return output;
}

PS_OUTPUT_SINGLE SkyPS(VS_OUTPUT_POS input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	output.vTarget = g_CubeTex.Sample(g_DiffuseSampler, input.vProjPos.xyz);

	return output;
}
