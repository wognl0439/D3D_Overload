#include "Share.fx"

SamplerState		g_DistortionSampler : register(s1);
Texture2D			g_BackBufferTexture : register(t15);
Texture2D			g_GBufferDepth	: register(t17);

VS_OUTPUT_TEX_NORMAL BurnVS(VS_INPUT_TEX_NORMAL input)
{
	VS_OUTPUT_TEX_NORMAL output = (VS_OUTPUT_TEX_NORMAL)0;

	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;
	output.vProjectionPos = mul(float4(vPos, 1.0f), g_matWVP);
	output.vPos = output.vProjectionPos;
	output.vNormal = mul(float4(input.vNormal, 0.f), g_matWV).xyz;
	output.vNormal = normalize(output.vNormal);
	output.vViewPos = mul(float4(vPos, 1.f), g_matWV).xyz;
	output.vUV = input.vUV;// 3D ������ cbuffer �� Animation ������Ʈ �и��� ��
	output.vLocalPos = vPos;

	return output;
}

PS_OUTPUT BurnPS(VS_OUTPUT_TEX_NORMAL input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	float fProgress = g_NoiseTexture.Sample(g_DiffuseSampler, input.vUV).r;

	if (fProgress <= g_fFXBurnProgress)
		clip(-1);

	//Forward
	if (g_iRenderMode == 0)
	{
		_tagLightInfo	tMtrl = ComputeLight(input.vViewPos, input.vNormal);

		float4 vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV) * g_vMaterialDiffuse;

		if (vColor.a == 0.0f)
		{
			clip(-1);
		}
		output.vTarget0.rgb = vColor.rgb * (tMtrl.vDiffuse + tMtrl.vAmbient).rgb + tMtrl.vSpecular.rgb;
		output.vTarget0.a = vColor.a;

	}
	//Deferred
	else
	{

		_tagLightInfo	tMtrl = ComputeLight(input.vViewPos, input.vNormal);
		float4 vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV) * g_vMaterialDiffuse;

		if (vColor.a == 0.0f)
		{
			clip(-1);
		}

		if (fProgress <= g_fFXBurnProgress + g_fFXBurnBorderRangePercentage)
		{
			float fLocalProgress = fProgress / (g_fFXBurnProgress + g_fFXBurnBorderRangePercentage);
			vColor = float4(g_vFXBurnColor.rgb, fLocalProgress);
		}

		// Albedo
		output.vTarget0.rgb = vColor.rgb;
		output.vTarget0.a = vColor.a;

		// Normal
		output.vTarget1.rgb = input.vNormal * 0.5f + 0.5f;
		output.vTarget1.a = 1.f;

		// Depth
		output.vTarget2.rgb = (float3)(input.vProjectionPos.z / input.vProjectionPos.w);
		output.vTarget2.a = input.vProjectionPos.w;

		//Specular Target
		output.vTarget3.r = ConvertColor(g_vMaterialDiffuse);
		output.vTarget3.g = ConvertColor(g_vMaterialAmbient);
		output.vTarget3.b = ConvertColor(g_vMaterialSpecular);
		output.vTarget3.a = g_vMaterialSpecular.a;
	}

	return output;
}

VS_OUTPUT_TEX_NORMAL RimLightVS(VS_INPUT_TEX_NORMAL input)
{
	VS_OUTPUT_TEX_NORMAL output = (VS_OUTPUT_TEX_NORMAL)0;

	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;
	output.vProjectionPos = mul(float4(vPos, 1.0f), g_matWVP);
	output.vPos = output.vProjectionPos;
	output.vNormal = mul(float4(input.vNormal, 0.f), g_matWV).xyz;
	output.vNormal = normalize(output.vNormal);
	output.vViewPos = mul(float4(vPos, 1.f), g_matWV).xyz;
	output.vUV = input.vUV;// 3D ������ cbuffer �� Animation ������Ʈ �и��� ��
	output.vLocalPos = vPos;

	return output;
}

