#include "Share.fx"

//#define LUMINANCE_FACTOR float4(0.299f, 0.587f, 0.114f, 0.0f)
#define LUMINANCE_FACTOR float4(0.27f, 0.67f, 0.06f, 0.0f)
#define DELTA 0.00000001f
#define EXPOSURE 0.08f

SamplerState		g_GBufferSampler		: register(s11);
SamplerState		g_HDRSampler		: register(s12);
Texture2D			g_GBufferAlbedo		: register(t11);
Texture2D			g_GBufferNormal		: register(t12);
Texture2D			g_GBufferDepth			: register(t13);
Texture2D			g_GBufferSpecular		: register(t14);
Texture2D			g_LightDif					: register(t15);
Texture2D			g_LightSpc					: register(t16);

Texture2D			g_MotionBlurDepthTarget	: register(t17);
Texture2D			g_ShadowMap				: register(t18);

Texture2D			g_DownScaleTex				: register(t19);
Texture2D			g_DownScaleTex2				: register(t20);
Texture2D			g_PrevAvgLumTex				: register(t21);

static const float2 vPos[4] =
{
	float2(-1.0f, 1.0f),
	float2(1.0f, 1.0f),
	float2(-1.0f, -1.0f),
	float2(1.0f, -1.0f),
};

static const float2 vUV[4] =
{
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f),
	float2(0.0f, 1.0f),
	float2(1.0f, 1.0f),
};

cbuffer	ShadowRendering	: register(b3)
{
	int		g_iShadowEnable;
	float	g_fShadowBias;
	float2	g_vShadowResolution;
	float3	g_vShadowLightPos;
	float	g_vShadowEmpty;
}

struct PS_OUTPUT_LIGHTACC
{
	float4 vDiffuse		:SV_TARGET0;
	float4 vSpecular		:SV_TARGET1;
};


float GaussianDistribution(uint iSampleCount, uint iIndex, float fSigma)
{
	float fContainer[64] = {
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
	};

	float fResult = 0;
	[unroll]
	for (uint i = 0; i < iSampleCount; ++i)
	{
		fContainer[i] = exp(-1.0f * ((i * i) / (2 * fSigma * fSigma)));
		fResult += fContainer[i];
	}

	[unroll]
	for (uint j = 0; j < iSampleCount; ++j)
	{
		fContainer[j] = fContainer[j] / fResult;
	}

	return fContainer[iIndex];
}



///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////HDR/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

//Compute Resources
Texture2D<float4> InputRenderTarget : register( t19 );
RWStructuredBuffer<float> AverageLuminance : register(u0);
groupshared float2 avgTemp[720];
[numthreads(720, 1, 1)]
void AverageLuminanceCS(uint3 dispatchThreadID : SV_DispatchThreadID, uint3 groupThreadID : SV_GroupThreadID, uint3 groupID : SV_GroupID)
{
	float fHorizontalPixelUnit	= 0.00078125f;
	float fVerticalPixelUnit = 0.00138889f;
	//720개로 1280 * 720개의  픽셀을 돌아야 한다.
	//일단은 720개로 1280번 돌아서 값이 나오는지 확인해보자
	for (int i = 0; i < 1280; ++i)
	{
		//x축은 i이고, y 축은 groupThreadID.x 이다.
		//인덱스는 y * 1280 + x 이므로 Load할 수 있다.
		uint iIndex = groupThreadID.x * 1280 + i;
		float2 vUV = float2(i *fHorizontalPixelUnit, groupThreadID.x *fVerticalPixelUnit);
		float4 vColor = InputRenderTarget.SampleLevel(g_GBufferSampler, vUV, 0);

		//가로 방향 픽셀들의 색상 누적
		if (vColor.a != 0)
		{
			float fLuminance = log(dot(vColor, LUMINANCE_FACTOR) + DELTA);
			avgTemp[groupThreadID.x].x += fLuminance;
			//알파는 클리핑 되어버리는 픽셀의 갯수를 넣어준다.
			//avgTemp[groupThreadID.x].a += 1.0f;
		}
		else
		{
			avgTemp[groupThreadID.x].y += 1.0f;
		}
	}

	//모든 픽셀들의 색상을 모을 때 까지 기다린다.
	GroupMemoryBarrierWithGroupSync();

	//다 모았으면 한곳에 모은다.
	float2 fLuminance = (float2)0;

	//0번 스레드를 사용한다.
	if (groupThreadID.x == 0)
	{
		for (uint i = 0; i < 720; ++i)
		{
			fLuminance.x += avgTemp[i].x;
			fLuminance.y += avgTemp[i].y;
		}
		//후처리는 CPU에서 한번만 하도록
		//CPU로 넘겨준다.
		float fAverage = fLuminance.x / (fLuminance.y);

		fAverage = EXPOSURE / (exp(fAverage) - DELTA);

		AverageLuminance[groupThreadID.x] = max(0.0f, fAverage);
	}
}



