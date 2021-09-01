#include "Share.fx"

Texture2D g_OriginTexture : register(t10);
Texture2D g_MaskTexture : register(t9);

VS_OUTPUT_TEX AlphaMaskVS(VS_INPUT_TEX input)
{
	VS_OUTPUT_TEX	output = (VS_OUTPUT_TEX)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWP);
	output.vUV = input.vUV;

	return output;
}

PS_OUTPUT_SINGLE AlphaMaskPS(VS_OUTPUT_TEX input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	//���� �ؽ��� UV + ����ũ �ؽ����� UV �� ���Ͽ� �ȼ� ���� �̾���.
	float4 vColor = g_MaskTexture.Sample(g_DiffuseSampler, input.vUV);

	//����ũ�� ���İ� 0�̸� clip ó���� ����.
	if (vColor.a == 0.0f)
	{
		clip(-1);
	}

	//���İ� 0�� �ƴ� �κ��� �����ϰ� ������ ������.
	vColor += g_OriginTexture.Sample(g_DiffuseSampler, input.vUV);

	//RGB �� 0�� �κ�(������) �� �ĳ���.
	if (vColor.r == 0.0f)
	{
		clip(-1);
	}

	output.vTarget = vColor;

	return output;
}




//Test Slider Shader
//==================================================================



VS_OUTPUT_TEX SliderVS(VS_INPUT_TEX input)
{
	VS_OUTPUT_TEX	output = (VS_OUTPUT_TEX)0;


	
	//float3	vPos = input.vPos - g_vPivot * g_vMeshLength;
	//vPos.y += 0.1f;

	float zPosition = input.vPos.z;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWP);		

	output.vPos.z += zPosition;

	output.vUV = input.vUV;

	return output;
}

PS_OUTPUT_SINGLE SliderPS(VS_OUTPUT_TEX input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	float4 vColor = g_MaskTexture.Sample(g_DiffuseSampler, input.vUV);

	if (vColor.a == 0.0f)
	{
		clip(-1);
	}

	input.vUV.y += input.vPos.z;


	vColor += g_OriginTexture.Sample(g_DiffuseSampler, input.vUV);

	if (vColor.r == 0.0f)
	{
		clip(-1);
	}

	output.vTarget = vColor;

	return output;
}
