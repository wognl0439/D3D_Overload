// Hierarchy.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "resource.h"
#include "Hierarchy.h"
#include "Tool.h"
#include "Inspector.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"
#include "Engine.h"
#include "Transform.h"
#include "CollisionManager.h"
#include "Component.h"

#include "Texture.h"

#include "ResourcesManager.h"
#include "EditorArchive.h"
#include "TransitionNode.h"


// Client Component
#include "ClientComponent\CameraMove.h"
#include "ClientComponent\PlayerController.h"
#include "ClientComponent\BurnFX.h"

#include "SceneManager.h"

// CHierarchy

IMPLEMENT_DYNCREATE(CHierarchy, CFormView)

CHierarchy::CHierarchy()
	: CFormView(IDD_DIALOG_OBJECT_FORM),
	m_pSelectObject(NULL),
	m_bDrag(false),
	m_bFocus(false),
	m_bSceneStart(false)
{

}

CHierarchy::~CHierarchy()
{
	//if (m_mapGameObject.size() > 0)
	//{
	//	unordered_map<HTREEITEM, CGameObject*>::iterator	iter;
	//	unordered_map<HTREEITEM, CGameObject*>::iterator	iterEnd = m_mapGameObject.end();
	//
	//	for (iter = m_mapGameObject.begin(); iter != iterEnd; ++iter)
	//	{
	//		iter->second->Destroy(TRUE);
	//		//SAFE_RELEASE(iter->second);
	//	}
	//
	//	m_mapGameObject.clear();
	//}
	m_mapGameObject.clear();
}

void CHierarchy::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HIERARCHY, m_ctrObjectTree);
	DDX_Control(pDX, IDC_BUTTON_DELETE_OBJECT, m_ctrDeleteButton);
}

void CHierarchy::OnCreateObject()
{
	CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();

	CLayer*	pLayer = pScene->FindLayer("Default");

	SAFE_RELEASE(pScene);
	
	CGameObject* pObject = CGameObject::CreateObject(GET_SINGLE(CEditorArchive)->GetInspector()->GetObjectTag().c_str(), pLayer);
	HTREEITEM root_Object = m_ctrObjectTree.InsertItem(CA2CT(GET_SINGLE(CEditorArchive)->GetInspector()->GetObjectTag().c_str()));

	CGizmo*	pGizmo = GET_SINGLE(CEditorArchive)->GetGizmo();
	
	if (pGizmo == NULL)
	{
		//m_pGizmo = pObject->AddComponent<CGizmo>("Gizmo");
		//m_pGizmo->SetGizmoState(GS_TRANSFORMATION);
		//SAFE_RELEASE(m_pGizmo);
	}
	else
	{
		CGameObject*	pGizmoObj = pGizmo->GetGameObject();
		pGizmoObj->Enable(true);
		pGizmo->SetGizmoTarget(pObject);
		SAFE_RELEASE(pGizmoObj);
	}
	
	SAFE_RELEASE(pGizmo);

	m_mapGameObject.insert(make_pair(root_Object, pObject));
	
	m_pSelectObject = pObject;
	GET_SINGLE(CEditorArchive)->SetSelectObject(pObject);
	m_hSelectItem = root_Object;
	GET_SINGLE(CEditorArchive)->SetTreeItem(&m_hSelectItem);

	m_ctrObjectTree.SelectItem(NULL);

	if (m_pSelectObject)
	{
		//CGameObject*	pMainCamera = FindGameObject("Main Camera");
		//CCameraMove*	pCamera = pMainCamera->GetComponentFromTag<CCameraMove>("Camera Move");
		//pCamera->SetPlayer(m_pSelectObject);
		//SAFE_RELEASE(pCamera);

		GET_SINGLE(CEditorArchive)->GetInspector()->SetSelctObject(m_pSelectObject);

		GET_SINGLE(CEditorArchive)->GetInspector()->DrawInspector();
	}

	SAFE_RELEASE(pObject);
	SAFE_RELEASE(pLayer);

	UpdateData(FALSE);
}

