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
	//마우스 좌표 받아오기
	POINT pt = CPGC_MAININPUT.getMousePos();

	//뷰포트 좌표계 -> 투영 좌표계
	float px, py, pz;
	px = 2 * (pt.x - m_targetVPort.TopLeftX) / m_targetVPort.Width - 1;
	py = -2 * (pt.y - m_targetVPort.TopLeftY) / m_targetVPort.Height + 1;
	pz = 1;

	//투영 좌표계 -> 뷰 좌표계
	float vx, vy, vz;
	vx = px / m_matProj._11;
	vy = py / m_matProj._22;
	vz = 1;

	//Ray : 뷰 좌표계 -> 월드 좌표계 / 로컬 좌표계
	//대상 피킹 오브젝트가 월드 행렬이 적용된 경우 해당 피킹 오브젝트의 정점 버퍼는
	//좌표계 변환 행렬이 반영되기 이전의 상태이므로 월드 행렬 * 뷰 행렬의 역행렬을 곱하여
	//피킹 지점을 맞추어 준다.
	//맵의 경우에는 따로 월드 행렬이 적용되지 않으므로 단위행렬이 월드 행렬이 된다.
	m_ray.stPos = { 0.0f, 0.0f, 0.0f };
	m_ray.direction = { vx, vy, vz };
	CPM_Matrix invWorldViewM = (m_matWorld * m_matView).Invert();

	//Ray의 원점과 방향 모두 역행렬로 곱해 좌표계를 이동시킨다.
	//역행렬과 곱하되 위치 벡터는 w값으로 나누는 XMVector3TransformCoord함수를,
	//방향 벡터는 w값으로 나누지 않는 XMVector3TransformNormal함수를 사용한다.
	m_ray.stPos = DirectX::XMVector3TransformCoord(m_ray.stPos, invWorldViewM);
	m_ray.direction = DirectX::XMVector3TransformNormal(m_ray.direction, invWorldViewM);

	//방향벡터는 정규화 된 값이 아니므로 정규화 작업을 수행한다.
	m_ray.direction.Normalize();

	m_ray.ext = CPGC_RAY_LENGTH_LIMIT;
}

