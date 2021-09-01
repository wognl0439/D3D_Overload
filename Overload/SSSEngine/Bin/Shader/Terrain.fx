#include "Share.fx"


cbuffer LandScape	: register(b11)
{
	float	g_fDetailLevel;
	int		g_iSplatCount;
	float2	g_vMousePosition;
	float	g_fRadius;
	float3	g_vEmpty3;
}

Texture2DArray	g_SplatDiffuse	: register(t6);
Texture2DArray	g_SplatNormal		: register(t7);
Texture2DArray	g_SplatSpecular		: register(t8);
Texture2DArray	g_SplatAlpha	: register(t9);

SamplerState	g_SplatSampler	 : register(s10);


VS_OUTPUT_BUMP TerrainVS(VS_INPUT_BUMP input)
{
	VS_OUTPUT_BUMP	output = (VS_OUTPUT_BUMP)0;

	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;

	output.vProjPos = mul(float4(vPos, 1.f), g_matWVP);
	output.vPos = output.vProjPos;
	// Normal은 뷰공간으로 만들어준다.
	output.vNormal = mul(float4(input.vNormal, 0.f), g_matWV).xyz;
	output.vNormal = normalize(output.vNormal);
	output.vTangent = mul(float4(input.vTangent, 0.f), g_matWV).xyz;
	output.vTangent = normalize(output.vTangent);
	output.vBinormal = mul(float4(input.vBinormal, 0.f), g_matWV).xyz;
	output.vBinormal = normalize(output.vBinormal);
	output.vViewPos = mul(float4(vPos, 1.f), g_matWV).xyz;
	output.vUV = input.vUV;

	return output;
}

PS_OUTPUT  TerrainPS(VS_OUTPUT_BUMP input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	float2	vDetailUV = input.vUV * g_fDetailLevel;
	float4	vAlbedo = g_DiffuseTexture.Sample(g_DiffuseSampler, vDetailUV);

	// Bump Mapping
	float4	vNormalMap = g_NormalMapTexture.Sample(g_DiffuseSampler, vDetailUV);

	float3	vNormal = vNormalMap.xyz * 2.f - 1.f;
	vNormal = normalize(vNormal);

	// Albedo와 Normal을 스플래팅 한다.
	for (int i = 0; i < g_iSplatCount; ++i)
	{
		// Alpha 텍스쳐에서 색상정보를 얻어온다.
		float3	vUVW = float3(input.vUV, i);
		float4	vAlpha = g_SplatAlpha.Sample(g_SplatSampler, vUVW);

		vUVW = float3(vDetailUV, i);
		float4	vSplatAlbedo = g_SplatDiffuse.Sample(g_SplatSampler, vUVW);
		float4	vSplatNormal = g_SplatNormal.Sample(g_SplatSampler, vUVW);
		float3	vSplatNrm = normalize(vSplatNormal.xyz * 2.f - 1.f);


		vAlbedo.rgb = vAlbedo.rgb * (float3)(1.f - vAlpha.r) +
			vSplatAlbedo.rgb * (float3)vAlpha.r;
		vNormal.rgb = vNormal.rgb * (1.f - vAlpha.r) +
			vSplatNrm * vAlpha.r;
	}

	vNormal = normalize(vNormal);

	float3x3	mat =
	{
		input.vTangent,
		input.vBinormal,
		input.vNormal
	};

	vNormal = mul(vNormal, mat);
	vNormal = normalize(vNormal);

	vNormal = vNormal * g_fNormalScale;

	if (g_iRenderMode == 0)
	{
		_tagLightInfo	tMtrl = ComputeLight(input.vViewPos, vNormal);

		output.vTarget0.rgb = vAlbedo.rgb * (tMtrl.vDiffuse + tMtrl.vAmbient).rgb + tMtrl.vSpecular.rgb;
		output.vTarget0.a = vAlbedo.a;
	}

	else
	{
		// Albedo
		output.vTarget0.rgb = vAlbedo.rgb;
		output.vTarget0.a = vAlbedo.a;

		// Normal
		output.vTarget1.rgb = vNormal * 0.5f + 0.5f;
		output.vTarget1.a = 1.f;

		output.vTarget2.rgb = (float3)(input.vProjPos.z / input.vProjPos.w);
		output.vTarget2.a = input.vProjPos.w;

		// Specular Target
		// 색상 변환
		float4	vMtrlSpc = g_vMaterialSpecular;

		if (g_vMaterialAmbient.a != 0.f)
		{
			vMtrlSpc = g_SpecularMapTexture.Sample(g_DiffuseSampler, vDetailUV);

			// Specular를 스플래팅 한다.
			for (int i = 0; i < g_iSplatCount; ++i)
			{
				// Alpha 텍스쳐에서 색상정보를 얻어온다.
				float3	vUVW = float3(input.vUV, i);
				float4	vAlpha = g_SplatAlpha.Sample(g_SplatSampler, vUVW);

				vUVW = float3(vDetailUV, i);
				float4	vSplatSpc = g_SplatSpecular.Sample(g_SplatSampler, vUVW);

				vMtrlSpc.rgb = vMtrlSpc.rgb * (1.f - vAlpha.r) +
					vSplatSpc.rgb * vAlpha.r;
			}
		}

		vMtrlSpc.a = g_vMaterialSpecular.a;

		output.vTarget3.r = ConvertColor(g_vMaterialDiffuse);
		output.vTarget3.g = ConvertColor(g_vMaterialAmbient);
		output.vTarget3.b = ConvertColor(vMtrlSpc);
		output.vTarget3.a = ConvertColor(g_vMaterialEmissive);;

		output.vTarget4.xyz = input.vTangent * 0.5f + 0.5f;
		output.vTarget5.xyz = input.vBinormal * 0.5f + 0.5f;
		output.vTarget4.w = 1.f;
		output.vTarget5.w = 1.f;
	}

	if (g_fRadius != 0)
	{
		float2 Result = g_vMousePosition - input.vUV;
		float	fDist = length(Result);

		if (fDist < g_fRadius && fDist > g_fRadius - 0.01f * g_fRadius)
		{
			output.vTarget0.rgba = float4(1, 1, 1, 1);

			return output;
		}
	}

	return output;
}
