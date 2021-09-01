#pragma once
#include "Macro.h"

SSS_BEGIN


typedef struct SSS_DLL _tagVector2
{
	float x;
	float y;


	static const _tagVector2	Zero;
	static const _tagVector2	One;
	static const _tagVector2	Half;
	static const _tagVector2 Infinity;


	_tagVector2() :	x(0.f), y(0.f)	{};
	_tagVector2(float _x, float _y) : x(_x), y(_y)	{};
	_tagVector2(const _tagVector2& _v) :	x(_v.x), y(_v.y) {};
	_tagVector2(const float f[2]) : x(f[0]), y(f[1]) {};
	_tagVector2(const int i[2]) : x((float)i[0]), y((float)i[1]) {};
	_tagVector2(const XMVECTOR& _v) { XMStoreFloat2((XMFLOAT2*)this, _v); };
		
	XMVECTOR Convert()	const;
	void Convert(const XMVECTOR& v);

	// ==============  operator =  =================
	void operator =(const _tagVector2& _v);
	void operator =(const XMVECTOR& _v);
	void operator =(const float f[2]);
	void operator =(const int i[2]);

	// ==============  operator +  =================
	_tagVector2 operator +(const _tagVector2& _v)	const;
	_tagVector2 operator +(const XMVECTOR& _v)	const;
	_tagVector2 operator +(const float f[2])	const;
	_tagVector2 operator +(const int i[2])	const;
	_tagVector2 operator +(float f)	const;
	_tagVector2 operator +(int i)	const;

	// ==============  operator +=  =================
	_tagVector2& operator +=(const _tagVector2& _v);
	_tagVector2& operator +=(const XMVECTOR& _v);
	_tagVector2& operator +=(const float f[2]);
	_tagVector2& operator +=(const int i[2]);
	_tagVector2& operator +=(float f);
	_tagVector2& operator +=(int i);

	// ==============  operator -  =================
	//Inverse Vector

	_tagVector2 operator -();

	_tagVector2 operator -(const _tagVector2& _v)	const;
	_tagVector2 operator -(const XMVECTOR& _v)	const;
	_tagVector2 operator -(const float f[2])	const;
	_tagVector2 operator -(const int i[2])	const;
	_tagVector2 operator -(float f)	const;
	_tagVector2 operator -(int i)	const;

	// ==============  operator -=  =================
	_tagVector2& operator -=(const _tagVector2& _v);
	_tagVector2& operator -=(const XMVECTOR& _v);
	_tagVector2& operator -=(const float f[2]);
	_tagVector2& operator -=(const int i[2]);
	_tagVector2& operator -=(float f);
	_tagVector2& operator -=(int i);


	// ==============  operator *  =================
	_tagVector2 operator *(float f)	const;
	_tagVector2 operator *(int i)	const;

	// ==============  operator *=  =================
	_tagVector2& operator *=(float f);
	_tagVector2& operator *=(int i);


	// ==============  operator /  =================
	_tagVector2 operator /(float f)	const;
	_tagVector2 operator /(int i)	const;

	// ==============  operator /=  =================
	_tagVector2& operator /=(float f);
	_tagVector2& operator /=(int i);

	// ==============  operator ==, !=  =================
	bool operator ==(const _tagVector2& _v)	const;
	bool operator ==(const XMVECTOR& _v)	const;
	bool operator !=(const _tagVector2& _v)	const;
	bool operator !=(const XMVECTOR& _v)	const;

	// =============== Conversion ================
	operator XMVECTOR ()
	{
		return Convert();
	}


	// =============== 벡터 관련 함수들 ================
	_tagVector2 Abs();
	float Length()	const;
	_tagVector2 Normalize()	const;
	void normalize();
	float Distance(_tagVector2 _v);
	static float Distance(_tagVector2 _v1, _tagVector2 _v2);

	float Dot(const _tagVector2& _v)	const;
	float Dot(const XMVECTOR& _v)	const;
	float Angle(const _tagVector2& _v)	const;
	float Angle(const XMVECTOR& _v)	const;
	//Degree
	static _tagVector2 AngleToDirection(float fAngle);
	static _tagVector2 RandomDirection();
	static _tagVector2 RandomPointInsideCircle(const _tagVector2& vCenter, float fRadius, float fMinRadius = 0.0f);
	static bool Intersection(const _tagVector2& vStartPoint, const _tagVector2& vEndPoint, const _tagVector2& vTargetStartPoint, const _tagVector2& vTargetEndPoint);
	_tagVector2 Cross(const _tagVector2& _v)	const;
	_tagVector2 Cross(const XMVECTOR& _v)	const;

}Vector2, *PVector2;



SSS_END
