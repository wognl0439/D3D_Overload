#include "Share.fx"

TextureCube		g_CubeTex	: register(t10);

cbuffer WaterCBuffer : register(b6)
{
	int		g_iCameraPos;
	float	g_fWaterSpeed;
	float2  g_vWaterEmpty;
}


struct VS_OUTPUT_WATER
{
	float4	vPos	: SV_POSITION;
	float3	vNormal	: NORMAL;
	float2	vUV	: TEXCOORD0;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float3	vViewPos	: POSITION1;
	float4	vProjPos	: POSITION2;
};


VS_OUTPUT_WATER WaterVS(VS_INPUT_BUMP input)
{
	VS_OUTPUT_WATER	output = (VS_OUTPUT_WATER)0;

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


PS_OUTPUT_SINGLE WaterPS(VS_OUTPUT_WATER input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	float2 vNewUV = input.vUV;

	vNewUV.x += g_fWaterSpeed * 0.2f;

	// 교란 맵 적용 
	float4	vderangement = g_SpecularMapTexture.Sample(g_DiffuseSampler, vNewUV);

	vderangement = vderangement * 2.f - 1.f;

	vderangement.y += g_fWaterSpeed * 0.1f;

	float4	vNormalMap = g_NormalMapTexture.Sample(g_DiffuseSampler, vderangement.xy);

	float3	vNormal = vNormalMap.xyz * 2.f - 1.f;
	vNormal = normalize(vNormal);

	float3x3	mat =
	{
		input.vTangent,
		input.vBinormal,
		input.vNormal
	};

	vNormal = mul(vNormal, mat);
	vNormal = normalize(vNormal);

	// 반사벡터를 구한다.
	// 카메라에서 픽셀 위치를 바라보는 벡터를 구한다.
	float3	vDir = -normalize(input.vViewPos);
	float fCos = dot(input.vNormal, vDir);
	float3	vReflect = 2.f * input.vNormal * fCos - vDir;
	vReflect = normalize(vReflect);

	// 반사벡터를 로컬공간으로 변환한다.
	vReflect = normalize(mul(float4(vReflect, 0.f), g_matInvWV));

	// 굴절벡터를 구한다
	float3 vRefract = refract(-vDir, -input.vNormal, 0.99f);
	vRefract = normalize(vRefract);
	vRefract = normalize(mul(float4(vRefract, 0.f), g_matInvWV));

	_tagLightInfo	tMtrl = ComputeLight(input.vViewPos, vNormal);

	float4	vAlbedo = g_vMaterialDiffuse;

	output.vTarget.rgb = vAlbedo.rgb * (tMtrl.vDiffuse + tMtrl.vAmbient).rgb + tMtrl.vSpecular.rgb;

	output.vTarget.a = vAlbedo.a;

	float4 vReflection = g_CubeTex.Sample(g_DiffuseSampler, vReflect);

	float4 vRefraction = g_CubeTex.Sample(g_DiffuseSampler, vRefract);

	float4 vRefracColor = (vRefraction * (1.f - 0.5f)) + (output.vTarget * 0.5f);

	float fCoefficient = acos(fCos);

	// 물 위에 카메라가 있을 경우 
	if (g_iCameraPos == 0)
		fCoefficient = 3.14f - fCoefficient;

	fCoefficient = max(0.f, saturate(fCoefficient - 0.5f));

	float4 vFinalColor = (vRefracColor * (1.f - fCoefficient)) + (vReflection * fCoefficient);

	vFinalColor.a = fCoefficient;

	output.vTarget = vFinalColor;

	if (output.vTarget.a <= 0.f)
		clip(-1);

	return output;
}
