// DlgSceneEdit.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgSceneEdit.h"
#include "afxdialogex.h"
#include "Tool.h"
#include "EditorArchive.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"


// CDlgSceneEdit ��ȭ �����Դϴ�.

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


// CDlgSceneEdit �޽��� ó�����Դϴ�.

// ���̾� ����Ʈ ��ư �̺�Ʈ
void CDlgSceneEdit::OnBnClickedButtonDeleteLayer()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

// ���̾� �߰� ��ư �̺�Ʈ
void CDlgSceneEdit::OnBnClickedButtonAddLayer()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

// �浹 ���̾� ����Ʈ ������ Ŭ�� �̺�Ʈ
void CDlgSceneEdit::OnLbnSelchangeList1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
		
}

// ���浹 ���̾� ����Ʈ ������ Ŭ�� �̺�Ʈ
void CDlgSceneEdit::OnLbnSelchangeList2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


BOOL CDlgSceneEdit::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL CDlgSceneEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

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

// �浹 ���̾� ����Ʈ ��ư Ŭ�� �̺�Ʈ
void CDlgSceneEdit::OnBnClickedButtonCollideList()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

// ���浹 ���̾� ����Ʈ ��ư Ŭ�� �̺�Ʈ
void CDlgSceneEdit::OnBnClickedButtonNoneCollideList()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

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

// ���̾� �޺��ڽ� �� ü���� �̺�Ʈ.
void CDlgSceneEdit::OnCbnSelchangeComboLayer()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	//GET_SINGLE(CEditorArchive)->DeleteDlgSceneEdit();
	//OnCancel();
}


void CDlgSceneEdit::OnDestroy()
{
	CDialogEx::OnDestroy();
	CDialogEx::PostNcDestroy();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	//OnCancel();
	//GET_SINGLE(CEditorArchive)->DeleteDlgSceneEdit();
}


void CDlgSceneEdit::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	GET_SINGLE(CEditorArchive)->DeleteDlgSceneEdit();
}
