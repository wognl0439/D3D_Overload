#include "Share.fx"

SamplerState		g_DistortionSampler : register(s1);
Texture2D			g_BackBufferTexture : register(t15);

Texture2D		g_GBufferDepth			: register(t17);
Texture2D		g_AnimationFrameTexture	: register(t8);
#define	ANIMATION_NONE		0
#define	ANIMATION_2D			1
#define ANIMATION_3D			2
#define ANIMATION_BOTH		3

cbuffer	Billboard	: register(b11)
{
	float3	g_vMeshCenter;
	float		g_fBillSizeX;
	float3	g_vCamAxisX;
	float		g_fBillSizeY;
	float3	g_vCamAxisY;
	float		g_fBillEmpty3;
}

VS_OUTPUT_POS BillboardVS(VS_INPUT_POS input)
{
	VS_OUTPUT_POS	output = (VS_OUTPUT_POS)0;
	return output;
}

[maxvertexcount(4)]
void BillboardGS(point VS_OUTPUT_POS input[1],
	inout TriangleStream<VS_OUTPUT_TEX> stream)
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
	
	VS_OUTPUT_TEX	output = (VS_OUTPUT_TEX)0;
	for (int i = 0; i < 4; ++i)
	{
		output.vProjPos = mul(float4(vPos[i], 1.f), g_matVP);
		//output.vUV = ComputeAnimation2DUV(vUV[i]);
		output.vUV = vUV[i];
		output.vPos = output.vProjPos;

		stream.Append(output);
	}
}


PS_OUTPUT_SINGLE BillboardPS(VS_OUTPUT_TEX input)
{
	// Alpha Texture를 사용할때
	if (g_vMaterialAlpha.w == 1.f)
	{
		float3	vAlpha = g_AlphaMapTexture.Sample(g_DiffuseSampler, input.vUV);

		uint3	vTempAlpha;
		vTempAlpha = asuint(vAlpha * 10000.f);
		vAlpha = asfloat(vTempAlpha * 0.0001f);

		if (vAlpha.r == g_vMaterialAlpha.r)
			clip(-1);
	}

	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	// Distortion 사용하는 Billboard 인 경우 들어옴..
	if (g_fBillEmpty3 == 1.f)
	{
		// Bump 맵핑을 한 Normal값을 교란 값으로 이용한다...
		float4	vNormalMap = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV);

		float3	vNormal = vNormalMap.xyz * 2.f - 1.f;
		vNormal = normalize(vNormal);

		vNormal = vNormal * g_fNormalScale;

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
			//vFinalColor = g_BackBufferTexture.Sample(g_DiffuseSampler, noiseCoords);
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
	else
	{
		float4	vColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
		output.vTarget = vColor;

		//Soft Particle 포함
		if (g_iHasAnimation & ANIMATION_2D)
		{
			//float4 vSampleFrame = g_AnimationFrameTexture.Load(int3(0, 1, 0));
			//float4 vSampleAtlas = g_AnimationFrameTexture.Load(int3(1, 1, 0));

			float2 vAnimationUV = ComputeAnimation2DUV(input.vUV);
			vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, vAnimationUV) * g_vMaterialDiffuse;
		}
		else
		{
			vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV) * g_vMaterialDiffuse;
		}

		// clip 함수에 -1을 넣으면 픽셀도 안쓰고 깊이버퍼에 깊이값도 안쓰게 된다.
		if (vColor.a == 0.f)
			clip(-1);
		if (vColor.r == 0.f)
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
		fAlpha = min(fAlpha, 1.f);
		vColor.a *= fAlpha;

		output.vTarget = vColor;
	}

	return output;
}
