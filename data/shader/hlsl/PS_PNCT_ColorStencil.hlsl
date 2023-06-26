//////////////////////////////////////
//
// PS_PNCT_ColorStencil.hlsl
//		PNCT정점 구조를 사용하는 컬러 스텐실 기입용 Pixel shader
// 
//////////////////////////////////////

Texture2D g_tex0 : register(t0);
Texture2D g_tex1 : register(t1);

SamplerState g_sampler : register(s0);

struct PS_INPUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;

	float4 worldVPos	: TEXCOORD1;	//라이트 방향벡터 결정을 위한 월드 정점 위치
};

struct PS_OUTPUT
{
	float4 colorStencil		: SV_TARGET3;
};

PS_OUTPUT PS(PS_INPUT pIn)
{
	PS_OUTPUT finalPixel = (PS_OUTPUT)0;
	finalPixel.colorStencil = g_tex0.Sample(g_sampler, pIn.uvCoord);

	return finalPixel;
}
