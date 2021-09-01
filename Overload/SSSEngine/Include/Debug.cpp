#include "Debug.h"
#include "Core.h"
#include "Device.h"
#include "Mesh.h"
#include "ResourcesManager.h"
#include "RenderManager.h"
#include "RenderState.h"
#include "Component\Camera.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ShaderManager.h"
#include "Shader.h"
#include "Sampler.h"
#include "Timer.h"
#include "PathManager.h"
#include "TimerManager.h"
#include "FontManager.h"



SSS_USING

DEFINITION_SINGLE(CDebug)


CDebug::CDebug():
	m_pInputLayout(NULL),
	m_bUseConsole(false)
{
	m_pDX11Debug = NULL;
	m_pDebugTextFormat = NULL;
	m_pDebugBrush = NULL;


	m_pDebugTextFormat = GET_SINGLE(CFontManager)->MakeFont(L"나눔고딕 ExtraBold",
		10.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STRETCH_ULTRA_EXPANDED,
		10.0f, L"ko");

	m_pDebugFPSTextFormat = GET_SINGLE(CFontManager)->MakeFont(L"나눔고딕 ExtraBold",
		30.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STRETCH_ULTRA_EXPANDED,
		30.0f, L"ko");

	m_pDebugBrush = GET_SINGLE(CFontManager)->CreateBrush(Vector4::black);
	m_pDebugFPSBrush = GET_SINGLE(CFontManager)->CreateBrush(Vector4::green);

	memset(m_pRenderState, 0, sizeof(CRenderState*) * 3);

	m_pShader = GET_SINGLE(CShaderManager)->FindShader(COLLIDER_SHADER);
	SetRenderState(ALPHA_BLEND);
	SetRenderState(CULLNONE);
	SetRenderState(DEPTH_DISABLE);
	m_pSampler = GET_SINGLE(CResourcesManager)->FindSampler(SAMPLER_LINEAR);
	m_pInputLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_COLOR_LAYOUT);

	UseConsole(false);
}


CDebug::~CDebug()
{
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pSampler);
	for (int i = 0; i < RS_END; ++i)
	{
		SAFE_RELEASE(m_pRenderState[i]);
	}
	Clear();

	//ReportLiveDeviceObjects();
	if(m_mapProfileRecord.size() > 0)
		SaveReport();

	SAFE_RELEASE(m_pDX11Debug);
	UseConsole(false);
}

void CDebug::UseConsole(bool bUse)
{
	if (m_bUseConsole)
	{
		if (!bUse)
		{
			m_bUseConsole = bUse;
			FreeConsole();
		}
	}
	else
	{
		if (bUse)
		{
			m_bUseConsole = bUse;
			AllocConsole();
		}
	}
}

void CDebug::ReportLiveDeviceObjects()
{
	HRESULT Result = m_pDX11Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
}

void CDebug::StartReport(const string & strExternalScopeName)
{
	unordered_map<string, PROFILE>::iterator iter = m_mapProfile.find(strExternalScopeName);

	if (iter == m_mapProfile.end())
	{
		PROFILE tProfile;
		tProfile.strExternalScopeName = strExternalScopeName;	
		LARGE_INTEGER iTimer;
		QueryPerformanceCounter(&iTimer);
		tProfile.iStartFrequency = iTimer.QuadPart;
		m_mapProfile.insert(make_pair(strExternalScopeName, tProfile));

		PROFILERECORD tRecord;
		tRecord.iRecordCounter = 0;
		m_mapProfileRecord.insert(make_pair(strExternalScopeName, tRecord));
	}
}

void CDebug::CheckReport(const string & strExternalScopeName, const string & strInternalScopeName)
{
	unordered_map<string, PROFILE>::iterator iter = m_mapProfile.find(strExternalScopeName);

	if (iter != m_mapProfile.end())
	{
		PROFILEINFO tInfo;
		LARGE_INTEGER iTimer;
		QueryPerformanceCounter(&iTimer);		
		tInfo.iTimeStep = iTimer.QuadPart;	
		tInfo.strInternalScopeName = strInternalScopeName;
		(*iter).second.vecInfo.push_back(tInfo);
	}

	//ExternalScope가 존재하지 않을 경우 기록하지 않음.	
}

