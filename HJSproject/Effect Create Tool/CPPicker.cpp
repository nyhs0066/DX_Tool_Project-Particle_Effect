#include "CPPicker.h"

CPPicker::CPPicker()
{
	ZeroMemory(&m_targetVPort, sizeof(D3D11_VIEWPORT));
}

CPPicker::~CPPicker()
{
}

bool CPPicker::init()
{
	return true;
}

bool CPPicker::release()
{
	return true;
}

void CPPicker::setTargetWindow(const D3D11_VIEWPORT& vPort)
{
	m_targetVPort = vPort;
}

void CPPicker::setMatrix(const CPM_Matrix* pWorldM, const CPM_Matrix* pViewM, const CPM_Matrix* pProjM)
{
	if (pWorldM) { m_matWorld = *pWorldM; }
	if (pViewM) { m_matView = *pViewM; }
	if (pProjM) { m_matProj = *pProjM; }

	updateRay();
}

void CPPicker::updateRay()
{
	//���콺 ��ǥ �޾ƿ���
	POINT pt = CPGC_MAININPUT.getMousePos();

	//����Ʈ ��ǥ�� -> ���� ��ǥ��
	float px, py, pz;
	px = 2 * (pt.x - m_targetVPort.TopLeftX) / m_targetVPort.Width - 1;
	py = -2 * (pt.y - m_targetVPort.TopLeftY) / m_targetVPort.Height + 1;
	pz = 1;

	//���� ��ǥ�� -> �� ��ǥ��
	float vx, vy, vz;
	vx = px / m_matProj._11;
	vy = py / m_matProj._22;
	vz = 1;

	//Ray : �� ��ǥ�� -> ���� ��ǥ�� / ���� ��ǥ��
	//��� ��ŷ ������Ʈ�� ���� ����� ����� ��� �ش� ��ŷ ������Ʈ�� ���� ���۴�
	//��ǥ�� ��ȯ ����� �ݿ��Ǳ� ������ �����̹Ƿ� ���� ��� * �� ����� ������� ���Ͽ�
	//��ŷ ������ ���߾� �ش�.
	//���� ��쿡�� ���� ���� ����� ������� �����Ƿ� ��������� ���� ����� �ȴ�.
	m_ray.stPos = { 0.0f, 0.0f, 0.0f };
	m_ray.direction = { vx, vy, vz };
	CPM_Matrix invWorldViewM = (m_matWorld * m_matView).Invert();

	//Ray�� ������ ���� ��� ����ķ� ���� ��ǥ�踦 �̵���Ų��.
	//����İ� ���ϵ� ��ġ ���ʹ� w������ ������ XMVector3TransformCoord�Լ���,
	//���� ���ʹ� w������ ������ �ʴ� XMVector3TransformNormal�Լ��� ����Ѵ�.
	m_ray.stPos = DirectX::XMVector3TransformCoord(m_ray.stPos, invWorldViewM);
	m_ray.direction = DirectX::XMVector3TransformNormal(m_ray.direction, invWorldViewM);

	//���⺤�ʹ� ����ȭ �� ���� �ƴϹǷ� ����ȭ �۾��� �����Ѵ�.
	m_ray.direction.Normalize();

	m_ray.ext = CPGC_RAY_LENGTH_LIMIT;
}

//�۵� ������ ���� ����� ���� �Լ� ����
bool CPPicker::getTriangleIntersection(const CPM_Vector3& v0,
	const CPM_Vector3& v1,
	const CPM_Vector3& v2)
{
	//�ﰢ�� ����
	//		v0
	// eB /    \ eA
	//   v2 �� v1
	//		
	//v0�� �������� �Ѵ�.
	CPM_Vector3 eA = v1 - v0;
	CPM_Vector3 eB = v2 - v0;

	CPM_Vector3 TUV;

	//���� �������� �������� ��Ľ� ���� : | -dir eA eB | = -dir*(eA X eB) = eA*(dir X eB)
	CPM_Vector3 qVec = m_ray.direction.Cross(eB);
	FLOAT fDet = eA.Dot(qVec);
	if (fDet < 0.0001f) { return false; }	//Divide By Zero���� ������ ���� ��� ������ ���Ѻ��� ������� ��Ľ��� 0�� ������ �����Ѵ�.

	//�и�� ����ϴ� ��Ľ��� ������ ��� ����� ��ȯ - �������� ���� pVec�� ��ȣ�� �����ϴ� ������ ��ü
	CPM_Vector3 pVec;
	if (fDet > 0) { pVec = m_ray.stPos - v0; }
	else { pVec = v0 - m_ray.stPos; fDet = -fDet; }

	// u ���
	TUV.y = pVec.Dot(qVec);
	if (TUV.y < 0.0f || TUV.y > fDet) { return false; } //0.0f ~ 1.0f ������ ����ٸ� �ﰢ���� �����.

	// v ���
	CPM_Vector3 rVec = pVec.Cross(eA);
	TUV.z = m_ray.direction.Dot(rVec);
	if (TUV.z < 0.0f || TUV.y + TUV.z > fDet) { return false; } // u,v ���� �ջ��� 0.0f ~ 1.0f������ ����ٸ� �ﰢ���� �����.

	// t ��� : ����ȭ �Ǿ��ִ� ���� ���͸� ����� ���̹Ƿ� u, v�� �´ٸ� t�� �Ÿ��� ���� �� �ִ�.
	TUV.x = eB.Dot(rVec);

	// ��Ľ��� ������ ���� �����ν� �������� ����Ѵ�.
	FLOAT fInvDet = 1.0f / fDet;
	TUV.x *= fInvDet;
	TUV.y *= fInvDet;
	TUV.z *= fInvDet;

	m_vISPoint = m_ray.stPos + (TUV.x * m_ray.direction);

	return true;
}

