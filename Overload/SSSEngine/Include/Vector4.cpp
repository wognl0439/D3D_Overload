
#include "Vector4.h"
#include "Mathf.h"

SSS_USING


_tagVector4 _tagVector4::clear =		_tagVector4(0.0f, 0.0f, 0.0f, 0.0f);
_tagVector4 _tagVector4::white =		_tagVector4(1.0f, 1.0f, 1.0f, 1.0f);
_tagVector4 _tagVector4::black =		_tagVector4(0.0f, 0.0f, 0.0f, 1.0f);
_tagVector4 _tagVector4::red =			_tagVector4(1.0f, 0.0f, 0.0f, 1.0f);
_tagVector4 _tagVector4::green =		_tagVector4(0.0f, 1.0f, 0.0f, 1.0f);
_tagVector4 _tagVector4::blue =			_tagVector4(0.0f, 0.0f, 1.0f, 1.0f);
_tagVector4 _tagVector4::yellow =		_tagVector4(1.0f, 1.0f, 0.0f, 1.0f);
_tagVector4 _tagVector4::magenta =	_tagVector4(1.0f, 0.0f, 1.0f, 1.0f);
_tagVector4 _tagVector4::cyan =		_tagVector4(0.0f, 1.0f, 1.0f, 1.0f);

_tagVector4 _tagVector4::hotpink = _tagVector4(1.0f, 0.4117f, 0.7058f, 1.0f);


_tagVector4::_tagVector4() :
	x(0.f),
	y(0.f),
	z(0.f),
	w(0.f)
{
}

_tagVector4::_tagVector4(float _x, float _y, float _z, float _w) :
	x(_x),
	y(_y),
	z(_z),
	w(_w)
{
}

_tagVector4::_tagVector4(const _tagVector4& _v) :
	x(_v.x),
	y(_v.y),
	z(_v.z),
	w(_v.w)
{
}

_tagVector4::_tagVector4(const float f[4]) :
	x(f[0]),
	y(f[1]),
	z(f[2]),
	w(f[3])
{
}

_tagVector4::_tagVector4(const int i[4]) :
	x((float)i[0]),
	y((float)i[1]),
	z((float)i[2]),
	w((float)i[3])
{
}

_tagVector4::_tagVector4(const XMVECTOR& _v)
{
	XMStoreFloat4((XMFLOAT4*)this, _v);
}

XMVECTOR _tagVector4::Convert() const
{
	return XMLoadFloat4((XMFLOAT4*)this);
}

void _tagVector4::Convert(const XMVECTOR & v)
{
	XMStoreFloat4((XMFLOAT4*)this, v);
}

// ==============  operator =  =================
void _tagVector4::operator =(const _tagVector4& _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
	w = _v.w;
}

void _tagVector4::operator =(const XMVECTOR& _v)
{
	XMStoreFloat4((XMFLOAT4*)this, _v);
}

void _tagVector4::operator=(const float f[4])
{
	x = f[0];
	y = f[1];
	z = f[2];
	w = f[3];
}

void _tagVector4::operator=(const int i[4])
{
	x = (float)i[0];
	y = (float)i[1];
	z = (float)i[2];
	w = (float)i[3];
}

// ==============  operator +  =================
_tagVector4 _tagVector4::operator+(const _tagVector4 & _v)	const
{
	/*_tagVector4	v;
	v.Convert(Convert() + _v.Convert());
	return v;*/
	return _tagVector4(x + _v.x, y + _v.y, z + _v.z, w + _v.w);
}

_tagVector4 _tagVector4::operator+(const XMVECTOR & _v)	const
{
	return _tagVector4(Convert() + _v);
}

_tagVector4 _tagVector4::operator+(const float f[4])	const
{
	return _tagVector4(x + f[0], y + f[1], z + f[2], w + f[3]);
}

_tagVector4 _tagVector4::operator+(const int i[4])	const
{
	return _tagVector4(x + i[0], y + i[1], z + i[2], w + i[3]);
}

_tagVector4 _tagVector4::operator+(float f)	const
{
	return _tagVector4(x + f, y + f, z + f, w + f);
}

_tagVector4 _tagVector4::operator+(int i)	const
{
	return _tagVector4(x + i, y + i, z + i, w + i);
}

// ==============  operator +=  =================
_tagVector4& _tagVector4::operator+=(const _tagVector4 & _v)
{
	x += _v.x;
	y += _v.y;
	z += _v.z;
	w += _v.w;
	return *this;
}

_tagVector4& _tagVector4::operator+=(const XMVECTOR & _v)
{
	Convert(Convert() + _v);
	return *this;
}

