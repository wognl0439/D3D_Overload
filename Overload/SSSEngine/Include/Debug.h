#pragma once

#include "Engine.h"


SSS_BEGIN


typedef struct _tagPerforanceProfileInfo
{
	string strInternalScopeName;
	long long iTimeStep;
	float fLocalRate;

	_tagPerforanceProfileInfo(): 
		iTimeStep(0),
		fLocalRate(0.0f)
	{};

}PROFILEINFO, *PPROFILEINFO;

typedef struct _tagProfile
{
	string strExternalScopeName;
	long long iStartFrequency;
	long long iEndFrequency;
	vector<PROFILEINFO> vecInfo;	

	_tagProfile():
		iStartFrequency(0),
		iEndFrequency(0)
	{
	};

}PROFILE, *PPROFILE;

typedef struct _tagProfileRecord
{
	size_t iRecordCounter;
	vector<PROFILE> vecProfile;
}PROFILERECORD, *PPROFILERECORD;


typedef struct _tagReport
{
	size_t iRecordedFrameCount = 0;
	size_t iRecordedLocalCount = 0;
	long long iTotalFrequencyLength = 0;
	vector<string> vecScopeName;
	vector<long long> vecFrequency;
	vector<double> vecLocalRate;
}REPORT, *PREPORT;


class SSS_DLL CDebug
{
	DECLARE_SINGLE(CDebug)
	
private:
	bool m_bUseConsole;

	vector<class CMesh*> m_vecLineMesh;
	vector<class CMesh*> m_vecRayMesh;
	vector<PLINE> m_vecLine;
	vector<PRAY> m_vecRay;

	IDWriteTextFormat*	m_pDebugFPSTextFormat;
	IDWriteTextFormat*	m_pDebugTextFormat;
	ID2D1SolidColorBrush*	m_pDebugFPSBrush;
	ID2D1SolidColorBrush*	m_pDebugBrush;

	class CShader* m_pShader ;
	ID3D11InputLayout*	m_pInputLayout;
	class CRenderState*	m_pRenderState[RS_END];
	class CSampler*			m_pSampler;
	ID3D11Debug*			m_pDX11Debug;
	
	//Key : ExternalScopeName
	unordered_map<string, PROFILE> m_mapProfile;
	//Key : ExternalScopeName
	unordered_map<string, PROFILERECORD> m_mapProfileRecord;

public:
	void UseConsole(bool bUse);
	void ReportLiveDeviceObjects();
	void StartReport(const string& strExternalScopeName);
	void CheckReport(const string& strExternalScopeName, const string& strInternalScopeName);
	void EndReport(const string& strExternalScopeName);

	//프로파일링 리포트는 DataPath에 저장 됩니다.
	void SaveReport();

private:



private:
	void SetRenderState(const string & strKey);
	class CMesh* CreateLineMesh(PLINE pLine);
	class CMesh* CreateRayMesh(PRAY pRay);
	void Clear();
	void UpdateLineTransform(size_t idx);
	void UpdateRayTransform(size_t idx);


public:
	bool Initialize();

public:
	void DrawLine(vector<Vector3>& vecPoints, const Vector4& vColor);
	void DrawLine(const Vector3& vBegin, const Vector3& vEnd, const Vector4& vColor);
	void DrawLine(LINE* pLine);
	void DrawRay(RAY* pRay);
	void Render(float fTime);
	void Log(const string& strLog);
	void Log(const wstring& strLog);


};

SSS_END