void CDebug::EndReport(const string & strExternalScopeName)
{
	unordered_map<string, PROFILE>::iterator iter = m_mapProfile.find(strExternalScopeName);

	if (iter != m_mapProfile.end())
	{
		LARGE_INTEGER iTimer;
		QueryPerformanceCounter(&iTimer);
		iter->second.iEndFrequency = iTimer.QuadPart;
	
		//기록을 저장하고 비워줌.
		//History의 인덱스는 Tick Count가 됨.
		unordered_map<string, PROFILERECORD>::iterator iterRecord = m_mapProfileRecord.find(strExternalScopeName);
		iterRecord->second.iRecordCounter++;
		iterRecord->second.vecProfile.push_back(iter->second);
		m_mapProfile.erase(iter);
	}			
}

void CDebug::SaveReport()
{
	//Key : ExternalScopeName
	unordered_map<string, REPORT> Archive;
	unordered_map<string, PROFILERECORD>::iterator iterRecord;
	unordered_map<string, PROFILERECORD>::iterator iterRecordEnd = m_mapProfileRecord.end();

	for (iterRecord = m_mapProfileRecord.begin(); iterRecord != iterRecordEnd; ++iterRecord)
	{
		REPORT tBase;
		size_t iCount = iterRecord->second.vecProfile[0].vecInfo.size();
		tBase.iRecordedLocalCount = iCount;
		
		for (size_t i = 0; i < iCount; ++i)
		{
			tBase.vecScopeName.push_back(iterRecord->second.vecProfile[0].vecInfo[i].strInternalScopeName);
			tBase.vecFrequency.push_back(0);
		}
		Archive.insert(make_pair(iterRecord->first, tBase));
	}
	
	for (iterRecord = m_mapProfileRecord.begin(); iterRecord != iterRecordEnd; ++iterRecord)
	{
		//모든 기록을 돌면서 누적한다.
		string strKey = iterRecord->first;
		unordered_map<string, REPORT>::iterator iterArchive = Archive.find(strKey);
		//Archive의 EndFrequency는 전체 측정 클럭 수(Length) 이다.
		if (iterArchive != Archive.end())
		{
			size_t iFrameCount = iterRecord->second.iRecordCounter;
			iterArchive->second.iRecordedFrameCount = iFrameCount;
			for (size_t i = 0; i < iFrameCount; ++i)
			{
				iterArchive->second.iTotalFrequencyLength += iterRecord->second.vecProfile[i].iEndFrequency - iterRecord->second.vecProfile[i].iStartFrequency;

				//매 프레임에 걸리는 총 클럭 수를 누적해준다.

				size_t iLocalScopeCount = iterRecord->second.vecProfile[i].vecInfo.size();
				for (size_t j = 0; j < iLocalScopeCount; ++j)
				{
					if (j == 0)
					{
						long long fAccumulatedFrequency = iterRecord->second.vecProfile[i].vecInfo[j].iTimeStep - iterRecord->second.vecProfile[i].iStartFrequency;
						iterArchive->second.vecFrequency[j] += fAccumulatedFrequency;
					}
					else
					{
						long long fAccumulatedFrequency = iterRecord->second.vecProfile[i].vecInfo[j].iTimeStep - iterRecord->second.vecProfile[i].vecInfo[j - 1].iTimeStep;
						iterArchive->second.vecFrequency[j] += fAccumulatedFrequency;
					}
				}		
			}
		}
	}



	//Achive에 있는 누계된 값들을 나눠준다.	
	unordered_map<string, REPORT>::iterator iter;
	unordered_map<string, REPORT>::iterator iterEnd = Archive.end();
	//모든 External Scope를 돈다.
	for (iter = Archive.begin(); iter != iterEnd; ++iter)
	{
		size_t iLocalScopeCount = iter->second.iRecordedLocalCount;

		for (size_t i = 0; i < iLocalScopeCount; ++i)
		{
			double fLocalRate = (double)iter->second.vecFrequency[i] / (double)iter->second.iTotalFrequencyLength;
			iter->second.vecLocalRate.push_back(fLocalRate);
		}
	}



	FILE* pFile = NULL;



	time_t     now = time(0);
	struct tm  tstruct;
	char       buffer[80];
	tstruct = *localtime(&now);
	strftime(buffer, sizeof(buffer), "%Y%m%d", &tstruct); // YYYY-MM-DD

	string strFileName = GET_SINGLE(CPathManager)->FindPathMultiByte(DATA_PATH);

#ifdef _WIN64
#ifdef _DEBUG
	strFileName += string(buffer) + string("Report_Debug64");
#else
	strFileName += string(buffer) + string("Report_Release64");
#endif // _DEBUG
#else
#ifdef _DEBUG
	strFileName += string(buffer) + string("Report_Debug86");
#else
	strFileName += string(buffer) + string("Report_Release86");
#endif // _DEBUG
#endif // 

	bool bExist = GET_SINGLE(CPathManager)->ExistFile(strFileName + ".txt");
	
	int i = 0;
	while (bExist)
	{
		string strBase = strFileName;
		++i;
		strBase += " (" + to_string(i) + ")";
		bExist = GET_SINGLE(CPathManager)->ExistFile(strBase + ".txt");		
	}

	if (i == 0)
		strFileName += ".txt";
	else
		strFileName += " (" + to_string(i) + ").txt";

	fopen_s(&pFile, strFileName.c_str(), "wt");
	


	for (iter = Archive.begin(); iter != iterEnd; ++iter)
	{
		string strExternalScopeName = iter->first;
		size_t iLocalCount = iter->second.iRecordedLocalCount;
		size_t iFrameCount = iter->second.iRecordedFrameCount;
		
		string strBuffer = "Frame Count : " + to_string(iFrameCount) + "  Local Scope Count : " + to_string(iLocalCount) + "\n";
		fwrite(strBuffer.c_str(), sizeof(char), strBuffer.length(), pFile);
		
		strBuffer = "Base Scope : " + strExternalScopeName + "\n{\n";
		fwrite(strBuffer.c_str(), sizeof(char), strBuffer.length(), pFile);		

		double fTotalRate = 0.0f;
		for (size_t i = 0; i < iLocalCount; ++i)
		{
			strBuffer = "\t";
			fwrite(strBuffer.c_str(), sizeof(char), strBuffer.length(), pFile);

			strBuffer = iter->second.vecScopeName[i];
			fwrite(strBuffer.c_str(), sizeof(char), strBuffer.length(), pFile);

			fTotalRate += iter->second.vecLocalRate[i];
			strBuffer = "\t" + to_string(iter->second.vecLocalRate[i] * 100.0) + " %\n";
			fwrite(strBuffer.c_str(), sizeof(char), strBuffer.length(), pFile);
		}

		strBuffer = "\tOthers\t" + to_string((1.0 - fTotalRate) * 100.0) + " %\n";
		fwrite(strBuffer.c_str(), sizeof(char), strBuffer.length(), pFile);


		strBuffer = "}//" + strExternalScopeName + " End \n\n\n";
		fwrite(strBuffer.c_str(), sizeof(char), strBuffer.length(), pFile);
	}

	fclose(pFile);

}


