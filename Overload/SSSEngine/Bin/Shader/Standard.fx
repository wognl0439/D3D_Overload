
#include "Share.fx"

//Render Target 용
///////////////////////////////////////////////////////////////////////////////
VS_OUTPUT_TEX UITargetVS(VS_INPUT_TEX input)
{
	VS_OUTPUT_TEX	output = (VS_OUTPUT_TEX)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWP);

	output.vUV = input.vUV;

	return output;
}

PS_OUTPUT_SINGLE UITargetPS(VS_OUTPUT_TEX input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	float4 vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV);

	if (vColor.a == 0.f)
		clip(-1);

	output.vTarget = vColor;

	return output;
}
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////

VS_OUTPUT_COLOR StandardColorVS(VS_INPUT_COLOR input)
{
	VS_OUTPUT_COLOR output = (VS_OUTPUT_COLOR)0;
	//폴리곤의 각 버텍스 위치의 평균이 무게중심이 된다. 이 값은 cbuffer로 받아온다.
	//행렬 계산이 끝난 버텍스 위치에 무게중심과의 거리만큼 평행이동 시키면 된다.	

	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;

	output.vPos = mul(float4(vPos, 1.f), g_matWVP);
	
	output.vColor = input.vColor;
	
	output.vLocalPosition = input.vPos;

	return output;
}

PS_OUTPUT_SINGLE StandardColorPS(VS_OUTPUT_COLOR input)
{
	PS_OUTPUT_SINGLE output = (PS_OUTPUT_SINGLE)0;
	output.vTarget = (input.vColor * input.vColor.a) + (float4(1.0f, 0.78125f, 0, 1) * (1 - input.vColor.a));			

	return output;
}




VS_OUTPUT_TEX StandardTextureVS(VS_INPUT_TEX input)
{
	VS_OUTPUT_TEX	output = (VS_OUTPUT_TEX)0;
	
	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;
	
	// mul : 행렬을 곱해주는 함수이다. 벡터도 행렬로 취급한다.
	output.vPos = mul(float4(vPos, 1.f), g_matWVP);
	float2	vUV = ComputeAnimation2DUV(input.vUV);
	output.vUV = vUV;
	return output;
}

PS_OUTPUT_SINGLE StandardTexturePS(VS_OUTPUT_TEX input)
{
	// Alpha Texture를 사용할때
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

	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;
	float4 vColor;

	if (g_iHasAnimation & ANIMATION_2D)
	{
		float2 vAnimationUV = ComputeAnimation2DUV(input.vUV);
		vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, vAnimationUV) * g_vMaterialDiffuse;
	}
	else
	{
		vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV) * g_vMaterialDiffuse;
	}
	
	if (vColor.a == 0.0f)
		clip(-1);

	output.vTarget = vColor;

	return output;
}


VS_OUTPUT_TEX ColoredTextureVS(VS_INPUT_TEX input)
{
	VS_OUTPUT_TEX	output = (VS_OUTPUT_TEX)0;

	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;

	// mul : 행렬을 곱해주는 함수이다. 벡터도 행렬로 취급한다.
	output.vPos = mul(float4(vPos, 1.f), g_matWVP);
	float2	vUV = ComputeAnimation2DUV(input.vUV);
	output.vUV = vUV;
	return output;
}

PS_OUTPUT_SINGLE ColoredTexturePS(VS_OUTPUT_TEX input)
{
	// Alpha Texture를 사용할때
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

	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;
	float4 vColor;

	if (g_iHasAnimation & ANIMATION_2D)
	{
		float2 vAnimationUV = ComputeAnimation2DUV(input.vUV);
		vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, vAnimationUV) * g_vMaterialDiffuse;
	}
	else
	{
		vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV) * g_vMaterialDiffuse;
	}

	if (vColor.a == 0.0f)
	{
		clip(-1);
	}
	else
	{
		vColor = g_vMaterialDiffuse;
	}

	output.vTarget = vColor;

	return output;
}




VS_OUTPUT_TEX UITextureVS(VS_INPUT_TEX input)
{
	VS_OUTPUT_TEX	output = (VS_OUTPUT_TEX)0;

	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;

	output.vPos = mul(float4(vPos, 1.f), g_matWP);

	float2	vUV = ComputeAnimation2DUV(input.vUV);
	output.vUV = vUV;

	return output;
}

