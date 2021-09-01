#include "stdafx.h"
#include "AtlasEditorArchive.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "Inspector.h"

DEFINITION_SINGLE(CAtlasEditorArchive)

CAtlasEditorArchive::CAtlasEditorArchive():
	m_pMainFrame(NULL),
	m_pInspector(NULL),
	m_pView(NULL)
{
}

CAtlasEditorArchive::~CAtlasEditorArchive()
{
}

void CAtlasEditorArchive::SetView(CToolView * pView)
{
	m_pView = pView;
}

void CAtlasEditorArchive::SetMainFrame(CMainFrame * pMainFrame)
{
	m_pMainFrame = pMainFrame;
}

void CAtlasEditorArchive::SetInspector(CInspector * pInspector)
{
	m_pInspector = pInspector;
}

CMainFrame * CAtlasEditorArchive::GetMainFrame() const
{
	return m_pMainFrame;
}

CInspector * CAtlasEditorArchive::GetInspector() const
{
	return m_pInspector;
}

CToolView * CAtlasEditorArchive::GetToolView() const
{
	return m_pView;
}

bool CAtlasEditorArchive::Initialize()
{
	return true;
}


bool CAtlasEditorArchive::HasFocus()
{
	HWND hViewHandle = m_pView->m_hWnd;
	HWND hInspectorHandle = m_pInspector->m_hWnd;
	HWND hMainFrameHandle = m_pMainFrame->m_hWnd;
	HWND hWnd = m_pMainFrame->GetFocus()->GetSafeHwnd();

	return (hWnd == hViewHandle) || (hWnd == hInspectorHandle) || (hWnd == hMainFrameHandle);
}