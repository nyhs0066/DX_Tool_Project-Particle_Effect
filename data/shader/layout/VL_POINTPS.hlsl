//////////////////////////////////////
//
// VL_POINTPS.hlsl
//		PointParticle을 위한 Layout용 Shader
//
//////////////////////////////////////

struct VS_INPUT
{
	float3 pos			: POSITION0;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;
	float4 spriteRT		: SPRITERT0;
	float4x4 rot		: ROTATION;
	float3 scale		: SCALE0;
};

float4 VSLayout(VS_INPUT vIn) : SV_POSITION
{
	return float4(vIn.pos, 1.0f);
}