PS_OUTPUT RimLightPS(VS_OUTPUT_TEX_NORMAL input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;
	
	//Forward
	if (g_iRenderMode == 0)
	{
		_tagLightInfo	tMtrl = ComputeLight(input.vViewPos, input.vNormal);

		float4 vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV) * g_vMaterialDiffuse;

		if (vColor.a == 0.0f)
		{
			clip(-1);
		}
		output.vTarget0.rgb = ComputeRimLight(vColor.rgb, input.vNormal ,input.vViewPos) * (tMtrl.vDiffuse + tMtrl.vAmbient).rgb + tMtrl.vSpecular.rgb;
		output.vTarget0.a = vColor.a;

	}
	//Deferred
	else
	{
		//_tagLightInfo	tMtrl = ComputeLight(input.vViewPos, input.vNormal);
		float4 vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV) * g_vMaterialDiffuse;

		if (vColor.a == 0.0f)
		{
			clip(-1);
		}

		// Albedo
		output.vTarget0.rgb = ComputeRimLight(vColor.rgb, input.vNormal, input.vViewPos);
		output.vTarget0.a = vColor.a;

		// Normal
		float3 vNormal = input.vNormal * g_fNormalScale;
		output.vTarget1.rgb = vNormal * 0.5f + 0.5f;
		output.vTarget1.a = 1.f;

		// Depth
		output.vTarget2.rgb = (float3)(input.vProjectionPos.z / input.vProjectionPos.w);
		output.vTarget2.a = input.vProjectionPos.w;

		//Specular Target
		output.vTarget3.r = ConvertColor(g_vMaterialDiffuse);
		output.vTarget3.g = ConvertColor(g_vMaterialAmbient);
		output.vTarget3.b = ConvertColor(g_vMaterialSpecular);
		output.vTarget3.a = g_vMaterialSpecular.a;
	}

	return output;
}


VS_OUTPUT_BUMP BUMPANIM_VS(VS_INPUT_BUMP_ANIM input)
{
	VS_OUTPUT_BUMP	output = (VS_OUTPUT_BUMP)0;

	_tagSkinning tSkinning = Skinning(input.vPos, input.vNormal, input.vTangent, input.vBinormal, input.vWeights, input.vIndices);
	float3 vPos = tSkinning.vPos - g_vPivot * g_vMeshLength;

	output.vProjPos = mul(float4(vPos, 1.f), g_matWVP);
	output.vPos = output.vProjPos;
	// Normal�� ��������� ������ش�.
	output.vNormal = mul(float4(tSkinning.vNormal, 0.f), g_matWV).xyz;
	output.vNormal = normalize(output.vNormal);
	output.vTangent = mul(float4(tSkinning.vTangent, 0.f), g_matWV).xyz;
	output.vTangent = normalize(output.vTangent);
	output.vBinormal = mul(float4(tSkinning.vBinormal, 0.f), g_matWV).xyz;
	output.vBinormal = normalize(output.vBinormal);
	output.vViewPos = mul(float4(vPos, 1.f), g_matWV).xyz;
	output.vUV = input.vUV;

	return output;
}


