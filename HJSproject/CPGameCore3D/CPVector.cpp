#include "CPVector.h"

CPVector2F::CPVector2F() : CPVector2F(0.0f, 0.0f) {}

CPVector2F::CPVector2F(float x, float y) { set(x, y); }

CPVector2F::CPVector2F(const CPVector2F& v) { set(v.x, v.y); }

CPVector2F CPVector2F::operator+(const CPVector2F& opd)	const
{
	return CPVector2F(x + opd.x, y + opd.y);
}

CPVector2F CPVector2F::operator+=(const CPVector2F& opd)
{
	x += opd.x;
	y += opd.y;
	return *this;
}

CPVector2F CPVector2F::operator-(const CPVector2F& opd)	const
{
	return CPVector2F(x - opd.x, y - opd.y);
}

CPVector2F CPVector2F::operator-=(const CPVector2F& opd)
{
	x -= opd.x;
	y -= opd.y;
	return *this;
}

bool CPVector2F::operator==(const CPVector2F& opd)	const
{
	if (fabs(x - opd.x) < CPM_FLOAT_ERANGE)
	{
		if (fabs(y - opd.y) < CPM_FLOAT_ERANGE) { return true; }
	}
	return false;
}

bool CPVector2F::operator!=(const CPVector2F& opd) const { return !(*this == opd); }

CPVector2F CPVector2F::operator*(const CPMatrix3x3& opd)	const
{
	CPVector2F ret;

	ret.x = x * opd._11 + y * opd._21 + 1.0f * opd._31;
	ret.y = x * opd._12 + y * opd._22 + 1.0f * opd._32;

	return ret;
}

CPVector2F CPVector2F::operator*(float scalar)	const
{
	return CPVector2F(x * scalar, y * scalar);
}

CPVector2F& CPVector2F::operator=(const CPVector2F& opd)
{
	x = opd.x;
	y = opd.y;

	return *this;
}

CPVector2F CPVector2F::operator-()
{
	return CPVector2F(-x, -y);
}

void CPVector2F::set(float x, float y)
{
	this->x = x;
	this->y = y;
}

float CPVector2F::length() const { return sqrtf(lengthSquare()); }

float CPVector2F::lengthSquare() const { return x * x + y * y; }

CPVector2F CPVector2F::getUnit() const
{
	float len = 1.0f / length();

	float tx = x * len;
	if (isfinite(tx))
	{
		float ty = y * len;
		if (isfinite(ty))
		{
			return CPVector2F(tx, ty);
		}
	}

	return CPVector2F();
}

bool CPVector2F::normalization()
{
	CPVector2F temp = getUnit();
	CPVector2F zero;

	if (temp == zero) { return false; }
	else
	{
		for (int i = 0; i < 2; i++)
		{
			v[i] = temp.v[i];
		}
	}

	return true;
}

float CPVector2F::getDegAngle(const CPVector2F& opd) const
{
	return CPM_RAD2DEG(getRadAngle(opd));
}

float CPVector2F::getRadAngle(const CPVector2F& opd) const
{
	float dotProduct = x * opd.x + y * opd.y;
	float inverseLen = 1.0f / (length() * opd.length());

	return acosf(dotProduct * inverseLen);
}

void CPVector2F::setZero()
{
	x = y = 0.0f;
}

CPVector2F operator*(float scalar, const CPVector2F& opd)
{
	return CPVector2F(scalar * opd.x, scalar * opd.y);
}

float CPVector2F::getDistance(const CPVector2F& p1, const CPVector2F& p2)
{
	return sqrtf(getDistanceSquare(p1, p2));
}

float CPVector2F::getDistanceSquare(const CPVector2F& p1, const CPVector2F& p2)
{
	float ret = 0;
	for (int i = 0; i < 2; i++) { ret += powf(p1.v[i] - p2.v[i], 2); }

	return ret;
}

CPVector3F::CPVector3F() : CPVector3F(0.0f, 0.0f, 0.0f) {}

CPVector3F::CPVector3F(float x, float y, float z) { set(x, y, z); }

CPVector3F::CPVector3F(const CPVector3F& v) { set(v.x, v.y, v.z); }

CPVector3F CPVector3F::operator+(const CPVector3F& opd)	const
{
	return CPVector3F(x + opd.x, y + opd.y, z + opd.z);
}

CPVector3F CPVector3F::operator+=(const CPVector3F& opd)
{
	x += opd.x;
	y += opd.y;
	z += opd.z;
	return *this;
}