void CHierarchy::OnCreateUI()
{
	CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();

	CLayer*	pLayer = pScene->FindLayer("UI");

	SAFE_RELEASE(pScene);

	CGameObject* pObject = CGameObject::CreateObject(GET_SINGLE(CEditorArchive)->GetInspector()->GetObjectTag().c_str(), pLayer);
	HTREEITEM root_Object = m_ctrObjectTree.InsertItem(CA2CT(GET_SINGLE(CEditorArchive)->GetInspector()->GetObjectTag().c_str()));

	m_mapGameObject.insert(make_pair(root_Object, pObject));
	SAFE_RELEASE(pObject);

	m_pSelectObject = FindGameObject(root_Object);
	GET_SINGLE(CEditorArchive)->SetSelectObject(m_pSelectObject);
	m_hSelectItem = root_Object;
	GET_SINGLE(CEditorArchive)->SetTreeItem(&m_hSelectItem);

	m_ctrObjectTree.SelectItem(root_Object);

	if (m_pSelectObject)
	{
		GET_SINGLE(CEditorArchive)->GetInspector()->SetSelctObject(m_pSelectObject);
		GET_SINGLE(CEditorArchive)->GetInspector()->DrawInspector();
	}

	SAFE_RELEASE(pLayer);

	UpdateData(FALSE);
}

void CHierarchy::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	CScene*	pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();

	CLayer*	pLayer = pScene->FindLayer("Default");

	{
		CGameObject*	pMainCamera = pScene->GetMainCameraObject();

		pMainCamera->SetTag("Main Camera");
		pMainCamera->SetScene(pScene);
		pMainCamera->SetLayer(pLayer);

		CCameraMove* pCameraMove = pMainCamera->GetComponent<CCameraMove>();
		if(!pCameraMove)
			pCameraMove = pMainCamera->AddComponent<CCameraMove>("Camera Move");
		SAFE_RELEASE(pCameraMove);

		HTREEITEM root_Object = FindKey(pMainCamera);
		if (!root_Object)
		{
			root_Object = m_ctrObjectTree.InsertItem(CA2CT(pMainCamera->GetTag().c_str()));
			m_mapGameObject.insert(make_pair(root_Object, pMainCamera));
		}
		SAFE_RELEASE(pMainCamera);
	}

	/*{
		CGameObject*	pLightObj = CGameObject::CreateObject("Directional Light", pLayer);
		CDirectionalLight*	pDirLight = pLightObj->AddComponent<CDirectionalLight>("Directional Light");
		HTREEITEM root_Object = m_ctrObjectTree.InsertItem(CA2CT(pLightObj->GetTag().c_str()));
		m_mapGameObject.insert(make_pair(root_Object, pLightObj));
		SAFE_RELEASE(pDirLight);
		SAFE_RELEASE(pLightObj);
	}*/

	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(pScene);

	GET_SINGLE(CEditorArchive)->CreateGizmoControl();
}

void CHierarchy::SetWndHierarchy(CWnd * pWnd)
{
	m_wndHierarchy = pWnd;
}

CGameObject * CHierarchy::FindGameObject(HTREEITEM hSelectItem)
{
	unordered_map<HTREEITEM, class CGameObject*>::iterator	iter = m_mapGameObject.find(hSelectItem);

	if (iter == m_mapGameObject.end())
		return NULL;

	//iter->second->AddRef();

	return iter->second;
}

CGameObject * CHierarchy::FindGameObject(string strTag)
{
	unordered_map<HTREEITEM, class CGameObject*>::iterator	iter;
	unordered_map<HTREEITEM, class CGameObject*>::iterator	iterEnd = m_mapGameObject.end();

	for (iter = m_mapGameObject.begin(); iter != iterEnd; ++iter)
	{
		if (iter->second->GetTag() == strTag)
		{
			return iter->second;
		}
	}

	return NULL;
}


HTREEITEM CHierarchy::FindKey(CGameObject * pSelectObject)
{
	unordered_map<HTREEITEM, class CGameObject*>::iterator	iter;
	unordered_map<HTREEITEM, class CGameObject*>::iterator	iterEnd = m_mapGameObject.end();

	for (iter = m_mapGameObject.begin(); iter != iterEnd; ++iter)
	{
		if (iter->second == pSelectObject)
		{
			return iter->first;
		}
	}

	return NULL;
}