PS_OUTPUT LIM_BUMPANIM_PS(VS_OUTPUT_BUMP input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	// Alpha Texture�� ����Ҷ�
	if (g_vMaterialAlpha.w == 1.f)
	{
		float3	vAlpha = g_AlphaMapTexture.Sample(g_DiffuseSampler, input.vUV);
		uint3	vTempAlpha;
		vTempAlpha = asuint(vAlpha * 10000.f);
		vAlpha = asfloat(vTempAlpha * 0.0001f);

		if (vAlpha.r == g_vMaterialAlpha.r
			&& vAlpha.g == g_vMaterialAlpha.g
			&& vAlpha.b == g_vMaterialAlpha.b)
			clip(-1);
	}

	float4	vAlbedo = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV);

	// Bump Mapping
	float4	vNormalMap = g_NormalMapTexture.Sample(g_DiffuseSampler, input.vUV);

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

	vNormal = vNormal * g_fNormalScale;

	if (g_iRenderMode == 0)
	{
		_tagLightInfo	tMtrl = ComputeLight(input.vViewPos, vNormal);

		output.vTarget0.rgb = vAlbedo.rgb * (tMtrl.vDiffuse + tMtrl.vAmbient).rgb + tMtrl.vSpecular.rgb;
		output.vTarget0.a = vAlbedo.a;
	}

	else
	{
		if (vAlbedo.a == 0.0f)
		{
			clip(-1);
		}

		// Albedo
		output.vTarget0.rgb = ComputeRimLight(vAlbedo.rgb, input.vNormal, input.vViewPos);
		output.vTarget0.a = vAlbedo.a;

		// Normal
		output.vTarget1.rgb = vNormal * 0.5f + 0.5f;
		output.vTarget1.a = 1.f;

		// Depth
		// ���� �������� ��ȯ�� ������ w���� ����������� z���� �� �ְԵȴ�.
		// �� ���� �߿��� ������ ���α׷��Ӱ� ���� �������ִ� ���� �ƴ����� DirectX�� ����������
		// ��ü�� ���ٰ��� �������ֱ� ���ؼ� ������������ ��ȯ�� x, y, z, w ���� w������ ���
		// �������ش�. ��������� x / w, y / w, z / w, w / w �� �ǰ�
		// x / w, y / w, z / w, 1 �� �ȴ�.
		output.vTarget2.rgb = (float3)(input.vProjPos.z / input.vProjPos.w);
		output.vTarget2.a = input.vProjPos.w;

		// Specular Target
		// ���� ��ȯ
		float4	vMtrlSpc = g_vMaterialSpecular;

		if (g_vMaterialAmbient.a != 0.f)
		{
			vMtrlSpc = g_SpecularMapTexture.Sample(g_DiffuseSampler, input.vUV);
		}

		vMtrlSpc.a = g_vMaterialSpecular.a;

		output.vTarget3.r = ConvertColor(g_vMaterialDiffuse);
		output.vTarget3.g = ConvertColor(g_vMaterialAmbient);
		output.vTarget3.b = ConvertColor(vMtrlSpc);
		output.vTarget3.a = g_vMaterialSpecular.a;

		output.vTarget4.xyz = input.vTangent * 0.5f + 0.5f;
		output.vTarget5.xyz = input.vBinormal * 0.5f + 0.5f;
		output.vTarget4.w = 1.f;
		output.vTarget5.w = 1.f;
	}
	return output;
}

PS_OUTPUT BURN_BUMPANIM_PS(VS_OUTPUT_BUMP input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	// Alpha Texture�� ����Ҷ�
	if (g_vMaterialAlpha.w == 1.f)
	{
		float3	vAlpha = g_AlphaMapTexture.Sample(g_DiffuseSampler, input.vUV);
		uint3	vTempAlpha;
		vTempAlpha = asuint(vAlpha * 10000.f);
		vAlpha = asfloat(vTempAlpha * 0.0001f);

		if (vAlpha.r == g_vMaterialAlpha.r
			&& vAlpha.g == g_vMaterialAlpha.g
			&& vAlpha.b == g_vMaterialAlpha.b)
			clip(-1);
	}

	float4	vAlbedo = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV);

	// Bump Mapping
	float4	vNormalMap = g_NormalMapTexture.Sample(g_DiffuseSampler, input.vUV);

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

	vNormal = vNormal * g_fNormalScale;

	if (g_iRenderMode == 0)
	{
		_tagLightInfo	tMtrl = ComputeLight(input.vViewPos, vNormal);

		output.vTarget0.rgb = vAlbedo.rgb * (tMtrl.vDiffuse + tMtrl.vAmbient).rgb + tMtrl.vSpecular.rgb;
		output.vTarget0.a = vAlbedo.a;
	}

	else
	{
		float fProgress = g_NoiseTexture.Sample(g_DiffuseSampler, input.vUV).r;
		if (fProgress <= g_fFXBurnProgress)
			clip(-1);
		
		/*if (fProgress <= g_fFXBurnProgress)
		{
			vAlbedo.r += fProgress;
		}*/

		// Albedo
		output.vTarget0.rgb = vAlbedo.rgb;
		output.vTarget0.a = vAlbedo.a;

		// Normal
		output.vTarget1.rgb = vNormal * 0.5f + 0.5f;
		output.vTarget1.a = 1.f;

		// Depth
		output.vTarget2.rgb = (float3)(input.vProjPos.z / input.vProjPos.w);
		output.vTarget2.a = input.vProjPos.w;

		// Specular Target
		// ���� ��ȯ
		float4	vMtrlSpc = g_vMaterialSpecular;

		if (g_vMaterialAmbient.a != 0.f)
		{
			vMtrlSpc = g_SpecularMapTexture.Sample(g_DiffuseSampler, input.vUV);
		}

		vMtrlSpc.a = g_vMaterialSpecular.a;

		output.vTarget3.r = ConvertColor(g_vMaterialDiffuse);
		output.vTarget3.g = ConvertColor(g_vMaterialAmbient);
		output.vTarget3.b = ConvertColor(vMtrlSpc);
		output.vTarget3.a = g_vMaterialSpecular.a;

		output.vTarget4.xyz = input.vTangent * 0.5f + 0.5f;
		output.vTarget5.xyz = input.vBinormal * 0.5f + 0.5f;
		output.vTarget4.w = 1.f;
		output.vTarget5.w = 1.f;
	}
	return output;
}



