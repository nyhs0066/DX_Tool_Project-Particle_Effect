//////////////////////////////////////
//
// VL_PNCT.hlsl
//		PNCT���� ������ ����ϴ� Layout�� Shader
// 
//////////////////////////////////////

struct VS_INPUT
{
	float3 pos			: POSITION0;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;
};

float4 VSLayout(VS_INPUT vIn) : SV_POSITION
{
	return float4(vIn.pos, 1.0f);
}