void CDebug::SetRenderState(const string & strKey)
{
	CRenderState*	pRenderState = GET_SINGLE(CRenderManager)->FindRenderState(strKey);

	if (!pRenderState)
		return;

	SAFE_RELEASE(m_pRenderState[pRenderState->GetRenderStateType()]);

	m_pRenderState[pRenderState->GetRenderStateType()] = pRenderState;	
}

CMesh* CDebug::CreateLineMesh(PLINE pLine)
{
	PVERTEXCOLOR pVertices = new VERTEXCOLOR[pLine->vecPoint.size()];

	size_t iSize = pLine->vecPoint.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		pVertices[i].vertexPosition = pLine->vecPoint[i] - pLine->vecPoint[0];
		pVertices[i].vertexColor = pLine->vColor;
	}

	CMesh* pMesh = new CMesh;

	PMESHCONTAINER pContainer = new MESHCONTAINER;

	pMesh->m_vecMeshContainer.push_back(pContainer);

	if (!pMesh->CreateVertexBuffer(iSize, sizeof(VERTEXCOLOR),
		D3D11_USAGE_DEFAULT, &pVertices[0], D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, pContainer))
	{
		return NULL;
	}

	if (!pMesh->CreateIndexBuffer(0, 0, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, NULL, pContainer))
	{
		return NULL;
	}
	
	delete[] pVertices;

	return pMesh;
}

