//////////////////////////////////////
//
// PS_PointParticle_Alphablend.hlsl
//		PointParticle�� ���� ���� ���� ���� Pixel shader
//		���İ��� ���� �ؽ�ó�� ���� ó���Ѵ�.
// 
//////////////////////////////////////

//PS�ܰ迡�� �ʿ��� �͸� �Ѱ��ش�
struct PS_INPUT
{
	float4 pos				: SV_POSITION;
	float3 normal			: NORMAL0;
	float4 color			: COLOR0;
	float2 uvCoord			: TEXCOORD0;
	float2 spriteUvCoord	: TEXCOORD1;
};

struct PS_OUTPUT
{
	float4 color	: SV_TARGET0;
	float4 normal	: SV_TARGET2;
};

Texture2D g_tex0 : register(t0);
Texture2D g_tex1 : register(t1);

SamplerState g_sampler : register(s0);


//�� PS�ܰ迡���� ���� ������ �ݿ��Ѵ�.
//PS�� �Է� �μ��� GS�ܰ迡�� �þ ������ ������ �ݿ��Ѵ�.
//���ε� ���� ���� ���̴� �ؽ�ó ������ ���ø��� ���� (0, 0, 0, 0)�� ������ �ȴ�.
PS_OUTPUT PS(PS_INPUT pIn)
{
	PS_OUTPUT finalPixel = (PS_OUTPUT)0;
	finalPixel.color = g_tex0.Sample(g_sampler, pIn.spriteUvCoord) * pIn.color;
	finalPixel.normal = float4(pIn.normal, 1.0f);

	return finalPixel;
}

