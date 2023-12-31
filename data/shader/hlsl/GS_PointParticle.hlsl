//////////////////////////////////////
//
// GS_PointParticle.hlsl
//		PointParticle을 위한 Gemometry Shader
//		VS에서 입력으로 받은 단일 정점을 GS셰이더에서 면으로 변경해 트랜스폼을 적용한다.
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

//PS단계에서 필요한 것만 넘겨준다
struct GS_OUTPUT
{
	float4 pos				: SV_POSITION;
	float3 normal			: NORMAL0;
	float4 color			: COLOR0;
	float2 uvCoord			: TEXCOORD0;
	float2 spriteUvCoord	: TEXCOORD1;
};

//GS는 반환값이 없다.
//GS단계의 한번의 작업으로 늘어나는 정점개수의 상한을 지정한다.
[maxvertexcount(MAX_GS_VERTEXCOUNT)]
void GS(in point GS_INPUT gIn[1], inout TriangleStream<GS_OUTPUT> gOut)
{
	//출력인 TriangleStream은 출력 Primitive Topology가 Strip타입이다.
	//출력을 일정 단위로 끊고 싶은 경우 작업 요소 단위 별로 출력후 추가 기하요소에 대해
	//RestartStrip()함수를 호출해 연결 관계를 끊는 작업이 필요하다.

	//추가하고자 하는 정점을 새로 만든다. 당연히 정점 구조를 출력형식에 맞춘다.
	//시스템에서의 삼각형 스트립의 생성은 다음의 구조를 따른다.
	//    v1 -  v3 -  v5 
	//  /	\  /  \  /   \  ...
	// v0  - v2  - v4  - v6

	// v0 -> v1 -> v2	0번
	// v1 -> v3 -> v2	1번
	// v2 -> v3 -> v4	2번	...

	//활성화 되어 있지 않으면 추가 안함
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
			//초기 사각형 정의
			newV.pos = float4(newP[i].xyz, 1.0);
			newV.pos = mul(newV.pos, scale);
			newV.pos = mul(newV.pos, gIn[0].rot);

			//회전 빌보드 적용 : 월드 스케일 포함
			newV.pos = mul(newV.pos, matBillboard);

			//중복 적용 스케일 나누기
			newV.pos.xyz = newV.pos.xyz / float3(matWorld._11, matWorld._22, matWorld._33);

			//월드 행렬 반영
			newV.pos = mul(newV.pos, matWorld);

			//해당 위치에서의 로컬 오프셋 추가
			newV.pos.xyz += gIn[0].pos.xyz;

			//정점 정보 구조체 채우기
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