bool CPPicker::getPickingRay(float x, float y,
	const D3D11_VIEWPORT& vPort,
	const CPM_Matrix* pWorldM,
	const CPM_Matrix* pViewM,
	const CPM_Matrix* pProjM,
	CPM_Ray* pOut)
{
	if (!pViewM || !pProjM) { return false; }

	//����Ʈ ��ǥ�� -> ���� ��ǥ��
	// ���� ������ Z���� ���� ���ۿ� ����Ǿ� �ִ� ���̸� ���Ѱ� ������ ����Ʈ ������ �� ���� �ȴ�.

	//����Ʈ ��� : X, Y�� ����Ʈ ���� ��ǥ
	//	Width * 0.5f				0				0				0
	//		0				-Height * 0.5f			0				0
	//		0						0			MaxZ - MinZ			0
	//	X + Width * 0.5f	Y - Height * 0.5f		MinZ			1

	//	��ȯ ���� : Sx - ȭ�� ��ǥ / Px - ���� ��ǥ
	//	Sx = (Px * Width * 0.5f) + X + Width * 0.5f = (Px + 1) * 0.5f * Width + X
	//	Sy = (-Py * Height * 0.5f) + Y - Height * 0.5f = (-Py - 1) * 0.5f * Height + Y
	//	Sz = (Pz * (MaxZ - MinZ)) + MinZ

	//	����ȯ ����
	//	Px = 2 * (Sx - X) / Width - 1;
	//	Py = -2 * (Sx - Y) / Height + 1;
	//	Pz = (Sz - MinZ) / (MaxZ - MinZ);	���̹����� ������ 1.0f�̰� ������ 0.0f�̸� Pz = Sz

	float px, py, pz;
	px = 2 * (x - vPort.TopLeftX) / vPort.Width - 1;
	py = -2 * (y - vPort.TopLeftY) / vPort.Height + 1;
	pz = 1;

	//���� ��ǥ�� -> �� ��ǥ��

	//���� ��� : �ٴܸ��� �ʺ� W, �ٴܸ��� ���� H, �ٴܸ� ������ �Ÿ� nearZ ���ܸ� ������ �Ÿ� farZ
	//	2 * nearZ / W			0					0						0
	//		0			2 * nearZ / H				0						0
	//		0					0			farZ / (farZ - nearZ)			1
	//		0					0		(-farZ * nearZ) / (farZ - nearZ)	0

	//�þ߰��� �̿��� ���� ��� : ���� �þ߰� HFOV, ȭ�� ��Ⱦ�� AspectRatio, �ٴܸ� ������ �Ÿ� nearZ, ���ܸ� ������ �Ÿ� farZ
	//	AspectRatio / tan(HFOV / 2)				0							0							0
	//				0					1 / tan(HFOV / 2)					0							0
	//				0							0					farZ / (farZ - nearZ)				1
	//				0							0				(-farZ * nearZ) / (farZ - nearZ)		0

	//	��ȯ ����
	//	Px = 2 * Vx * nearZ / W = Vx * AspectRatio / tan(HFOV / 2);
	//	Py = 2 * Vy * nearZ / H = Vy / tan(HFOV / 2);
	//	Pz = Vz * farZ / (farZ - nearZ) - (farZ * nearZ) / (farZ - nearZ)
	//		= (Vz - nearZ) * farZ / (farZ - nearZ);
	//
	//	Pw = Vz;

	//	����ȯ ���� : ���� ����� ������ ��ġ�� �κ��� _11, _22���� ���̹Ƿ� �̸� �ݿ��Ѵ�.
	//	Vx = Px * W / (2 * nearZ) = Px * tan(HFOV / 2) / AspectRatio;
	//	Vy = Py * H / (2 * nearZ) = Py * tan(HFOV / 2);
	//	Vz = (Pz * (farZ - nearZ) / farZ) + nearZ
	// 
	//���콺 ��ŷ�� ī�޶� ��ġ���� ����ǥ�� �ٴܸ鿡���� ���������� Ray�� ��� �Ϳ� �ش��ϹǷ� Vz = nearZ

	float vx, vy, vz;
	vx = px / (*pProjM)._11;
	vy = py / (*pProjM)._22;
	vz = 1;							//vz = matProj.fNearZ; �ٴܸ� ������ �Ÿ��� 1�� �ƴ϶�� �̷��� ���ִ°� �°���

	//�� ��ǥ�迡�� Ray���� : Ray�� ������ �� ��ǥ���� ����, ���� ��ǥ�� Ray�� ���� ���Ϳ� �ش��Ѵ�.
	//�� �� Ray�� �� ���� z��ǥ�� ���ܸ��� �Ÿ������� �Ѱ踦 �ִ� ���� �ٶ����ϴ�.
	//�浹 ����� ���� ��ǥ��� �Ǿ� �ִ� ��� ���� ����� ����ı��� �����Ѵ�. 
	// (���� ��� * �� ���).invert()

	//Ray : �� ��ǥ�� -> ���� ��ǥ�� / ���� ��ǥ��
	//��� ��ŷ ������Ʈ�� ���� ����� ����� ��� �ش� ��ŷ ������Ʈ�� ���� ���۴�
	//��ǥ�� ��ȯ ����� �ݿ��Ǳ� ������ �����̹Ƿ� ���� ��� * �� ����� ������� ���Ͽ�
	//��ŷ ������ ���߾� �ش�.
	pOut->stPos = { 0.0f, 0.0f, 0.0f };
	pOut->direction = { vx, vy, vz };

	CPM_Matrix invWorldViewM;

	if (pWorldM)
	{
		invWorldViewM = ((*pWorldM) * (*pViewM)).Invert();
	}
	else
	{
		invWorldViewM = (*pViewM).Invert();
	}

	//Ray�� ������ ���� ��� ����ķ� ���� ��ǥ�踦 �̵���Ų��.
	//����İ� ���ϵ� ��ġ ���ʹ� w������ ������ XMVector3TransformCoord�Լ���,
	//���� ���ʹ� w������ ������ �ʴ� XMVector3TransformNormal�Լ��� ����Ѵ�.
	pOut->stPos = DirectX::XMVector3TransformCoord(pOut->stPos, invWorldViewM);
	pOut->direction = DirectX::XMVector3TransformNormal(pOut->direction, invWorldViewM);

	//���⺤�ʹ� ����ȭ �� ���� �ƴϹǷ� ����ȭ �۾��� �����Ѵ�.
	pOut->direction.Normalize();

	pOut->ext = CPGC_RAY_LENGTH_LIMIT;

	return true;
}

