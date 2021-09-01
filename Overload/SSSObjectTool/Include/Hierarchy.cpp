// Hierarchy.cpp : 구현 파일입니다.
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


// CHierarchy 진단입니다.

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


// CHierarchy 메시지 처리기입니다.



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
	//OnInitialUpdate(); 안에서 기즈모 생성 함
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	m_bFocus = true;
}



// 트리 아이템 드래그
void CHierarchy::OnTvnBegindragHierarchy(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		
	// 마우스 메시지
	SetCapture();
	
	// 현재 선택된 아이템 핸들
	m_hSelectItem = pNMTreeView->itemNew.hItem;
	m_bDrag = true;
	*pResult = 0;
}

// 마우스 왼클릭 업
void CHierarchy::OnLButtonUp(UINT nFlags, CPoint point)
{
	 //TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	 //드래그 중 이었다면
	if (m_bDrag)
	{
		// 마우스 메시지 캡쳐 기능을 제거
		ReleaseCapture();
	
		m_bDrag = false;
		// 마지막으로 밝게 표시되었던 항목이 있는지 찾는다.
		HTREEITEM hTargetItem = m_ctrObjectTree.GetDropHilightItem();
	
		// 밝게 표시된 드롭 항목의 선택을 취소한다.
		m_ctrObjectTree.SelectDropTarget(NULL);
	
		// 선택된 아이템이 있다면
		if (hTargetItem)
		{
			// 선택된 아이템과 이동될 곳의 아이템이 같다면 이동할 필요가 없다.
			// 즉 다를 때만 이동 됨
			if (m_hSelectItem != hTargetItem)
			{
				// 현재 자식의 부모 아이템 핸들을 구한다.
				HTREEITEM hParentItem = m_ctrObjectTree.GetNextItem(m_hSelectItem, TVGN_PARENT);
	
				// 이동하려는 곳이 자신이 직접 속한 항목이라면 이동할 필요가 없다.
				// 다를 때만 이동 됨
				if (hParentItem != hTargetItem)
				{
					// 트리 내용을 이동 ( 트리, 옴길 아이템 ( 자식 ), 옴겨질 위치 아이템 ( 부모 )
					MoveTreeItem(&m_ctrObjectTree, m_hSelectItem, hTargetItem);
	
					// 이동된 곳의 트리 확장
					m_ctrObjectTree.Expand(hTargetItem, TVE_EXPAND);
	
					// 이미지도 확장한걸로 변경
					m_ctrObjectTree.SetItemImage(hTargetItem, 1, 1);
	
					// 원본 트리의 모든 아이템이 사라졌다면 이미지 그림을 기본으로 바꾼다.
					HTREEITEM hItem = m_ctrObjectTree.GetChildItem(hParentItem);
					if (!hItem)
					{
						m_ctrObjectTree.SetItemImage(hParentItem, 0, 0);
					}
				}
			}
		}

		// 부모가 선택 안됨 ( 루트로 뺌 )
		else if (hTargetItem == NULL)
		{
			// 트리 내용을 이동 ( 트리, 옴길 아이템 ( 자식 ), 옴겨질 위치 아이템 ( 부모 )
			MoveTreeItem(&m_ctrObjectTree, m_hSelectItem, hTargetItem);

			// 이동된 곳의 트리 확장
			//m_ctrObjectTree.Expand(hTargetItem, TVE_EXPAND);

			// 이미지도 확장한걸로 변경
			//m_ctrObjectTree.SetItemImage(hTargetItem, 1, 1);

		}
	
		m_hSelectItem = NULL;
	}
	

	CFormView::OnLButtonUp(nFlags, point);
}

// 마우스 이동 ( 핸들에서의 위치를 확인한다. )
void CHierarchy::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
		// 드래그시 생성된 이미지가 있을때 (드래그 중)
		//if (m_pTreeDragImage)
		if (m_bDrag)
		{
			// 트리 컨트롤 기준 마우스 좌표 계산
			CPoint pt = point;
			ClientToScreen(&pt);
			// 트리 컨트롤에서 마우스 좌표를 받음
			::ScreenToClient(m_ctrObjectTree.m_hWnd, &pt); // :: 스코프 연산자로 한단계 위로 올라감
	
														 // 마우스가 위치한 아이템을 검사. 항목이 트리 뷰 항목 위에 있는지 확인,
														 // 있다면 항목이 밝게 표시.
			HTREEITEM hItem = m_ctrObjectTree.HitTest(pt);
	
			// 밝게 표시된 부분이 현재 선택된 아이템과 틀릴 때
			if (hItem != m_ctrObjectTree.GetDropHilightItem())
			{
				// 드래그 이미지 그리기 중지
				//m_pTreeDragImage->DragLeave(&m_tObjectTree);
	
				// 새로운 항목을 밝게 표시
				m_ctrObjectTree.SelectDropTarget(hItem);
	
				// 드래그 이미지를 다시 보여준다.
				//m_pTreeDragImage->DragEnter(&m_tObjectTree, pt);
			}
			else
			{
				//m_pTreeDragImage->DragMove(pt);
			}
		}

	CFormView::OnMouseMove(nFlags, point);
}


