
#include "Vector2.h"
#include "Engine.h"
#include "Mathf.h"


SSS_USING

const _tagVector2	_tagVector2::Zero = Vector2(0.0f, 0.0f);
const _tagVector2	_tagVector2::One = Vector2(1.0f, 1.0f);
const _tagVector2	_tagVector2::Half = Vector2(0.5f, 0.5f);
const _tagVector2	_tagVector2::Infinity = Vector2(INFINITY, INFINITY);

XMVECTOR _tagVector2::Convert() const
{
	return XMLoadFloat2((XMFLOAT2*)this);
}

void _tagVector2::Convert(const XMVECTOR & v)
{
	XMStoreFloat2((XMFLOAT2*)this, v);
}

// ==============  operator =  =================
void _tagVector2::operator =(const _tagVector2& _v)
{
	x = _v.x;
	y = _v.y;
}

void _tagVector2::operator =(const XMVECTOR& _v)
{
	XMStoreFloat2((XMFLOAT2*)this, _v);
}

void _tagVector2::operator=(const float f[2])
{
	x = f[0];
	y = f[1];
}

void _tagVector2::operator=(const int i[2])
{
	x =(float)i[0];
	y =(float)i[1];
}

// ==============  operator +  =================
_tagVector2 _tagVector2::operator+(const _tagVector2 & _v)	const
{
	/*_tagVector2	v;
	v.Convert(Convert() + _v.Convert());
	return v;*/
	return _tagVector2(x + _v.x, y + _v.y);
}

_tagVector2 _tagVector2::operator+(const XMVECTOR & _v)	const
{
	return _tagVector2(Convert() + _v);
}

_tagVector2 _tagVector2::operator+(const float f[2])	const
{
	return _tagVector2(x + f[0], y + f[1]);
}

_tagVector2 _tagVector2::operator+(const int i[2])	const
{
	return _tagVector2(x + i[0], y + i[1]);
}

_tagVector2 _tagVector2::operator+(float f)	const
{
	return _tagVector2(x + f, y + f);
}

_tagVector2 _tagVector2::operator+(int i)	const
{
	return _tagVector2(x + i, y + i);
}

// ==============  operator +=  =================
_tagVector2& _tagVector2::operator+=(const _tagVector2 & _v)
{
	x += _v.x;
	y += _v.y;
	return *this;
}

_tagVector2& _tagVector2::operator+=(const XMVECTOR & _v)
{
	Convert(Convert() + _v);
	return *this;
}

_tagVector2& _tagVector2::operator+=(const float f[2])
{
	x += f[0];
	y += f[1];
	return *this;
}

_tagVector2& _tagVector2::operator+=(const int i[2])
{
	x += i[0];
	y += i[1];
	return *this;
}

_tagVector2& _tagVector2::operator+=(float f)
{
	x += f;
	y += f;
	return *this;
}

_tagVector2& _tagVector2::operator+=(int i)
{
	x += i;
	y += i;
	return *this;
}

// ==============  operator -  =================

_tagVector2 _tagVector2::operator-()
{
	*this *= -1;
	return *this;
}

_tagVector2 _tagVector2::operator-(const _tagVector2 & _v)	const
{
	/*_tagVector2	v;
	v.Convert(Convert() - _v.Convert());
	return v;*/
	return _tagVector2(x - _v.x, y - _v.y);
}

_tagVector2 _tagVector2::operator-(const XMVECTOR & _v)	const
{
	return _tagVector2(Convert() - _v);
}

_tagVector2 _tagVector2::operator-(const float f[2])	const
{
	return _tagVector2(x - f[0], y - f[1]);
}

_tagVector2 _tagVector2::operator-(const int i[2])	const
{
	return _tagVector2(x - i[0], y - i[1]);
}

_tagVector2 _tagVector2::operator-(float f)	const
{
	return _tagVector2(x - f, y - f);
}

_tagVector2 _tagVector2::operator-(int i)	const
{
	return _tagVector2(x - i, y - i);
}

// ==============  operator -=  =================
_tagVector2& _tagVector2::operator-=(const _tagVector2 & _v)
{
	x -= _v.x;
	y -= _v.y;
	return *this;
}

_tagVector2& _tagVector2::operator-=(const XMVECTOR & _v)
{
	Convert(Convert() - _v);
	return *this;
}

_tagVector2& _tagVector2::operator-=(const float f[2])
{
	x -= f[0];
	y -= f[1];
	return *this;
}

_tagVector2& _tagVector2::operator-=(const int i[2])
{
	x -= i[0];
	y -= i[1];
	return *this;
}

_tagVector2& _tagVector2::operator-=(float f)
{
	x -= f;
	y -= f;
	return *this;
}

_tagVector2& _tagVector2::operator-=(int i)
{
	x -= i;
	y -= i;
	return *this;
}


// ==============  operator *  =================

_tagVector2 _tagVector2::operator*(float f)	const
{
	return _tagVector2(x * f, y * f);
}

_tagVector2 _tagVector2::operator*(int i)	const
{
	return _tagVector2(x * i, y * i);
}

// ==============  operator *=  =================

_tagVector2& _tagVector2::operator*=(float f)
{
	x *= f;
	y *= f;
	return *this;
}

_tagVector2& _tagVector2::operator*=(int i)
{
	x *= i;
	y *= i;
	return *this;
}