//작동 설명은 하위 기술된 정적 함수 참고
bool CPPicker::getTriangleIntersection(const CPM_Vector3& v0,
	const CPM_Vector3& v1,
	const CPM_Vector3& v2)
{
	//삼각형 예시
	//		v0
	// eB /    \ eA
	//   v2 ㅡ v1
	//		
	//v0을 기준으로 한다.
	CPM_Vector3 eA = v1 - v0;
	CPM_Vector3 eB = v2 - v0;

	CPM_Vector3 TUV;

	//연립 방정식의 계수행렬의 행렬식 계산부 : | -dir eA eB | = -dir*(eA X eB) = eA*(dir X eB)
	CPM_Vector3 qVec = m_ray.direction.Cross(eB);
	FLOAT fDet = eA.Dot(qVec);
	if (fDet < 0.0001f) { return false; }	//Divide By Zero오류 방지를 위해 허용 오차의 하한보다 작은경우 행렬식이 0인 것으로 간주한다.

	//분모로 사용하는 행렬식이 음수인 경우 양수로 변환 - 공통으로 들어가는 pVec의 부호를 변경하는 것으로 대체
	CPM_Vector3 pVec;
	if (fDet > 0) { pVec = m_ray.stPos - v0; }
	else { pVec = v0 - m_ray.stPos; fDet = -fDet; }

	// u 계산
	TUV.y = pVec.Dot(qVec);
	if (TUV.y < 0.0f || TUV.y > fDet) { return false; } //0.0f ~ 1.0f 범위를 벗어난다면 삼각형을 벗어난다.

	// v 계산
	CPM_Vector3 rVec = pVec.Cross(eA);
	TUV.z = m_ray.direction.Dot(rVec);
	if (TUV.z < 0.0f || TUV.y + TUV.z > fDet) { return false; } // u,v 둘의 합산이 0.0f ~ 1.0f범위를 벗어난다면 삼각형을 벗어난다.

	// t 계산 : 졍규화 되어있는 방향 벡터를 사용할 것이므로 u, v만 맞다면 t로 거리를 구할 수 있다.
	TUV.x = eB.Dot(rVec);

	// 행렬식의 역수를 곱해 줌으로써 나눗셈을 대신한다.
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

	//뷰포트 좌표계 -> 투영 좌표계
	// 투영 정점의 Z값은 깊이 버퍼에 저장되어 있는 값이며 상한과 하한은 뷰포트 생성시 준 값이 된다.

	//뷰포트 행렬 : X, Y는 뷰포트 시작 좌표
	//	Width * 0.5f				0				0				0
	//		0				-Height * 0.5f			0				0
	//		0						0			MaxZ - MinZ			0
	//	X + Width * 0.5f	Y - Height * 0.5f		MinZ			1

	//	변환 수식 : Sx - 화면 좌표 / Px - 투영 좌표
	//	Sx = (Px * Width * 0.5f) + X + Width * 0.5f = (Px + 1) * 0.5f * Width + X
	//	Sy = (-Py * Height * 0.5f) + Y - Height * 0.5f = (-Py - 1) * 0.5f * Height + Y
	//	Sz = (Pz * (MaxZ - MinZ)) + MinZ

	//	역변환 수식
	//	Px = 2 * (Sx - X) / Width - 1;
	//	Py = -2 * (Sx - Y) / Height + 1;
	//	Pz = (Sz - MinZ) / (MaxZ - MinZ);	깊이버퍼의 상한이 1.0f이고 하한은 0.0f이면 Pz = Sz

	float px, py, pz;
	px = 2 * (x - vPort.TopLeftX) / vPort.Width - 1;
	py = -2 * (y - vPort.TopLeftY) / vPort.Height + 1;
	pz = 1;

	//투영 좌표계 -> 뷰 좌표계

	//투영 행렬 : 근단면의 너비 W, 근단면의 높이 H, 근단면 까지의 거리 nearZ 원단면 까지의 거리 farZ
	//	2 * nearZ / W			0					0						0
	//		0			2 * nearZ / H				0						0
	//		0					0			farZ / (farZ - nearZ)			1
	//		0					0		(-farZ * nearZ) / (farZ - nearZ)	0

	//시야각을 이용한 투영 행렬 : 수직 시야각 HFOV, 화면 종횡비 AspectRatio, 근단면 까지의 거리 nearZ, 원단면 까지의 거리 farZ
	//	AspectRatio / tan(HFOV / 2)				0							0							0
	//				0					1 / tan(HFOV / 2)					0							0
	//				0							0					farZ / (farZ - nearZ)				1
	//				0							0				(-farZ * nearZ) / (farZ - nearZ)		0

	//	변환 수식
	//	Px = 2 * Vx * nearZ / W = Vx * AspectRatio / tan(HFOV / 2);
	//	Py = 2 * Vy * nearZ / H = Vy / tan(HFOV / 2);
	//	Pz = Vz * farZ / (farZ - nearZ) - (farZ * nearZ) / (farZ - nearZ)
	//		= (Vz - nearZ) * farZ / (farZ - nearZ);
	//
	//	Pw = Vz;

	//	역변환 수식 : 투영 행렬이 영향을 미치는 부분은 _11, _22원소 뿐이므로 이를 반영한다.
	//	Vx = Px * W / (2 * nearZ) = Px * tan(HFOV / 2) / AspectRatio;
	//	Vy = Py * H / (2 * nearZ) = Py * tan(HFOV / 2);
	//	Vz = (Pz * (farZ - nearZ) / farZ) + nearZ
	// 
	//마우스 피킹은 카메라 위치에서 뷰좌표의 근단면에서의 지점으로의 Ray를 쏘는 것에 해당하므로 Vz = nearZ

	float vx, vy, vz;
	vx = px / (*pProjM)._11;
	vy = py / (*pProjM)._22;
	vz = 1;							//vz = matProj.fNearZ; 근단면 까지의 거리가 1이 아니라면 이렇게 해주는게 맞겠지

	//뷰 좌표계에서 Ray생성 : Ray의 시점은 뷰 좌표계의 원점, 구한 좌표는 Ray의 방향 벡터에 해당한다.
	//이 때 Ray의 끝 지점 z좌표는 원단면의 거리까지로 한계를 주는 것이 바람직하다.
	//충돌 대상이 로컬 좌표계로 되어 있는 경우 월드 행렬의 역행렬까지 적용한다. 
	// (월드 행렬 * 뷰 행렬).invert()

	//Ray : 뷰 좌표계 -> 월드 좌표계 / 로컬 좌표계
	//대상 피킹 오브젝트가 월드 행렬이 적용된 경우 해당 피킹 오브젝트의 정점 버퍼는
	//좌표계 변환 행렬이 반영되기 이전의 상태이므로 월드 행렬 * 뷰 행렬의 역행렬을 곱하여
	//피킹 지점을 맞추어 준다.
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

	//Ray의 원점과 방향 모두 역행렬로 곱해 좌표계를 이동시킨다.
	//역행렬과 곱하되 위치 벡터는 w값으로 나누는 XMVector3TransformCoord함수를,
	//방향 벡터는 w값으로 나누지 않는 XMVector3TransformNormal함수를 사용한다.
	pOut->stPos = DirectX::XMVector3TransformCoord(pOut->stPos, invWorldViewM);
	pOut->direction = DirectX::XMVector3TransformNormal(pOut->direction, invWorldViewM);

	//방향벡터는 정규화 된 값이 아니므로 정규화 작업을 수행한다.
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

	//확인하고자하는 레이
	CPM_Ray ray = { vStart , vEnd - vStart, -1.0f };	//방향 정규화 안했음

	//방향 벡터를 평면의 노말과 내적해 방향벡터의 평면 법선 벡터와 같은 방향으로의 전체 길이를 구한다.
	float D = vPlaneNormal.Dot(ray.direction);

	//레이의 시점과 교점 판단을 하고자 하는 삼각형의 정점을 이은 벡터 하나와 평면 법선벡터를 내적해
	//시점에서 평면까지의 거리를 구한다.
	float a0 = vPlaneNormal.Dot(v0 - vStart);

	//레이의 시점과 교점까지의 거리를 판단하기 위해 계산한 두벡터의 길이의 비율을 구한다.
	float fRatio = a0 / D;

	//0.0f ~ 1.0f의 길이의 비를 벗어나는 경우에는 실패 처리한다.
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
	//삼각형이 다음과 같이 구성되어 있고 평면 노말은 화면 바깥쪽을 향하는 방향으로 가정한다.
	//		v0
	// e2 /    \ e0
	//   v2 ㅡ v1
	//		e1

	CPM_Vector3 e0, e1, e2, eTarget, vTargetNormal;
	e0 = v1 - v0;
	e1 = v2 - v1;
	e2 = v0 - v2;

	//삼각형을 이루는 간선과 삼각형과 교점을 이은 간선과의 외적을 통해 얻은 노말을 
	//인수로 받은 노말과 내적해 방향성을 확인한다. 내적의 결과가 양수면 삼각형 안쪽일 가능성이 있으며 음수인 경우 무조건 외부로 처리한다.

	//e0과 v0 - 교점을 이은선과의 외적 
	eTarget = vTarget - v0;
	vTargetNormal = e0.Cross(eTarget);
	vTargetNormal.Normalize();

	float fRet = vPlaneNormal.Dot(vTargetNormal);
	if (fRet < 0.0f) { return false; }

	//e1과 v1 - 교점을 이은선과의 외적
	eTarget = vTarget - v1;
	vTargetNormal = e1.Cross(eTarget);
	vTargetNormal.Normalize();

	fRet = vPlaneNormal.Dot(vTargetNormal);
	if (fRet < 0.0f) { return false; }

	////e2과 v2 - 교점을 이은선과의 외적
	eTarget = vTarget - v2;
	vTargetNormal = e2.Cross(eTarget);
	vTargetNormal.Normalize();

	fRet = vPlaneNormal.Dot(vTargetNormal);
	if (fRet < 0.0f) { return false; }

	//전부 통과 했다면 삼각형 내부의 점이므로 참을 반환한다.
	return true;
};

