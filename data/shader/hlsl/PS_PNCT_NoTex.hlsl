//////////////////////////////////////
//
// PS_PNCT_NoTex.hlsl
//		PNCT정점 구조를 사용하는 Pixel shader
//		정점 컬러를 그대로 반환한다.
// 
//////////////////////////////////////

struct PS_INPUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;

	float4 worldVPos			: TEXCOORD1;	//라이트 방향벡터 결정을 위한 월드 정점 위치
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
	finalPixel.normal = float4(pIn.normal, 1.0f);

	return finalPixel;
}