#include "EditorArchive.h"
#include "Inspector.h"
#include "Hierarchy.h"
#include "Flags.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"

#include "DlgTabControl.h"
#include "TransitionNode.h"
#include "DlgAnimationClip.h"
#include "DlgSceneEdit.h"
#include "CollisionManager.h"

DEFINITION_SINGLE(CEditorArchive)

CEditorArchive::CEditorArchive():
	m_pSelectObject(NULL),
	m_pGizmoObject(NULL),
	m_pDlgSceneEdit(NULL),
	m_pDlgTabCtr(NULL),
	m_bAnimationDlg(false),
	m_bGameViewDlg(false),
	m_bTransformState(true)
{
}

CEditorArchive::~CEditorArchive()
{
	ClearControls();
	SAFE_DELETE(m_pDlgTabCtr);
	SAFE_DELETE(m_pDlgSceneEdit);
	RemoveGizmoControl();
}

void CEditorArchive::SetTransformState(bool bTransformState)
{
	m_bTransformState = bTransformState;
}

void CEditorArchive::SetAnimationDlgState(bool bAnimationDlgState)
{
	m_bAnimationDlg = bAnimationDlgState;
}

void CEditorArchive::SetContext(CCreateContext* Context)
{
	m_pContext = Context;
}

void CEditorArchive::SetToolViewHandle(CWnd * pwndToolView)
{
	m_pToolViewHandle = pwndToolView;
}

void CEditorArchive::SetInspectorHandle(CWnd * pwndInspector)
{
	m_pInspectorHandle = pwndInspector;
}

void CEditorArchive::SetHierarchyHandle(CWnd * pwndHierarchy)
{
	m_pHierarchyHandle = pwndHierarchy;
}

void CEditorArchive::SetDlgTabControlHandle(CWnd * pwndTabCtr)
{
	m_pTabControlHandle = pwndTabCtr;
}

void CEditorArchive::SetAnimationHandle(CWnd * pwndAnimation)
{
	m_pAnimationHandle = pwndAnimation;
}

CCreateContext * CEditorArchive::GetContext()
{
	return m_pContext;
}

CWnd * CEditorArchive::GetToolViewHandle()
{
	return m_pToolViewHandle;
}

CWnd * CEditorArchive::GetInspectorHandle()
{
	return m_pInspectorHandle;
}

CWnd * CEditorArchive::GetHierarchyHandle()
{
	return m_pHierarchyHandle;
}

CWnd * CEditorArchive::GetTabControlHandle()
{
	return m_pTabControlHandle;
}

CWnd * CEditorArchive::GetAnimationHandle()
{
	return m_pAnimationHandle;
}

void CEditorArchive::SetToolView(CToolView * pToolView)
{
	m_pView = pToolView;
}

void CEditorArchive::SetInspector(CInspector * pInspector)
{
	m_pInspector = pInspector;
}

void CEditorArchive::SetHierarchy(CHierarchy * pHierarchy)
{
	m_pHierarchy = pHierarchy;
}

void CEditorArchive::SetDlgTabControl(CDlgTabControl * pTabCtr)
{
	m_pDlgTabCtr = pTabCtr;
}

void CEditorArchive::SetDlgAnimationClip(CDlgAnimationClip * pDlgAnimationClip)
{
	m_pDlgAnimationClip = pDlgAnimationClip;
}


void CEditorArchive::SetAnimationControllerInspector(CAnimationControllerInspector* pAnimationInspector)
{
	m_pAnimationCtrInspector = pAnimationInspector;
}

void CEditorArchive::SetTransitionNode(CTransitionNode * pTransitionNode)
{
	m_pTransitionNode = pTransitionNode;
}

CToolView * CEditorArchive::GetToolView()
{
	return m_pView;
}

CInspector * CEditorArchive::GetInspector()
{
	return m_pInspector;
}

CHierarchy * CEditorArchive::GetHierarchy()
{
	return m_pHierarchy;
}

CDlgTabControl * CEditorArchive::GetDlgTabControl()
{
	return m_pDlgTabCtr;
}

CDlgAnimationClip * CEditorArchive::GetDlgAnimationClip()
{
	return m_pDlgAnimationClip;
}

CAnimationControllerInspector * CEditorArchive::GetAnimationControllerInspector()
{
	return m_pAnimationCtrInspector;
}

CTransitionNode * CEditorArchive::GetTransitionNode()
{
	return m_pTransitionNode;
}

