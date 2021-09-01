#pragma once
#include "stdafx.h"
#include "Tool.h"

#include "Engine.h"
#include "Component.h"
//#include "GameObject.h"
#include "Component/Gizmo.h"
#include "SoundManager.h"


SSS_USING

typedef struct _tagComponentControl
{
	CWnd* hWnd;
	CComponent* pComponent;
}COMPONENTCONTROL, *PCOMPONENTCONTROL;

typedef struct _tagGameObjectControl
{
	CWnd* hWnd;
	CGameObject* pGameObject;
}GAMEOBJECTCONTROL, *PGAMEOBJECTCONTROL;

class CEditorArchive
{
	DECLARE_SINGLE(CEditorArchive)

private:	
	size_t m_iVerticalOffset;
	size_t m_iHorizontalOffset;

	vector<COMPONENTCONTROL*> m_vecComponentControl;
	vector<GAMEOBJECTCONTROL*> m_vecGameObjectControl;
	
	class CToolView*		m_pView;
	class CInspector*		m_pInspector;
	class CHierarchy*		m_pHierarchy;
	class CDlgTabControl*	m_pDlgTabCtr;
	class CDlgSceneEdit*	m_pDlgSceneEdit;
	class CDlgAnimationClip*	m_pDlgAnimationClip;
	class CGameObject*		m_pSelectObject;
	class CAnimationControllerInspector* m_pAnimationCtrInspector;
	class CTransitionNode*	m_pTransitionNode;
	HTREEITEM*				m_phTreeItem;
	CCreateContext*			m_pContext;

	CWnd* m_pToolViewHandle;
	CWnd* m_pInspectorHandle;
	CWnd* m_pHierarchyHandle;
	CWnd* m_pTabControlHandle;
	CWnd* m_pAnimationHandle;

	class CGameObject*		m_pGizmoObject;

	bool m_bAnimationDlg;
	bool m_bGameViewDlg;
	bool m_bTransformState;

public:
	void SetTransformState(bool bTransformState);
	void SetAnimationDlgState(bool bAnimationDlgState);
	void SetContext(CCreateContext* Context);
	void SetToolViewHandle(CWnd* pwndToolView);
	void SetInspectorHandle(CWnd* pwndInspector);
	void SetHierarchyHandle(CWnd* pwndHierarchy);
	void SetDlgTabControlHandle(CWnd* pwndTabCtr);
	void SetAnimationHandle(CWnd* pwndAnimation);
	CCreateContext*	GetContext();
	CWnd* GetToolViewHandle();
	CWnd* GetInspectorHandle();
	CWnd* GetHierarchyHandle();
	CWnd* GetTabControlHandle();
	CWnd* GetAnimationHandle();
	void SetToolView(class CToolView* pToolView);
	void SetInspector(class CInspector* pInspector);
	void SetHierarchy(class CHierarchy* pHierarchy);
	void SetDlgTabControl(class CDlgTabControl* pTabCtr);
	void SetDlgAnimationClip(class CDlgAnimationClip* pDlgAnimationClip);
	void SetAnimationControllerInspector(class CAnimationControllerInspector* pAnimationInspector);
	void SetTransitionNode(class CTransitionNode* pTransitionNode);
	class CToolView* GetToolView();
	class CInspector* GetInspector();
	class CHierarchy* GetHierarchy();
	class CDlgTabControl* GetDlgTabControl();
	class CDlgAnimationClip* GetDlgAnimationClip();
	class CAnimationControllerInspector*	GetAnimationControllerInspector();
	class CTransitionNode* GetTransitionNode();
	void SetSelectObject(class CGameObject* pSelectObject);
	class CGameObject* GetSelectObject();
	void SetTreeItem(HTREEITEM* hTreeItem);
	HTREEITEM* GetTreeItem();
	vector<COMPONENTCONTROL*>	GetComponentControl();
	CWnd* GetComponentControl(class CComponent* pComponent, UINT iID);
	CGizmo*		GetGizmo();
	bool GetAnimationDlgState();
	bool GetTransformState();
	void PlayMusic(SOUND_TYPE ST_TYPE, int iPlayNumber);

public:	
	void ClearControls();
	void CreateGizmoControl();
	void RemoveGizmoControl();

