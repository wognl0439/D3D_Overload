#pragma once
#include "Macro.h"

SSS_BEGIN

typedef union SSS_DLL _tagMatrix
{

	XMMATRIX	mat;

	struct
	{
		float	_11, _12, _13, _14;
		float	_21, _22, _23, _24;
		float	_31, _32, _33, _34;
		float	_41, _42, _43, _44;
	};

	struct
	{
		float	_m00, _m01, _m02, _m03;
		float	_m10, _m11, _m12, _m13;
		float	_m20, _m21, _m22, _m23;
		float	_m30, _m31, _m32, _m33;
	};

	float fMat[4][4];

	_tagMatrix();
	_tagMatrix(float rc00, float rc01, float rc02, float rc03, 
		float rc10, float rc11, float rc12, float rc13, 
		float rc20, float rc21, float rc22, float rc23,
		float rc30, float rc31, float rc32, float rc33 );
	_tagMatrix(const _tagMatrix& m);
	_tagMatrix(const XMMATRIX& m);
	operator XMMATRIX();
	void* operator new(size_t size);
	void operator delete(void* p);
	void operator =(const XMMATRIX& m);
	void operator =(const _tagMatrix& m);

	void operator =(float f[4][4]);
		
	XMMATRIX Convert()	const;
	void Convert(const XMMATRIX& v);



	XMMATRIX operator *(const _tagMatrix& m);
	XMMATRIX operator *(const XMMATRIX& m);
	XMMATRIX& operator *=(const _tagMatrix& m);
	XMMATRIX& operator *=(const XMMATRIX& m);

	XMMATRIX& Identity();
	XMMATRIX& Transpose();
	XMMATRIX& Inverse();
	
	static XMMATRIX IdentityMatrix();
	static XMMATRIX Transpose(const _tagMatrix& m);
	static XMMATRIX Inverse(const _tagMatrix& m);
	static XMMATRIX Scaling(const struct _tagVector3& vScale);
	static XMMATRIX Translation(const struct _tagVector3& vPos);
	//ºñ½Î´Ù
	static struct _tagVector3 GetEulerAngle(const _tagMatrix& mat);
	static struct _tagVector3 GetTranslation(const _tagMatrix& mat);
	static struct _tagVector4 GetQuaternion(const _tagMatrix& mat);
	static struct _tagVector3 GetScale(const _tagMatrix& mat);
	//ºñ½Î´Ù
	static void Decompose(const _tagMatrix& mat, struct _tagVector3* vOutPosition, struct _tagVector3* vOutEulerAngle, struct _tagVector3* vOutScale);
	static void Decompose(const _tagMatrix& mat, struct _tagVector3* vOutPosition, struct _tagVector4* vOutQuaternion, struct _tagVector3* vOutScale);


}Matrix, *PMatrix;


SSS_END