VS_OUTPUT_COLOR_NORMAL OutLineVS(VS_INPUT_TEX_NORMAL input)
{
	VS_OUTPUT_COLOR_NORMAL output = (VS_OUTPUT_COLOR_NORMAL)0;

	float fYScale = (1.0f - g_fFXOutlineSize);
	float3	vPos = float3(input.vPos * g_fFXOutlineSize - g_vFXOutlineOffset) * float3(1.0f, 1.0f + fYScale, 1.0f);
	float4	vProjPos = mul(float4(vPos,1.0f), g_matWVP);

	output.vPos = vProjPos;
	//output.vColor = input.vColor;

	return output;
}


PS_OUTPUT_SINGLE OutLinePS(VS_OUTPUT_COLOR input)
{
	PS_OUTPUT_SINGLE	 output = (PS_OUTPUT_SINGLE)0;
	output.vTarget = g_vFXOutlineColor;
	return output;
}


VS_OUTPUT_BUMP DISTORTION_VS(VS_INPUT_BUMP input)
{
	VS_OUTPUT_BUMP	output = (VS_OUTPUT_BUMP)0;

	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;

	output.vProjPos = mul(float4(vPos, 1.f), g_matWVP);
	output.vPos = output.vProjPos;

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

PS_OUTPUT_SINGLE DISTORTION_PS(VS_OUTPUT_BUMP input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	// Bump ������ �� Normal���� ���� ������ �̿��Ѵ�...
	float4	vNormalMap = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV);

	float3	vNormal = vNormalMap.xyz * 2.f - 1.f;
	vNormal = normalize(vNormal);

	if (input.vBinormal.x != 0.f)
	{
		float3x3	mat =
		{
			-input.vTangent,
			-input.vBinormal,
			-input.vNormal
		};

		vNormal = mul(vNormal, mat);

		vNormal = normalize(vNormal);
	}


	vNormal = vNormal * g_fNormalScale;

	// ����Ʈ(��üȭ��)�� uv�� ���Ѵ�
	float2	vUV = input.vProjPos.xy;
	vUV /= input.vProjPos.w;
	vUV.x = vUV.x * 0.5f + 0.5f;
	vUV.y = vUV.y * -0.5f + 0.5f;

	vNormal.x = vNormal.x / 80.f;
	vNormal.y = vNormal.y / 45.f;

	float2 noiseCoords = vNormal.xy + vUV;

	float4 vDepth = g_GBufferDepth.Sample(g_DiffuseSampler, noiseCoords);

	// �� ���������� ���� ����
	float	fDepthDiff = vDepth.w - input.vProjPos.w;

	float4 vFinalColor = (float4)0;

	// ���̺񱳽� �ڿ��ִ� �ȼ��� ��� 
	if (fDepthDiff > 0.f
		|| vDepth.w == 0.f)
	{
		vFinalColor = g_BackBufferTexture.Sample(g_DistortionSampler, noiseCoords);
	}

	// �տ� �ִ� �ȼ��� ��� ���� ����� ���..
	else
	{
		vFinalColor = g_BackBufferTexture.Sample(g_DiffuseSampler, vUV);
	}

	output.vTarget = vFinalColor;


	return output;
}