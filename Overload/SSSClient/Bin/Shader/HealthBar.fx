#include "Share.fx"
//특정 약식의 텍스쳐에만 사용 가능하도록 함.
//범용 아니니까 돌려쓰지 말것.

SamplerState		g_LinearSampler : register(s0);
Texture2D			g_HealthBarBackground : register(t0); // Diffuse
Texture2D			g_HealthBarForeground : register(t1); // Normal

cbuffer	HealthBarBillboardInfo	: register(b11)
{
	//HealthBar Info
	float		g_fHealthBarAlpha;
	float		g_fHealthPercentage;
	float2	g_vHealthBarPadding;
	float4	g_vHealthBarProgressColor;
}

VS_OUTPUT_TEX HealthBarVS(VS_INPUT_TEX input)
{
	VS_OUTPUT_TEX	output = (VS_OUTPUT_TEX)0;
	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;
	output.vPos = mul(float4(vPos, 1.f), g_matWP);
	output.vUV = input.vUV;
	return output;
}


PS_OUTPUT_SINGLE HealthBarPS(VS_OUTPUT_TEX input)
{
	if (g_fHealthBarAlpha <= 0.0f)
		clip(-1);
	
	float4 vBackgroundColor = g_HealthBarBackground.Sample(g_LinearSampler, input.vUV);
	float4 vForegroundColor = g_HealthBarForeground.Sample(g_LinearSampler, input.vUV);
	float4 vResultColor = (float4)0;

	vForegroundColor = vForegroundColor * g_vHealthBarProgressColor;
	//1/16 부터 시작, 15/16에서 끝
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	float2 vTextureOffsetThreshold = float2 (1.0f / 16.0f, 15.0f / 16.0f);
	float fHealthLength = vTextureOffsetThreshold.y - vTextureOffsetThreshold.x;
	float fEmptyHealthEntryPosition =  vTextureOffsetThreshold.x + fHealthLength * g_fHealthPercentage;
		
	float4 fColor = vForegroundColor * g_vHealthBarProgressColor;
	//비어있기 시작하는 위치

	if (vForegroundColor.a >= 0.0f)
	{
		if (input.vUV.x >= fEmptyHealthEntryPosition)
		{
			vResultColor = vBackgroundColor * 0.5f + fColor * 0.5f;
		}
		else
		{
			vResultColor = fColor;
		}
	}
	else
	{
		vResultColor = vBackgroundColor;
	}

	vResultColor.a = vResultColor.a * g_fHealthBarAlpha * 0.5f;

	//output.vTarget.rgb = vResultColor.rgb;
	//output.vTarget.a = g_fHealthBarAlpha;
	
	output.vTarget = vResultColor;
	return output;
}

cbuffer	BossHealthBar	: register(b7)
{
	float3  g_vBossHealthBarColor;
	float	g_fBossHealthBarProgress;
	int		g_iBossHealthBarLineCount;
	float3	g_vBossHealthBarBackColor;
}

PS_OUTPUT_SINGLE UIBossHealthBarPS(VS_OUTPUT_TEX input)
{
	float4 vBackgroundColor = g_HealthBarBackground.Sample(g_LinearSampler, input.vUV);
	float4 vForegroundColor = g_HealthBarForeground.Sample(g_LinearSampler, input.vUV);
	float4 vResultColor = (float4)0;

	vForegroundColor = vForegroundColor * float4(g_vBossHealthBarColor, 1.f);

	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	float4 fColor = vForegroundColor * float4(g_vBossHealthBarColor, 1.f);
	//비어있기 시작하는 위치
	float2 vTextureOffsetThreshold = float2 (1.0f / 16.0f, 15.0f / 16.0f);
	float fHealthLength = vTextureOffsetThreshold.y - vTextureOffsetThreshold.x;
	float fEmptyHealthEntryPosition = vTextureOffsetThreshold.x + fHealthLength * g_fBossHealthBarProgress;

	if (vForegroundColor.a > 0.80f)
	{
		if (input.vUV.x <= g_fBossHealthBarProgress)
		{
			vResultColor = fColor;
		}
		else
		{
			vResultColor = vForegroundColor * float4(g_vBossHealthBarBackColor, 1.f);
		}
	}
	else
	{
		vResultColor = vBackgroundColor;
	}
	
	vResultColor.a = vResultColor.a;

	output.vTarget = vResultColor;

	return output;
}