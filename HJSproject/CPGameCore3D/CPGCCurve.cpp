#include "CPGCCurve.h"

void QuaternionToEulerRad(CPM_Quaternion Q, CPM_Vector3 vOut)
{
	//��ǥ�� �࿡ ���� ȸ�� ��������� ���� �޸� �����ȴ�.
	//DX�� PYR��������� YXZ������ �����Ƿ� �̸� �ݿ��Ѵ�.
	//�� ���� 180�� �̻��� ȸ���� ���� ����ó���� ���� �ʾ����Ƿ� 
	//ȸ���� ������ �׻� �ݽð� ������ �ƴ� �� ������ ����

	//Yaw - Pitch - Roll
	float x = Q.y;
	float y = Q.x;
	float z = Q.z;
	float w = Q.w;

	float sqX = x * x;
	float sqY = y * y;
	float sqZ = z * z;
	float sqW = w * w;

	float C0 = 2.0f * (y * z + x * w);
	float C1 = -2.0f * (x * z - y * w);
	float C2 = 2.0f * (x * y + z * w);

	float C3 = -sqX - sqY + sqZ + sqW;
	float C4 = sqX - sqY - sqZ + sqW;

	vOut.x = atan2f(C0, C3);
	vOut.y = asinf(C1);
	vOut.z = atan2f(C2, C4);
}

CPM_Matrix TRANSFORM_KEY::getT()
{
	return CPM_Matrix::CreateTranslation(vPos);
}

CPM_Matrix TRANSFORM_KEY::getR()
{
	return CPM_Matrix::CreateFromQuaternion(qRot);
}

CPM_Matrix TRANSFORM_KEY::getS()
{
	return CPM_Matrix::CreateScale(vScale);
}

CPM_Matrix TRANSFORM_KEY::getWorld()
{
	return CPM_Matrix::CreateScale(vScale) * CPM_Matrix::CreateFromQuaternion(qRot) * CPM_Matrix::CreateTranslation(vPos);
}

CPGCCurve::CPGCCurve()
{
	m_loopOpt = SPLINE_LOOP_OPT::SPLOPT_NOLOOP_ONEWAY;
	m_fTension = 0.0f;
	m_fTime = 0.0f;
	m_bPaused = true;
	m_bReverse = false;
}

CPGCCurve::CPGCCurve(SPLINE_LOOP_OPT SPLOpt, TRANSFORM_KEY* keyList, int iNumCP, bool bPaused, float fTension)
{
	m_loopOpt = SPLOpt;
	m_fTension = fTension;
	m_bPaused = bPaused;
	m_bReverse = false;

	m_fTime = 0.0f;

	for (int i = 0; i < iNumCP; i++) { m_CPList.push_back(*keyList++); }

	m_curKey = m_CPList[0];
}

CPGCCurve::~CPGCCurve()
{
}

bool CPGCCurve::update(float dt)
{
	if (!m_bPaused)
	{
		m_fTime += dt;

		size_t len = m_CPList.size();

		if (len)
		{
			if (len == 1) { m_curKey = m_CPList[0]; return true; }

			if (m_fTime < 0.0f) { m_fTime = 0.0f; }
			else if (m_fTime > m_CPList[len - 1].fTime)
			{
				switch (m_loopOpt)
				{
				case SPLINE_LOOP_OPT::SPLOPT_NOLOOP_ONEWAY:
				{
					m_curKey = m_CPList[len - 1];
					pause();
				} return true;

				case SPLINE_LOOP_OPT::SPLOPT_NOLOOP_ROUNDTRIP:
				{
					if (!m_bReverse)
					{
						m_fTime = 0.0f;
						m_bReverse = true;
					}
					else
					{
						m_bReverse = false;
						pause();
						return true;
					}
				}break;

				case SPLINE_LOOP_OPT::SPLOPT_LOOP_ONEWAY:
				{
					m_fTime = 0.0f;
				}break;

				case SPLINE_LOOP_OPT::SPLOPT_LOOP_ROUNDTRIP:
				{
					if (!m_bReverse)
					{
						m_fTime = 0.0f;
						m_bReverse = true;
					}
					else
					{
						m_bReverse = false;
						m_fTime = 0.0f;
					}
				}break;
				}
			}

			if (len == 2)
			{
				if (m_bReverse)
				{
					Lerp(m_CPList[1], m_CPList[0], m_fTime);
				}
				else
				{
					Lerp(m_CPList[0], m_CPList[1], m_fTime);
				}

			}
			else { SplineInterp(m_fTime); }
		}
	}

	return true;
}

