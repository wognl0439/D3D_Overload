#pragma once
#include "Engine.h"

SSS_BEGIN
class CMathf
{
	DECLARE_SINGLE(CMathf)
	
public:
	bool Initialize();

public:
	static float RandomBetweenZeroToOne();
	static float RandomRange(float fMin, float fMax);
	static float GaussianDistribution(float fHorizontalPosition, float fSigma , uint32_t iSampleLevel);
	static float RadianToDegree(float fRadian)
	{
		return fRadian * R2D;
	}
	static float DegreeToRadian(float fDegree)
	{
		return fDegree * D2R;
	}
	static float Clamp(float fValue, float fMin, float fMax)
	{
		if (fValue < fMin)
			return fMin;
		if (fValue > fMax)
			return fMax;
		return fValue;
	}
	static float Lerp(float fBegin, float fEnd, float fRate)
	{
		return (1 - fRate) * fBegin + fRate * fEnd;
	}

	static float Slerp(float fBegin, float fEnd, float fRate)
	{
		XMVECTOR temp = XMVectorReplicate(fBegin);
		return 0;
	}

	static _tagVector3 QuaternionToEulerAngle(const _tagVector4& vQuaternion)
	{
		Vector3 vResult;
		// roll (x-axis rotation)
		double sinr = +2.0 * (vQuaternion.w * vQuaternion.x + vQuaternion.y * vQuaternion.z);
		double cosr = +1.0 - 2.0 * (vQuaternion.x * vQuaternion.x + vQuaternion.y * vQuaternion.y);
		vResult.x = atan2(sinr, cosr);

		// pitch (y-axis rotation)
		double sinp = +2.0 * (vQuaternion.w * vQuaternion.y - vQuaternion.z * vQuaternion.x);
		if (fabs(sinp) >= 1)
			vResult.y = copysign(PI / 2, sinp); // use 90 degrees if out of range
		else
			vResult.y = asin(sinp);

		// yaw (z-axis rotation)
		double siny = +2.0 * (vQuaternion.w * vQuaternion.z + vQuaternion.x * vQuaternion.y);
		double cosy = +1.0 - 2.0 * (vQuaternion.y * vQuaternion.y + vQuaternion.z * vQuaternion.z);
		vResult.z = atan2(siny, cosy);

		return vResult;
	}
	static _tagVector4 EulerAngleToQuaternion(const _tagVector3& vEulerAngle)
	{
		return XMQuaternionRotationRollPitchYawFromVector(vEulerAngle.Convert());
	}
};

SSS_END