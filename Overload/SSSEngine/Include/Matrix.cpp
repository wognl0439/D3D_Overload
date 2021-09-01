#include "Matrix.h"
#include "Vector3.h"

SSS_USING

_tagMatrix::_tagMatrix()
{
	// XMMatrixIdentity() : 항등행렬 (E)을 만들어주는 함수이다.
	mat = XMMatrixIdentity();
}

_tagMatrix::_tagMatrix(float rc00, float rc01, float rc02, float rc03, float rc10, float rc11, float rc12, float rc13, float rc20, float rc21, float rc22, float rc23, float rc30, float rc31, float rc32, float rc33)
{
	_m00 = rc00;
	_m01 = rc01;
	_m02 = rc02;
	_m03 = rc03;

	_m10 = rc10;
	_m11 = rc11;
	_m12 = rc12;
	_m13 = rc13;

	_m20 = rc20;
	_m21 = rc21;
	_m22 = rc22;
	_m23 = rc23;

	_m30 = rc30;
	_m31 = rc31;
	_m32 = rc32;
	_m33 = rc33;
}

_tagMatrix::_tagMatrix(const _tagMatrix & m)
{
	*this = m;
}
_tagMatrix::_tagMatrix(const XMMATRIX & m)
{
	mat = m;
}

_tagMatrix::operator XMMATRIX()
{
	return mat;
}

void * _tagMatrix::operator new(size_t size)
{
	_tagMatrix* pMatrix = (_tagMatrix*)_aligned_malloc(size, 16);

	pMatrix->mat = XMMatrixIdentity();

	return pMatrix;
}

void _tagMatrix::operator delete(void * p)
{
	_aligned_free(p);
}

void _tagMatrix::operator=(const XMMATRIX & m)
{
	mat = m;
}

void _tagMatrix::operator=(const _tagMatrix & m)
{
	mat = m.mat;
}

void _tagMatrix::operator=(float f[4][4])
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			fMat[i][j] = f[i][j];
		}
	}
}

XMMATRIX _tagMatrix::Convert() const
{
	return XMLoadFloat4x4((XMFLOAT4X4*)this);
}

void _tagMatrix::Convert(const XMMATRIX & v)
{
	XMStoreFloat4x4((XMFLOAT4X4*)this, v);
}

XMMATRIX _tagMatrix::operator*(const _tagMatrix & m)
{
	return mat * m.mat;
}

XMMATRIX _tagMatrix::operator*(const XMMATRIX & m)
{
	return mat * m;
}

XMMATRIX & _tagMatrix::operator*=(const _tagMatrix & m)
{
	*this = mat * m.mat;
	return mat;
}

XMMATRIX & _tagMatrix::operator*=(const XMMATRIX & m)
{
	*this = mat * m;
	return mat;
}

XMMATRIX& _tagMatrix::Identity()
{
	return XMMatrixIdentity();
}

XMMATRIX& _tagMatrix::Transpose()
{
	mat = XMMatrixTranspose(mat);
	return mat;
}

XMMATRIX & _tagMatrix::Inverse()
{
	mat = XMMatrixInverse(&XMMatrixDeterminant(mat), mat);
	return mat;
}

XMMATRIX _tagMatrix::IdentityMatrix()
{
	return XMMatrixIdentity();
}

XMMATRIX _tagMatrix::Transpose(const _tagMatrix & m)
{
	return XMMatrixTranspose(m.mat);
}

XMMATRIX _tagMatrix::Inverse(const _tagMatrix & m)
{
	return XMMatrixInverse(&XMMatrixDeterminant(m.mat), m.mat);
}

XMMATRIX _tagMatrix::Scaling(const _tagVector3 & vScale)
{
	return XMMatrixScaling(vScale.x, vScale.y, vScale.z);
}

XMMATRIX _tagMatrix::Translation(const _tagVector3 & vPos)
{
	return XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
}

Vector3 _tagMatrix::GetTranslation(const _tagMatrix & mat)
{
	XMVECTOR vOutput;
	XMMatrixDecompose(NULL, NULL, &vOutput, mat.Convert());
	return vOutput;
}

//비싸다
Vector3 _tagMatrix::GetEulerAngle(const _tagMatrix & mat)
{
	XMVECTOR vOutput;
	XMMatrixDecompose(NULL, &vOutput, NULL, mat.Convert());
	return ((Vector4)vOutput).EulerAngles();
}

Vector4 _tagMatrix::GetQuaternion(const _tagMatrix & mat)
{
	XMVECTOR vOutput;
	XMMatrixDecompose(NULL, &vOutput, NULL, mat.Convert());
	return (Vector4)vOutput;
}

Vector3 _tagMatrix::GetScale(const _tagMatrix & mat)
{
	XMVECTOR vOutput;
	XMMatrixDecompose(&vOutput, NULL,  NULL, mat.Convert());
	return (Vector4)vOutput;
}

void _tagMatrix::Decompose(const _tagMatrix & mat, _tagVector3 * vOutPosition, _tagVector3 * vOutEulerAngle, _tagVector3 * vOutScale)
{
	XMVECTOR vQuaternion;
	XMVECTOR vScale;
	XMVECTOR vPosition;
	XMMatrixDecompose(&vScale, &vQuaternion, &vPosition, mat.Convert());
	*vOutScale = vScale;
	*vOutEulerAngle = ((Vector4)vQuaternion).EulerAngles();
	*vOutPosition = vPosition;
}

void _tagMatrix::Decompose(const _tagMatrix & mat, _tagVector3 * vOutPosition, _tagVector4 * vOutQuaternion, _tagVector3 * vOutScale)
{
	XMVECTOR vQuaternion;
	XMVECTOR vScale;
	XMVECTOR vPosition;
	XMMatrixDecompose(&vScale, &vQuaternion, &vPosition, mat.Convert());
	*vOutScale = vScale;
	*vOutQuaternion = vQuaternion;
	*vOutPosition = vPosition;
}
