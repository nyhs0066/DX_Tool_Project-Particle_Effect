//////////////////////////////////////
//
// VS_DepthMapShadow.hlsl
//		PNCT정점 구조를 사용하는 그림자 처리를 위한 Shader
//		깊이 맵을 이용해 그림자 데이터를 생성한다.
//		맵에서 테스트용으로 사용한 것이므로 구조 변화가 있을 수 있다.
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
	float4 worldVPos		: TEXCOORD1;	//라이트 방향벡터 결정을 위한 월드 정점 위치

	float4 shadowData		: TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos = float4(vIn.pos, 1.0f);
	output.pos = mul(output.pos, matWorld);
	output.worldVPos = output.pos;

	//사실 맵에 월드행렬을 따로 적용하지 않기에 그냥 위치 좌표를 받아서 그림자 행렬을 곱해도 된다.
	output.shadowData = mul(output.pos, cbMatShadow);

	output.pos = mul(output.pos, matView);
	output.pos = mul(output.pos, matProj);

	output.normal = mul(vIn.normal, (float3x3)matWorld);
	output.normal = normalize(output.normal);

	output.color = vIn.color;
	output.uvCoord = vIn.uvCoord;

	return output;
}
