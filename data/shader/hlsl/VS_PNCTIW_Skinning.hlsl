//////////////////////////////////////
//
// VS_PNCTIW_Skinning.hlsl
//		PNCT + IW정점 구조를 사용하는 스키닝 애니메이션을 수행하거나 스킨정보를 포함한
//		FBX를 위한 Vertex shader
//		스키닝 애니메이션 행렬은 가중치가 높은 상위 4개의 행렬만 반영한다.
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

	float4 worldVPos	: TEXCOORD1;	//라이트 방향벡터 결정을 위한 월드 정점 위치

	float4 shadowData	: TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	float4 vPos = float4(vIn.pos, 1.0f);

	float4x4 matWorldAnim = 0;
	float3x3 matWorldRot = 0;

	//가중치의 합산이 1이어야한다.
	for (int idx = 0; idx < 4; idx++)
	{
		//행렬 간소화
		matWorldAnim += g_animMat[vIn.boneIndex[idx]] * vIn.weight[idx];
	}

	matWorldAnim = mul(matWorldAnim, matWorld);

	//노말에 영향을 미치는 것은 스케일과 회전 행렬뿐이다.
	//스케일과 회전 행렬은 직교 행렬이므로 전치행렬 연산으로 역행렬 연산을 대신할 수 있으며
	//상수 버퍼로 넘길때 전치행렬을 넘기므로 월드 행렬의 회전 행렬부를 그대로 곱해줌으로 노말 변환이 가능하다.
	matWorldRot = (float3x3)matWorldAnim;

	output.pos = mul(vPos, matWorldAnim);
	output.worldVPos = output.pos;

	//사실 맵에 월드행렬을 따로 적용하지 않기에 그냥 위치 좌표를 받아서 그림자 행렬을 곱해도 된다.
	output.shadowData = mul(output.pos, cbMatShadow);

	output.pos = mul(output.pos, matView);
	output.pos = mul(output.pos, matProj);

	output.normal = mul(vIn.normal, matWorldRot);
	output.normal = normalize(output.normal);

	output.color = vIn.color;
	output.uvCoord = vIn.uvCoord;

	return output;
}
