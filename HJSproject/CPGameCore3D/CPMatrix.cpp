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
	//카메라가 이동하는 방향과 회전 하는 방향은 월드의 움직임의 역동작에 해당하므로 역행렬을 사용한다.
	//이동 후에 회전하는 것에 해당하므로 T.inv * R.inv 행렬이 된다.
	CPMatrix4x4 ret;
	CPVector3F zero;	//영벡터

	CPVector3F vFront = at - camPos;
	vFront.normalization();

	CPVector3F vUp = up - ((vFront | up) * vFront);
	vUp.normalization();

	CPVector3F vRight = vUp ^ vFront;	//외적의 결과는 정규화된 단위 법선벡터가 나온다.

	//이동 행렬의 역행렬은 직교행렬이 아니지만 반대로 이동하는 것이므로 직관적이고
	//2D프로젝트에서 카메라의 위치를 빼주는 것으로 보일 위치를 계산한 것과 뷰 이동 행렬은 같은 맥락이다.
	//회전 행렬의 역행렬은 직교행렬이어서 전치 행렬에 해당하므로 미리 계산하면
	//다음의 고정 형태의 행렬이 나온다.
	//마지막 행의 세 값은 이동행렬과 회전행렬을 곱한 결과가 내적의 형태이므로 아래와 같이 표현된다.
	//부동소수점 계산 오차를 잡지 않았으므로 사용에 위험이 있다.

	ret._11 = vRight.x, ret._12 = vUp.x, ret._13 = vFront.x, ret._14 = 0.0f;
	ret._21 = vRight.y, ret._22 = vUp.y, ret._23 = vFront.y, ret._24 = 0.0f;
	ret._31 = vRight.z, ret._32 = vUp.z, ret._33 = vFront.z, ret._34 = 0.0f;
	ret._41 = -(vRight | camPos), ret._42 = -(vUp | camPos), ret._43 = -(vFront | camPos), ret._44 = 1.0f;

	return ret;
}

CPMatrix4x4 CPMatrix4x4::view_camDBCR(CPVector3F camPos, CPVector3F at, CPVector3F up)
{
	CPMatrix4x4 ret;
	CPVector3F zero;	//영벡터

	CPVector3F vFront = at - camPos;
	vFront.normalization();

	CPVector3F vRight = up ^ vFront;
	vRight.normalization();

	if (vRight == zero) { return ret; }

	CPVector3F vUp = vRight ^ vFront;	//외적의 결과는 정규화된 단위 법선벡터가 나온다.

	ret._11 = vRight.x, ret._12 = vUp.x, ret._13 = vFront.x, ret._14 = 0.0f;
	ret._21 = vRight.y, ret._22 = vUp.y, ret._23 = vFront.y, ret._24 = 0.0f;
	ret._31 = vRight.z, ret._32 = vUp.z, ret._33 = vFront.z, ret._34 = 0.0f;
	ret._41 = -(vRight | camPos), ret._42 = -(vUp | camPos), ret._43 = -(vFront | camPos), ret._44 = 1.0f;

	return ret;
}

CPMatrix4x4 CPMatrix4x4::objLookAt(CPVector3F pos, CPVector3F at, CPVector3F up)
{
	//월드는 가만히 있고 자신만 이동과 회전을 하므로 카메라 뷰 행렬과는 역행렬 관계가 된다.
	//그렇다면 회전후 이동하는 행렬이 되므로 R*T가 된다.
	//객체만 회전 : 오브젝트 회전행렬 | 객체만 제외하고 다 회전 : 오브젝트 회전 행렬의 역행렬 == 카메라 회전 행렬
	//이 경우 회전행렬과 이동행렬과의 곱셈에서 이동행렬 성분이 그대로 결과 행렬로 이동하는 모양새가 된다.

	CPMatrix4x4 ret;

	CPVector3F vFront = at - pos;
	vFront.normalization();

	CPVector3F vUp = up - ((vFront | up) * vFront);
	vUp.normalization();

	CPVector3F vRight = vUp ^ vFront;	//외적의 결과는 정규화된 단위 법선벡터가 나온다.

	//이동 행렬의 역행렬은 직교행렬이 아니지만 반대로 이동하는 것이므로 직관적이고
	//회전 행렬의 역행렬은 직교행렬이어서 전치 행렬에 해당하므로 미리 계산하면
	//다음의 고정 형태의 행렬이 나온다.
	//마지막 행의 세 값은 이동행렬과 회전행렬을 곱한 결과가 내적의 형태이므로 아래와 같이 표현된다.
	//부동소수점 계산 오차를 잡지 않았으므로 사용에 위험이 있다.

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

	//NDC로의 정규화 (X => -1.0f~1.0f / Y => -1.0f~1.0f / Z => 0.0f~1.0f)
	ret._11 = 2.0f / w, ret._22 = 2.0f / h, ret._33 = 1.0f / l;

	//proj(Z) = (view(Z) / length) - (nearDistance / length)
	//				 ret._33				 ret._43
	ret._43 = -(nearDist / l);

	return ret;
}

