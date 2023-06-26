#pragma once
#include "CPGCStd.h"
#include "CPMatrix.h"

#define CPM_PI							3.1415926f
#define CPM_TAU							6.2831853f

#define CPM_RAD2DEG(x)					( ( x ) * ( 180.0f / CPM_PI ) )
#define CPM_DEG2RAD(x)					( ( x ) * ( CPM_PI / 180.0f ) )

#define CPM_FLOAT_ERANGE				1.0E-5f		//Error Range

class CPMatrix2x2;
class CPMatrix3x3;
class CPMatrix4x4;

struct CPSFLOAT2
{
	union
	{
		struct { float x, y; };
		float v[2];
	};
};

struct CPSFLOAT3
{
	union
	{
		struct { float x, y, z; };
		float v[3];
	};
};

struct CPSFLOAT4
{
	union
	{
		struct { float x, y, z, w; };
		float v[4];
	};
};

struct CPVector2F : CPSFLOAT2
{
	friend CPVector2F operator*(float scalar, const CPVector2F& opd);

public:
	//생성자
	CPVector2F();
	CPVector2F(float x, float y);
	CPVector2F(const CPVector2F& v);

	//연산자 재정의 : 이항
	CPVector2F	operator+(const CPVector2F& opd)		const;
	CPVector2F	operator+=(const CPVector2F& opd);
	CPVector2F	operator-(const CPVector2F& opd)		const;
	CPVector2F	operator-=(const CPVector2F& opd);
	
	bool		operator==(const CPVector2F& opd)		const;
	bool		operator!=(const CPVector2F& opd)		const;

	CPVector2F	operator*(const CPMatrix3x3& opd)		const;
	CPVector2F	operator*(float scalar)					const;

	CPVector2F& operator=(const CPVector2F& opd);

	//연산자 재정의 : 단항
	CPVector2F	operator-();

public:
	void		set(float x, float y);

	float		length()							const;
	float		lengthSquare()						const;
	CPVector2F	getUnit()							const;
	bool		normalization();

	float		getDegAngle(const CPVector2F& opd)	const;
	float		getRadAngle(const CPVector2F& opd)	const;
	void		setZero();

public:
	static float getDistance(const CPVector2F& p1,
		const CPVector2F& p2);
	static float getDistanceSquare(const CPVector2F& p1,
		const CPVector2F& p2);
};

struct CPVector3F : CPSFLOAT3
{
	friend CPVector3F operator*(float scalar, const CPVector3F& opd);

public:
	CPVector3F();
	CPVector3F(float x, float y, float z);
	CPVector3F(const CPVector3F& v);

public:
	CPVector3F	operator+(const CPVector3F& opd)	const;
	CPVector3F	operator+=(const CPVector3F& opd);
	CPVector3F	operator-(const CPVector3F& opd)	const;
	CPVector3F	operator-=(const CPVector3F& opd);
	CPVector3F	operator*(float scalar);

	CPVector3F	operator-();

	bool		operator==(const CPVector3F& opd)	const;
	bool		operator!=(const CPVector3F& opd)	const;

	//Dot Product
	float		operator|(const CPVector3F& opd) const;

	//Cross Product
	CPVector3F	operator^(const CPVector3F& opd) const;

	CPVector3F	operator*(const CPMatrix3x3& opd);
	CPVector3F	operator*(const CPMatrix4x4& opd);

public:
	void		set(float x, float y, float z);
	float		length()							const;
	float		lengthSquare()						const;
	CPVector3F	getUnit()						const;
	bool		normalization();

	float		getDegAngle(const CPVector3F& opd)	const;
	float		getRadAngle(const CPVector3F& opd)	const;
	void		setZero();

	static float getDistance(const CPVector3F& p1,
		const CPVector3F& p2);
	static float getDistanceSquare(const CPVector3F& p1,
		const CPVector3F& p2);
};

struct CPVector4F : CPSFLOAT4
{
	friend CPVector4F operator*(float scalar, const CPVector4F& opd);

public:
	CPVector4F();
	CPVector4F(float x, float y, float z, float w);
	CPVector4F(const CPVector4F& v);

public:
	CPVector4F	operator+(const CPVector4F& opd)	const;
	CPVector4F	operator+=(const CPVector4F& opd);
	CPVector4F	operator-(const CPVector4F& opd)	const;
	CPVector4F	operator-=(const CPVector4F& opd);
	CPVector4F	operator*(float scalar);

	CPVector4F	operator-();

	bool		operator==(const CPVector4F& opd)	const;
	bool		operator!=(const CPVector4F& opd)	const;

	//Dot Product
	float		operator|(const CPVector4F& opd) const;

	//Cross Product
	CPVector4F	operator^(const CPVector4F& opd) const;

	CPVector4F	operator*(const CPMatrix4x4& opd);

public:
	void		set(float x, float y, float z, float w);
	float		length()							const;
	float		lengthSquare()						const;
	CPVector4F	getUnit()						const;
	bool		normalization();

	float		getDegAngle(const CPVector4F& opd)	const;
	float		getRadAngle(const CPVector4F& opd)	const;
	void		setZero();
public:
	static float getDistance(const CPVector4F& p1,
		const CPVector4F& p2);
	static float getDistanceSquare(const CPVector4F& p1,
		const CPVector4F& p2);
};