PS_OUTPUT_SINGLE UITexturePS(VS_OUTPUT_TEX input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	float4	vColor;
	if (g_iHasAnimation & ANIMATION_2D)
	{
		float2 vAnimationUV = ComputeAnimation2DUV(input.vUV);
		vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, vAnimationUV) * g_vMaterialDiffuse;
	}
	else
	{
		vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV) * g_vMaterialDiffuse;
	}

	if (vColor.a == 0.f)
		clip(-1);

	output.vTarget = vColor;

	return output;
}



VS_OUTPUT_COLOR_NORMAL StandardColorNormalVS(VS_INPUT_COLOR_NORMAL input)
{
	VS_OUTPUT_COLOR_NORMAL output = (VS_OUTPUT_COLOR_NORMAL)0;

	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;

	output.vProjectionPos = mul(float4(vPos, 1.0f), g_matWVP);
	output.vPos = output.vProjectionPos;
	output.vNormal = mul(float4(input.vNormal, 0.f), g_matWV).xyz;
	output.vNormal = normalize(output.vNormal);
	output.vViewPos = mul(float4(vPos, 1.f), g_matWV).xyz;
	output.vColor = input.vColor;
	output.vLocalPos = vPos;

	return output;
}

PS_OUTPUT StandardColorNormalPS(VS_OUTPUT_COLOR_NORMAL input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	//Forward
	if (g_iRenderMode == 0)
	{
		_tagLightInfo	tMtrl = ComputeLight(input.vViewPos, input.vNormal);
		output.vTarget0.rgb = input.vColor.rgb * (tMtrl.vDiffuse + tMtrl.vAmbient).rgb + tMtrl.vSpecular.rgb;
		output.vTarget0.a = input.vColor.a;
	}
	else
	{
		_tagLightInfo	tMtrl = ComputeLight(input.vViewPos, input.vNormal);
		output.vTarget0.rgb = input.vColor.rgb * (tMtrl.vDiffuse + tMtrl.vAmbient).rgb + tMtrl.vSpecular.rgb;
		output.vTarget0.a = input.vColor.a;


		// Albedo
		output.vTarget0.rgb = input.vColor.rgb;
		output.vTarget0.a = input.vColor.a;
		
		// Normal
		float3 vNormal = input.vNormal * g_fNormalScale;
		output.vTarget1.rgb = vNormal * 0.5f + 0.5f;
		output.vTarget1.a = 1.f;
		//
		// Depth
		// 투영 공간으로 변환된 정점은 w값에 뷰공간에서의 z값이 들어가 있게된다.
		// 이 값이 중요한 이유는 프로그래머가 직접 연산해주는 것은 아니지만 DirectX가 내부적으로
		// 물체의 원근감을 적용해주기 위해서 투영공간으로 변환된 x, y, z, w 값을 w값으로 모두
		// 나누어준다. 결과적으로 x / w, y / w, z / w, w / w 가 되고
		// x / w, y / w, z / w, 1 이 된다.
		output.vTarget2.rgb = (float3)(input.vProjectionPos.z / input.vProjectionPos.w);
		output.vTarget2.a = input.vProjectionPos.w;
		
		// Specular Target
		output.vTarget3.r = ConvertColor(g_vMaterialDiffuse);
		output.vTarget3.g = ConvertColor(g_vMaterialAmbient);
		output.vTarget3.b = ConvertColor(g_vMaterialSpecular);
		output.vTarget3.a = ConvertColor(g_vMaterialEmissive);
	}

	return output;
}



VS_OUTPUT_TEX_NORMAL StandardTexNormalVS(VS_INPUT_TEX_NORMAL input)
{
	VS_OUTPUT_TEX_NORMAL output = (VS_OUTPUT_TEX_NORMAL)0;
		
	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;
	output.vProjectionPos = mul(float4(vPos, 1.0f), g_matWVP);
	output.vPos = output.vProjectionPos;
	output.vNormal = mul(float4(input.vNormal, 0.f), g_matWV).xyz;
	output.vNormal = normalize(output.vNormal);
	output.vViewPos = mul(float4(vPos, 1.f), g_matWV).xyz;
	output.vUV = input.vUV;// 3D 용으로 cbuffer 및 Animation 컴포넌트 분리할 것
	output.vLocalPos = vPos;

	return output;
}