void CHierarchy::DeleteGameObject(HTREEITEM hSelectItem)
{
	unordered_map<HTREEITEM, class CGameObject*>::iterator	iter = m_mapGameObject.find(hSelectItem);

	if (iter != m_mapGameObject.end())
	{
		CLayer* pLayer = iter->second->GetLayer();
		pLayer->RemoveGameObject(iter->second);
		SAFE_RELEASE(pLayer);

		list<CComponent*>::const_iterator iterComponent = iter->second->GetAllComponents().begin();
		list<CComponent*>::const_iterator iterComponentEnd = iter->second->GetAllComponents().end();
		for (; iterComponent != iterComponentEnd;)
		{
			(*iterComponent)->DestroyImmediate();

			iter->second->GetAllComponents().erase(iterComponent);

			iterComponent = iter->second->GetAllComponents().begin();
			iterComponentEnd = iter->second->GetAllComponents().end();
		}

		iter->second->DestroyImmediate();


		DeleteChildTreeItem(iter->first);

		//m_mapGameObject.erase(iter);
		//m_ctrObjectTree.DeleteItem(iter->first);

		GET_SINGLE(CEditorArchive)->GetInspector()->SetSelctObject(NULL);
		GET_SINGLE(CEditorArchive)->SetSelectObject(NULL);
		GET_SINGLE(CEditorArchive)->SetTreeItem(NULL);

		CGizmo*	pGizmo = GET_SINGLE(CEditorArchive)->GetGizmo();
		pGizmo->SetGizmoTarget(NULL);
		SAFE_RELEASE(pGizmo);

		m_hSelectItem = NULL;
		m_pSelectObject = NULL;
		m_ctrObjectTree.SelectItem(NULL);
	}
}

void CHierarchy::DeleteGameObject(CGameObject * pSelectObject)
{
	unordered_map<HTREEITEM, class CGameObject*>::iterator	iter;
	unordered_map<HTREEITEM, class CGameObject*>::iterator	iterEnd = m_mapGameObject.end();

	for (iter = m_mapGameObject.begin(); iter != iterEnd; ++iter)
	{
		if (iter->second == pSelectObject)
		{
			CLayer* pLayer = iter->second->GetLayer();
			pLayer->RemoveGameObject(iter->second);
			SAFE_RELEASE(pLayer);

			list<CComponent*>::const_iterator iterComponent = iter->second->GetAllComponents().begin();
			list<CComponent*>::const_iterator iterComponentEnd = iter->second->GetAllComponents().end();
			for (; iterComponent != iterComponentEnd;)
			{
				(*iterComponent)->DestroyImmediate();

				iter->second->GetAllComponents().erase(iterComponent);

				iterComponent = iter->second->GetAllComponents().begin();
				iterComponentEnd = iter->second->GetAllComponents().end();
			}

			iter->second->DestroyImmediate();

			DeleteChildTreeItem(iter->first);

			m_ctrObjectTree.DeleteItem(iter->first);
			m_mapGameObject.erase(iter);

			GET_SINGLE(CEditorArchive)->GetInspector()->SetSelctObject(NULL);
			GET_SINGLE(CEditorArchive)->SetSelectObject(NULL);
			GET_SINGLE(CEditorArchive)->SetTreeItem(NULL);

			CGizmo*	pGizmo = GET_SINGLE(CEditorArchive)->GetGizmo();
			pGizmo->SetGizmoTarget(NULL);
			SAFE_RELEASE(pGizmo);

			m_hSelectItem = NULL;
			m_pSelectObject = NULL;
			m_ctrObjectTree.SelectItem(NULL);

		}
	}
}

void CHierarchy::CheckCollider()
{	
}

void CHierarchy::SetInspector(CInspector * pInspector)
{
	m_pInspector = pInspector;
}

void CHierarchy::SetHierarchyItemTag(CString strTag)
{
	m_hSelectItem = FindKey(m_pSelectObject);
	m_ctrObjectTree.SetItemText(m_hSelectItem, strTag);
}

void CHierarchy::CreateParentTreeItem(CGameObject * pObject)
{
	HTREEITEM root_Object = m_ctrObjectTree.InsertItem(CA2CT(pObject->GetTag().c_str()));
	m_mapGameObject.insert(make_pair(root_Object, pObject));
	/*CTransform*	pParentTr = pObject->GetTransform();
	size_t	tSize = pParentTr->GetChildrenCount();
	if (tSize != 0)
	{ 
		for (unsigned int i = 0; i < tSize; ++i)
		{
			CTransform* pChildTr = pParentTr->GetChild(i);
			CGameObject* pChildObject = pChildTr->GetGameObject();
			CreateChildTreeItem(pObject, pChildObject);
			SAFE_RELEASE(pChildTr);
			SAFE_RELEASE(pChildObject);
		}
	}
	SAFE_RELEASE(pParentTr)
	SAFE_RELEASE(pObject);*/
	UpdateData(FALSE);
}