void CEditorArchive::SetSelectObject(CGameObject * pSelectObject)
{
	m_pSelectObject = pSelectObject;
	m_pInspector->SetSelctObject(m_pSelectObject);
	m_pHierarchy->m_pSelectObject = m_pSelectObject;

}

CGameObject * CEditorArchive::GetSelectObject()
{
	return m_pSelectObject;
}

void CEditorArchive::SetTreeItem(HTREEITEM * hTreeItem)
{
	m_phTreeItem = hTreeItem;
}

HTREEITEM * CEditorArchive::GetTreeItem()
{
	return m_phTreeItem;
}

vector<COMPONENTCONTROL*> CEditorArchive::GetComponentControl()
{
	return m_vecComponentControl;
}

CWnd * CEditorArchive::GetComponentControl(CComponent * pComponent, UINT iID)
{
	size_t iCount = m_vecComponentControl.size();
	for (size_t i = 0; i < iCount; ++i)
	{
		if (m_vecComponentControl[i]->pComponent == pComponent)
		{
			if (m_vecComponentControl[i]->hWnd->GetDlgCtrlID() == iID)
			{
				return m_vecComponentControl[i]->hWnd;
			}
		}
	}

	return NULL;
}

CGizmo * CEditorArchive::GetGizmo()
{	
	if (m_pGizmoObject)
	{
		CGizmo*	pGizmo = m_pGizmoObject->GetComponent<CGizmo>();
		if (pGizmo != NULL)
			return pGizmo;
		else
			return NULL;
	}
	else
		return NULL;
}

bool CEditorArchive::GetAnimationDlgState()
{
	return m_bAnimationDlg;
}

bool CEditorArchive::GetTransformState()
{
	return m_bTransformState;
}

void CEditorArchive::PlayMusic(SOUND_TYPE ST_TYPE, int iPlay)
{
	switch (ST_TYPE)
	{
	case SOUND_BGM:
	{
		GET_SINGLE(CSoundManager)->Stop(SOUND_BGM);
		if (iPlay == 0)
			GET_SINGLE(CSoundManager)->Play("TestBGM0");

		else if (iPlay == 1)
			GET_SINGLE(CSoundManager)->Play("TestBGM1");

		else
		{
		}
	}
		break;
	}
}

void CEditorArchive::ClearControls()
{
	size_t iSize = m_vecComponentControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		if (m_vecComponentControl[i])
		{
			delete m_vecComponentControl[i]->hWnd;
			delete m_vecComponentControl[i];
		}
	}

	iSize = m_vecGameObjectControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		if (m_vecGameObjectControl[i])
		{
			delete m_vecGameObjectControl[i]->hWnd;
			delete m_vecGameObjectControl[i];
		}
	}

	m_vecComponentControl.clear();
	m_vecGameObjectControl.clear();

	m_pInspectorHandle->RedrawWindow();
}

void CEditorArchive::CreateGizmoControl()
{
	CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
	CLayer*	pLayer = pScene->FindLayer("Default");

	CGameObject* pGizmoObject = pLayer->FindGameObjectWithTag("GizmoObject");

	if (!pGizmoObject)
	{
		pGizmoObject = CGameObject::CreateObject("GizmoObject", pLayer);
		CGizmo* pGizmo = pGizmoObject->AddComponent<CGizmo>("Gizmo");
		pGizmo->SetGizmoState(GS_TRANSFORMATION);
		SAFE_RELEASE(pGizmo);
	}
	pGizmoObject->Enable(false);
	//SAFE_RELEASE(m_pGizmoObject);
	m_pGizmoObject = pGizmoObject;
	SAFE_RELEASE(pGizmoObject);

	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(pScene);
}

void CEditorArchive::RemoveGizmoControl()
{
	if (m_pGizmoObject)
	{					
		CLayer* pLayer = m_pGizmoObject->GetLayer();
		pLayer->RemoveGameObject(m_pGizmoObject);
		SAFE_RELEASE(pLayer);
		
		//CCollider*	pGizmoCollider = pGizmo->GetComponent<CCollider>();
		list<CCollider*>::const_iterator	iter = m_pGizmoObject->GetColliderList()->begin();
		list<CCollider*>::const_iterator	iterEnd = m_pGizmoObject->GetColliderList()->end();

		list<CComponent*>::iterator	Comiter = m_pGizmoObject->GetAllComponents().begin();
		list<CComponent*>::iterator	ComiterEnd = m_pGizmoObject->GetAllComponents().end();

		for (; Comiter != ComiterEnd;)
		{
			(*Comiter)->DestroyImmediate();					
			Comiter = m_pGizmoObject->GetAllComponents().erase(Comiter);
		}
		SAFE_RELEASE(m_pGizmoObject);
		m_pGizmoObject = NULL;
	}
}

