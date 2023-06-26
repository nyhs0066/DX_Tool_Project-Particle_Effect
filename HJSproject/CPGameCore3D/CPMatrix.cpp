#include "CPMatrix.h"

CPMatrix3x3::CPMatrix3x3()
{
	setIdentity();
}

CPMatrix3x3::~CPMatrix3x3()
{
}

void CPMatrix3x3::setIdentity()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++) { m[i][j] = (i == j); }
	}
}

CPMatrix3x3 CPMatrix3x3::transpose()
{
	CPMatrix3x3 ret;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			ret.m[i][j] = m[j][i];
		}
	}

	return ret;
}

FLOAT CPMatrix3x3::calculateDeterminant()
{
	//a11a22a33 + a12a23a31 + a13a21a32
	//-a13a22a31 -a11a23a32 - a12a21a33
	FLOAT ret = _11 * (_22 * _33 - _23 * _32)
		+ _12 * (_23 * _31 - _21 * _33)
		+ _13 * (_21 * _32 - _22 * _31);

	return ret;
}

CPMatrix3x3 CPMatrix3x3::cofactor()
{
	CPMatrix3x3 ret;

	ret._11 = (_22 * _33) - (_23 * _32);
	ret._12 = -((_21 * _33) - (_23 * _31));
	ret._13 = (_21 * _32) - (_22 * _31);

	ret._21 = -((_12 * _33) - (_13 * _32));
	ret._22 = (_11 * _33) - (_13 * _31);
	ret._23 = -((_11 * _32) - (_12 * _31));

	ret._31 = (_12 * _23) - (_13 * _22);
	ret._32 = -((_11 * _23) - (_13 * _21));
	ret._33 = (_11 * _22) - (_12 * _21);

	return ret;
}

CPMatrix3x3 CPMatrix3x3::inverse()
{
	FLOAT D = calculateDeterminant();

	CPMatrix3x3 ret;

	if (fabs(D) > CPM_FLOAT_ERANGE)
	{
		CPMatrix3x3 cofactorMat = cofactor();
		ret = (1.0f / D) * cofactorMat.transpose();
	}

	return ret;
}

CPMatrix3x3 CPMatrix3x3::operator+(const CPMatrix3x3& opd)
{
	CPMatrix3x3 ret;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++) { ret.m[i][j] = m[i][j] + opd.m[i][j]; }
	}

	return ret;
}

CPMatrix3x3 CPMatrix3x3::operator-(const CPMatrix3x3& opd)
{
	CPMatrix3x3 ret;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++) { ret.m[i][j] = m[i][j] - opd.m[i][j]; }
	}

	return ret;
}

CPMatrix3x3 CPMatrix3x3::operator*(FLOAT f)
{
	CPMatrix3x3 ret;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++) { ret.m[i][j] = m[i][j] * f; }
	}

	return ret;
}

CPMatrix3x3 CPMatrix3x3::operator*(const CPMatrix3x3& opd)
{
	CPMatrix3x3 ret;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			ret.m[i][j] = 0;

			for (int k = 0; k < 3; k++)
			{
				ret.m[i][j] += this->m[i][k] * opd.m[k][j];
			}

			if (fabs(ret.m[i][j]) < CPM_FLOAT_ERANGE) { ret.m[i][j] = 0.0f; }
		}
	}

	return ret;
}

CPMatrix3x3 operator*(FLOAT f, const CPMatrix3x3& opd)
{
	CPMatrix3x3 ret;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++) { ret.m[i][j] = opd.m[i][j] * f; }
	}

	return ret;
}

CPMatrix3x3 CPMatrix3x3::scale(FLOAT x, FLOAT y)
{
	CPMatrix3x3 ret;

	ret._11 = x;
	ret._22 = y;

	return ret;
}

CPMatrix3x3 CPMatrix3x3::rotation(FLOAT fRad)
{
	CPMatrix3x3 ret;

	FLOAT cth = cosf(fRad);
	FLOAT sth = sinf(fRad);

	ret._11 = cth, ret._12 = sth;
	ret._21 = -sth, ret._22 = cth;

	return ret;
}

CPMatrix3x3 CPMatrix3x3::translation(FLOAT dx, FLOAT dy)
{
	CPMatrix3x3 ret;

	ret._31 = dx, ret._32 = dy;

	return ret;
}

