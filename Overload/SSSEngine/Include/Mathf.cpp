#include "Mathf.h"

SSS_USING

DEFINITION_SINGLE(CMathf)

CMathf::CMathf()
{
}


CMathf::~CMathf()
{
}

bool CMathf::Initialize()
{
	return true;
}

float CMathf::RandomBetweenZeroToOne()
{
	int i = rand() % 10001;
	return (float)i * 0.0001f;
}

float CMathf::RandomRange(float fMin, float fMax)
{
	float fRange = fMax - fMin;
	float fValue = RandomBetweenZeroToOne() * fRange;
	fValue += fMin;

	return fValue;
}

float CMathf::GaussianDistribution(float fHorizontalPosition, float fSigma, uint32_t iSampleLevel)
{
	return expf(-1.0f * ((iSampleLevel * iSampleLevel) / (2 * fSigma * fSigma)));
}