float4 ToneMapping(float4 vOriginColor, float2 vUV)
{
	float3 Lw = vOriginColor;
	
	// LIGHT BLOOM
	if (g_iPostProcessingFlags & 0x0004)
	{
		float3 vBloom = g_DownScaleTex.SampleLevel(g_HDRSampler, vUV, 0).xyz;

		Lw += 0.6f * vBloom;
	}

	float avgLum = g_DownScaleTex2.Load(float3(0, 0, 0)).r;
	//avgLum = 1.f - avgLum;
	float PrevAvgLum = g_PrevAvgLumTex.Load(float3(0, 0, 0)).r;

	float LScale = dot(Lw, LUMINANCE_FACTOR);
	LScale *= g_fPostProcessingHDRExposure / avgLum;
	LScale = (LScale + LScale * LScale / g_fPostProcessingHDRWhiteThreshold) / (1.f + LScale);
	Lw *= LScale;

	return float4(Lw, vOriginColor.a);
}

//float4 ToneMapping(float4 vOriginColor)
//{
//	float4 vColor= pow(vOriginColor, 2.2f);
//	vColor = float4( g_fPostProcessingHDRAverageLuminance * g_fPostProcessingHDRExposure * vColor.rgb, 1.0f);
//	vColor.rgb = (vColor.rgb / (1 + vColor.rgb)) ;
//	//float fSquare = g_fPostProcessingHDRAverageLuminance * g_fPostProcessingHDRAverageLuminance;
//	//float fScale = (45 * fSquare) / (35 * fSquare - 5 * g_fPostProcessingHDRAverageLuminance + 5);
//	vColor.rgb = pow(vColor.rgb , 1.0f / 2.2f);
//	vColor.a = 1.0f;
//	return vColor;	
//}

//float4 ToneMapping(float4 vOriginColor)
//{
//	float4 vColor= pow(vOriginColor, 2.2f);
//	vColor = float4( g_fPostProcessingHDRAverageLuminance * g_fPostProcessingHDRExposure * vColor.rgb, 1.0f);
//	vColor.rgb = (vColor.rgb / (1 + vColor.rgb)) ;
//	//float fSquare = g_fPostProcessingHDRAverageLuminance * g_fPostProcessingHDRAverageLuminance;
//	//float fScale = (45 * fSquare) / (35 * fSquare - 5 * g_fPostProcessingHDRAverageLuminance + 5);
//	vColor.rgb = pow(vColor.rgb , 1.0f / 2.2f);
//	vColor.a = 1.0f;
//	return vColor;	
//}

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////HDR/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