void CHierarchy::CreateChildTreeItem(CGameObject * pParentObject)
{
	HTREEITEM	hBase = FindKey(pParentObject);
	CTransform* pParentTransform = pParentObject->GetTransform();

	const vector<CTransform*>*	pChildren = pParentTransform->GetChildren();

	vector<CTransform*>::const_iterator	iter;
	vector<CTransform*>::const_iterator	iterEnd = pChildren->end();

	for (iter = pChildren->begin(); iter != iterEnd; ++iter)
	{
		CGameObject* pChild = (*iter)->GetGameObject();
		string strChildName = pChild->GetTag();
		HTREEITEM	hChildItem = m_ctrObjectTree.InsertItem(CA2CT(strChildName.c_str()));
		m_mapGameObject.insert(make_pair(hChildItem, pChild));
		MoveTreeItem(&m_ctrObjectTree, hChildItem, hBase);
		CreateChildTreeItem(pChild);
		SAFE_RELEASE(pChild);
	}

	SAFE_RELEASE(pParentTransform);
	m_ctrObjectTree.Expand(hBase, TVE_COLLAPSE);
}

void CHierarchy::LoadChildTreeItem(CGameObject * pParentObject)
{
	CreateParentTreeItem(pParentObject);
	CreateChildTreeItem(pParentObject);	
}

void CHierarchy::ClearTreeItem()
{
	m_pSelectObject = NULL;
	GET_SINGLE(CEditorArchive)->SetSelectObject(m_pSelectObject);
	m_ctrObjectTree.DeleteAllItems();
	m_mapGameObject.clear();
}

void CHierarchy::SetSceneStartObject()
{
	if (!m_bSceneStart)
	{
		m_bSceneStart = true;
		unordered_map<HTREEITEM, CGameObject*>::const_iterator	iter;
		unordered_map<HTREEITEM, CGameObject*>::const_iterator	iterEnd = m_mapGameObject.end();

		for (iter = m_mapGameObject.begin(); iter != iterEnd; ++iter)
		{
			if (iter->second->HasComponent<CAnimator>())
			{
				CAnimator*	pAnimator = iter->second->GetComponent<CAnimator>();
				if (pAnimator)
				{
					HTREEITEM	hRootItem = iter->first;

					if (m_ctrObjectTree.ItemHasChildren(hRootItem))
					{
						HTREEITEM hNextItem;
						HTREEITEM hChildItem = m_ctrObjectTree.GetChildItem(hRootItem);

						while (hChildItem != NULL)
						{
							hNextItem = m_ctrObjectTree.GetNextItem(hChildItem, TVGN_NEXT);
							{
								m_ctrObjectTree.DeleteItem(hChildItem);
								m_mapGameObject.erase(hChildItem);
							}
							hChildItem = hNextItem;
						}
					}


					vector<BONETRANSFORM>*	pBoneTransform = pAnimator->GetRegisteredBoneObject();
					vector<BONETRANSFORM>::iterator	Boneiter;
					vector<BONETRANSFORM>::iterator	BoneiterEnd = pBoneTransform->end();
					for (Boneiter = pBoneTransform->begin(); Boneiter != BoneiterEnd; ++Boneiter)
					{
						CGameObject*	pBoneObject = Boneiter->pTransform->GetGameObject();

						{
							HTREEITEM hBoneItem = m_ctrObjectTree.InsertItem(CA2CT(pBoneObject->GetTag().c_str()), 1, 1, hRootItem, TVI_LAST);
							m_mapGameObject.insert(make_pair(hBoneItem, pBoneObject));
						}

						SAFE_RELEASE(pBoneObject);
					}


					SAFE_RELEASE(pAnimator);
				}
			}
		}
	}
}

BEGIN_MESSAGE_MAP(CHierarchy, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CHierarchy::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CHierarchy::OnBnClickedButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CHierarchy::OnBnClickedButtonStop)
	ON_NOTIFY(NM_CLICK, IDC_HIERARCHY, &CHierarchy::OnNMClickObjectTree)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_HIERARCHY, &CHierarchy::OnTvnBegindragHierarchy)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_DELETE_OBJECT, &CHierarchy::OnBnClickedButtonDeleteObject)
	ON_NOTIFY(TVN_SELCHANGED, IDC_HIERARCHY, &CHierarchy::OnTvnSelchangedHierarchy)
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_NOTIFY(NM_SETFOCUS, IDC_HIERARCHY, &CHierarchy::OnNMSetfocusHierarchy)
	ON_NOTIFY(NM_KILLFOCUS, IDC_HIERARCHY, &CHierarchy::OnNMKillfocusHierarchy)
END_MESSAGE_MAP()


// CHierarchy �����Դϴ�.

#ifdef _DEBUG
void CHierarchy::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CHierarchy::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CHierarchy �޽��� ó�����Դϴ�.



