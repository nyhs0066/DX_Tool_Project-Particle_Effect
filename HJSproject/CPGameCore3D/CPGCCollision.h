//////////////////////////////////////////////////
//
// CPGCCollision.h
//		�浹 �˻縦 ���� ���
//		�浹 ���� �� ����, ���� ���� �˻� �Ϻθ� �����Ѵ�.
// 
//////////////////////////////////////////////////

#pragma once
#include "CPRSShape.h"

enum CPGC_COLLISION_TYPE
{
	CPGC_CO_NO_HIT = 0,
	CPGC_CO_HIT = 1,
	CPGC_CO_NUMBER_OF_TYPE
};

class CPGCCollision
{
public:
	//���� ����

	//������ ����
	static UINT pointToPoint(const CPM_Vector3& p1, const CPM_Vector3& p2);

	//���� �� ���׸�Ʈ���� ���� : �ֵ� ������ ��ŷ ������ ���
	static UINT rayToPoint(const CPM_Ray& ray, const CPM_Vector3& pt);
	static UINT rayToRay(const CPM_Ray& ray1, const CPM_Ray& ray2);

	static UINT rayToAABB(const CPM_Ray& ray, const CPRS_BOUNDING_BOX& box);
	static UINT rayToAABB(const CPM_Ray& ray, const CPRS_AABB& aabb);

	//6���� �и����� �̿��Ͽ� ���� ������ �����Ѵ�.
	static UINT rayToOBB(const CPM_Ray& ray, const CPRS_BOUNDING_BOX& box);
	static UINT rayToOBB(const CPM_Ray& ray, const CPRS_OBB& obb);

	//OBB-OBB
	static UINT OBBToOBB(const CPRS_BOUNDING_BOX& box1, const CPRS_BOUNDING_BOX& box2);


	//���� ����, ���� ���ϱ�

	//���� ���� ���� ����
	
};

class CPGCBoundingVolume
{
public:
	DirectX::BoundingOrientedBox InitOBB;
	DirectX::BoundingOrientedBox OBB;

	DirectX::BoundingSphere InitSphere;
	DirectX::BoundingSphere Sphere;

	CPM_Matrix matWorld;

public:
	CPGCBoundingVolume();
	CPGCBoundingVolume(CPM_Vector3 exts, CPM_Vector3 center = { 0.0f, 0.0f, 0.0f });
	CPGCBoundingVolume(CPM_Vector3 center, CPM_Vector3 exts, CPM_Quaternion rotation);
	CPGCBoundingVolume(float radius, CPM_Vector3 center = { 0.0f, 0.0f, 0.0f });
	CPGCBoundingVolume(DirectX::BoundingOrientedBox& OBB);

	void adjustVolume(CPM_Vector3 exts);
	void adjustVolume(float fMaxDist);

	void setMatrix(CPM_Matrix* pWorld);
};