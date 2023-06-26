//////////////////////////////////////
//
// PS_PNCT_OutlineTexVColor.hlsl
//		PNCT구조를 사용하는 기본 Pixel shader
//		텍스처의 색상에 정점의 색상을 곱해 외곽선 위치에 따라 마지막 색상을 결정한다.
// 
//////////////////////////////////////

Texture2D g_tex0 : register(t0);
Texture2D g_tex1 : register(t1);

SamplerState g_sampler : register(s0);

cbuffer CBUF_OUTLINE_RENDER_DATA : register(b0)
{
	float4  camPos				: packoffset(c0);
	float4  outlineColor		: packoffset(c1);
	float	screenWidth			: packoffset(c2.x);
	float	screenHeight		: packoffset(c2.y);
	float	Threshold			: packoffset(c2.z);
	float	dummy0_outline		: packoffset(c2.w);
};

cbuffer CBUF_MOTION_TRAIL_DATA : register(b1)
{
	float3			pos					: packoffset(c0.x);
	float			dummy0				: packoffset(c0.w);

	float			timer				: packoffset(c1.x);
	float			hitColorDuration	: packoffset(c1.y);
	float			dummy1				: packoffset(c1.z);
	float			dummy2				: packoffset(c1.w);
};

struct PS_INPUT
{
	float4 pos					: SV_POSITION;
	float3 normal				: NORMAL0;
	float4 color				: COLOR0;
	float2 uvCoord				: TEXCOORD0;

	float4 worldVPos			: TEXCOORD1;	//라이트 방향벡터 결정을 위한 월드 정점 위치

	float4 shadowData			: TEXCOORD2;
};

struct PS_OUTPUT
{
	float4 color	: SV_TARGET0;
	float4 normal	: SV_TARGET2;
};

//픽셀값의 변화량 반영 비율을 결정하는 미분 마스크

//소벨 필터
static float SobelXFilter[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
static float SobelYFilter[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };

//샤르 필터
static float ScharrXfilter[9] = { -3, 0, 3, -10, 0, 10, -3, 0, 3 };
static float ScharrYfilter[9] = { -3, -10, -3, 0, 0, 0, -3, -10, -3 };

//라플라시안 필터
static float LaplasianFilter_Mid4[9] = { 0, -1, 0, -1, 4, -1, 0, -1, 0 };
static float LaplasianFilter_Mid8[9] = { -1, -1, -1, -1, 8, -1, -1, -1, -1 };

float4 calculateRimRight(float4 cRimColor, float3 vNormal, float3 vCamDir, float fAdjustIntesity)
{
	normalize(vCamDir);

	float RimPower = 1.0f - saturate(dot(-vCamDir, vNormal) - fAdjustIntesity);

	return cRimColor * RimPower;
}

PS_OUTPUT PS(PS_INPUT pIn)
{
	float4 texColor = g_tex0.Sample(g_sampler, pIn.uvCoord) * pIn.color;

	float dx = 1.0f / screenWidth;
	float dy = 1.0f / screenHeight;

	float ambientLightIntencity = 0.5f;

	float4 finalColor = texColor * ambientLightIntencity;
	finalColor.a = 1.0f;

	//외곽선 검출 필터를 이용한 외곽선 추출

	//림 라이트
	float4 RimColor = calculateRimRight(float4(1.0f, 0.0f, 0.0f, 1.0f),
		pIn.worldVPos.xyz - camPos.xyz,
		pIn.normal,
		0.5f);

	PS_OUTPUT finalPixel = (PS_OUTPUT)0;
	finalPixel.color = finalColor + RimColor * (1.0f - smoothstep(0.0f, hitColorDuration, timer));
	finalPixel.normal = float4(pIn.normal, 1.0f);

	return finalPixel;
}