bool CPGCCurve::Lerp(TRANSFORM_KEY& p0, TRANSFORM_KEY& p1, float time)
{
	UINT p[4] = { 0 };

	CPM_Vector3			vPos;
	CPM_Quaternion		qRot;
	CPM_Vector3			vScale;

	float tSt = min(p0.fTime, p1.fTime);
	float tEd = max(p0.fTime, p1.fTime);

	float	t = (time - tSt) / (tEd - tSt);
	m_curKey.fTime = t;

	m_curKey.vPos = CPM_Vector3::Lerp(p0.vScale, p1.vScale, t);
	m_curKey.qRot = CPM_Quaternion::Slerp(p0.qRot, p1.qRot, t);
	m_curKey.vScale = CPM_Vector3::Lerp(p0.vScale, p1.vScale, t);

	return true;
}

bool CPGCCurve::SplineInterp(float time)
{
	int p[4] = { 0 };

	CPM_Vector3			vPos;
	CPM_Quaternion		qRot;
	CPM_Vector3			vScale;

	//���� ó��
	size_t len = m_CPList.size();
	if (!len) { return false; }

	//1. �ð��� �ش��ϴ� ���� ������ ������ ����Ʈ �����ϱ�
	if (!m_bReverse)
	{
		for (int i = 0; i < len; i++)
		{
			if (time < m_CPList[i].fTime)
			{
				p[2] = i;

				//���� ������ 2��
				if (p[1] == 0) { p[0] = 0; }
				else { p[0] = p[1] - 1; }

				//������ ������ 2��
				if (p[2] == len - 1) { p[3] = p[2]; }
				else { p[3] = p[2] + 1; }
				break;
			}

			p[1] = i;
		}

	}
	else
	{
		p[2] = len - 1;

		for (int i = len - 1; i >= 0; i--)
		{
			if (m_CPList[len - 1].fTime - time > m_CPList[i].fTime)
			{
				p[2] = i;

				//���� ������ 2��
				if (p[2] == 0) { p[3] = 0; }
				else { p[3] = p[2] - 1; }

				//������ ������ 2��
				if (p[1] == len - 1) { p[0] = p[1]; }
				else { p[0] = p[1] + 1; }
				break;
			}

			p[1] = i;
		}
	}

	float tSt = min(m_CPList[p[1]].fTime, m_CPList[p[2]].fTime);
	float tEd = max(m_CPList[p[1]].fTime, m_CPList[p[2]].fTime);
	float	t = 0.0f;

	if (!m_bReverse)
	{
		t = (time - tSt) / (tEd - tSt);
	}
	else
	{
		t = (tEd - m_CPList[len - 1].fTime + time) / (tEd - tSt);
	}

	m_curKey.fTime = t;

	if (m_fTension <= 0.0f || m_fTension >= 1.0f)
	{
		//�ټ��� �⺻��(0.5f)���� ����� Ĺ-�ַ� ���ö��� ��ġ ��ȯ�Լ�
		//p[1]~p[2]������ ��� ���ǵȴ�.
		CPM_Vector3::CatmullRom(m_CPList[p[0]].vPos,
			m_CPList[p[1]].vPos,
			m_CPList[p[2]].vPos,
			m_CPList[p[3]].vPos,
			t, m_curKey.vPos);

		//ȸ�� ���� �ذ��� 1 : ���࿡ ���� ȸ�� ������ ���ͷ� ����Ѵ�.
		//ȸ�� ��ȭ���� ���ö������� �����Ѵ�. ���� �ð� �������� �����ȴٸ� 
		//������ ȸ�������� �ٸ��� �ָ� �������� �ִϸ��̼� �ӵ��� �޶�����.
#ifdef PYR_INTERP
		CPM_Vector3::CatmullRom(m_CPList[p[0]].vRot,
			m_CPList[p[1]].vRot,
			m_CPList[p[2]].vRot,
			m_CPList[p[3]].vRot,
			t, m_curKey.vRot);
#else 
		//ȸ�� ���� �ذ��� 2 : ���ʹϾ��� ���� �����Ѵ�.
		//�׻� �ִܰŸ��� �����ϹǷ� �� ������ PI Rad.�̻��� ȸ������ ������
		//���� ����� �Ÿ��� �켱 ���� �ϹǷ� �̸� ������ ��
		m_curKey.qRot = DirectX::XMVectorCatmullRom(m_CPList[p[0]].qRot,
			m_CPList[p[1]].qRot,
			m_CPList[p[2]].qRot,
			m_CPList[p[3]].qRot,
			t);
#endif //PYR_INTERP
	}
	else
	{
		Catmull_RomSpline(t, m_fTension, m_CPList[p[0]].vPos,
			m_CPList[p[1]].vPos,
			m_CPList[p[2]].vPos,
			m_CPList[p[3]].vPos,
			m_curKey.vPos);

#ifdef PYR_INTERP
		Catmull_RomSpline(t, m_fTension, m_CPList[p[0]].vRot,
			m_CPList[p[1]].vRot,
			m_CPList[p[2]].vRot,
			m_CPList[p[3]].vRot,
			m_curKey.vRot);
#else
		Catmull_RomSpline(t, m_fTension, m_CPList[p[0]].qRot,
			m_CPList[p[1]].qRot,
			m_CPList[p[2]].qRot,
			m_CPList[p[3]].qRot,
			m_curKey.qRot);
#endif //PYR_INTERP
	}

#ifdef PYR_INTERP
	m_curKey.qRot = DirectX::XMQuaternionRotationRollPitchYawFromVector(m_curKey.vRot);
#elif USE_SLERP
	m_curKey.qRot = CPM_Quaternion::Slerp(m_CPList[p[1]].qRot, m_CPList[p[2]].qRot, t);
#endif //PYR_INTERP

	m_curKey.vScale = CPM_Vector3::Lerp(m_CPList[p[1]].vScale, m_CPList[p[2]].vScale, t);

	return true;
}