bool CPPicker::pointInPlane(CPM_Vector3 vStart, CPM_Vector3 vEnd,
	CPM_Vector3 vPlaneNormal,
	CPM_Vector3 v0, CPM_Vector3 v1, CPM_Vector3 v2, CPM_Vector3* pVOut)
{
	//Ray : vStart / vEnd 
	//Plane : vPlaneNormal, v0, v1, v2

	if (!pVOut) { return false; }

	//Ȯ���ϰ����ϴ� ����
	CPM_Ray ray = { vStart , vEnd - vStart, -1.0f };	//���� ����ȭ ������

	//���� ���͸� ����� �븻�� ������ ���⺤���� ��� ���� ���Ϳ� ���� ���������� ��ü ���̸� ���Ѵ�.
	float D = vPlaneNormal.Dot(ray.direction);

	//������ ������ ���� �Ǵ��� �ϰ��� �ϴ� �ﰢ���� ������ ���� ���� �ϳ��� ��� �������͸� ������
	//�������� �������� �Ÿ��� ���Ѵ�.
	float a0 = vPlaneNormal.Dot(v0 - vStart);

	//������ ������ ���������� �Ÿ��� �Ǵ��ϱ� ���� ����� �κ����� ������ ������ ���Ѵ�.
	float fRatio = a0 / D;

	//0.0f ~ 1.0f�� ������ �� ����� ��쿡�� ���� ó���Ѵ�.
	if (fRatio < 0.0f || fRatio > 1.0f)
	{
		return false;
	}

	ray.direction.Normalize();
	*pVOut = vStart + ray.direction * fRatio;
	return true;
}

