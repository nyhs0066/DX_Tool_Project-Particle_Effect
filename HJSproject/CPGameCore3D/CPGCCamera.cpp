#include "CPGCCamera.h"

CPGCCamera::CPGCCamera()
{
	ZeroMemory(&m_ProjState, sizeof(CPRS_PROJECTION_STATE));
}

CPGCCamera::~CPGCCamera()
{

}

bool CPGCCamera::init()
{
	//카메라 최초 생성시 원점에서 Z축 방향으로 -DEFAULT_DISTANCE만큼 떨어져 원점을 바라보며 y축이 상향벡터인 뷰행렬과
	//90도 각도를 시야각(Field Of View)로 가지며 초기 설정된 뷰포트의 종횡비에 맞춰진 투영행렬을 구성한다.

	CPM_Vector3 initAt = { 0.0f, 0.0f, 1.0f };
	CPM_Vector3 initUp = { 0.0f, 1.0f, 0.0f };

	setViewMat(m_vPos, initAt, initUp);
	setProjMat_PerspFOV(CPGC_DEFAULT_FOV, 
						m_iVPortWidth / (FLOAT)m_iVPortHeight,
						CPGC_DEFAULT_NEARPLANE_DIST,
						CPGC_DEFAULT_FARPLANE_DIST);
	return true;
}

bool CPGCCamera::update()
{
	return true;
}

bool CPGCCamera::render()
{
	return true;
}

bool CPGCCamera::release()
{
	CPGCActor::release();

	return true;
}

void CPGCCamera::updateState()
{
	//회전을 정규 기저 벡터의 회전값의 벡터로 표현하기에는 연산이 불필요하게 많다.
	//임의의 축에 대한 회전을 수행 또는 다른 회전 행렬의 복합적인 곱셈 이후에 
	//적용된 최종행렬에서 추출하는 것이 일반적이다.
	//이 함수는 복잡한 스케일 변화를 반영하지는 않는다.

	// 월드 행렬 R*T
	// Right.x	Right.y	Right.z	0
	// Up.x		Up.y	Up.z	0
	// Look.x	Look.y	Look.z	0
	// Pos.x	Pos.y	Pos.z	1

	//전치 이전의 행렬에서 추출한다.
	m_vPrevPos = m_vPos;
	m_vPos = { m_matWorld._41, m_matWorld._42, m_matWorld._43 };

	//obj Right-Axis
	m_sightAxis.vRight = { m_matView._11, m_matView._21, m_matView._31 };
	m_sightAxis.vRight.Normalize();

	//obj Up-Axis
	m_sightAxis.vUp = { m_matView._12, m_matView._22, m_matView._32 };
	m_sightAxis.vUp.Normalize();

	//obj At-Axis
	m_sightAxis.vLook = { m_matView._13, m_matView._23 , m_matView._33 };
	m_sightAxis.vLook.Normalize();

	m_frustum.create(m_matView, m_matProj);
}

const CPM_Matrix* CPGCCamera::getViewMatPtr()
{
	return &m_matView;
}

const CPM_Matrix* CPGCCamera::getProjMatPtr()
{
	return &m_matProj;
}

void CPGCCamera::setViewMat(const CPM_Vector3& vPos, const CPM_Vector3& vAt, const CPM_Vector3& vUp)
{
	m_matWorld._41 = vPos.x;
	m_matWorld._42 = vPos.y;
	m_matWorld._43 = vPos.z;

	m_matView = DirectX::XMMatrixLookAtLH(vPos, vAt, vUp);

	updateState();
}

void CPGCCamera::setTargetViewMat(const CPRS_AABB& boundVol)
{
	//보이는 방향의 
	FLOAT halfZ = boundVol.m_vCPos.z - boundVol.m_vMin.z;
	FLOAT halfH = boundVol.m_vMax.y - boundVol.m_vMin.y * 0.5f;

	//근단면 까지의 거리 : 현재 설정된 시야각을 기준으로 구한다.
	FLOAT nearDist = halfH / tanf(m_ProjState.m_fVFOV / 2.0f);

	CPM_Vector3 vNewPos = boundVol.m_vCPos - (m_sightAxis.vLook * (nearDist + halfZ));

	m_matView = DirectX::XMMatrixLookAtLH(vNewPos, m_sightAxis.vLook, m_sightAxis.vUp);
}

