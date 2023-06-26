//////////////////////////////////////////
// 
//	VL_PC.hlsl
//		선으로 그릴 도형을 위한 Layout용 Shader
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