float4 ComputeMotionBlur(float3 vViewPos, float2 vUV, float4 vOriginColor)
{
	// // g_GBufferDepth
	float4	vBlurDepth;

	// Object Motion Blur
	//if (g_iMotionBlurMode == 0)
	//	vBlurDepth = g_MotionBlurDepthTarget.Sample(g_GBufferSampler, vUV);

	// Camera Motion Blur
	//else
	// 우선은 Alpha 오브젝트 깊이 정보가 없는 관계로, Default 설정으로 카메라모션블러만 사용하게 하겠습니다..
	vBlurDepth = g_GBufferDepth.Sample(g_GBufferSampler, vUV);

	// 현재 픽셀이 Alpha 그룹 또는 SkyBox 일때
	if (vBlurDepth.w == 0.f)
	{
		//float4 vPrev = mul(float4(vUV, 0.f, 1.f), g_matPreViewProj);
		//float4 vPrevUV = vPrev / vPrev.w;
		////vPrevUV = normalize(vPrevUV);

		//float4 vCurrent = mul(float4(vUV, 0.f, 1.f), g_matVP);
		//float4 vCurrentUV = vCurrent / vCurrent.w;
		////vCurrentUV = normalize(vCurrentUV);

		//float2 velocity = (vCurrentUV - vPrevUV);
		//float fClampDelta = 0.005f;
		//velocity = clamp(velocity, -fClampDelta, fClampDelta);

		//int inewCount;
		//inewCount = 5;
		//velocity = velocity / inewCount;

		//velocity.x = -velocity.x;

		//float4 color = vOriginColor;

		//float2 vNewUV = vUV;

		//int SampleCount = 1;
		//for (int i = 1; i < inewCount; ++i)
		//{
		//	vNewUV = vNewUV + velocity;

		//	float4 vAlbedo = g_GBufferAlbedo.Sample(g_GBufferSampler, vNewUV);

		//	color += vAlbedo;
		//	++SampleCount;
		//}

		//color = color / inewCount;

		//float4 vOutput = float4(color.rgb, 1.f);

		//return vOutput;
		
		// Depth 가 없는 영역은 따로 처리하거나 처리하지 않는 방향으로..
		// 가장유력한 방법은 velocity map을 따로 생성한뒤 평균 velocity 를 구하는 방법일듯합니다..
		return vOriginColor;
	}

	float4	vNewPos;
	vNewPos.x = (vUV.x * 2.f - 1.f) * vBlurDepth.w;
	vNewPos.y = (vUV.y * -2.f + 1.f) * vBlurDepth.w;
	vNewPos.z = vBlurDepth.x * vBlurDepth.w;
	vNewPos.w = vBlurDepth.w;

	float4 currentPos = vNewPos;

	currentPos /= currentPos.w;

	vNewPos = mul(vNewPos, g_matInvVP);
	vNewPos.w = 1.f;

	vNewPos = mul(vNewPos, g_matPreViewProj);

	float4 previousPos = vNewPos;

	previousPos /= previousPos.w;

	float2 velocity = (currentPos - previousPos);

	int inewCount;
	inewCount = 5;
	velocity = velocity / inewCount;

	// 오브젝트 모션블러 전용 
	/*if (vDepth != vBlurDepth.w)
	{
		inewCount = 0;

		return vOriginColor;
	}*/

	velocity.x = -velocity.x;

	float4 color = vOriginColor;

	bool bSampling = false;
	float2 vNewUV = vUV;
	for (int i = 1; i < inewCount; ++i)
	{
		vNewUV = vNewUV + velocity;

		float4 vAlbedo = g_GBufferAlbedo.Sample(g_GBufferSampler, vNewUV);

		color += vAlbedo;
		bSampling = true;
	}

	if (bSampling)
		color = color / inewCount;

	float4 vOutput = float4(color.rgb, 1.f);

	return vOutput;
}

VS_OUTPUT_TEX LightAccDirVS(uint iVertexID : SV_VertexID)
{
	VS_OUTPUT_TEX output = (VS_OUTPUT_TEX)0;

	output.vPos = float4(vPos[iVertexID], 0.0f, 1.0f);
	output.vUV = vUV[iVertexID];

	return output;
}


VS_OUTPUT_TEX LightAccPointVS(VS_INPUT_POS input)
{
	VS_OUTPUT_TEX	output = (VS_OUTPUT_TEX)0;

	float4	vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vPos = vPos;

	// UV값은 화면공간으로 줍니다..
	output.vUV.x = vPos.x / vPos.w * 0.5f + 0.5f;
	output.vUV.y = vPos.y / vPos.w * -0.5f + 0.5f;

	return output;
}