void CPGCCamera::setProjMat_Ortho(FLOAT w, FLOAT h, FLOAT nearPlane, FLOAT farPlane)
{
	m_ProjState.m_fWidth = w;
	m_ProjState.m_fHeight = h;
	m_ProjState.m_fNear = nearPlane;
	m_ProjState.m_fFar = farPlane;
	m_ProjState.m_fVFOV = FLT_MAX;
	m_ProjState.m_fAspectRatio = w / h;

	m_matProj = DirectX::XMMatrixOrthographicLH(w, h, nearPlane, farPlane);
}

void CPGCCamera::setProjMat_Persp(FLOAT w, FLOAT h, FLOAT nearPlane, FLOAT farPlane)
{
	m_ProjState.m_fWidth = w;
	m_ProjState.m_fHeight = h;
	m_ProjState.m_fNear = nearPlane;
	m_ProjState.m_fFar = farPlane;
	m_ProjState.m_fVFOV = atanf((h * 0.5f) / nearPlane);
	m_ProjState.m_fAspectRatio = w / h;

	m_matProj = DirectX::XMMatrixPerspectiveLH(w, h, nearPlane, farPlane);
}

void CPGCCamera::setProjMat_PerspFOV(FLOAT VFOV, FLOAT aspectRatio, FLOAT nearPlane, FLOAT farPlane)
{
	m_ProjState.m_fHeight = tan(VFOV * 0.5f) * nearPlane;
	m_ProjState.m_fWidth = aspectRatio * m_ProjState.m_fHeight;
	m_ProjState.m_fNear = nearPlane;
	m_ProjState.m_fFar = farPlane;
	m_ProjState.m_fVFOV = VFOV;
	m_ProjState.m_fAspectRatio = aspectRatio;

	m_matProj = DirectX::XMMatrixPerspectiveFovLH(VFOV, aspectRatio, nearPlane, farPlane);
}

void CPGCCamera::setScreenWH(UINT iWidth, UINT iHeight)
{
	m_iVPortWidth = iWidth;
	m_iVPortHeight = iHeight;
}

bool CPGCCamera::resizeComponent(UINT iWidth, UINT iHeight)
{
	//1. 화면 너비 / 높이 변수 갱신
	setScreenWH(iWidth, iHeight);

	//2. 투영 행렬 갱신 - FOV를 사용한다.
	float aspectRatio = iWidth / (FLOAT)iHeight;
	setProjMat_PerspFOV(m_ProjState.m_fVFOV, aspectRatio, m_ProjState.m_fNear, m_ProjState.m_fFar);

	//3. 속성값 갱신
	updateState();

	return true;
}

CPM_Vector3 CPGCCamera::getVRight()
{
	return m_sightAxis.vRight;
}

CPM_Vector3 CPGCCamera::getVUp()
{
	return m_sightAxis.vUp;
}

CPM_Vector3 CPGCCamera::getVLook()
{
	return m_sightAxis.vLook;
}

CPM_Vector3 CPGCCamera::getPos()
{
	return m_vPos;
}

CPGCDebugCamera::CPGCDebugCamera()
{
	m_physicsState.fSpeed = 1.0f;
	m_physicsState.fAngularSpeed = CPGC_TAU * 0.4f;
}

CPGCDebugCamera::~CPGCDebugCamera()
{
}