// Play
void CHierarchy::OnBnClickedButtonPlay()
{
	m_pSelectObject = NULL;
	m_bSceneStart = false;
	GET_SINGLE(CEditorArchive)->SetSelectObject(NULL);
	GET_SINGLE(CEditorArchive)->RemoveGizmoControl();
	GET_SINGLE(CSceneManager)->Play();

	GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
}

// Pause
void CHierarchy::OnBnClickedButtonPause()
{
	GET_SINGLE(CSceneManager)->Pause();


	SCENE_STATE eState = GET_SINGLE(CSceneManager)->GetSceneState();

	if (eState == SS_PAUSE)
	{
		GET_SINGLE(CEditorArchive)->CreateGizmoControl();
	}
	else if (eState == SS_PLAY)
	{
		GET_SINGLE(CEditorArchive)->RemoveGizmoControl();
	}
	
	GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
}


// Stop
void CHierarchy::OnBnClickedButtonStop()
{
	m_pSelectObject = NULL;
	GET_SINGLE(CEditorArchive)->SetSelectObject(NULL);
	GET_SINGLE(CSceneManager)->Stop();
	//GET_SINGLE(CEditorArchive)->CreateGizmoControl();
	//OnInitialUpdate(); �ȿ��� ����� ���� ��
	ClearTreeItem();

	GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	
	GET_SINGLE(CEditorArchive)->ClearControls();

	CScene* pCurrentScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
	vector<CLayer*>*	pLoadLayers = pCurrentScene->GetLayers(); // No Ref
	SAFE_RELEASE(pCurrentScene);

	vector<CLayer*>::iterator	iter;
	vector<CLayer*>::iterator	iterEnd = pLoadLayers->end();

	for (iter = pLoadLayers->begin(); iter != iterEnd; ++iter)
	{
		list<CGameObject*>::const_iterator	Objiter;
		list<CGameObject*>::const_iterator	ObjiterEnd = (*iter)->GetAllGameObjects()->end();

		for (Objiter = (*iter)->GetAllGameObjects()->begin(); Objiter != ObjiterEnd; ++Objiter)
		{
			if ((*Objiter)->HasComponent<CGizmo>())
			{
				continue;
			}

			CTransform*	pObjTr = (*Objiter)->GetTransform();
			CTransform* pParentTr = pObjTr->GetParent();

			if (!pParentTr)
			{
				GET_SINGLE(CEditorArchive)->GetHierarchy()->LoadChildTreeItem(*Objiter);
			}

			SAFE_RELEASE(pObjTr);
		}
	}

	OnInitialUpdate();
}


// Object Click
void CHierarchy::OnNMClickObjectTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	m_bFocus = true;
}



// Ʈ�� ������ �巡��
void CHierarchy::OnTvnBegindragHierarchy(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
		
	// ���콺 �޽���
	SetCapture();
	
	// ���� ���õ� ������ �ڵ�
	m_hSelectItem = pNMTreeView->itemNew.hItem;
	m_bDrag = true;
	*pResult = 0;
}

