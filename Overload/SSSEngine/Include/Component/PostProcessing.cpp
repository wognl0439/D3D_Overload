	#include "PostProcessing.h"
#include "../ShaderManager.h"
#include "../RenderManager.h"


SSS_USING

CPostProcessing::CPostProcessing() :
	m_iPostProcessFlags(0),
	m_fHDRExposure(0.425461411f),
	m_fHDRWhiteThreshold(9.70112610f),
	m_fHDRPrevAverageLuminance(0.5f),
	m_fHDRAdaptionInterval(1.0f),
	m_fHDRAverageLuminance(0.0f)
{
}

CPostProcessing::~CPostProcessing()
{
	//GET_SINGLE(CRenderManager)->SetPostProcessing(NULL);
}

void CPostProcessing::SetFogColor(const Vector4 & vColor)
{
	m_vFogColor = vColor;
}

void CPostProcessing::SetFogDistance(float fMinDistance, float fMaxDistance)
{
	m_fFogMinDistance = fMinDistance;
	m_fFogMaxDistance = fMaxDistance;
}

Vector4 CPostProcessing::GetFogColor() const
{
	return m_vFogColor;
}

float CPostProcessing::GetFogMinDistance() const
{
	return m_fFogMinDistance;
}

float CPostProcessing::GetFogMaxDistance() const
{
	return m_fFogMaxDistance;
}

Vector2 CPostProcessing::GetFogDistance() const
{
	return Vector2(m_fFogMinDistance, m_fFogMaxDistance);
}

void CPostProcessing::SetPostProcessingFlags(uint32_t iTypes)
{
	m_iPostProcessFlags = iTypes;
}

void CPostProcessing::ToggleFlags(uint32_t iTypes)
{
	m_iPostProcessFlags ^= iTypes;
}

uint32_t CPostProcessing::GetPostProcessingFlags() const
{
	return m_iPostProcessFlags;
}

void CPostProcessing::SetBlurSampleCount(size_t iCount)
{
	m_iBlurSampleCount = iCount;
}

size_t CPostProcessing::GetBlurSampleCount() const
{
	return m_iBlurSampleCount;
}

void CPostProcessing::SetDepthOfField(float fMinRange, float fMaxRange, float fMaxSigma)
{
	m_fDepthOfFieldMinRange = fMinRange;
	m_fDepthOfFieldMaxRange = fMaxRange;
	m_fDepthOfFieldMaxSigma = fMaxSigma;
}

float CPostProcessing::GetDepthOfFieldMinRange() const
{
	return m_fDepthOfFieldMinRange;
}

float CPostProcessing::GetDepthOfFieldMaxRange() const
{
	return m_fDepthOfFieldMaxRange;
}

float CPostProcessing::GetDepthOfFieldMaxSigma() const
{
	return m_fDepthOfFieldMaxSigma;
}

void CPostProcessing::SetAdaptionInterval(float fInterval)
{
	m_fHDRAdaptionInterval = fInterval;
}

void CPostProcessing::SetHDRExposure(float fExposure)
{
	m_fHDRExposure = fExposure;
}

void CPostProcessing::SetHDRWhiteThreshold(float fWhiteThreshold)
{
	m_fHDRWhiteThreshold = fWhiteThreshold;
}

float CPostProcessing::GetAdaptedLuminance(float fDeltaTime)
{
	m_fHDRAverageLuminance = Lerp(m_fHDRPrevAverageLuminance, m_fHDRAverageLuminance, fDeltaTime );

	return m_fHDRAverageLuminance;
}

float CPostProcessing::GetHDRExposure() const
{
	return m_fHDRExposure;
}

float CPostProcessing::GetHDRWhiteThreshold() const
{
	return m_fHDRWhiteThreshold;
}

bool CPostProcessing::Initialize()
{
	return GET_SINGLE(CRenderManager)->SetPostProcessing(this);
}

bool CPostProcessing::Save(FILE * pFile)
{
	// Flags
	fwrite(&m_iPostProcessFlags, sizeof(uint32_t), 1, pFile);
	// Fog
	fwrite(&m_fFogMinDistance, sizeof(float), 1, pFile);
	fwrite(&m_fFogMaxDistance, sizeof(float), 1, pFile);
	fwrite(&m_vFogColor, sizeof(Vector4), 1, pFile);
	// Blur
	fwrite(&m_iBlurSampleCount, sizeof(uint32_t), 1, pFile);
	// Depth of Field
	fwrite(&m_fDepthOfFieldMinRange, sizeof(float), 1, pFile);
	fwrite(&m_fDepthOfFieldMaxRange, sizeof(float), 1, pFile);
	fwrite(&m_fDepthOfFieldMaxSigma, sizeof(float), 1, pFile);
	// HDR
	fwrite(&m_fHDRExposure, sizeof(float), 1, pFile);
	fwrite(&m_fHDRWhiteThreshold, sizeof(float), 1, pFile);
	fwrite(&m_fHDRAverageLuminance, sizeof(float), 1, pFile);
	fwrite(&m_fHDRPrevAverageLuminance, sizeof(float), 1, pFile);
	fwrite(&m_fHDRAdaptionInterval, sizeof(float), 1, pFile);

	return true;
}

bool CPostProcessing::Load(FILE * pFile)
{
	// Flags
	fread(&m_iPostProcessFlags, sizeof(uint32_t), 1, pFile);
	// Fog
	fread(&m_fFogMinDistance, sizeof(float), 1, pFile);
	fread(&m_fFogMaxDistance, sizeof(float), 1, pFile);
	fread(&m_vFogColor, sizeof(Vector4), 1, pFile);
	// Blur
	fread(&m_iBlurSampleCount, sizeof(uint32_t), 1, pFile);
	// Depth of Field
	fread(&m_fDepthOfFieldMinRange, sizeof(float), 1, pFile);
	fread(&m_fDepthOfFieldMaxRange, sizeof(float), 1, pFile);
	fread(&m_fDepthOfFieldMaxSigma, sizeof(float), 1, pFile);
	// HDR
	fread(&m_fHDRExposure, sizeof(float), 1, pFile);
	fread(&m_fHDRWhiteThreshold, sizeof(float), 1, pFile);
	fread(&m_fHDRAverageLuminance, sizeof(float), 1, pFile);
	fread(&m_fHDRPrevAverageLuminance, sizeof(float), 1, pFile);
	fread(&m_fHDRAdaptionInterval, sizeof(float), 1, pFile);

	return true;
}
