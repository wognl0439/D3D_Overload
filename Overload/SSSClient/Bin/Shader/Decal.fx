#include "Share.fx"

SamplerState		g_GBufferSampler		: register(s11);
Texture2D			g_GBufferAlbedo			: register(t11);
Texture2D			g_GBufferNormal			: register(t12);
Texture2D			g_GBufferDepth			: register(t13);
Texture2D			g_GBufferSpecular		: register(t14);
Texture2D			g_GBufferTangent		: register(t15);
Texture2D			g_GBufferBinormal		: register(t16);

struct VS_OUTPUT_DECAL
{
	float4	vPos	: SV_POSITION;
	float4	vProjPos	: POSITION;
};

struct PS_OUTPUT_DECAL
{
	float4	vAlbedo	: SV_Target;
	float4	vNormal	: SV_Target1;
	float4	vSpecular	: SV_Target2;
};

VS_OUTPUT_DECAL ScreenSpaceDecalVS(VS_INPUT_POS input)
{
	VS_OUTPUT_DECAL	output = (VS_OUTPUT_DECAL)0;

	output.vProjPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vPos = output.vProjPos;

	return output;
}

PS_OUTPUT_DECAL ScreenSpaceDecalPS(VS_OUTPUT_DECAL input)
{
	PS_OUTPUT_DECAL	output = (PS_OUTPUT_DECAL)0;

	float2	vUV = input.vProjPos.xy / input.vProjPos.w;
	vUV = vUV * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

	// 현재 화면의 깊이를 구한다.
	float4	vDepth = g_GBufferDepth.Sample(g_GBufferSampler, vUV);

	if (vDepth.w == 0.f)
		clip(-1);

	// 위치정보를 구한다.
	float4	vPos;
	vPos.x = vUV.x * 2.f - 1.f;
	vPos.y = vUV.y * -2.f + 1.f;
	vPos.z = vDepth.x;

	vPos.xyz *= vDepth.w;
	vPos.w = vDepth.w;

	float3	vLocalPos = mul(vPos, g_matInvWVP).xyz;

	clip(0.5f - abs(vLocalPos));

	// 스크린 UV좌표를 얻어온다.
	float2 vScreenUV = vLocalPos.xz + float2(0.5f, 0.5f);
	vScreenUV.y = 1.f - vScreenUV.y;

	float4 vAlbedo = g_DiffuseTexture.Sample(g_DiffuseSampler, vScreenUV);

	if (vAlbedo.a == 0.f)
		clip(-1);

	output.vAlbedo = vAlbedo;

	float4	vTangent = g_GBufferTangent.Sample(g_GBufferSampler, vUV);
	float4	vBinormal = g_GBufferBinormal.Sample(g_GBufferSampler, vUV);

	vTangent.xyz = vTangent.xyz * 2.f - 1.f;
	vBinormal.xyz = vBinormal.xyz * 2.f - 1.f;

	float4	vVtxNormal = (float4)0;
	vVtxNormal.xyz = normalize(cross(vTangent.xyz, vBinormal.xyz));

	// Normal Texture가 있을 경우
	if (g_vMaterialEmissive.w == 1.f)
	{
		float4	vNormalMap = g_NormalMapTexture.Sample(g_DiffuseSampler, vScreenUV);

		float3	vNormal = vNormalMap.xyz * 2.f - 1.f;
		vNormal = normalize(vNormal);

		float3x3	mat =
		{
			vTangent.xyz,
			vBinormal.xyz,
			vVtxNormal.xyz
		};

		vNormal = mul(vNormal, mat);
		vNormal = normalize(vNormal) * 0.5f + 0.5f;

		output.vNormal.xyz = vNormal;
		output.vNormal.w = 1.f;
	}

	else
	{
		output.vNormal = vVtxNormal;
		output.vNormal.w = 1.f;
	}

	float4 vMtrlSpc = g_vMaterialSpecular;

	if (g_vMaterialAmbient.a != 0.f)
	{
		vMtrlSpc = g_SpecularMapTexture.Sample(g_DiffuseSampler, vScreenUV);
	}

	vMtrlSpc.a = g_vMaterialSpecular.a;

	output.vSpecular.r = ConvertColor(g_vMaterialDiffuse);
	output.vSpecular.g = ConvertColor(g_vMaterialAmbient);
	output.vSpecular.b = ConvertColor(vMtrlSpc);
	output.vSpecular.a = ConvertColor(g_vMaterialEmissive);

	return output;
}
