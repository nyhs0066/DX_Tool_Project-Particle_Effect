#include "CPRSShape.h"

CPRS_PLANE::CPRS_PLANE(const CPM_Vector3& V0,
							const CPM_Vector3& V1,
							const CPM_Vector3& V2)
{
	CPM_Vector3 vEdge1 = V1 - V0;
	CPM_Vector3 vEdge2 = V2 - V0;

	CPM_Vector3 vNormal = vEdge1.Cross(vEdge2);
	vNormal.Normalize();

	a = vNormal.x;
	b = vNormal.y;
	c = vNormal.z;

	d = vNormal.Dot(V0);
}

CPRS_PLANE::CPRS_PLANE(const CPM_Vector3& vNormal,
							const CPM_Vector3& V0)
{
	CPM_Vector3 vN = vNormal;
	vN.Normalize();

	a = vN.x;
	b = vN.y;
	c = vN.z;

	d = vN.Dot(V0);
}

CPRS_SPHERE::CPRS_SPHERE()
{
	m_fRadius = 1.0f;
}

CPRS_SPHERE::CPRS_SPHERE(const CPM_Vector3& vCPos, float fRadius)
{
	m_vCPos = vCPos;
	m_fRadius = fRadius;
}

CPRS_AABB::CPRS_AABB()
{
	m_vMin = { -0.5f, -0.5f, -0.5f };
	m_vMax = { 0.5f, 0.5f, 0.5f };
}

CPRS_AABB::CPRS_AABB(const CPM_Vector3& vMin, const CPM_Vector3& vMax)
{
	m_vMin = vMin;
	m_vMax = vMax;
	m_vCPos = { (vMin.x + vMax.x) / 2.0f, (vMin.y + vMax.y) / 2.0f, (vMin.z + vMax.z) / 2.0f };
}

CPRS_OBB::CPRS_OBB()
{
	m_vAxis[0] = {1.0f, 0.0f, 0.0f};
	m_vAxis[1] = { 0.0f, 1.0f, 0.0f };
	m_vAxis[2] = { 0.0f, 0.0f, 1.0f };
}

CPRS_OBB::CPRS_OBB(const CPM_Vector3& vCPos, const CPRS_AXIS& vAxis, const CPM_Vector3 vExt)
{
	m_vCPos = vCPos;
	m_vAxis[0] = vAxis.vRight;
	m_vAxis[1] = vAxis.vUp;
	m_vAxis[2] = vAxis.vLook;
	m_vExt = vExt;
}

CPRS_BOUNDING_BOX::CPRS_BOUNDING_BOX()
{
	m_vMin = { -0.5f, -0.5f, -0.5f };
	m_vMax = { 0.5f, 0.5f, 0.5f };

	m_vAxis[0] = { 1.0f, 0.0f, 0.0f };
	m_vAxis[1] = { 0.0f, 1.0f, 0.0f };
	m_vAxis[2] = { 0.0f, 0.0f, 1.0f };
}

CPRS_BOUNDING_BOX::CPRS_BOUNDING_BOX(const CPM_Vector3& vMin, const CPM_Vector3& vMax)
{
	m_vMin = vMin;
	m_vMax = vMax;
	m_vCPos = { (vMin.x + vMax.x) / 2.0f, (vMin.y + vMax.y) / 2.0f, (vMin.z + vMax.z) / 2.0f };
}

CPRS_BOUNDING_BOX::CPRS_BOUNDING_BOX(const CPM_Vector3& vCPos, const CPRS_AXIS& vAxis, const CPM_Vector3 vExt)
{
	m_vCPos = vCPos;
	m_vAxis[0] = vAxis.vRight;
	m_vAxis[1] = vAxis.vUp;
	m_vAxis[2] = vAxis.vLook;
	m_vExt = vExt;
}

CPRS_FRUSTUM::CPRS_FRUSTUM()
{

}

CPRS_FRUSTUM::~CPRS_FRUSTUM()
{
}

