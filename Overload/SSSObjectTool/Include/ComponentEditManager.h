#pragma once
#include "Engine.h"

SSS_USING

#define		IDC_POSISION_X		1100
#define		IDC_POSISION_Y		1101
#define		IDC_POSISION_Z		1102
#define		IDC_SCALE_X			1103
#define		IDC_SCALE_Y			1104
#define		IDC_SCALE_Z			1105
#define		IDC_ROTATION_X		1106
#define		IDC_ROTATION_Y		1107
#define		IDC_ROTATION_Z		1108


typedef struct SSS_DLL _tagComponentEdit
{
	vector<CEdit*>			m_vComponentEdit;
	CRect					m_rComponenetSize;
	
	// Component가 그려지는 총 높이. ( 다음 컴포넌트 위치 잡기 위함 )
	int						m_iHeight;
	int						m_iComponentCount;
	
}COMPONENTEDIT, *PCOMPONENTEDIT;

class CComponentEditManager
{
	DECLARE_SINGLE(CComponentEditManager)

private:
	unordered_map<string, class CEdit*>		m_mapEditControl;
	unordered_map<string, PCOMPONENTEDIT>	m_mapComponentEdit;

	vector<CStatic*>		m_vComponentStatic;
	CRect					m_rComponentRect;
	CRect					m_rComponenetEditSize;
	
	CWnd*					m_wndInspector;

	// Component가 그려지는 총 높이. ( 다음 컴포넌트 위치 잡기 위함 )
	int						m_iHeight;

public:
	void	ShowComponent(const string& strComponentTag);

public:
	void	CreateEditControl(const string& strEditTag);
	CEdit*	FindEditControl(const string& strEditTag);

public:
	void	SetInspectorWnd(CWnd* wndInspector);
};