CPVector3F CPVector3F::operator-(const CPVector3F& opd)	const
{
	return CPVector3F(x - opd.x, y - opd.y, z - opd.z);
}

CPVector3F CPVector3F::operator-=(const CPVector3F& opd)
{
	x -= opd.x;
	y -= opd.y;
	z -= opd.z;
	return *this;
}

CPVector3F CPVector3F::operator*(float scalar)
{
	return CPVector3F(x * scalar, y * scalar, z * scalar);
}

CPVector3F CPVector3F::operator-()
{
	return CPVector3F(-x, -y, -z);
}

bool CPVector3F::operator==(const CPVector3F& opd)	const
{
	if (fabs(x - opd.x) < CPM_FLOAT_ERANGE)
	{
		if (fabs(y - opd.y) < CPM_FLOAT_ERANGE)
		{
			if (fabs(z - opd.z) < CPM_FLOAT_ERANGE) { return true; }
		}
	}
	return false;
}

bool CPVector3F::operator!=(const CPVector3F& opd) const
{
	return !(*this == opd);
}

//dot product
float CPVector3F::operator|(const CPVector3F& opd) const
{
	return x * opd.x + y * opd.y + z * opd.z;
}

//cross product
CPVector3F CPVector3F::operator^(const CPVector3F& opd) const
{
	return CPVector3F(y * opd.z - z * opd.y, z * opd.x - x * opd.z, x * opd.y - y * opd.x);
}

CPVector3F CPVector3F::operator*(const CPMatrix3x3& opd)
{
	CPVector3F ret;

	ret.x = x * opd._11 + y * opd._21 + z * opd._31;
	ret.y = x * opd._12 + y * opd._22 + z * opd._32;
	ret.z = x * opd._13 + y * opd._23 + z * opd._33;

	return ret;
}

CPVector3F CPVector3F::operator*(const CPMatrix4x4& opd)
{
	CPVector3F ret;

	ret.x = x * opd._11 + y * opd._21 + z * opd._31 + 1.0f * opd._41;
	ret.y = x * opd._12 + y * opd._22 + z * opd._32 + 1.0f * opd._42;
	ret.z = x * opd._13 + y * opd._23 + z * opd._33 + 1.0f * opd._43;

	return ret;
}