CMesh * CDebug::CreateRayMesh(PRAY pRay)
{
	PVERTEXCOLOR pVertices = new VERTEXCOLOR[2];

	
	pVertices[0].vertexPosition = pRay->vOrigin;
	pVertices[0].vertexColor = pRay->vColor;

	pVertices[1].vertexPosition = pRay->vOrigin + pRay->vDirection.Normalize();
	pVertices[1].vertexColor = pRay->vColor;

	CMesh* pMesh = new CMesh;

	PMESHCONTAINER pContainer = new MESHCONTAINER;

	pMesh->m_vecMeshContainer.push_back(pContainer);

	if (!pMesh->CreateVertexBuffer(2, sizeof(VERTEXCOLOR),
		D3D11_USAGE_DEFAULT, &pVertices[0], D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, pContainer))
	{
		return NULL;
	}

	if (!pMesh->CreateIndexBuffer(0, 0, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, NULL, pContainer))
	{
		return NULL;
	}

	delete[] pVertices;

	return pMesh;
}

void CDebug::Clear()
{
	size_t iSize = m_vecLine.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		SAFE_DELETE(m_vecLine[i]);
	}
	m_vecLine.clear();

	iSize = m_vecRay.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		SAFE_DELETE(m_vecRay[i]);
	}
	m_vecRay.clear();

	iSize = m_vecLineMesh.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		SAFE_RELEASE(m_vecLineMesh[i]);
	}
	m_vecLineMesh.clear();


	iSize = m_vecRayMesh.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		SAFE_RELEASE(m_vecRayMesh[i]);
	}
	m_vecRayMesh.clear();
}

void CDebug::UpdateLineTransform(size_t idx)
{
	TRANSFORMCBUFFER	tBuffer;

	CScene* pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();

	// Camera를 얻어온다.
	CCamera*	pCamera = pScene->GetMainCamera();
	SAFE_RELEASE(pScene);

	tBuffer.matWorld = Matrix::Translation(m_vecLine[idx]->vecPoint[0]);
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProjection = pCamera->GetPerspectiveProjectionMatrix();
	tBuffer.matWV = tBuffer.matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
	tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
	tBuffer.matWP = tBuffer.matWorld * tBuffer.matProjection;
	tBuffer.matInvProjection = Matrix::Inverse(tBuffer.matProjection);
	tBuffer.matInvVP = Matrix::Inverse(tBuffer.matVP);
	tBuffer.vPivot = Vector3::Zero;
	tBuffer.iRenderMode = GET_SINGLE(CRenderManager)->GetRenderMode() ? 1 : 0;
	tBuffer.vLength = Vector3::Zero;
	SAFE_RELEASE(pCamera);

	tBuffer.matWorld = tBuffer.matWorld.Transpose();
	tBuffer.matView = tBuffer.matView.Transpose();
	tBuffer.matProjection = tBuffer.matProjection.Transpose();
	tBuffer.matWV = tBuffer.matWV.Transpose();
	tBuffer.matWVP = tBuffer.matWVP.Transpose();
	tBuffer.matVP = tBuffer.matVP.Transpose();
	tBuffer.matWP = tBuffer.matWP.Transpose();
	tBuffer.matInvProjection = tBuffer.matInvProjection.Transpose();
	tBuffer.matInvVP = tBuffer.matInvVP.Transpose();

	// Transform 정보를 Shader 상수버퍼로 넘겨준다.
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(TRANSFORM_CBUFFER, &tBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);
}

