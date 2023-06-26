//////////////////////////////////////
//
// PS_PNCT_DepthMapShadow.hlsl
//		PNCT���� ������ ����ϴ� �׸��� ó���� Pixel shader
//		���̸� �ؽ�ó�� �޾� ���ø��Ѵ�.
//		Border���÷��� �̿��� ������ ��� ������ ���� ��� ����ó���Ѵ�.
//		�� ���÷��� PCF�� �����Ѵ�.
// 
//////////////////////////////////////

#define FZERO_RANGE 0.0005f
#define SMAP_SIZE 1024

//�Ϲ� �ؽ�ó
Texture2D g_tex0 : register(t0);
Texture2D g_tex1 : register(t1);

//ShadowMap : ���� �� �ؽ�ó
Texture2D g_shadowMap0 : register(t2);

//�Ϲ� ���÷�
SamplerState g_sampler : register(s0);

//ShadowMap�� ���÷� : �ؽ�ó ���� �� ��ǥ������ ��� ��� ��ǥ�� ���� ����ó���� ���������ν�
//��� ������ ���� ǥ�ø� �����Ѵ�.
SamplerState g_shadowMapSampler : register(s1);

//PCF(Percentage Closer Filtering)�� �����ϱ� ���� �� ���÷�
SamplerComparisonState g_smapCompSampler : register(s2);

struct PS_INPUT
{
	float4 pos				: SV_POSITION;
	float3 normal			: NORMAL0;
	float4 color			: COLOR0;
	float2 uvCoord			: TEXCOORD0;
	float4 worldVPos		: TEXCOORD1;	//����Ʈ ���⺤�� ������ ���� ���� ���� ��ġ

	float4 shadowData		: TEXCOORD2;
};

struct PS_OUTPUT
{
	float4 color	: SV_TARGET0;
	float4 normal	: SV_TARGET2;
};

PS_OUTPUT PS(PS_INPUT pIn)
{
	//�׸��� ���ο� ������ �����ϱ� ���� �� ����
	float3 shadowUnit = pIn.shadowData.xyz / pIn.shadowData.w;
	float4 diffuseColor = g_tex0.Sample(g_sampler, pIn.uvCoord);

	/*
	//��� 1 : �������� �ٷ� ��
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
	//��� 2 : ���� �׸��� ���� => ���� ������ ��� ���ϴ��Ŀ� ���� ����� �޶����� ����
	//4���� �ؼ� - �̼��� ���͸��� ����ȿ���� ������
	//�ؼ��� ���� �ø��� ���� ���� ���� ȿ���� ��Ÿ������ �׸�ŭ ���귮�� ��������.

	//�ؼ��� ����
	float TexelDist = 1.0f / SMAP_SIZE;

	//�밢�� �ؼ� ������
	float2 texOffset[4] =
	{
		{-TexelDist, TexelDist},
		{TexelDist, TexelDist},
		{-TexelDist, -TexelDist},
		{TexelDist, -TexelDist}
	};

	//���� �ҽ� ��
	float interpSrc[4] =
	{
		(((shadowUnit.z > FZERO_RANGE + g_shadowMap0.Sample(g_shadowMapSampler, shadowUnit.xy + texOffset[0]).r)) ? 0.0f : 1.0f),
		(((shadowUnit.z > FZERO_RANGE + g_shadowMap0.Sample(g_shadowMapSampler, shadowUnit.xy + texOffset[1]).r)) ? 0.0f : 1.0f),
		(((shadowUnit.z > FZERO_RANGE + g_shadowMap0.Sample(g_shadowMapSampler, shadowUnit.xy + texOffset[2]).r)) ? 0.0f : 1.0f),
		(((shadowUnit.z > FZERO_RANGE + g_shadowMap0.Sample(g_shadowMapSampler, shadowUnit.xy + texOffset[3]).r)) ? 0.0f : 1.0f),
	};

	//lerp���� ������ ���� ���� �ؼ��� ��ġ�� ���Ⱚ
	//frac : ���� ���� �Ҽ��� ��ȯ
	float2 interpRatio = frac(shadowUnit.xy * SMAP_SIZE);

	float LightIntensity = lerp(lerp(interpSrc[0], interpSrc[1], interpRatio.x), lerp(interpSrc[2], interpSrc[3], interpRatio.x), interpRatio.y);

	float4 vFinalColor = diffuseColor * max(LightIntensity, 0.5f);
	vFinalColor.a = 1.0f;
	*/

	//��� 3 : SamplerComparisonState�� �̿��� �׸��� ����
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
			// sampleTarget < shadowUnit.z
			//SampleCmpLevelZero() MipMapLOD0������ ���Ѵ�.
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