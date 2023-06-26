#include "CPGCCollision.h"

UINT CPGCCollision::pointToPoint(const CPM_Vector3& p1, const CPM_Vector3& p2)
{
	//�Ÿ��� 0���� �����Ǵ� �����ȿ� ��� �Դ��� �Ǵ��Ѵ�.
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

	//������ �Ÿ��� �̿��� �����Ѵ�.
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
	//�� ������ ��ġ����� ������ �ִ�. ��ġ���迡 ���� �����Ѵ�.
	//������ �ִ� ��� : ��ġ, 1�� ����
	//������ ���� ��� : ����, ������ġ

	//������ ���� ���
	if (ray1.direction == ray2.direction)
	{
		//������ ������ ��� �ϳ��� �������� ������ ����
		if (rayToPoint(ray1, ray2.stPos) || rayToPoint(ray1, ray2.stPos + ray2.direction * ray2.ext))
		{
			return CPGC_CO_HIT;
		}
		else { return CPGC_CO_NO_HIT; }
	}
	else
	{
		//������ �ٸ� ���

		//���� ��ġ�� �ִٸ� �Ÿ��� �����Ѵ�.
		//�� ������ �̷�� ����� �������Ϳ� �� ������ ���Ե� �� ���� ���� ���͸� �翵�غ���.

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
	//�и��� �̷��� �̿��Ѵ�.
	//OBB�� ������ ������ ������ ���� ����(R = O - C)�� �̿��� 
	//������ ���⺤�Ϳ� OBB�� �� �𼭸��� ������ ���� ���� ���� ������ �����ϰ�
	//OBB�� ���� ���� ���� ������ �������� ������ ���⼺ �˻�� ����Ѵ�.

	//1. OBB�� ���� ���� �� �˻� : �������� ������ ���⼺ �˻縦 �Բ� �����Ѵ�.
	//���� �˻�� OBB�� �������� ���̿� R�� OBB�� ��� ������ ���� ���̸� ���Ѵ�.
	//(OBB�� �������� ����) < abs(DOT(R, OBB�� ��)) => �ܺ�
	//���� �˻�� ������ ���⺤�Ϳ� R���� ������ ���� ���� ��ȣ�� �����Ѵ�.
	// R�� ������ ���Ϳ��� ������ �����̹Ƿ� �� ������ ����� ����� �ڽ��� ��� ��������
	// ���̰� �����Ѵٴ� �ǹ̰��ȴ�.
	//DOT(ray.direction, OBB�� ��) * DOT(R, OBB�� ��) >= 0.0f => �ܺ�

	//����ȭ ����
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

	//����ȭ ���� : OD => Origin Dot / DD => Direction Dot
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

	//2. ������ ���⺤�Ϳ� �𼭸��� �������� �� ������ �˻�

	//����ȭ ����
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

	//����ȭ ���� : ��Į�� �������� Ư���� �̿��� ���� ������ 
	//������ �� �ึ�� �����ϴ� ���� �ƴ� ������ ���⺤�Ϳ� centerToOrigin���� �ѹ��� ������
	//�ݺ��Ͽ� ����Ѵ�.
	//OBB�� �����ε� OBB�� ������ ������ �ٸ� OBB�� ���� �ȴٴ� ����
	//��Į�� �������� Ư��, ������ ����� ������ Ư���� �̿��� ������ �����Ѵ�.
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
	//�и��� �̷��� �̿��Ѵ�.
	//15���� �и����� �˻��Ѵ�.
	//	-�� OBB�������� �������� 6��
	//	-�� OBB�� ������ �������� �ϳ��� �ٸ� OBB�� ������ �������͸� ������ ���� ���ο� �и��� ���� 9��
	//�� OBB�� ������ ���� ������ �и��� ���� ���̿� �� OBB�� �������� �и��� ���������� ���� ���Ѵ�.
	// 
	//(�� OBB�� ������ ���� ������ �и��� ���� ����) > (�� OBB�� �������� �и��� ���������� ��) : �浹 ����
	//�̿��� ����� �浹 ������ �����Ѵ�.

	//0. 15���� �и���

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

	//1. �˻� ���� ����

	CPM_Vector3 C2C = box1.m_vCPos - box2.m_vCPos;
	CPM_Vector3 B1Ext = box1.m_vAxis[0] * box1.m_vExt.x + box1.m_vAxis[1] * box1.m_vExt.y + box1.m_vAxis[2] * box1.m_vExt.z;
	CPM_Vector3 B2Ext = box2.m_vAxis[0] * box2.m_vExt.x + box2.m_vAxis[1] * box2.m_vExt.y + box2.m_vAxis[2] * box2.m_vExt.z;

	//����ȭ ����
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