void CDebug::UpdateRayTransform(size_t idx)
{
	TRANSFORMCBUFFER	tBuffer;

	CScene* pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();

	// Camera를 얻어온다.
	CCamera*	pCamera = pScene->GetMainCamera();

	SAFE_RELEASE(pScene);



	//
	Matrix tScale = Matrix::Scaling(Vector3(1000000.0f, 1000000.0f, 1000000.0f));
	Matrix tTranslate = Matrix::Translation(m_vecRay[idx]->vOrigin);

	tBuffer.matWorld = tScale * tTranslate;
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProjection = pCamera->GetPerspectiveProjectionMatrix();
	tBuffer.matWV = tBuffer.matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProjection;
	tBuffer.matVP = tBuffer.matView * tBuffer.matProjection;
	tBuffer.matWP = tBuffer.matWorld * tBuffer.matProjection;
	tBuffer.matInvProjection = Matrix::Inverse(tBuffer.matProjection);
	tBuffer.matInvVP = Matrix::Inverse(tBuffer.matVP);
	tBuffer.vPivot = Vector3::Zero;
	tBuffer.iRenderMode = GET_SINGLE(CRenderManager)->GetRenderMode() ? 1 : 0;
	tBuffer.vLength = Vector3::Zero;
	SAFE_RELEASE(pCamera);

	tBuffer.matWorld = tBuffer.matWorld.Transpose();
	tBuffer.matView = tBuffer.matView.Transpose();
	tBuffer.matProjection = tBuffer.matProjection.Transpose();
	tBuffer.matWV = tBuffer.matWV.Transpose();
	tBuffer.matWVP = tBuffer.matWVP.Transpose();
	tBuffer.matVP = tBuffer.matVP.Transpose();
	tBuffer.matWP = tBuffer.matWP.Transpose();
	tBuffer.matInvProjection = tBuffer.matInvProjection.Transpose();
	tBuffer.matInvVP = tBuffer.matInvVP.Transpose();

	// Transform 정보를 Shader 상수버퍼로 넘겨준다.
	GET_SINGLE(CShaderManager)->UpdateConstantBuffer(TRANSFORM_CBUFFER, &tBuffer, CBT_VERTEX | CBT_PIXEL | CBT_GEOMETRY);
}


bool CDebug::Initialize()
{
	GET_SINGLE(CDevice)->GetDevice()->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&m_pDX11Debug));
	return true;
}

void CDebug::DrawLine(vector<Vector3>& vecPoints, const Vector4 & vColor)
{
	LINE* pLine = new LINE;
	pLine->vecPoint.resize(vecPoints.size());
	memcpy(&pLine->vecPoint[0], &vecPoints[0], sizeof(Vector3) * vecPoints.size());
	pLine->vColor = vColor;
	m_vecLine.push_back(pLine);


	CMesh* pMesh = CreateLineMesh(pLine);
	if (pMesh)
	{
		m_vecLineMesh.push_back(pMesh);
	}	
}

void CDebug::DrawLine(const Vector3 & vBegin, const Vector3 & vEnd, const Vector4 & vColor)
{
	LINE* pLine = new LINE;
	pLine->vecPoint.push_back(vBegin);
	pLine->vecPoint.push_back(vEnd);
	pLine->vColor = vColor;
	m_vecLine.push_back(pLine);

	CMesh* pMesh = CreateLineMesh(pLine);
	if (pMesh)
	{
		m_vecLineMesh.push_back(pMesh);
	}
}

void CDebug::DrawLine(LINE* pLine)
{
	m_vecLine.push_back(pLine);
}