void CPGCCurve::start()
{
	m_bPaused = false;
}

void CPGCCurve::pause()
{
	m_bPaused = true;
}

void CPGCCurve::reset()
{
	m_fTime = 0.0f;
	m_bPaused = true;
}

CPM_Vector3 CPGCCurve::HornerRule_VPoly(CPM_Vector3* vfactorList, int iNumE, int t)
{
	CPM_Vector3 ret;

	if (vfactorList && iNumE > 0)
	{
		ret = vfactorList[0];

		//��� ����Ʈ�� ������ ������ ��������
		for (int i = 1; i < iNumE; i++) { ret = (ret * t) + vfactorList[i]; }
	}

	return ret;
}

void CPGCCurve::QuadraticCardinalSpline(float t, CPM_Vector3& p0, CPM_Vector3& p1, CPM_Vector3& p2, CPM_Vector3& pOut)
{
	//p(t) = F[0] * t^2 + F[1] * t + F[2]
	CPM_Vector3 F[4];

	F[0] = (2.0f * p0) + (-4.0f * p1) + (2.0f * p2);
	F[1] = (-3.0f * p0) + (4.0f * p1) + (-1.0f * p2);
	F[2] = (1.0f * p0); // + (0.0f * p1) + (0.0f * p2);

	//pOut = HornerRule_VPoly(F, CPGC_ARRAY_ELEM_NUM(F), t);
	pOut = F[0];
	for (int i = 1; i < 3; i++) { pOut = (pOut * t) + F[i]; }
}

