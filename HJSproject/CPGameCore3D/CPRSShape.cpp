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
	* 1. 정규화된 절두체 정점 구성하기
	* 2. 뷰행렬과 투영행렬의 역행렬을 이용해 절두체 정점의 월드 좌표 계산하기
	* 3. 정점을 토대로 6개의 평면정보 구성하기
	*
	*	절두체의 평면은 법선벡터가 내부로 향하게끔 만들 것임
	*   절두체를 구성하는 정점 순서는 다음과 같이 배치한다.
	*   근단면		원단면		 좌면		우면			상면			하면
	*    1  2		 6  5		 5  1		2  6		5  6		0  3
	*    0  3		 7  4		 4  0		3  7		1  2		4  7
	*/

	//1. 정규화된 절두체 정점 구성하기

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

	//2. 월드 좌표 계산하기
	CPM_Matrix worldMat = (viewMat * projMat); //proj^-1 * view^-1
	worldMat.Invert(worldMat);

	if (DirectX::XMMatrixIsNaN(worldMat)) { return false; }

	for (int i = 0; i < 8; i++) { m_vPlaneVertex[i] = CPM_Vector3::Transform(m_vPlaneVertex[i], worldMat); }

	//3. 평면 정보 구성하기 : 수학 라이브러리의 Plane은 XMFLOAT로 되어 있으므로
	//x, y, z, w임에 유의 {x, y, z}는 법선벡터가 되고 w는 원점에서 평면까지의 거리가 되므로 참고
	//외적으로 구성하기에 근단면의 정점을 기준으로 외적하면 오차가 커질 수 있으니 근단면 에서 원단면으로 향하는 정점으로
	//법선 벡터를 구성하는 평면의 방정식을 만들 수 있도록 하는 것이 좋다.

	//near : 0 1 2 면 바깥쪽 법선 벡터
	m_plane[0] = CPM_Plane::Plane(m_vPlaneVertex[0], m_vPlaneVertex[2], m_vPlaneVertex[1]);
	//right : 2 6 7 이면 바깥쪽 법선 벡터 => 원단면의 두 점에서 근단면의 한 점으로 향하는 벡터로 평면 구성
	m_plane[1] = CPM_Plane::Plane(m_vPlaneVertex[2], m_vPlaneVertex[7], m_vPlaneVertex[6]);
	//far : 7 6 5 이면 바깥쪽 법선 벡터
	m_plane[2] = CPM_Plane::Plane(m_vPlaneVertex[7], m_vPlaneVertex[5], m_vPlaneVertex[6]);
	//left : 0 4 5 이면 바깥쪽 법선 벡터 => 원단면의 두 점에서 근단면의 한 점으로 향하는 벡터로 평면 구성
	m_plane[3] = CPM_Plane::Plane(m_vPlaneVertex[0], m_vPlaneVertex[5], m_vPlaneVertex[4]);
	//top : 1 5 6 이면 바깥쪽 법선 벡터 => 원단면의 두 점에서 근단면의 한 점으로 향하는 벡터로 평면 구성
	m_plane[4] = CPM_Plane::Plane(m_vPlaneVertex[1], m_vPlaneVertex[6], m_vPlaneVertex[5]);
	//bot : 0 7 4 이면 바깥쪽 법선 벡터 => 원단면의 두 점에서 근단면의 한 점으로 향하는 벡터로 평면 구성
	m_plane[5] = CPM_Plane::Plane(m_vPlaneVertex[0], m_vPlaneVertex[4], m_vPlaneVertex[7]);

	return true;
}

UINT CPRS_FRUSTUM::classifyPoint(const CPM_Vector3& vPt)
{
	UINT ret = CPRS_FCULL_FRONT;

	for (auto& it : m_plane)
	{
		//판별식의 값이 음수면 정점은 절두체 면의 뒷면에 있다.
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
		//면의 앞쪽에 있으면 항상 양수이므로 음수 반지름보다 크다.
		//음수면 면의 뒤쪽 즉, 절두체 바깥쪽에 있는 것인데
		//반지름보다 더 작으면 절두체 외부에 있는 것을 의미한다.
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
		//OBB 각 기저축을 면과 맞닿는 데까지 연장한 벡터와 평면의 법선벡터를 내적한 결과의 절댓값은는 
		//법선벡터가 정규화 되어 있다는 전제하에 중앙점으로부터 평면과 가장 가까운 점까지의 방향 벡터를
		//법선벡터로 사영한 거리와 같다.
		//그 거리가 중앙점에서 평면까지의 거리보다 크다면 평면을 벗어난게 된다.
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
		//OBB 각 기저축을 면과 맞닿는 데까지 연장한 벡터와 평면의 법선벡터를 내적한 결과의 절댓값은는 
		//법선벡터가 정규화 되어 있다는 전제하에 중앙점으로부터 평면과 가장 가까운 점까지의 방향 벡터를
		//법선벡터로 사영한 거리와 같다.
		//그 거리가 중앙점에서 평면까지의 거리보다 크다면 평면을 벗어난게 된다.
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
