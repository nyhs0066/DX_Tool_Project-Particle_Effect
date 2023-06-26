#pragma once
#include "CPGCInput.h"
#include "CPGCCollision.h"

class CPPicker
{
public:
	CPM_Ray			m_ray;				//피킹 레이
	CPM_Vector3		m_vISPoint;			//교점

	//레이변환을 위한 좌표계 변환 행렬
	CPM_Matrix		m_matWorld;			
	CPM_Matrix		m_matView;
	CPM_Matrix		m_matProj;

	//피킹 대상 뷰포트
	D3D11_VIEWPORT	m_targetVPort;

public:
	CPPicker();
	~CPPicker();

	bool init();
	bool release();

	void setTargetWindow(const D3D11_VIEWPORT& vPort);

	//피킹레이를 구하기 위한 행렬 세팅
	void setMatrix(const CPM_Matrix* pWorldM,
		const CPM_Matrix* pViewM,
		const CPM_Matrix* pProjM);

	//피킹 레이 속성 업데이트
	void updateRay();
	bool getTriangleIntersection(const CPM_Vector3& v0,
		const CPM_Vector3& v1,
		const CPM_Vector3& v2);

public:
	//피킹 레이 구하기
	static bool getPickingRay(float x, float y,
		const D3D11_VIEWPORT& vPort,
		const CPM_Matrix* pWorldM,
		const CPM_Matrix* pViewM,
		const CPM_Matrix* pProjM,
		CPM_Ray* pOut);

	//교점 찾기

	//외적을 이용한 방법 - 교점이 존재해야 할 평면위에 점이 있는지 확인한다. 
	static bool pointInPlane(CPM_Vector3 vStart, CPM_Vector3 vEnd,
		CPM_Vector3 vPlaneNormal,
		CPM_Vector3 v0, CPM_Vector3 v1, CPM_Vector3 v2, CPM_Vector3* pVOut);

	//위 방법으로 구한 교점이 삼각형 내부에 존재한다는 보장을 위한 점 포함 테스트를 수행하는 함수
	//삼각형을 구성하는 각 간선과 내부 확인 검사를 수행한다.
	static bool pointInPolygon(CPM_Vector3 vTarget, CPM_Vector3 vPlaneNormal,
		CPM_Vector3 v0, CPM_Vector3 v1, CPM_Vector3 v2);

	//^^^ 위 방법은 노말의 방향이 바뀌어도 처리는 가능하지만 백페이스 방향에서의 레이에 의한 교점 형성을 체크해 주어야한다.
	//^^^ 그래서 사전에 노말이 계산되어 고정된 방법으로 처리되는 것을 기준으로 한다.

	//vvv아래의 방법은 레이와 삼각형의 정점만 주면 교점인지 아닌지 판단한다.

	//비율 매개변수 t,u,v를 이용한 방법 - t, u, v만 구하는 것이므로 교점을 따로 만들어 주어야 한다.
	//교점은 다음과 같다.
	//반직선의 시점과 방향 벡터를 이용 = origin + t * direction
	static bool IntersectTriangle(CPM_Vector3& origin, CPM_Vector3& dir,
		CPM_Vector3& v0, CPM_Vector3& v1, CPM_Vector3& v2, CPM_Vector3& outTUV);
};