_tagLightInfo ComputeLightAcc(float3 vViewPos, float3 vViewNormal, float2 vUV)
{
	_tagLightInfo	tInfo = (_tagLightInfo)0;

	float3	vLightDir = (float3)0;
	float	fAtt = 1.f;
	float fAttenuationDescendRate;

	//Directional Light
	if (g_iLightType == 0)
	{
		// 조명 방향을 뷰 공간으로 변환한다.
		vLightDir = mul(float4(g_vLightDirection, 0.f), g_matView).xyz;
		vLightDir = -normalize(vLightDir);
	}
	//Point Light
	else if (g_iLightType == 1)
	{
		// 조명 위치를 뷰 공간으로 변환한다.
		float3	vLightViewPos = mul(float4(g_vLightPosition, 1.f), g_matView).xyz;

		// 정점에서 조명을 향하는 벡터를 만든다.
		vLightDir = vLightViewPos - vViewPos;

		// 조명과 정점간의 거리를 구한다.
		float	fDist = length(vLightDir);

		vLightDir = -normalize(vLightDir);

		if (fDist > g_fLightRange)
			fAtt = 0.f;

		else
			fAtt = 1.f - fDist / g_fLightRange;
	}
	//Spot Light
	else if (g_iLightType == 2)
	{
		// 조명 방향을 뷰 공간으로 변환한다.
		vLightDir = mul(float4(g_vLightDirection, 0.f), g_matView).xyz;
		vLightDir = normalize(vLightDir);

		//조명 위치를 뷰 공간으로 변환한다.
		float3	vLightViewPos = mul(float4(g_vLightPosition, 1.f), g_matView).xyz;

		// 조명위치에서 정점위치로 향하는 벡터를 구해준다.
		float3 vLightDirectionToTarget = vViewPos - vLightViewPos;
		vLightDirectionToTarget = normalize(vLightDirectionToTarget);

		float	fViewAngle = dot(vLightDir, vLightDirectionToTarget);

		// 바깥쪽 각도를 벗어나면 조명 범위 안에 없다는 것이다.
		if (fViewAngle < g_fLightOutAngle)
		{
			fAtt = 0.f;
		}
		// 이 안에 들어올 경우에만 나머지 연산을 해준다.
		else
		{
			float	fDistance = distance(vLightViewPos, vViewPos);
			float fDot = dot(-vLightDir, vViewNormal);

			//조명의 진행 방향과 법선 벡터와의 내적 결과가 양수이면 빛을 받을 수 없는 위치이다.
			//= 조명의 진행방향의 역방향과 법선 벡터와의 내적 결과가 음수이면 빛을 받을 수 없는 위치이다.
			if (fDot > 0)
			{
				float fDistanceAttenuation = max(0, 1.f - fDistance / g_fLightRange);

				if (fDistance <= g_fLightRange)
				{
					//InAngle 안쪽
					if (fViewAngle >= g_fLightInAngle)
					{
						fAtt = 1.0f;
					}
					//InAngle 바깥쪽 이면서 OutAngle안쪽
					else
					{
						//빛의 방향을 기준으로 대칭되어진 cos파형을 따르게 되므로, 범위는 0 부터 PI까지이며,
						//각도가 클 수록 값은 작아진다. 따라서 cos함수의 정의역은 InAngle이 더 크게 되므로,
						//InAngle에서 OutAngle을 빼줘서 분모를 만들어준다.
						//분자도 같은 이치로 InAngle과 OutAngle 사이에 있다면 내적한 결과는 항상 InAngle보다 작으므로
						//InAngle에서 내적한 결과를 빼준다.
						fAtt = 1.0f - ((g_fLightInAngle - fViewAngle) / (g_fLightInAngle - g_fLightOutAngle));
					}
				}
				//LightRange 바깥쪽
				else
				{
					fAtt = 0.0f;
				}
				//거리에 따른 감쇠
				fAtt *= fDistanceAttenuation * fDistanceAttenuation;
			}
			//빛을 받을 수 없음
			else
			{
				float fDistanceAttenuation = max(0, 1.f - fDistance / g_fLightRange);

				fAtt = 1.0f - min(1, -fDot);

				if (fDistance <= g_fLightRange)
				{
					if (fViewAngle < g_fLightInAngle)
					{
						fAtt *= (1.0f - ((g_fLightInAngle - fViewAngle) / (g_fLightInAngle - g_fLightOutAngle)));
					}
				}

				fAtt *= fDistanceAttenuation * fDistanceAttenuation;
			}
		}

		vLightDir = -vLightDir;
	}


	float4	vSpcCol = g_GBufferSpecular.Sample(g_GBufferSampler, vUV);
	float4	vDif = ConvertColor(vSpcCol.r);
	float4	vAmb = ConvertColor(vSpcCol.g);
	float4	vSpc = ConvertColor(vSpcCol.b);

	tInfo.vDiffuse = g_vLightDiffuse * vDif * max(0, dot(vLightDir, vViewNormal)) * fAtt;
	tInfo.vAmbient = g_vLightAmbient * vAmb * fAtt;

	float3	vView = -normalize(vViewPos);
	float3	vR = 2.f * vViewNormal * dot(vViewNormal, vLightDir) - vLightDir;
	vR = normalize(vR);

	//tInfo.vSpecular = (g_vLightSpecular * vSpc * pow(max(0, dot(vView, vR)), vSpcCol.w) * fAtt);
	tInfo.vSpecular = (g_vLightSpecular * vSpc * pow(max(0, dot(vView, vR)), vSpc.w) * fAtt);
	tInfo.vSpecular.a = 1.0f;

	return tInfo;
}


