//////////////////////////////////////////
// 
//	VL_PC.hlsl
//		������ �׸� ������ ���� Layout�� Shader
// 
//////////////////////////////////////////

struct VS_INPUT
{
	float3 pos		: POSITION0;
	float4 color	: COLOR0;
};

float4 VSLayout(VS_INPUT vIn) : SV_POSITION
{
	return float4(vIn.pos, 1.0f);
}