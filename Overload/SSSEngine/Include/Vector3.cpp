
#include "Vector3.h"
#include "Engine.h"
#include "Mathf.h"

SSS_USING



const _tagVector3	_tagVector3::Zero = Vector3(0, 0, 0);
const _tagVector3	_tagVector3::One = Vector3(1, 1, 1);
const _tagVector3	_tagVector3::Half = Vector3(0.5f, 0.5f, 0.5f);
const _tagVector3 _tagVector3::Infinity = Vector3(INFINITY, INFINITY, INFINITY);
const _tagVector3	_tagVector3::Axis[AX_END] =
{
	_tagVector3(1.f, 0.f, 0.f),
	_tagVector3(0.f, 1.f, 0.f),
	_tagVector3(0.f, 0.f, 1.f)
};

_tagVector3::_tagVector3() :
	x(0.f),
	y(0.f),
	z(0.f)
{
}

_tagVector3::_tagVector3(float _x, float _y, float _z) :
	x(_x),
	y(_y),
	z(_z)
{
}

_tagVector3::_tagVector3(const _tagVector3& _v) :
	x(_v.x),
	y(_v.y),
	z(_v.z)
{
}

_tagVector3::_tagVector3(const float f[3]) :
	x(f[0]),
	y(f[1]),
	z(f[2])
{
}

_tagVector3::_tagVector3(const int i[3]) :
	x((float)i[0]),
	y((float)i[1]),
	z((float)i[2])
{
}

_tagVector3::_tagVector3(const XMVECTOR& _v)
{
	XMStoreFloat3((XMFLOAT3*)this, _v);
}

XMVECTOR _tagVector3::Convert() const
{
	return XMLoadFloat3((XMFLOAT3*)this);
}

void _tagVector3::Convert(const XMVECTOR & v)
{
	XMStoreFloat3((XMFLOAT3*)this, v);
}

// ==============  operator =  =================
void _tagVector3::operator =(const _tagVector3& _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
}

void _tagVector3::operator =(const XMVECTOR& _v)
{
	XMStoreFloat3((XMFLOAT3*)this, _v);
}

void _tagVector3::operator=(const float f[3])
{
	x = f[0];
	y = f[1];
	z = f[2];
}

void _tagVector3::operator=(const int i[3])
{
	x =(float)i[0];
	y =(float)i[1];
	z =(float)i[2];
}

// ==============  operator +  =================
_tagVector3 _tagVector3::operator+(const _tagVector3 & _v)	const
{
	/*_tagVector3	v;
	v.Convert(Convert() + _v.Convert());
	return v;*/
	return _tagVector3(x + _v.x, y + _v.y, z + _v.z);
}

_tagVector3 _tagVector3::operator+(const XMVECTOR & _v)	const
{
	return _tagVector3(Convert() + _v);
}

_tagVector3 _tagVector3::operator+(const float f[3])	const
{
	return _tagVector3(x + f[0], y + f[1], z + f[2]);
}

_tagVector3 _tagVector3::operator+(const int i[3])	const
{
	return _tagVector3(x + i[0], y + i[1], z + i[2]);
}

_tagVector3 _tagVector3::operator+(float f)	const
{
	return _tagVector3(x + f, y + f, z + f);
}

_tagVector3 _tagVector3::operator+(int i)	const
{
	return _tagVector3(x + i, y + i, z + i);
}

// ==============  operator +=  =================
_tagVector3& _tagVector3::operator+=(const _tagVector3 & _v)
{
	x += _v.x;
	y += _v.y;
	z += _v.z;
	return *this;
}

_tagVector3& _tagVector3::operator+=(const XMVECTOR & _v)
{
	Convert(Convert() + _v);
	return *this;
}

_tagVector3& _tagVector3::operator+=(const float f[3])
{
	x += f[0];
	y += f[1];
	z += f[2];
	return *this;
}