CPMatrix4x4 CPMatrix4x4::proj_orthoOffCenter(FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, FLOAT farDist, FLOAT nearDist)
{
	/*X좌표를 예시로 든다.
	* proj(x) = (2 * (view(x) - ((left + right) / 2.0f))) / width
	*		  = ((2 * view(x)) / width) - ((left + right) / width)
	*					 _11						_41
	* Y도 이와 동일한 방식
	* Z는 근단면과 원단면 사이의 거리가 바뀌지 않았으므로(직교 투영이라서) proj_orthoViewOrigin과 동일
	*/

	CPMatrix4x4 ret;

	FLOAT w = right - left;
	FLOAT h = top - bottom;
	FLOAT l = farDist - nearDist;

	//NDC로의 정규화 (X => -1.0f~1.0f / Y => -1.0f~1.0f / Z => 0.0f~1.0f)
	ret._11 = 2.0f / w, ret._22 = 2.0f / h, ret._33 = 1.0f / l;

	//proj(Z) = (view(Z) / length) - (nearDistance / length)
	//				 ret._33				 ret._43
	ret._41 = -(left + right) / w, ret._42 = -(top + bottom) / h, ret._43 = -(nearDist / l);

	return ret;
}

CPMatrix4x4 CPMatrix4x4::proj_perspectveViewOrigin(FLOAT w, FLOAT h, FLOAT farDist, FLOAT nearDist)
{
	//벡터와의 곱셈 계산의 결과로 w가 정점의 z값으로 변하니 나중에 벡터의 모든 요소를 w값으로
	//나누어 주어야 함을 잊지말자.
	//쉐이더에서 mul로 행렬 연산시 자동으로 w값으로 나누어 주므로 참조
	CPMatrix4x4 ret;

	FLOAT l = farDist - nearDist;

	FLOAT SX = 2.0f * nearDist / w;
	FLOAT SY = 2.0f * nearDist / h;
	FLOAT SZ = farDist / l;

	ret._11 = SX; ret._22 = SY; ret._33 = SZ;

	ret._34 = 1;	//이게 w가 정점의 z값을 가지도록 하는 부분 정점의 z값은 정점 종속이므로 외부에서 처리하도록 하는 것
	ret._43 = (-nearDist * farDist) / l;
	ret._44 = 0;	//각별히 유의할 것

	return ret;
}

CPMatrix4x4 CPMatrix4x4::proj_perspectveOffCenter(FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, FLOAT farDist, FLOAT nearDist)
{
	//벡터와의 곱셈 계산의 결과로 w가 정점의 z값으로 변하니 나중에 벡터의 모든 요소를 w값으로
	//나누어 주어야 함을 잊지말자.
	CPMatrix4x4 ret;

	FLOAT w = right - left;
	FLOAT h = top - bottom;
	FLOAT l = farDist - nearDist;

	FLOAT SX = 2.0f * nearDist / w;
	FLOAT SY = 2.0f * nearDist / h;
	FLOAT SZ = farDist / l;

	ret._11 = SX, ret._22 = SY, ret._33 = SZ;

	ret._34 = 1;	//이게 w가 정점의 z값을 가지도록 하는 부분 정점의 z값은 정점 종속이므로 외부에서 처리하도록 하는 것

	//이동 연산으로 인한 추가부
	ret._41 = (right + left) / w;			//((right + left) / 2.0f) * (2.0f / w)
	ret._42 = (top + bottom) / h;			//((top + bottom) / 2.0f) * (2.0f / h)
	ret._43 = (-nearDist * farDist) / l;	// z는 해당사항 없음
	ret._44 = 0;

	return ret;
}

CPMatrix4x4 CPMatrix4x4::proj_perspectveFOVViewOrigin(FLOAT VFOV, FLOAT aspectRatio, FLOAT farDist, FLOAT nearDist)
{
	//벡터와의 곱셈 계산의 결과로 w가 정점의 z값으로 변하니 나중에 벡터의 모든 요소를 w값으로
	//나누어 주어야 함을 잊지말자.
	//화면 종횡비 = (가시 부피 가로길이) / (가시 부피 세로길이)
	CPMatrix4x4 ret;

	FLOAT l = farDist - nearDist;

	//tan(VFOV * 0.5f) = (h / 2.0f) / nearDist
	//cot(VFOV * 0.5f) = (2.0f * nearDist) / h
	FLOAT SY = 1 / tanf(VFOV * 0.5f);

	//줄어든 y스케일량에서 화면 종횡비에 반비례한 값이 변화량에 해당
	FLOAT SX = SY / aspectRatio;
	FLOAT SZ = farDist / l;

	ret._11 = SX, ret._22 = SY, ret._33 = SZ;

	ret._34 = 1;

	ret._43 = (-nearDist * farDist) / l;	// z는 해당사항 없음
	ret._44 = 0;

	return ret;
}