CPMatrix4x4::CPMatrix4x4()
{
	setIdentity();
}

CPMatrix4x4::~CPMatrix4x4()
{
}

void CPMatrix4x4::setIdentity()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) { m[i][j] = (i == j); }
	}
}

CPMatrix4x4 CPMatrix4x4::transpose()
{
	CPMatrix4x4 ret;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ret.m[i][j] = m[j][i];
		}
	}

	return ret;
}

FLOAT CPMatrix4x4::calculateDeterminant()
{
	FLOAT ret = 0.0f;

	FLOAT d[6];
	d[0] = (_33 * _44 - _34 * _43);
	d[1] = (_31 * _43 - _33 * _41);
	d[2] = (_32 * _43 - _33 * _42);
	d[3] = (_31 * _42 - _32 * _41);
	d[4] = (_32 * _44 - _34 * _42);
	d[5] = (_34 * _41 - _31 * _44);

	ret += _11 * (_22 * d[0] + _23 * (-d[4]) + _24 * d[2]);
	ret += (-_12) * (_21 * d[0] + _23 * d[5] + _24 * d[1]);
	ret += _13 * (_21 * d[4] + _22 * d[5] + _24 * d[3]);
	ret += (-_14) * (_21 * d[2] + _22 * (-d[1]) + _23 * d[3]);

	return ret;
}

CPMatrix4x4 CPMatrix4x4::cofactor()
{
	CPMatrix4x4 ret;

	FLOAT d[6];
	d[0] = (_33 * _44 - _34 * _43);
	d[1] = (_31 * _43 - _33 * _41);
	d[2] = (_32 * _43 - _33 * _42);
	d[3] = (_31 * _42 - _32 * _41);
	d[4] = (_32 * _44 - _34 * _42);
	d[5] = (_34 * _41 - _31 * _44);

	ret._11 = _22 * d[0] + _23 * (-d[4]) + _24 * d[2];
	ret._12 = -(_21 * d[0] + _23 * d[5] + _24 * d[1]);
	ret._13 = _21 * d[4] + _22 * d[5] + _24 * d[3];
	ret._14 = -(_21 * d[2] + _22 * (-d[1]) + _23 * d[3]);

	ret._21 = -(_12 * d[0] + _13 * (-d[4]) + _14 * d[2]);
	ret._22 = _11 * d[0] + _13 * d[5] + _14 * d[1];
	ret._23 = -(_11 * d[4] + _12 * d[5] + _14 * d[3]);
	ret._24 = _11 * d[2] + _12 * (-d[1]) + _13 * d[3];

	d[0] = (_13 * _24 - _14 * _23);
	d[1] = (_11 * _23 - _13 * _21);
	d[2] = (_12 * _23 - _13 * _22);
	d[3] = (_11 * _22 - _12 * _21);
	d[4] = (_12 * _24 - _14 * _22);
	d[5] = (_14 * _21 - _11 * _24);

	ret._31 = _42 * d[0] + _43 * (-d[4]) + _44 * d[2];
	ret._32 = -(_41 * d[0] + _43 * d[5] + _44 * d[1]);
	ret._33 = _41 * d[4] + _42 * d[5] + _44 * d[3];
	ret._34 = -(_41 * d[2] + _42 * (-d[1]) + _43 * d[3]);

	ret._41 = -(_32 * d[0] + _33 * (-d[4]) + _34 * d[2]);
	ret._42 = _31 * d[0] + _33 * d[5] + _34 * d[1];
	ret._43 = -(_31 * d[4] + _32 * d[5] + _34 * d[3]);
	ret._44 = _31 * d[2] + _32 * (-d[1]) + _33 * d[3];

	return ret;
}

CPMatrix4x4 CPMatrix4x4::inverse()
{
	FLOAT D = calculateDeterminant();

	CPMatrix4x4 ret;

	if (fabs(D) > CPM_FLOAT_ERANGE)
	{
		CPMatrix4x4 cofactorMat = cofactor();
		ret = (1.0f / D) * cofactorMat.transpose();
	}

	return ret;
}

CPMatrix4x4 CPMatrix4x4::operator+(const CPMatrix4x4& opd)
{
	CPMatrix4x4 ret;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) { ret.m[i][j] = m[i][j] + opd.m[i][j]; }
	}

	return ret;
}