// ==============  operator /  =================

_tagVector2 _tagVector2::operator/(float f)	const
{
	return _tagVector2(x / f, y / f);
}

_tagVector2 _tagVector2::operator/(int i)	const
{
	return _tagVector2(x / i, y / i);
}

// ==============  operator /=  =================

_tagVector2& _tagVector2::operator/=(float f)
{
	x /= f;
	y /= f;
	return *this;
}

_tagVector2& _tagVector2::operator/=(int i)
{
	x /= i;
	y /= i;
	return *this;
}

bool _tagVector2::operator==(const _tagVector2 & _v) const
{
	return x == _v.x && y == _v.y;
}

bool _tagVector2::operator==(const XMVECTOR & _v) const
{
	_tagVector2	v;
	v.Convert(_v);
	return x == v.x && y == v.y ;
}

bool _tagVector2::operator!=(const _tagVector2 & _v) const
{
	return x != _v.x || y != _v.y;
}

bool _tagVector2::operator!=(const XMVECTOR & _v) const
{
	_tagVector2	v;
	v.Convert(_v);
	return x != v.x || y != v.y;
}

_tagVector2 _tagVector2::Abs()
{
	return _tagVector2(fabsf(x), fabsf(y));
}

float _tagVector2::Length() const
{
	return sqrtf(x * x + y * y);
}

_tagVector2 _tagVector2::Normalize()	const
{
	_tagVector2	v;
	v.Convert(XMVector2Normalize(Convert()));
	return v;
}

void _tagVector2::normalize()
{
	*this = XMVector2Normalize(Convert());
}

float _tagVector2::Distance(_tagVector2 _v)
{
	return (*this - _v).Length();
}

float _tagVector2::Distance(_tagVector2 _v1, _tagVector2 _v2)
{
	return (_v1 - _v2).Length();
}

float _tagVector2::Dot(const _tagVector2 & _v) const
{
	return x * _v.x + y * _v.y ;
}

float _tagVector2::Dot(const XMVECTOR & _v) const
{
	_tagVector2	v;
	v.Convert(XMVector2Dot(Convert(), _v));
	return v.x;
}

float _tagVector2::Angle(const _tagVector2 & _v) const
{
	_tagVector2	v = Normalize();
	float	fAngle = v.Dot(_v.Normalize());
	return acosf(fAngle);
}

float _tagVector2::Angle(const XMVECTOR & _v) const
{
	_tagVector2	v = Normalize();
	_tagVector2	v1(_v);
	float	fAngle = v.Dot(v1.Normalize());
	return acosf(fAngle);
}

_tagVector2 _tagVector2::AngleToDirection(float fAngle) 
{
	return Vector2(cosf(fAngle * D2R), sinf(fAngle * D2R));
}

_tagVector2 _tagVector2::RandomDirection()
{
	float fAngle = rand() % 3601;
	return AngleToDirection(fAngle * 0.1f);
}

_tagVector2 _tagVector2::RandomPointInsideCircle(const _tagVector2 & vCenter, float fRadius, float fMinRadius)
{
	float fRandomDistance = CMathf::RandomRange(fMinRadius, fRadius);
	return vCenter + (RandomDirection() * fRandomDistance);
}

bool _tagVector2::Intersection(const _tagVector2& vStartPoint, const _tagVector2& vEndPoint, const _tagVector2& vTargetStartPoint, const _tagVector2& vTargetEndPoint)
{
	float ab = (vEndPoint - vStartPoint).Cross(vTargetStartPoint - vStartPoint).x * (vEndPoint - vStartPoint).Cross(vTargetEndPoint - vStartPoint).x;
	float cd = (vTargetEndPoint - vTargetStartPoint).Cross(vStartPoint - vTargetStartPoint).x * (vTargetEndPoint - vTargetStartPoint).Cross(vEndPoint - vTargetStartPoint).x;

	//두 선이 한 직선위에 있거나 끝점이 겹치는 경우
	
	if (ab == 0 && cd == 0)
	{
		Vector2 vSP;
		Vector2 vEP;
		Vector2 vTSP;
		Vector2 vTEP;

		if (vEndPoint.x < vStartPoint.x && vEndPoint.y < vStartPoint.y)
		{
			vSP = vEndPoint;
			vEP = vStartPoint;
		}
		else
		{
			vSP = vStartPoint;
			vEP = vEndPoint;
		}

		if (vTargetEndPoint.x < vTargetStartPoint.x && vTargetEndPoint.y < vTargetStartPoint.y)
		{
			vTSP = vTargetEndPoint;
			vTEP = vTargetStartPoint;
		}
		else
		{
			vTSP = vTargetStartPoint;
			vTEP = vTargetEndPoint;
		}

		return !((vEP.x < vTSP.x && vEP.y < vTSP.y) || (vTEP.x < vSP.x && vTEP.y < vSP.y));
	}

	return ab <= 0 && cd <= 0;
}

_tagVector2 _tagVector2::Cross(const _tagVector2 & _v) const
{
	XMVECTOR	v = XMVector2Cross(Convert(), _v.Convert());
	return _tagVector2(v);
}

_tagVector2 _tagVector2::Cross(const XMVECTOR & _v) const
{
	XMVECTOR	v = XMVector2Cross(Convert(), _v);
	return _tagVector2(v);
}
