//////////////////////////////////////////
// 
//	VS_LineObject.hlsl
//		선으로 그릴 도형을 위한 Vertex Shader
//		PS_LineObject.hlsl과 짝
// 
//////////////////////////////////////////

cbuffer CBUF_COORDCONV_MATSET : register(b0)
{
	matrix matWorld			: register(c0);
	matrix matInvWorld		: register(c4);
	matrix matView			: register(c8);
	matrix matProj			: register(c12);
};
struct VS_INPUT
{
	float3 pos		: POSITION0;
	float4 color	: COLOR0;
};

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION0;
	float4 color	: COLOR0;
};

VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT vOut = (VS_OUTPUT)0;

	vOut.pos = float4(vIn.pos, 1.0f);
	vOut.pos = mul(vOut.pos, matWorld);
	vOut.pos = mul(vOut.pos, matView);
	vOut.pos = mul(vOut.pos, matProj);

	vOut.color = vIn.color;

	return vOut;
}