_tagVector3& _tagVector3::operator+=(const int i[3])
{
	x += i[0];
	y += i[1];
	z += i[2];
	return *this;
}

_tagVector3& _tagVector3::operator+=(float f)
{
	x += f;
	y += f;
	z += f;
	return *this;
}

_tagVector3& _tagVector3::operator+=(int i)
{
	x += i;
	y += i;
	z += i;
	return *this;
}

// ==============  operator -  =================

_tagVector3 _tagVector3::operator-()
{
	*this *= -1;
	return *this;
}

_tagVector3 _tagVector3::operator-(const _tagVector3 & _v)	const
{
	/*_tagVector3	v;
	v.Convert(Convert() - _v.Convert());
	return v;*/
	return _tagVector3(x - _v.x, y - _v.y, z - _v.z);
}

_tagVector3 _tagVector3::operator-(const XMVECTOR & _v)	const
{
	return _tagVector3(Convert() - _v);
}

_tagVector3 _tagVector3::operator-(const float f[3])	const
{
	return _tagVector3(x - f[0], y - f[1], z - f[2]);
}

_tagVector3 _tagVector3::operator-(const int i[3])	const
{
	return _tagVector3(x - i[0], y - i[1], z - i[2]);
}

_tagVector3 _tagVector3::operator-(float f)	const
{
	return _tagVector3(x - f, y - f, z - f);
}

_tagVector3 _tagVector3::operator-(int i)	const
{
	return _tagVector3(x - i, y - i, z - i);
}

// ==============  operator -=  =================
_tagVector3& _tagVector3::operator-=(const _tagVector3 & _v)
{
	x -= _v.x;
	y -= _v.y;
	z -= _v.z;
	return *this;
}

_tagVector3& _tagVector3::operator-=(const XMVECTOR & _v)
{
	Convert(Convert() - _v);
	return *this;
}

_tagVector3& _tagVector3::operator-=(const float f[3])
{
	x -= f[0];
	y -= f[1];
	z -= f[2];
	return *this;
}

_tagVector3& _tagVector3::operator-=(const int i[3])
{
	x -= i[0];
	y -= i[1];
	z -= i[2];
	return *this;
}

_tagVector3& _tagVector3::operator-=(float f)
{
	x -= f;
	y -= f;
	z -= f;
	return *this;
}

_tagVector3& _tagVector3::operator-=(int i)
{
	x -= i;
	y -= i;
	z -= i;
	return *this;
}


// ==============  operator *  =================
_tagVector3 _tagVector3::operator*(float f)	const
{
	return _tagVector3(x * f, y * f, z * f);
}

_tagVector3 _tagVector3::operator*(int i)	const
{
	return _tagVector3(x * i, y * i, z * i);
}

_tagVector3 _tagVector3::operator*(_tagVector3 _v) const
{
	return _tagVector3(_v.x * x, _v.y * y, _v.z * z);
}

// ==============  operator *=  =================

_tagVector3& _tagVector3::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

_tagVector3& _tagVector3::operator*=(int i)
{
	x *= i;
	y *= i;
	z *= i;
	return *this;
}


// ==============  operator /  =================

_tagVector3 _tagVector3::operator/(float f)	const
{
	return _tagVector3(x / f, y / f, z / f);
}

_tagVector3 _tagVector3::operator/(int i)	const
{
	return _tagVector3(x / i, y / i, z / i);
}

// ==============  operator /=  =================

_tagVector3& _tagVector3::operator/=(float f)
{
	x /= f;
	y /= f;
	z /= f;
	return *this;
}

_tagVector3& _tagVector3::operator/=(int i)
{
	x /= i;
	y /= i;
	z /= i;
	return *this;
}

_tagVector3 & _tagVector3::operator++()
{
	++x;
	++y;
	++z;
	return *this;
}

_tagVector3 & _tagVector3::operator--()
{
	--x;
	--y;
	--z;
	return *this;
}

