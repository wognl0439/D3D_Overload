#include "CardinalSpline.h"
#include "../Debug.h"
SSS_USING

CCardinalSpline::CCardinalSpline():
	m_fTension(1.0f),
	m_vColor(Vector4::red)
{
}


CCardinalSpline::CCardinalSpline(const CCardinalSpline & component)
{
}

CCardinalSpline::~CCardinalSpline()
{
}

void CCardinalSpline::DrawSpline()
{
}

Vector3 CCardinalSpline::GetCardinalSplinePosition(float fProgress, float fTension, Vector3 Point1, Vector3 Point2, Vector3 Point3, Vector3 Point4)
{
	Vector3 vResult;
	float fTemp0, fTemp1;

	fTemp0 = (float)(Point3.x - Point1.x) * (1.0f - fTension);
	fTemp1 = (float)(Point4.x - Point2.x) * (1.0f - fTension);

	vResult.x = Hermite(fProgress, Point1.x, Point2.x, fTemp0, fTemp1);

	fTemp0 = (float)(Point3.y - Point1.y) * (1.0f - fTension);
	fTemp1 = (float)(Point4.y - Point2.y) * (1.0f - fTension);

	vResult.y = Hermite(fProgress, Point1.y, Point2.y, fTemp0, fTemp1);

	fTemp0 = (float)(Point3.z - Point1.z) * (1.0f - fTension);
	fTemp1 = (float)(Point4.z - Point2.z) * (1.0f - fTension);

	vResult.z = Hermite(fProgress, Point1.z, Point2.z, fTemp0, fTemp1);

	return vResult;
}

float CCardinalSpline::Hermite(float fProgress, float Point1, float Point2, float Point3, float Point4)
{
	Matrix matrix;
	
	matrix._m00 = 2.0f;      matrix._m01 = -2.0f;     matrix._m02 = 1.0f;       matrix._m03 = 1.0f;
	matrix._m10 = -3.0f;     matrix._m11 = 3.0f;      matrix._m12 = -2.0f;      matrix._m13 = -1.0f;
	matrix._m20 = 0.0f;      matrix._m21 = 0.0f;      matrix._m22 = 1.0f;       matrix._m23 = 0.0f;
	matrix._m30 = 1.0f;      matrix._m31 = 0.0f;      matrix._m32 = 0.0f;        matrix._m33 = 0.0f;

	Vector4 vTempA(fProgress * fProgress * fProgress, fProgress * fProgress, fProgress, 1);
	Vector4 vTempB(Point1, Point2, Point3, Point4);

	
	float fResult = vTempA.Dot(MaxtrixMultiplyVector4(matrix, vTempB));

	return fResult;
}

Vector4 CCardinalSpline::MaxtrixMultiplyVector4(const Matrix& mat, Vector4 vector)
{
	Vector4 TempVec;

	TempVec.x	= mat._m00 * vector.x + mat._m01 * vector.y + mat._m02 * vector.z + mat._m03 * vector.w;
	TempVec.y	= mat._m10 * vector.x + mat._m11 * vector.y + mat._m12 * vector.z + mat._m13 * vector.w;
	TempVec.z	= mat._m20 * vector.x + mat._m21 * vector.y + mat._m22 * vector.z + mat._m23 * vector.w;
	TempVec.w = mat._m30 * vector.x + mat._m31 * vector.y + mat._m32 * vector.z + mat._m33 * vector.w;

	return TempVec;
}

void CCardinalSpline::AddPoint(vector<Vector3>& vecPoints)
{
	size_t iSize = vecPoints.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		m_vecPoint.push_back(vecPoints[i]);
	}
}

void CCardinalSpline::AddPoint(const Vector3 & vPoint)
{
	m_vecPoint.push_back(vPoint);
}

void CCardinalSpline::AddPoint(float fX, float fY, float fZ)
{
	m_vecPoint.push_back(Vector3(fX, fY, fZ));
}

void CCardinalSpline::SetTension(float fTension)
{
	m_fTension = fTension;
}

void CCardinalSpline::SetSegment(uint32_t iSegment)
{
	m_iSegment = iSegment;
}

void CCardinalSpline::SetColor(const Vector4 & vColor)
{
	m_vColor = vColor;
}

bool CCardinalSpline::Initialize()
{
	return true;
}

int CCardinalSpline::Update(float fTime)
{
	uint32_t iMaxCount = m_vecPoint.size();

	Vector3 LastPoint ;

	vector<Vector3> vecLine;
	//더미로 마지막 오브젝트가 존재해야함

	for (int i = 0; i < iMaxCount - 1; i++)
	{
		if (i == 0)
		{
			LastPoint = m_vecPoint[0];

			vecLine.push_back(m_vecPoint[0]);

			for (int j = 0; j < m_iSegment; ++j)
			{
				float fProgress = (float)j / (float)m_iSegment;
				Vector3 DrawPosition = GetCardinalSplinePosition(
					fProgress, m_fTension,
					m_vecPoint[i],
					m_vecPoint[i],
					m_vecPoint[i],
					m_vecPoint[i]);


				vecLine.push_back(DrawPosition);
				//GET_SINGLE(CDebug)->DrawLine(LastPoint, DrawPosition, m_vColor);
				LastPoint = DrawPosition;
			}
		}
		else if (i == iMaxCount - 2)
		{
			for (int j = 0; j < m_iSegment; ++j)
			{
				//어후 길다;;;
				float fProgress = (float)j / (float)m_iSegment;
				Vector3 DrawPosition = GetCardinalSplinePosition(
					fProgress, m_fTension,
					m_vecPoint[i - 1],
					m_vecPoint[i],
					m_vecPoint[i + 1],
					m_vecPoint[i + 1]);
				
				vecLine.push_back(DrawPosition);
				LastPoint = DrawPosition;
			}
		}
		else
		{
			for (int j = 0; j < m_iSegment; ++j)
			{
				float fProgress = (float)j / (float)m_iSegment;
				Vector3 DrawPosition = GetCardinalSplinePosition(
					fProgress, m_fTension,
					m_vecPoint[i - 1],
					m_vecPoint[i],
					m_vecPoint[i + 1],
					m_vecPoint[i + 2]);

				vecLine.push_back(DrawPosition);
				LastPoint = DrawPosition;
			}
		}
	}

	GET_SINGLE(CDebug)->DrawLine(vecLine, m_vColor);

	return 0;
}

bool CCardinalSpline::Save(FILE * pFile)
{
	fwrite(&m_vColor, sizeof(Vector4), 1, pFile);
	size_t iSize = m_vecPoint.size();
	fwrite(&iSize, sizeof(size_t), 1, pFile);
	for (size_t i = 0; i < iSize; ++i)
	{
		fwrite(&m_vecPoint[i], sizeof(Vector3), 1, pFile);
	}
	fwrite(&m_iSegment, sizeof(unsigned int), 1, pFile);
	fwrite(&m_fTension, sizeof(float), 1, pFile);

	return true;
}

bool CCardinalSpline::Load(FILE * pFile)
{
	fread(&m_vColor, sizeof(Vector4), 1, pFile);
	size_t iSize = 0;
	fread(&iSize, sizeof(size_t), 1, pFile);
	for (size_t i = 0; i < iSize; ++i)
	{
		fwrite(&m_vecPoint[i], sizeof(Vector3), 1, pFile);
	}
	fread(&m_iSegment, sizeof(unsigned int), 1, pFile);
	fread(&m_fTension, sizeof(float), 1, pFile);

	return true;
}