void CPGCCurve::QuadraticBezierCurve(float t, CPM_Vector3& p0, CPM_Vector3& p1, CPM_Vector3& p2, CPM_Vector3& pOut)
{
	//p(t) = F[0] * t^2 + F[1] * t + F[2]
	CPM_Vector3 F[4];

	F[0] = (1.0f * p0) + (-2.0f * p1) + (1.0f * p2);
	F[1] = (-2.0f * p0) + (2.0f * p1);// + (0.0f * p2)
	F[2] = (1.0f * p0); // + (0.0f * p1) + (0.0f * p2);

	//pOut = HornerRule_VPoly(F, CPGC_ARRAY_ELEM_NUM(F), t);
	pOut = F[0];
	for (int i = 1; i < 3; i++) { pOut = (pOut * t) + F[i]; }
}

void CPGCCurve::CubicBezierCurve(float t, CPM_Vector3& p0, CPM_Vector3& p1, CPM_Vector3& p2, CPM_Vector3& p3, CPM_Vector3& pOut)
{
	//p(t) = F[0] * t^3 + F[1] * t^2 + F[2] * t + F[3]
	CPM_Vector3 F[4];

	F[0] = (-1.0f * p0) + (3.0f * p1) + (-3.0f * p2) + (1.0f * p3);
	F[1] = (3.0f * p0) + (-6.0f * p1) + (3.0f * p2);// +(0.0f * p3);
	F[2] = (-3.0f * p0) + (3.0f * p1);// +(0.0f * p2) + (0.0f * p3);
	F[3] = (1.0f * p0);// +(0.0f * p1) + (0.0f * p2) + (0.0f * p3);

	//pOut = HornerRule_VPoly(F, CPGC_ARRAY_ELEM_NUM(F), t);
	pOut = F[0];
	for (int i = 1; i < 4; i++) { pOut = (pOut * t) + F[i]; }
}

void CPGCCurve::HermiteSpline(float t, CPM_Vector3& p0, CPM_Vector3& p1, CPM_Vector3& tan1, CPM_Vector3& tan2, CPM_Vector3& pOut)
{
	//p(t) = F[0] * t^3 + F[1] * t^2 + F[2] * t + F[3]
	CPM_Vector3 F[4];

	F[0] = (2.0f * p0) + (-2.0f * p1) + (1.0f * tan1) + (1.0f * tan2);
	F[1] = (-3.0f * p0) + (3.0f * p1) + (-2.0f * tan1) + (-1.0f * tan2);
	F[2] = /*(0.0f * p0) + (0.0f * p1) + */ (1.0f * tan1); // +(0.0f * p3);
	F[3] = (1.0f * p0); // +(0.0f * p1) + (0.0f * p2) + (0.0f * p3);

	//pOut = HornerRule_VPoly(F, CPGC_ARRAY_ELEM_NUM(F), t);
	pOut = F[0];
	for (int i = 1; i < 4; i++) { pOut = (pOut * t) + F[i]; }
}

