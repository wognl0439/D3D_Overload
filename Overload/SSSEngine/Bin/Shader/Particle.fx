#include "Share.fx"


Texture2D	g_GBufferDepth		: register(t17);


struct VS_INPUT_PARTICLE
{
	float3	vPos	: POSITION;
	float2	vSize	: SIZE;
	float		fAlpha : ALPHA;
	uint		iEnabled : ENABLED;
};

struct GS_OUTPUT_PARTICLE
{
	float4	vPos			: SV_POSITION;
	float4	vProjPos	: POSITION;
	float2	vUV			: TEXCOORD;
	float		fAlpha		: ALPHA;
};


cbuffer	Billboard	: register(b11)
{
	float3	g_vMeshCenter;
	float		g_fBillSizeX;
	float3	g_vCamAxisX;
	float		g_fBillSizeY;
	float3	g_vCamAxisY;
	float		g_fBillEmpty3;
}

VS_INPUT_PARTICLE ParticleVS(VS_INPUT_PARTICLE input)
{
	return input;
}

[maxvertexcount(4)]
void ParticleGS(point VS_INPUT_PARTICLE input[1],
	inout TriangleStream<GS_OUTPUT_PARTICLE> stream)
{
	if (input[0].iEnabled == 0)
		return;

	float	fHalfX = 0.5f * input[0].vSize.x;
	float	fHalfY = 0.5f * input[0].vSize.y;


	float2	vUV[4] =
	{
		float2(0.f, 1.f),
		float2(0.f, 0.f),
		float2(1.f, 1.f),
		float2(1.f, 0.f)
	};

	float3	vPos[4];
	vPos[0] = input[0].vPos - g_vCamAxisX * fHalfX - g_vCamAxisY * fHalfY;
	vPos[1] = input[0].vPos - g_vCamAxisX * fHalfX + g_vCamAxisY * fHalfY;
	vPos[2] = input[0].vPos + g_vCamAxisX * fHalfX - g_vCamAxisY * fHalfY;
	vPos[3] = input[0].vPos + g_vCamAxisX * fHalfX + g_vCamAxisY * fHalfY;
	
	GS_OUTPUT_PARTICLE	output = (GS_OUTPUT_PARTICLE)0;
	for (int i = 0; i < 4; ++i)
	{
		output.vProjPos = mul(float4(vPos[i], 1.f), g_matVP);
		output.vUV = vUV[i];
		output.vPos = output.vProjPos;
		output.fAlpha = input[0].fAlpha;
		stream.Append(output);
	}
}

PS_OUTPUT_SINGLE ParticlePS(GS_OUTPUT_PARTICLE input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;
	
	if (input.fAlpha <= 0.f)
		clip(-1);

	float4	vColor;
	vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV);

	if (vColor.a <= 0.4f)
		clip(-1);

	float2	vUV = input.vProjPos.xy;
	vUV /= input.vProjPos.w;
	vUV.x = vUV.x * 0.5f + 0.5f;
	vUV.y = vUV.y * -0.5f + 0.5f;
	float4 vDepth = g_GBufferDepth.Sample(g_DiffuseSampler, vUV);
	
	// 뷰 공간에서의 깊이 차이를 이용해서 알파값을 만들어낸다.
	float	fDepthDiff = vDepth.w - input.vProjPos.w;
	float	fMaxDist = 0.4f;
	
	if (vDepth.w == 0.f)
		fDepthDiff = fMaxDist;
	
	else if (fDepthDiff < 0.f)
		clip(-1);
	
	float fAlpha = fDepthDiff / fMaxDist;
	fAlpha = min(fAlpha, 1.f) * input.fAlpha;
	//vColor.a *= fAlpha;

	output.vTarget = vColor;

	return output;
}
