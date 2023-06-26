#pragma once
#include "CPGCObject.h"

#define CPGC_DEFAULT_FOV				CPGC_TAU / 4.0f
#define CPGC_DEFAULT_NEARPLANE_DIST		1.0f
#define CPGC_DEFAULT_FARPLANE_DIST		10000.0f

struct CPRS_PROJECTION_STATE
{
	//���� ����� �����ϱ� ���� ���ú��� / ���� �þ߰� / ȭ�� ��Ⱦ��
	float m_fWidth;
	float m_fHeight;
	float m_fNear;
	float m_fFar;
	float m_fVFOV;
	float m_fAspectRatio;
};

//�⺻ ī�޶� Ŭ����
//���� �޽ø� �̿��� ���� ������ ���� �� ���� �ִ�.
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

	//��ġ, ��ǥ ����, ������ ���� ���ͷ� ī�޶� ���� ������ �� ����� �����Ѵ�.
	void setViewMat(const CPM_Vector3& vPos,
					const CPM_Vector3& vAt,
					const CPM_Vector3& vUp);

	//��ǥ ������Ʈ�� AABB �ٿ�� ���� ũ�⿡ ������ ����(nearDist + halfZ)�� ���߾� �����ִ� ī�޶� �� ��� ����
	void setTargetViewMat(const CPRS_AABB& boundVol);

	//���� ��� ������
	void setProjMat_Ortho(FLOAT w, FLOAT h, FLOAT nearPlane, FLOAT farPlane);					//
	void setProjMat_Persp(FLOAT w, FLOAT h, FLOAT nearPlane, FLOAT farPlane);
	void setProjMat_PerspFOV(FLOAT VFOV, FLOAT aspectRatio, FLOAT nearPlane, FLOAT farPlane);

	//ȭ�� ����, ���� ���� ����
	void setScreenWH(UINT iWidth, UINT iHeight);

	//ȭ�� ũ�� ���� ��� ����
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