void CPGCCurve::Catmull_RomSpline(float t, float tension, CPM_Vector3& p0, CPM_Vector3& p1, CPM_Vector3& p2, CPM_Vector3& p3, CPM_Vector3& pOut)
{
	//p(t) = F[0] * t^3 + F[1] * t^2 + F[2] * t + F[3]
	CPM_Vector3 F[4];

	F[0] = (-tension * p0) + ((2.0f - tension) * p1) + ((tension - 2.0f) * p2) + (tension * p3);
	F[1] = (2.0f * tension * p0) + ((tension - 3.0f) * p1) + ((3.0f - 2 * tension) * p2) + (-tension * p3);
	F[2] = (-tension * p0) /*+(0.0f * p1)*/ + (tension * p2);// +(0.0f * p3);
	F[3] = /*(1.0f * p0);*/ (1.0f * p1); // +(0.0f * p2) + (0.0f * p3);

	//pOut = HornerRule_VPoly(F, CPGC_ARRAY_ELEM_NUM(F), t);
	pOut = F[0];
	for (int i = 1; i < 4; i++) { pOut = (pOut * t) + F[i]; }
}

void CPGCCurve::SimpleCatmull_RomSpline(float t, CPM_Vector3& p0, CPM_Vector3& p1, CPM_Vector3& p2, CPM_Vector3& p3, CPM_Vector3& pOut)
{
	//p(t) = F[0] * t^3 + F[1] * t^2 + F[2] * t + F[3]
	CPM_Vector3 F[4];

	F[0] = (-1.0f * p0) + (3.0f * p1) + (-3.0f * p2) + (1.0f * p3);
	F[1] = (2.0f * p0) + (-5.0f * p1) + (4.0f * p2) + (-1.0f * p3);
	F[2] = (-1.0f * p0) /*+(0.0f * p1)*/ + (1.0f * p2);// +(0.0f * p3);
	F[3] = /*(1.0f * p0);*/ (2.0f * p1); // +(0.0f * p2) + (0.0f * p3);

	//pOut = HornerRule_VPoly(F, CPGC_ARRAY_ELEM_NUM(F), t);
	pOut = F[0];
	for (int i = 1; i < 4; i++) { pOut = (pOut * t) + F[i]; }
}

void CPGCCurve::Kochanek_BartelsSpline(float t, CPM_Vector3 TBCFactor, CPM_Vector3& p0, CPM_Vector3& p1, CPM_Vector3& p2, CPM_Vector3& p3, CPM_Vector3& pOut)
{
	//p(t) = F[0] * t^3 + F[1] * t^2 + F[2] * t + F[3]
	CPM_Vector3 F[4];

	float Bp = (1.0f + TBCFactor.y);
	float Cp = (1.0f + TBCFactor.z);

	float Tm = (1.0f - TBCFactor.x);
	float Bm = (1.0f - TBCFactor.y);
	float Cm = (1.0f - TBCFactor.z);

	CPM_Vector3 D0, D1;
	// 0.5f * (1-t)(1+b)(1+c)(p1 - p0) + 0.5f * (1-t)(1-b)(1-c)(p2 - p1)
	D0 = 0.5f * Tm * (Bp * Cp * (p1 - p0) + Bm * Cm * (p2 - p1));

	// 0.5f * (1-t)(1+b)(1-c)(p2 - p1) + 0.5f * (1-t)(1-b)(1+c)(p3 - p2)
	D1 = 0.5f * Tm * (Bp * Cm * (p2 - p1) + Bm * Cp * (p3 - p2));

	//p(t) = C3 * t^3 + C2 * t^2 + C1 * t + C0
	CPM_Vector3 C0, C1, C2, C3;

	F[0] = (2.0f * p1) + (-2.0f * p2) + (1.0f * D0) + (1.0f * D1);
	F[1] = (-3.0f * p1) + (3.0f * p2) + (-2.0f * D0) + (-1.0f * D1);
	F[2] = /*(0.0f * p1) + (0.0f * p2) + */ (1.0f * D0); // +(0.0f * D1);
	F[3] = (1.0f * p1); // +(0.0f * p2) + (0.0f * D0) + (0.0f * D1);

	//pOut = HornerRule_VPoly(F, CPGC_ARRAY_ELEM_NUM(F), t);
	pOut = F[0];
	for (int i = 1; i < 4; i++) { pOut = (pOut * t) + F[i]; }
}
