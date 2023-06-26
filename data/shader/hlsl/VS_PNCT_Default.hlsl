//////////////////////////////////////
//
// VS_PNCT_Default.hlsl
//		PNCT���� ������ ����ϴ� ������ ������ Vertex shader
//		Ʈ�������� Vertex shader���� �ݿ��Ѵ�.
// 
//////////////////////////////////////

cbuffer CBUF_COORDCONV_MATSET : register(b0)
{
	matrix matWorld			: register(c0);
	matrix matInvWorld		: register(c4);
	matrix matView			: register(c8);
	matrix matProj			: register(c12);
};

struct VS_INPUT
{
	float3 pos			: POSITION0;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;

	float4 worldVPos	: TEXCOORD1;	//����Ʈ ���⺤�� ������ ���� ���� ���� ��ġ
};

VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos = float4(vIn.pos, 1.0f);
	output.pos = mul(output.pos, matWorld);
	output.worldVPos = output.pos;

	output.pos = mul(output.pos, matView);
	output.pos = mul(output.pos, matProj);

	output.normal = mul(vIn.normal, (float3x3)matWorld);
	output.color = vIn.color;
	output.uvCoord = vIn.uvCoord;

	return output;
}
