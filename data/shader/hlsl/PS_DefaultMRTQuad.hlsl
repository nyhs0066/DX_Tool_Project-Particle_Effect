//////////////////////////////////////
//
// PS_DefaultMRTQuad.hlsl
//		PNCT정점 구조를 사용하는 MRT용 Pixel shader
// 
//////////////////////////////////////

cbuffer CBUF_OUTLINE_RENDER_DATA : register(b0)
{
	float4  camPos				: packoffset(c0);
	float4  outlineColor		: packoffset(c1);
	float	screenWidth			: packoffset(c2.x);
	float	screenHeight		: packoffset(c2.y);
	float	Threshold			: packoffset(c2.z);
	float	OutlineThickness	: packoffset(c2.w);
};

Texture2D		g_colorMap			: register(t0);
//Texture2D		g_dualSourceMap		: register(t1); t1 : Not Used
Texture2D		g_normalMap			: register(t2);
Texture2D		g_colorStencilMap	: register(t3);
Texture2D		g_depthStencilMap	: register(t4);

SamplerState	g_SP_LINEAR_WRAP		: register(s0);
SamplerState	g_SP_LINEAR_CLAMP		: register(s1);

struct PS_INPUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;
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

//비교 픽셀 오프셋
static float PixelOffset[3] = { -1, 0, 1 };

float4 PS(PS_INPUT pIn) : SV_TARGET
{
	float4 finalColor = g_colorMap.Sample(g_SP_LINEAR_WRAP, pIn.uvCoord);
	finalColor.w = 1.0f;

	//float4 pixelColor = 0;
	//float delta = 0;

	//float dx = 0;
	//float dy = 0;

	//float2 UVCoord = 0;

	//float4 filterResX = 0;
	//float4 filterResY = 0;

	////외곽선 검출 필터를 이용한 외곽선 추출
	//for (int i = 0; i < 3; i++)
	//{
	//	dy = (PixelOffset[i] * OutlineThickness) / screenHeight;

	//	for (int j = 0; j < 3; j++)
	//	{
	//		dx = (PixelOffset[j] * OutlineThickness) / screenWidth;

	//		UVCoord = float2(pIn.uvCoord.x + dx, pIn.uvCoord.y + dy);

	//		pixelColor = g_colorStencilMap.Sample(g_SP_LINEAR_CLAMP, UVCoord);

	//		filterResX += LaplasianFilter_Mid8[i * 3 + j] * pixelColor;
	//		//filterResY += SobelYFilter[i * 3 + j] * pixelColor;
	//	}
	//}

	//delta = ((filterResX.x + filterResX.y + filterResX.z) + (filterResY.x + filterResY.y + filterResY.z)) / 3.0f;

	//if (delta > Threshold)
	//{
	//	finalColor = outlineColor;
	//}

	return finalColor;
}
