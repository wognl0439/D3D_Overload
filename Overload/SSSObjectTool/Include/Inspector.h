#pragma once
#include "GameObject.h"
#include "afxwin.h"
#include "Flags.h"


//	Component 헤더
#include "Mesh.h"
#include "Component/MeshRenderer.h"
#include "Component/SpriteRenderer.h"
#include "Material.h"
#include "Component/BoxCollider.h"
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
#include "Component/Gizmo.h"
#include "Component/TerrainCollider.h"
#include "Component/Rigidbody.h"
#include "Component/MeshCollider.h"
#include "Component/SpawningPool.h"
#include "Component.h"
#include "afxcmn.h"

//Terrain
#include "Component/Terrain.h"
#include "ClientComponent/TerrainEditor.h"

// CInspector 폼 뷰입니다.

using namespace SSS;

class CInspector : public CFormView
{
	DECLARE_DYNCREATE(CInspector)

	friend class CEditorArchive;

public:
	CInspector();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CInspector();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FORM };
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
	//afx_msg void OnCreateObject();


public:
	virtual void OnInitialUpdate();
	CComboBox m_LayerCombo;
	afx_msg void OnBnClickedButtonComponentAdd();
	void	SetWndInspector(CWnd*	pWnd);

private:
	vector<CGameObject*>	m_vGameObject;
	vector<CSliderCtrl*>	m_vLightSliderEdit;

	CRect					m_rComponentRect;
	CSize					m_sDlgSize;

	CComboBox*				m_ComponentCombo;

	CWnd*					m_wndInspector;
	CDC*					m_DC;

	bool					m_bLocalTrans;

private:
	CGameObject*	m_pSelectObject;
	CEdit			m_ObjectTag;

	// 하이어라키랑 연동될 함수들
public:
	string GetObjectTag();
	void SetObjectTag(string strTag);
	void DrawInspector();
	void CheckTransform();
	void SetSelctObject(CGameObject* pObj);
	class CHierarchy*	m_pHierarchy;
	void SetHierarchy(class CHierarchy* pHierarchy);
	void SetComponentComboBox(CComboBox* pComboBox);

public:
	void RigidbodyLayout(CComponent* pComponent);
	void MeshColliderLayout(CComponent* pComponent);
	void TerrainColliderLayout(CComponent* pComponent);
	void PlayerCameraLayout(CComponent* pComponent);
	void PlayerLayout(CComponent* pComponent);
	void PlayerControllerLayout(CComponent* pComponent);
	void MonsterAILayout(CComponent* pComponent);
	void NPCIDLEAILayout(CComponent* pComponent);
	void BoneDragonAILayout(CComponent* pComponent);
	void TransformLayOut();
	void SplashLayOut(CComponent* pComponent);
	void MeshRendererLayOut(CComponent* pComponent);
	void ContainerUpdate(uint32_t iContainer, uint32_t iSubset);
	void DirectionalLightLayOut(CComponent* pComponent);
	void PointLightLayOut(CComponent* pComponent);
	void SpotLightLayOut(CComponent* pComponent);
	void CameraMoveLayOut(CComponent* pComponent);
	void RectColliderLayOut(CComponent* pComponent);
	void OutLineLayOut(CComponent* pComponent);
	void TrailLayOut(CComponent* pComponent);
	void DistortionLayOut(CComponent* pComponent);
	void FireLayOut(CComponent* pComponent);
	void WaterLayOut(CComponent* pComponent);
	void DecalLayOut(CComponent* pComponent);
	void BillboardLayOut(CComponent* pComponent);
	void SphereColliderLayOut(CComponent* pComponent);
	void BoxColliderLayOut(CComponent* pComponent);
	void CircleCollider2DLayOut(CComponent* pComponent);
	void AnimatorLayOut(CComponent* pComponent);
	void SpriteRendererLayOut(CComponent* pComponent);
	void PostProcessingLayOut(CComponent* pComponent);
	void CameraLayOut(CComponent* pComponent);
	void CardinalSplineLayOut(CComponent* pComponent);
	void EffectLayOut(CComponent* pComponent);
	void GameSystemLayOut(CComponent* pComponent);
	void PortalLayOut(CComponent* pComponent);

public:
	void	UpdateRigidbody();
	void	LoadTerrainColliderHeightTexture();
	void	EditTerrainColliderHeightTexture();
	void	SplashLoadScene();
	void	SplashClearScene();
	void	EditDirectionalLight();
	void	EditPointLight();
	void	EditSpotLight();
	void	EditTransform();
	void	EditMaterialColor();
	void	EditShaderChange();
	void	EditPortalSceneChange();
	void	EditCameraMove();
	void	EditRectCollider2D();
	void	EditOutLine();
	void	EditSphereCollider();
	void	EditBoxCollider();
	void	EditCircleCollider2D();
	void	EditPostProcessingFog();
	void	EditPostProcessingBlur();
	void	EditPostProcessingFlag();
	void	EditPostProcessingDOF();
	void	EditPostProcessingHDR();
	void	EditSpriteRenderer();
	void	EditTrail();
	afx_msg void EditNormalScale(NMHDR * pNMHDR, LRESULT * pResult);
	void	ChangeTransform();
	void	LoadMesh();
	void	SaveMesh();
	void	LoadMaterialTexture();
	void	ClearMaterialTexture();
	void	RemoveComponent();
	void	AddContainer();
	void	AddSubset();

	//Terrain

	// GameSytem
	void	EditGameSystem();
private:
	CTerrain*				m_pTerrain;
	CTerrainEditor*			m_pTerrainEditor;
	bool					m_bNaviMesh;
	//Terrain Funtion
	void TerrainLayOut(CComponent* pComponent);
	void TerrainEditorLayout(CComponent* pComponent);

	void	EditMaxHeight();
	void	EditMinHeight();
	void	EditDetailLevel();
	void	EditBrushRadius();
	void	EditBrushPower();
	void	ClickSplate();
	void	ClickHeight();
	void	ClickNone();
	void	LoadHeightMap();
	void	EditCurrentSplate();
	void	SaveAlphaTexture();
	void	SaveHeightMapForBMP();
	void	SetBaseTerrainTexture();
	void	SetNavigationCheckBox();
	void	UpdateNormal();
	void	SaveTerrain();
	void	LoadTerrain();

	void	LoadUITexture();
	void	AddRadioButton();
	void	LoadAlphaMaskTexture();
	void	SetLocalTransformfromMesh();

public:
	afx_msg void OnLbnDblclkObjectList();
	afx_msg void OnEnKillfocusEditObjectTag();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult);

	//RendererControl	m_tRC;
	afx_msg void OnCbnSelchangeCombo1();
	
	CButton m_StaticCheck;
	afx_msg void OnBnClickedStaticCheck();
};