void CPVector3F::set(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

float CPVector3F::length() const { return sqrtf(lengthSquare()); }

float CPVector3F::lengthSquare() const { return x * x + y * y + z * z; }

CPVector3F CPVector3F::getUnit() const
{
	float len = 1.0f / length();

	float tx = x * len;
	if (isfinite(tx))
	{
		float ty = y * len;
		if (isfinite(ty))
		{
			float tz = z * len;
			if (isfinite(tz))
			{
				return CPVector3F(tx, ty, tz);
			}
		}
	}

	return CPVector3F();
}

bool CPVector3F::normalization()
{
	CPVector3F temp = getUnit();
	CPVector3F zero;

	if (temp == zero) { return false; }
	else
	{
		for (int i = 0; i < 3; i++)
		{
			v[i] = temp.v[i];
		}
	}

	return true;
}

float CPVector3F::getDegAngle(const CPVector3F& opd) const
{
	return CPM_RAD2DEG(getRadAngle(opd));
}

float CPVector3F::getRadAngle(const CPVector3F& opd) const
{
	float dotProduct = x * opd.x + y * opd.y + z * opd.z;
	float inverseLen = 1.0f / (length() * opd.length());

	return acosf(dotProduct * inverseLen);
}

void CPVector3F::setZero()
{
	x = y = z = 0.0f;
}

CPVector3F operator*(float scalar, const CPVector3F& opd)
{
	return CPVector3F(scalar * opd.x, scalar * opd.y, scalar * opd.z);
}

float CPVector3F::getDistance(const CPVector3F& p1, const CPVector3F& p2)
{
	return sqrtf(getDistanceSquare(p1, p2));
}

float CPVector3F::getDistanceSquare(const CPVector3F& p1, const CPVector3F& p2)
{
	float ret = 0;
	for (int i = 0; i < 3; i++) { ret += powf(p1.v[i] - p2.v[i], 2); }

	return ret;
}

CPVector4F::CPVector4F() : CPVector4F(0.0f, 0.0f, 0.0f, 0.0f) {}

CPVector4F::CPVector4F(float x, float y, float z, float w) { set(x, y, z, w); }

CPVector4F::CPVector4F(const CPVector4F& v) { set(v.x, v.y, v.z, v.w); }

CPVector4F CPVector4F::operator+(const CPVector4F& opd)	const
{
	return CPVector4F(x + opd.x, y + opd.y, z + opd.z, w + opd.w);
}

CPVector4F CPVector4F::operator+=(const CPVector4F& opd)
{
	x += opd.x;
	y += opd.y;
	z += opd.z;
	w += opd.w;
	return *this;
}

CPVector4F CPVector4F::operator-(const CPVector4F& opd)	const
{
	return CPVector4F(x - opd.x, y - opd.y, z - opd.z, w - opd.w);
}

CPVector4F CPVector4F::operator-=(const CPVector4F& opd)
{
	x -= opd.x;
	y -= opd.y;
	z -= opd.z;
	w -= opd.w;
	return *this;
}

CPVector4F CPVector4F::operator*(float scalar)
{
	return CPVector4F(x * scalar, y * scalar, z * scalar, w * scalar);
}

CPVector4F CPVector4F::operator-()
{
	return CPVector4F(-x, -y, -z, -w);
}

bool CPVector4F::operator==(const CPVector4F& opd)	const
{
	if (fabs(x - opd.x) < CPM_FLOAT_ERANGE)
	{
		if (fabs(y - opd.y) < CPM_FLOAT_ERANGE)
		{
			if (fabs(z - opd.z) < CPM_FLOAT_ERANGE)
			{
				if (fabs(w - opd.w) < CPM_FLOAT_ERANGE) { return true; }
			}
		}
	}
	return false;
}

bool CPVector4F::operator!=(const CPVector4F& opd) const { return !(*this == opd); }

float CPVector4F::operator|(const CPVector4F& opd) const
{
	return x * opd.x + y * opd.y + z * opd.z;
}

CPVector4F CPVector4F::operator^(const CPVector4F& opd) const
{
	return CPVector4F(y * opd.z - z * opd.y, z * opd.x - x * opd.z, x * opd.y - y * opd.x, 1.0f);
}

CPVector4F CPVector4F::operator*(const CPMatrix4x4& opd)
{
	CPVector4F ret;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) { ret.v[i] += v[j] * opd.m[j][i]; }
	}

	if (isfinite(ret.w))
	{
		ret = { ret.x / ret.w, ret.y / ret.w, ret.z / ret.w, 1.0f };
	}

	return ret;
}

void CPVector4F::set(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

float CPVector4F::length() const { return sqrtf(lengthSquare()); }

float CPVector4F::lengthSquare() const { return x * x + y * y + z * z; }

CPVector4F CPVector4F::getUnit() const
{
	float len = 1.0f / length();

	float tx = x * len;
	if (isfinite(tx))
	{
		float ty = y * len;
		if (isfinite(ty))
		{
			float tz = z * len;
			if (isfinite(tz))
			{
				return CPVector4F(tx, ty, tz, 1.0f);
			}
		}
	}

	return CPVector4F();
}

bool CPVector4F::normalization()
{
	CPVector4F temp = getUnit();
	CPVector4F zero = { 0.0f, 0.0f, 0.0f, 1.0f };

	if (temp == zero) { return false; }
	else
	{
		for (int i = 0; i < 4; i++)
		{
			v[i] = temp.v[i];
		}
	}

	return true;
}

float CPVector4F::getDegAngle(const CPVector4F& opd) const
{
	return CPM_RAD2DEG(getRadAngle(opd));
}

float CPVector4F::getRadAngle(const CPVector4F& opd) const
{
	float dotProduct = x * opd.x + y * opd.y + z * opd.z;
	float inverseLen = 1.0f / (length() * opd.length());

	return acosf(dotProduct * inverseLen);
}

void CPVector4F::setZero()
{
	x = y = z = w = 0.0f;
}

CPVector4F operator*(float scalar, const CPVector4F& opd)
{
	return CPVector4F(scalar * opd.x, scalar * opd.y, scalar * opd.z, scalar * opd.w);
}

float CPVector4F::getDistance(const CPVector4F& p1, const CPVector4F& p2)
{
	return sqrtf(getDistanceSquare(p1, p2));
}

float CPVector4F::getDistanceSquare(const CPVector4F& p1, const CPVector4F& p2)
{
	float ret = 0;
	for (int i = 0; i < 4; i++) { ret += powf(p1.v[i] - p2.v[i], 2); }

	return ret;
}