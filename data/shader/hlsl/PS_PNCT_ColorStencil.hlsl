//////////////////////////////////////
//
// PS_PNCT_ColorStencil.hlsl
//		PNCT���� ������ ����ϴ� �÷� ���ٽ� ���Կ� Pixel shader
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

	float4 worldVPos	: TEXCOORD1;	//����Ʈ ���⺤�� ������ ���� ���� ���� ��ġ
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
