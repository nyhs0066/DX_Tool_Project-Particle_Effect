//////////////////////////////////////
//
// PS_PNCT_NoTex.hlsl
//		PNCT���� ������ ����ϴ� Pixel shader
//		���� �÷��� �״�� ��ȯ�Ѵ�.
// 
//////////////////////////////////////

struct PS_INPUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;

	float4 worldVPos			: TEXCOORD1;	//����Ʈ ���⺤�� ������ ���� ���� ���� ��ġ
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