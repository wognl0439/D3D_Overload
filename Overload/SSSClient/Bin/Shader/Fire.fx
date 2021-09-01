#include "Share.fx"


Texture2D		g_GBufferDepth	: register(t17);
SamplerState	g_SamplerClamp : register(s1);


struct VS_OUTPUT_FIRE
{
	float4	vPos	: SV_POSITION;
	float2	vUV		: TEXCOORD;
	float4	vProjPos	: POSITION1;
	float2	vNoiseUV1 : TEXCOORD1;
	float2	vNoiseUV2 : TEXCOORD2;
	float2	vNoiseUV3 : TEXCOORD3;
};

cbuffer	FireCBuffer	: register(b5)
{
	float g_fFireRandomSpeed;
	float3 scrollSpeeds;
	float3 scales;
	float OffsetY;
	float2 distortion1;
	float2 distortion2;
	float2 distortion3;
	float distortionScale;
	float distortionBias;
}



cbuffer	Billboard	: register(b11)
{
	float3	g_vMeshCenter;
	float		g_fBillSizeX;
	float3	g_vCamAxisX;
	float		g_fBillSizeY;
	float3	g_vCamAxisY;
	float		g_fBillEmpty3;
}


float4 ComputeFire(float2 tex, float2 texCoords1, float2 texCoords2, float2 texCoords3)
{
	float4 noise1;
	float4 noise2;
	float4 noise3;
	float4 finalNoise;
	float perturb;
	float2 noiseCoords;
	float4 fireColor;
	float4 alphaColor;


	// 3 개의 서로 다른 텍스처 좌표를 사용하여 동일한 노이즈 텍스처를 샘플링하여 3 개의 다른 노이즈 스케일을 얻습니다.
	// 이때 UV가 1을 벗어나있으므로 Texture Address WRAP 모드 사용해야 합니다
	noise1 = g_NormalMapTexture.Sample(g_DiffuseSampler, texCoords1);
	noise2 = g_NormalMapTexture.Sample(g_DiffuseSampler, texCoords2);
	noise3 = g_NormalMapTexture.Sample(g_DiffuseSampler, texCoords3);

	// (0, 1) 범위에서 (-1, +1) 범위로 노이즈를 이동합니다.
	noise1 = (noise1 - 0.5f) * 2.0f;
	noise2 = (noise2 - 0.5f) * 2.0f;
	noise3 = (noise3 - 0.5f) * 2.0f;

	// 3 개의 서로 다른 왜곡 x와 y 값으로 세 개의 노이즈 x와 y 좌표를 왜곡합니다.
	noise1.xy = noise1.xy * distortion1.xy;
	noise2.xy = noise2.xy * distortion2.xy;
	noise3.xy = noise3.xy * distortion3.xy;

	// 세 가지 왜곡 된 노이즈 결과를 모두 하나의 노이즈 결과로 결합합니다.
	finalNoise = noise1 + noise2 + noise3;

	// 왜곡 스케일 및 바이어스 값에 의해 입력 텍스처 Y 좌표를 왜곡합니다.
	// 위쪽 효과에서 불꽃이 깜박 거리는 텍스처를 위로 움직이면 섭동이 강해집니다.
	perturb = ((1.0f - tex.y) * distortionScale) + distortionBias;

	// 이제 화재 색상 텍스처를 샘플링하는 데 사용할 교란되고 왜곡 된 텍스처 샘플링 좌표를 만듭니다.
	noiseCoords.xy = (finalNoise.xy * perturb) + tex.xy;

	noiseCoords.y += OffsetY;

	// 섭동되고 왜곡 된 텍스처 샘플링 좌표를 사용하여 화재 텍스처에서 색상을 샘플링합니다.
	// 화스랩 상태를 감싸는 대신 클램프 샘플 상태를 사용하여 화염을 감싸는 것을 방지합니다.		
	fireColor = g_DiffuseTexture.Sample(g_SamplerClamp, noiseCoords.xy);

	// 교란되고 왜곡 된 텍스처 샘플링 좌표를 사용하여 알파 텍스처에서 알파 값을 샘플링합니다.
	// 이것은 불의 투명도에 사용됩니다.
	// 화스랩 상태를 감싸는 대신 클램프 샘플 상태를 사용하여 화염을 감싸는 것을 방지합니다.
	alphaColor = g_AlphaMapTexture.Sample(g_SamplerClamp, noiseCoords.xy);

	// 화재의 알파 블렌딩을 불안정하고 왜곡 된 알파 텍스처 값으로 설정합니다.
	fireColor.a = alphaColor;

	return fireColor;
}