PS_OUTPUT_LIGHTACC LightAccPS(VS_OUTPUT_TEX input)
{
	PS_OUTPUT_LIGHTACC	output = (PS_OUTPUT_LIGHTACC)0;
	
	float4	vDepth = g_GBufferDepth.Sample(g_GBufferSampler, input.vUV);

	if (vDepth.a == 0.f)
		clip(-1);

	// 만약 알파가 0이 아니라면 UV와 Depth를 이용하여 좌표를 구한다.
	float3	vPos;
	vPos.x = (input.vUV.x * 2.f - 1.f) * vDepth.w;
	vPos.y = (input.vUV.y * -2.f + 1.f) * vDepth.w;
	vPos.z = vDepth.x * vDepth.w;

	// 뷰공간의 위치를 구한다.
	vPos = mul(float4(vPos, vDepth.w), g_matInvProj);

	// Point Light 또는 Spot Light 일 경우
	if (g_iLightType != 0)
	{
		// 조명 위치를 뷰 공간으로 변환한다.
		float3	vLightViewPos = mul(float4(g_vLightPosition, 1.f), g_matView).xyz;

		float	fDist = distance(vLightViewPos, vPos);

		// 빛의 범위를 벗어났을 경우 처리안함..
		if (fDist > g_fLightRange)
			clip(-1);
	}

	float4	vNormalCol = g_GBufferNormal.Sample(g_GBufferSampler, input.vUV);
	float3	vNormal = vNormalCol.xyz * 2.f - 1.f;

	_tagLightInfo	tInfo = ComputeLightAcc(vPos, vNormal, input.vUV);

	output.vDiffuse = tInfo.vDiffuse + tInfo.vAmbient;
	output.vSpecular = tInfo.vSpecular;
	output.vSpecular.a = 1.f;

	return output;
}

float4 ComputeFog(float3 vViewPos, float4 vOriginColor)
{
	float fRate = 0;
	// Camera와의 거리. View공간에서 Camera의 Position은 원점
	float	fDistance = length(vViewPos);

	if (fDistance > g_fPostProcessingFogMaxDistance)
	{
		fRate = 1.0f;
	}
	else
	{
		if (fDistance > g_fPostProcessingFogMinDistance)
		{
			fRate = (fDistance - g_fPostProcessingFogMinDistance) / (g_fPostProcessingFogMaxDistance - g_fPostProcessingFogMinDistance);
		}
	}

	float4 vColor;
	vColor.rgb = ((1.0f - fRate) * vOriginColor.rgb) + ((fRate)* g_vPostProcessingFogColor.rgb);
	vColor.a = 1.0f;
	return vColor;
}