CPMatrix4x4 CPMatrix4x4::operator-(const CPMatrix4x4& opd)
{
	CPMatrix4x4 ret;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) { ret.m[i][j] = m[i][j] - opd.m[i][j]; }
	}

	return ret;
}

CPMatrix4x4 CPMatrix4x4::operator*(FLOAT f)
{
	CPMatrix4x4 ret;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) { ret.m[i][j] = m[i][j] * f; }
	}

	return ret;
}

CPMatrix4x4 CPMatrix4x4::operator*(const CPMatrix4x4& opd)
{
	CPMatrix4x4 ret;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ret.m[i][j] = 0;

			for (int k = 0; k < 4; k++)
			{
				ret.m[i][j] += this->m[i][k] * opd.m[k][j];
			}

			if (fabs(ret.m[i][j]) < CPM_FLOAT_ERANGE) { ret.m[i][j] = 0.0f; }
		}
	}

	return ret;
}

CPMatrix4x4 CPMatrix4x4::operator*=(const CPMatrix4x4& opd)
{
	FLOAT temp[4];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			temp[j] = 0;

			for (int k = 0; k < 4; k++)
			{
				temp[j] += this->m[i][k] * opd.m[k][j];
			}

			if (fabs(temp[j]) < CPM_FLOAT_ERANGE) { temp[j] = 0.0f; }
		}

		this->m[i][0] = temp[0];
		this->m[i][1] = temp[1];
		this->m[i][2] = temp[2];
		this->m[i][3] = temp[3];
	}

	return *this;
}

CPMatrix4x4 operator*(FLOAT f, const CPMatrix4x4& opd)
{
	CPMatrix4x4 ret;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) { ret.m[i][j] = opd.m[i][j] * f; }
	}

	return ret;
}

CPMatrix4x4 CPMatrix4x4::scale(FLOAT x, FLOAT y, FLOAT z)
{
	CPMatrix4x4 ret;

	ret._11 = x;
	ret._22 = y;
	ret._33 = z;

	return ret;
}

CPMatrix4x4 CPMatrix4x4::rotationX(FLOAT fRad)
{
	CPMatrix4x4 ret;

	FLOAT cth = cosf(fRad);
	FLOAT sth = sinf(fRad);

	ret._22 = cth, ret._23 = sth;
	ret._32 = -sth, ret._33 = cth;

	return ret;
}

CPMatrix4x4 CPMatrix4x4::rotationY(FLOAT fRad)
{
	CPMatrix4x4 ret;

	FLOAT cth = cosf(fRad);
	FLOAT sth = sinf(fRad);

	ret._11 = cth, ret._13 = -sth;
	ret._31 = sth, ret._33 = cth;

	return ret;
}

CPMatrix4x4 CPMatrix4x4::rotationZ(FLOAT fRad)
{
	CPMatrix4x4 ret;

	FLOAT cth = cosf(fRad);
	FLOAT sth = sinf(fRad);

	ret._11 = cth, ret._12 = sth;
	ret._21 = -sth, ret._22 = cth;

	return ret;
}

CPMatrix4x4 CPMatrix4x4::rotationPitchYawRoll(CPVector3F vPYR)
{
	CPMatrix4x4 ret = CPMatrix4x4::rotationZ(vPYR.z) * CPMatrix4x4::rotationX(vPYR.x) * CPMatrix4x4::rotationY(vPYR.y);
	return ret;
}

CPMatrix4x4 CPMatrix4x4::axisRotation(CPVector3F vAxis, FLOAT fRad)
{
	CPMatrix4x4 ret;

	FLOAT c = cosf(fRad);
	FLOAT s = sinf(fRad);
	FLOAT t = 1.0f - c;

	ret._11 = t * vAxis.x * vAxis.x + c;
	ret._21 = t * vAxis.x * vAxis.y - (s * vAxis.z);
	ret._31 = t * vAxis.x * vAxis.z + (s * vAxis.y);

	ret._12 = t * vAxis.x * vAxis.y + (s * vAxis.z);
	ret._22 = t * vAxis.y * vAxis.y + c;
	ret._32 = t * vAxis.y * vAxis.z - (s * vAxis.x);

	ret._13 = t * vAxis.x * vAxis.z - (s * vAxis.y);
	ret._23 = t * vAxis.y * vAxis.z + (s * vAxis.x);
	ret._33 = t * vAxis.z * vAxis.z + c;

	ret._41 = ret._42 = ret._43 = 0.0f;
	ret._44 = 1.0f;

	return ret;
}