bool CPRS_FRUSTUM::create(CPM_Matrix& viewMat, CPM_Matrix& projMat)
{
	/*
	* 1. ����ȭ�� ����ü ���� �����ϱ�
	* 2. ����İ� ��������� ������� �̿��� ����ü ������ ���� ��ǥ ����ϱ�
	* 3. ������ ���� 6���� ������� �����ϱ�
	*
	*	����ü�� ����� �������Ͱ� ���η� ���ϰԲ� ���� ����
	*   ����ü�� �����ϴ� ���� ������ ������ ���� ��ġ�Ѵ�.
	*   �ٴܸ�		���ܸ�		 �¸�		���			���			�ϸ�
	*    1  2		 6  5		 5  1		2  6		5  6		0  3
	*    0  3		 7  4		 4  0		3  7		1  2		4  7
	*/

	//1. ����ȭ�� ����ü ���� �����ϱ�

	//Near Plane
	m_vPlaneVertex[0] = { -1.0f, -1.0f,  0.0f };
	m_vPlaneVertex[1] = { -1.0f,  1.0f,  0.0f };
	m_vPlaneVertex[2] = { 1.0f,  1.0f,  0.0f };
	m_vPlaneVertex[3] = { 1.0f, -1.0f,  0.0f };

	//Far Plane
	m_vPlaneVertex[4] = { -1.0f, -1.0f,  1.0f };
	m_vPlaneVertex[5] = { -1.0f,  1.0f,  1.0f };
	m_vPlaneVertex[6] = { 1.0f,  1.0f,  1.0f };
	m_vPlaneVertex[7] = { 1.0f, -1.0f,  1.0f };

	//2. ���� ��ǥ ����ϱ�
	CPM_Matrix worldMat = (viewMat * projMat); //proj^-1 * view^-1
	worldMat.Invert(worldMat);

	if (DirectX::XMMatrixIsNaN(worldMat)) { return false; }

	for (int i = 0; i < 8; i++) { m_vPlaneVertex[i] = CPM_Vector3::Transform(m_vPlaneVertex[i], worldMat); }

	//3. ��� ���� �����ϱ� : ���� ���̺귯���� Plane�� XMFLOAT�� �Ǿ� �����Ƿ�
	//x, y, z, w�ӿ� ���� {x, y, z}�� �������Ͱ� �ǰ� w�� �������� �������� �Ÿ��� �ǹǷ� ����
	//�������� �����ϱ⿡ �ٴܸ��� ������ �������� �����ϸ� ������ Ŀ�� �� ������ �ٴܸ� ���� ���ܸ����� ���ϴ� ��������
	//���� ���͸� �����ϴ� ����� �������� ���� �� �ֵ��� �ϴ� ���� ����.

	//near : 0 1 2 �� �ٱ��� ���� ����
	m_plane[0] = CPM_Plane::Plane(m_vPlaneVertex[0], m_vPlaneVertex[2], m_vPlaneVertex[1]);
	//right : 2 6 7 �̸� �ٱ��� ���� ���� => ���ܸ��� �� ������ �ٴܸ��� �� ������ ���ϴ� ���ͷ� ��� ����
	m_plane[1] = CPM_Plane::Plane(m_vPlaneVertex[2], m_vPlaneVertex[7], m_vPlaneVertex[6]);
	//far : 7 6 5 �̸� �ٱ��� ���� ����
	m_plane[2] = CPM_Plane::Plane(m_vPlaneVertex[7], m_vPlaneVertex[5], m_vPlaneVertex[6]);
	//left : 0 4 5 �̸� �ٱ��� ���� ���� => ���ܸ��� �� ������ �ٴܸ��� �� ������ ���ϴ� ���ͷ� ��� ����
	m_plane[3] = CPM_Plane::Plane(m_vPlaneVertex[0], m_vPlaneVertex[5], m_vPlaneVertex[4]);
	//top : 1 5 6 �̸� �ٱ��� ���� ���� => ���ܸ��� �� ������ �ٴܸ��� �� ������ ���ϴ� ���ͷ� ��� ����
	m_plane[4] = CPM_Plane::Plane(m_vPlaneVertex[1], m_vPlaneVertex[6], m_vPlaneVertex[5]);
	//bot : 0 7 4 �̸� �ٱ��� ���� ���� => ���ܸ��� �� ������ �ٴܸ��� �� ������ ���ϴ� ���ͷ� ��� ����
	m_plane[5] = CPM_Plane::Plane(m_vPlaneVertex[0], m_vPlaneVertex[4], m_vPlaneVertex[7]);

	return true;
}

UINT CPRS_FRUSTUM::classifyPoint(const CPM_Vector3& vPt)
{
	UINT ret = CPRS_FCULL_FRONT;

	for (auto& it : m_plane)
	{
		//�Ǻ����� ���� ������ ������ ����ü ���� �޸鿡 �ִ�.
		FLOAT D = it.DotCoordinate(vPt);

		if (fabs(D) < CPGC_FZERO_BOUND) { ret = CPRS_FCULL_ONPLANE; break; }
		if (D < 0.0f) { ret = CPRS_FCULL_BACK; break; }
	}

	return ret;
}

