#pragma once
#include "Engine.h"

SSS_USING

class CAtlasEditorArchive
{
	DECLARE_SINGLE(CAtlasEditorArchive)

private:
	class CMainFrame* m_pMainFrame;
	class CInspector* m_pInspector;
	class CToolView* m_pView;

public:
	void SetView(class CToolView* pView);
	void SetMainFrame(class CMainFrame* pMainFrame);
	void SetInspector(class CInspector* pInspector);
	class CMainFrame* GetMainFrame() const;
	class CInspector* GetInspector() const;
	class CToolView* GetToolView() const;
public:
	bool Initialize();
	bool HasFocus();

};

