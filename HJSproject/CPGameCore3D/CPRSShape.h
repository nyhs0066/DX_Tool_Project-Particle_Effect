#pragma once
#include "CPRSBaseStruct.h"

enum CPRS_FCULL_RESULT
{
	CPRS_FCULL_BACK = 0,
	CPRS_FCULL_FRONT,
	CPRS_FCULL_ONPLANE,
	CPRS_FCULL_SPAN,
	CPRS_FCULL_NUMBER_OF_TYPE
};

struct CPRS_Shape
{
};

struct CPRS_RECT
{
	union
	{
		struct
		{
			float l, b, r, t;
		};

		struct
		{
			CPM_Vector2 vMin;
			CPM_Vector2 vMax;
		};
	};
	CPRS_RECT()
	{
		vMax = vMin = { 0.0f, 0.0f };
	}

	CPRS_RECT(float l, float b, float r, float t)
	{
		this->l = l;
		this->b = b;
		this->r = r;
		this->t = t;
	}

	CPRS_RECT(CPM_Vector2 vMin, CPM_Vector2 vMax)
	{
		this->vMin = vMin;
		this->vMax = vMax;
	}
};

//�� ����ü : DX ���� ���̺귯���� �ִ� ���� ������ ����ϰ� ������ �� ����
struct CPRS_PLANE : public CPRS_Shape
{
	FLOAT a, b, c, d;

	CPRS_PLANE(const CPM_Vector3& V0, 
				const CPM_Vector3& V1, 
				const CPM_Vector3& V2);

	CPRS_PLANE(const CPM_Vector3& vNormal, 
				const CPM_Vector3& V0);
};

//�ٿ�� ���� ����ü
struct CPRS_SPHERE : public CPRS_Shape
{
	CPM_Vector3	m_vCPos;
	float							m_fRadius;

	CPRS_SPHERE();
	CPRS_SPHERE(const CPM_Vector3& vCPos, float fRadius);
};

struct CPRS_AABB : public CPRS_Shape
{
	CPM_Vector3	m_vCPos;
	CPM_Vector3	m_vMin;
	CPM_Vector3	m_vMax;

	CPRS_AABB();
	CPRS_AABB(const	CPM_Vector3& vMin, 
				const CPM_Vector3& vMax);
};

struct CPRS_OBB : public CPRS_Shape
{
	CPM_Vector3	m_vCPos;
	CPM_Vector3	m_vAxis[3];
	CPM_Vector3	m_vExt;

	CPRS_OBB();
	CPRS_OBB(const CPM_Vector3& vCPos,
				const CPRS_AXIS& vAxis,
				const CPM_Vector3 vExt);
};

struct CPRS_BOUNDING_BOX : public CPRS_Shape
{
	CPM_Vector3	m_vCPos;

	CPM_Vector3	m_vMin;
	CPM_Vector3	m_vMax;

	CPM_Vector3	m_vAxis[3];
	CPM_Vector3	m_vExt;

	CPRS_BOUNDING_BOX();
	CPRS_BOUNDING_BOX(const	CPM_Vector3& vMin,
						const CPM_Vector3& vMax);
	CPRS_BOUNDING_BOX(const CPM_Vector3& vCPos,
						const CPRS_AXIS& vAxis,
						const CPM_Vector3 vExt);
};

//����ü
struct CPRS_FRUSTUM : public CPRS_Shape
{
	CPRS_FRUSTUM();
	~CPRS_FRUSTUM();

	CPM_Vector3 m_vPlaneVertex[8];
	CPM_Plane m_plane[6];

	bool create(CPM_Matrix& viewMat, CPM_Matrix& projMat);

	//�з� ��� : ������ �߰� �ؾ���
	//DotCoodinate : ���� ����� �������� ������ ������ ���� ������ �Ÿ��� ���´�.
	//DotNormal : ����� �������� �������Ϳ� �μ��� �Ѱ��� ���͸� ������ 
	//				�μ����͸� �������Ϳ� �翵�� ���̰� ���´�.
	UINT classifyPoint(const CPM_Vector3& vPt);
	UINT classifySphere(const CPRS_SPHERE& sphere);
	UINT classifyAABB(const CPRS_AABB& AABB);
	UINT classifyOBB(const CPRS_OBB& OBB);
	UINT classifyBox(const CPRS_BOUNDING_BOX& box);
};
