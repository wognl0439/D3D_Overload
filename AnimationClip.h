#pragma once
#include "Engine.h"
#include "Mesh.h"

SSS_BEGIN

typedef struct SSS_DLL _tagAnimationClip2D
{
	class CTexture* pTexture;
	string strAtlasKey;
}ANIMATIONCLIP2D, *PANIMATIONCLIP2D;

class SSS_DLL CAnimationClip
{
private:
	friend class CAnimationState;
	friend class CAnimator;

private:
	CAnimationClip();
	CAnimationClip(const CAnimationClip& clip);
	~CAnimationClip();

private:
	class CAnimationState* m_pAnimationState;
	class CAnimationStateController* m_pAnimationController;
	class CAnimator* m_pAnimator;
		
private:
	vector<PANIMATIONCLIP2D> m_vecKeyFrame2D;
	vector<PBONEKEYFRAME>		m_vecKeyFrame;
	vector<PBONEKEYFRAME>		m_vecCurrentFrame;

	ANIMATIONCLIP m_tClipInfo;

	vector<KEYFRAMEBUFFER>		m_vecKeyFrameBuffer;
	
	float		m_fTime;
	size_t	m_iFrameRate;
	size_t	m_iTotalFrameCount;
	size_t	m_iCurrentFrameIndex;

	float	m_fProgress;
	bool m_bOnExit;

private:
	void ComputeKeyFrame2D();

		
public:
	float GetIntervalToNextFrame();
	float GetProgressToNextFrame();


	void SetProgressOffset(float fProgress);
	float GetProgressAtTime(float fTime) const;
	int GetCurrentKeyFrameIndex();
	int GetNextKeyFrameIndex();
	int GetKeyFrameIndex(float fProgress);
	float GetProgressAtIndex(int iIndex) const;
	vector<KEYFRAMEBUFFER>& GetCurrentKeyFrame();
	vector<KEYFRAMEBUFFER>& GetNextKeyFrame();
	vector<KEYFRAMEBUFFER>& GetKeyFrame(float fProgress);
	KEYFRAMEBUFFER2D GetCurrentKeyFrame2D();
	KEYFRAMEBUFFER2D GetNextKeyFrame2D();

	size_t GetFrameRate() const;
	float GetProgress() const;
	const string& GetClipName() const;
	size_t GetClipLength() const;
	PKEYFRAME GetKeyFrame(int iBone, int iFrame)	const;

	//2D
	void SetKeyFrame(size_t iFrameIndex, const string& strAtlasMetadataKey, const string& strTextureKey, wchar_t* pFileName, const string& strPathKey = TEXTURE_PATH);
	void SetKeyFrame(size_t iFrameIndex, const string& strAtlasMetadataKey, const string& strTextureKey);
	void SetKeyFrame(size_t iFrameIndex, const string& strAtlasMetadataKey);
		
	void SetFrameRate(size_t iFrameRate);
	void SetFrameRate(FbxTime::EMode eMode);
	void SetKeyFrame(PANIMATIONCLIP pClipInfo);
	void SetKeyFrame(const string& strClipName);
	void SetKeyFrame(size_t iClipIndexFromMesh);
	void SetKeyFrame(const string& strClipName, size_t iStartFrame, size_t iEndFrame);
	void SetClipName(const string& strName);
	void ClearClip();
	void ResetClip();
	bool IsEmptyFrame(int iBone)	const;

private:
	bool Update(float fTime);

public:
	void Save(FILE* pFile);
	void Load(FILE* pFile);


};

SSS_END

