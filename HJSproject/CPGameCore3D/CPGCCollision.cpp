#include "CPGCCollision.h"

UINT CPGCCollision::pointToPoint(const CPM_Vector3& p1, const CPM_Vector3& p2)
{
	//거리가 0으로 판정되는 범위안에 들어 왔는지 판단한다.
	if (fabs(p1.x - p2.x) < CPGC_FZERO_BOUND &&
		fabs(p1.y - p2.y) < CPGC_FZERO_BOUND) 
	{ 
		return CPGC_CO_HIT; 
	}

	return CPGC_CO_NO_HIT;
}

UINT CPGCCollision::rayToPoint(const CPM_Ray& ray, const CPM_Vector3& pt)
{
	if (ray.stPos == pt) { return CPGC_CO_HIT; }

	//점과의 거리를 이용해 판정한다.
	CPM_Vector3 targetVector = pt - ray.stPos;

	float dotRay = targetVector.Dot(ray.direction);
	
	if (dotRay >= 0.0f)
	{
		CPM_Vector3 dottedTarget = ray.stPos + ray.direction * dotRay;

		float dist = (targetVector - dottedTarget).LengthSquared();

		if (dist < CPGC_FZERO_BOUND) { return CPGC_CO_HIT; }
	}

	return CPGC_CO_NO_HIT;
}

UINT CPGCCollision::rayToRay(const CPM_Ray& ray1, const CPM_Ray& ray2)
{	
	//두 직선의 위치관계는 다음이 있다. 위치관계에 따라 판정한다.
	//교점이 있는 경우 : 일치, 1점 교차
	//교점이 없는 경우 : 평행, 꼬인위치

	//방향이 같은 경우
	if (ray1.direction == ray2.direction)
	{
		//시점과 종점중 어느 하나라도 포함하지 않으면 평행
		if (rayToPoint(ray1, ray2.stPos) || rayToPoint(ray1, ray2.stPos + ray2.direction * ray2.ext))
		{
			return CPGC_CO_HIT;
		}
		else { return CPGC_CO_NO_HIT; }
	}
	else
	{
		//방향이 다른 경우

		//꼬인 위치에 있다면 거리가 존재한다.
		//두 직선이 이루는 평면의 법선벡터에 두 직선에 포함된 각 점을 이은 벡터를 사영해본다.

		CPM_Vector3 tV = ray1.stPos - ray2.stPos;
		CPM_Vector3 normal = ray1.direction.Cross(ray2.direction);
		normal.Normalize();

		float dist = fabs(tV.Dot(normal));

		if (dist > CPGC_FZERO_BOUND) { return CPGC_CO_NO_HIT; }
		else { return CPGC_CO_HIT; }
	}

	return 0;
}

UINT CPGCCollision::rayToAABB(const CPM_Ray& ray, const CPRS_BOUNDING_BOX& box)
{
	return 0;
}

UINT CPGCCollision::rayToAABB(const CPM_Ray& ray, const CPRS_AABB& aabb)
{
	return 0;
}