float4 ComputeBloom(float2 vUV, float4 vOriginColor, float fSigma)
{
	float4	vColor = vOriginColor;

	uint iSamplingRange = 3;

	float2 vPixelUnit = float2(0.00078125f, 0.00138889f);
	float2 vCurrentUV;

	float weight[32] = {
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	};

	for (uint i = 0; i < iSamplingRange; ++i)
	{
		weight[i] = GaussianDistribution(iSamplingRange, i, fSigma);
	}

	vColor = (vOriginColor * weight[0]) * 2.0f;

	for (int i = 1; i < iSamplingRange; ++i)
	{
		vCurrentUV = vPixelUnit * i;
		vCurrentUV.y = 0.0f;

		// HDR 켜져있으면 Bloom Texture 가져옵니당
		if (g_iPostProcessingFlags & 0x0010)
		{
			float4 vDownScaleColorX = g_DownScaleTex.SampleLevel(g_HDRSampler, vUV + vCurrentUV, 0);
			float4 vDownScaleColorY = g_DownScaleTex.SampleLevel(g_HDRSampler, vUV - vCurrentUV, 0);
			vColor += vDownScaleColorX;
			vColor += vDownScaleColorY;

			//vColor += g_DownScaleTex.SampleLevel(g_GBufferSampler, vUV + vCurrentUV, 3) * weight[i];
			//vColor += g_DownScaleTex.SampleLevel(g_GBufferSampler, vUV - vCurrentUV, 3) * weight[i];
		}
		else
		{
			vColor += g_GBufferAlbedo.Sample(g_GBufferSampler, vUV + vCurrentUV) * weight[i];
			vColor += g_GBufferAlbedo.Sample(g_GBufferSampler, vUV - vCurrentUV) * weight[i];
		}
	}


	for (int i = 1; i < iSamplingRange; ++i)
	{
		vCurrentUV = vPixelUnit * i;
		vCurrentUV.x = 0.0f;

		// HDR 켜져있으면 Bloom Texture 가져옵니당
		if (g_iPostProcessingFlags & 0x0010)
		{
			float4 vDownScaleColorX = g_DownScaleTex.SampleLevel(g_HDRSampler, vUV + vCurrentUV, 0);
			float4 vDownScaleColorY = g_DownScaleTex.SampleLevel(g_HDRSampler, vUV - vCurrentUV, 0);
			vColor += vDownScaleColorX;
			vColor += vDownScaleColorY;

			//vColor += g_DownScaleTex.SampleLevel(g_GBufferSampler, vUV + vCurrentUV, 3) * weight[i];
			//vColor += g_DownScaleTex.SampleLevel(g_GBufferSampler, vUV - vCurrentUV, 3) * weight[i];
		}
		else
		{
			vColor += g_GBufferAlbedo.Sample(g_GBufferSampler, vUV + vCurrentUV) * weight[i];
			vColor += g_GBufferAlbedo.Sample(g_GBufferSampler, vUV - vCurrentUV) * weight[i];
		}
	}

	vColor = vColor * 0.5f;	

	return vColor;
}

float4 PostProcess(float3 vViewPos, float3 vViewNormal, float2 vUV, float4 vOriginColor, float fDepthValue)
{
	float4 vColor = vOriginColor;
	float fDistance = length(vViewPos);

	if (g_iPostProcessingFlags == 0)
		return vColor;

	if (g_iPostProcessingFlags & 0x0020)
	{
		//MOTION_BLUR
		vColor = ComputeMotionBlur(vViewPos, vUV, vColor);
	}

	if (g_iPostProcessingFlags & 0x0001)
	{
		//ANTIALIASING
	}
	if (g_iPostProcessingFlags & 0x0002)
	{
		//EYE_ADAPTION
	}
	if (g_iPostProcessingFlags & 0x0080)
	{
		//DEPTH_OF_FIELD
		float4 vDOFColor = (float4)0;
		float fDistanceValue = (fDistance - g_fPostProcessingDepthOfFieldMinRange) /
			(g_fPostProcessingDepthOfFieldMaxRange - g_fPostProcessingDepthOfFieldMinRange);
		float fSigma = min(1.0f, fDistanceValue);
		fSigma = fSigma * g_fPostProcessingDepthOfFieldMaxSigma;
		if (fSigma > 0)
		{
			vDOFColor = ComputeBloom(vUV, vColor, 1);
			//vDOFColor = ComputeBloom(vUV, vColor, fSigma);
			fDistanceValue = min(1.0f, fDistanceValue);
			vColor = (vColor * (1.f - fDistanceValue)) + (vDOFColor * fDistanceValue);
		}
	}
	//if (g_iPostProcessingFlags & 0x0008) // 이전 단계로 옮겨짐
	//{
	//	//FOG
	//	vColor = ComputeFog(vViewPos, vColor);
	//}
	if (g_iPostProcessingFlags & 0x0010)
	{
		//HDR
		vColor = ToneMapping(vColor, vUV);
	}
	if (g_iPostProcessingFlags & 0x0004)
	{
		//BLOOM (HDR ToneMapping에 통합)
		//vColor = ComputeBloom(vUV, vColor, 10);
	}
	if (g_iPostProcessingFlags & 0x0040)
	{
		//AMBIENT_OCCLUSION
	}

	return vColor;
}




