//////////////////////////////////////
//
// GS_PointParticle.hlsl
//		PointParticle�� ���� Gemometry Shader
//		VS���� �Է����� ���� ���� ������ GS���̴����� ������ ������ Ʈ�������� �����Ѵ�.
// 
//////////////////////////////////////

#define MAX_GS_VERTEXCOUNT 4

cbuffer CBUF_COORDCONV_MATSET : register(b0)
{
	matrix matWorld			: register(c0);
	matrix matInvWorld		: register(c4);
	matrix matView			: register(c8);
	matrix matProj			: register(c12);
};

cbuffer CBUF_BILLBOARD_MAT : register(b1)
{
	matrix matBillboard		: register(c0);
};

struct GS_INPUT
{
	float4		pos			: SV_POSITION;
	float3		normal		: NORMAL0;
	float4		color		: COLOR0;
	float2		uvCoord		: TEXCOORD0;
	float4		spriteRT	: SPRITERT0;
	float4x4	rot			: ROTATION;
	float3		scale		: SCALE0;
};

//PS�ܰ迡�� �ʿ��� �͸� �Ѱ��ش�
struct GS_OUTPUT
{
	float4 pos				: SV_POSITION;
	float3 normal			: NORMAL0;
	float4 color			: COLOR0;
	float2 uvCoord			: TEXCOORD0;
	float2 spriteUvCoord	: TEXCOORD1;
};

//GS�� ��ȯ���� ����.
//GS�ܰ��� �ѹ��� �۾����� �þ�� ���������� ������ �����Ѵ�.
[maxvertexcount(MAX_GS_VERTEXCOUNT)]
void GS(in point GS_INPUT gIn[1], inout TriangleStream<GS_OUTPUT> gOut)
{
	//����� TriangleStream�� ��� Primitive Topology�� StripŸ���̴�.
	//����� ���� ������ ���� ���� ��� �۾� ��� ���� ���� ����� �߰� ���Ͽ�ҿ� ����
	//RestartStrip()�Լ��� ȣ���� ���� ���踦 ���� �۾��� �ʿ��ϴ�.

	//�߰��ϰ��� �ϴ� ������ ���� �����. �翬�� ���� ������ ������Ŀ� �����.
	//�ý��ۿ����� �ﰢ�� ��Ʈ���� ������ ������ ������ ������.
	//    v1 -  v3 -  v5 
	//  /	\  /  \  /   \  ...
	// v0  - v2  - v4  - v6

	// v0 -> v1 -> v2	0��
	// v1 -> v3 -> v2	1��
	// v2 -> v3 -> v4	2��	...

	//Ȱ��ȭ �Ǿ� ���� ������ �߰� ����
	if (gIn[0].scale.z > 0.0f)
	{
		float3 newP[4] = {
			{-0.5f, 0.5f, 0.0f},
			{0.5f, 0.5f, 0.0f},
			{-0.5f, -0.5f, 0.0f},
			{0.5f, -0.5f, 0.0f}
		};

		float2 newT[4] = {
			{gIn[0].spriteRT.x, gIn[0].spriteRT.y},
			{gIn[0].spriteRT.z, gIn[0].spriteRT.y},
			{gIn[0].spriteRT.x, gIn[0].spriteRT.w},
			{gIn[0].spriteRT.z, gIn[0].spriteRT.w}
		};

		matrix scale = {
			gIn[0].scale.x, 0.0f, 0.0f, 0.0f,
			0.0f, gIn[0].scale.y, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		GS_OUTPUT newV;

		for (int i = 0; i < MAX_GS_VERTEXCOUNT; i++)
		{
			//�ʱ� �簢�� ����
			newV.pos = float4(newP[i].xyz, 1.0);
			newV.pos = mul(newV.pos, scale);
			newV.pos = mul(newV.pos, gIn[0].rot);

			//ȸ�� ������ ���� : ���� ������ ����
			newV.pos = mul(newV.pos, matBillboard);

			//�ߺ� ���� ������ ������
			newV.pos.xyz = newV.pos.xyz / float3(matWorld._11, matWorld._22, matWorld._33);

			//���� ��� �ݿ�
			newV.pos = mul(newV.pos, matWorld);

			//�ش� ��ġ������ ���� ������ �߰�
			newV.pos.xyz += gIn[0].pos.xyz;

			//���� ���� ����ü ä���
			newV.normal = gIn[0].normal;
			newV.color = gIn[0].color;
			newV.uvCoord = gIn[0].uvCoord;
			newV.spriteUvCoord = newT[i];

			newV.pos = mul(newV.pos, matView);
			newV.pos = mul(newV.pos, matProj);

			gOut.Append(newV);
		}

		gOut.RestartStrip();
	}
}