UINT CPGCCollision::rayToOBB(const CPM_Ray& ray, const CPRS_BOUNDING_BOX& box)
{
	//분리축 이론을 이용한다.
	//OBB의 중점과 레이의 시점을 이은 벡터(R = O - C)를 이용해 
	//레이의 방향벡터와 OBB의 각 모서리를 외적해 나온 벡터 축의 검증을 수행하고
	//OBB의 법선 벡터 축의 검증은 반직선의 시점과 방향성 검사로 대신한다.

	//1. OBB의 법선 벡터 축 검사 : 반직선의 시점과 방향성 검사를 함께 수행한다.
	//시점 검사는 OBB의 연장축의 길이와 R을 OBB의 축과 내적해 나온 길이를 비교한다.
	//(OBB의 연장축의 길이) < abs(DOT(R, OBB의 축)) => 외부
	//방향 검사는 레이의 방향벡터와 R과의 내적을 통해 나온 부호로 검증한다.
	// R의 방향이 센터에서 레이의 원점이므로 두 내적의 결과가 양수면 박스를 벗어난 방향으로
	// 레이가 진행한다는 의미가된다.
	//DOT(ray.direction, OBB의 축) * DOT(R, OBB의 축) >= 0.0f => 외부

	//최적화 이전
	/*CPM_Vector3 centerToOrigin = ray.stPos - box.m_vCPos;

	float OT0 = fabs(centerToOrigin.Dot(box.m_vAxis[0]));
	float DT0 = ray.direction.Dot(box.m_vAxis[0]) * centerToOrigin.Dot(box.m_vAxis[0]);

	if (OT0 > box.m_vExt.x && DT0 >= 0.0f) { return false; }

	float OT1 = fabs(centerToOrigin.Dot(box.m_vAxis[1]));
	float DT1 = ray.direction.Dot(box.m_vAxis[1]) * centerToOrigin.Dot(box.m_vAxis[1]);

	if (OT1 > box.m_vExt.y && DT1 >= 0.0f) { return false; }

	float OT2 = fabs(centerToOrigin.Dot(box.m_vAxis[2]));
	float DT2 = ray.direction.Dot(box.m_vAxis[2]) * centerToOrigin.Dot(box.m_vAxis[2]);

	if (OT2 > box.m_vExt.z && DT2 >= 0.0f) { return false; }*/

	//최적화 이후 : OD => Origin Dot / DD => Direction Dot
	float OD[3], ODAbs[3], DD[3], DDAbs[3];
	CPM_Vector3 centerToOrigin = ray.stPos - box.m_vCPos;

	DD[0] = ray.direction.Dot(box.m_vAxis[0]);
	OD[0] = centerToOrigin.Dot(box.m_vAxis[0]);
	DDAbs[0] = fabs(DD[0]);
	ODAbs[0] = fabs(OD[0]);
	if (ODAbs[0] > box.m_vExt.x && OD[0] * DD[0] >= 0.0f) { return false; }

	DD[1] = ray.direction.Dot(box.m_vAxis[1]);
	OD[1] = centerToOrigin.Dot(box.m_vAxis[1]);
	DDAbs[1] = fabs(DD[1]);
	ODAbs[1] = fabs(OD[1]);
	if (ODAbs[1] > box.m_vExt.y && OD[1] * DD[1] >= 0.0f) { return false; }

	DD[2] = ray.direction.Dot(box.m_vAxis[2]);
	OD[2] = centerToOrigin.Dot(box.m_vAxis[2]);
	DDAbs[2] = fabs(DD[2]);
	ODAbs[2] = fabs(OD[2]);
	if (ODAbs[2] > box.m_vExt.x && OD[2] * DD[2] >= 0.0f) { return false; }

	//2. 레이의 방향벡터와 모서리의 외적벡터 축 집합의 검사

	//최적화 이전
	/*CPM_Vector3 n0 = ray.direction.Cross(box.m_vAxis[0]);
	CPM_Vector3 n1 = ray.direction.Cross(box.m_vAxis[1]);
	CPM_Vector3 n2 = ray.direction.Cross(box.m_vAxis[2]);

	float LTBOX0 = n0.Dot(box.m_vAxis[0] * box.m_vExt.x) +
		n0.Dot(box.m_vAxis[1] * box.m_vExt.y) +
		n0.Dot(box.m_vAxis[2] * box.m_vExt.z);

	float LTCTO0 = n0.Dot(centerToOrigin);

	if (LTBOX0 < LTCTO0) { return false; }

	float LTBOX1 = n1.Dot(box.m_vAxis[0] * box.m_vExt.x) +
		n1.Dot(box.m_vAxis[1] * box.m_vExt.y) +
		n1.Dot(box.m_vAxis[2] * box.m_vExt.z);

	float LTCTO1 = n1.Dot(centerToOrigin);

	if (LTBOX1 < LTCTO1) { return false; }

	float LTBOX2 = n2.Dot(box.m_vAxis[0] * box.m_vExt.x) +
		n2.Dot(box.m_vAxis[1] * box.m_vExt.y) +
		n2.Dot(box.m_vAxis[2] * box.m_vExt.z);

	float LTCTO2 = n2.Dot(centerToOrigin);

	if (LTBOX2 < LTCTO2) { return false; }*/

	//최적화 이후 : 스칼라 삼중적의 특성을 이용해 식을 변경해 
	//외적을 각 축마다 수행하는 것이 아닌 레이의 방향벡터와 centerToOrigin과의 한번의 외적을
	//반복하여 사용한다.
	//OBB의 투영부도 OBB의 각축의 외적이 다른 OBB의 축이 된다는 점과
	//스칼라 삼중적의 특성, 절댓값이 적용된 외적의 특성을 이용해 구조를 변경한다.
	//DEC => Direction Edge Cross

	float DECAbs[3], OBBProjLen;
	CPM_Vector3 vDirXCTO;
	vDirXCTO = ray.direction.Cross(centerToOrigin);
	DECAbs[0] = fabs(vDirXCTO.Dot(box.m_vAxis[0]));
	OBBProjLen = box.m_vExt.y * DDAbs[2] + box.m_vExt.z * DDAbs[1];
	if (DECAbs[0] > OBBProjLen) { return false; }

	DECAbs[1] = fabs(vDirXCTO.Dot(box.m_vAxis[1]));
	OBBProjLen = box.m_vExt.x * DDAbs[2] + box.m_vExt.z * DDAbs[0];
	if (DECAbs[1] > OBBProjLen) { return false; }

	DECAbs[2] = fabs(vDirXCTO.Dot(box.m_vAxis[2]));
	OBBProjLen = box.m_vExt.x * DDAbs[1] + box.m_vExt.y * DDAbs[0];
	if (DECAbs[2] > OBBProjLen) { return false; }

	return true;
}

