#pragma once
#include "../Component.h"

SSS_BEGIN

class SSS_DLL CCardinalSpline :
	public CComponent
{
private:
	friend class CGameObject;
	
	CCardinalSpline();
	CCardinalSpline(const CCardinalSpline& component);
	~CCardinalSpline();


private:
	Vector4 m_vColor;
	vector<Vector3> m_vecPoint;
	unsigned int m_iSegment;
	float m_fTension;

private:
	void DrawSpline();
	Vector3 GetCardinalSplinePosition(float fProgress, float fTension, Vector3 Point1, Vector3 Point2, Vector3 Point3, Vector3 Point4);
	float Hermite(float fProgress, float Point1, float Point2, float Point3, float Point4);
	Vector4 MaxtrixMultiplyVector4(const Matrix& mat, Vector4 vector);

public:
	void AddPoint(vector<Vector3>& vecPoints);
	void AddPoint(const Vector3& vPoint);
	void AddPoint(float fX, float fY, float fZ);
	void SetTension(float fTension);
	void SetSegment(uint32_t iSegment);
	void SetColor(const Vector4& vColor);

public:
	bool Initialize() override;
	int Update(float fTime) override;
	
	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END