CPMatrix4x4 CPMatrix4x4::translation(FLOAT dx, FLOAT dy, FLOAT dz)
{
	CPMatrix4x4 ret;

	ret._41 = dx, ret._42 = dy, ret._43 = dz;

	return ret;
}

CPMatrix4x4 CPMatrix4x4::translation(CPVector3F vPos)
{
	CPMatrix4x4 ret;

	ret._41 = vPos.x, ret._42 = vPos.y, ret._43 = vPos.z;

	return ret;
}

CPMatrix4x4 CPMatrix4x4::createWorldMatrixPitchYawRoll(CPVector3F vS, CPVector3F vPYRRot, CPVector3F vT)
{
	CPMatrix4x4 ret = CPMatrix4x4::scale(vS.x, vS.y, vS.z) *
		CPMatrix4x4::rotationPitchYawRoll(vPYRRot) *
		CPMatrix4x4::translation(vT);
	return ret;
}

CPMatrix4x4 CPMatrix4x4::createWorldMatrixAxisRotation(CPVector3F vS, CPVector3F vAxis, FLOAT fRad, CPVector3F vT)
{
	CPMatrix4x4 ret = CPMatrix4x4::scale(vS.x, vS.y, vS.z) *
		CPMatrix4x4::axisRotation(vAxis, fRad) *
		CPMatrix4x4::translation(vT);
	return ret;
}

CPMatrix4x4 CPMatrix4x4::view_camGS(CPVector3F camPos, CPVector3F at, CPVector3F up)
{
	//ī�޶� �̵��ϴ� ����� ȸ�� �ϴ� ������ ������ �������� �����ۿ� �ش��ϹǷ� ������� ����Ѵ�.
	//�̵� �Ŀ� ȸ���ϴ� �Ϳ� �ش��ϹǷ� T.inv * R.inv ����� �ȴ�.
	CPMatrix4x4 ret;
	CPVector3F zero;	//������

	CPVector3F vFront = at - camPos;
	vFront.normalization();

	CPVector3F vUp = up - ((vFront | up) * vFront);
	vUp.normalization();

	CPVector3F vRight = vUp ^ vFront;	//������ ����� ����ȭ�� ���� �������Ͱ� ���´�.

	//�̵� ����� ������� ��������� �ƴ����� �ݴ�� �̵��ϴ� ���̹Ƿ� �������̰�
	//2D������Ʈ���� ī�޶��� ��ġ�� ���ִ� ������ ���� ��ġ�� ����� �Ͱ� �� �̵� ����� ���� �ƶ��̴�.
	//ȸ�� ����� ������� ��������̾ ��ġ ��Ŀ� �ش��ϹǷ� �̸� ����ϸ�
	//������ ���� ������ ����� ���´�.
	//������ ���� �� ���� �̵���İ� ȸ������� ���� ����� ������ �����̹Ƿ� �Ʒ��� ���� ǥ���ȴ�.
	//�ε��Ҽ��� ��� ������ ���� �ʾ����Ƿ� ��뿡 ������ �ִ�.

	ret._11 = vRight.x, ret._12 = vUp.x, ret._13 = vFront.x, ret._14 = 0.0f;
	ret._21 = vRight.y, ret._22 = vUp.y, ret._23 = vFront.y, ret._24 = 0.0f;
	ret._31 = vRight.z, ret._32 = vUp.z, ret._33 = vFront.z, ret._34 = 0.0f;
	ret._41 = -(vRight | camPos), ret._42 = -(vUp | camPos), ret._43 = -(vFront | camPos), ret._44 = 1.0f;

	return ret;
}