_tagVector4& _tagVector4::operator+=(const float f[4])
{
	x += f[0];
	y += f[1];
	z += f[2];
	w += f[3];
	return *this;
}

_tagVector4& _tagVector4::operator+=(const int i[4])
{
	x += i[0];
	y += i[1];
	z += i[2];
	w += i[3];
	return *this;
}

_tagVector4& _tagVector4::operator+=(float f)
{
	x += f;
	y += f;
	z += f;
	w += f;
	return *this;
}

_tagVector4& _tagVector4::operator+=(int i)
{
	x += i;
	y += i;
	z += i;
	w += i;
	return *this;
}


// ==============  operator -  =================
_tagVector4 _tagVector4::operator-(const _tagVector4 & _v)	const
{
	/*_tagVector4	v;
	v.Convert(Convert() - _v.Convert());
	return v;*/
	return _tagVector4(x - _v.x, y - _v.y, z - _v.z, w - _v.w);
}

_tagVector4 _tagVector4::operator-(const XMVECTOR & _v)	const
{
	return _tagVector4(Convert() - _v);
}

_tagVector4 _tagVector4::operator-(const float f[4])	const
{
	return _tagVector4(x - f[0], y - f[1], z - f[2], w - f[3]);
}

_tagVector4 _tagVector4::operator-(const int i[4])	const
{
	return _tagVector4(x - i[0], y - i[1], z - i[2], w - i[3]);
}

_tagVector4 _tagVector4::operator-(float f)	const
{
	return _tagVector4(x - f, y - f, z - f, w - f);
}

_tagVector4 _tagVector4::operator-(int i)	const
{
	return _tagVector4(x - i, y - i, z - i, w - i);
}

_tagVector4 _tagVector4::operator-()
{
	*this *= -1;
	return *this;
}

// ==============  operator -=  =================
_tagVector4& _tagVector4::operator-=(const _tagVector4 & _v)
{
	x -= _v.x;
	y -= _v.y;
	z -= _v.z;
	w -= _v.w;
	return *this;
}

_tagVector4& _tagVector4::operator-=(const XMVECTOR & _v)
{
	Convert(Convert() - _v);
	return *this;
}

_tagVector4& _tagVector4::operator-=(const float f[4])
{
	x -= f[0];
	y -= f[1];
	z -= f[2];
	w -= f[3];
	return *this;
}

_tagVector4& _tagVector4::operator-=(const int i[4])
{
	x -= i[0];
	y -= i[1];
	z -= i[2];
	w -= i[3];
	return *this;
}

_tagVector4& _tagVector4::operator-=(float f)
{
	x -= f;
	y -= f;
	z -= f;
	w -= f;
	return *this;
}

_tagVector4& _tagVector4::operator-=(int i)
{
	x -= i;
	y -= i;
	z -= i;
	w -= i;
	return *this;
}


// ==============  operator *  =================
_tagVector4 _tagVector4::operator*(const _tagVector4 & _v)	const
{
	/*_tagVector4	v;
	v.Convert(Convert() * _v.Convert());
	return v;*/
	return _tagVector4(x * _v.x, y * _v.y, z * _v.z, w * _v.w);
}

_tagVector4 _tagVector4::operator*(const XMVECTOR & _v)	const
{
	return _tagVector4(Convert() * _v);
}

_tagVector4 _tagVector4::operator*(const float f[4])	const
{
	return _tagVector4(x * f[0], y * f[1], z * f[2], w * f[3]);
}

_tagVector4 _tagVector4::operator*(const int i[4])	const
{
	return _tagVector4(x * i[0], y * i[1], z * i[2], w * i[3]);
}

_tagVector4 _tagVector4::operator*(float f)	const
{
	return _tagVector4(x * f, y * f, z * f, w * f);
}

_tagVector4 _tagVector4::operator*(int i)	const
{
	return _tagVector4(x * i, y * i, z * i, w * i);
}

// ==============  operator *=  =================
_tagVector4& _tagVector4::operator*=(const _tagVector4 & _v)
{
	x *= _v.x;
	y *= _v.y;
	z *= _v.z;
	w *= _v.w;
	return *this;
}

_tagVector4& _tagVector4::operator*=(const XMVECTOR & _v)
{
	Convert(Convert() * _v);
	return *this;
}

_tagVector4& _tagVector4::operator*=(const float f[4])
{
	x *= f[0];
	y *= f[1];
	z *= f[2];
	w *= f[3];
	return *this;
}

_tagVector4& _tagVector4::operator*=(const int i[4])
{
	x *= i[0];
	y *= i[1];
	z *= i[2];
	w *= i[3];
	return *this;
}

_tagVector4& _tagVector4::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;
	w *= f;
	return *this;
}