PS_OUTPUT StandardTexNormalPS(VS_OUTPUT_TEX_NORMAL input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	// Alpha Texture를 사용할때
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
	else
	{
		_tagLightInfo	tMtrl = ComputeLight(input.vViewPos, input.vNormal);
		float4 vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, input.vUV) * g_vMaterialDiffuse;

		if (vColor.a == 0.0f)
		{
			clip(-1);
		}

		float3 vNormal = input.vNormal * g_fNormalScale;
		
		// Albedo
		if (g_iEffectFlag == 0)
		{
			output.vTarget0.rgb = vColor.rgb;
			output.vTarget0.a = vColor.a;
		}
		// Burn
		if (g_iEffectFlag & 0x00000001)
		{
			float fProgress = g_NoiseTexture.Sample(g_DiffuseSampler, input.vUV).r;

			if (fProgress <= g_fFXBurnProgress)
				clip(-1);

			if (fProgress <= g_fFXBurnProgress + g_fFXBurnBorderRangePercentage)
			{
				float fLocalProgress = fProgress / (g_fFXBurnProgress + g_fFXBurnBorderRangePercentage);
				vColor = float4(g_vFXBurnColor.rgb, fLocalProgress);
			}
			output.vTarget0.rgb = vColor.rgb;
			output.vTarget0.a = vColor.a;
		}
		// LimLight
		if (g_iEffectFlag & 0x00000002)
		{
			output.vTarget0.rgb = ComputeRimLight(vColor.rgb, vNormal, input.vViewPos);
			output.vTarget0.a = vColor.a;
		}
		
		// Normal
		output.vTarget1.rgb = vNormal * 0.5f + 0.5f;
		output.vTarget1.a = 1.f;
		
		// Depth
		output.vTarget2.rgb = (float3)(input.vProjectionPos.z / input.vProjectionPos.w);
		output.vTarget2.a = input.vProjectionPos.w;
		
		//Specular Target
		float4	vMtrlSpc = g_vMaterialSpecular;

		if (g_vMaterialAmbient.a != 0.f)
		{
			vMtrlSpc = g_SpecularMapTexture.Sample(g_DiffuseSampler, input.vUV);
		}

		vMtrlSpc.a = g_vMaterialSpecular.a;
		output.vTarget3.r = ConvertColor(g_vMaterialDiffuse);
		output.vTarget3.g = ConvertColor(g_vMaterialAmbient);
		output.vTarget3.b = ConvertColor(vMtrlSpc);
		output.vTarget3.a = ConvertColor(g_vMaterialEmissive);
	}

	return output;
}




VS_OUTPUT_BUMP StandardBumpVS(VS_INPUT_BUMP input)
{
	VS_OUTPUT_BUMP	output = (VS_OUTPUT_BUMP)0;

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

PS_OUTPUT StandardBumpPS(VS_OUTPUT_BUMP input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	// Alpha Texture를 사용할때
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
		// Albedo
		if (g_iEffectFlag == 0)
		{
			output.vTarget0.rgb = vAlbedo.rgb;
			output.vTarget0.a = vAlbedo.a;
		}
		// Burn
		if (g_iEffectFlag & 0x00000001)
		{
			float fProgress = g_NoiseTexture.Sample(g_DiffuseSampler, input.vUV).r;

			if (fProgress <= g_fFXBurnProgress)
				clip(-1);

			if (fProgress <= g_fFXBurnProgress + g_fFXBurnBorderRangePercentage)
			{
				float fLocalProgress = fProgress / (g_fFXBurnProgress + g_fFXBurnBorderRangePercentage);
				vAlbedo = float4(g_vFXBurnColor.rgb, fLocalProgress);
			}
			output.vTarget0.rgb = vAlbedo.rgb;
			output.vTarget0.a = vAlbedo.a;
		}
		// LimLight
		if (g_iEffectFlag & 0x00000002)
		{
			output.vTarget0.rgb = ComputeRimLight(vAlbedo.rgb, vNormal, input.vViewPos);
			output.vTarget0.a = vAlbedo.a;
		}
		
		// Normal
		output.vTarget1.rgb = vNormal * 0.5f + 0.5f;
		output.vTarget1.a = 1.f;
		
		// Depth
		output.vTarget2.rgb = (float3)(input.vProjPos.z / input.vProjPos.w);
		output.vTarget2.a = input.vProjPos.w;
	
		// Specular Target
		// 색상 변환
		float4	vMtrlSpc = g_vMaterialSpecular;
	
		if (g_vMaterialAmbient.a != 0.f)
		{
			vMtrlSpc = g_SpecularMapTexture.Sample(g_DiffuseSampler, input.vUV);
		}
		
		vMtrlSpc.a = g_vMaterialSpecular.a;

		output.vTarget3.r = ConvertColor(g_vMaterialDiffuse);
		output.vTarget3.g = ConvertColor(g_vMaterialAmbient);
		output.vTarget3.b = ConvertColor(vMtrlSpc);
		output.vTarget3.a = ConvertColor(g_vMaterialEmissive);

		output.vTarget4.xyz = input.vTangent * 0.5f + 0.5f;
		output.vTarget5.xyz = input.vBinormal * 0.5f + 0.5f;
		output.vTarget4.w = 1.f;
		output.vTarget5.w = 1.f;
	}
	return output;
}