CPMatrix4x4 CPMatrix4x4::view_camDBCR(CPVector3F camPos, CPVector3F at, CPVector3F up)
{
	CPMatrix4x4 ret;
	CPVector3F zero;	//������

	CPVector3F vFront = at - camPos;
	vFront.normalization();

	CPVector3F vRight = up ^ vFront;
	vRight.normalization();

	if (vRight == zero) { return ret; }

	CPVector3F vUp = vRight ^ vFront;	//������ ����� ����ȭ�� ���� �������Ͱ� ���´�.

	ret._11 = vRight.x, ret._12 = vUp.x, ret._13 = vFront.x, ret._14 = 0.0f;
	ret._21 = vRight.y, ret._22 = vUp.y, ret._23 = vFront.y, ret._24 = 0.0f;
	ret._31 = vRight.z, ret._32 = vUp.z, ret._33 = vFront.z, ret._34 = 0.0f;
	ret._41 = -(vRight | camPos), ret._42 = -(vUp | camPos), ret._43 = -(vFront | camPos), ret._44 = 1.0f;

	return ret;
}

CPMatrix4x4 CPMatrix4x4::objLookAt(CPVector3F pos, CPVector3F at, CPVector3F up)
{
	//����� ������ �ְ� �ڽŸ� �̵��� ȸ���� �ϹǷ� ī�޶� �� ��İ��� ����� ���谡 �ȴ�.
	//�׷��ٸ� ȸ���� �̵��ϴ� ����� �ǹǷ� R*T�� �ȴ�.
	//��ü�� ȸ�� : ������Ʈ ȸ����� | ��ü�� �����ϰ� �� ȸ�� : ������Ʈ ȸ�� ����� ����� == ī�޶� ȸ�� ���
	//�� ��� ȸ����İ� �̵���İ��� �������� �̵���� ������ �״�� ��� ��ķ� �̵��ϴ� ������ �ȴ�.

	CPMatrix4x4 ret;

	CPVector3F vFront = at - pos;
	vFront.normalization();

	CPVector3F vUp = up - ((vFront | up) * vFront);
	vUp.normalization();

	CPVector3F vRight = vUp ^ vFront;	//������ ����� ����ȭ�� ���� �������Ͱ� ���´�.

	//�̵� ����� ������� ��������� �ƴ����� �ݴ�� �̵��ϴ� ���̹Ƿ� �������̰�
	//ȸ�� ����� ������� ��������̾ ��ġ ��Ŀ� �ش��ϹǷ� �̸� ����ϸ�
	//������ ���� ������ ����� ���´�.
	//������ ���� �� ���� �̵���İ� ȸ������� ���� ����� ������ �����̹Ƿ� �Ʒ��� ���� ǥ���ȴ�.
	//�ε��Ҽ��� ��� ������ ���� �ʾ����Ƿ� ��뿡 ������ �ִ�.

	ret._11 = vRight.x, ret._12 = vRight.y, ret._13 = vRight.z, ret._14 = 0.0f;

	ret._21 = vUp.x, ret._22 = vUp.y, ret._23 = vUp.z, ret._24 = 0.0f;

	ret._31 = vFront.x, ret._32 = vFront.y, ret._33 = vFront.z, ret._34 = 0.0f;

	ret._41 = pos.x, ret._42 = pos.y, ret._43 = pos.z, ret._44 = 1.0f;

	return ret;
}

CPMatrix4x4 CPMatrix4x4::proj_orthoViewOrigin(FLOAT w, FLOAT h, FLOAT farDist, FLOAT nearDist)
{
	CPMatrix4x4 ret;

	FLOAT l = farDist - nearDist;

	//NDC���� ����ȭ (X => -1.0f~1.0f / Y => -1.0f~1.0f / Z => 0.0f~1.0f)
	ret._11 = 2.0f / w, ret._22 = 2.0f / h, ret._33 = 1.0f / l;

	//proj(Z) = (view(Z) / length) - (nearDistance / length)
	//				 ret._33				 ret._43
	ret._43 = -(nearDist / l);

	return ret;
}

CPMatrix4x4 CPMatrix4x4::proj_orthoOffCenter(FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, FLOAT farDist, FLOAT nearDist)
{
	/*X��ǥ�� ���÷� ���.
	* proj(x) = (2 * (view(x) - ((left + right) / 2.0f))) / width
	*		  = ((2 * view(x)) / width) - ((left + right) / width)
	*					 _11						_41
	* Y�� �̿� ������ ���
	* Z�� �ٴܸ�� ���ܸ� ������ �Ÿ��� �ٲ��� �ʾ����Ƿ�(���� �����̶�) proj_orthoViewOrigin�� ����
	*/

	CPMatrix4x4 ret;

	FLOAT w = right - left;
	FLOAT h = top - bottom;
	FLOAT l = farDist - nearDist;

	//NDC���� ����ȭ (X => -1.0f~1.0f / Y => -1.0f~1.0f / Z => 0.0f~1.0f)
	ret._11 = 2.0f / w, ret._22 = 2.0f / h, ret._33 = 1.0f / l;

	//proj(Z) = (view(Z) / length) - (nearDistance / length)
	//				 ret._33				 ret._43
	ret._41 = -(left + right) / w, ret._42 = -(top + bottom) / h, ret._43 = -(nearDist / l);

	return ret;
}