//Light 적용 된 렌더타겟의 값을 받아와 처리함
PS_OUTPUT_SINGLE LightBlendPS(VS_OUTPUT_TEX input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	float4	vAlbedo = g_GBufferAlbedo.Sample(g_GBufferSampler, input.vUV);
	
	if (vAlbedo.a == 0.f)
		clip(-1);

	float4	vDif = g_LightDif.Sample(g_GBufferSampler, input.vUV);
	float4	vSpc = g_LightSpc.Sample(g_GBufferSampler, input.vUV);

	float4	vSpcCol = g_GBufferSpecular.Sample(g_GBufferSampler, input.vUV);
	float4	vMaterialDiffuse = ConvertColor(vSpcCol.r);
	ConvertColor(vSpcCol.g);
	ConvertColor(vSpcCol.b);
	float4	vMaterialEmissive = ConvertColor(vSpcCol.a);
	//float4	vMaterialEmissive = ConvertColor(vNormalCol.a);
	float3	vEmissive = max(0.f, vMaterialEmissive.xyz * vMaterialEmissive.a);
	vEmissive = vMaterialDiffuse.xyz * vEmissive;

	float4	vDepth = g_GBufferDepth.Sample(g_GBufferSampler, input.vUV);

	float4	vPos;
	vPos.x = (input.vUV.x * 2.f - 1.f) * vDepth.w;
	vPos.y = (input.vUV.y * -2.f + 1.f) * vDepth.w;
	vPos.z = vDepth.x * vDepth.w;
	vPos.w = vDepth.w;

	float3 vWorldPos = vPos;
	
	// 뷰공간의 위치를 구한다.
	float4 vViewPos = mul(vPos, g_matInvProj);

	// 월드공간의 위치를 구한다.
	vPos = mul(vPos, g_matInvVP);
	vPos.w = 1.f;
	
	// 그림자를 적용할지 판단한다.
	if (g_iShadowEnable == 1)
	{
		// 조명에서 바라본 투영 위치를 구한다.
		vPos = mul(vPos, g_matShadowVP);

		float3	vShadowUV;
		vShadowUV = vPos.xyz / vPos.w;
		vShadowUV.xy = 0.5f * vShadowUV.xy + 0.5f;
		vShadowUV.y = 1.f - vShadowUV.y;

		float3 vLightPos = g_vShadowLightPos;

		vLightPos = mul(float4(vLightPos, 1.f), g_matVP);

		vWorldPos = vLightPos - vWorldPos;

		vWorldPos = normalize(vWorldPos);

		float vNewBias = 0.f;

		float4	vNormalCol = g_GBufferNormal.Sample(g_GBufferSampler, input.vUV);

		float3	vNormal = vNormalCol.xyz * 2.f - 1.f;

		vNormal = normalize(vNormal);

		float cosTheta = dot(vNormal, vWorldPos);

		float fTan = tan(acos(cosTheta));
		vNewBias = g_fShadowBias * fTan;
		vNewBias = clamp(vNewBias, 0.05f, 0.5f);

		//vNewBias = g_fShadowBias * tan(acos(dot(vNormal, vWorldPos))); // 밑의 식과 동일..하다는듯 합니다..
		vNewBias = g_fShadowBias * sqrt(1 - pow(cosTheta, 2)) / cosTheta;

		float4	vShadow = 0.f;

		// 그림자 맵에 저장된 깊이 값을 받아온다..
		vShadow = g_ShadowMap.Sample(g_GBufferSampler, vShadowUV.xy);

		float	fShadow = 0.f;
		// 그림자가 생기는 부분일때
		if (vShadow.z < vPos.z / vShadow.w - vNewBias)
		{
			fShadow = 1.f;
		}

		// 깊이 값이 저장되어 있지 않은 픽셀일때..
		if (vShadow.x == 0.f)
			fShadow = 0.f;

		// 그림자가 생기는 부분일때
		if (fShadow.x == 1.f)
		{
			float visibility = 1.f;

			int iCount = 1;

			int iRoop = 2;

			// PCF 적용 단계..
			for (int i = -iRoop; i <= iRoop; i++)
			{
				for (int j = -iRoop; j <= iRoop; ++j)
				{
					float fWidth = 1 / g_vShadowResolution.x; // 해상도 바뀔때 Roop값과 함께 바꿔줘야함..
					float fHeight = 1 / g_vShadowResolution.y;

					fWidth *= i;
					fHeight *= j;
					float2 vNewUV = vShadowUV;
					vNewUV.xy = vNewUV.xy + float2(fWidth, fHeight);
					float4 vSHADOW = g_ShadowMap.Sample(g_GBufferSampler, vNewUV.xy);
					if (vSHADOW.z < vPos.z / vSHADOW.w - vNewBias)
					{
						++iCount;
						float fShadowFactor = 0.00175f;
						visibility -= fShadowFactor * iCount;
					}
				}
			}

			output.vTarget = vAlbedo * vDif + vSpc;
			output.vTarget = output.vTarget * visibility;
			output.vTarget.a = vAlbedo.a;
		}
		// 그림자가 아닌 부분일때 원본 출력
		else
		{
			output.vTarget = vAlbedo * vDif + vSpc;
			output.vTarget.a = vAlbedo.a;
		}
	}
	
	else
	{
		output.vTarget = vAlbedo * vDif + vSpc;
		output.vTarget.a = vAlbedo.a;
	}

	// 최종 컬러에 자체발광값을 더해줍니당..
	float4 vFinalColor = output.vTarget;
	vFinalColor.xyz += vEmissive;
	output.vTarget = vFinalColor;

	// Fog 계산 합니다
	if (g_iPostProcessingFlags & 0x0008)
	{
		float3 vColor = output.vTarget.rgb;
		output.vTarget.rgb = ComputeFog(vViewPos, float4(vColor, 1.f));
	}

	return output;
}

