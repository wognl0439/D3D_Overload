#pragma once

#include "afxext.h"
#include "GameObject.h"
#include "afxcmn.h"


//	Component 헤더
#include "Component/MeshRenderer.h"
#include "Component/SpriteRenderer.h"
#include "Material.h"
#include "Component/CircleCollider2D.h"
#include "Component/RectCollider2D.h"
#include "Component/SphereCollider.h"
#include "Component/UICollider.h"
#include "Component/Animator.h"
#include "Component/Cursor.h"
#include "Component/Camera.h"
#include "Component/DirectionalLight.h"
#include "Component/PointLight.h"
#include "Component/SpotLight.h"
#include "Component/PostProcessing.h"
#include "Component/CardinalSpline.h"
#include "Component/OutLine.h"
#include "afxwin.h"


using namespace SSS;

// CHierarchy 폼 뷰입니다.

class CHierarchy : public CFormView
{
	DECLARE_DYNCREATE(CHierarchy)
	friend class CEditorArchive;

protected:
	CHierarchy();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CHierarchy();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_OBJECT_FORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCreateObject();
	afx_msg void OnCreateUI();

public:
	virtual void OnInitialUpdate();
	void	SetWndHierarchy(CWnd*	pWnd);
	CGameObject*	FindGameObject(HTREEITEM hSelectItem);
	CGameObject*	FindGameObject(string strTag);
	HTREEITEM		FindKey(CGameObject* pSelectObject);
	void	DeleteGameObject(HTREEITEM hSelectItem);
	void	DeleteGameObject(CGameObject* pSelectObject);
	void	CheckCollider();

	// 인스펙터랑 연동할 애들
public:
	class CInspector*	m_pInspector;
	void SetInspector(CInspector* pInspector);
	void SetHierarchyItemTag(CString strTag);
	void CreateParentTreeItem(CGameObject* pObject);
	void CreateChildTreeItem(CGameObject* pParentObject);
	void LoadChildTreeItem(CGameObject* pParentObject);
	void ClearTreeItem();
	void SetSceneStartObject();

private:
	CWnd*			m_wndHierarchy;
	CGameObject*	m_pSelectObject;
	HTREEITEM		m_hSelectItem;
	bool			m_bDrag;
	bool			m_bFocus;
	bool			m_bSceneStart;

private:
	bool	MoveTreeItem(CTreeCtrl* pTree, HTREEITEM hSrcItem, HTREEITEM hDestItem);
	bool	MoveChildTreeItem(CTreeCtrl* pTree, HTREEITEM hChildItem, HTREEITEM hDestItem);
	void	DeleteChildTreeItem(HTREEITEM hParentItem);

private:
	//class CCameraMove*					m_pTargetCameraMove;
	unordered_map<HTREEITEM, CGameObject*>	m_mapGameObject;

public:
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnBnClickedButtonStop();
	CTreeCtrl m_ctrObjectTree;
	afx_msg void OnNMClickObjectTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBegindragHierarchy(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CButton m_ctrDeleteButton;
	afx_msg void OnBnClickedButtonDeleteObject();
	afx_msg void OnTvnSelchangedHierarchy(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocusHierarchy(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusHierarchy(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
};


