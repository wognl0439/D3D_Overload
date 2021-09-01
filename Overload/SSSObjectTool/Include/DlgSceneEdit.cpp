// DlgSceneEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgSceneEdit.h"
#include "afxdialogex.h"
#include "Tool.h"
#include "EditorArchive.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"


// CDlgSceneEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSceneEdit, CDialogEx)

CDlgSceneEdit::CDlgSceneEdit(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SCENE, pParent)
{

}

CDlgSceneEdit::~CDlgSceneEdit()
{
}

void CDlgSceneEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LAYER, m_CBoxLayer);
	DDX_Control(pDX, IDC_BUTTON_DELETE_LAYER, m_btnDeleteLayer);
	DDX_Control(pDX, IDC_EDIT_LAYER_NAME, m_editAddLayerName);
	DDX_Control(pDX, IDC_BUTTON_ADD_LAYER, m_btnAddLayer);
	DDX_Control(pDX, IDC_LIST_COLLIDE_LAYER, m_listCollideLayer);
	DDX_Control(pDX, IDC_LIST_NONE_COLLIDE_LAYER, m_listNoneCollideLayer);
	DDX_Control(pDX, IDC_BUTTON_COLLIDE_LIST, m_btnCollideLayer);
	DDX_Control(pDX, IDC_BUTTON_NONE_COLLIDE_LIST, m_btnNoneCollideLayer);
	DDX_Control(pDX, IDC_EDIT_LAYER_ZORDER, m_editLayerZOrder);
}


BEGIN_MESSAGE_MAP(CDlgSceneEdit, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_LAYER, &CDlgSceneEdit::OnBnClickedButtonDeleteLayer)
	ON_BN_CLICKED(IDC_BUTTON_ADD_LAYER, &CDlgSceneEdit::OnBnClickedButtonAddLayer)
	ON_LBN_SELCHANGE(IDC_LIST_COLLIDE_LAYER, &CDlgSceneEdit::OnLbnSelchangeList1)
	ON_LBN_SELCHANGE(IDC_LIST_NONE_COLLIDE_LAYER, &CDlgSceneEdit::OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_BUTTON_COLLIDE_LIST, &CDlgSceneEdit::OnBnClickedButtonCollideList)
	ON_BN_CLICKED(IDC_BUTTON_NONE_COLLIDE_LIST, &CDlgSceneEdit::OnBnClickedButtonNoneCollideList)
	ON_CBN_SELCHANGE(IDC_COMBO_LAYER, &CDlgSceneEdit::OnCbnSelchangeComboLayer)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlgSceneEdit 메시지 처리기입니다.

// 레이어 딜리트 버튼 이벤트
void CDlgSceneEdit::OnBnClickedButtonDeleteLayer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CGameObject*	pSelectObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
	CScene*	pScene = pSelectObject->GetScene();

	int		iBoxSelCurSel = m_CBoxLayer.GetCurSel();
	if (iBoxSelCurSel)
	{
		CString SelLayer;
		m_CBoxLayer.GetLBText(iBoxSelCurSel, SelLayer);
		string strSelLayer = CT2CA(SelLayer);
		m_CBoxLayer.DeleteString(iBoxSelCurSel);

		vector<CLayer*>*	pvecLayer = pScene->GetLayers();

		vector<CLayer*>::iterator	iter;
		vector<CLayer*>::iterator	iterEnd = pvecLayer->end();
		for (iter = pvecLayer->begin(); iter != iterEnd; ++iter)
		{
			if ((*iter)->GetTag() == strSelLayer)
			{
				(*iter)->Destroy(true);
				m_listCollideLayer.ResetContent();
				m_listNoneCollideLayer.ResetContent();
				break;
			}
		}
	}
	SAFE_RELEASE(pScene);
}

// 레이어 추가 버튼 이벤트
void CDlgSceneEdit::OnBnClickedButtonAddLayer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CGameObject*	pSelectObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
	CScene*	pScene = pSelectObject->GetScene();
	CLayer*	pSelLayer;
	CLayer*	pCollideLayer;

	CString AddData;
	m_editAddLayerName.GetWindowTextW(AddData);
	string strSelLayer = CT2CA(AddData);

	m_CBoxLayer.AddString(AddData);
	m_listNoneCollideLayer.AddString(AddData);

	m_editLayerZOrder.GetWindowTextW(AddData);
	float fZOrder = (float)atof((CStringA)AddData);

	pScene->CreateLayer(strSelLayer, fZOrder);
	
	SAFE_RELEASE(pScene);
}

// 충돌 레이어 리스트 아이템 클릭 이벤트
void CDlgSceneEdit::OnLbnSelchangeList1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		
}

// 비충돌 레이어 리스트 아이템 클릭 이벤트
void CDlgSceneEdit::OnLbnSelchangeList2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


BOOL CDlgSceneEdit::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CGameObject*	pSelectObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
	CScene*	pScene = pSelectObject->GetScene();
	vector<CLayer*>*	pvecLayer = pScene->GetLayers();
	
	vector<CLayer*>::iterator	iter;
	vector<CLayer*>::iterator	iterEnd = pvecLayer->end();
	for (iter = pvecLayer->begin(); iter != iterEnd; ++iter)
	{
		CString	strLayer = CA2CT((*iter)->GetTag().c_str());
		m_CBoxLayer.AddString(strLayer);

		m_listNoneCollideLayer.AddString(strLayer);
	}

	m_editAddLayerName.SetWindowTextW(L"None");
	m_editLayerZOrder.SetWindowTextW(L"0");

	SAFE_RELEASE(pScene);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDlgSceneEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			GET_SINGLE(CEditorArchive)->DeleteDlgSceneEdit();
		}
		else if (pMsg->wParam == VK_RETURN)
			return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

