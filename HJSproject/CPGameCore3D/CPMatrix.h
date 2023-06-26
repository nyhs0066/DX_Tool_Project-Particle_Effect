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

	//��ġ������ �����ϹǷ� ���� Ŭ���������� ��İ��� ���� �����ڸ� �������Ѵ�.
	//����� ��ġ ���� �ҰŸ� ����� ��ġ���� ����ؾ��ϴ� �Ϳ� ����.
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

	//��ġ������ �����ϹǷ� ���� Ŭ���������� ��İ��� ���� �����ڸ� �������Ѵ�.
	//����� ��ġ ���� �ҰŸ� ����� ��ġ���� ����ؾ��ϴ� �Ϳ� ����.
	static CPMatrix4x4 scale(FLOAT x, FLOAT y, FLOAT z);
	static CPMatrix4x4 rotationX(FLOAT fRad);
	static CPMatrix4x4 rotationY(FLOAT fRad);
	static CPMatrix4x4 rotationZ(FLOAT fRad);

	static CPMatrix4x4 rotationPitchYawRoll(CPVector3F vPYR);
	static CPMatrix4x4 axisRotation(CPVector3F vAxis, FLOAT fRad);

	static CPMatrix4x4 translation(FLOAT dx, FLOAT dy, FLOAT dz);
	static CPMatrix4x4 translation(CPVector3F vPos);

	//���� ��� ����
	static CPMatrix4x4 createWorldMatrixPitchYawRoll(CPVector3F vS, CPVector3F vPYRRot, CPVector3F vT);
	static CPMatrix4x4 createWorldMatrixAxisRotation(CPVector3F vS, CPVector3F vAxis, FLOAT fRad, CPVector3F vT);

	//������Ʈ �̵�-ȸ�� ���
	//ȸ�� ���(R) * �̵� ���(T) ���´�. ī�޶� ����� ����İ��� ���� �μ��� �־����� ����� ���谡 �ȴ�.
	static CPMatrix4x4 objLookAt(CPVector3F pos, CPVector3F at, CPVector3F up);

	//�� ��� ���� : up���ʹ� �Ϲ��� ��Ȳ���� [0, 1, 0]�� �شٰ� �����Ѵ�.
	// �̵� ���(T) * ȸ�� ���(R) ���´�.

	//�׶�-����Ʈ ���μ���
	static CPMatrix4x4 view_camGS(CPVector3F camPos, CPVector3F at, CPVector3F up);
	//���� �ι�
	static CPMatrix4x4 view_camDBCR(CPVector3F camPos, CPVector3F at, CPVector3F up);

	//���� ���� ��� ����

	//�� ��ǥ ���� ���� ���� ����
	//	-���ú����� width, height, �ٴܸ�� ���ܸ� ������ �Ÿ� length = far - near�� ���� ������ �ϴ� ���
	//	-�� ��ǥ�� ������ �������� ������ ���ú����̸�, �ٴܸ���� �Ÿ��� ����Ǵ� z������ �̵��� ������ 
	//		�̵� ����� ���� ������ ��ȯ�� �����ϴ� ���� ���� ���
	static CPMatrix4x4 proj_orthoViewOrigin(FLOAT w, FLOAT h, FLOAT farDist, FLOAT nearDist);

	//�� ��ǥ Ư�� ���� ���� ����
	//	-Ư�� ��ġ�� AABB((AABB:Axis Aligned Bounding Box)���·� �����ϴ� ���� ���Ǹ� ���� �����ϴ� ���
	//	-���� ���ǰ� �̵������Ƿ� �̵� ��İ� ������ ��ȯ ����� ������ ������ ���� ���� ���
	//	-ī�޶� �ٶ󺸴� ������ ������ �ʾ����Ƿ� �ٴܸ�/���ܸ���� �Ÿ��� ������ �ʴ´�.
	static CPMatrix4x4 proj_orthoOffCenter(FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, FLOAT farDist, FLOAT nearDist);

	//�� ��ǥ ���� ���� ���� ����
	//	-�� ��ǥ ������ �ٶ󺸴� ī�޶� ���� ���� ����
	//	-���� ���Ǵ� ����ü ���°� �Ǹ� �ﰢ���� ������ �̿��� ��ǥ�� ���� ������ ������ �����Ѵ�.
	//	-���� ������ �ʺ�� ���� �ٴܸ�/���ܸ��� ������ �Ѱ��־����Ƿ� Ư�� ������ �����ϴ� ���� �Ұ���
	static CPMatrix4x4 proj_perspectveViewOrigin(FLOAT w, FLOAT h, FLOAT farDist, FLOAT nearDist);

	//�� ��ǥ Ư�� ���� ���� ����
	//	-Ư�� ��ġ�� AABB((AABB:Axis Aligned Bounding Box)���·� �����ϴ� ���� ���Ǹ� ���� �����ϴ� ���
	//	-���� ���ǰ� �̵������Ƿ� �̵� ��İ� ������ ��ȯ ����� ������ ������ ���� ���� ���
	//	-ī�޶� �ٶ󺸴� ������ ������ �ʾ����Ƿ� �ٴܸ�/���ܸ���� �Ÿ��� ������ �ʴ´�.
	static CPMatrix4x4 proj_perspectveOffCenter(FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, FLOAT farDist, FLOAT nearDist);

	//���� �þ߰��� ��Ⱦ�� ���� ��������
	// ���� �þ߰�(VFOV)�� ��Ⱦ��� ���� ����� ����Ѵ�.
	static CPMatrix4x4 proj_perspectveFOVViewOrigin(FLOAT VFOV, FLOAT aspectRatio, FLOAT farDist, FLOAT nearDist);
};