UINT CPGCCollision::rayToOBB(const CPM_Ray& ray, const CPRS_OBB& obb)
{
	float OD[3], ODAbs[3], DD[3], DDAbs[3];
	CPM_Vector3 centerToOrigin = ray.stPos - obb.m_vCPos;

	DD[0] = ray.direction.Dot(obb.m_vAxis[0]);
	OD[0] = centerToOrigin.Dot(obb.m_vAxis[0]);
	DDAbs[0] = fabs(DD[0]);
	ODAbs[0] = fabs(OD[0]);
	if (ODAbs[0] > obb.m_vExt.x && OD[0] * DD[0] >= 0.0f) { return false; }

	DD[1] = ray.direction.Dot(obb.m_vAxis[1]);
	OD[1] = centerToOrigin.Dot(obb.m_vAxis[1]);
	DDAbs[1] = fabs(DD[1]);
	ODAbs[1] = fabs(OD[1]);
	if (ODAbs[1] > obb.m_vExt.y && OD[1] * DD[1] >= 0.0f) { return false; }

	DD[2] = ray.direction.Dot(obb.m_vAxis[2]);
	OD[2] = centerToOrigin.Dot(obb.m_vAxis[2]);
	DDAbs[2] = fabs(DD[2]);
	ODAbs[2] = fabs(OD[2]);
	if (ODAbs[2] > obb.m_vExt.x && OD[2] * DD[2] >= 0.0f) { return false; }

	float DECAbs[3], OBBProjLen;
	CPM_Vector3 vDirXCTO;
	vDirXCTO = ray.direction.Cross(centerToOrigin);
	DECAbs[0] = fabs(vDirXCTO.Dot(obb.m_vAxis[0]));
	OBBProjLen = obb.m_vExt.y * DDAbs[2] + obb.m_vExt.z * DDAbs[1];
	if (DECAbs[0] > OBBProjLen) { return false; }

	DECAbs[1] = fabs(vDirXCTO.Dot(obb.m_vAxis[1]));
	OBBProjLen = obb.m_vExt.x * DDAbs[2] + obb.m_vExt.z * DDAbs[0];
	if (DECAbs[1] > OBBProjLen) { return false; }

	DECAbs[2] = fabs(vDirXCTO.Dot(obb.m_vAxis[2]));
	OBBProjLen = obb.m_vExt.x * DDAbs[1] + obb.m_vExt.y * DDAbs[0];
	if (DECAbs[2] > OBBProjLen) { return false; }

	return true;
}