VS_OUTPUT_FIRE FireVS(VS_INPUT_TEX input)
{
	VS_OUTPUT_FIRE	output = (VS_OUTPUT_FIRE)0;

	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;

	output.vProjPos = mul(float4(vPos, 1.f), g_matWVP);
	output.vPos = output.vProjPos;

	output.vUV = input.vUV;

	// 첫 번째 스케일 및 위쪽 스크롤링 속도 값을 사용하여 첫 번째 노이즈 텍스처에 대한 텍스처 좌표를 계산합니다.
	output.vNoiseUV1 = (output.vUV * scales.x);
	output.vNoiseUV1.y = output.vNoiseUV1.y + (scrollSpeeds.x) + g_fFireRandomSpeed;

	// 두 번째 스케일 및 위쪽 스크롤링 속도 값을 사용하여 두 번째 노이즈 텍스처의 텍스처 좌표를 계산합니다.
	output.vNoiseUV2 = (output.vUV * scales.y);
	output.vNoiseUV2.y = output.vNoiseUV2.y + (scrollSpeeds.y) + g_fFireRandomSpeed;

	// 세 번째 스케일 및 위쪽 스크롤링 속도 값을 사용하여 세 번째 노이즈 텍스처의 텍스처 좌표를 계산합니다.
	output.vNoiseUV3 = (output.vUV * scales.z);
	output.vNoiseUV3.y = output.vNoiseUV3.y + (scrollSpeeds.z) + g_fFireRandomSpeed;

	return output;
}


VS_OUTPUT_POS BillboardFireVS(VS_INPUT_POS input)
{
	VS_OUTPUT_POS	output = (VS_OUTPUT_POS)0;
	return output;
}


[maxvertexcount(4)]
void BillboardFireGS(point VS_OUTPUT_POS input[1],
	inout TriangleStream<VS_OUTPUT_FIRE> stream)
{
	float	fHalfX = 0.5f * g_fBillSizeX;
	float	fHalfY = 0.5f * g_fBillSizeY;

	float3	vPos[4];
	vPos[0] = g_vMeshCenter - g_vCamAxisX * fHalfX - g_vCamAxisY * fHalfY;
	vPos[1] = g_vMeshCenter - g_vCamAxisX * fHalfX + g_vCamAxisY * fHalfY;
	vPos[2] = g_vMeshCenter + g_vCamAxisX * fHalfX - g_vCamAxisY * fHalfY;
	vPos[3] = g_vMeshCenter + g_vCamAxisX * fHalfX + g_vCamAxisY * fHalfY;

	float2	vUV[4] =
	{
		float2(0.f, 1.f),
		float2(0.f, 0.f),
		float2(1.f, 1.f),
		float2(1.f, 0.f)
	};
	/*float2	vUV[4] =
	{
		float2(0.f, 0.f),
		float2(1.f, 0.f),
		float2(0.f, 1.f),
		float2(1.f, 1.f)
	};*/

	VS_OUTPUT_FIRE	output = (VS_OUTPUT_FIRE)0;
	for (int i = 0; i < 4; ++i)
	{
		output.vProjPos = mul(float4(vPos[i], 1.f), g_matVP);
		output.vUV = vUV[i];
		output.vPos = output.vProjPos;

		// 첫 번째 스케일 및 위쪽 스크롤링 속도 값을 사용하여 첫 번째 노이즈 텍스처에 대한 텍스처 좌표를 계산합니다.
		output.vNoiseUV1 = (output.vUV * scales.x);
		output.vNoiseUV1.y = output.vNoiseUV1.y + (scrollSpeeds.x);

		// 두 번째 스케일 및 위쪽 스크롤링 속도 값을 사용하여 두 번째 노이즈 텍스처의 텍스처 좌표를 계산합니다.
		output.vNoiseUV2 = (output.vUV * scales.y);
		output.vNoiseUV2.y = output.vNoiseUV2.y + (scrollSpeeds.y);

		// 세 번째 스케일 및 위쪽 스크롤링 속도 값을 사용하여 세 번째 노이즈 텍스처의 텍스처 좌표를 계산합니다.
		output.vNoiseUV3 = (output.vUV * scales.z);
		output.vNoiseUV3.y = output.vNoiseUV3.y + (scrollSpeeds.z);


		stream.Append(output);
	}
}

PS_OUTPUT_SINGLE FirePS(VS_OUTPUT_FIRE input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	float4 vAlbedo = ComputeFire(input.vUV, input.vNoiseUV1, input.vNoiseUV2, input.vNoiseUV3);

	// 소프트 파티클 처리를 한다.
	float2	vUV = input.vProjPos.xy;
	vUV /= input.vProjPos.w;
	vUV.x = vUV.x * 0.5f + 0.5f;
	vUV.y = vUV.y * -0.5f + 0.5f;
	float4 vDepth = g_GBufferDepth.Sample(g_DiffuseSampler, vUV);

	float	fDepthDiff = vDepth.w - input.vProjPos.w;
	float	fMaxDist = 0.4f;

	if (vDepth.w == 0.f)
		fDepthDiff = fMaxDist;

	else if (fDepthDiff < 0.f)
		clip(-1);

	float fAlpha = fDepthDiff / fMaxDist;
	fAlpha = min(fAlpha, 1.f);
	vAlbedo.a *= fAlpha;
	
	output.vTarget = vAlbedo;

	if (output.vTarget.a <= 0.4f)
		clip(-1);

	return output;
}
