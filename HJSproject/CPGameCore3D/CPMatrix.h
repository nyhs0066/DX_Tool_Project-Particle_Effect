#pragma once
#include "CPGCStd.h"
#include "CPVector.h"

struct CPVector2F;
struct CPVector3F;
struct CPVector4F;

struct CPSFloat3x3
{
	union
	{
		struct
		{
			FLOAT _11, _12, _13;
			FLOAT _21, _22, _23;
			FLOAT _31, _32, _33;
		};

		FLOAT m[3][3];
	};
};

struct CPSFloat4x4
{
	union
	{
		struct
		{
			FLOAT _11, _12, _13, _14;
			FLOAT _21, _22, _23, _24;
			FLOAT _31, _32, _33, _34;
			FLOAT _41, _42, _43, _44;
		};

		FLOAT m[4][4];
	};
};

class CPMatrix3x3 : public CPSFloat3x3
{
public:
	CPMatrix3x3();
	~CPMatrix3x3();

	void setIdentity();
	CPMatrix3x3 transpose();

	FLOAT calculateDeterminant();
	CPMatrix3x3 cofactor();
	CPMatrix3x3 inverse();

	CPMatrix3x3 operator+(const CPMatrix3x3& opd);
	CPMatrix3x3 operator-(const CPMatrix3x3& opd);
	CPMatrix3x3 operator*(FLOAT f);
	CPMatrix3x3 operator*(const CPMatrix3x3& opd);

	friend CPMatrix3x3 operator*(FLOAT f, const CPMatrix3x3& opd);

	//전치연산을 수행하므로 벡터 클래스에서만 행렬과의 곱셈 연산자를 재정의한다.
	//행렬을 후치 연산 할거면 행렬을 전치시켜 사용해야하는 것에 유의.
	static CPMatrix3x3 scale(FLOAT x, FLOAT y);
	static CPMatrix3x3 rotation(FLOAT fRad);
	static CPMatrix3x3 translation(FLOAT dx, FLOAT dy);
};

class CPMatrix4x4 : public CPSFloat4x4
{
public:
	CPMatrix4x4();
	~CPMatrix4x4();

	void setIdentity();
	CPMatrix4x4 transpose();

	FLOAT calculateDeterminant();
	CPMatrix4x4 cofactor();
	CPMatrix4x4 inverse();

	CPMatrix4x4 operator+(const CPMatrix4x4& opd);
	CPMatrix4x4 operator-(const CPMatrix4x4& opd);
	CPMatrix4x4 operator*(FLOAT f);
	CPMatrix4x4 operator*(const CPMatrix4x4& opd);
	CPMatrix4x4 operator*=(const CPMatrix4x4& opd);

	friend CPMatrix4x4 operator*(FLOAT f, const CPMatrix4x4& opd);

	//전치연산을 수행하므로 벡터 클래스에서만 행렬과의 곱셈 연산자를 재정의한다.
	//행렬을 후치 연산 할거면 행렬을 전치시켜 사용해야하는 것에 유의.
	static CPMatrix4x4 scale(FLOAT x, FLOAT y, FLOAT z);
	static CPMatrix4x4 rotationX(FLOAT fRad);
	static CPMatrix4x4 rotationY(FLOAT fRad);
	static CPMatrix4x4 rotationZ(FLOAT fRad);

	static CPMatrix4x4 rotationPitchYawRoll(CPVector3F vPYR);
	static CPMatrix4x4 axisRotation(CPVector3F vAxis, FLOAT fRad);

	static CPMatrix4x4 translation(FLOAT dx, FLOAT dy, FLOAT dz);
	static CPMatrix4x4 translation(CPVector3F vPos);

	//월드 행렬 생성
	static CPMatrix4x4 createWorldMatrixPitchYawRoll(CPVector3F vS, CPVector3F vPYRRot, CPVector3F vT);
	static CPMatrix4x4 createWorldMatrixAxisRotation(CPVector3F vS, CPVector3F vAxis, FLOAT fRad, CPVector3F vT);

	//오브젝트 이동-회전 행렬
	//회전 행렬(R) * 이동 행렬(T) 형태다. 카메라가 만드는 뷰행렬과는 같은 인수가 주어지면 역행렬 관계가 된다.
	static CPMatrix4x4 objLookAt(CPVector3F pos, CPVector3F at, CPVector3F up);

	//뷰 행렬 생성 : up벡터는 일반적 상황에서 [0, 1, 0]을 준다고 가정한다.
	// 이동 행렬(T) * 회전 행렬(R) 형태다.

	//그람-슈미트 프로세스
	static CPMatrix4x4 view_camGS(CPVector3F camPos, CPVector3F at, CPVector3F up);
	//외적 두번
	static CPMatrix4x4 view_camDBCR(CPVector3F camPos, CPVector3F at, CPVector3F up);

	//직교 투영 행렬 생성

	//뷰 좌표 원점 기준 직교 투영
	//	-가시부피의 width, height, 근단면과 원단면 사이의 거리 length = far - near로 직교 투영을 하는 행렬
	//	-뷰 좌표의 원점을 기준으로 설정된 가시부피이며, 근단면과의 거리가 고려되는 z성분의 이동을 제외한 
	//		이동 행렬이 없는 스케일 변환을 수행하는 직교 투영 행렬
	static CPMatrix4x4 proj_orthoViewOrigin(FLOAT w, FLOAT h, FLOAT farDist, FLOAT nearDist);

	//뷰 좌표 특정 영역 직교 투영
	//	-특정 위치에 AABB((AABB:Axis Aligned Bounding Box)형태로 존재하는 가시 부피를 직교 투영하는 행렬
	//	-가시 부피가 이동했으므로 이동 행렬과 스케일 변환 행렬이 곱해진 형태의 직교 투영 행렬
	//	-카메라가 바라보는 방향은 변하지 않았으므로 근단면/원단면과의 거리는 변하지 않는다.
	static CPMatrix4x4 proj_orthoOffCenter(FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, FLOAT farDist, FLOAT nearDist);

	//뷰 좌표 원점 기준 원근 투영
	//	-뷰 좌표 원점을 바라보는 카메라에 대한 원근 투영
	//	-가시 부피는 절두체 형태가 되며 삼각형의 닮음비를 이용해 좌표에 대한 스케일 연산을 수행한다.
	//	-가시 부피의 너비와 높이 근단면/원단면의 정보만 넘겨주었으므로 특정 영역만 투영하는 것은 불가능
	static CPMatrix4x4 proj_perspectveViewOrigin(FLOAT w, FLOAT h, FLOAT farDist, FLOAT nearDist);

	//뷰 좌표 특정 영역 직교 투영
	//	-특정 위치에 AABB((AABB:Axis Aligned Bounding Box)형태로 존재하는 가시 부피를 직교 투영하는 행렬
	//	-가시 부피가 이동했으므로 이동 행렬과 스케일 변환 행렬이 곱해진 형태의 직교 투영 행렬
	//	-카메라가 바라보는 방향은 변하지 않았으므로 근단면/원단면과의 거리는 변하지 않는다.
	static CPMatrix4x4 proj_perspectveOffCenter(FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, FLOAT farDist, FLOAT nearDist);

	//수직 시야각과 종횡비를 통한 직교투영
	// 수직 시야각(VFOV)과 종횡비로 투영 행렬을 계산한다.
	static CPMatrix4x4 proj_perspectveFOVViewOrigin(FLOAT VFOV, FLOAT aspectRatio, FLOAT farDist, FLOAT nearDist);
};