bool CPGCDebugCamera::update()
{
	FLOAT dt = CPGC_MAINTIMER.getOneFrameTimeF();
	POINT offset = { 0, 0 };

	if (CPGC_MAININPUT.getKeyState(VK_LSHIFT))
	{
		m_physicsState.fSpeed = min(50.0f, m_physicsState.fSpeed + dt * 2.5f);
	}
	else
	{
		m_physicsState.fSpeed = max(1.0f, m_physicsState.fSpeed - (m_physicsState.fSpeed)*dt);
	}

	if (CPGC_MAININPUT.getKeyState('W'))
	{
		m_vPos += m_sightAxis.vLook * m_physicsState.fSpeed * dt;
	}

	if (CPGC_MAININPUT.getKeyState('S'))
	{
		m_vPos -= m_sightAxis.vLook * m_physicsState.fSpeed * dt;
	}

	if (CPGC_MAININPUT.getKeyState('A'))
	{
		m_vPos -= m_sightAxis.vRight * m_physicsState.fSpeed * dt;
	}

	if (CPGC_MAININPUT.getKeyState('D'))
	{
		m_vPos += m_sightAxis.vRight * m_physicsState.fSpeed * dt;
	}

	if (CPGC_MAININPUT.getKeyState('Q'))
	{
		m_vPos += m_sightAxis.vUp * m_physicsState.fSpeed * dt;
	}

	if (CPGC_MAININPUT.getKeyState('E'))
	{
		m_vPos -= m_sightAxis.vUp * m_physicsState.fSpeed * dt;
	}

	if (CPGC_MAININPUT.getKeyState(VK_LBUTTON))
	{
		offset = CPGC_MAININPUT.getMouseOffset();
	}

	//마우스 휠에 따른 추가 거리 보정
	CPGC_MOUSE_WHEEL_STATE wheelState = CPGC_MAININPUT.getMouseWheelState();

	if (wheelState.bMoveWheel)
	{
		m_vPos += m_sightAxis.vLook * wheelState.iWheelDt * 0.01f;
	}

	//마우스 드래그로 조종
	FLOAT rotRightAxis = offset.y * m_physicsState.fAngularSpeed * dt;
	FLOAT rotUpAxis = offset.x * m_physicsState.fAngularSpeed * dt;

	CPM_Matrix translation = CPM_Matrix::CreateTranslation(m_vPos);
	m_matWorld = translation;

	m_vPitchYawRoll.x += -rotRightAxis;
	m_vPitchYawRoll.y += -rotUpAxis;

	CPM_Quaternion qRot = CPM_Quaternion::CreateFromYawPitchRoll(m_vPitchYawRoll);
	CPM_Matrix rotation = CPM_Matrix::CreateFromQuaternion(qRot);

	m_matView = (rotation * translation).Invert();

	updateState();

	return true;
}

CPGCModelViewCamera::CPGCModelViewCamera()
{
	m_fRadius = 2.0f;
	m_physicsState.fAngularSpeed = CPGC_TAU * 0.4f;
}

CPGCModelViewCamera::~CPGCModelViewCamera()
{
}

bool CPGCModelViewCamera::update()
{
	FLOAT dt = CPGC_MAINTIMER.getOneFrameTimeF();
	POINT offset = { 0, 0 };

	if (CPGC_MAININPUT.getKeyState(VK_LBUTTON))
	{
		offset = CPGC_MAININPUT.getMouseOffset();
	}

	//마우스 드래그로 조종
	m_vPitchYawRoll.x += -(offset.y * m_physicsState.fAngularSpeed * dt);
	m_vPitchYawRoll.y += -(offset.x * m_physicsState.fAngularSpeed * dt);

	CPM_Vector3 vWorldLookAt = { 0.0f, 0.0f, 1.0f };
	CPM_Vector3 vWorldUp = { 0.0f, 1.0f, 0.0f };

	CPM_Matrix PYRmat = CPM_Matrix::CreateFromYawPitchRoll(m_vPitchYawRoll);

	CPM_Vector3 newLookAt = CPM_Vector3::Transform(vWorldLookAt, PYRmat);
	CPM_Vector3 newUp = CPM_Vector3::Transform(vWorldUp, PYRmat);
	CPM_Vector3 newPos = m_vTargetPos - (newLookAt * m_fRadius);

	setViewMat(newPos, newLookAt, newUp);

	updateState();

	return true;
}


