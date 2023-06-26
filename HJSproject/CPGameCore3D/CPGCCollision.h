//////////////////////////////////////////////////
//
// CPGCCollision.h
//		충돌 검사를 위한 헤더
//		충돌 감지 및 교점, 교차 영역 검사 일부를 수행한다.
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
	//교차 판정

	//점과의 판정
	static UINT pointToPoint(const CPM_Vector3& p1, const CPM_Vector3& p2);

	//레이 및 세그먼트와의 판정 : 주된 목적은 피킹 레이의 사용
	static UINT rayToPoint(const CPM_Ray& ray, const CPM_Vector3& pt);
	static UINT rayToRay(const CPM_Ray& ray1, const CPM_Ray& ray2);

	static UINT rayToAABB(const CPM_Ray& ray, const CPRS_BOUNDING_BOX& box);
	static UINT rayToAABB(const CPM_Ray& ray, const CPRS_AABB& aabb);

	//6개의 분리축을 이용하여 교차 판정을 수행한다.
	static UINT rayToOBB(const CPM_Ray& ray, const CPRS_BOUNDING_BOX& box);
	static UINT rayToOBB(const CPM_Ray& ray, const CPRS_OBB& obb);

	//OBB-OBB
	static UINT OBBToOBB(const CPRS_BOUNDING_BOX& box1, const CPRS_BOUNDING_BOX& box2);


	//교차 지점, 영역 구하기

	//레이 교차 지점 판정
	
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