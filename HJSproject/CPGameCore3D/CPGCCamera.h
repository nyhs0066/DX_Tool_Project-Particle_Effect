#pragma once
#include "CPGCObject.h"

#define CPGC_DEFAULT_FOV				CPGC_TAU / 4.0f
#define CPGC_DEFAULT_NEARPLANE_DIST		1.0f
#define CPGC_DEFAULT_FARPLANE_DIST		10000.0f

struct CPRS_PROJECTION_STATE
{
	//투영 행렬을 결정하기 위한 가시부피 / 수직 시야각 / 화면 종횡비
	float m_fWidth;
	float m_fHeight;
	float m_fNear;
	float m_fFar;
	float m_fVFOV;
	float m_fAspectRatio;
};

//기본 카메라 클래스
//직접 메시를 이용해 렌더 과정에 참여 할 수도 있다.
class CPGCCamera : public CPGCActor
{
public:
	UINT					m_iVPortWidth;
	UINT					m_iVPortHeight;

	CPM_Vector3				m_vPitchYawRoll;

	CPRS_FRUSTUM			m_frustum;
	CPRS_PROJECTION_STATE	m_ProjState;

public:
	CPGCCamera();
	virtual ~CPGCCamera();

	virtual bool init() override;
	virtual bool update() override;
	virtual bool render() override;
	virtual bool release() override;

	virtual void updateState() override;

	const CPM_Matrix* getViewMatPtr();
	const CPM_Matrix* getProjMatPtr();

	//위치, 목표 지점, 임의의 상향 벡터로 카메라 방향 조절후 뷰 행렬을 구성한다.
	void setViewMat(const CPM_Vector3& vPos,
					const CPM_Vector3& vAt,
					const CPM_Vector3& vUp);

	//목표 오브젝트의 AABB 바운딩 볼륨 크기에 적당한 간격(nearDist + halfZ)로 맞추어 보여주는 카메라 뷰 행렬 설정
	void setTargetViewMat(const CPRS_AABB& boundVol);

	//투영 행렬 구성부
	void setProjMat_Ortho(FLOAT w, FLOAT h, FLOAT nearPlane, FLOAT farPlane);					//
	void setProjMat_Persp(FLOAT w, FLOAT h, FLOAT nearPlane, FLOAT farPlane);
	void setProjMat_PerspFOV(FLOAT VFOV, FLOAT aspectRatio, FLOAT nearPlane, FLOAT farPlane);

	//화면 가로, 세로 길이 세팅
	void setScreenWH(UINT iWidth, UINT iHeight);

	//화면 크기 의존 요소 갱신
	virtual bool resizeComponent(UINT iWidth, UINT iHeight);

	CPM_Vector3 getVRight();
	CPM_Vector3 getVUp();
	CPM_Vector3 getVLook();

	CPM_Vector3 getPos();
};

//first-person View Camera
class CPGCDebugCamera : public CPGCCamera
{
public:
	CPGCDebugCamera();
	~CPGCDebugCamera();

	bool update() override;
};

//Model-View Camera
class CPGCModelViewCamera : public CPGCCamera
{
public:
	FLOAT m_fRadius;
	CPM_Vector3 m_vTargetPos;

	CPGCModelViewCamera();
	~CPGCModelViewCamera();

	bool update() override;
};