void CDebug::DrawRay(RAY* pRay)
{
	CMesh* pMesh = CreateRayMesh(pRay);
	if (pMesh)
	{
		m_vecRayMesh.push_back(pMesh);
	}
	m_vecRay.push_back(pRay);
}

void CDebug::Render(float fTime)
{
#pragma region FPS
	{
		char	strTemp[64] = {};
		sprintf_s(strTemp, "%.2f", 1.0f / GET_SINGLE(CTimerManager)->FindTimer("MainTimer")->GetFixedDeltaTime());
		string strTransfer(strTemp);

		wstring wstrFPS(strTransfer.begin(), strTransfer.end());

		ID2D1RenderTarget* pTarget = GET_SINGLE(CDevice)->Get2DRenderTarget();

		RESOLUTION tResolution = GET_SINGLE(CDevice)->GetDeviceResolution();

		GET_SINGLE(CDevice)->Get2DRenderTarget()->BeginDraw();
		GET_SINGLE(CDevice)->Get2DRenderTarget()->DrawText(wstrFPS.c_str(), wstrFPS.length(), m_pDebugFPSTextFormat,
			D2D1::RectF(0.0f, (float)tResolution.iHeight - 40.0f, 100.0f, (float)tResolution.iHeight), m_pDebugFPSBrush);

		GET_SINGLE(CDevice)->Get2DRenderTarget()->EndDraw();
	}
#pragma endregion



#pragma region  Line

	size_t iSize = m_vecLine.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		UpdateLineTransform(i);

		for (int i = 0; i < RS_END; ++i)
		{
			if (m_pRenderState[i])
				m_pRenderState[i]->SetState();
		}

		COLLIDERCBUFFER tBuffer = {};
		tBuffer.bUI = 0;
		tBuffer.vColor = m_vecLine[i]->vColor;
		GET_SINGLE(CShaderManager)->UpdateConstantBuffer(COLLIDER_CBUFFER, &tBuffer, CBT_VERTEX);

		DEVICE_CONTEXT->IASetInputLayout(m_pInputLayout);
		m_pShader->SetShader();
		if (m_pSampler)
		{
			m_pSampler->SetSampler(0);
		}
		m_vecLineMesh[i]->Render();


		for (int i = 0; i < RS_END; ++i)
		{
			if (m_pRenderState[i])
				m_pRenderState[i]->ResetState();
		}
	}
#pragma endregion
	

#pragma region  Ray

	iSize = m_vecRay.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		UpdateRayTransform(i);

		for (int i = 0; i < RS_END; ++i)
		{
			if (m_pRenderState[i])
				m_pRenderState[i]->SetState();
		}

		COLLIDERCBUFFER tBuffer = {};
		tBuffer.bUI = 0;
		tBuffer.vColor = m_vecLine[i]->vColor;
		GET_SINGLE(CShaderManager)->UpdateConstantBuffer(COLLIDER_CBUFFER, &tBuffer, CBT_VERTEX);

		DEVICE_CONTEXT->IASetInputLayout(m_pInputLayout);
		m_pShader->SetShader();
		if (m_pSampler)
		{
			m_pSampler->SetSampler(0);
		}

		m_vecRayMesh[i]->Render();

		for (int i = 0; i < RS_END; ++i)
		{
			if (m_pRenderState[i])
				m_pRenderState[i]->ResetState();
		}
	}
#pragma endregion

	
	Clear();
	if (GET_SINGLE(CCore)->GetEngineState())
	{
		//모든 Layer를 돌면서 디버그용 렌더를 그려줌	
		CScene* pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
		pScene->RenderDebug(fTime);
		SAFE_RELEASE(pScene);
	}
}

void CDebug::Log(const string & strLog)
{
	if(m_bUseConsole)
		_cprintf(strLog.c_str());
}

void CDebug::Log(const wstring & strLog)
{
	if (m_bUseConsole)
		_cwprintf(strLog.c_str());
}
