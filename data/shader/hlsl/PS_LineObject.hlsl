//////////////////////////////////////////
// 
//	PS_LineObject.hlsl
//		선으로 그릴 도형을 위한 Pixel Shader
//		VS_LineObject.hlsl과 짝
// 
//////////////////////////////////////////

struct PS_INPUT
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR0;
};

struct PS_OUTPUT
{
	float4 color	: SV_TARGET0;
	float4 normal	: SV_TARGET2;
};

PS_OUTPUT PS(PS_INPUT pIn)
{
	PS_OUTPUT finalPixel = (PS_OUTPUT)0; 
	finalPixel.color = pIn.color;
	finalPixel.normal = 0;

	return finalPixel;
}