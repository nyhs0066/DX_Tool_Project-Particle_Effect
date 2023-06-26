#pragma once
#include <SimpleMath.h>

#define CPGC_PI									3.1415926f
#define CPGC_TAU								6.2831853f

#define CPGC_RAD2DEG(x)							(x)*(180.0f/CPGC_PI)
#define CPGC_DEG2RAD(x)							(x)*(CPGC_PI/180.0f)

#define CPGC_FZERO_BOUND						1.0E-5F

#define CPGC_RAY_LENGTH_LIMIT					10000.0f

//�ε��Ҽ���
using CPM_Float2 = DirectX::XMFLOAT2;
using CPM_Float3 = DirectX::XMFLOAT3;
using CPM_Float4 = DirectX::XMFLOAT4;

//����
using CPM_Vector2 = DirectX::SimpleMath::Vector2;
using CPM_Vector3 = DirectX::SimpleMath::Vector3;
using CPM_Vector4 = DirectX::SimpleMath::Vector4;

//4X4 ���
using CPM_Matrix = DirectX::SimpleMath::Matrix;

//��� / ��� ������
using CPM_Plane = DirectX::SimpleMath::Plane;

//����� 
using CPM_Quaternion = DirectX::SimpleMath::Quaternion;

//�÷� ����ü
using CPM_Color = DirectX::SimpleMath::Color;

//Ray
//using CPM_Ray = DirectX::SimpleMath::Ray;
struct CPM_Ray
{
	//Ray�� origin�� �ش�
	CPM_Vector3 stPos;
	CPM_Vector3 direction;

	//���� ���� : �� ���� ������ ������( ext >= 0.0f ) ���׸�Ʈ�� ����Ѵ�.
	float		ext;

	bool operator==(const CPM_Ray& other) const;
	bool operator!=(const CPM_Ray& other) const;

	CPM_Ray() { ext = -1.0f; }
	CPM_Ray(CPM_Vector3 st, CPM_Vector3 dir, float extend)
	{
		stPos = st;
		direction = dir;
		ext = extend;
	}
};

//Viewport
//using CPM_Viewport = DirectX::SimpleMath::Viewport;