// 충돌 레이어 리스트 버튼 클릭 이벤트
void CDlgSceneEdit::OnBnClickedButtonCollideList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iCurSel = m_listCollideLayer.GetCurSel();
	CString	strData;
	string	strLayer;
	m_listCollideLayer.GetText(iCurSel, strData);
	strLayer = CT2CA(strData);
	m_listCollideLayer.DeleteString(iCurSel);
	m_listNoneCollideLayer.AddString(strData);

	string strSelLayer;
	int iSelectLayerCurSel = m_CBoxLayer.GetCurSel();
	if (iSelectLayerCurSel >= 0)
	{
		m_CBoxLayer.GetLBText(iSelectLayerCurSel, strData);
		strSelLayer = CT2CA(strData);

		CGameObject*	pSelectObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
		CScene*	pScene = pSelectObject->GetScene();
		CLayer*	pSelLayer;
		CLayer*	pCollideLayer;

		vector<CLayer*>*	pvecLayer = pScene->GetLayers();

		vector<CLayer*>::iterator	iter;
		vector<CLayer*>::iterator	iterEnd = pvecLayer->end();
		for (iter = pvecLayer->begin(); iter != iterEnd; ++iter)
		{
			if ((*iter)->GetTag() == strSelLayer)
				pSelLayer = (*iter);
			if ((*iter)->GetTag() == strLayer)
				pCollideLayer = (*iter);
		}

		pScene->SetCollisionMatrix(pSelLayer, pCollideLayer, false);


		SAFE_RELEASE(pScene);
	}
	else
		MessageBox(L"Choose Layer ComboBox!");
}

// 비충돌 레이어 리스트 버튼 클릭 이벤트
void CDlgSceneEdit::OnBnClickedButtonNoneCollideList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int iCurSel = m_listNoneCollideLayer.GetCurSel();
	CString	strData;
	string	strLayer;
	m_listNoneCollideLayer.GetText(iCurSel, strData);
	strLayer = CT2CA(strData);
	m_listNoneCollideLayer.DeleteString(iCurSel);
	m_listCollideLayer.AddString(strData);

	string strSelLayer;
	int iSelectLayerCurSel = m_CBoxLayer.GetCurSel();
	if (iSelectLayerCurSel >= 0)
	{
		m_CBoxLayer.GetLBText(iSelectLayerCurSel, strData);
		strSelLayer = CT2CA(strData);

		CGameObject*	pSelectObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
		CScene*	pScene = pSelectObject->GetScene();
		CLayer*	pSelLayer;
		CLayer*	pNoneCollideLayer;

		vector<CLayer*>*	pvecLayer = pScene->GetLayers();

		vector<CLayer*>::iterator	iter;
		vector<CLayer*>::iterator	iterEnd = pvecLayer->end();
		for (iter = pvecLayer->begin(); iter != iterEnd; ++iter)
		{
			if ((*iter)->GetTag() == strSelLayer)
				pSelLayer = (*iter);
			if ((*iter)->GetTag() == strLayer)
				pNoneCollideLayer = (*iter);
		}

		pScene->SetCollisionMatrix(pSelLayer, pNoneCollideLayer, true);
		SAFE_RELEASE(pScene);
	}
	else
		MessageBox(L"Choose Layer ComboBox!");
}

// 레이어 콤보박스 셀 체인지 이벤트.
void CDlgSceneEdit::OnCbnSelchangeComboLayer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_listCollideLayer.ResetContent();
	m_listNoneCollideLayer.ResetContent();

	int		iBoxSelCurSel = m_CBoxLayer.GetCurSel();
	CString SelLayer;
	m_CBoxLayer.GetLBText(iBoxSelCurSel, SelLayer);
	string strSelLayer = CT2CA(SelLayer);
	CString	strData;
	CString	strLayer;

	CGameObject*	pSelectObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
	CScene*	pScene = pSelectObject->GetScene();
	CLayer*	pSelLayer;

	vector<CLayer*>*	pvecLayer = pScene->GetLayers();
	bool bFindLayer = false;

	vector<CLayer*>::iterator	iter;
	vector<CLayer*>::iterator	iterEnd = pvecLayer->end();
	for (iter = pvecLayer->begin(); iter != iterEnd;)
	{
		if (!bFindLayer)
		{
			if ((*iter)->GetTag() == strSelLayer)
			{
				bFindLayer = true;
				pSelLayer = (*iter);
				iter = pvecLayer->begin();
			}
			else
				++iter;
		}
		else
		{
			if (pScene->GetLayerCollision(pSelLayer, (*iter)))
			{
				strLayer = CA2CT((*iter)->GetTag().c_str());
				m_listCollideLayer.AddString(strLayer);
			}
			else
			{
				strLayer = CA2CT((*iter)->GetTag().c_str());
				m_listNoneCollideLayer.AddString(strLayer);
			}
			++iter;
		}
	}
	SAFE_RELEASE(pScene);
}


void CDlgSceneEdit::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//GET_SINGLE(CEditorArchive)->DeleteDlgSceneEdit();
	//OnCancel();
}


void CDlgSceneEdit::OnDestroy()
{
	CDialogEx::OnDestroy();
	CDialogEx::PostNcDestroy();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//OnCancel();
	//GET_SINGLE(CEditorArchive)->DeleteDlgSceneEdit();
}


void CDlgSceneEdit::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	GET_SINGLE(CEditorArchive)->DeleteDlgSceneEdit();
}