PS_OUTPUT_SINGLE RenderBlendPS(VS_OUTPUT_TEX input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	float4	vAlbedo = g_GBufferAlbedo.Sample(g_GBufferSampler, input.vUV);

	if (vAlbedo.a == 0.f)
		clip(-1);

	//PostProcessing을 위해..
	float4	vDepth = g_GBufferDepth.Sample(g_GBufferSampler, input.vUV);
	float3	vPos;
	vPos.x = (input.vUV.x * 2.f - 1.f) * vDepth.w;
	vPos.y = (input.vUV.y * -2.f + 1.f) * vDepth.w;
	vPos.z = vDepth.x * vDepth.w;
	
	// 뷰공간의 위치를 구한다.
	vPos = mul(float4(vPos, vDepth.w), g_matInvProj);
	
	float4	vNormalCol = g_GBufferNormal.Sample(g_GBufferSampler, input.vUV);
	float3	vNormal = vNormalCol.xyz * 2.f - 1.f;
	
	output.vTarget = PostProcess(vPos, vNormal, input.vUV, vAlbedo, vDepth.x);
	
	return output;
}



PS_OUTPUT_SINGLE RenderPrevBackBufferPS(VS_OUTPUT_TEX input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	float4	vAlbedo = g_GBufferAlbedo.Sample(g_GBufferSampler, input.vUV);

	if (vAlbedo.a == 0.f)
		clip(-1);

	output.vTarget = vAlbedo;

	return output;
}