VS_OUTPUT_TEX_NORMAL StandardTexNormalAnimVS(VS_INPUT_TEX_NORMAL_ANIM input)
{
	VS_OUTPUT_TEX_NORMAL	output = (VS_OUTPUT_TEX_NORMAL)0;

	_tagSkinning tSkinning = Skinning(input.vPos, input.vNormal, input.vWeights, input.vIndices);

	float3	vPos = tSkinning.vPos - g_vPivot * g_vMeshLength;

	output.vProjectionPos = mul(float4(vPos, 1.f), g_matWVP);
	output.vPos = output.vProjectionPos;
	// Normal은 뷰공간으로 만들어준다.
	output.vNormal = mul(float4(tSkinning.vNormal, 0.f), g_matWV).xyz;
	output.vNormal = normalize(output.vNormal);
	output.vViewPos = mul(float4(vPos, 1.f), g_matWV).xyz;
	output.vUV = input.vUV;

	return output;
}

VS_OUTPUT_BUMP StandardBumpAnimVS(VS_INPUT_BUMP_ANIM input)
{
	VS_OUTPUT_BUMP	output = (VS_OUTPUT_BUMP)0;

	_tagSkinning tSkinning = Skinning(input.vPos, input.vNormal, input.vTangent, input.vBinormal, input.vWeights, input.vIndices);
	float3 vPos = tSkinning.vPos - g_vPivot * g_vMeshLength;

	output.vProjPos = mul(float4(vPos, 1.f), g_matWVP);
	output.vPos = output.vProjPos;
	// Normal은 뷰공간으로 만들어준다.
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

// Shadow Map 생성단계..
// 버텍스 정보의 처음 float3는 무조건 Position 이기 때문에 VS_INPUT_POS 를 InputLayout으로 사용..
VS_OUTPUT_POS ShadowVS(VS_INPUT_POS input)
{
	VS_OUTPUT_POS	output = (VS_OUTPUT_POS)0;

	float3	vPos = input.vPos - g_vPivot * g_vMeshLength;

	output.vProjPos = mul(float4(vPos, 1.f), g_matShadowWVP);
	output.vPos = output.vProjPos;

	return output;
}

// Animation 쓰는 경우..
VS_OUTPUT_POS ShadowTexNormalAnimVS(VS_INPUT_TEX_NORMAL_ANIM input)
{
	VS_OUTPUT_POS	output = (VS_OUTPUT_POS)0;

	_tagSkinning tSkinning = Skinning(input.vPos, input.vNormal, input.vWeights, input.vIndices);

	float3	vPos = tSkinning.vPos - g_vPivot * g_vMeshLength;

	output.vProjPos = mul(float4(vPos, 1.f), g_matShadowWVP);
	output.vPos = output.vProjPos;

	return output;
}

// Animation 쓰는 경우..
VS_OUTPUT_POS ShadowBumpAnimVS(VS_INPUT_BUMP_ANIM input)
{
	VS_OUTPUT_POS	output = (VS_OUTPUT_POS)0;

	_tagSkinning tSkinning = Skinning(input.vPos, input.vNormal, input.vTangent, input.vBinormal, input.vWeights, input.vIndices);

	float3 vPos = tSkinning.vPos - g_vPivot * g_vMeshLength;

	output.vProjPos = mul(float4(vPos, 1.f), g_matShadowWVP);
	output.vPos = output.vProjPos;

	return output;
}

PS_OUTPUT_SINGLE ShadowPS(VS_OUTPUT_POS input)
{
	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;

	output.vTarget.rgb = (float3)(input.vProjPos.z / input.vProjPos.w);
	output.vTarget.a = input.vProjPos.w;

	return output;
}
