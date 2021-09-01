#include "Share.fx"

SamplerState		g_DistortionSampler : register(s1);
Texture2D			g_BackBufferTexture : register(t15);
Texture2D			g_GBufferDepth	: register(t17);

cbuffer	Trail	: register(b13)
{
	float3	g_vTrailCurrentPos;
	float	g_fTrailScale;
	float3	g_vTrailDirection;
	int		g_iTrailSize;
	float3	g_vTrailAxisY;
	float	g_fTrailEmpty;
	float4	g_vTrailVelocity[10];
}


VS_INPUT_POS TrailVS(VS_INPUT_POS input)
{
	VS_INPUT_POS	output = (VS_INPUT_POS)0;

	output.vPos = mul(float4(input.vPos, 1.0f), g_matWorld);

	return output;
}

//TriangleStream LineStream
// 크기가 제한이 되어 있땅 
[maxvertexcount(50)]
void TrailGS(point VS_INPUT_POS input[1],
	inout TriangleStream<VS_OUTPUT_TEX> stream)
{
	float3	vPos[20];

	for (int i = 0; i < 10; ++i)
	{
		vPos[i * 2] = g_vTrailVelocity[i];
		vPos[i * 2 + 1] = g_vTrailVelocity[i] + (g_vTrailDirection * g_fTrailScale);
	}

	float2	vUV[20];

	float	vUVPos = 0.f;


	for (int i = 0; i < 10; ++i)
	{
		vUV[i * 2] = float2(vUVPos, 0.f);
		vUV[i * 2 + 1] = float2(vUVPos, 1.f);
		vUVPos += 1.f / 10;
	}

	VS_OUTPUT_TEX	output = (VS_OUTPUT_TEX)0;
	for (int i = 0; i < 20; ++i)
	{
		output.vPos = mul(float4(vPos[i], 1.f), g_matVP);
		output.vProjPos = output.vPos;
		output.vUV = vUV[i];

		stream.Append(output);
	}
}

PS_OUTPUT_SINGLE TrailPS(VS_OUTPUT_TEX input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	float4	vColor;

	vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV) * g_vMaterialDiffuse;
	//vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV);

	if (vColor.a <= 0.f)
		clip(-1);

	if (vColor.r <= 0.f)
		clip(-1);

	output.vTarget = vColor;

	return output;
}



PS_OUTPUT_SINGLE TRAIL_DISTORTION_PS(VS_OUTPUT_TEX input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	// Bump 맵핑을 한 Normal값을 교란 값으로 이용한다...
	float4	vNormalMap = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV);

	float3	vNormal = vNormalMap.xyz * 2.f - 1.f;
	vNormal = normalize(vNormal);

	// 뷰포트(전체화면)의 uv를 구한다
	float2	vUV = input.vProjPos.xy;
	vUV /= input.vProjPos.w;
	vUV.x = vUV.x * 0.5f + 0.5f;
	vUV.y = vUV.y * -0.5f + 0.5f;

	vNormal.x = vNormal.x / 80.f;
	vNormal.y = vNormal.y / 45.f;

	float2 noiseCoords = vNormal.xy + vUV;

	float4 vDepth = g_GBufferDepth.Sample(g_DiffuseSampler, noiseCoords);

	// 뷰 공간에서의 깊이 차이
	float	fDepthDiff = vDepth.w - input.vProjPos.w;

	float4 vFinalColor = (float4)0;

	// 깊이비교시 뒤에있는 픽셀일 경우 
	if (fDepthDiff > 0.f
		|| vDepth.w == 0.f)
	{
		vFinalColor = g_BackBufferTexture.Sample(g_DistortionSampler, noiseCoords);
	}

	// 앞에 있는 픽셀일 경우 원본 백버퍼 출력..
	else
	{
		vFinalColor = g_BackBufferTexture.Sample(g_DiffuseSampler, vUV);
	}

	output.vTarget = vFinalColor;


	return output;
}