#pragma once
#include "CPGCInput.h"
#include "CPGCCollision.h"

class CPPicker
{
public:
	CPM_Ray			m_ray;				//��ŷ ����
	CPM_Vector3		m_vISPoint;			//����

	//���̺�ȯ�� ���� ��ǥ�� ��ȯ ���
	CPM_Matrix		m_matWorld;			
	CPM_Matrix		m_matView;
	CPM_Matrix		m_matProj;

	//��ŷ ��� ����Ʈ
	D3D11_VIEWPORT	m_targetVPort;

public:
	CPPicker();
	~CPPicker();

	bool init();
	bool release();

	void setTargetWindow(const D3D11_VIEWPORT& vPort);

	//��ŷ���̸� ���ϱ� ���� ��� ����
	void setMatrix(const CPM_Matrix* pWorldM,
		const CPM_Matrix* pViewM,
		const CPM_Matrix* pProjM);

	//��ŷ ���� �Ӽ� ������Ʈ
	void updateRay();
	bool getTriangleIntersection(const CPM_Vector3& v0,
		const CPM_Vector3& v1,
		const CPM_Vector3& v2);

public:
	//��ŷ ���� ���ϱ�
	static bool getPickingRay(float x, float y,
		const D3D11_VIEWPORT& vPort,
		const CPM_Matrix* pWorldM,
		const CPM_Matrix* pViewM,
		const CPM_Matrix* pProjM,
		CPM_Ray* pOut);

	//���� ã��

	//������ �̿��� ��� - ������ �����ؾ� �� ������� ���� �ִ��� Ȯ���Ѵ�. 
	static bool pointInPlane(CPM_Vector3 vStart, CPM_Vector3 vEnd,
		CPM_Vector3 vPlaneNormal,
		CPM_Vector3 v0, CPM_Vector3 v1, CPM_Vector3 v2, CPM_Vector3* pVOut);

	//�� ������� ���� ������ �ﰢ�� ���ο� �����Ѵٴ� ������ ���� �� ���� �׽�Ʈ�� �����ϴ� �Լ�
	//�ﰢ���� �����ϴ� �� ������ ���� Ȯ�� �˻縦 �����Ѵ�.
	static bool pointInPolygon(CPM_Vector3 vTarget, CPM_Vector3 vPlaneNormal,
		CPM_Vector3 v0, CPM_Vector3 v1, CPM_Vector3 v2);

	//^^^ �� ����� �븻�� ������ �ٲ� ó���� ���������� �����̽� ���⿡���� ���̿� ���� ���� ������ üũ�� �־���Ѵ�.
	//^^^ �׷��� ������ �븻�� ���Ǿ� ������ ������� ó���Ǵ� ���� �������� �Ѵ�.

	//vvv�Ʒ��� ����� ���̿� �ﰢ���� ������ �ָ� �������� �ƴ��� �Ǵ��Ѵ�.

	//���� �Ű����� t,u,v�� �̿��� ��� - t, u, v�� ���ϴ� ���̹Ƿ� ������ ���� ����� �־�� �Ѵ�.
	//������ ������ ����.
	//�������� ������ ���� ���͸� �̿� = origin + t * direction
	static bool IntersectTriangle(CPM_Vector3& origin, CPM_Vector3& dir,
		CPM_Vector3& v0, CPM_Vector3& v1, CPM_Vector3& v2, CPM_Vector3& outTUV);
};

