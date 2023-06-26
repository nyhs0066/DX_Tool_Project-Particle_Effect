//////////////////////////////////////
//
// PS_PNCT_DepthMapShadow.hlsl
//		PNCT정점 구조를 사용하는 그림자 처리용 Pixel shader
//		깊이맵 텍스처를 받아 샘플링한다.
//		Border샘플러를 이용해 범위를 벗어난 영역에 대해 모두 음영처리한다.
//		비교 샘플러는 PCF를 수행한다.
// 
//////////////////////////////////////

#define FZERO_RANGE 0.0005f
#define SMAP_SIZE 1024

//일반 텍스처
Texture2D g_tex0 : register(t0);
Texture2D g_tex1 : register(t1);

//ShadowMap : 깊이 맵 텍스처
Texture2D g_shadowMap0 : register(t2);

//일반 샘플러
SamplerState g_sampler : register(s0);

//ShadowMap용 샘플러 : 텍스처 맵의 본 좌표영역을 벗어난 모든 좌표에 대해 음영처리를 수행함으로써
//벗어난 영역에 대한 표시를 수행한다.
SamplerState g_shadowMapSampler : register(s1);

//PCF(Percentage Closer Filtering)를 수행하기 위한 비교 샘플러
SamplerComparisonState g_smapCompSampler : register(s2);

struct PS_INPUT
{
	float4 pos				: SV_POSITION;
	float3 normal			: NORMAL0;
	float4 color			: COLOR0;
	float2 uvCoord			: TEXCOORD0;
	float4 worldVPos		: TEXCOORD1;	//라이트 방향벡터 결정을 위한 월드 정점 위치

	float4 shadowData		: TEXCOORD2;
};

struct PS_OUTPUT
{
	float4 color	: SV_TARGET0;
	float4 normal	: SV_TARGET2;
};

PS_OUTPUT PS(PS_INPUT pIn)
{
	//그림자 여부와 색상을 결정하기 위한 비교 유닛
	float3 shadowUnit = pIn.shadowData.xyz / pIn.shadowData.w;
	float4 diffuseColor = g_tex0.Sample(g_sampler, pIn.uvCoord);

	/*
	//방법 1 : 보정없이 바로 비교
	float4 ShadowMapDepth = g_shadowMap0.Sample(g_shadowMapSampler, shadowUnit.xy);
	float4 vFinalColor = diffuseColor;

	if (shadowUnit.z + FZERO_RANGE > ShadowMapDepth.r)
	{
		vFinalColor = vFinalColor * 0.5f;
		vFinalColor.a = 1.0f;
	}

	return vFinalColor;
	*/

	/*
	//방법 2 : 직접 그림자 보간 => 오차 범위를 어디에 더하느냐에 따라 결과가 달라지니 주의
	//4개의 텍셀 - 이선형 필터링의 보간효과는 약한편
	//텍셀의 수를 늘리면 더욱 좋은 보간 효과가 나타나지만 그만큼 연산량이 많아진다.

	//텍셀간 간격
	float TexelDist = 1.0f / SMAP_SIZE;

	//대각선 텍셀 오프셋
	float2 texOffset[4] =
	{
		{-TexelDist, TexelDist},
		{TexelDist, TexelDist},
		{-TexelDist, -TexelDist},
		{TexelDist, -TexelDist}
	};

	//보간 소스 값
	float interpSrc[4] =
	{
		(((shadowUnit.z > FZERO_RANGE + g_shadowMap0.Sample(g_shadowMapSampler, shadowUnit.xy + texOffset[0]).r)) ? 0.0f : 1.0f),
		(((shadowUnit.z > FZERO_RANGE + g_shadowMap0.Sample(g_shadowMapSampler, shadowUnit.xy + texOffset[1]).r)) ? 0.0f : 1.0f),
		(((shadowUnit.z > FZERO_RANGE + g_shadowMap0.Sample(g_shadowMapSampler, shadowUnit.xy + texOffset[2]).r)) ? 0.0f : 1.0f),
		(((shadowUnit.z > FZERO_RANGE + g_shadowMap0.Sample(g_shadowMapSampler, shadowUnit.xy + texOffset[3]).r)) ? 0.0f : 1.0f),
	};

	//lerp강도 결정을 위한 현재 텍셀의 위치의 편향값
	//frac : 현재 값의 소수부 반환
	float2 interpRatio = frac(shadowUnit.xy * SMAP_SIZE);

	float LightIntensity = lerp(lerp(interpSrc[0], interpSrc[1], interpRatio.x), lerp(interpSrc[2], interpSrc[3], interpRatio.x), interpRatio.y);

	float4 vFinalColor = diffuseColor * max(LightIntensity, 0.5f);
	vFinalColor.a = 1.0f;
	*/

	//방법 3 : SamplerComparisonState를 이용한 그림자 보간
	//커널의 한줄 개수를 홀수로 잡는 것이 바람직

	const float TexelDist = 1.0f / SMAP_SIZE;
	int numOfKernel = 9;	//n * n
	float fLightIntensity = 0.0f;

	int iHalf = (numOfKernel - 1) / 2;

	for (int v = -iHalf; v <= iHalf; v++)
	{
		for (int u = -iHalf; u <= iHalf; u++)
		{
			float2 vOffset = float2(u * TexelDist, v * TexelDist);

			//D3D11_COMPARISON_LESS로 되어있다.
			// sampleTarget < shadowUnit.z
			//SampleCmpLevelZero() MipMapLOD0에서만 비교한다.
			fLightIntensity +=
				g_shadowMap0.SampleCmpLevelZero(g_smapCompSampler,
					shadowUnit.xy + vOffset,
					shadowUnit.z - FZERO_RANGE).r;
		}
	}

	fLightIntensity /= numOfKernel * numOfKernel;
	float4 vFinalColor = diffuseColor * max(0.5f, fLightIntensity);
	vFinalColor.a = 1.0f;

	PS_OUTPUT finalPixel = (PS_OUTPUT)0;
	finalPixel.color = vFinalColor;
	finalPixel.normal = float4(pIn.normal, 1.0f);

	return finalPixel;
}