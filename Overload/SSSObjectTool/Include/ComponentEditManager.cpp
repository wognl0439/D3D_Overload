#include "stdafx.h"
#include "ComponentEditManager.h"

DEFINITION_SINGLE(CComponentEditManager)

CComponentEditManager::CComponentEditManager()
{
}


CComponentEditManager::~CComponentEditManager()
{
	if (m_vComponentStatic.size() > 0)
	{
		vector<CStatic*>::iterator	iter;
		vector<CStatic*>::iterator	iterEnd = m_vComponentStatic.end();

		for (iter = m_vComponentStatic.begin(); iter != iterEnd; ++iter)
		{
			delete(*iter);
		}

		m_vComponentStatic.clear();
	}


	if (m_mapEditControl.size() > 0)
	{
		unordered_map<string, class CEdit*>::iterator	iter;
		unordered_map<string, class CEdit*>::iterator	iterEnd = m_mapEditControl.end();

		for (iter = m_mapEditControl.begin(); iter != iterEnd; ++iter)
		{
			delete(*iter->second);
		}

		m_mapEditControl.clear();
	}
}

void CComponentEditManager::CreateEditControl(const string& strEditTag)
{
}

void CComponentEditManager::ShowComponent(const string& strComponentTag)
{
	// Position Edit Control
	CEdit*	pPositionZ = new CEdit;

	pPositionZ->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | SS_RIGHT,
		m_rComponentRect, m_wndInspector, IDC_POSISION_Z);

	if (strComponentTag == "MeshRenderer")
	{
	}

	else if (strComponentTag == "SpriteRenderer")
	{

	}

	else if (strComponentTag == "Material")
	{

	}

	else if (strComponentTag == "CircleCollider2D")
	{

	}

	else if (strComponentTag == "RectCollider2D")
	{

	}

	else if (strComponentTag == "SphereCollider")
	{

	}

	else if (strComponentTag == "UICollider")
	{

	}

	else if (strComponentTag == "Animator")
	{

	}

	else if (strComponentTag == "Cursor")
	{

	}

	else if (strComponentTag == "Camera")
	{

	}

	else if (strComponentTag == "Text")
	{

	}

	else if (strComponentTag == "Directional Light")
	{
		
	}

	else if (strComponentTag == "Terrain")
	{

	}
}

CEdit * CComponentEditManager::FindEditControl(const string& strEdit)
{
	unordered_map<string, CEdit*>::iterator iter = m_mapEditControl.find(strEdit);

	if (iter == m_mapEditControl.end())
	{
		return nullptr;
	}

	return iter->second;
}

void CComponentEditManager::SetInspectorWnd(CWnd * wndInspector)
{
	m_wndInspector = wndInspector;
}
