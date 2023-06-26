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

	//위치만 사용
	TRANSFORM_KEY(CPM_Vector3 Pos, float time)
	{
		fTime = time;
		vPos = Pos;
	}

	//PYR 회전
	TRANSFORM_KEY(CPM_Vector3 Pos, CPM_Vector3 PitchYawRoll, float time, CPM_Vector3 Scale = { 1.0f, 1.0f, 1.0f })
	{
		fTime = time;

		vPos = Pos;
		vRot = { CPGC_DEG2RAD(PitchYawRoll.x), CPGC_DEG2RAD(PitchYawRoll.y), CPGC_DEG2RAD(PitchYawRoll.z) };
		qRot = DirectX::XMQuaternionRotationRollPitchYawFromVector(vRot);

		vScale = Scale;
	}

	//QT 회전
	TRANSFORM_KEY(CPM_Vector3 Pos, CPM_Quaternion Rot, float time, CPM_Vector3 Scale = { 1.0f, 1.0f, 1.0f })
	{
		fTime = time;

		vPos = Pos;
		qRot = Rot;
		vScale = Scale;
	}

	//임의의 축 회전
	TRANSFORM_KEY(CPM_Vector3 Pos, CPM_Vector3 Axis, float fRadAngle, float time, CPM_Vector3 Scale = { 1.0f, 1.0f, 1.0f })
	{
		fTime = time;

		vPos = Pos;
		qRot = CPM_Quaternion::CreateFromAxisAngle(Axis, fRadAngle);

		vRot = { CPM_Quaternion::Angle({1, 0, 0, 0}, qRot),  CPM_Quaternion::Angle({0, 1, 0, 0}, qRot), CPM_Quaternion::Angle({0, 0, 1, 0}, qRot) };
		vScale = Scale;
	}

	//Look벡터 회전
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

	//다항식을 표현하는 함수 : 다항식의 각 항의 곱을 반복문으로 처리한다.
	static CPM_Vector3 HornerRule_VPoly(CPM_Vector3* vfactorList, int iNumE, int t);

	//2차 곡선

	//세점을 무조건 지나가는 2차 카디널 스플라인의 지점을 구하는 함수
	static void QuadraticCardinalSpline(float t, CPM_Vector3& p0,
		CPM_Vector3& p1,
		CPM_Vector3& p2,
		CPM_Vector3& pOut);

	//세점의 보간으로 만드는 2차 베지어 커브의 지점을 구하는 함수
	static void QuadraticBezierCurve(float t, CPM_Vector3& p0,
		CPM_Vector3& p1,
		CPM_Vector3& p2,
		CPM_Vector3& pOut);

	//3차 곡선

	//네점의 보간으로 만드는 3차 베지어 커브의 지점을 구하는 함수
	static void CubicBezierCurve(float t, CPM_Vector3& p0,
		CPM_Vector3& p1,
		CPM_Vector3& p2,
		CPM_Vector3& p3,
		CPM_Vector3& pOut);

	//두점과 두개의 접선벡터(Tangent)로 만드는 에르미트 스플라인의 지점을 구하는 함수
	static void HermiteSpline(float t, CPM_Vector3& p0,
		CPM_Vector3& p1,
		CPM_Vector3& tan1,
		CPM_Vector3& tan2,
		CPM_Vector3& pOut);

	//4개의 점을 사용하되 p1 ~ p2에서만 곡선이 정의되고 나머지 두점 p0, p3은 접선벡터의 계산에
	//이용되는 캣멀-롬 스플라인의 지점을 구하는 함수
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

	//위 함수의 tension이 0.5인 단순한 스플라인의 지점을 구하는 함수
	static void SimpleCatmull_RomSpline(float t,
		CPM_Vector3& p0,
		CPM_Vector3& p1,
		CPM_Vector3& p2,
		CPM_Vector3& p3,
		CPM_Vector3& pOut);

	//Tension(접선 벡터의 길이의 변화), Bias(접선의 방향 벡터의 초기 변화량), 
	//Continuity(접선 벡터간 변화율의 기울기)를 조정하여 만드는 스플라인의 지점을 구하는 함수
	//구간별 동일 시간으로 만들어지는 경우만 반영
	static void Kochanek_BartelsSpline(float t, CPM_Vector3 TCBFactor,
		CPM_Vector3& p0,
		CPM_Vector3& p1,
		CPM_Vector3& p2,
		CPM_Vector3& p3,
		CPM_Vector3& pOut);
};