// ���콺 ��Ŭ�� ��
void CHierarchy::OnLButtonUp(UINT nFlags, CPoint point)
{
	 //TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	 //�巡�� �� �̾��ٸ�
	if (m_bDrag)
	{
		// ���콺 �޽��� ĸ�� ����� ����
		ReleaseCapture();
	
		m_bDrag = false;
		// ���������� ��� ǥ�õǾ��� �׸��� �ִ��� ã�´�.
		HTREEITEM hTargetItem = m_ctrObjectTree.GetDropHilightItem();
	
		// ��� ǥ�õ� ��� �׸��� ������ ����Ѵ�.
		m_ctrObjectTree.SelectDropTarget(NULL);
	
		// ���õ� �������� �ִٸ�
		if (hTargetItem)
		{
			// ���õ� �����۰� �̵��� ���� �������� ���ٸ� �̵��� �ʿ䰡 ����.
			// �� �ٸ� ���� �̵� ��
			if (m_hSelectItem != hTargetItem)
			{
				// ���� �ڽ��� �θ� ������ �ڵ��� ���Ѵ�.
				HTREEITEM hParentItem = m_ctrObjectTree.GetNextItem(m_hSelectItem, TVGN_PARENT);
	
				// �̵��Ϸ��� ���� �ڽ��� ���� ���� �׸��̶�� �̵��� �ʿ䰡 ����.
				// �ٸ� ���� �̵� ��
				if (hParentItem != hTargetItem)
				{
					// Ʈ�� ������ �̵� ( Ʈ��, �ȱ� ������ ( �ڽ� ), �Ȱ��� ��ġ ������ ( �θ� )
					MoveTreeItem(&m_ctrObjectTree, m_hSelectItem, hTargetItem);
	
					// �̵��� ���� Ʈ�� Ȯ��
					m_ctrObjectTree.Expand(hTargetItem, TVE_EXPAND);
	
					// �̹����� Ȯ���Ѱɷ� ����
					m_ctrObjectTree.SetItemImage(hTargetItem, 1, 1);
	
					// ���� Ʈ���� ��� �������� ������ٸ� �̹��� �׸��� �⺻���� �ٲ۴�.
					HTREEITEM hItem = m_ctrObjectTree.GetChildItem(hParentItem);
					if (!hItem)
					{
						m_ctrObjectTree.SetItemImage(hParentItem, 0, 0);
					}
				}
			}
		}

		// �θ� ���� �ȵ� ( ��Ʈ�� �� )
		else if (hTargetItem == NULL)
		{
			// Ʈ�� ������ �̵� ( Ʈ��, �ȱ� ������ ( �ڽ� ), �Ȱ��� ��ġ ������ ( �θ� )
			MoveTreeItem(&m_ctrObjectTree, m_hSelectItem, hTargetItem);

			// �̵��� ���� Ʈ�� Ȯ��
			//m_ctrObjectTree.Expand(hTargetItem, TVE_EXPAND);

			// �̹����� Ȯ���Ѱɷ� ����
			//m_ctrObjectTree.SetItemImage(hTargetItem, 1, 1);

		}
	
		m_hSelectItem = NULL;
	}
	

	CFormView::OnLButtonUp(nFlags, point);
}

// ���콺 �̵� ( �ڵ鿡���� ��ġ�� Ȯ���Ѵ�. )
void CHierarchy::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	
		// �巡�׽� ������ �̹����� ������ (�巡�� ��)
		//if (m_pTreeDragImage)
		if (m_bDrag)
		{
			// Ʈ�� ��Ʈ�� ���� ���콺 ��ǥ ���
			CPoint pt = point;
			ClientToScreen(&pt);
			// Ʈ�� ��Ʈ�ѿ��� ���콺 ��ǥ�� ����
			::ScreenToClient(m_ctrObjectTree.m_hWnd, &pt); // :: ������ �����ڷ� �Ѵܰ� ���� �ö�
	
														 // ���콺�� ��ġ�� �������� �˻�. �׸��� Ʈ�� �� �׸� ���� �ִ��� Ȯ��,
														 // �ִٸ� �׸��� ��� ǥ��.
			HTREEITEM hItem = m_ctrObjectTree.HitTest(pt);
	
			// ��� ǥ�õ� �κ��� ���� ���õ� �����۰� Ʋ�� ��
			if (hItem != m_ctrObjectTree.GetDropHilightItem())
			{
				// �巡�� �̹��� �׸��� ����
				//m_pTreeDragImage->DragLeave(&m_tObjectTree);
	
				// ���ο� �׸��� ��� ǥ��
				m_ctrObjectTree.SelectDropTarget(hItem);
	
				// �巡�� �̹����� �ٽ� �����ش�.
				//m_pTreeDragImage->DragEnter(&m_tObjectTree, pt);
			}
			else
			{
				//m_pTreeDragImage->DragMove(pt);
			}
		}

	CFormView::OnMouseMove(nFlags, point);
}