	class CComponent* GetCurrentControlledComponent();
	class CGameObject* GetCurrentControlledGameObject();

	size_t GetCurrentVerticalOffset() const;
	size_t GetCurrentHorizontalOffset() const;
	
	// LT, RB
	CWnd* CreateButton(class CComponent* pTargetComponent, CString strName, DWORD iStyle, const POINT& LT, const POINT& RB, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateButton(class CGameObject* pTargetComponent, CString strName, DWORD iStyle, const POINT& LT, const POINT& RB, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateSpinButton(class CComponent* pTargetComponent, DWORD iStyle, const POINT& LT, const POINT& RB, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateSpinButton(class CGameObject* pTargetComponent, DWORD iStyle, const POINT& LT, const POINT& RB, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateStatic(class CComponent* pTargetComponent, CString strName, DWORD iStyle, const POINT& LT, const POINT& RB, UINT iID = 65535U, CWnd* pParent = NULL);
	CWnd* CreateStatic(class CGameObject* pTargetComponent, CString strName, DWORD iStyle, const POINT& LT, const POINT& RB, UINT iID = 65535U, CWnd* pParent = NULL);
	CWnd* CreateEdit(class CComponent* pTargetComponent, DWORD iStyle, const POINT& LT, const POINT& RB, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateEdit(class CGameObject* pTargetComponent, DWORD iStyle, const POINT& LT, const POINT& RB, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateSlider(class CComponent* pTargetComponent, DWORD iStyle, const POINT& LT, const POINT& RB, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateSlider(class CGameObject* pTargetComponent, DWORD iStyle, const POINT& LT, const POINT& RB, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateComboBox(class CGameObject* pTargetComponent, DWORD iStyle, const POINT& LT, const POINT& RB, int ComboBoxType, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateComboBox(class CComponent* pTargetComponent, DWORD iStyle, const POINT& LT, const POINT& RB, int ComboBoxType, UINT iID, CWnd* pParent = NULL);

	// RECT
	CWnd* CreateButton(class CComponent* pTargetComponent, CString strName, DWORD iStyle, RECT rc, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateButton(class CGameObject* pTargetComponent, CString strName, DWORD iStyle, RECT rc, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateSpinButton(class CComponent* pTargetComponent, DWORD iStyle, RECT rc, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateSpinButton(class CGameObject* pTargetComponent, DWORD iStyle, RECT rc, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateStatic(class CComponent* pTargetComponent, CString strName, DWORD iStyle, RECT rc, UINT iID = 65535U, CWnd* pParent = NULL);
	CWnd* CreateStatic(class CGameObject* pTargetComponent, CString strName, DWORD iStyle, RECT rc, UINT iID = 65535U, CWnd* pParent = NULL);
	CWnd* CreateEdit(class CComponent* pTargetComponent, DWORD iStyle, RECT rc, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateEdit(class CGameObject* pTargetComponent, DWORD iStyle, RECT rc, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateSlider(class CComponent* pTargetComponent, DWORD iStyle, RECT rc, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateSlider(class CGameObject* pTargetComponent, DWORD iStyle, RECT rc, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateComboBox(class CGameObject* pTargetComponent, DWORD iStyle, RECT rc, int ComboBoxType, UINT iID, CWnd* pParent = NULL);
	CWnd* CreateComboBox(class CComponent* pTargetComponent, DWORD iStyle, RECT rc, int ComboBoxType, UINT iID, CWnd* pParent = NULL);

public:
	void CreateAnimationDlg();
	void DeleteAnimationDlg();
	void CreateDlgSceneEdit();
	void DeleteDlgSceneEdit();

public:
	void SavePrefab();
	void LoadPrefab();
};