bool CPPicker::IntersectTriangle(CPM_Vector3& origin, CPM_Vector3& dir,
	CPM_Vector3& v0, CPM_Vector3& v1, CPM_Vector3& v2, CPM_Vector3& outTUV)
{
	//삼각형 예시
	//		v0
	// eB /    \ eA
	//   v2 ㅡ v1
	//		

	//Ray의 direction은 정규화 되어 있다고 가정한다.
	//식을 변경해 공통으로 들어가는 특정 값을 반복하여 사용한다.

	//v0을 기준으로 한다.
	CPM_Vector3 eA = v1 - v0;
	CPM_Vector3 eB = v2 - v0;

	// fDet = det| -dir eA eB |
	// pVec = origin - origin - (v0 - origin) = origin - v0
	// qVec = dir X eB
	// rVec = pVec X eA

	//t = |pVec eA eB| / |-dir eA eB| = 
	//u = |-dir pVec eB| / |-dir eA eB| = pVec*qVec / det
	//v = |-dir eA pVec| / |-dir eA eB| = 

	//연립 방정식의 계수행렬의 행렬식 계산부 : | -dir eA eB | = -dir*(eA X eB) = eA*(dir X eB)
	CPM_Vector3 qVec = dir.Cross(eB);
	FLOAT fDet = eA.Dot(qVec);
	if (fDet < 0.0001f) { return false; }	//Divide By Zero오류 방지를 위해 허용 오차의 하한보다 작은경우 행렬식이 0인 것으로 간주한다.

	//분모로 사용하는 행렬식이 음수인 경우 양수로 변환 - 공통으로 들어가는 pVec의 부호를 변경하는 것으로 대체
	CPM_Vector3 pVec;
	if (fDet > 0) { pVec = origin - v0; }
	else { pVec = v0 - origin; fDet = -fDet; }

	// u 계산
	outTUV.y = pVec.Dot(qVec);
	if (outTUV.y < 0.0f || outTUV.y > fDet) { return false; } //0.0f ~ 1.0f 범위를 벗어난다면 삼각형을 벗어난다.

	// v 계산
	CPM_Vector3 rVec = pVec.Cross(eA);
	outTUV.z = dir.Dot(rVec);
	if (outTUV.z < 0.0f || outTUV.y + outTUV.z > fDet) { return false; } // u,v 둘의 합산이 0.0f ~ 1.0f범위를 벗어난다면 삼각형을 벗어난다.

	// t 계산 : 졍규화 되어있는 방향 벡터를 사용할 것이므로 u, v만 맞다면 t로 거리를 구할 수 있다.
	outTUV.x = eB.Dot(rVec);

	// 행렬식의 역수를 곱해 줌으로써 나눗셈을 대신한다.
	FLOAT fInvDet = 1.0f / fDet;
	outTUV.x *= fInvDet;
	outTUV.y *= fInvDet;
	outTUV.z *= fInvDet;
	return true;
}