bool CPPicker::pointInPolygon(CPM_Vector3 vTarget, CPM_Vector3 vPlaneNormal,
	CPM_Vector3 v0, CPM_Vector3 v1, CPM_Vector3 v2)
{
	//�ﰢ���� ������ ���� �����Ǿ� �ְ� ��� �븻�� ȭ�� �ٱ����� ���ϴ� �������� �����Ѵ�.
	//		v0
	// e2 /    \ e0
	//   v2 �� v1
	//		e1

	CPM_Vector3 e0, e1, e2, eTarget, vTargetNormal;
	e0 = v1 - v0;
	e1 = v2 - v1;
	e2 = v0 - v2;

	//�ﰢ���� �̷�� ������ �ﰢ���� ������ ���� �������� ������ ���� ���� �븻�� 
	//�μ��� ���� �븻�� ������ ���⼺�� Ȯ���Ѵ�. ������ ����� ����� �ﰢ�� ������ ���ɼ��� ������ ������ ��� ������ �ܺη� ó���Ѵ�.

	//e0�� v0 - ������ ���������� ���� 
	eTarget = vTarget - v0;
	vTargetNormal = e0.Cross(eTarget);
	vTargetNormal.Normalize();

	float fRet = vPlaneNormal.Dot(vTargetNormal);
	if (fRet < 0.0f) { return false; }

	//e1�� v1 - ������ ���������� ����
	eTarget = vTarget - v1;
	vTargetNormal = e1.Cross(eTarget);
	vTargetNormal.Normalize();

	fRet = vPlaneNormal.Dot(vTargetNormal);
	if (fRet < 0.0f) { return false; }

	////e2�� v2 - ������ ���������� ����
	eTarget = vTarget - v2;
	vTargetNormal = e2.Cross(eTarget);
	vTargetNormal.Normalize();

	fRet = vPlaneNormal.Dot(vTargetNormal);
	if (fRet < 0.0f) { return false; }

	//���� ��� �ߴٸ� �ﰢ�� ������ ���̹Ƿ� ���� ��ȯ�Ѵ�.
	return true;
};

bool CPPicker::IntersectTriangle(CPM_Vector3& origin, CPM_Vector3& dir,
	CPM_Vector3& v0, CPM_Vector3& v1, CPM_Vector3& v2, CPM_Vector3& outTUV)
{
	//�ﰢ�� ����
	//		v0
	// eB /    \ eA
	//   v2 �� v1
	//		

	//Ray�� direction�� ����ȭ �Ǿ� �ִٰ� �����Ѵ�.
	//���� ������ �������� ���� Ư�� ���� �ݺ��Ͽ� ����Ѵ�.

	//v0�� �������� �Ѵ�.
	CPM_Vector3 eA = v1 - v0;
	CPM_Vector3 eB = v2 - v0;

	// fDet = det| -dir eA eB |
	// pVec = origin - origin - (v0 - origin) = origin - v0
	// qVec = dir X eB
	// rVec = pVec X eA

	//t = |pVec eA eB| / |-dir eA eB| = 
	//u = |-dir pVec eB| / |-dir eA eB| = pVec*qVec / det
	//v = |-dir eA pVec| / |-dir eA eB| = 

	//���� �������� �������� ��Ľ� ���� : | -dir eA eB | = -dir*(eA X eB) = eA*(dir X eB)
	CPM_Vector3 qVec = dir.Cross(eB);
	FLOAT fDet = eA.Dot(qVec);
	if (fDet < 0.0001f) { return false; }	//Divide By Zero���� ������ ���� ��� ������ ���Ѻ��� ������� ��Ľ��� 0�� ������ �����Ѵ�.

	//�и�� ����ϴ� ��Ľ��� ������ ��� ����� ��ȯ - �������� ���� pVec�� ��ȣ�� �����ϴ� ������ ��ü
	CPM_Vector3 pVec;
	if (fDet > 0) { pVec = origin - v0; }
	else { pVec = v0 - origin; fDet = -fDet; }

	// u ���
	outTUV.y = pVec.Dot(qVec);
	if (outTUV.y < 0.0f || outTUV.y > fDet) { return false; } //0.0f ~ 1.0f ������ ����ٸ� �ﰢ���� �����.

	// v ���
	CPM_Vector3 rVec = pVec.Cross(eA);
	outTUV.z = dir.Dot(rVec);
	if (outTUV.z < 0.0f || outTUV.y + outTUV.z > fDet) { return false; } // u,v ���� �ջ��� 0.0f ~ 1.0f������ ����ٸ� �ﰢ���� �����.

	// t ��� : ����ȭ �Ǿ��ִ� ���� ���͸� ����� ���̹Ƿ� u, v�� �´ٸ� t�� �Ÿ��� ���� �� �ִ�.
	outTUV.x = eB.Dot(rVec);

	// ��Ľ��� ������ ���� �����ν� �������� ����Ѵ�.
	FLOAT fInvDet = 1.0f / fDet;
	outTUV.x *= fInvDet;
	outTUV.y *= fInvDet;
	outTUV.z *= fInvDet;
	return true;
}