UINT CPGCCollision::OBBToOBB(const CPRS_BOUNDING_BOX& box1, const CPRS_BOUNDING_BOX& box2)
{
	//분리축 이론을 이용한다.
	//15개의 분리축을 검사한다.
	//	-각 OBB구성면의 법선벡터 6개
	//	-한 OBB의 구성면 법선벡터 하나와 다른 OBB의 구성면 법선벡터를 외적해 나온 새로운 분리축 벡터 9개
	//각 OBB의 중점을 이은 벡터의 분리축 투영 길이와 각 OBB의 연장축의 분리축 투영길이의 합을 비교한다.
	// 
	//(각 OBB의 중점을 이은 벡터의 분리축 투영 길이) > (각 OBB의 연장축의 분리축 투영길이의 합) : 충돌 안함
	//이외의 결과는 충돌 함으로 간주한다.

	//0. 15개의 분리축

	CPM_Vector3 sepAxis[15];
	sepAxis[0] = box1.m_vAxis[0];
	sepAxis[1] = box1.m_vAxis[1];
	sepAxis[2] = box1.m_vAxis[2];
	sepAxis[3] = box2.m_vAxis[0];
	sepAxis[4] = box2.m_vAxis[1];
	sepAxis[5] = box2.m_vAxis[2];

	sepAxis[6] = sepAxis[0].Cross(sepAxis[3]);
	sepAxis[6].Normalize();

	sepAxis[7] = sepAxis[0].Cross(sepAxis[4]);
	sepAxis[7].Normalize();

	sepAxis[8] = sepAxis[0].Cross(sepAxis[5]);
	sepAxis[8].Normalize();

	sepAxis[9] = sepAxis[1].Cross(sepAxis[3]);
	sepAxis[9].Normalize();

	sepAxis[10] = sepAxis[1].Cross(sepAxis[4]);
	sepAxis[10].Normalize();

	sepAxis[11] = sepAxis[1].Cross(sepAxis[5]);
	sepAxis[11].Normalize();

	sepAxis[12] = sepAxis[2].Cross(sepAxis[3]);
	sepAxis[12].Normalize();

	sepAxis[13] = sepAxis[2].Cross(sepAxis[4]);
	sepAxis[13].Normalize();

	sepAxis[14] = sepAxis[2].Cross(sepAxis[5]);
	sepAxis[14].Normalize();

	//1. 검사 벡터 생성

	CPM_Vector3 C2C = box1.m_vCPos - box2.m_vCPos;
	CPM_Vector3 B1Ext = box1.m_vAxis[0] * box1.m_vExt.x + box1.m_vAxis[1] * box1.m_vExt.y + box1.m_vAxis[2] * box1.m_vExt.z;
	CPM_Vector3 B2Ext = box2.m_vAxis[0] * box2.m_vExt.x + box2.m_vAxis[1] * box2.m_vExt.y + box2.m_vAxis[2] * box2.m_vExt.z;

	//최적화 이전
	for (int i = 0; i < 15; i++)
	{
		float OT = fabs(C2C.Dot(sepAxis[i]));
		float DT = fabs(B1Ext.Dot(sepAxis[i])) + fabs(B2Ext.Dot(sepAxis[i]));

		if (OT > DT) { return false; }
	}

	return 0;
}

CPGCBoundingVolume::CPGCBoundingVolume()
{
}

CPGCBoundingVolume::CPGCBoundingVolume(CPM_Vector3 exts, CPM_Vector3 center)
{
	InitOBB.Center = center;
	InitOBB.Extents = exts;
	InitOBB.Orientation = CPM_Quaternion(CPM_Vector3(0.0f, 0.0f, 0.0f), 1.0f);

	DirectX::BoundingSphere::CreateFromBoundingBox(InitSphere, InitOBB);
}

CPGCBoundingVolume::CPGCBoundingVolume(CPM_Vector3 center, CPM_Vector3 exts, CPM_Quaternion rotation)
{
	InitOBB.Center = center;
	InitOBB.Extents = exts;
	InitOBB.Orientation = rotation;

	DirectX::BoundingSphere::CreateFromBoundingBox(InitSphere, InitOBB);
}

CPGCBoundingVolume::CPGCBoundingVolume(float radius, CPM_Vector3 center)
{
	InitSphere.Center = center;
	InitSphere.Radius = radius;

	InitOBB.Center = center;
	InitOBB.Extents = CPM_Vector3(radius);
	InitOBB.Orientation = CPM_Quaternion(CPM_Vector3(0.0f, 0.0f, 0.0f), 1.0f);
}

CPGCBoundingVolume::CPGCBoundingVolume(DirectX::BoundingOrientedBox& OBB)
{
	InitOBB = OBB;
	DirectX::BoundingSphere::CreateFromBoundingBox(InitSphere, OBB);
}

void CPGCBoundingVolume::adjustVolume(CPM_Vector3 exts)
{
	InitOBB.Extents = exts;

	float Radius = max(max(exts.x, exts.y), exts.z);
	InitSphere.Radius = Radius;
}

void CPGCBoundingVolume::adjustVolume(float fMaxDist)
{
	InitOBB.Extents = { fMaxDist , fMaxDist , fMaxDist };
	InitSphere.Radius = fMaxDist;
}

void CPGCBoundingVolume::setMatrix(CPM_Matrix* pWorld)
{
	if (pWorld) { matWorld = *pWorld; }

	InitOBB.Transform(OBB, matWorld);
	InitSphere.Transform(Sphere, matWorld);
}