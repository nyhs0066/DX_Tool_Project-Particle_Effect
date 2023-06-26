//////////////////////////////////////
//
// VS_DepthMapShadow.hlsl
//		PNCT���� ������ ����ϴ� �׸��� ó���� ���� Shader
//		���� ���� �̿��� �׸��� �����͸� �����Ѵ�.
//		�ʿ��� �׽�Ʈ������ ����� ���̹Ƿ� ���� ��ȭ�� ���� �� �ִ�.
// 
//////////////////////////////////////

cbuffer CBUF_COORDCONV_MATSET : register(b0)
{
	matrix matWorld			: register(c0);
	matrix matInvWorld		: register(c4);
	matrix matView			: register(c8);
	matrix matProj			: register(c12);
};

cbuffer CBUF_SHADOW_MATSET : register(b2)
{
	matrix cbMatShadow	: register(c0);
}

struct VS_INPUT
{
	float3 pos			: POSITION0;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos				: SV_POSITION;
	float3 normal			: NORMAL0;
	float4 color			: COLOR0;
	float2 uvCoord			: TEXCOORD0;
	float4 worldVPos		: TEXCOORD1;	//����Ʈ ���⺤�� ������ ���� ���� ���� ��ġ

	float4 shadowData		: TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos = float4(vIn.pos, 1.0f);
	output.pos = mul(output.pos, matWorld);
	output.worldVPos = output.pos;

	//��� �ʿ� ��������� ���� �������� �ʱ⿡ �׳� ��ġ ��ǥ�� �޾Ƽ� �׸��� ����� ���ص� �ȴ�.
	output.shadowData = mul(output.pos, cbMatShadow);

	output.pos = mul(output.pos, matView);
	output.pos = mul(output.pos, matProj);

	output.normal = mul(vIn.normal, (float3x3)matWorld);
	output.normal = normalize(output.normal);

	output.color = vIn.color;
	output.uvCoord = vIn.uvCoord;

	return output;
}