_tagVector4& _tagVector4::operator*=(int i)
{
	x *= i;
	y *= i;
	z *= i;
	w *= i;
	return *this;
}


// ==============  operator /  =================
_tagVector4 _tagVector4::operator/(const _tagVector4 & _v)	const
{
	/*_tagVector4	v;
	v.Convert(Convert() / _v.Convert());
	return v;*/
	return _tagVector4(x / _v.x, y / _v.y, z / _v.z, w / _v.w);
}

_tagVector4 _tagVector4::operator/(const XMVECTOR & _v)	const
{
	return _tagVector4(Convert() / _v);
}

_tagVector4 _tagVector4::operator/(const float f[4])	const
{
	return _tagVector4(x / f[0], y / f[1], z / f[2], w / f[3]);
}

_tagVector4 _tagVector4::operator/(const int i[4])	const
{
	return _tagVector4(x / i[0], y / i[1], z / i[2], w / i[3]);
}

_tagVector4 _tagVector4::operator/(float f)	const
{
	return _tagVector4(x / f, y / f, z / f, w / f);
}

_tagVector4 _tagVector4::operator/(int i)	const
{
	return _tagVector4(x / i, y / i, z / i, w / i);
}

// ==============  operator /=  =================
_tagVector4& _tagVector4::operator/=(const _tagVector4 & _v)
{
	x /= _v.x;
	y /= _v.y;
	z /= _v.z;
	w /= _v.w;
	return *this;
}

_tagVector4& _tagVector4::operator/=(const XMVECTOR & _v)
{
	Convert(Convert() / _v);
	return *this;
}

_tagVector4& _tagVector4::operator/=(const float f[4])
{
	x /= f[0];
	y /= f[1];
	z /= f[2];
	w /= f[3];
	return *this;
}

_tagVector4& _tagVector4::operator/=(const int i[4])
{
	x /= i[0];
	y /= i[1];
	z /= i[2];
	w /= i[3];
	return *this;
}

_tagVector4& _tagVector4::operator/=(float f)
{
	x /= f;
	y /= f;
	z /= f;
	w /= f;
	return *this;
}

_tagVector4& _tagVector4::operator/=(int i)
{
	x /= i;
	y /= i;
	z /= i;
	w /= i;
	return *this;
}

_tagVector4 & _tagVector4::operator++()
{
	++x;
	++y;
	++z;
	++w;
	return *this;
}

_tagVector4 & _tagVector4::operator--()
{
	--x;
	--y;
	--z;
	--w;
	return *this;
}

bool _tagVector4::operator==(const _tagVector4 & _v) const
{
	return x == _v.x && y == _v.y && z == _v.z && w == _v.w;
}

bool _tagVector4::operator==(const XMVECTOR & _v) const
{
	_tagVector4	v;
	v.Convert(_v);
	return x == v.x && y == v.y && z == v.z && w == v.w;
}

bool _tagVector4::operator!=(const _tagVector4 & _v) const
{
	return x != _v.x || y != _v.y || z != _v.z || w != _v.w;
}

bool _tagVector4::operator!=(const XMVECTOR & _v) const
{
	_tagVector4	v;
	v.Convert(_v);
	return x != v.x || y != v.y || z != v.z || w != v.w;
}

float _tagVector4::Length() const
{
	return sqrtf(x * x + y * y + z * z);
}

_tagVector4 _tagVector4::Normalize()	const
{
	_tagVector4	v;
	v.Convert(XMVector4Normalize(Convert()));
	return v;
}

void _tagVector4::normalize()
{
	*this = XMVector4Normalize(Convert());
}

float _tagVector4::Dot(const _tagVector4 & _v) const
{
	return x * _v.x + y * _v.y + z * _v.z + w * _v.w;
}

float _tagVector4::Dot(const XMVECTOR & _v) const
{
	_tagVector4	v;
	v.Convert(XMVector4Dot(Convert(), _v));
	return v.x;
}

float _tagVector4::Angle(const _tagVector4 & _v) const
{
	_tagVector4	v = Normalize();
	float	fAngle = v.Dot(_v.Normalize());
	return acosf(fAngle);
}

float _tagVector4::Angle(const XMVECTOR & _v) const
{
	_tagVector4	v = Normalize();
	_tagVector4	v1(_v);
	float	fAngle = v.Dot(v1.Normalize());
	return acosf(fAngle);
}

_tagVector3 _tagVector4::EulerAngles(_tagVector4 vQuaternion)
{
	return CMathf::QuaternionToEulerAngle(vQuaternion);
}

_tagVector3 _tagVector4::EulerAngles()
{
	return CMathf::QuaternionToEulerAngle(*this);
}
