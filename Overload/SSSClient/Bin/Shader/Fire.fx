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


	// 3 ���� ���� �ٸ� �ؽ�ó ��ǥ�� ����Ͽ� ������ ������ �ؽ�ó�� ���ø��Ͽ� 3 ���� �ٸ� ������ �������� ����ϴ�.
	// �̶� UV�� 1�� ��������Ƿ� Texture Address WRAP ��� ����ؾ� �մϴ�
	noise1 = g_NormalMapTexture.Sample(g_DiffuseSampler, texCoords1);
	noise2 = g_NormalMapTexture.Sample(g_DiffuseSampler, texCoords2);
	noise3 = g_NormalMapTexture.Sample(g_DiffuseSampler, texCoords3);

	// (0, 1) �������� (-1, +1) ������ ����� �̵��մϴ�.
	noise1 = (noise1 - 0.5f) * 2.0f;
	noise2 = (noise2 - 0.5f) * 2.0f;
	noise3 = (noise3 - 0.5f) * 2.0f;

	// 3 ���� ���� �ٸ� �ְ� x�� y ������ �� ���� ������ x�� y ��ǥ�� �ְ��մϴ�.
	noise1.xy = noise1.xy * distortion1.xy;
	noise2.xy = noise2.xy * distortion2.xy;
	noise3.xy = noise3.xy * distortion3.xy;

	// �� ���� �ְ� �� ������ ����� ��� �ϳ��� ������ ����� �����մϴ�.
	finalNoise = noise1 + noise2 + noise3;

	// �ְ� ������ �� ���̾ ���� ���� �Է� �ؽ�ó Y ��ǥ�� �ְ��մϴ�.
	// ���� ȿ������ �Ҳ��� ���� �Ÿ��� �ؽ�ó�� ���� �����̸� ������ �������ϴ�.
	perturb = ((1.0f - tex.y) * distortionScale) + distortionBias;

	// ���� ȭ�� ���� �ؽ�ó�� ���ø��ϴ� �� ����� �����ǰ� �ְ� �� �ؽ�ó ���ø� ��ǥ�� ����ϴ�.
	noiseCoords.xy = (finalNoise.xy * perturb) + tex.xy;

	noiseCoords.y += OffsetY;

	// �����ǰ� �ְ� �� �ؽ�ó ���ø� ��ǥ�� ����Ͽ� ȭ�� �ؽ�ó���� ������ ���ø��մϴ�.
	// ȭ���� ���¸� ���δ� ��� Ŭ���� ���� ���¸� ����Ͽ� ȭ���� ���δ� ���� �����մϴ�.		
	fireColor = g_DiffuseTexture.Sample(g_SamplerClamp, noiseCoords.xy);

	// �����ǰ� �ְ� �� �ؽ�ó ���ø� ��ǥ�� ����Ͽ� ���� �ؽ�ó���� ���� ���� ���ø��մϴ�.
	// �̰��� ���� ������ ���˴ϴ�.
	// ȭ���� ���¸� ���δ� ��� Ŭ���� ���� ���¸� ����Ͽ� ȭ���� ���δ� ���� �����մϴ�.
	alphaColor = g_AlphaMapTexture.Sample(g_SamplerClamp, noiseCoords.xy);

	// ȭ���� ���� ������ �Ҿ����ϰ� �ְ� �� ���� �ؽ�ó ������ �����մϴ�.
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

	// ù ��° ������ �� ���� ��ũ�Ѹ� �ӵ� ���� ����Ͽ� ù ��° ������ �ؽ�ó�� ���� �ؽ�ó ��ǥ�� ����մϴ�.
	output.vNoiseUV1 = (output.vUV * scales.x);
	output.vNoiseUV1.y = output.vNoiseUV1.y + (scrollSpeeds.x) + g_fFireRandomSpeed;

	// �� ��° ������ �� ���� ��ũ�Ѹ� �ӵ� ���� ����Ͽ� �� ��° ������ �ؽ�ó�� �ؽ�ó ��ǥ�� ����մϴ�.
	output.vNoiseUV2 = (output.vUV * scales.y);
	output.vNoiseUV2.y = output.vNoiseUV2.y + (scrollSpeeds.y) + g_fFireRandomSpeed;

	// �� ��° ������ �� ���� ��ũ�Ѹ� �ӵ� ���� ����Ͽ� �� ��° ������ �ؽ�ó�� �ؽ�ó ��ǥ�� ����մϴ�.
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

		// ù ��° ������ �� ���� ��ũ�Ѹ� �ӵ� ���� ����Ͽ� ù ��° ������ �ؽ�ó�� ���� �ؽ�ó ��ǥ�� ����մϴ�.
		output.vNoiseUV1 = (output.vUV * scales.x);
		output.vNoiseUV1.y = output.vNoiseUV1.y + (scrollSpeeds.x);

		// �� ��° ������ �� ���� ��ũ�Ѹ� �ӵ� ���� ����Ͽ� �� ��° ������ �ؽ�ó�� �ؽ�ó ��ǥ�� ����մϴ�.
		output.vNoiseUV2 = (output.vUV * scales.y);
		output.vNoiseUV2.y = output.vNoiseUV2.y + (scrollSpeeds.y);

		// �� ��° ������ �� ���� ��ũ�Ѹ� �ӵ� ���� ����Ͽ� �� ��° ������ �ؽ�ó�� �ؽ�ó ��ǥ�� ����մϴ�.
		output.vNoiseUV3 = (output.vUV * scales.z);
		output.vNoiseUV3.y = output.vNoiseUV3.y + (scrollSpeeds.z);


		stream.Append(output);
	}
}

PS_OUTPUT_SINGLE FirePS(VS_OUTPUT_FIRE input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	float4 vAlbedo = ComputeFire(input.vUV, input.vNoiseUV1, input.vNoiseUV2, input.vNoiseUV3);

	// ����Ʈ ��ƼŬ ó���� �Ѵ�.
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
