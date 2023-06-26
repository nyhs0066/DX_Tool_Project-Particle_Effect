//////////////////////////////////////
//
// PS_PNCT_OutlineNoTex.hlsl
//		PNCT정점 구조를 사용하는 Pixel shader
//		외곽선 판정이 나지 않는경우 정점 컬러를 그대로 반환한다.
// 
//////////////////////////////////////

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

float4 calculateRimRight(float4 cRimColor, float3 vNormal, float3 vCamDir, float fAdjustIntesity)
{
	normalize(vCamDir);

	float RimPower = 1.0f - saturate(dot(-vCamDir, vNormal) - fAdjustIntesity);

	return cRimColor * RimPower;
}

PS_OUTPUT PS(PS_INPUT pIn)
{
	float4 VColor = pIn.color;

	float ambientLightIntencity = 0.5f;

	float4 finalColor = VColor * ambientLightIntencity;

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