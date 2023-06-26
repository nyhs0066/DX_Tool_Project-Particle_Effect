//////////////////////////////////////
//
// PS_PNCT_Light.hlsl
//		PNCT���� ������ ����ϴ� ������ ������ �⺻ ����Ʈ ó���� Pixel shader
//		���� ����
// 
//////////////////////////////////////

#define FZERO_RANGE						0.0005f
#define SMAP_SIZE						1024
#define MAX_NUMBER_OF_LIGHTS			256
#define LIGHT_ATTENUATION_APPLY_RANGE	10.0f
#define NUMBER_OF_ENABLE_LIGHTS			4

//#define USE_HALFVECTOR

cbuffer CBUF_MATERIAL_INFO : register(b0)
{
	float4	g_ambientMaterial		: register(c0);
	float4	g_ambientLightColor		: register(c1);
	float4	g_diffuseMaterial		: register(c2);
	float4	g_diffuseLightColor		: register(c3);
	float4	g_specularMaterial		: register(c4);
	float4	g_specularLightColor	: register(c5);
};

cbuffer CBUF_LIGHT_RENDER_OPTION  : register(b1)
{
	float4	g_viewDirection;
	int		g_numberOfLight;
	float	g_reflectionFactor;
	float	g_deltaTime;
	float	g_dummy0;
};

cbuffer CBUF_LIGHT_INFO : register(b2)
{
	struct
	{
		float4	lightPos;
		float4	lightDirection;
		float4	lightColor;
		int		lightType;
		float	pointLightRange;
		float	g_dummy0;
		float	dummy1;
	} g_LInfo[MAX_NUMBER_OF_LIGHTS] : register(c0);
};

cbuffer CBUF_OUTLINE_RENDER_DATA : register(b3)
{
	float4  camPos			: packoffset(c0);
	float4  outlineColor	: packoffset(c1);
	float	dotPassBound : packoffset(c2.x);
	float	dummy1 : packoffset(c2.y);
	float	dummy2 : packoffset(c2.z);
	float	dummy3 : packoffset(c2.w);
};

struct PS_INPUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;
	float4 worldVPos	: TEXCOORD1;	//����Ʈ ���⺤�� ������ ���� ���� ���� ��ġ

	float4 shadowData	: TEXCOORD2;
};

struct PS_OUTPUT
{
	float4 color	: SV_TARGET0;
	float4 normal	: SV_TARGET2;
};

Texture2D		g_diffuseTex	: register(t0);
Texture2D		g_ExtraTex		: register(t1);
Texture2DArray	g_shadowMap0	: register(t2);	//ShadowMap : ���� �� �ؽ�ó

sampler g_linearClamp : register(s0);
sampler g_linearWrap : register(s1);

//PCF(Percentage Closer Filtering)�� �����ϱ� ���� �� ���÷�
SamplerComparisonState g_smapCompSampler : register(s2);

float CalculateShadowAttenuation(PS_INPUT pIn)
{
	//�׸��� ���ο� ������ �����ϱ� ���� �� ����
	float3 shadowUnit = pIn.shadowData.xyz / pIn.shadowData.w;

	//SamplerComparisonState�� �̿��� �׸��� ����
	//Ŀ���� ���� ������ Ȧ���� ��� ���� �ٶ���
	const float TexelDist = 1.0f / SMAP_SIZE;
	int numOfKernel = 9;	//n * n
	float fLightIntensity = 0.0f;

	int iHalf = (numOfKernel - 1) / 2;

	for (int v = -iHalf; v <= iHalf; v++)
	{
		for (int u = -iHalf; u <= iHalf; u++)
		{
			float2 vOffset = float2(u * TexelDist, v * TexelDist);

			//D3D11_COMPARISON_LESS�� �Ǿ��ִ�.
			//sampleTarget < shadowUnit.z
			//SampleCmpLevelZero() MipMapLOD0������ ���Ѵ�.
			fLightIntensity +=
				g_shadowMap0.SampleCmpLevelZero(g_smapCompSampler,
					float3(shadowUnit.xy + vOffset, 0.0f),
					shadowUnit.z - FZERO_RANGE).r;
		}
	}

	fLightIntensity /= numOfKernel * numOfKernel;

	return max(0.5f, fLightIntensity);
}

