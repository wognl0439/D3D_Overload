#pragma once
#include "../Component.h"

SSS_BEGIN

enum POSTPROCESS_TYPE
{
	PT_NONE = 0x0000,
	PT_ANTIALIASING = 0x0001,
	PT_EYE_ADAPTION = 0x0002,
	PT_BLOOM = 0x0004,
	PT_FOG = 0x0008,
	PT_HDR = 0x0010,
	PT_MOTION_BLUR = 0x0020,
	PT_AMBIENT_OCCLUSION = 0x0040,
	PT_DEPTH_OF_FIELD = 0x0080,
};

class SSS_DLL CPostProcessing :
	public CComponent
{
private:
	friend class CGameObject;
	friend class CRenderManager;

private:
	CPostProcessing();
	~CPostProcessing();


#pragma region Post-Processing Properties

private:
	uint32_t m_iPostProcessFlags;
	
public:
	/*
	작성 : 김은수
	일자 : 2018. 08. 08
	요약 : 처리할 효과들을 정해줍니다.
	내용 : POSTPROCESS_TYPE 에 정의되어 있는 값들을 or 연산으로 넣어줄 수 있도록 합니다.
	반환 :
	비고 : RenderManager에서 일괄 처리됩니다.
	*/
	void SetPostProcessingFlags(uint32_t iTypes);
	void ToggleFlags(uint32_t iTypes);
	uint32_t GetPostProcessingFlags() const;
#pragma endregion



#pragma region Fog
private:
	float m_fFogMinDistance;
	float m_fFogMaxDistance;
	Vector4 m_vFogColor;

public:
	void SetFogColor(const Vector4& vColor);
	void SetFogDistance(float fMinDistance, float fMaxDistance);
	Vector4 GetFogColor() const;
	float GetFogMinDistance() const;
	float GetFogMaxDistance() const;
	/*
	작성 : 김은수
	일자 : 2018. 08. 08
	요약 : Fog의 Min, Max 거리를 얻어옵니다.
	반환 : x는 Min, y는 Max
	*/
	Vector2 GetFogDistance() const;
#pragma endregion


#pragma region Blur
private:
	uint32_t m_iBlurSampleCount;

public:
	void SetBlurSampleCount(size_t iCount);
	size_t GetBlurSampleCount() const;
#pragma endregion

#pragma region Depth of Field

private:
	float m_fDepthOfFieldMinRange;
	float m_fDepthOfFieldMaxRange;
	float m_fDepthOfFieldMaxSigma;

public:
	void SetDepthOfField(float fMinRange, float fMaxRange, float fMaxSigma);
	float GetDepthOfFieldMinRange() const;
	float GetDepthOfFieldMaxRange() const;
	float GetDepthOfFieldMaxSigma() const;
#pragma endregion


#pragma region HDR
private:
	float m_fHDRExposure;
	float m_fHDRWhiteThreshold;
	float m_fHDRAverageLuminance;
	float m_fHDRPrevAverageLuminance;
	float m_fHDRAdaptionInterval;


public:
	void SetAdaptionInterval(float fInterval);
	void SetHDRExposure(float fExposure);
	void SetHDRWhiteThreshold(float fWhiteThreshold);
	float GetAdaptedLuminance(float fDeltaTime);
	float GetHDRExposure() const;
	float GetHDRWhiteThreshold() const;
#pragma endregion


public:
	bool Initialize() override;

	bool Save(FILE* pFile) override;
	bool Load(FILE* pFile) override;
};

SSS_END