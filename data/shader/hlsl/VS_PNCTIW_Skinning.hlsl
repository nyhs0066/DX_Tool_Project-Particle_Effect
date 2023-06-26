//////////////////////////////////////
//
// VS_PNCTIW_Skinning.hlsl
//		PNCT + IW���� ������ ����ϴ� ��Ű�� �ִϸ��̼��� �����ϰų� ��Ų������ ������
//		FBX�� ���� Vertex shader
//		��Ű�� �ִϸ��̼� ����� ����ġ�� ���� ���� 4���� ��ĸ� �ݿ��Ѵ�.
//		register(c0) == packoffset(c0)
// 
//////////////////////////////////////

#define MAX_NUMBER_OF_MATRIX 256

cbuffer CBUF_COORDCONV_MATSET : register(b0)
{
	matrix matWorld			: register(c0);
	matrix matInvWorld		: register(c4);
	matrix matView			: register(c8);
	matrix matProj			: register(c12);
};

cbuffer CBuf_AnimMat : register(b1)
{
	matrix g_animMat[MAX_NUMBER_OF_MATRIX] : packoffset(c0);
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
	float4 boneIndex	: INDEX0;
	float4 weight		: WEIGHT0;
};

struct VS_OUTPUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;

	float4 worldVPos	: TEXCOORD1;	//����Ʈ ���⺤�� ������ ���� ���� ���� ��ġ

	float4 shadowData	: TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	float4 vPos = float4(vIn.pos, 1.0f);

	float4x4 matWorldAnim = 0;
	float3x3 matWorldRot = 0;

	//����ġ�� �ջ��� 1�̾���Ѵ�.
	for (int idx = 0; idx < 4; idx++)
	{
		//��� ����ȭ
		matWorldAnim += g_animMat[vIn.boneIndex[idx]] * vIn.weight[idx];
	}

	matWorldAnim = mul(matWorldAnim, matWorld);

	//�븻�� ������ ��ġ�� ���� �����ϰ� ȸ�� ��Ļ��̴�.
	//�����ϰ� ȸ�� ����� ���� ����̹Ƿ� ��ġ��� �������� ����� ������ ����� �� ������
	//��� ���۷� �ѱ涧 ��ġ����� �ѱ�Ƿ� ���� ����� ȸ�� ��ĺθ� �״�� ���������� �븻 ��ȯ�� �����ϴ�.
	matWorldRot = (float3x3)matWorldAnim;

	output.pos = mul(vPos, matWorldAnim);
	output.worldVPos = output.pos;

	//��� �ʿ� ��������� ���� �������� �ʱ⿡ �׳� ��ġ ��ǥ�� �޾Ƽ� �׸��� ����� ���ص� �ȴ�.
	output.shadowData = mul(output.pos, cbMatShadow);

	output.pos = mul(output.pos, matView);
	output.pos = mul(output.pos, matProj);

	output.normal = mul(vIn.normal, matWorldRot);
	output.normal = normalize(output.normal);

	output.color = vIn.color;
	output.uvCoord = vIn.uvCoord;

	return output;
}