CComponent * CEditorArchive::GetCurrentControlledComponent()
{
	HWND pCurrentFocusHandle = GetFocus();

	size_t iSize = m_vecComponentControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		if (m_vecComponentControl[i]->hWnd->m_hWnd == pCurrentFocusHandle)
		{
			return m_vecComponentControl[i]->pComponent;
		}
	}

	return NULL;
}

CGameObject * CEditorArchive::GetCurrentControlledGameObject()
{
	HWND pCurrentFocusHandle = GetFocus();

	size_t iSize = m_vecGameObjectControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		if (m_vecGameObjectControl[i]->hWnd->m_hWnd == pCurrentFocusHandle)
		{
			return m_vecGameObjectControl[i]->pGameObject;
		}
	}
	return NULL;
}

size_t CEditorArchive::GetCurrentVerticalOffset() const
{
	return m_iVerticalOffset;
}

size_t CEditorArchive::GetCurrentHorizontalOffset() const
{
	return m_iHorizontalOffset;
}

CWnd * CEditorArchive::CreateButton(CComponent * pTarget, CString strName, DWORD iStyle, const POINT & LT, const POINT & RB, UINT iID, CWnd* pParent)
{
	if (m_pInspectorHandle || pParent)
	{
		CWnd* pCurrentParent = m_pInspectorHandle;
		if (pParent)
		{
			pCurrentParent = pParent;
		}

		
		RECT tRect = { LT.x, LT.y, RB.x, RB.y };

		CButton* pButton = new CButton;		

		if (pButton->Create(strName, iStyle, tRect, pCurrentParent, iID))
		{
			COMPONENTCONTROL* pControl = new COMPONENTCONTROL;
			pControl->hWnd = pButton;
			pControl->pComponent = pTarget;
			m_vecComponentControl.push_back(pControl);			
			return pButton;
		}
		else
		{
			delete pButton;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

CWnd * CEditorArchive::CreateButton(CGameObject * pTarget, CString strName, DWORD iStyle, const POINT & LT, const POINT & RB, UINT iID, CWnd* pParent)
{
	if (m_pInspectorHandle || pParent)
	{
		CWnd* pCurrentParent = m_pInspectorHandle;
		if (pParent)
		{
			pCurrentParent = pParent;
		}

		RECT tRect = { LT.x, LT.y, RB.x, RB.y };

		CButton* pButton = new CButton;

		if (pButton->Create(strName, iStyle, tRect, pCurrentParent, iID))
		{
			GAMEOBJECTCONTROL* pControl = new GAMEOBJECTCONTROL;
			pControl->hWnd = pButton;
			pControl->pGameObject = pTarget;
			m_vecGameObjectControl.push_back(pControl);
			return pButton;
		}
		else
		{
			delete pButton;
			return NULL;
		}

	}
	else
	{
		return NULL;
	}
}

CWnd * CEditorArchive::CreateSpinButton(CComponent * pTarget, DWORD iStyle, const POINT & LT, const POINT & RB, UINT iID, CWnd * pParent)
{
	if (m_pInspectorHandle || pParent)
	{
		CWnd* pCurrentParent = m_pInspectorHandle;
		if (pParent)
		{
			pCurrentParent = pParent;
		}

		RECT tRect = { LT.x, LT.y, RB.x, RB.y };

		CSpinButtonCtrl* pSpinButton = new CSpinButtonCtrl;

		if (pSpinButton->Create(iStyle, tRect, pCurrentParent, iID))
		{
			COMPONENTCONTROL* pControl = new COMPONENTCONTROL;
			pControl->hWnd = pSpinButton;
			pControl->pComponent = pTarget;
			m_vecComponentControl.push_back(pControl);
			return pSpinButton;
		}
		else
		{
			delete pSpinButton;
			return NULL;
		}

	}
	else
	{
		return NULL;
	}
}

CWnd * CEditorArchive::CreateSpinButton(CGameObject * pTarget, DWORD iStyle, const POINT & LT, const POINT & RB, UINT iID, CWnd * pParent)
{
	if (m_pInspectorHandle || pParent)
	{
		CWnd* pCurrentParent = m_pInspectorHandle;
		if (pParent)
		{
			pCurrentParent = pParent;
		}

		RECT tRect = { LT.x, LT.y, RB.x, RB.y };

		CSpinButtonCtrl* pSpinButton = new CSpinButtonCtrl;

		if (pSpinButton->Create(iStyle, tRect, pCurrentParent, iID))
		{
			GAMEOBJECTCONTROL* pControl = new GAMEOBJECTCONTROL;
			pControl->hWnd = pSpinButton;
			pControl->pGameObject = pTarget;
			m_vecGameObjectControl.push_back(pControl);
			return pSpinButton;
		}
		else
		{
			delete pSpinButton;
			return NULL;
		}

	}
	else
	{
		return NULL;
	}
}

CWnd * CEditorArchive::CreateStatic(CComponent * pTarget, CString strName, DWORD iStyle, const POINT & LT, const POINT & RB, UINT iID, CWnd* pParent)
{
	if (m_pInspectorHandle || pParent)
	{
		CWnd* pCurrentParent = m_pInspectorHandle;
		if (pParent)
		{
			pCurrentParent = pParent;
		}

		RECT tRect = { LT.x, LT.y, RB.x, RB.y };

		CStatic* pControl = new CStatic;

		if (pControl->Create(strName, iStyle, tRect, pCurrentParent, iID))
		{
			COMPONENTCONTROL* pControlInfo = new COMPONENTCONTROL;
			pControlInfo->hWnd = pControl;
			pControlInfo->pComponent = pTarget;
			m_vecComponentControl.push_back(pControlInfo);
			return pControl;
		}
		else
		{
			delete pControl;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

CWnd * CEditorArchive::CreateStatic(CGameObject * pTarget, CString strName, DWORD iStyle, const POINT & LT, const POINT & RB, UINT iID, CWnd* pParent)
{
	if (m_pInspectorHandle || pParent)
	{
		CWnd* pCurrentParent = m_pInspectorHandle;
		if (pParent)
		{
			pCurrentParent = pParent;
		}

		RECT tRect = { LT.x, LT.y, RB.x, RB.y };

		CStatic* pControl = new CStatic;

		if (pControl->Create(strName, iStyle, tRect, pCurrentParent, iID))
		{
			GAMEOBJECTCONTROL* pControlInfo = new GAMEOBJECTCONTROL;
			pControlInfo->hWnd = pControl;
			pControlInfo->pGameObject = pTarget;
			m_vecGameObjectControl.push_back(pControlInfo);
			return pControl;
		}
		else
		{
			delete pControl;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

CWnd * CEditorArchive::CreateEdit(CComponent * pTarget, DWORD iStyle, const POINT & LT, const POINT & RB, UINT iID, CWnd* pParent)
{
	if (m_pInspectorHandle || pParent)
	{
		CWnd* pCurrentParent = m_pInspectorHandle;
		if (pParent)
		{
			pCurrentParent = pParent;
		}

		RECT tRect = { LT.x, LT.y, RB.x, RB.y };

		CEdit* pControl = new CEdit;

		if (pControl->Create(iStyle, tRect, pCurrentParent, iID))
		{
			COMPONENTCONTROL* pControlInfo = new COMPONENTCONTROL;
			pControlInfo->hWnd = pControl;
			pControlInfo->pComponent = pTarget;
			m_vecComponentControl.push_back(pControlInfo);
			return pControl;
		}
		else
		{
			delete pControl;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}
CWnd * CEditorArchive::CreateEdit(CGameObject * pTarget, DWORD iStyle, const POINT & LT, const POINT & RB, UINT iID, CWnd* pParent)
{
	if (m_pInspectorHandle || pParent)
	{
		CWnd* pCurrentParent = m_pInspectorHandle;
		if (pParent)
		{
			pCurrentParent = pParent;
		}

		RECT tRect = { LT.x, LT.y, RB.x, RB.y };

		CEdit* pControl = new CEdit;

		if (pControl->Create(iStyle, tRect, pCurrentParent, iID))
		{
			GAMEOBJECTCONTROL* pControlInfo = new GAMEOBJECTCONTROL;
			pControlInfo->hWnd = pControl;
			pControlInfo->pGameObject = pTarget;
			m_vecGameObjectControl.push_back(pControlInfo);
			return pControl;
		}
		else
		{
			delete pControl;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}





CWnd * CEditorArchive::CreateButton(CComponent * pTargetComponent, CString strName, DWORD iStyle, RECT rc, UINT iID, CWnd * pParent)
{
	POINT	LT = { rc.left, rc.top };
	POINT	RB = { rc.right, rc.bottom };

	return CreateButton(pTargetComponent, strName, iStyle, LT, RB, iID, pParent);
}
CWnd * CEditorArchive::CreateButton(CGameObject * pTargetComponent, CString strName, DWORD iStyle, RECT rc, UINT iID, CWnd * pParent)
{
	POINT	LT = { rc.left, rc.top };
	POINT	RB = { rc.right, rc.bottom };

	return CreateButton(pTargetComponent, strName, iStyle, LT, RB, iID, pParent);
}
CWnd * CEditorArchive::CreateSpinButton(CComponent * pTargetComponent,  DWORD iStyle, RECT rc, UINT iID, CWnd * pParent)
{
	POINT	LT = { rc.left, rc.top };
	POINT	RB = { rc.right, rc.bottom };

	return CreateSpinButton(pTargetComponent, iStyle, LT, RB, iID, pParent);
}
CWnd * CEditorArchive::CreateSpinButton(CGameObject * pTargetComponent,  DWORD iStyle, RECT rc, UINT iID, CWnd * pParent)
{
	POINT	LT = { rc.left, rc.top };
	POINT	RB = { rc.right, rc.bottom };

	return CreateSpinButton(pTargetComponent, iStyle, LT, RB, iID, pParent);
}
CWnd * CEditorArchive::CreateStatic(CComponent * pTargetComponent, CString strName, DWORD iStyle, RECT rc, UINT iID, CWnd * pParent)
{
	POINT	LT = { rc.left, rc.top };
	POINT	RB = { rc.right, rc.bottom };

	return CreateStatic(pTargetComponent, strName, iStyle, LT, RB, iID, pParent);
}
CWnd * CEditorArchive::CreateStatic(CGameObject * pTargetComponent, CString strName, DWORD iStyle, RECT rc, UINT iID, CWnd * pParent)
{
	POINT	LT = { rc.left, rc.top };
	POINT	RB = { rc.right, rc.bottom };

	return CreateStatic(pTargetComponent, strName, iStyle, LT, RB, iID, pParent);
}
CWnd * CEditorArchive::CreateEdit(CComponent * pTargetComponent, DWORD iStyle, RECT rc, UINT iID, CWnd * pParent)
{
	POINT	LT = { rc.left, rc.top };
	POINT	RB = { rc.right, rc.bottom };

	return CreateEdit(pTargetComponent, iStyle, LT, RB, iID, pParent);
}
CWnd * CEditorArchive::CreateEdit(CGameObject * pTargetComponent, DWORD iStyle, RECT rc, UINT iID, CWnd * pParent)
{
	POINT	LT = { rc.left, rc.top };
	POINT	RB = { rc.right, rc.bottom };

	return CreateEdit(pTargetComponent, iStyle, LT, RB, iID, pParent);
}
CWnd * CEditorArchive::CreateSlider(CComponent * pTargetComponent, DWORD iStyle, RECT rc, UINT iID, CWnd * pParent)
{
	POINT	LT = { rc.left, rc.top };
	POINT	RB = { rc.right, rc.bottom };

	return CreateSlider(pTargetComponent, iStyle, LT, RB, iID, pParent);
}
CWnd * CEditorArchive::CreateSlider(CGameObject * pTargetComponent, DWORD iStyle, RECT rc, UINT iID, CWnd * pParent)
{
	POINT	LT = { rc.left, rc.top };
	POINT	RB = { rc.right, rc.bottom };

	return CreateSlider(pTargetComponent, iStyle, LT, RB, iID, pParent);
}
CWnd * CEditorArchive::CreateComboBox(CGameObject * pTargetComponent, DWORD iStyle, RECT rc, int ComboBoxType, UINT iID, CWnd * pParent)
{
	POINT	LT = { rc.left, rc.top };
	POINT	RB = { rc.right, rc.bottom };

	return CreateComboBox(pTargetComponent, iStyle, LT, RB, ComboBoxType, iID, pParent);
}
CWnd * CEditorArchive::CreateComboBox(CComponent * pTargetComponent, DWORD iStyle, RECT rc, int ComboBoxType, UINT iID, CWnd * pParent)
{
	POINT	LT = { rc.left, rc.top };
	POINT	RB = { rc.right, rc.bottom };

	return CreateComboBox(pTargetComponent, iStyle, LT, RB, ComboBoxType, iID, pParent);
}

void CEditorArchive::CreateAnimationDlg()
{
	if (!m_pDlgTabCtr)
	{
		m_pDlgTabCtr = new CDlgTabControl;
		m_pDlgTabCtr->Create(IDD_DIALOG_TAB, CWnd::GetDesktopWindow());
		m_pDlgTabCtr->ShowWindow(SW_SHOW);
	}
}

void CEditorArchive::DeleteAnimationDlg()
{
	SAFE_DELETE(m_pDlgTabCtr);
	m_pDlgTabCtr = NULL;
}

void CEditorArchive::CreateDlgSceneEdit()
{
	if (m_pSelectObject)
	{
		if (!m_pDlgSceneEdit)
		{
			m_pDlgSceneEdit = new CDlgSceneEdit;
			m_pDlgSceneEdit->Create(IDD_DIALOG_SCENE, CWnd::GetDesktopWindow());
			m_pDlgSceneEdit->ShowWindow(SW_SHOW);
		}
	}
	else
		MessageBox(NULL, L"Select Target Object!", L"", MB_OK);
}

void CEditorArchive::DeleteDlgSceneEdit()
{
	SAFE_DELETE(m_pDlgSceneEdit);
	m_pDlgSceneEdit = NULL;
}


void CEditorArchive::SavePrefab()
{
	if (m_pSelectObject)
	{
		CFileDialog	dlg(FALSE, L"SSSPrefab", L"Prefab", OFN_HIDEREADONLY,
			L"SSSPrefab(*.SSSPrefab)|*.*|AllFiles(*.*)|*.*||");

		if (dlg.DoModal() == IDOK)
		{
			wstring	strPath = dlg.GetPathName();
			m_pSelectObject->Save(strPath.c_str(), NO_USE_PATH);
		}
	}
	else
	{
		MessageBox(NULL, L"Select Target Object", L"Exception", MB_OK);
	}
}

void CEditorArchive::LoadPrefab()
{
	CFileDialog	dlg(TRUE, L"", L"", OFN_HIDEREADONLY,
		L"SSSPrefab(*.SSSPrefab)|*.*|AllFiles(*.*)|*.*||");

	if (dlg.DoModal() == IDOK)
	{
		wstring	strPath = dlg.GetPathName();
		CScene* pCurrentScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
		CGameObject* pGameObject = CGameObject::Load(pCurrentScene, strPath.c_str(), NO_USE_PATH);
		m_pHierarchy->LoadChildTreeItem(pGameObject);
		SAFE_RELEASE(pGameObject);
		SAFE_RELEASE(pCurrentScene);
	}
}

CWnd * CEditorArchive::CreateComboBox(CComponent * pTarget, DWORD iStyle, const POINT & LT, const POINT & RB, int ComboBoxType, UINT iID, CWnd* pParent)
{
	if (m_pInspectorHandle || pParent)
	{
		CWnd* pCurrentParent = m_pInspectorHandle;
		if (pParent)
		{
			pCurrentParent = pParent;
		}

		RECT tRect = { LT.x, LT.y, RB.x, RB.y };

		CComboBox* pControl = new CComboBox;

		if (pControl->Create(iStyle, tRect, pCurrentParent, iID))
		{
			COMPONENTCONTROL* pControlInfo = new COMPONENTCONTROL;
			pControlInfo->hWnd = pControl;
			pControlInfo->pComponent = pTarget;

			switch (ComboBoxType)
			{
			case CCT_COMPONENT:
				m_pInspector->SetComponentComboBox(pControl);

				//Renderer
				pControl->AddString(L"MeshRenderer");
				pControl->AddString(L"SpriteRenderer");

				//Coliider		
				pControl->AddString(L"Box Collider");
				pControl->AddString(L"CircleCollider2D");
				pControl->AddString(L"RectCollider2D");
				pControl->AddString(L"SphereCollider");
				pControl->AddString(L"UICollider");

				//Animator		
				pControl->AddString(L"Animator");

				//UI			
				pControl->AddString(L"UIPanel");
				pControl->AddString(L"UIButton");
				pControl->AddString(L"UICheckBox");
				pControl->AddString(L"UIRadioButton");
				pControl->AddString(L"AlphaMask");
				pControl->AddString(L"UISlider");

				//Camera		
				pControl->AddString(L"Camera");

				//Text			
				pControl->AddString(L"Text");

				//Light			
				pControl->AddString(L"Directional Light");
				pControl->AddString(L"Point Light");
				pControl->AddString(L"Spot Light");

				//Post Process	
				pControl->AddString(L"Post Processing");

				//Spline		
				pControl->AddString(L"CardinalSpline");

				//Effects		
				pControl->AddString(L"OutLine");
				pControl->AddString(L"Trail");
				pControl->AddString(L"Distortion");
				pControl->AddString(L"Fire");
				pControl->AddString(L"Water");

				pControl->AddString(L"Gizmo");

				//Billboard
				pControl->AddString(L"Billboard");

				// Client
				pControl->AddString(L"Camera Move");
				pControl->AddString(L"Chat");
				pControl->AddString(L"Effect");
				pControl->AddString(L"PlayerController");


				//Terrain
				pControl->AddString(L"Decal");
				pControl->AddString(L"Terrain");
				pControl->AddString(L"TerrainEditor");

				//Splash
				pControl->AddString(L"Splash");
				pControl->AddString(L"Player");
				pControl->AddString(L"Player Camera");
				pControl->AddString(L"Terrain Collider");
				pControl->AddString(L"MonsterAI");
				pControl->AddString(L"NPC_IDLE_AI");
				pControl->AddString(L"Rigidbody");
				pControl->AddString(L"Mesh Collider");

				// GameSystem
				pControl->AddString(L"GameSystem");

				pControl->AddString(L"BoneDragon AI");

				// Portal
				pControl->AddString(L"Portal");
				break;

			case CCT_SHADER:
				pControl->AddString(L"Render Target");
				pControl->AddString(L"Standard Color");
				pControl->AddString(L"Standard Tex");
				pControl->AddString(L"Colored Tex");
				pControl->AddString(L"Collider");
				pControl->AddString(L"UI");
				pControl->AddString(L"UI Collider");
				pControl->AddString(L"UI Grid");
				pControl->AddString(L"Standard Color Normal");
				pControl->AddString(L"Standard Texture Normal");
				pControl->AddString(L"Standard Bump");
				pControl->AddString(L"Rim Light");
				pControl->AddString(L"Out line");
				pControl->AddString(L"Trail Shader");
				pControl->AddString(L"Trail Distortion Shader");
				pControl->AddString(L"StandardBumpAnimShader");
				pControl->AddString(L"StandardTextureNormalAnimShader");
				break;

			case CCT_SCENE:
				pControl->AddString(L"Terny Day");
				pControl->AddString(L"Terny Night");
				pControl->AddString(L"Kings Road Day");
				pControl->AddString(L"Kings Road Night");
				pControl->AddString(L"Battle Field");
				break;
			}

			m_vecComponentControl.push_back(pControlInfo);
			return pControl;
		}
		else
		{
			delete pControl;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}
CWnd * CEditorArchive::CreateComboBox(CGameObject * pTarget, DWORD iStyle, const POINT & LT, const POINT & RB, int ComboBoxType, UINT iID, CWnd* pParent)
{
	if (m_pInspectorHandle || pParent)
	{
		CWnd* pCurrentParent = m_pInspectorHandle;
		if (pParent)
		{
			pCurrentParent = pParent;
		}

		RECT tRect = { LT.x, LT.y, RB.x, RB.y };

		CComboBox* pControl = new CComboBox;

		if (pControl->Create(iStyle, tRect, pCurrentParent, iID))
		{
			GAMEOBJECTCONTROL* pControlInfo = new GAMEOBJECTCONTROL;
			pControlInfo->hWnd = pControl;
			pControlInfo->pGameObject = pTarget;
			

			switch (ComboBoxType)
			{
			case CCT_COMPONENT:
				m_pInspector->SetComponentComboBox(pControl);
				//Renderer
				pControl->AddString(L"MeshRenderer");
				pControl->AddString(L"SpriteRenderer");

				//Coliider		
				pControl->AddString(L"Box Collider");
				pControl->AddString(L"CircleCollider2D");
				pControl->AddString(L"RectCollider2D");
				pControl->AddString(L"SphereCollider");
				pControl->AddString(L"UICollider");

				//Animator		
				pControl->AddString(L"Animator");

				//UI			
				pControl->AddString(L"UIPanel");
				pControl->AddString(L"UIButton");
				pControl->AddString(L"UICheckBox");
				pControl->AddString(L"UIRadioButton");
				pControl->AddString(L"AlphaMask");
				pControl->AddString(L"UISlider");

				//Camera		
				pControl->AddString(L"Camera");

				//Text			
				pControl->AddString(L"Text");

				//Light			
				pControl->AddString(L"Directional Light");
				pControl->AddString(L"Point Light");
				pControl->AddString(L"Spot Light");

				//Post Process	
				pControl->AddString(L"Post Processing");

				//Spline		
				pControl->AddString(L"CardinalSpline");

				//Effects		
				pControl->AddString(L"OutLine");
				pControl->AddString(L"Trail");
				pControl->AddString(L"Distortion");
				pControl->AddString(L"Fire");
				pControl->AddString(L"Water");

				pControl->AddString(L"Gizmo");

				//Billboard
				pControl->AddString(L"Billboard");

				// Client
				pControl->AddString(L"Camera Move");
				pControl->AddString(L"Chat");
				pControl->AddString(L"Effect");
				pControl->AddString(L"PlayerController");

				//Terrain
				pControl->AddString(L"Decal");
				pControl->AddString(L"Terrain");
				pControl->AddString(L"TerrainEditor");

				//Splash
				pControl->AddString(L"Splash");
				pControl->AddString(L"Player");
				pControl->AddString(L"Player Camera");
				pControl->AddString(L"Terrain Collider");
				pControl->AddString(L"MonsterAI");
				pControl->AddString(L"NPC_IDLE_AI");
				pControl->AddString(L"Rigidbody");
				pControl->AddString(L"Mesh Collider");

				// GameSystem
				pControl->AddString(L"GameSystem");

				pControl->AddString(L"BoneDragon AI");

				// Portal
				pControl->AddString(L"Portal");
				break;

			case CCT_SHADER:
				pControl->AddString(L"Render Target");
				pControl->AddString(L"Standard Color");
				pControl->AddString(L"Standard Tex");
				pControl->AddString(L"Colored Tex");
				pControl->AddString(L"Collider");
				pControl->AddString(L"UI");
				pControl->AddString(L"UI Collider");
				pControl->AddString(L"UI Grid");
				pControl->AddString(L"Standard Color Normal");
				pControl->AddString(L"Standard Texture Normal");
				pControl->AddString(L"Standard Bump");
				pControl->AddString(L"Rim Light");
				pControl->AddString(L"Out line");
				pControl->AddString(L"StandardBumpAnimShader");
				pControl->AddString(L"StandardTextureNormalAnimShader");
				break;

			case CCT_SCENE:
				pControl->AddString(L"Terny Day");
				pControl->AddString(L"Terny Night");
				pControl->AddString(L"Kings Road Day");
				pControl->AddString(L"Kings Road Night");
				pControl->AddString(L"Battle Field");
				break;
			}

			m_vecGameObjectControl.push_back(pControlInfo);
			return pControl;
		}
		else
		{
			delete pControl;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

CWnd * CEditorArchive::CreateSlider(CComponent * pTarget, DWORD iStyle, const POINT & LT, const POINT & RB, UINT iID, CWnd* pParent)
{
	if (m_pInspectorHandle || pParent)
	{
		CWnd* pCurrentParent = m_pInspectorHandle;
		if (pParent)
		{
			pCurrentParent = pParent;
		}

		RECT tRect = { LT.x, LT.y, RB.x, RB.y };

		CSliderCtrl* pControl = new CSliderCtrl;

		if (pControl->Create(iStyle, tRect, pCurrentParent, iID))
		{
			COMPONENTCONTROL* pControlInfo = new COMPONENTCONTROL;
			pControlInfo->hWnd = pControl;
			pControlInfo->pComponent = pTarget;
			m_vecComponentControl.push_back(pControlInfo);
			return pControl;
		}
		else
		{
			delete pControl;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}
CWnd * CEditorArchive::CreateSlider(CGameObject * pTarget, DWORD iStyle, const POINT & LT, const POINT & RB, UINT iID, CWnd* pParent)
{
	if (m_pInspectorHandle || pParent)
	{
		CWnd* pCurrentParent = m_pInspectorHandle;
		if (pParent)
		{
			pCurrentParent = pParent;
		}

		RECT tRect = { LT.x, LT.y, RB.x, RB.y };

		CSliderCtrl* pControl = new CSliderCtrl;

		if (pControl->Create(iStyle, tRect, pCurrentParent, iID))
		{
			GAMEOBJECTCONTROL* pControlInfo = new GAMEOBJECTCONTROL;
			pControlInfo->hWnd = pControl;
			pControlInfo->pGameObject = pTarget;
			m_vecGameObjectControl.push_back(pControlInfo);
			return pControl;
		}
		else
		{
			delete pControl;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