// 트리 아이템 이동 함수
bool CHierarchy::MoveTreeItem(CTreeCtrl * pTree, HTREEITEM hSrcItem, HTREEITEM hDestItem)
{
	// 이동할 아이템의 정보 ( 자식 )
	TVITEM	TV;
	char str[256];
	ZeroMemory(str, sizeof(str));
	TV.hItem = hSrcItem;
	TV.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TV.pszText = CA2CT(str);
	TV.cchTextMax = sizeof(str);
	m_ctrObjectTree.GetItem(&TV);
	DWORD dwData = pTree->GetItemData(hSrcItem);

	// 아이템을 추가 ( 부모 )
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

	// 현재 아이템에 자식 아이템이 있다면
	HTREEITEM hChildItem = pTree->GetChildItem(hSrcItem);
	if (hChildItem)
	{
		// 자식 아이템이 있다면 같이 이동한다.
		MoveChildTreeItem(pTree, hChildItem, hItem);
	}

	// 확장 여부를 알아서 똑같이 한다.
	TVITEM item;
	item.mask = TVIF_HANDLE;
	item.hItem = hSrcItem;
	pTree->GetItem(&item);
	if (item.state & TVIS_EXPANDED)
	{
		pTree->Expand(hItem, TVE_EXPAND);
	}

	// 아이템 선택
	pTree->SelectItem(hItem);
		

	// 기존 아이템 제거
	pTree->DeleteItem(hSrcItem);

	return TRUE;
}

bool CHierarchy::MoveChildTreeItem(CTreeCtrl * pTree, HTREEITEM hChildItem, HTREEITEM hDestItem)
{
	HTREEITEM hSrcItem = hChildItem;

	while (hSrcItem)
	{
		// 이동할 아이템의 정보를 알아냄
		TVITEM	TV;
		char	str[256];
		ZeroMemory(str, sizeof(str));
		TV.hItem = hSrcItem;
		TV.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		TV.pszText = CA2CT(str);
		TV.cchTextMax = sizeof(str);
		m_ctrObjectTree.GetItem(&TV);
		DWORD dwData = pTree->GetItemData(hSrcItem);

		// 아이템을 추가한다.
		TVINSERTSTRUCT	TI;
		TI.hParent = hDestItem;
		TI.hInsertAfter = TVI_LAST;
		TI.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		TI.item.iImage = TV.iImage;
		TI.item.iSelectedImage = TV.iSelectedImage;
		TI.item.pszText = TV.pszText;
		HTREEITEM hItem = pTree->InsertItem(&TI);
		pTree->SetItemData(hItem, dwData);

		// MapKey 값 바꾸기
		{
			CGameObject* pChild = FindGameObject(hSrcItem);

			m_mapGameObject.insert(make_pair(hItem, pChild));
			
			m_mapGameObject.erase(hSrcItem);
		}

		// 현재 아이템에 자식 아이템이 있다면
		HTREEITEM hChildItem = pTree->GetChildItem(hSrcItem);

		if (hChildItem)
		{
			MoveChildTreeItem(pTree, hChildItem, hItem);
		}

		// 확장 여부를 알아서 똑같이 한다.
		TVITEM item;
		item.mask = TVIF_HANDLE;
		item.hItem = hSrcItem;
		pTree->GetItem(&item);
		if (item.state & TVIS_EXPANDED)
		{
			pTree->Expand(hItem, TVE_EXPAND);
		}

		// 다음 아이템을 알아본다.
		hSrcItem = pTree->GetNextItem(hSrcItem, TVGN_NEXT);
	}

	// 기존 아이템을 제거한다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

// 트리 선택 아이템 변경
void CHierarchy::OnTvnSelchangedHierarchy(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;
		
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	//m_bFocus = true;
}

// KillFocusEvent
void CHierarchy::OnNMKillfocusHierarchy(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	m_bFocus = false;
}


void CHierarchy::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


DROPEFFECT CHierarchy::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::OnDragEnter(pDataObject, dwKeyState, point);
}