float4 calculateDiffuseLight(PS_INPUT pIn)
{
	float	LightIntensity = 0;
	float4	lightColor = 0;
	float	DiffuseLightIntensity = 0;
	float	fLuminance = 0;
	float	dist = 0;
	float3	direction = 0;

	[unroll(NUMBER_OF_ENABLE_LIGHTS)]
	for (int i = 0; i < g_numberOfLight; i++)
	{
		if (g_LInfo[i].lightType == 0)
		{
			direction = g_LInfo[i].lightDirection.xyz;
		}
		else if (g_LInfo[i].lightType == 1)
		{
			dist = distance(pIn.worldVPos.xyz, g_LInfo[i].lightPos.xyz);
			direction = normalize(pIn.worldVPos.xyz - g_LInfo[i].lightPos.xyz);
		}

		DiffuseLightIntensity = saturate(dot(-direction, pIn.normal));
		fLuminance = smoothstep(dist - LIGHT_ATTENUATION_APPLY_RANGE, dist, g_LInfo[i].pointLightRange);
		lightColor += g_LInfo[i].lightColor * DiffuseLightIntensity * fLuminance;
	}

	lightColor /= g_numberOfLight;

	return float4(g_diffuseMaterial.rgb * g_diffuseLightColor.rgb * lightColor.rgb, 1.0f);
}

float4 calculateSpecularLight(PS_INPUT pIn)
{
	float reflectExp = g_reflectionFactor;	//�ݻ� ����
	float LightIntensity = 0;
	float4 lightColor = 0;
	float fLuminance = 0;
	float dist = 0; 
	float specularLightIntensity = 0;
	float3 direction = 0;

	[unroll(NUMBER_OF_ENABLE_LIGHTS)]
	for (int i = 0; i < g_numberOfLight; i++)
	{
#ifndef USE_HALFVECTOR

		float3 vR = 0;

		if (g_LInfo[i].lightType == 0)
		{
			direction = g_LInfo[i].lightDirection.xyz;
			vR = reflect(direction, pIn.normal);	//�ݻ� ����
		}
		else if (g_LInfo[i].lightType == 1)
		{
			dist = distance(pIn.worldVPos.xyz, g_LInfo[i].lightPos.xyz);
			direction = normalize(pIn.worldVPos.xyz - g_LInfo[i].lightPos.xyz);
			vR = reflect(direction, pIn.normal);	//�ݻ� ����
		}

		fLuminance = smoothstep(dist - LIGHT_ATTENUATION_APPLY_RANGE, dist, g_LInfo[i].pointLightRange);
		specularLightIntensity = pow(saturate(dot(vR, -g_viewDirection.xyz)), reflectExp);	//�ݻ纤�ͷ� ���

		lightColor += g_LInfo[i].lightColor * specularLightIntensity * fLuminance;

#else //!USE_HALFVECTOR

		float3 vH = 0;

		if (g_LInfo[i].lightType == 0)
		{
			direction = g_LInfo[i].lightDirection.xyz;
			vH = normalize(normalize(-g_LInfo[i].lightDirection) + normalize(-g_viewDirection));	//���� ����
		}
		else if (g_LInfo[i].lightType == 1)
		{
			dist = distance(pIn.worldVPos.xyz, g_LInfo[i].lightPos.xyz);
			direction = normalize(pIn.worldVPos.xyz - g_LInfo[i].lightPos.xyz);
			vH = normalize(normalize(-direction) + normalize(-g_viewDirection));	//���� ����
		}

		fLuminance = smoothstep(dist - LIGHT_ATTENUATION_APPLY_RANGE, dist, g_LInfo[i].pointLightRange);
		specularLightIntensity = pow(saturate(dot(vH, pIn.normal)), reflectExp);		//�������ͷ� ���

		lightColor += g_LInfo[i].lightColor * specularLightIntensity * fLuminance;

#endif //USE_HALFVECTOR
	};

	lightColor /= g_numberOfLight;

	return float4(g_specularMaterial.rgb * g_specularLightColor.rgb * lightColor.rgb, 1.0f);
}

PS_OUTPUT PS(PS_INPUT pIn)
{
	//�ؽ�ó ����
	float4 texColor = g_diffuseTex.Sample(g_linearWrap, pIn.uvCoord);

	//�ֺ��� / Ȯ�걤 / �ݻ籤
	float4 AmbientLight = float4(g_ambientMaterial.rgb * g_ambientLightColor.rgb, 1.0f);
	float4 DiffuseLight = calculateDiffuseLight(pIn);
	float4 SpecularLight = calculateSpecularLight(pIn);
	float  ShadowAttenuation = CalculateShadowAttenuation(pIn);

	//���� ����
	float4 FinalColor = texColor * (AmbientLight + DiffuseLight + SpecularLight) * ShadowAttenuation;
	//float4 FinalColor = texColor * saturate(AmbientLight + DiffuseLight);
	FinalColor.a = 1.0f;

	float3 camDir = camPos.xyz - pIn.worldVPos.xyz;
	camDir = normalize(camDir);

	float invOutlineIntensity = saturate(dot(camDir, pIn.normal));

	if (invOutlineIntensity < dotPassBound)
	{
		FinalColor = outlineColor;
	}

	PS_OUTPUT finalPixel = (PS_OUTPUT)0;
	finalPixel.color = FinalColor;
	finalPixel.normal = float4(pIn.normal, 1.0f);

	return finalPixel;
}