CPMatrix4x4 CPMatrix4x4::proj_perspectveViewOrigin(FLOAT w, FLOAT h, FLOAT farDist, FLOAT nearDist)
{
	//���Ϳ��� ���� ����� ����� w�� ������ z������ ���ϴ� ���߿� ������ ��� ��Ҹ� w������
	//������ �־�� ���� ��������.
	//���̴����� mul�� ��� ����� �ڵ����� w������ ������ �ֹǷ� ����
	CPMatrix4x4 ret;

	FLOAT l = farDist - nearDist;

	FLOAT SX = 2.0f * nearDist / w;
	FLOAT SY = 2.0f * nearDist / h;
	FLOAT SZ = farDist / l;

	ret._11 = SX; ret._22 = SY; ret._33 = SZ;

	ret._34 = 1;	//�̰� w�� ������ z���� �������� �ϴ� �κ� ������ z���� ���� �����̹Ƿ� �ܺο��� ó���ϵ��� �ϴ� ��
	ret._43 = (-nearDist * farDist) / l;
	ret._44 = 0;	//������ ������ ��

	return ret;
}

CPMatrix4x4 CPMatrix4x4::proj_perspectveOffCenter(FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, FLOAT farDist, FLOAT nearDist)
{
	//���Ϳ��� ���� ����� ����� w�� ������ z������ ���ϴ� ���߿� ������ ��� ��Ҹ� w������
	//������ �־�� ���� ��������.
	CPMatrix4x4 ret;

	FLOAT w = right - left;
	FLOAT h = top - bottom;
	FLOAT l = farDist - nearDist;

	FLOAT SX = 2.0f * nearDist / w;
	FLOAT SY = 2.0f * nearDist / h;
	FLOAT SZ = farDist / l;

	ret._11 = SX, ret._22 = SY, ret._33 = SZ;

	ret._34 = 1;	//�̰� w�� ������ z���� �������� �ϴ� �κ� ������ z���� ���� �����̹Ƿ� �ܺο��� ó���ϵ��� �ϴ� ��

	//�̵� �������� ���� �߰���
	ret._41 = (right + left) / w;			//((right + left) / 2.0f) * (2.0f / w)
	ret._42 = (top + bottom) / h;			//((top + bottom) / 2.0f) * (2.0f / h)
	ret._43 = (-nearDist * farDist) / l;	// z�� �ش���� ����
	ret._44 = 0;

	return ret;
}

CPMatrix4x4 CPMatrix4x4::proj_perspectveFOVViewOrigin(FLOAT VFOV, FLOAT aspectRatio, FLOAT farDist, FLOAT nearDist)
{
	//���Ϳ��� ���� ����� ����� w�� ������ z������ ���ϴ� ���߿� ������ ��� ��Ҹ� w������
	//������ �־�� ���� ��������.
	//ȭ�� ��Ⱦ�� = (���� ���� ���α���) / (���� ���� ���α���)
	CPMatrix4x4 ret;

	FLOAT l = farDist - nearDist;

	//tan(VFOV * 0.5f) = (h / 2.0f) / nearDist
	//cot(VFOV * 0.5f) = (2.0f * nearDist) / h
	FLOAT SY = 1 / tanf(VFOV * 0.5f);

	//�پ�� y�����Ϸ����� ȭ�� ��Ⱦ�� �ݺ���� ���� ��ȭ���� �ش�
	FLOAT SX = SY / aspectRatio;
	FLOAT SZ = farDist / l;

	ret._11 = SX, ret._22 = SY, ret._33 = SZ;

	ret._34 = 1;

	ret._43 = (-nearDist * farDist) / l;	// z�� �ش���� ����
	ret._44 = 0;

	return ret;
}