UINT CPRS_FRUSTUM::classifySphere(const CPRS_SPHERE& sphere)
{
	UINT ret = CPRS_FCULL_FRONT;

	for (auto& it : m_plane)
	{
		//���� ���ʿ� ������ �׻� ����̹Ƿ� ���� ���������� ũ��.
		//������ ���� ���� ��, ����ü �ٱ��ʿ� �ִ� ���ε�
		//���������� �� ������ ����ü �ܺο� �ִ� ���� �ǹ��Ѵ�.
		FLOAT centerDist = it.DotCoordinate(sphere.m_vCPos);

		if (centerDist >= 0)
		{
			if (centerDist <= sphere.m_fRadius) { ret = CPRS_FCULL_SPAN; }
		}
		else
		{
			ret = CPRS_FCULL_BACK;

			if (centerDist >= -sphere.m_fRadius) { ret = CPRS_FCULL_SPAN; }

			break;
		}
	}

	return ret;
}

UINT CPRS_FRUSTUM::classifyAABB(const CPRS_AABB& AABB)
{
	UINT ret = CPRS_FCULL_FRONT;

	CPM_Vector3 vTest;

	FLOAT w = AABB.m_vMax.x - AABB.m_vMin.x;
	FLOAT h = AABB.m_vMax.y - AABB.m_vMin.y;
	FLOAT l = AABB.m_vMax.z - AABB.m_vMin.z;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				vTest = AABB.m_vMin + CPM_Vector3(w * i, h * j, l * k);

				ret = classifyPoint(vTest);

				if (ret != CPRS_FCULL_FRONT) return ret;
			}
		}
	}

	return ret;
}

UINT CPRS_FRUSTUM::classifyOBB(const CPRS_OBB& OBB)
{
	UINT ret = CPRS_FCULL_FRONT;

	for (auto& it : m_plane)
	{
		//OBB �� �������� ��� �´�� ������ ������ ���Ϳ� ����� �������͸� ������ ����� �������� 
		//�������Ͱ� ����ȭ �Ǿ� �ִٴ� �����Ͽ� �߾������κ��� ���� ���� ����� �������� ���� ���͸�
		//�������ͷ� �翵�� �Ÿ��� ����.
		//�� �Ÿ��� �߾������� �������� �Ÿ����� ũ�ٸ� ����� ����� �ȴ�.
		FLOAT centerDist = it.DotCoordinate(OBB.m_vCPos);

		FLOAT d1 = fabs(it.DotNormal(OBB.m_vAxis[0] * OBB.m_vExt.x));
		FLOAT d2 = fabs(it.DotNormal(OBB.m_vAxis[1] * OBB.m_vExt.y));
		FLOAT d3 = fabs(it.DotNormal(OBB.m_vAxis[2] * OBB.m_vExt.z));

		FLOAT dist = d1 + d2 + d3;

		if (centerDist <= dist) { ret = CPRS_FCULL_SPAN; }
		if (centerDist < -dist) { ret = CPRS_FCULL_BACK; break; }
	}

	return ret;
}

UINT CPRS_FRUSTUM::classifyBox(const CPRS_BOUNDING_BOX& box)
{
	UINT ret = CPRS_FCULL_FRONT;

	for (auto& it : m_plane)
	{
		//OBB �� �������� ��� �´�� ������ ������ ���Ϳ� ����� �������͸� ������ ����� �������� 
		//�������Ͱ� ����ȭ �Ǿ� �ִٴ� �����Ͽ� �߾������κ��� ���� ���� ����� �������� ���� ���͸�
		//�������ͷ� �翵�� �Ÿ��� ����.
		//�� �Ÿ��� �߾������� �������� �Ÿ����� ũ�ٸ� ����� ����� �ȴ�.
		FLOAT centerDist = it.DotCoordinate(box.m_vCPos);

		FLOAT d1 = fabs(it.DotNormal(box.m_vAxis[0] * box.m_vExt.x));
		FLOAT d2 = fabs(it.DotNormal(box.m_vAxis[1] * box.m_vExt.y));
		FLOAT d3 = fabs(it.DotNormal(box.m_vAxis[2] * box.m_vExt.z));

		FLOAT dist = d1 + d2 + d3;

		if (centerDist <= dist) { ret = CPRS_FCULL_SPAN; }
		if (centerDist < -dist) { ret = CPRS_FCULL_BACK; break; }
	}

	return ret;
}