// Ʈ�� ������ �̵� �Լ�
bool CHierarchy::MoveTreeItem(CTreeCtrl * pTree, HTREEITEM hSrcItem, HTREEITEM hDestItem)
{
	// �̵��� �������� ���� ( �ڽ� )
	TVITEM	TV;
	char str[256];
	ZeroMemory(str, sizeof(str));
	TV.hItem = hSrcItem;
	TV.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TV.pszText = CA2CT(str);
	TV.cchTextMax = sizeof(str);
	m_ctrObjectTree.GetItem(&TV);
	DWORD dwData = pTree->GetItemData(hSrcItem);

	// �������� �߰� ( �θ� )
	TVINSERTSTRUCT TI;
	TI.hParent = hDestItem;
	TI.hInsertAfter = TVI_LAST;
	TI.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TI.item.iImage = TV.iImage;
	TI.item.iSelectedImage = TV.iSelectedImage;
	TI.item.pszText = TV.pszText;
	HTREEITEM hItem = pTree->InsertItem(&TI);
	pTree->SetItemData(hItem, dwData);

	{
		CGameObject*	pParent = FindGameObject(hDestItem);
		CTransform*		pParentTr = NULL;
		if(pParent)
			pParentTr = pParent->GetTransform();

		CGameObject*	pChild = FindGameObject(hSrcItem);
		CTransform*		pChildTr = NULL;

		if (pChild)
		{
			pChildTr = pChild->GetTransform();

			m_mapGameObject.insert(make_pair(hItem, pChild));
			m_pSelectObject = pChild;
			GET_SINGLE(CEditorArchive)->SetSelectObject(m_pSelectObject);
			m_hSelectItem = hItem;
			GET_SINGLE(CEditorArchive)->SetTreeItem(&m_hSelectItem);
		}

		if (pParentTr)
		{
			pChildTr->SetParent(pParentTr);
			pChildTr->UpdateChildTransform();

			CGizmo*	pGizmo = GET_SINGLE(CEditorArchive)->GetGizmo();
			if (pGizmo)
			{
				CGameObject*	pGizmoObj = pGizmo->GetGameObject();
				pGizmoObj->Enable(true);
				pGizmo->SetGizmoTarget(pChild);
				SAFE_RELEASE(pGizmoObj);
			}
			SAFE_RELEASE(pGizmo);
		}
		else
			pChildTr->SetParent(NULL);

		m_mapGameObject.erase(hSrcItem);

		SAFE_RELEASE(pParentTr);
		SAFE_RELEASE(pChildTr);
	}

	// ���� �����ۿ� �ڽ� �������� �ִٸ�
	HTREEITEM hChildItem = pTree->GetChildItem(hSrcItem);
	if (hChildItem)
	{
		// �ڽ� �������� �ִٸ� ���� �̵��Ѵ�.
		MoveChildTreeItem(pTree, hChildItem, hItem);
	}

	// Ȯ�� ���θ� �˾Ƽ� �Ȱ��� �Ѵ�.
	TVITEM item;
	item.mask = TVIF_HANDLE;
	item.hItem = hSrcItem;
	pTree->GetItem(&item);
	if (item.state & TVIS_EXPANDED)
	{
		pTree->Expand(hItem, TVE_EXPAND);
	}

	// ������ ����
	pTree->SelectItem(hItem);
		

	// ���� ������ ����
	pTree->DeleteItem(hSrcItem);

	return TRUE;
}

bool CHierarchy::MoveChildTreeItem(CTreeCtrl * pTree, HTREEITEM hChildItem, HTREEITEM hDestItem)
{
	HTREEITEM hSrcItem = hChildItem;

	while (hSrcItem)
	{
		// �̵��� �������� ������ �˾Ƴ�
		TVITEM	TV;
		char	str[256];
		ZeroMemory(str, sizeof(str));
		TV.hItem = hSrcItem;
		TV.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		TV.pszText = CA2CT(str);
		TV.cchTextMax = sizeof(str);
		m_ctrObjectTree.GetItem(&TV);
		DWORD dwData = pTree->GetItemData(hSrcItem);

		// �������� �߰��Ѵ�.
		TVINSERTSTRUCT	TI;
		TI.hParent = hDestItem;
		TI.hInsertAfter = TVI_LAST;
		TI.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		TI.item.iImage = TV.iImage;
		TI.item.iSelectedImage = TV.iSelectedImage;
		TI.item.pszText = TV.pszText;
		HTREEITEM hItem = pTree->InsertItem(&TI);
		pTree->SetItemData(hItem, dwData);

		// MapKey �� �ٲٱ�
		{
			CGameObject* pChild = FindGameObject(hSrcItem);

			m_mapGameObject.insert(make_pair(hItem, pChild));
			
			m_mapGameObject.erase(hSrcItem);
		}

		// ���� �����ۿ� �ڽ� �������� �ִٸ�
		HTREEITEM hChildItem = pTree->GetChildItem(hSrcItem);

		if (hChildItem)
		{
			MoveChildTreeItem(pTree, hChildItem, hItem);
		}

		// Ȯ�� ���θ� �˾Ƽ� �Ȱ��� �Ѵ�.
		TVITEM item;
		item.mask = TVIF_HANDLE;
		item.hItem = hSrcItem;
		pTree->GetItem(&item);
		if (item.state & TVIS_EXPANDED)
		{
			pTree->Expand(hItem, TVE_EXPAND);
		}

		// ���� �������� �˾ƺ���.
		hSrcItem = pTree->GetNextItem(hSrcItem, TVGN_NEXT);
	}

	// ���� �������� �����Ѵ�.
	pTree->DeleteItem(hChildItem);

	return TRUE;
}