bool _tagVector3::operator==(const _tagVector3 & _v) const
{
	return x == _v.x && y == _v.y && z == _v.z;
}

bool _tagVector3::operator==(const XMVECTOR & _v) const
{
	_tagVector3	v;
	v.Convert(_v);
	return x == v.x && y == v.y && z == v.z;
}

bool _tagVector3::operator!=(const _tagVector3 & _v) const
{
	return x != _v.x || y != _v.y || z != _v.z;
}

bool _tagVector3::operator!=(const XMVECTOR & _v) const
{
	_tagVector3	v;
	v.Convert(_v);
	return x != v.x || y != v.y || z != v.z;
}

_tagVector4 _tagVector3::ToQuaternion()
{
	return XMQuaternionRotationRollPitchYawFromVector(Convert());
}

_tagVector4 _tagVector3::ToQuaternion(const _tagVector3 & vEulerAngle)
{
	return XMQuaternionRotationRollPitchYaw(vEulerAngle.x, vEulerAngle.y, vEulerAngle.z);
}

float _tagVector3::Min()
{
	float fMin = 100000000.0f;

	if (fMin >= x)
		fMin = x;
	
	if (fMin >= y)
		fMin = y;

	if (fMin >= z)
		fMin = z;

	return fMin;
}

float _tagVector3::Max()
{
	float fMax	 = -100000000.0f;

	if (fMax <= x)
		fMax = x;

	if (fMax <= y)
		fMax = y;

	if (fMax <= z)
		fMax = z;

	return fMax;
}

float _tagVector3::Length() const
{
	return sqrtf(x * x + y * y + z * z);
}

_tagVector3 _tagVector3::Normalize()	const
{
	_tagVector3	v;
	v.Convert(XMVector3Normalize(Convert()));
	return v;
}

void _tagVector3::normalize()
{
	*this = XMVector3Normalize(Convert());
}

_tagVector3 _tagVector3::Abs()
{
	return _tagVector3(fabsf(x), fabsf(y), fabsf(z));
}

_tagVector3 _tagVector3::Slerp(const _tagVector3 & vBegin, const _tagVector3 & vEnd, float fRate)
{
		float dot = vBegin.Dot(vEnd);
		dot = CMathf::Clamp(dot, -1.0f, 1.0f);
		float theta = acosf(dot) * fRate;
		_tagVector3 vRelativeVector = vEnd - vBegin * dot;
		vRelativeVector.normalize();

		return ((vBegin * cosf(theta)) + (vRelativeVector * sinf(theta)));
}

_tagVector3 _tagVector3::Lerp(const _tagVector3 & vBegin, const _tagVector3 & vEnd, float fRate)
{
	return XMVectorLerp(vBegin.Convert(), vEnd.Convert(), fRate);
}

_tagVector3 & _tagVector3::Maximum(const _tagVector3 & v1, const _tagVector3 & v2)
{
	Vector3 vResult = v1;
	if (v1.x < v2.x)
		vResult.x = v2.x;
	if (v1.y < v2.y)
		vResult.y = v2.y;
	if (v1.z < v2.z)
		vResult.z = v2.z;

	return vResult;
}

_tagVector3 & _tagVector3::Minimum(const _tagVector3 & v1, const _tagVector3 & v2)
{
	Vector3 vResult = v1;
	if (v1.x > v2.x)
		vResult.x = v2.x;
	if (v1.y > v2.y)
		vResult.y = v2.y;
	if (v1.z > v2.z)
		vResult.z = v2.z;

	return vResult;
}

void _tagVector3::Maximize(const _tagVector3 & vTarget)
{
	if (x < vTarget.x)
		x = vTarget.x;
	if (y < vTarget.y)
		y = vTarget.y;
	if (z < vTarget.z)
		z = vTarget.z;
}

