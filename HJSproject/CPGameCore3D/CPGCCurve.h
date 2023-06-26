#pragma once
#include "CPGCStd.h"

#define PYR_INTERP
//#define USE_SLERP

void QuaternionToEulerRad(CPM_Quaternion Q, CPM_Vector3 vOut);

enum class SPLINE_LOOP_OPT
{
	SPLOPT_NOLOOP_ONEWAY,
	SPLOPT_NOLOOP_ROUNDTRIP,
	SPLOPT_LOOP_ONEWAY,
	SPLOPT_LOOP_ROUNDTRIP,
	NUMBER_OF_SPLINE_LOOP_OPT
};

struct TRANSFORM_KEY
{
	CPM_Vector3			vPos;
	CPM_Vector3			vRot;
	CPM_Quaternion		qRot;
	CPM_Vector3			vScale;

	float				fTime;

	TRANSFORM_KEY() { fTime = -1.0f; }

	//��ġ�� ���
	TRANSFORM_KEY(CPM_Vector3 Pos, float time)
	{
		fTime = time;
		vPos = Pos;
	}

	//PYR ȸ��
	TRANSFORM_KEY(CPM_Vector3 Pos, CPM_Vector3 PitchYawRoll, float time, CPM_Vector3 Scale = { 1.0f, 1.0f, 1.0f })
	{
		fTime = time;

		vPos = Pos;
		vRot = { CPGC_DEG2RAD(PitchYawRoll.x), CPGC_DEG2RAD(PitchYawRoll.y), CPGC_DEG2RAD(PitchYawRoll.z) };
		qRot = DirectX::XMQuaternionRotationRollPitchYawFromVector(vRot);

		vScale = Scale;
	}

	//QT ȸ��
	TRANSFORM_KEY(CPM_Vector3 Pos, CPM_Quaternion Rot, float time, CPM_Vector3 Scale = { 1.0f, 1.0f, 1.0f })
	{
		fTime = time;

		vPos = Pos;
		qRot = Rot;
		vScale = Scale;
	}

	//������ �� ȸ��
	TRANSFORM_KEY(CPM_Vector3 Pos, CPM_Vector3 Axis, float fRadAngle, float time, CPM_Vector3 Scale = { 1.0f, 1.0f, 1.0f })
	{
		fTime = time;

		vPos = Pos;
		qRot = CPM_Quaternion::CreateFromAxisAngle(Axis, fRadAngle);

		vRot = { CPM_Quaternion::Angle({1, 0, 0, 0}, qRot),  CPM_Quaternion::Angle({0, 1, 0, 0}, qRot), CPM_Quaternion::Angle({0, 0, 1, 0}, qRot) };
		vScale = Scale;
	}

	//Look���� ȸ��
	TRANSFORM_KEY(CPM_Vector3 Pos, CPM_Vector3 from, CPM_Vector3 to, float time, CPM_Vector3 Scale = { 1.0f, 1.0f, 1.0f })
	{
		fTime = time;

		vPos = Pos;
		qRot = CPM_Quaternion::FromToRotation(from, to);

		vRot = { CPM_Quaternion::Angle({1, 0, 0, 0}, qRot),  CPM_Quaternion::Angle({0, 1, 0, 0}, qRot), CPM_Quaternion::Angle({0, 0, 1, 0}, qRot) };
		vScale = Scale;
	}

	CPM_Matrix getT();
	CPM_Matrix getR();
	CPM_Matrix getS();

	CPM_Matrix getWorld();
};

class CPGCCurve
{
public:
	std::wstring					m_wszName;

	bool							m_bPaused;
	bool							m_bReverse;
	float							m_fTime;

	SPLINE_LOOP_OPT					m_loopOpt;
	float							m_fTension;

	std::vector<TRANSFORM_KEY>		m_CPList;
	TRANSFORM_KEY					m_curKey;

	CPGCCurve();
	CPGCCurve(SPLINE_LOOP_OPT SPLOpt, TRANSFORM_KEY* keyList, int iNumCP, bool bPaused = true, float fTension = -1.0f);
	~CPGCCurve();

	bool update(float dt);
	bool Lerp(TRANSFORM_KEY& p0, TRANSFORM_KEY& p1, float time);
	bool SplineInterp(float time);

	void start();
	void pause();
	void reset();

public:

	//���׽��� ǥ���ϴ� �Լ� : ���׽��� �� ���� ���� �ݺ������� ó���Ѵ�.
	static CPM_Vector3 HornerRule_VPoly(CPM_Vector3* vfactorList, int iNumE, int t);

	//2�� �

	//������ ������ �������� 2�� ī��� ���ö����� ������ ���ϴ� �Լ�
	static void QuadraticCardinalSpline(float t, CPM_Vector3& p0,
		CPM_Vector3& p1,
		CPM_Vector3& p2,
		CPM_Vector3& pOut);

	//������ �������� ����� 2�� ������ Ŀ���� ������ ���ϴ� �Լ�
	static void QuadraticBezierCurve(float t, CPM_Vector3& p0,
		CPM_Vector3& p1,
		CPM_Vector3& p2,
		CPM_Vector3& pOut);

	//3�� �

	//������ �������� ����� 3�� ������ Ŀ���� ������ ���ϴ� �Լ�
	static void CubicBezierCurve(float t, CPM_Vector3& p0,
		CPM_Vector3& p1,
		CPM_Vector3& p2,
		CPM_Vector3& p3,
		CPM_Vector3& pOut);

	//������ �ΰ��� ��������(Tangent)�� ����� ������Ʈ ���ö����� ������ ���ϴ� �Լ�
	static void HermiteSpline(float t, CPM_Vector3& p0,
		CPM_Vector3& p1,
		CPM_Vector3& tan1,
		CPM_Vector3& tan2,
		CPM_Vector3& pOut);

	//4���� ���� ����ϵ� p1 ~ p2������ ��� ���ǵǰ� ������ ���� p0, p3�� ���������� ��꿡
	//�̿�Ǵ� Ĺ��-�� ���ö����� ������ ���ϴ� �Լ�
	static void Catmull_RomSpline(float t, float tension,
		CPM_Vector3& p0, 
		CPM_Vector3& p1, 
		CPM_Vector3& p2, 
		CPM_Vector3& p3, 
		CPM_Vector3& pOut);

	static void Catmull_RomSpline(float t, float tension,
		CPM_Quaternion& p0, 
		CPM_Quaternion& p1,
		CPM_Quaternion& p2, 
		CPM_Quaternion& p3, 
		CPM_Quaternion& pOut);

	//�� �Լ��� tension�� 0.5�� �ܼ��� ���ö����� ������ ���ϴ� �Լ�
	static void SimpleCatmull_RomSpline(float t,
		CPM_Vector3& p0,
		CPM_Vector3& p1,
		CPM_Vector3& p2,
		CPM_Vector3& p3,
		CPM_Vector3& pOut);

	//Tension(���� ������ ������ ��ȭ), Bias(������ ���� ������ �ʱ� ��ȭ��), 
	//Continuity(���� ���Ͱ� ��ȭ���� ����)�� �����Ͽ� ����� ���ö����� ������ ���ϴ� �Լ�
	//������ ���� �ð����� ��������� ��츸 �ݿ�
	static void Kochanek_BartelsSpline(float t, CPM_Vector3 TCBFactor,
		CPM_Vector3& p0,
		CPM_Vector3& p1,
		CPM_Vector3& p2,
		CPM_Vector3& p3,
		CPM_Vector3& pOut);
};

