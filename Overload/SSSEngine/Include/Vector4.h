#pragma once
#include "Macro.h"
#include "Vector3.h"

SSS_BEGIN


typedef struct SSS_DLL _tagVector4
{
	float x, y, z, w;

	_tagVector4();
	_tagVector4(float _x, float _y, float _z, float _w);
	_tagVector4(const _tagVector4& _v);
	_tagVector4(const float f[4]);
	_tagVector4(const int i[4]);
	_tagVector4(const XMVECTOR& _v);

	XMVECTOR Convert()	const;
	void Convert(const XMVECTOR& v);

	// ==============  operator =  =================
	void operator =(const _tagVector4& _v);
	void operator =(const XMVECTOR& _v);
	void operator =(const float f[4]);
	void operator =(const int i[4]);

	// ==============  operator +  =================
	_tagVector4 operator +(const _tagVector4& _v)	const;
	_tagVector4 operator +(const XMVECTOR& _v)	const;
	_tagVector4 operator +(const float f[4])	const;
	_tagVector4 operator +(const int i[4])	const;
	_tagVector4 operator +(float f)	const;
	_tagVector4 operator +(int i)	const;

	// ==============  operator +=  =================
	_tagVector4& operator +=(const _tagVector4& _v);
	_tagVector4& operator +=(const XMVECTOR& _v);
	_tagVector4& operator +=(const float f[4]);
	_tagVector4& operator +=(const int i[4]);
	_tagVector4& operator +=(float f);
	_tagVector4& operator +=(int i);

	// ==============  operator -  =================
	_tagVector4 operator -(const _tagVector4& _v)	const;
	_tagVector4 operator -(const XMVECTOR& _v)	const;
	_tagVector4 operator -(const float f[4])	const;
	_tagVector4 operator -(const int i[4])	const;
	_tagVector4 operator -(float f)	const;
	_tagVector4 operator -(int i)	const;

	// ==============  operator -=  =================
	_tagVector4 operator -();
	_tagVector4& operator -=(const _tagVector4& _v);
	_tagVector4& operator -=(const XMVECTOR& _v);
	_tagVector4& operator -=(const float f[4]);
	_tagVector4& operator -=(const int i[4]);
	_tagVector4& operator -=(float f);
	_tagVector4& operator -=(int i);


	// ==============  operator *  =================
	_tagVector4 operator *(const _tagVector4& _v)	const;
	_tagVector4 operator *(const XMVECTOR& _v)	const;
	_tagVector4 operator *(const float f[4])	const;
	_tagVector4 operator *(const int i[4])	const;
	_tagVector4 operator *(float f)	const;
	_tagVector4 operator *(int i)	const;

	// ==============  operator *=  =================
	_tagVector4& operator *=(const _tagVector4& _v);
	_tagVector4& operator *=(const XMVECTOR& _v);
	_tagVector4& operator *=(const float f[4]);
	_tagVector4& operator *=(const int i[4]);
	_tagVector4& operator *=(float f);
	_tagVector4& operator *=(int i);


	// ==============  operator /  =================
	_tagVector4 operator /(const _tagVector4& _v)	const;
	_tagVector4 operator /(const XMVECTOR& _v)	const;
	_tagVector4 operator /(const float f[4])	const;
	_tagVector4 operator /(const int i[4])	const;
	_tagVector4 operator /(float f)	const;
	_tagVector4 operator /(int i)	const;

	// ==============  operator /=  =================
	_tagVector4& operator /=(const _tagVector4& _v);
	_tagVector4& operator /=(const XMVECTOR& _v);
	_tagVector4& operator /=(const float f[4]);
	_tagVector4& operator /=(const int i[4]);
	_tagVector4& operator /=(float f);
	_tagVector4& operator /=(int i);

	// ==============  operator ++, --  =================
	_tagVector4& operator ++();
	_tagVector4& operator --();

	bool operator ==(const _tagVector4& _v)	const;
	bool operator ==(const XMVECTOR& _v)	const;
	bool operator !=(const _tagVector4& _v)	const;
	bool operator !=(const XMVECTOR& _v)	const;

	// =============== Conversion ================
	operator XMVECTOR ()
	{
		return Convert();
	}	
	// =============== º¤ÅÍ °ü·Ã ÇÔ¼öµé ================
	
	float Length()	const;
	_tagVector4 Normalize()	const;
	void normalize();
	float Dot(const _tagVector4& _v)	const;
	float Dot(const XMVECTOR& _v)	const;
	float Angle(const _tagVector4& _v)	const;
	float Angle(const XMVECTOR& _v)	const;

	//ºñ½Î´Ù
	static struct _tagVector3 EulerAngles(_tagVector4 vQuaternion);
	//ºñ½Î´Ù
	struct _tagVector3 EulerAngles();


	static _tagVector4 clear;
	static _tagVector4 white;
	static _tagVector4 black;
	static _tagVector4 red;
	static _tagVector4 blue;
	static _tagVector4 green;
	static _tagVector4 yellow;
	static _tagVector4 magenta;
	static _tagVector4 cyan;

	static _tagVector4 hotpink; 

}Vector4, Color, Quaternion, *PVector4, *PColor, *PQuaternion;

SSS_END