void CHierarchy::DeleteChildTreeItem(HTREEITEM hParentItem)
{
	//m_ctrObjectTree
	if (m_ctrObjectTree.ItemHasChildren(hParentItem))
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = m_ctrObjectTree.GetChildItem(hParentItem);

		while (hChildItem != NULL)
		{
			hNextItem = m_ctrObjectTree.GetNextItem(hChildItem, TVGN_NEXT);

			DeleteChildTreeItem(hChildItem);
			//unordered_map<HTREEITEM, class CGameObject*>::iterator	Childiter = m_mapGameObject.find(hChildItem);
			//m_mapGameObject.erase(Childiter)
			hChildItem = hNextItem;
		}

		//m_ctrObjectTree.DeleteItem(hChildItem);
	}
	unordered_map<HTREEITEM, class CGameObject*>::iterator	Childiter = m_mapGameObject.find(hParentItem);
	m_mapGameObject.erase(Childiter);
	m_ctrObjectTree.DeleteItem(hParentItem);
}

void CHierarchy::OnBnClickedButtonDeleteObject()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//m_pSelectObject = FindGameObject(m_hSelectItem);
	DeleteGameObject(m_hSelectItem);

	CGizmo*	pGizmo = GET_SINGLE(CEditorArchive)->GetGizmo();
	if (pGizmo)
	{
		CGameObject*	pGizmoObj = pGizmo->GetGameObject();
		pGizmoObj->Enable(false);
		SAFE_RELEASE(pGizmoObj);
	}
	SAFE_RELEASE(pGizmo);

	GET_SINGLE(CEditorArchive)->ClearControls();
}

// Ʈ�� ���� ������ ����
void CHierarchy::OnTvnSelchangedHierarchy(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;
		
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_bFocus)
	{
		m_hSelectItem = m_ctrObjectTree.GetSelectedItem();
		if (m_hSelectItem)
		{
			m_pSelectObject = FindGameObject(m_hSelectItem);
			GET_SINGLE(CEditorArchive)->SetSelectObject(m_pSelectObject);
			GET_SINGLE(CEditorArchive)->SetTreeItem(&m_hSelectItem);

			if (m_pSelectObject)
			{
				CSpriteRenderer*	pSpriteRenderer = m_pSelectObject->GetComponent<CSpriteRenderer>();

				// 3D Object
				if (!pSpriteRenderer)
				{
					if (m_pSelectObject->GetTag() != "Main Camera")
					{
						//CGameObject*	pMainCamera = FindGameObject("Main Camera");
						//CCameraMove*	pCamera = pMainCamera->GetComponentFromTag<CCameraMove>("Camera Move");
						//pCamera->SetPlayer(m_pSelectObject);
						//SAFE_RELEASE(pCamera);

						CGizmo*	pGizmo = GET_SINGLE(CEditorArchive)->GetGizmo();

						if (pGizmo != NULL)
						{
							CGameObject*	pGizmoObj = pGizmo->GetGameObject();
							pGizmoObj->Enable(true);
							pGizmo->SetGizmoTarget(m_pSelectObject);
							SAFE_RELEASE(pGizmoObj);
						}
						SAFE_RELEASE(pGizmo);
					}

					GET_SINGLE(CEditorArchive)->GetInspector()->SetObjectTag(m_pSelectObject->GetTag());

					GET_SINGLE(CEditorArchive)->GetInspector()->SetSelctObject(m_pSelectObject);

					GET_SINGLE(CEditorArchive)->GetInspector()->DrawInspector();

				}

				// 2D (UI)
				else
				{
					CGizmo*	pGizmo = GET_SINGLE(CEditorArchive)->GetGizmo();

					if (pGizmo != NULL)
					{
						CGameObject*	pGizmoObj = pGizmo->GetGameObject();
						pGizmoObj->Enable(false);
						SAFE_RELEASE(pGizmoObj);
					}
					SAFE_RELEASE(pGizmo);
				}

				SAFE_RELEASE(pSpriteRenderer);

			}
			UpdateData(FALSE);
		}
	}
}

// SetFocusEvent
void CHierarchy::OnNMSetfocusHierarchy(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	//m_bFocus = true;
}

// KillFocusEvent
void CHierarchy::OnNMKillfocusHierarchy(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	m_bFocus = false;
}


void CHierarchy::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}


DROPEFFECT CHierarchy::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CFormView::OnDragEnter(pDataObject, dwKeyState, point);
}