void _tagVector3::Minimize(const _tagVector3 & vTarget)
{
	if (x > vTarget.x)
		x = vTarget.x;
	if (y > vTarget.y)
		y = vTarget.y;
	if (z > vTarget.z)
		z = vTarget.z;
}


void _tagVector3::SetMinMaxVolume(_tagVector3 * pMinimizeTarget, _tagVector3 * pMaximizeTarget)
{
	Vector3 MinSort;
	Vector3 MaxSort;

	if (pMinimizeTarget->x < pMaximizeTarget->x)
	{
		MinSort.x = pMinimizeTarget->x;
		MaxSort.x = pMaximizeTarget->x;
	}
	else
	{
		MinSort.x = pMaximizeTarget->x;
		MaxSort.x = pMinimizeTarget->x;
	}

	if (pMinimizeTarget->y < pMaximizeTarget->y)
	{
		MinSort.y = pMinimizeTarget->y;
		MaxSort.y = pMaximizeTarget->y;
	}
	else
	{
		MinSort.y = pMaximizeTarget->y;
		MaxSort.y = pMinimizeTarget->y;
	}

	if (pMinimizeTarget->z < pMaximizeTarget->z)
	{
		MinSort.z = pMinimizeTarget->z;
		MaxSort.z = pMaximizeTarget->z;
	}
	else
	{
		MinSort.z = pMaximizeTarget->z;
		MaxSort.z = pMinimizeTarget->z;
	}

	*pMinimizeTarget = MinSort;
	*pMaximizeTarget = MaxSort;
}

float _tagVector3::Distance(_tagVector3 _v)
{
	return (*this - _v).Length();
}

float _tagVector3::Distance(_tagVector3 _v1, _tagVector3 _v2)
{
	return (_v1 - _v2).Length();
}


float _tagVector3::Dot(const _tagVector3 & _v) const
{
	return x * _v.x + y * _v.y + z * _v.z;
}

float _tagVector3::Dot(const XMVECTOR & _v) const
{
	_tagVector3	v;
	v.Convert(XMVector3Dot(Convert(), _v));
	return v.x;
}

float _tagVector3::Angle(const _tagVector3 & _v) const
{
	_tagVector3	v = Normalize();
	float	fAngle = v.Dot(_v.Normalize());
	return acosf(fAngle);
}

float _tagVector3::Angle(const XMVECTOR & _v) const
{
	_tagVector3	v = Normalize();
	_tagVector3	v1(_v);
	float	fAngle = v.Dot(v1.Normalize());
	return acosf(fAngle);
}

_tagVector3 _tagVector3::RandomDirection()
{
	return _tagVector3(RandomRange(-1.0f, 1.0f), RandomRange(-1.0f, 1.0f), RandomRange(-1.0f, 1.0f)).Normalize();
}

_tagVector3 _tagVector3::DirectionToAngle(_tagVector3 vDirection)
{
	_tagVector3 rt;
	return rt;
}

_tagVector3 _tagVector3::TransformNormal(const _tagMatrix & _m) const
{
	// 3차원 벡터와 4x4 행렬은 곱할 수 없다. Normald은 w를 0으로 해서 곱한다.
	_tagVector3	v;
	v = XMVector3TransformNormal(Convert(), _m.mat);
	return v;
}

_tagVector3 _tagVector3::TransformCoord(const _tagMatrix & _m) const
{
	// Coord는 w를 1로 해서 곱한다.
	_tagVector3	v;
	v = XMVector3TransformCoord(Convert(), _m.mat);
	return v;	
}



_tagVector3 _tagVector3::Cross(const _tagVector3 & _v) const
{
	XMVECTOR	v = XMVector3Cross(Convert(), _v.Convert());
	return _tagVector3(v);
}

_tagVector3 _tagVector3::Cross(const XMVECTOR & _v) const
{
	XMVECTOR	v = XMVector3Cross(Convert(), _v);
	return _tagVector3(v);
}

