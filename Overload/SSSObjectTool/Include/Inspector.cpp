/// Inspector.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "Inspector.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"
#include <string.h>
#include <string>
#include "Engine.h"
#include "Transform.h"
#include "Hierarchy.h"
#include "Texture.h"
#include "Mathf.h"
#include "Debug.h"


#include "ResourcesManager.h"
#include "PathManager.h"
#include "EditorArchive.h"
#include "AnimationState.h"
#include "Component/Animator.h"
#include "AnimationClip.h"
#include "EditorArchive.h"
#include "Component/MeshRenderer.h"
#include "AnimationStateController.h"
//#include "Mesh.h"


/***********Client Component************/

#include "ClientComponent\CameraMove.h"
#include "ClientComponent\PlayerController.h"
#include "ClientComponent\Splash.h"
#include "ClientComponent\Player.h"
#include "ClientComponent\PlayerCamera.h"
#include "ClientComponent\EffectController.h"
#include "ClientComponent\MonsterAI.h"
#include "ClientComponent\NPC_IDLE_AI.h"
#include "ClientComponent\GameSystem.h"
#include "ClientComponent\BoneDragonAI.h"
#include "ClientComponent\Portal.h"
/*************************************/

#include "Component/UICheckBox.h"
#include "Component/UIRadioButton.h"
#include "Component/UIButton.h"
#include "Component/UIPanel.h"

#include "Component/MoveObj.h"
#include "Component/UISlider.h"

// Effect
#include "Component/Effect.h"
#include "Component/Trail.h"
#include "Component/Distortion.h"
#include "Component/Fire.h"
#include "Component/Water.h"

// Billboard
#include "Component/Billboard.h"
// Decal
#include "Component/Decal.h"

// CInspector

SSS_USING

IMPLEMENT_DYNCREATE(CInspector, CFormView)

CInspector::CInspector()
	: CFormView(IDD_DIALOG_FORM),
	m_rComponentRect(0),
	m_bLocalTrans(FALSE),
	m_pSelectObject(NULL),
	m_pTerrain(NULL),
	m_pTerrainEditor(NULL)
{
}

CInspector::~CInspector()
{
	//if (m_vLightSliderEdit.size() > 0)
	//{
	//	vector<CSliderCtrl*>::iterator	iter;
	//	vector<CSliderCtrl*>::iterator	iterEnd = m_vLightSliderEdit.end();
	//
	//	for (iter = m_vLightSliderEdit.begin(); iter != iterEnd; ++iter)
	//	{
	//		delete(*iter);
	//	}
	//
	//	m_vLightSliderEdit.clear();
	//}


	/*if (m_vGameObject.size() > 0)
	{
	vector<CGameObject*>::iterator	iter;
	vector<CGameObject*>::iterator	iterEnd = m_vGameObject.end();

	for (iter = m_vGameObject.begin(); iter != iterEnd; ++iter)
	{
	(*iter)->Destroy(true);
	SAFE_RELEASE((*iter));
	}
	}

	if (m_pCameraMove)
	SAFE_RELEASE(m_pCameraMove);*/
}

void CInspector::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_LayerCombo);
	DDX_Control(pDX, IDC_EDIT_OBJECT_TAG, m_ObjectTag);
	DDX_Control(pDX, IDC_STATIC_CHECK, m_StaticCheck);
}

BEGIN_MESSAGE_MAP(CInspector, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_COMPONENT_ADD, &CInspector::OnBnClickedButtonComponentAdd)
	// Transform
	ON_EN_KILLFOCUS(IDC_EDIT_POSITION_X, &CInspector::EditTransform)
	ON_EN_KILLFOCUS(IDC_EDIT_POSITION_Y, &CInspector::EditTransform)
	ON_EN_KILLFOCUS(IDC_EDIT_POSITION_Z, &CInspector::EditTransform)
	ON_EN_KILLFOCUS(IDC_EDIT_SCALE_X, &CInspector::EditTransform)
	ON_EN_KILLFOCUS(IDC_EDIT_SCALE_Y, &CInspector::EditTransform)
	ON_EN_KILLFOCUS(IDC_EDIT_SCALE_Z, &CInspector::EditTransform)
	ON_EN_KILLFOCUS(IDC_EDIT_ROTATION_X, &CInspector::EditTransform)
	ON_EN_KILLFOCUS(IDC_EDIT_ROTATION_Y, &CInspector::EditTransform)
	ON_EN_KILLFOCUS(IDC_EDIT_ROTATION_Z, &CInspector::EditTransform)
	//Directional Light
	ON_EN_CHANGE(IDC_EDIT_LIGHT_DIRECTIONAL_DIFFUSE_R, &CInspector::EditDirectionalLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_DIRECTIONAL_DIFFUSE_G, &CInspector::EditDirectionalLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_DIRECTIONAL_DIFFUSE_B, &CInspector::EditDirectionalLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_DIRECTIONAL_DIFFUSE_A, &CInspector::EditDirectionalLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_DIRECTIONAL_AMBIENT_R, &CInspector::EditDirectionalLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_DIRECTIONAL_AMBIENT_G, &CInspector::EditDirectionalLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_DIRECTIONAL_AMBIENT_B, &CInspector::EditDirectionalLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_DIRECTIONAL_AMBIENT_A, &CInspector::EditDirectionalLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_DIRECTIONAL_SPECULAR_R, &CInspector::EditDirectionalLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_DIRECTIONAL_SPECULAR_G, &CInspector::EditDirectionalLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_DIRECTIONAL_SPECULAR_B, &CInspector::EditDirectionalLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_DIRECTIONAL_SPECULAR_A, &CInspector::EditDirectionalLight)
	// Point Light
	ON_EN_CHANGE(IDC_EDIT_LIGHT_POINT_DIFFUSE_R, &CInspector::EditPointLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_POINT_DIFFUSE_G, &CInspector::EditPointLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_POINT_DIFFUSE_B, &CInspector::EditPointLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_POINT_DIFFUSE_A, &CInspector::EditPointLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_POINT_AMBIENT_R, &CInspector::EditPointLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_POINT_AMBIENT_G, &CInspector::EditPointLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_POINT_AMBIENT_B, &CInspector::EditPointLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_POINT_AMBIENT_A, &CInspector::EditPointLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_POINT_SPECULAR_R, &CInspector::EditPointLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_POINT_SPECULAR_G, &CInspector::EditPointLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_POINT_SPECULAR_B, &CInspector::EditPointLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_POINT_SPECULAR_A, &CInspector::EditPointLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_POINT_RANGE, &CInspector::EditPointLight)
	// Spot Light
	ON_EN_CHANGE(IDC_EDIT_LIGHT_SPOT_DIFFUSE_R, &CInspector::EditSpotLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_SPOT_DIFFUSE_G, &CInspector::EditSpotLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_SPOT_DIFFUSE_B, &CInspector::EditSpotLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_SPOT_DIFFUSE_A, &CInspector::EditSpotLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_SPOT_AMBIENT_R, &CInspector::EditSpotLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_SPOT_AMBIENT_G, &CInspector::EditSpotLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_SPOT_AMBIENT_B, &CInspector::EditSpotLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_SPOT_AMBIENT_A, &CInspector::EditSpotLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_SPOT_SPECULAR_R, &CInspector::EditSpotLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_SPOT_SPECULAR_G, &CInspector::EditSpotLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_SPOT_SPECULAR_B, &CInspector::EditSpotLight)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_SPOT_SPECULAR_A, &CInspector::EditSpotLight)
	// Mesh Load
	ON_BN_CLICKED(IDC_EDIT_MESHLOAD_BUTTON, &CInspector::LoadMesh)
	ON_BN_CLICKED(IDC_EDIT_MESHSAVE_BUTTON, &CInspector::SaveMesh)
	ON_BN_CLICKED(IDC_EDIT_MATERIAL_DIFFUSE_TEXTURE_BUTTON, &CInspector::LoadMaterialTexture)
	ON_BN_CLICKED(IDC_EDIT_MATERIAL_NORMAL_TEXTURE_BUTTON, &CInspector::LoadMaterialTexture)
	ON_BN_CLICKED(IDC_EDIT_MATERIAL_SPECULAR_TEXTURE_BUTTON, &CInspector::LoadMaterialTexture)
	ON_BN_CLICKED(IDC_EDIT_MATERIAL_ALPHA_TEXTURE_BUTTON, &CInspector::LoadMaterialTexture)
	ON_BN_CLICKED(IDC_EDIT_MATERIAL_DIFFUSE_CLEAR_BUTTON, &CInspector::ClearMaterialTexture)
	ON_BN_CLICKED(IDC_EDIT_MATERIAL_NORMAL_CLEAR_BUTTON, &CInspector::ClearMaterialTexture)
	ON_BN_CLICKED(IDC_EDIT_MATERIAL_SPECULAR_CLEAR_BUTTON, &CInspector::ClearMaterialTexture)
	ON_BN_CLICKED(IDC_EDIT_MATERIAL_ALPHA_CLEAR_BUTTON, &CInspector::ClearMaterialTexture)
	// Material Color
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_DIFFUSE_R, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_DIFFUSE_G, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_DIFFUSE_B, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_DIFFUSE_A, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_AMBIENT_R, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_AMBIENT_G, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_AMBIENT_B, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_AMBIENT_A, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_SPECULAR_R, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_SPECULAR_G, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_SPECULAR_B, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_SPECULAR_A, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_EMISSIVE_R, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_EMISSIVE_G, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_EMISSIVE_B, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_EMISSIVE_A, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_ALPHA_R, &CInspector::EditMaterialColor)
	ON_EN_CHANGE(IDC_EDIT_MATERIAL_ALPHA_G, &CInspector::EditMaterialColor)
ON_EN_CHANGE(IDC_EDIT_MATERIAL_ALPHA_B, &CInspector::EditMaterialColor)
ON_EN_CHANGE(IDC_EDIT_MATERIAL_ALPHA_A, &CInspector::EditMaterialColor)
//Material Container
ON_BN_CLICKED(IDC_BUTTON_MATERIAL_CONTAINER_ADD, &CInspector::AddContainer)
ON_BN_CLICKED(IDC_BUTTON_MATERIAL_SUBSET_ADD, &CInspector::AddSubset)
// SpriteRenderer
ON_EN_CHANGE(IDC_EDIT_SPRITERENDERER_ZORDER, &CInspector::EditSpriteRenderer)
ON_EN_CHANGE(IDC_EDIT_SPRITERENDERER_DIFFUSE_R, &CInspector::EditSpriteRenderer)
ON_EN_CHANGE(IDC_EDIT_SPRITERENDERER_DIFFUSE_G, &CInspector::EditSpriteRenderer)
ON_EN_CHANGE(IDC_EDIT_SPRITERENDERER_DIFFUSE_B, &CInspector::EditSpriteRenderer)
ON_EN_CHANGE(IDC_EDIT_SPRITERENDERER_DIFFUSE_A, &CInspector::EditSpriteRenderer)
// RectCollider2D
ON_EN_CHANGE(IDC_EDIT_RECTCOLLIDE2D_PIVOT_X, &CInspector::EditRectCollider2D)
ON_EN_CHANGE(IDC_EDIT_RECTCOLLIDE2D_PIVOT_Y, &CInspector::EditRectCollider2D)
ON_EN_CHANGE(IDC_EDIT_RECTCOLLIDE2D_PIVOT_Z, &CInspector::EditRectCollider2D)
ON_EN_CHANGE(IDC_EDIT_RECTCOLLIDE2D_SCALE_X, &CInspector::EditRectCollider2D)
ON_EN_CHANGE(IDC_EDIT_RECTCOLLIDE2D_SCALE_Y, &CInspector::EditRectCollider2D)
ON_EN_CHANGE(IDC_EDIT_RECTCOLLIDE2D_SCALE_Z, &CInspector::EditRectCollider2D)
// OutLine
ON_EN_CHANGE(IDC_EDIT_OUTLINE_OFFSET_X, &CInspector::EditOutLine)
ON_EN_CHANGE(IDC_EDIT_OUTLINE_OFFSET_Y, &CInspector::EditOutLine)
ON_EN_CHANGE(IDC_EDIT_OUTLINE_OFFSET_Z, &CInspector::EditOutLine)
ON_EN_CHANGE(IDC_EDIT_OUTLINE_COLOR_R, &CInspector::EditOutLine)
ON_EN_CHANGE(IDC_EDIT_OUTLINE_COLOR_G, &CInspector::EditOutLine)
ON_EN_CHANGE(IDC_EDIT_OUTLINE_COLOR_B, &CInspector::EditOutLine)
ON_EN_CHANGE(IDC_EDIT_OUTLINE_COLOR_A, &CInspector::EditOutLine)
ON_EN_CHANGE(IDC_EDIT_OUTLINE_SIZE, &CInspector::EditOutLine)
// SphereCollider
ON_EN_CHANGE(IDC_EDIT_SPHERECOLLIDER_RADIUS, &CInspector::EditSphereCollider)
ON_EN_CHANGE(IDC_EDIT_SPHERECOLLIDER_LOCAL_X, &CInspector::EditSphereCollider)
ON_EN_CHANGE(IDC_EDIT_SPHERECOLLIDER_LOCAL_Y, &CInspector::EditSphereCollider)
ON_EN_CHANGE(IDC_EDIT_SPHERECOLLIDER_LOCAL_Z, &CInspector::EditSphereCollider)
// BoxCollider
ON_EN_CHANGE(IDC_EDIT_BOXCOLLIDER_VOLUME_X, &CInspector::EditBoxCollider)
ON_EN_CHANGE(IDC_EDIT_BOXCOLLIDER_VOLUME_Y, &CInspector::EditBoxCollider)
ON_EN_CHANGE(IDC_EDIT_BOXCOLLIDER_VOLUME_Z, &CInspector::EditBoxCollider)
ON_EN_CHANGE(IDC_EDIT_BOXCOLLIDER_LOCAL_X, &CInspector::EditBoxCollider)
ON_EN_CHANGE(IDC_EDIT_BOXCOLLIDER_LOCAL_Y, &CInspector::EditBoxCollider)
ON_EN_CHANGE(IDC_EDIT_BOXCOLLIDER_LOCAL_Z, &CInspector::EditBoxCollider)
// CircleCollider2D
ON_EN_CHANGE(IDC_EDIT_CIRCLECOLLIDER2D_RADIUS, &CInspector::EditCircleCollider2D)
// PostProcessing
ON_EN_CHANGE(IDC_EDIT_POSTPROCESSING_FOG_R, &CInspector::EditPostProcessingFog)
ON_EN_CHANGE(IDC_EDIT_POSTPROCESSING_FOG_G, &CInspector::EditPostProcessingFog)
ON_EN_CHANGE(IDC_EDIT_POSTPROCESSING_FOG_B, &CInspector::EditPostProcessingFog)
ON_EN_CHANGE(IDC_EDIT_POSTPROCESSING_FOG_A, &CInspector::EditPostProcessingFog)
ON_EN_CHANGE(IDC_EDIT_POSTPROCESSING_FOG_MIN, &CInspector::EditPostProcessingFog)
ON_EN_CHANGE(IDC_EDIT_POSTPROCESSING_FOG_MAX, &CInspector::EditPostProcessingFog)
ON_BN_CLICKED(IDC_BUTTON_POSTPROCESSING_ANTIALIASING, &CInspector::EditPostProcessingFlag)
ON_BN_CLICKED(IDC_BUTTON_POSTPROCESSING_EYE_ADAPTION, &CInspector::EditPostProcessingFlag)
ON_BN_CLICKED(IDC_BUTTON_POSTPROCESSING_BLOOM, &CInspector::EditPostProcessingFlag)
ON_BN_CLICKED(IDC_BUTTON_POSTPROCESSING_FOG, &CInspector::EditPostProcessingFlag)
ON_BN_CLICKED(IDC_BUTTON_POSTPROCESSING_HDR, &CInspector::EditPostProcessingFlag)
ON_BN_CLICKED(IDC_BUTTON_POSTPROCESSING_MOTION_BLUR, &CInspector::EditPostProcessingFlag)
ON_BN_CLICKED(IDC_BUTTON_POSTPROCESSING_AMBIENT_OCCLUSION, &CInspector::EditPostProcessingFlag)
ON_BN_CLICKED(IDC_BUTTON_POSTPROCESSING_DEPTH_OF_FIELD, &CInspector::EditPostProcessingFlag)
ON_EN_CHANGE(IDC_EDIT_POSTPROCESSING_SAMPLECOUNT, &CInspector::EditPostProcessingBlur)
ON_EN_CHANGE(IDC_EDIT_POSTPROCESSING_DEPTH_OF_FIELD_MIN, &CInspector::EditPostProcessingDOF)
ON_EN_CHANGE(IDC_EDIT_POSTPROCESSING_DEPTH_OF_FIELD_MAX, &CInspector::EditPostProcessingDOF)
ON_EN_CHANGE(IDC_EDIT_POSTPROCESSING_DEPTH_OF_FIELD_SIGMA, &CInspector::EditPostProcessingDOF)
ON_EN_CHANGE(IDC_EDIT_POSTPROCESSING_HDR_EXPOSURE, &CInspector::EditPostProcessingHDR)
ON_EN_CHANGE(IDC_EDIT_POSTPROCESSING_HDR_WHITETHRESHOLD, &CInspector::EditPostProcessingHDR)
// Shader ComboBox		
ON_CBN_SELCHANGE(IDC_EDIT_SHADER_COMBOBOX, &CInspector::EditShaderChange)
// Portal Scene ComboBox
ON_CBN_SELCHANGE(IDC_EDIT_SCENE_COMBOBOX, &CInspector::EditPortalSceneChange)
// Camera Target
ON_EN_KILLFOCUS(IDC_EDIT_CAMERA_TARGET, &CInspector::EditCameraMove)
ON_EN_KILLFOCUS(IDC_EDIT_CAMERA_MOVESPEED, &CInspector::EditCameraMove)
// Transform Mode Change
ON_BN_CLICKED(IDC_EDIT_TRANSFORM_LOCAL_BUTTON, &CInspector::ChangeTransform)
// Tag Change
ON_EN_KILLFOCUS(IDC_EDIT_OBJECT_TAG, &CInspector::OnEnKillfocusEditObjectTag)
// Component Delete
ON_BN_CLICKED(IDC_EDIT_COMPONENT_ERASE_BUTTON, &CInspector::RemoveComponent)

	// Terrain
	ON_EN_CHANGE(IDC_EDIT_HEIGHTMAX, &CInspector::EditMaxHeight)
	ON_EN_CHANGE(IDC_EDIT_HEIGHTMIN, &CInspector::EditMinHeight)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHTMAP, &CInspector::LoadHeightMap)
	ON_EN_CHANGE(IDC_EDIT_BRUSH, &CInspector::EditBrushRadius)
	ON_EN_CHANGE(IDC_EDIT_DETAIL, &CInspector::EditDetailLevel)
	ON_BN_CLICKED(IDC_RADIO_BUTTON_SPLATE, &CInspector::ClickSplate)
	ON_BN_CLICKED(IDC_RADIO_BUTTON_HEIGHT, &CInspector::ClickHeight)
	ON_BN_CLICKED(IDC_RADIO_BUTTON_NONE, &CInspector::ClickNone)
	ON_EN_CHANGE(IDC_EDIT_CURRENT_SPLATE, &CInspector::EditCurrentSplate)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_ALPHA, &CInspector::SaveAlphaTexture)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_HEIGHT, &CInspector::SaveHeightMapForBMP)
	ON_BN_CLICKED(IDC_BUTTON_NAVIGATION, &CInspector::SetNavigationCheckBox)
	ON_BN_CLICKED(IDC_BUTTON_TERRAIN_SAVE, &CInspector::SaveTerrain)
	ON_BN_CLICKED(IDC_BUTTON_TERRAIN_LOAD, &CInspector::LoadTerrain)
	ON_BN_CLICKED(IDC_BUTTON_TERRAIN_NORMAL, &CInspector::UpdateNormal)
	ON_BN_CLICKED(IDC_BUTTON_LOCAL_TRANSFORM, &CInspector::SetLocalTransformfromMesh)
	ON_EN_CHANGE(IDC_EDIT_BRUSH_POWER, &CInspector::EditBrushPower)

	//UIComponent
	ON_BN_CLICKED(IDC_LOADTEXTURE_BUTTON, &CInspector::LoadUITexture)
	ON_BN_CLICKED(IDC_ADD_RADIOBUTTON, &CInspector::AddRadioButton)
	ON_BN_CLICKED(IDC_LOAD_ALPHAMASK_TEXTURE, &CInspector::LoadAlphaMaskTexture)

	// Terrain
	ON_EN_CHANGE(IDC_EDIT_HEIGHTMAX, &CInspector::EditMaxHeight)
	ON_EN_CHANGE(IDC_EDIT_HEIGHTMIN, &CInspector::EditMinHeight)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHTMAP, &CInspector::LoadHeightMap)
	ON_EN_CHANGE(IDC_EDIT_BRUSH, &CInspector::EditBrushRadius)
	ON_EN_CHANGE(IDC_EDIT_DETAIL, &CInspector::EditDetailLevel)
	ON_BN_CLICKED(IDC_RADIO_BUTTON_SPLATE, &CInspector::ClickSplate)
	ON_BN_CLICKED(IDC_RADIO_BUTTON_HEIGHT, &CInspector::ClickHeight)
	ON_EN_CHANGE(IDC_EDIT_CURRENT_SPLATE, &CInspector::EditCurrentSplate)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_ALPHA, &CInspector::SaveAlphaTexture)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_HEIGHT, &CInspector::SaveHeightMapForBMP)
	ON_BN_CLICKED(IDC_BUTTON_NAVIGATION, &CInspector::SetNavigationCheckBox)
	ON_BN_CLICKED(IDC_BUTTON_TERRAIN_SAVE, &CInspector::SaveTerrain)
	ON_BN_CLICKED(IDC_BUTTON_TERRAIN_LOAD, &CInspector::LoadTerrain)
	ON_BN_CLICKED(IDC_BUTTON_TERRAIN_NORMAL, &CInspector::UpdateNormal)
	ON_BN_CLICKED(IDC_BUTTON_LOCAL_TRANSFORM, &CInspector::SetLocalTransformfromMesh)
	ON_EN_CHANGE(IDC_EDIT_BRUSH_POWER, &CInspector::EditBrushPower)
	//UIComponent
	ON_BN_CLICKED(IDC_LOADTEXTURE_BUTTON, &CInspector::LoadUITexture)
	ON_BN_CLICKED(IDC_ADD_RADIOBUTTON, &CInspector::AddRadioButton)
	ON_BN_CLICKED(IDC_LOAD_ALPHAMASK_TEXTURE, &CInspector::LoadAlphaMaskTexture)

// Terrain
ON_EN_CHANGE(IDC_EDIT_HEIGHTMAX, &CInspector::EditMaxHeight)
ON_EN_CHANGE(IDC_EDIT_HEIGHTMIN, &CInspector::EditMinHeight)
ON_BN_CLICKED(IDC_BUTTON_HEIGHTMAP, &CInspector::LoadHeightMap)
ON_EN_CHANGE(IDC_EDIT_BRUSH, &CInspector::EditBrushRadius)
ON_EN_CHANGE(IDC_EDIT_DETAIL, &CInspector::EditDetailLevel)
ON_BN_CLICKED(IDC_RADIO_BUTTON_SPLATE, &CInspector::ClickSplate)
ON_BN_CLICKED(IDC_RADIO_BUTTON_HEIGHT, &CInspector::ClickHeight)
ON_EN_CHANGE(IDC_EDIT_CURRENT_SPLATE, &CInspector::EditCurrentSplate)
ON_BN_CLICKED(IDC_BUTTON_SAVE_ALPHA, &CInspector::SaveAlphaTexture)
ON_BN_CLICKED(IDC_BUTTON_SAVE_HEIGHT, &CInspector::SaveHeightMapForBMP)
ON_BN_CLICKED(IDC_BUTTON_NAVIGATION, &CInspector::SetNavigationCheckBox)
ON_BN_CLICKED(IDC_BUTTON_TERRAIN_SAVE, &CInspector::SaveTerrain)
ON_BN_CLICKED(IDC_BUTTON_TERRAIN_LOAD, &CInspector::LoadTerrain)
ON_BN_CLICKED(IDC_BUTTON_TERRAIN_NORMAL, &CInspector::UpdateNormal)
ON_BN_CLICKED(IDC_BUTTON_LOCAL_TRANSFORM, &CInspector::SetLocalTransformfromMesh)
ON_EN_CHANGE(IDC_EDIT_BRUSH_POWER, &CInspector::EditBrushPower)
//UIComponent
ON_BN_CLICKED(IDC_LOADTEXTURE_BUTTON, &CInspector::LoadUITexture)
ON_BN_CLICKED(IDC_ADD_RADIOBUTTON, &CInspector::AddRadioButton)
ON_BN_CLICKED(IDC_LOAD_ALPHAMASK_TEXTURE, &CInspector::LoadAlphaMaskTexture)
	ON_EN_CHANGE(IDC_EDIT_BRUSH_POWER, &CInspector::EditBrushPower)
	//Effect
	ON_EN_CHANGE(IDC_EDIT_TRAIL_SIZE, &CInspector::EditTrail)
//Terrain Collider
ON_BN_CLICKED(IDC_BUTTON_LOAD_TERRAIN_COLLIDER_HEIGHT_TEXTURE, &CInspector::LoadTerrainColliderHeightTexture)
//Rigidbody
ON_BN_CLICKED(IDC_BUTTON_RIGIDBODY_IS_KINEMATIC, &CInspector::UpdateRigidbody)
ON_BN_CLICKED(IDC_BUTTON_RIGIDBODY_USE_GRAVITY, &CInspector::UpdateRigidbody)
ON_EN_CHANGE(IDC_EDIT_RIGIDBODY_MASS, &CInspector::UpdateRigidbody)
ON_EN_CHANGE(IDC_EDIT_RIGIDBODY_DRAG, &CInspector::UpdateRigidbody)
ON_EN_CHANGE(IDC_EDIT_RIGIDBODY_ANGULAR_DRAG, &CInspector::UpdateRigidbody)
//MeshCollider

// GameSystem
ON_EN_KILLFOCUS(IDC_EDIT_GAMESYSTEM_SCENE_ID, &CInspector::EditGameSystem)

	//Splash Component
	ON_BN_CLICKED(IDC_BUTTON_LOAD_TARGET_SCENE, &CInspector::SplashLoadScene)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_TARGET_SCENE, &CInspector::SplashClearScene)

ON_WM_LBUTTONDOWN()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MATERIAL_CONTAINER_SIZE, &CInspector::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MATERIAL_SUBSET_SIZE, &CInspector::OnDeltaposSpin2)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CInspector::OnCbnSelchangeCombo1)
ON_WM_KEYDOWN()
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_MATERIAL_NORMAL_SCALE, &CInspector::EditNormalScale)
	ON_BN_CLICKED(IDC_STATIC_CHECK, &CInspector::OnBnClickedStaticCheck)
END_MESSAGE_MAP()


// CInspector 진단입니다.

#ifdef _DEBUG
void CInspector::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CInspector::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void CInspector::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_sDlgSize.cx = 100;
	m_sDlgSize.cy = 100;

	//SetScrollSizes(MM_TEXT, m_sDlgSize);


	//Layer
	//m_LayerCombo.AddString(L"Default");
	//m_LayerCombo.AddString(L"UI");

	// 오브젝트 태그 초기 텍스트
	m_ObjectTag.SetWindowTextW(L"ObjectTag");

	m_DC = GetDC();

	UpdateData(FALSE);
}


// Object에 Component 추가
void CInspector::OnBnClickedButtonComponentAdd()
{
	CString strComponent;

	int iIndex = m_ComponentCombo->GetCurSel();	// 컴포넌트 콤보에서 현재 선택된 놈의 인덱스를 가져온다.
	m_ComponentCombo->GetLBText(iIndex, strComponent);	// 해당 인덱스의 컴포넌트 이름을 가져온다.
														// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_pSelectObject)
	{
		// 현재 리스트에서 선택 된 오브젝트에 해당 인덱스 컴포넌트를 추가해야한다.
		switch (iIndex)
		{
		case	OCT_RIGIDBODY:
		{
			CRigidbody* pComponent = m_pSelectObject->AddComponent<CRigidbody>("Rigidbody");
			SAFE_RELEASE(pComponent);
		}break;
		case OCT_MESHCOLLIDER: 
		{
			CMeshCollider* pComponent = m_pSelectObject->AddComponent<CMeshCollider>("Mesh Collider");
			SAFE_RELEASE(pComponent);
		}break;
		case OCT_TERRAINCOLLIDER:
		{
			CTerrainCollider* pComponent = m_pSelectObject->AddComponent<CTerrainCollider>("Terrain Collider");
			SAFE_RELEASE(pComponent);
		}break;
		case OCT_PLAYER:
		{
			CPlayer* pComponent = m_pSelectObject->AddComponent<CPlayer>("Player");
			SAFE_RELEASE(pComponent);
			
		}break;
		case OCT_PLAYER_CAMERA:
		{
			CPlayerCamera* pComponent = m_pSelectObject->AddComponent<CPlayerCamera>("Player Camera");
			SAFE_RELEASE(pComponent);
		}break;
		case OCT_SPLASH:
		{
			CSplash*	pComponent = m_pSelectObject->AddComponent<CSplash>("Splash");
			SAFE_RELEASE(pComponent);
		}break;
			// Renderer
		case OCT_MESHRENDERER:
		{
			CMeshRenderer*	pMeshRenderer = m_pSelectObject->AddComponent<CMeshRenderer>("Mesh Renderer");
			pMeshRenderer->SetMesh(PRIMITIVE_MESH_PYRAMID, true);
			SAFE_RELEASE(pMeshRenderer);
		}
		break;

		case OCT_SPRITERENDERER:
		{
			CSpriteRenderer*	pSpriteRenderer = m_pSelectObject->AddComponent<CSpriteRenderer>("Sprite Renderer");
			SAFE_RELEASE(pSpriteRenderer);
		}
		break;


		//Coliider
		case OCT_BOXCOLLIDER:
		{
			CBoxCollider*	pBoxCollider = m_pSelectObject->AddComponent<CBoxCollider>("BoxCollider");
			Vector3	vVolume = { 1,1,1 };
			pBoxCollider->SetVolume(vVolume);
			SAFE_RELEASE(pBoxCollider);
		}
		break;

		case OCT_CIRCLECOLLIDER2D:
		{
			CCircleCollider2D*	pCircleCollider2D = m_pSelectObject->AddComponent<CCircleCollider2D>("CircleCollider2D");
			pCircleCollider2D->SetRadius(1.0f);
			SAFE_RELEASE(pCircleCollider2D);
		}
		break;

		case OCT_RECTCOLLIDER2D:
		{
			CRectCollider2D*	pRectCollider2D = m_pSelectObject->AddComponent<CRectCollider2D>("RectCollider2D");
			Vector3	vVolume = { 1,1,1 };
			pRectCollider2D->SetScale(vVolume);
			SAFE_RELEASE(pRectCollider2D);
		}
		break;

		case OCT_SPHERECOLLIDER:
		{
			CSphereCollider*	pSphereCollider = m_pSelectObject->AddComponent<CSphereCollider>("SphereCollider");
			pSphereCollider->SetRadius(1.0f);
			SAFE_RELEASE(pSphereCollider);
		}
		break;

		case OCT_UICOLLIDER:
		{
			CUICollider*	pUICollider = m_pSelectObject->AddComponent<CUICollider>("UICollider");
			SAFE_RELEASE(pUICollider);
			CMoveObj* pMove = m_pSelectObject->AddComponent<CMoveObj>("UIMove");
			SAFE_RELEASE(pMove);
		}
		break;


		// Animator
		case OCT_ANIMATOR:
		{
			CAnimator*	pAnimator = m_pSelectObject->AddComponent<CAnimator>("Animator");
			CGameObject* pTempTarget = m_pSelectObject;
			if (pAnimator)
			{
				pAnimator->CreateAnimationController();
				pAnimator->CreateBoneObject();
				vector<class CTransform*>* m_vecBoneTransform = pAnimator->GetBoneTransform();
				
				if (m_vecBoneTransform && m_vecBoneTransform->size() > 0)
				{
					GET_SINGLE(CEditorArchive)->GetHierarchy()->CreateChildTreeItem(m_pSelectObject);
				}
			}		
			SAFE_RELEASE(pAnimator);
			GET_SINGLE(CEditorArchive)->SetSelectObject(pTempTarget);
		}
		break;

		// UI		
		case OCT_PANEL:
		{
			CUIPanel* pUI = m_pSelectObject->AddComponent<CUIPanel>("UIPanel");
			SAFE_RELEASE(pUI);
		}
		break;

		case OCT_BUTTON:
		{
			CUIButton* pButton = m_pSelectObject->AddComponent<CUIButton>("UIButton");
			SAFE_RELEASE(pButton);
		}
		break;

		case OCT_CHECKBOX:
		{
			CUICheckBox* pCheckBox = m_pSelectObject->AddComponent<CUICheckBox>("UICheckBox");
			SAFE_RELEASE(pCheckBox);
		}
		break;

		case OCT_RADIOBUTTON:
		{
			CUIRadioButton* pRadioBtn = m_pSelectObject->AddComponent<CUIRadioButton>("UIRadioButton");
			SAFE_RELEASE(pRadioBtn);
		}
		break;

		case OCT_ALPHAMASK:
		{
			//CEffectController*	pEffectCtrl = m_pSelectObject->AddComponent<CEffectController>("EffectCtrl");
			//SAFE_RELEASE(pEffectCtrl);
		}
		break;

		case OCT_SLIDER:
		{
			CUISlider* pSlider = m_pSelectObject->AddComponent<CUISlider>("Slider");

			SAFE_RELEASE(pSlider);
		}
		break;

		// Camera
		case OCT_CAMERA:
		{
			CCamera*	pCamera = m_pSelectObject->AddComponent<CCamera>("Camera");
			SAFE_RELEASE(pCamera);
		}
		break;

		// Text
		case OCT_TEXT:
		{
		}
		break;

		// Light
		case OCT_DIRECTIONALLIGHT:
		{
			CDirectionalLight* pLight = m_pSelectObject->AddComponent<CDirectionalLight>("Directional Light");
			SAFE_RELEASE(pLight);
		}
		break;

		case OCT_POINTLIGHT:
		{
			CPointLight* pLight = m_pSelectObject->AddComponent<CPointLight>("Point Light");
			SAFE_RELEASE(pLight);
		}
		break;

		case OCT_SPOTLIGHT:
		{
			CSpotLight* pLight = m_pSelectObject->AddComponent<CSpotLight>("Spot Light");
			SAFE_RELEASE(pLight);
		}
		break;

		// Post Process
		case OCT_POSTPROCESSING:
		{
			CPostProcessing* pPost = m_pSelectObject->AddComponent<CPostProcessing>("PostProcessing");

			pPost->SetPostProcessingFlags(PT_FOG | PT_DEPTH_OF_FIELD);
			pPost->SetFogColor(Vector4(0.5f, 0.75f, 1.0f, 1.0f));
			pPost->SetFogDistance(100.0f, 200.0f);
			pPost->SetBlurSampleCount(5);
			pPost->SetDepthOfField(100.0f, 200.0f, 50.0f);
			pPost->SetHDRExposure(1.0f);
			pPost->SetAdaptionInterval(2.0f);

			SAFE_RELEASE(pPost);
		}
		break;

		// Spline
		case OCT_CARDINALSPLINE:
		{
			CCardinalSpline* pCardinal = m_pSelectObject->AddComponent<CCardinalSpline>("Cardinal Spline");
			SAFE_RELEASE(pCardinal);
		}
		break;

		// Effects
		case OCT_OUTLINE:
		{
			CMeshRenderer*	pMeshRenderer = m_pSelectObject->GetComponent<CMeshRenderer>();
			if (pMeshRenderer)
			{
				COutLine* pOutLine = m_pSelectObject->AddComponent<COutLine>("Out Line");
				SAFE_RELEASE(pOutLine);
				SAFE_RELEASE(pMeshRenderer);
			}
			else
			{
				MessageBox(L"MeshRenderer가 없습니다.", L"Not Found");
			}
		}
		break;

		case OCT_TRAIL:
		{
			CTrail*	pTrailComponent = m_pSelectObject->AddComponent<CTrail>("Trail");
			SAFE_RELEASE(pTrailComponent);
		}
		break;

		case OCT_DISTORTION:
		{
			CDistortion*	pDistortionComponent = m_pSelectObject->AddComponent<CDistortion>("Distortion");
			SAFE_RELEASE(pDistortionComponent);
		}
		break;

		case OCT_FIRE:
		{
			CFire*	pFireComponent = m_pSelectObject->AddComponent<CFire>("Fire");
			SAFE_RELEASE(pFireComponent);
		}
		break;

		case OCT_WATER:
		{
			CWater*	pWaterComponent = m_pSelectObject->AddComponent<CWater>("Water");
			SAFE_RELEASE(pWaterComponent);
		}
		break;

		case OCT_GIZMO:
		{
			CGizmo* pGizmo = m_pSelectObject->AddComponent<CGizmo>("Gizmo");
			SAFE_RELEASE(pGizmo);
		}
		break;

		// Client
		case OCT_CAMERAMOVE:
		{
			CCameraMove* pCameraMove = m_pSelectObject->AddComponent<CCameraMove>("Camera Move");
			SAFE_RELEASE(pCameraMove);
		}
		break;
		case OCT_CHAT:
			break;
		case OCT_EFFECT:
		{
			CMeshRenderer*	pMeshRenderer = m_pSelectObject->GetComponent<CMeshRenderer>();
			if (pMeshRenderer)
			{
				CEffect* pEffect = m_pSelectObject->AddComponent<CEffect>("Effect");
				SAFE_RELEASE(pEffect);
				SAFE_RELEASE(pMeshRenderer);
			}
			else
			{
				MessageBox(L"MeshRenderer가 없습니다.", L"Not Found");
			}

		}
		break;

		case OCT_PLAYERCONTROLLER:
		{
			CPlayerController* pPlayerController = m_pSelectObject->AddComponent<CPlayerController>("Player Controller");
			SAFE_RELEASE(pPlayerController);
		}
		break;

		case OCT_DECAL:
		{
			CDecal* pDecal = m_pSelectObject->AddComponent<CDecal>("Decal");
			SAFE_RELEASE(pDecal);
		}
		break;

		case OCT_TERRAIN:
		{
			CTerrain* pTerrainComponent = m_pSelectObject->AddComponent<CTerrain>("Terrain");

			pTerrainComponent->LoadHeightMap("Default Height", 30.f, "Terrains/Default Height.bmp");
			pTerrainComponent->SetBaseDiffuseTexture("TerrainBaseTex", L"Terrains\\BD_Terrain_Cliff05.dds");
			pTerrainComponent->SetBaseNormalTexture("TerrainBaseNrmTex", L"Terrains\\BD_Terrain_Cliff05_NRM.bmp");
			pTerrainComponent->SetBaseSpecularTexture("TerrainBaseTex", L"Terrains\\BD_Terrain_Cliff05_SPEC.bmp");

			vector<wstring>	vecMultiDif;
			vector<wstring>	vecMultiNrm;
			vector<wstring>	vecMultiSpc;
			vector<wstring>	vecMultiAlpha;

			vecMultiDif.push_back(L"Terrain Tile\\LV_TurtleLake_Tile_Rock01_D_HMJ.tga");
			vecMultiDif.push_back(L"Terrain Tile\\LV_Navarra_Tile_Road_D_AHH.tga");
			vecMultiDif.push_back(L"Terrain Tile\\LV_Elrano_Tile_EGrass_D_JYI.tga");
			vecMultiDif.push_back(L"Terrain Tile\\LV_Hieracon_BrickFloor01_D_HMJ.tga");
			vecMultiDif.push_back(L"Terrain Tile\\LV_NestOfVentus_Lava_Floor01_D_EYE.tga");
			vecMultiDif.push_back(L"Terrain Tile\\3_LV_Elrano_Tile_Cliff01_D_AHH.tga");
			vecMultiDif.push_back(L"Terrain Tile\\LV_Neutral_Tile_A_Stone01_D_KSJ.tga");
			vecMultiDif.push_back(L"Terrain Tile\\LV_WyvernsValley_Tile_Cliff01_D_IJH.tga");

			vecMultiNrm.push_back(L"Terrain Tile\\LV_TurtleLake_Tile_Rock01_N_HMJ.tga");
			vecMultiNrm.push_back(L"Terrain Tile\\LV_Navarra_Tile_Road_N_AHH.tga");
			vecMultiNrm.push_back(L"Terrain Tile\\3_LV_Elrano_Tile_EGrass_N_KSV.tga");
			vecMultiNrm.push_back(L"Terrain Tile\\LV_Hieracon_BrickFloor01_N_HMJ.tga");
			vecMultiNrm.push_back(L"Terrain Tile\\LV_NestOfVentus_Lava_Floor01_N_EYE.tga");
			vecMultiNrm.push_back(L"Terrain Tile\\LV_Elrano_Tile_Cliff01_N_AHH.tga");
			vecMultiNrm.push_back(L"Terrain Tile\\LV_Neutral_Tile_A_Stone01_N_CMJ.tga");
			vecMultiNrm.push_back(L"Terrain Tile\\LV_WyvernsValley_Tile_Cliff01_N_IJH.tga");

			//vecMultiSpc.push_back(L"Terrains\\Terrain_Cliff_15_Large_SPEC.bmp");
			//vecMultiSpc.push_back(L"Terrains\\BD_Terrain_Cliff05_SPEC.bmp");
			//vecMultiSpc.push_back(L"Terrains\\Terrain_Cliff_11_SPEC.bmp");
			//vecMultiSpc.push_back(L"Terrains\\Terrain_Pebbles_01_SPEC.bmp");

			vecMultiAlpha.push_back(L"Terrain Tile\\Default AlphaTexture.bmp");
			vecMultiAlpha.push_back(L"Terrain Tile\\Default AlphaTexture.bmp");
			vecMultiAlpha.push_back(L"Terrain Tile\\Default AlphaTexture.bmp");
			vecMultiAlpha.push_back(L"Terrain Tile\\Default AlphaTexture.bmp");
			vecMultiAlpha.push_back(L"Terrain Tile\\Default AlphaTexture.bmp");
			vecMultiAlpha.push_back(L"Terrain Tile\\Default AlphaTexture.bmp");
			vecMultiAlpha.push_back(L"Terrain Tile\\Default AlphaTexture.bmp");
			vecMultiAlpha.push_back(L"Terrain Tile\\Default AlphaTexture.bmp");

			pTerrainComponent->SetSampler();

			pTerrainComponent->AddSplatDiffuseTexture("Splat", "SplatDif", vecMultiDif);
			pTerrainComponent->AddSplatNormalTexture("Splat", "SplatNrm", vecMultiNrm);
			//pTerrainComponent->AddSplatSpecularTexture("Splat", "SplatSpc", vecMultiSpc);
			pTerrainComponent->AddSplatAlphaTexture("Splat", "SplatAlpha", vecMultiAlpha);


			Vector2 vSize = pTerrainComponent->GetTerrainSize();
			SAFE_RELEASE(pTerrainComponent);

			CBoxCollider* pCollider = m_pSelectObject->AddComponent<CBoxCollider>("BoxCollider");
			pCollider->SetVolume(Vector3(vSize.x, 0.01f, vSize.y));
			pCollider->SetLocalPosition(0.0f, 0.0f, 0.0f);
			SAFE_RELEASE(pCollider);

			//터레인 컴퍼넌트안에서 자동으로 생성되는 메쉬렌더러의 이름을 지어준다 안할 경우 기본 tag인 "SSS CMeshRenderer" 가 된다
			CMeshRenderer* pRenderer = m_pSelectObject->GetComponent<CMeshRenderer>();
			pRenderer->SetTag("Mesh Renderer");
			SAFE_RELEASE(pRenderer);
		}
		break;

		case OCT_TERRAINEDITOR:
		{
			CTerrainEditor* pEditor = m_pSelectObject->AddComponent<CTerrainEditor>("Terrain Editor"); //브러쉬 (현재 클라에 있음)
			SAFE_RELEASE(pEditor);
		}
		break;

		case OCT_BILLBOARD:
		{
			CBillboard* pBillboard = m_pSelectObject->AddComponent<CBillboard>("Billboard");
			SAFE_RELEASE(pBillboard);
		}
		break;

		case OCT_MONSTERAI:
		{
			CMonsterAI* pMonsterAI = m_pSelectObject->AddComponent<CMonsterAI>("MonsterAI");
			SAFE_RELEASE(pMonsterAI);
		}
		break;
		case OCT_NPC_IDLE_AI:
		{
			CNPC_IDLE_AI* pNPCAI = m_pSelectObject->AddComponent<CNPC_IDLE_AI>("NPC_IDLE_AI");
			SAFE_RELEASE(pNPCAI);
		}
		break;
		
		case OCT_GAMESYSTEM:
		{
			CGameSystem* pGameSystem = m_pSelectObject->AddComponent<CGameSystem>("GameSystem");
			SAFE_RELEASE(pGameSystem);
		}
		break;

		case OCT_BONEDRAGONAI:
		{
			CBoneDragonAI* pAI = m_pSelectObject->AddComponent<CBoneDragonAI>("BoneDragonAI");
			SAFE_RELEASE(pAI);
		}
		break;

		case OCT_PORTAL:
		{
			CPortal* pPortal = m_pSelectObject->AddComponent<CPortal>("Portal");
			SAFE_RELEASE(pPortal);
		}
		break;

		}
	}
	DrawInspector();

	UpdateData(FALSE);
}


//	Obj List 목록 더블 클릭
void CInspector::OnLbnDblclkObjectList()
{
	UpdateData(TRUE);


	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//int iObjectIndex = m_ObjectList.GetCurSel();

	if (m_pSelectObject)
	{
		// 기본적으로 들어가있어야하는 위치 조정 기능. 컴포넌트화가 되어 있지 않아 좀 까다로울지도..
		// 트랜스폼은 삭제 불가능!
		// 컴포넌트 중 제일 먼저 표시됨.		

		list<CComponent*>::iterator	iter;
		list<CComponent*>::iterator	iterEnd = m_pSelectObject->GetAllComponents().end();

		// 현재 오브젝트 리스트에서 선택된 인덱스의 오브젝트가 가진 모든 컴포넌트를 확인한다.
		for (iter = m_pSelectObject->GetAllComponents().begin(); iter != iterEnd; ++iter)
		{
			if (!(*iter)->IsDestroy())
			{
				// 컴포넌트 리스트에 추가.
				CString strComponent = CA2CT((*iter)->GetTag().c_str());

				// Mesh Renderer
				if (strComponent == L"Mesh Renderer")
				{
				}


				else if (strComponent == L"Sprite Renderer")
				{
				}


				else if (strComponent == L"CircleCollider2D")
				{
				}

				else if (strComponent == L"RectCollider2D")
				{
				}

				else if (strComponent == L"SphereCollider")
				{
				}

				else if (strComponent == L"UICollider")
				{
				}

				else if (strComponent == L"Animator")
				{
				}

				else if (strComponent == L"Cursor")
				{
				}

				else if (strComponent == L"Camera" || strComponent == L"Main Camera")
				{
				}

				else if (strComponent == L"Text")
				{
				}

				else if (strComponent == L"PostProcessing")
				{
				}
				else if (strComponent == L"Cardinal Spline")
				{
				}
				else if (strComponent == L"Out Line")
				{
				}


				// Client Component

				else if (strComponent == L"Camera Move")
				{
				}

				else if (strComponent == L"Chat")
				{
				}
				else if (strComponent == L"VFX Effect")
				{
				}
				else if (strComponent == L"Player Controller")
				{
				}



			}
		}
		m_sDlgSize.cy = m_rComponentRect.bottom + 50;

		//SetScrollSizes(MM_TEXT, m_sDlgSize);

	}

	// 선택된 오브젝트가 없음. ( 다른 함수들에서 현재 함수를 호출해서 사용 할 경우를 위해
	// 예외 처리 )
	else
	{
	}


	UpdateData(FALSE);
}

// Inspector CWnd Seting
void CInspector::SetWndInspector(CWnd * pWnd)
{
	m_wndInspector = pWnd;
}

string CInspector::GetObjectTag()
{
	CString cstrObjTag;
	m_ObjectTag.GetWindowTextW(cstrObjTag);

	string strObjTag;
	strObjTag = CT2CA(cstrObjTag);

	return strObjTag;
}

void CInspector::SetObjectTag(string strTag)
{
	CString		strData = CA2CT(strTag.c_str());
	m_ObjectTag.SetWindowTextW(strData);
}

void CInspector::DrawInspector()
{
	GET_SINGLE(CEditorArchive)->ClearControls();

	m_sDlgSize.cy = 0;
	SetScrollSizes(MM_TEXT, m_sDlgSize);
	m_wndInspector->RedrawWindow();

	if (m_pSelectObject)
	{
		// 트랜스폼은 삭제 불가능!
		// 컴포넌트 중 제일 먼저 표시됨.		
		TransformLayOut();

		list<CComponent*>::iterator	iter;
		list<CComponent*>::iterator	iterEnd = m_pSelectObject->GetAllComponents().end();

		// 현재 오브젝트 리스트에서 선택된 인덱스의 오브젝트가 가진 모든 컴포넌트를 확인한다.
		for (iter = m_pSelectObject->GetAllComponents().begin(); iter != iterEnd; ++iter)
		{
			if (!(*iter)->IsDestroy())
			{
				if ((*iter)->IsComponent<CMeshRenderer>())
				{
					MeshRendererLayOut(*iter);
				}
				else if ((*iter)->IsComponent<CRigidbody>())
				{
					RigidbodyLayout(*iter);
				}
				else if ((*iter)->IsComponent<CMeshCollider>())
				{
					MeshColliderLayout(*iter);
				}
				else if ((*iter)->IsComponent<CTerrainCollider>())
				{
					TerrainColliderLayout(*iter);
				}
				else if ((*iter)->IsComponent<CSpriteRenderer>())
				{
					SpriteRendererLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CDirectionalLight>())
				{
					DirectionalLightLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CPointLight>())
				{
					PointLightLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CSpotLight>())
				{
					SpotLightLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CCameraMove>())//->GetTag() == "Camera Move")
				{
					CameraMoveLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CRectCollider2D>())
				{
					RectColliderLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CEffect>())
				{
					EffectLayOut(*iter);
				}

				else if ((*iter)->IsComponent<COutLine>())
				{
					OutLineLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CTrail>())
				{
					TrailLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CFire>())
				{
					FireLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CWater>())
				{
					WaterLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CDistortion>())
				{
					DistortionLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CDecal>())
				{
					DecalLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CBillboard>())
				{
					BillboardLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CSphereCollider>())
				{
					SphereColliderLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CBoxCollider>())
				{
					BoxColliderLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CCircleCollider2D>())
				{
					CircleCollider2DLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CAnimator>())
				{
					AnimatorLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CPostProcessing>())
				{
					PostProcessingLayOut(*iter);
				}

				else if ((*iter)->IsComponent<COutLine>())
				{
					OutLineLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CCamera>())
				{
					CameraLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CCardinalSpline>())
				{
					CardinalSplineLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CTerrain>())
				{
					TerrainLayOut(*iter);
				}

				else if ((*iter)->IsComponent<CTerrainEditor>())
				{
					TerrainEditorLayout(*iter);
				}

				else if ((*iter)->GetTag() == "UIPanel")
				{
					m_rComponentRect.top += 30;
					m_rComponentRect.bottom += 50;

					GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"LoadUITexture", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						m_rComponentRect, IDC_LOADTEXTURE_BUTTON);
				}
				else if ((*iter)->GetTag() == "AlphaMask")
				{
					//m_rComponentRect.top += 30;
					//m_rComponentRect.bottom += 50;
					//
					//GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"LoadAlphaMask", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					//	m_rComponentRect, IDC_LOAD_ALPHAMASK_TEXTURE);
				}

				else if ((*iter)->GetTag() == "UIRadioButton")
				{
					m_rComponentRect.top += 30;
					m_rComponentRect.bottom += 50;
					GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"AddRadioButton", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						m_rComponentRect, IDC_ADD_RADIOBUTTON);
					//m_rComponentRect.top += 10;
					//m_rComponentRect.bottom += 50;
					//GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"AddButton", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					//	m_rComponentRect, IDC_ADD_RADIOBUTTON);
				}

				else if ((*iter)->GetTag() == "AlphaMask")
				{
					//m_rComponentRect.bottom += 50;
					//GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"LoadAlphaMask", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					//	m_rComponentRect, IDC_LOAD_ALPHAMASK);
				}
				else if ((*iter)->IsComponent<CSplash>())
				{
					SplashLayOut(*iter);
				}
				else if ((*iter)->IsComponent<CPlayer>())
				{
					PlayerLayout(*iter);
				}
				else if ((*iter)->IsComponent<CPlayerCamera>())
				{
					PlayerCameraLayout(*iter);
				}
				else if ((*iter)->IsComponent<CPlayerController>())
				{
					PlayerControllerLayout(*iter);
				}
				else if ((*iter)->IsComponent<CMonsterAI>())
				{
					MonsterAILayout(*iter);
				}
				else if ((*iter)->IsComponent<CNPC_IDLE_AI>())
				{
					NPCIDLEAILayout(*iter);
				}
				else if ((*iter)->IsComponent<CGameSystem>())
				{
					GameSystemLayOut(*iter);
				}
				else if ((*iter)->IsComponent<CBoneDragonAI>())
				{
					BoneDragonAILayout(*iter);
				}

				else if ((*iter)->IsComponent<CPortal>())
				{
					PortalLayOut(*iter);
				}
			}
		}

		// Add Compnent 
		{
			m_rComponentRect.left = 5;
			m_rComponentRect.top = m_rComponentRect.bottom + 20;

			m_rComponentRect.right = 100;
			m_rComponentRect.bottom = m_rComponentRect.top + 20;

			GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"Component", WS_CHILD | WS_VISIBLE | SS_LEFT,
				m_rComponentRect);


			m_rComponentRect.left = m_rComponentRect.right + 5;
			m_rComponentRect.right = m_rComponentRect.left + 150;

			GET_SINGLE(CEditorArchive)->CreateComboBox(m_pSelectObject, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
				m_rComponentRect, CCT_COMPONENT, IDC_COMBO_COMPONENT);


			m_rComponentRect.left = m_rComponentRect.right + 10;
			m_rComponentRect.top = m_rComponentRect.top - 5;
			m_rComponentRect.right = m_rComponentRect.left + 50;
			m_rComponentRect.bottom = m_rComponentRect.bottom + 5;

			GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"Add", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				m_rComponentRect, IDC_BUTTON_COMPONENT_ADD);

		}
		// 스크롤 사이즈 재정의
		m_sDlgSize.cy = m_rComponentRect.bottom + 50;

		SetScrollSizes(MM_TEXT, m_sDlgSize);
	}


	UpdateData(FALSE);
}

//Transform Getter
void CInspector::CheckTransform()
{
	HWND pCurrentFocusHandle = GetFocus()->GetSafeHwnd();
	if (pCurrentFocusHandle == GetDlgItem(IDC_EDIT_POSITION_X)->GetSafeHwnd() ||
		pCurrentFocusHandle == GetDlgItem(IDC_EDIT_POSITION_Y)->GetSafeHwnd() ||
		pCurrentFocusHandle == GetDlgItem(IDC_EDIT_POSITION_Z)->GetSafeHwnd() ||
		pCurrentFocusHandle == GetDlgItem(IDC_EDIT_SCALE_X)->GetSafeHwnd() ||
		pCurrentFocusHandle == GetDlgItem(IDC_EDIT_SCALE_Y)->GetSafeHwnd() ||
		pCurrentFocusHandle == GetDlgItem(IDC_EDIT_SCALE_Z)->GetSafeHwnd() ||
		pCurrentFocusHandle == GetDlgItem(IDC_EDIT_ROTATION_X)->GetSafeHwnd() ||
		pCurrentFocusHandle == GetDlgItem(IDC_EDIT_ROTATION_Y)->GetSafeHwnd() ||
		pCurrentFocusHandle == GetDlgItem(IDC_EDIT_ROTATION_Z)->GetSafeHwnd())
	{
		GET_SINGLE(CEditorArchive)->SetTransformState(false);
	}

	else if (m_pSelectObject)
	{
		//UpdateData(TRUE);
		CTransform*	pObjectTr = m_pSelectObject->GetTransform();
		Vector3		vPosition;
		Vector3		vScale;
		Vector3		vRotation;
		CString		strData = L"";
		float		fData = 0;

		if (m_bLocalTrans)
		{
			vPosition = pObjectTr->GetLocalPosition();
			vScale = pObjectTr->GetLocalScale();
			vRotation = pObjectTr->GetLocalRotation();
		}
		else
		{
			vPosition = pObjectTr->GetWorldPosition();
			vScale = pObjectTr->GetWorldScale();
			vRotation = pObjectTr->GetWorldRotation();
		}

		vRotation *= R2D;

#pragma region POSITION SETTING
		{
			CWnd* pWnd = GetDlgItem(IDC_EDIT_POSITION_X);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				if (vPosition.x != fData)
				{
					strData.Format(_T("%f"), vPosition.x);
					strData.TrimRight(L"0");
					strData.TrimRight(L".");
					pWnd->SetWindowTextW(strData);
				}
			}

			pWnd = GetDlgItem(IDC_EDIT_POSITION_Y);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				if (vPosition.y != fData)
				{
					strData.Format(_T("%f"), vPosition.y);
					strData.TrimRight(L"0");
					strData.TrimRight(L".");
					pWnd->SetWindowTextW(strData);
				}
			}

			pWnd = GetDlgItem(IDC_EDIT_POSITION_Z);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				if (vPosition.z != fData)
				{
					strData.Format(_T("%f"), vPosition.z);
					strData.TrimRight(L"0");
					strData.TrimRight(L".");
					pWnd->SetWindowTextW(strData);
				}
			}
		}
#pragma endregion

#pragma region SCALE SETTING
		{
			CWnd* pWnd = GetDlgItem(IDC_EDIT_SCALE_X);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				if (vScale.x != fData)
				{
					strData.Format(_T("%f"), vScale.x);
					strData.TrimRight(L"0");
					strData.TrimRight(L".");
					pWnd->SetWindowTextW(strData);
				}
			}

			pWnd = GetDlgItem(IDC_EDIT_SCALE_Y);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				if (vScale.y != fData)
				{
					strData.Format(_T("%f"), vScale.y);
					strData.TrimRight(L"0");
					strData.TrimRight(L".");
					pWnd->SetWindowTextW(strData);
				}
			}

			pWnd = GetDlgItem(IDC_EDIT_SCALE_Z);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				if (vScale.z != fData)
				{
					strData.Format(_T("%f"), vScale.z);
					strData.TrimRight(L"0");
					strData.TrimRight(L".");
					pWnd->SetWindowTextW(strData);
				}
			}
		}
#pragma endregion

#pragma region ROTATION SETTING
		{
			CWnd* pWnd = GetDlgItem(IDC_EDIT_ROTATION_X);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				if (vRotation.x != fData)
				{
					strData.Format(_T("%f"), vRotation.x);
					strData.TrimRight(L"0");
					strData.TrimRight(L".");
					pWnd->SetWindowTextW(strData);
				}
			}

			pWnd = GetDlgItem(IDC_EDIT_ROTATION_Y);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				if (vRotation.y != fData)
				{
					strData.Format(_T("%f"), vRotation.y);
					strData.TrimRight(L"0");
					strData.TrimRight(L".");
					pWnd->SetWindowTextW(strData);
				}
			}

			pWnd = GetDlgItem(IDC_EDIT_ROTATION_Z);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				if (vRotation.z != fData)
				{
					strData.Format(_T("%f"), vRotation.z);
					strData.TrimRight(L"0");
					strData.TrimRight(L".");
					pWnd->SetWindowTextW(strData);
				}
			}
		}
#pragma endregion

#pragma region Gizmo Setting
		{
			CGizmo*	pGizmo = GET_SINGLE(CEditorArchive)->GetGizmo();
			if (pGizmo)
			{
				if (!m_bLocalTrans)
				{
					pObjectTr->SetWorldPosition(vPosition);
					pObjectTr->SetWorldScale(vScale);
					pObjectTr->SetWorldRotation(vRotation * D2R);

					CTransform*	pParentTr = pObjectTr->GetParent();
					if (pParentTr)
					{
						pObjectTr->UpdateChildTransform();
					}
					pGizmo->UpdateGizmoTransform();
				}
				else
				{
					pObjectTr->SetLocalPosition(vPosition);
					pObjectTr->SetLocalScale(vScale);
					pObjectTr->SetLocalRotation(vRotation * D2R);

					CTransform*	pParentTr = pObjectTr->GetParent();
					if (pParentTr)
					{
						pObjectTr->UpdateChildTransform();
					}
					pGizmo->UpdateGizmoLocalTransform();
				}
			}

			SAFE_RELEASE(pObjectTr);
			SAFE_RELEASE(pGizmo);

			UpdateData(FALSE);
		}
#pragma endregion			
	}
}

void CInspector::SetSelctObject(CGameObject * pObj)
{
	m_pSelectObject = pObj;

	if(m_pSelectObject)
	{
		m_LayerCombo.ResetContent();
		//CGameObject*	pSelectObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
		CScene*	pScene = m_pSelectObject->GetScene();
		vector<CLayer*>*	pvecLayer = pScene->GetLayers();

		CString strLayer;

		vector<CLayer*>::iterator	Layeriter;
		vector<CLayer*>::iterator	LayeriterEnd = pvecLayer->end();
		for (Layeriter = pvecLayer->begin(); Layeriter != LayeriterEnd; ++Layeriter)
		{
			strLayer = CA2CT((*Layeriter)->GetTag().c_str());
			m_LayerCombo.AddString(strLayer);
		}
	}
}

void CInspector::SetHierarchy(CHierarchy * pHierarchy)
{
	m_pHierarchy = pHierarchy;
}

void CInspector::SetComponentComboBox(CComboBox * pComboBox)
{
	m_ComponentCombo = pComboBox;
}

void CInspector::RigidbodyLayout(CComponent * pComponent)
{
	CEdit*		pEdit;
	CRigidbody* pCastedComponent = dynamic_cast<CRigidbody*>(pComponent);

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Rigidbody", WS_CHILD | WS_VISIBLE | SS_LEFT, m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);


	m_rComponentRect.top = m_rComponentRect.bottom + 8;
	m_rComponentRect.bottom = m_rComponentRect.top + 30;
	m_rComponentRect.left = 5;
	m_rComponentRect.right = m_rComponentRect.left + 200;
	CButton* pKinematic = (CButton*)GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Is Kinematic", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
		m_rComponentRect, IDC_BUTTON_RIGIDBODY_IS_KINEMATIC);
		
	pKinematic->SetCheck(pCastedComponent->IsKinematic());

	m_rComponentRect.top = m_rComponentRect.bottom + 8;
	m_rComponentRect.bottom = m_rComponentRect.top + 30;
	m_rComponentRect.left = 5;
	m_rComponentRect.right = m_rComponentRect.left + 200;
	CButton* pGravity = (CButton*)GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Use Gravity", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
		m_rComponentRect, IDC_BUTTON_RIGIDBODY_USE_GRAVITY);
	pGravity->SetCheck(pCastedComponent->UseGravity());

	
	//Mass
	m_rComponentRect.top = m_rComponentRect.bottom + 8;
	m_rComponentRect.bottom = m_rComponentRect.top + 30;
	m_rComponentRect.left = 5;
	m_rComponentRect.right = m_rComponentRect.left + 120;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Mass", WS_CHILD | WS_VISIBLE | SS_LEFT, m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 20;
	m_rComponentRect.right = m_rComponentRect.left + 100;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_RIGIDBODY_MASS);
	
	wstring strValue = to_wstring(pCastedComponent->GetMass());
	CString cstrValue = strValue.c_str();
	cstrValue.TrimRight(L'0');
	cstrValue.TrimRight(L'.');
	pEdit->SetWindowTextW(cstrValue);

	//Drag
	m_rComponentRect.top = m_rComponentRect.bottom + 8;
	m_rComponentRect.bottom = m_rComponentRect.top + 30;
	m_rComponentRect.left = 5;
	m_rComponentRect.right = m_rComponentRect.left + 120;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Drag", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 20;
	m_rComponentRect.right = m_rComponentRect.left + 100;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_RIGIDBODY_DRAG);

	strValue = to_wstring(pCastedComponent->GetDrag()); 
	cstrValue = strValue.c_str();
	cstrValue.TrimRight(L'0');
	cstrValue.TrimRight(L'.');
	pEdit->SetWindowTextW(cstrValue);


	//Angular Drag
	m_rComponentRect.top = m_rComponentRect.bottom + 8;
	m_rComponentRect.bottom = m_rComponentRect.top + 30;
	m_rComponentRect.left = 5;
	m_rComponentRect.right = m_rComponentRect.left + 120;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Angular Drag", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 20;
	m_rComponentRect.right = m_rComponentRect.left + 100;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_RIGIDBODY_ANGULAR_DRAG);

	strValue = to_wstring(pCastedComponent->GetAngularDrag());
	cstrValue = strValue.c_str();
	cstrValue.TrimRight(L'0');
	cstrValue.TrimRight(L'.');
	pEdit->SetWindowTextW(cstrValue);
}

void CInspector::MeshColliderLayout(CComponent * pComponent)
{
	CString		strValue;
	CEdit*		pEdit;
	CMeshCollider* pCastedComponent = dynamic_cast<CMeshCollider*>(pComponent);

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Mesh Collider", WS_CHILD | WS_VISIBLE | SS_LEFT, m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

}

void CInspector::TerrainColliderLayout(CComponent * pComponent)
{
	CString		strValue;
	CEdit*		pEdit;
	CTerrainCollider* pCastedComponent = dynamic_cast<CTerrainCollider*>(pComponent);

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Terrain Collider", WS_CHILD | WS_VISIBLE | SS_LEFT, m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);
	 

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 60;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Height Map", WS_CHILD | WS_VISIBLE | SS_LEFT, m_rComponentRect);

	//SSSScene File Directory
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 200;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent,
		WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL | ES_READONLY | SS_LEFT,
		m_rComponentRect, IDC_EDIT_LOAD_TERRAIN_COLLIDER_HEIGHT_TEXTURE);
	CTexture* pTexture = pCastedComponent->GetTexture();
	strValue = ""; //pTexture->GetKey().c_str();
	pEdit->SetWindowTextW(strValue);

	// Material Diffuse Texture Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Load", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_BUTTON_LOAD_TERRAIN_COLLIDER_HEIGHT_TEXTURE);

	//// Material Diffuse Delete Button
	//m_rComponentRect.left = m_rComponentRect.right + 5;
	//m_rComponentRect.right = m_rComponentRect.left + 50;
	//GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Clear", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	//	m_rComponentRect, IDC_BUTTON_CLEAR_TARGET_SCENE);
}

void CInspector::PlayerCameraLayout(CComponent * pComponent)
{
	CPlayerCamera* pCastedComponent = dynamic_cast<CPlayerCamera*>(pComponent);

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Player Camera", WS_CHILD | WS_VISIBLE | SS_LEFT, m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

}

void CInspector::PlayerLayout(CComponent* pComponent)
{
	CString		strValue;
	CEdit*		pEdit;

	CPlayer* pCastedComponent = dynamic_cast<CPlayer*>(pComponent);

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Player", WS_CHILD | WS_VISIBLE | SS_LEFT, m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

}

void CInspector::PlayerControllerLayout(CComponent * pComponent)
{
	CString		strValue;
	CEdit*		pEdit;

	CPlayerController* pCastedComponent = dynamic_cast<CPlayerController*>(pComponent);

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"PlayerContorller", WS_CHILD | WS_VISIBLE | SS_LEFT, m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);
}

void CInspector::MonsterAILayout(CComponent * pComponent)
{
	CString		strValue;
	CEdit*		pEdit;

	CMonsterAI* pCastedComponent = dynamic_cast<CMonsterAI*>(pComponent);

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"MonsterAI", WS_CHILD | WS_VISIBLE | SS_LEFT, m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);
}

void CInspector::NPCIDLEAILayout(CComponent * pComponent)
{
	CString		strValue;
	CEdit*		pEdit;

	CNPC_IDLE_AI* pCastedComponent = dynamic_cast<CNPC_IDLE_AI*>(pComponent);

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"NPC_IDLE_AI", WS_CHILD | WS_VISIBLE | SS_LEFT, m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);
}

void CInspector::BoneDragonAILayout(CComponent * pComponent)
{
	CString		strValue;
	CEdit*		pEdit;

	CBoneDragonAI* pCastedComponent = dynamic_cast<CBoneDragonAI*>(pComponent);

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"BoneDragon AI", WS_CHILD | WS_VISIBLE | SS_LEFT, m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);
}

void CInspector::TransformLayOut()
{
	// Transform LayOut
	{
		CTransform*		pObjTr = m_pSelectObject->GetTransform();

		CEdit*		pEdit;
		CString		strData;
		Vector3		vObjPos;
		Vector3		vObjSca;
		Vector3		vObjRot;

		if (!m_bLocalTrans)
		{
			vObjPos = pObjTr->GetWorldPosition();
			vObjSca = pObjTr->GetWorldScale();
			vObjRot = pObjTr->GetWorldRotation();
		}
		else
		{
			vObjPos = pObjTr->GetLocalPosition();
			vObjSca = pObjTr->GetLocalScale();
			vObjRot = pObjTr->GetLocalRotation();
		}
		vObjRot *= R2D;

		//  Static 생성
		m_rComponentRect.left = 5;
		m_rComponentRect.top = 50;

		m_rComponentRect.right = 150;
		m_rComponentRect.bottom = m_rComponentRect.top + 15;


		// Component Tag
		if (!m_bLocalTrans)
		{
			GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"World Transform", WS_CHILD | WS_VISIBLE | SS_LEFT,
				m_rComponentRect);
		}

		else
		{
			GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"Local Transform", WS_CHILD | WS_VISIBLE | SS_LEFT,
				m_rComponentRect);
		}

		// Local Trans CheckButton
		m_rComponentRect.left = m_rComponentRect.right + 10;
		m_rComponentRect.right = m_rComponentRect.left + 100;

		CButton*	pLocalTransButton = (CButton*)GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"Local", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			m_rComponentRect, IDC_EDIT_TRANSFORM_LOCAL_BUTTON);

		if (m_bLocalTrans)
			pLocalTransButton->SetCheck(1);
		else
			pLocalTransButton->SetCheck(0);



		////////////		 Position			/////////////////
		m_rComponentRect.left = 5;
		m_rComponentRect.top = m_rComponentRect.bottom + 5;

		m_rComponentRect.right = 100;
		m_rComponentRect.bottom = m_rComponentRect.top + 15;

		GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"Position", WS_CHILD | WS_VISIBLE | SS_LEFT,
			m_rComponentRect);


		// X
		m_rComponentRect.left = m_rComponentRect.right + 5;

		m_rComponentRect.right = m_rComponentRect.left + 10;
		m_rComponentRect.bottom = m_rComponentRect.top + 20;

		GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"X", WS_CHILD | WS_VISIBLE | SS_LEFT,
			m_rComponentRect);


		// PositionX Edit Control
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 60;

		pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
			ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSITION_X);

		strData.Format(_T("%f"), vObjPos.x);
		strData.TrimRight(L"0");
		strData.TrimRight(L".");
		pEdit->SetWindowTextW(strData);



		// Y
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 10;

		GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"Y", WS_CHILD | WS_VISIBLE | SS_LEFT,
			m_rComponentRect);


		// Position Edit Control
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 60;

		pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
			ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSITION_Y);

		strData.Format(_T("%f"), vObjPos.y);
		strData.TrimRight(L"0");
		strData.TrimRight(L".");
		pEdit->SetWindowTextW(strData);


		// Z
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 10;

		GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"Z", WS_CHILD | WS_VISIBLE | SS_LEFT,
			m_rComponentRect);

		// Position Edit Control
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 60;

		pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
			ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSITION_Z);

		strData.Format(_T("%f"), vObjPos.z);
		strData.TrimRight(L"0");
		strData.TrimRight(L".");
		pEdit->SetWindowTextW(strData);

		////////////////		Position End		//////////////////



		///////////////			Scale		////////////////////////
		m_rComponentRect.left = 5;
		m_rComponentRect.top = m_rComponentRect.bottom + 5;

		m_rComponentRect.right = 100;
		m_rComponentRect.bottom = m_rComponentRect.top + 20;

		GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"Scale", WS_CHILD | WS_VISIBLE | SS_LEFT,
			m_rComponentRect);

		// X
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 10;

		GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"X", WS_CHILD | WS_VISIBLE | SS_LEFT,
			m_rComponentRect);


		// ScaleX Edit Control
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 60;

		pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
			ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_SCALE_X);

		strData.Format(_T("%f"), vObjSca.x);
		strData.TrimRight(L"0");
		strData.TrimRight(L".");
		pEdit->SetWindowTextW(strData);

		// Y
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 10;

		GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"Y", WS_CHILD | WS_VISIBLE | SS_LEFT,
			m_rComponentRect);


		// ScaleY Edit Control
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 60;

		pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
			ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_SCALE_Y);

		strData.Format(_T("%f"), vObjSca.y);
		strData.TrimRight(L"0");
		strData.TrimRight(L".");
		pEdit->SetWindowTextW(strData);




		// Z
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 10;

		GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"Z", WS_CHILD | WS_VISIBLE | SS_LEFT,
			m_rComponentRect);


		// ScaleZ Edit Control
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 60;

		pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
			ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_SCALE_Z);

		strData.Format(_T("%f"), vObjSca.z);
		strData.TrimRight(L"0");
		strData.TrimRight(L".");
		pEdit->SetWindowTextW(strData);


		/////////////////////////		Scale End		//////////////////////////



		//////////////////		Rotation		////////////////////////
		m_rComponentRect.left = 5;
		m_rComponentRect.top = m_rComponentRect.bottom + 5;

		m_rComponentRect.right = 100;
		m_rComponentRect.bottom = m_rComponentRect.top + 20;

		GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"Rotation", WS_CHILD | WS_VISIBLE | SS_LEFT,
			m_rComponentRect);

		// X
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 10;

		GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"X", WS_CHILD | WS_VISIBLE | SS_LEFT,
			m_rComponentRect);


		// RotationX Edit Control
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 60;

		pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
			ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_ROTATION_X);

		strData.Format(_T("%f"), vObjRot.x);
		strData.TrimRight(L"0");
		strData.TrimRight(L".");
		pEdit->SetWindowTextW(strData);

		// Y
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 10;

		GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"Y", WS_CHILD | WS_VISIBLE | SS_LEFT,
			m_rComponentRect);


		// Rotation Edit Control
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 60;

		pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
			ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_ROTATION_Y);

		strData.Format(_T("%f"), vObjRot.y);
		strData.TrimRight(L"0");
		strData.TrimRight(L".");
		pEdit->SetWindowTextW(strData);


		// Z
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 10;

		GET_SINGLE(CEditorArchive)->CreateStatic(m_pSelectObject, L"Z", WS_CHILD | WS_VISIBLE | SS_LEFT,
			m_rComponentRect);


		// Rotation Edit Control
		m_rComponentRect.left = m_rComponentRect.right + 5;
		m_rComponentRect.right = m_rComponentRect.left + 60;

		pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
			ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_ROTATION_Z);

		strData.Format(_T("%f"), vObjRot.z);
		strData.TrimRight(L"0");
		strData.TrimRight(L".");
		pEdit->SetWindowTextW(strData);

		///////////////////		Rotation End		/////////////////////////

		SAFE_RELEASE(pObjTr);


		// Line
		m_rComponentRect.top = m_rComponentRect.bottom + 5;
		m_rComponentRect.bottom = m_rComponentRect.top + 5;

		m_DC->MoveTo(0, m_rComponentRect.bottom);
		m_DC->LineTo(420, m_rComponentRect.bottom);
	}
}

void CInspector::SplashLayOut(CComponent* pComponent)
{
	CString		strValue;
	CEdit*		pEdit;

	CSplash* pCastedComponent = dynamic_cast<CSplash*>(pComponent);

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Splash", WS_CHILD | WS_VISIBLE | SS_LEFT, m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);
	
	
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 60;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Scene", WS_CHILD | WS_VISIBLE | SS_LEFT, m_rComponentRect);

	//SSSScene File Directory
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 200;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent,
		WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL | ES_READONLY | SS_LEFT,
		m_rComponentRect, IDC_EDIT_SPLASH_SCENE);
	strValue = pCastedComponent->GetPath().c_str();
	pEdit->SetWindowTextW(strValue);

	// Material Diffuse Texture Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Load", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		m_rComponentRect, IDC_BUTTON_LOAD_TARGET_SCENE);

	// Material Diffuse Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Clear", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		m_rComponentRect, IDC_BUTTON_CLEAR_TARGET_SCENE);

}

void CInspector::MeshRendererLayOut(CComponent* pComponent)
{
	CMeshRenderer* pMeshRenderer = dynamic_cast<CMeshRenderer*>(pComponent);

	CMaterial*	pMaterial = pMeshRenderer->GetMaterial();

	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();

	size_t iSize = pComponentControl.size();

	CTexture*	pDiffuseTexture = pMaterial->GetDiffuseTexture();
	string		strDiffuseKey;
	if (pDiffuseTexture)
		strDiffuseKey = pDiffuseTexture->GetKey();
	else
		strDiffuseKey = "NONE";
	SAFE_RELEASE(pDiffuseTexture);

	CTexture*	pNormalTexture = pMaterial->GetNormalTexture();
	string		strNormalKey;
	if (pNormalTexture)
		strNormalKey = pNormalTexture->GetKey();
	else
		strNormalKey = "NONE";
	SAFE_RELEASE(pNormalTexture);

	CTexture*	pSpecularTexture = pMaterial->GetSpecularTexture();
	string		strSpecularKey;
	if (pSpecularTexture)
		strSpecularKey = pSpecularTexture->GetKey();
	else
		strSpecularKey = "NONE";
	SAFE_RELEASE(pSpecularTexture);

	CTexture*	pAlphaTexture = pMaterial->GetAlphaTexture();
	string		strAlphaKey;
	if (pAlphaTexture)
		strAlphaKey = pAlphaTexture->GetKey();
	else
		strAlphaKey = "NONE";
	SAFE_RELEASE(pAlphaTexture);

	float	fNormalScale = CMathf::Clamp(pMaterial->GetNormalScale(), 0.0f, 1.0f);

	Vector4		vDiffuse = pMaterial->GetDiffuseColor();
	Vector4		vAmbient = pMaterial->GetAmbientColor();
	Vector4		vSpecular = pMaterial->GetSpecularColor();
	Vector4		vEmissive = pMaterial->GetEmissiveColor();
	Vector4		vAlpha = pMaterial->GetAlphaColor();

	int	iContainerCount = pMaterial->GetContainerCount();
	int	iSubsetCount = pMaterial->GetSubsetCount(iContainerCount - 1);

	CString		strValue;
	CEdit*		pEdit;

	//  Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Mesh Renderer", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);


	m_rComponentRect.top = m_rComponentRect.bottom + 8;
	m_rComponentRect.bottom = m_rComponentRect.top + 30;
	m_rComponentRect.left = 5;
	m_rComponentRect.right = m_rComponentRect.left + 180;
	CButton* pOverrideMaterial = (CButton*)GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Override Material", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
		m_rComponentRect, IDC_BUTTON_OVERRIDE_MATERIAL);
	pOverrideMaterial->SetCheck(TRUE);

	m_rComponentRect.left = m_rComponentRect.right + 20;
	m_rComponentRect.right = m_rComponentRect.left + 70;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Load", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_MESHLOAD_BUTTON);

	m_rComponentRect.left = m_rComponentRect.right + 10;
	m_rComponentRect.right = m_rComponentRect.left + 60;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Save", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_MESHSAVE_BUTTON);


	// Shader Key
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 10;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Select Shader", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 200;
	GET_SINGLE(CEditorArchive)->CreateComboBox(pComponent, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
		m_rComponentRect, CCT_SHADER, IDC_EDIT_SHADER_COMBOBOX);



	// Material
	m_rComponentRect.left = 5;
	m_rComponentRect.right = 100;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Material", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);
	

	// Container Add Button
	m_rComponentRect.left = m_rComponentRect.right;
	m_rComponentRect.right = m_rComponentRect.left + 100;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Add Container", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_BUTTON_MATERIAL_CONTAINER_ADD);


	// Container
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 70;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Container", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Container Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_CONTAINER_SIZE);

	//strValue = CW2CT(to_wstring(iContainerCount).c_str());
	//pEdit->SetWindowTextW(strValue);

	// Container SpinButton Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	CSpinButtonCtrl*	pSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->CreateSpinButton(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | UDS_AUTOBUDDY | UDS_SETBUDDYINT |
		UDS_ALIGNRIGHT, m_rComponentRect, IDC_SPIN_MATERIAL_CONTAINER_SIZE);
	
	pSpin->SetRange(0, iContainerCount - 1);
	pSpin->SetPos(0);





	// SubSet Static
	m_rComponentRect.left = m_rComponentRect.right + 20;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Subset", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// SubSet Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY |
		ES_AUTOHSCROLL | SS_LEFT , m_rComponentRect, IDC_EDIT_MATERIAL_SUBSET_SIZE);

	//strValue = CW2CT(to_wstring(iSubsetCount).c_str());
	//pEdit->SetWindowTextW(strValue);

	// SubSet SpinButton Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	pSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->CreateSpinButton(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | UDS_AUTOBUDDY | UDS_SETBUDDYINT |
		UDS_ALIGNRIGHT, m_rComponentRect, IDC_SPIN_MATERIAL_SUBSET_SIZE);

	iSubsetCount = pMaterial->GetSubsetCount(iContainerCount - 1);
	pSpin->SetRange(0, iSubsetCount - 1);
	pSpin->SetPos(0);

	// Container Add Button
	//m_rComponentRect.left = m_rComponentRect.right;
	//m_rComponentRect.right = m_rComponentRect.left + 40;
	//GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Add", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	//	m_rComponentRect, IDC_BUTTON_MATERIAL_SUBSET_ADD);


	// Diffuse Color
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Diffuse Color", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);



	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse R Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_DIFFUSE_R);

	strValue = CW2CT(to_wstring(vDiffuse.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);



	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);



	// Diffuse G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_DIFFUSE_G);

	strValue = CW2CT(to_wstring(vDiffuse.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);




	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Diffuse B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_DIFFUSE_B);

	strValue = CW2CT(to_wstring(vDiffuse.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_DIFFUSE_A);

	strValue = CW2CT(to_wstring(vDiffuse.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	////////////////		Diffuse End		//////////////////



	//----------------------------		Ambient		-------------------------------------------

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Ambient", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;

	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Ambient R Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_AMBIENT_R);

	strValue = CW2CT(to_wstring(vAmbient.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Ambient G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_AMBIENT_G);

	strValue = CW2CT(to_wstring(vAmbient.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);



	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Ambient B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_AMBIENT_B);

	strValue = CW2CT(to_wstring(vAmbient.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);



	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Ambient A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_AMBIENT_A);

	strValue = CW2CT(to_wstring(vAmbient.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);

	////////////////		Ambient End		//////////////////



	//----------------------------		Specular		-------------------------------------------

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Specular", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Specular R Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_SPECULAR_R);

	strValue = CW2CT(to_wstring(vSpecular.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// G

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Specular G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_SPECULAR_G);

	strValue = CW2CT(to_wstring(vSpecular.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);



	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Specular B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_SPECULAR_B);

	strValue = CW2CT(to_wstring(vSpecular.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Specular A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_SPECULAR_A);

	strValue = CW2CT(to_wstring(vSpecular.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);

	////////////////		Specular End		//////////////////


	//----------------------------		Emissive		-------------------------------------------

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Emissive", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Emissive R Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_EMISSIVE_R);

	strValue = CW2CT(to_wstring(vEmissive.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Emissive G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_EMISSIVE_G);

	strValue = CW2CT(to_wstring(vEmissive.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Emissive B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_EMISSIVE_B);

	strValue = CW2CT(to_wstring(vEmissive.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Emissive A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_EMISSIVE_A);

	strValue = CW2CT(to_wstring(vEmissive.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	////////////////		Emissive End		//////////////////

	///////////////			Alpha			///////////////////////
	// Alpha Color
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Alpha Color", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);



	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Alpha R Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_ALPHA_R);

	strValue = CW2CT(to_wstring(vAlpha.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);



	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);



	// Alpha G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_ALPHA_G);

	strValue = CW2CT(to_wstring(vAlpha.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);




	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Alpha B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_ALPHA_B);

	strValue = CW2CT(to_wstring(vAlpha.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_ALPHA_A);

	strValue = CW2CT(to_wstring(vAlpha.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	////////////////		Alpha End		//////////////////





	////////////////		Texture		//////////////////

	// Texture Static
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Texture", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Diffuse Static
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 60;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Diffuse", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Diffuse Texture Key Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 200;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | ES_READONLY | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_DIFFUSE_KEY);

	strValue = CA2CT(strDiffuseKey.c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);

	// Material Diffuse Texture Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Load", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_MATERIAL_DIFFUSE_TEXTURE_BUTTON);

	// Material Diffuse Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Clear", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_MATERIAL_DIFFUSE_CLEAR_BUTTON);


	// Normal Static
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 60;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Normal", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Normal Texture Key Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 200;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | ES_READONLY | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_NORMAL_KEY);

	strValue = CA2CT(strNormalKey.c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);

	// Material Normal Texture Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Load", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_MATERIAL_NORMAL_TEXTURE_BUTTON);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Clear", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_MATERIAL_NORMAL_CLEAR_BUTTON);


	// Specular Static
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 60;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Specular", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Specular Texture Key Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 200;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | ES_READONLY | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_SPECULAR_KEY);

	strValue = CA2CT(strSpecularKey.c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);

	// Material Specular Texture Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Load", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_MATERIAL_SPECULAR_TEXTURE_BUTTON);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Clear", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_MATERIAL_SPECULAR_CLEAR_BUTTON);


	// Alpha Static
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 60;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Alpha", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Alpha Texture Key Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 200;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | ES_READONLY | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_ALPHA_KEY);

	strValue = CA2CT(strAlphaKey.c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);

	// Material Alpha Texture Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Load", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_MATERIAL_ALPHA_TEXTURE_BUTTON);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Clear", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_MATERIAL_ALPHA_CLEAR_BUTTON);

	////////////////		Texture	End		//////////////////


	// Normal Scaler Static
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Normal Scaler", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 200;
	CSliderCtrl*	pSlider = (CSliderCtrl*)GET_SINGLE(CEditorArchive)->CreateSlider(pComponent, WS_CHILD | WS_VISIBLE | TBS_NOTIFYBEFOREMOVE, m_rComponentRect,
		IDC_SLIDER_MATERIAL_NORMAL_SCALE);

	pSlider->SetRange(0, 100);
	pSlider->SetLineSize(1);
	pSlider->SetPos(fNormalScale * 100.0f);


	// Slider Pos Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 30;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | ES_READONLY | SS_LEFT, m_rComponentRect, IDC_EDIT_MATERIAL_NORMAL_SCALE);

	//strValue = CW2CT(to_wstring(fNormalScale * 100).c_str());
	//strValue.TrimRight(L"0");
	//strValue.TrimRight(L".");
	
	strValue = CW2CT(to_wstring(pSlider->GetPos()).c_str());
	pEdit->SetWindowTextW(strValue);

	//Set Local Transform from Mesh
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 300;
	m_rComponentRect.bottom = m_rComponentRect.top + 25;

	GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"Set Local Transform from Mesh", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_BUTTON_LOCAL_TRANSFORM);


	SAFE_RELEASE(pMaterial);


	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::ContainerUpdate(uint32_t iContainer, uint32_t iSubset)
{
	// 현재 컨테이너, 서브셋에 들어있는 값으로 인스펙터에 표시된 메테리얼의 값들을 갱신.

	CMeshRenderer* pMeshRenderer = (CMeshRenderer*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();
	CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
	
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();

	size_t iSize = pComponentControl.size();

	CSpinButtonCtrl*	pContainerSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SPIN_MATERIAL_CONTAINER_SIZE);
	CSpinButtonCtrl*	pSubsetSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SPIN_MATERIAL_SUBSET_SIZE);
	
	float		fNormalScale;

	CSliderCtrl*	pSlider = (CSliderCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SLIDER_MATERIAL_NORMAL_SCALE);
	fNormalScale = pMaterial->GetNormalScale(iContainer, iSubset);
	pSlider->SetPos(fNormalScale * 100);
	CEdit*	pNormalEdit = (CEdit*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_EDIT_MATERIAL_NORMAL_SCALE);
	CString strNormal = CW2CT(to_wstring(pSlider->GetPos()).c_str());
	pNormalEdit->SetWindowTextW(strNormal);

	CTexture*	pDiffuseTexture = pMaterial->GetDiffuseTexture(iContainer, iSubset);
	string		strDiffuseKey;
	if (pDiffuseTexture)
		strDiffuseKey = pDiffuseTexture->GetKey();
	else
		strDiffuseKey = "NONE";
	SAFE_RELEASE(pDiffuseTexture);

	CTexture*	pNormalTexture = pMaterial->GetNormalTexture(iContainer, iSubset);
	string		strNormalKey;
	if (pNormalTexture)
		strNormalKey = pNormalTexture->GetKey();
	else
		strNormalKey = "NONE";
	SAFE_RELEASE(pNormalTexture);

	CTexture*	pSpecularTexture = pMaterial->GetSpecularTexture(iContainer, iSubset);
	string		strSpecularKey;
	if (pSpecularTexture)
		strSpecularKey = pSpecularTexture->GetKey();
	else
		strSpecularKey = "NONE";
	SAFE_RELEASE(pSpecularTexture);

	CTexture*	pAlphaTexture = pMaterial->GetAlphaTexture(iContainer, iSubset);
	string		strAlphaKey;
	if (pAlphaTexture)
		strAlphaKey = pAlphaTexture->GetKey();
	else
		strAlphaKey = "NONE";
	SAFE_RELEASE(pAlphaTexture);

	Vector4		vDiffuse = pMaterial->GetDiffuseColor(iContainer, iSubset);
	Vector4		vAmbient = pMaterial->GetAmbientColor(iContainer, iSubset);
	Vector4		vSpecular = pMaterial->GetSpecularColor(iContainer, iSubset);
	Vector4		vEmissive = pMaterial->GetEmissiveColor(iContainer, iSubset);
	Vector4		vAlpha = pMaterial->GetAlphaColor(iContainer, iSubset);

	CString		strValue;
	CEdit*		pEdit;

	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pMeshRenderer == pComponentControl[i]->pComponent)
		{
			// ID값으로 해당 핸들 값을 가져오면 된다.
			// Diffuse
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_DIFFUSE_R)
			{
				strValue = CW2CT(to_wstring(vDiffuse.x).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_DIFFUSE_G)
			{
				strValue = CW2CT(to_wstring(vDiffuse.y).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_DIFFUSE_B)
			{
				strValue = CW2CT(to_wstring(vDiffuse.z).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_DIFFUSE_A)
			{
				strValue = CW2CT(to_wstring(vDiffuse.w).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_AMBIENT_R)
			{
				strValue = CW2CT(to_wstring(vAmbient.x).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_AMBIENT_G)
			{
				strValue = CW2CT(to_wstring(vAmbient.y).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_AMBIENT_B)
			{
				strValue = CW2CT(to_wstring(vAmbient.z).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_AMBIENT_A)
			{
				strValue = CW2CT(to_wstring(vAmbient.w).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_SPECULAR_R)
			{
				strValue = CW2CT(to_wstring(vSpecular.x).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_SPECULAR_G)
			{
				strValue = CW2CT(to_wstring(vSpecular.y).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_SPECULAR_B)
			{
				strValue = CW2CT(to_wstring(vSpecular.z).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_SPECULAR_A)
			{
				strValue = CW2CT(to_wstring(vSpecular.w).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_EMISSIVE_R)
			{
				strValue = CW2CT(to_wstring(vEmissive.x).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_EMISSIVE_G)
			{
				strValue = CW2CT(to_wstring(vEmissive.y).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_EMISSIVE_B)
			{
				strValue = CW2CT(to_wstring(vEmissive.z).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_EMISSIVE_A)
			{
				strValue = CW2CT(to_wstring(vEmissive.w).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_ALPHA_R)
			{
				strValue = CW2CT(to_wstring(vAlpha.x).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_ALPHA_G)
			{
				strValue = CW2CT(to_wstring(vAlpha.y).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_ALPHA_B)
			{
				strValue = CW2CT(to_wstring(vAlpha.z).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_ALPHA_A)
			{
				strValue = CW2CT(to_wstring(vAlpha.w).c_str());
				strValue.TrimRight(L"0");
				strValue.TrimRight(L".");
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(strValue);
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_DIFFUSE_KEY)
			{
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(CA2CT(strDiffuseKey.c_str()));
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_NORMAL_KEY)
			{
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(CA2CT(strNormalKey.c_str()));
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_SPECULAR_KEY)
			{
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(CA2CT(strSpecularKey.c_str()));
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_ALPHA_KEY)
			{
				((CEdit*)pComponentControl[i]->hWnd)->SetWindowTextW(CA2CT(strAlphaKey.c_str()));
			}
		}
	}


	SAFE_RELEASE(pMaterial);
}

void CInspector::DirectionalLightLayOut(CComponent * pComponent)
{
	CDirectionalLight* pDirLight = dynamic_cast<CDirectionalLight*>(pComponent);

	Vector4		vDiffuse = pDirLight->GetDiffuseColor();
	Vector4		vAmbient = pDirLight->GetAmbientColor();
	Vector4		vSpecular = pDirLight->GetSpecularColor();
	CString		strValue;
	CEdit*		pEdit;

	//  Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Directional Light", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);



	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);


	////////////		 Diffuse			/////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Diffuse", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse R EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_DIRECTIONAL_DIFFUSE_R);

	strValue = CW2CT(to_wstring(vDiffuse.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_DIRECTIONAL_DIFFUSE_G);

	strValue = CW2CT(to_wstring(vDiffuse.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_DIRECTIONAL_DIFFUSE_B);

	strValue = CW2CT(to_wstring(vDiffuse.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_DIRECTIONAL_DIFFUSE_A);

	strValue = CW2CT(to_wstring(vDiffuse.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);



	////////////////		Diffuse End		//////////////////



	//----------------------------		Ambient		-------------------------------------------

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Ambient", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Ambient R Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_DIRECTIONAL_AMBIENT_R);

	strValue = CW2CT(to_wstring(vAmbient.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_DIRECTIONAL_AMBIENT_G);

	strValue = CW2CT(to_wstring(vAmbient.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Diffuse B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_DIRECTIONAL_AMBIENT_B);

	strValue = CW2CT(to_wstring(vAmbient.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_DIRECTIONAL_AMBIENT_A);

	strValue = CW2CT(to_wstring(vAmbient.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	////////////////		Ambient End		//////////////////



	//----------------------------		Specular		-------------------------------------------

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Specular", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Ambient R Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_DIRECTIONAL_SPECULAR_R);

	strValue = CW2CT(to_wstring(vSpecular.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_DIRECTIONAL_SPECULAR_G);

	strValue = CW2CT(to_wstring(vSpecular.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_DIRECTIONAL_SPECULAR_B);

	strValue = CW2CT(to_wstring(vSpecular.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_DIRECTIONAL_SPECULAR_A);

	strValue = CW2CT(to_wstring(vSpecular.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);

	////////////////		Ambient End		//////////////////


	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::PointLightLayOut(CComponent * pComponent)
{
	CPointLight* pPointLight = dynamic_cast<CPointLight*>(pComponent);

	Vector4		vDiffuse = pPointLight->GetDiffuseColor();
	Vector4		vAmbient = pPointLight->GetAmbientColor();
	Vector4		vSpecular = pPointLight->GetSpecularColor();
	CString		strValue;
	CEdit*		pEdit;

	//  Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Point Light", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);


	////////////		 Diffuse			/////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Diffuse", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse R EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_POINT_DIFFUSE_R);

	strValue = CW2CT(to_wstring(vDiffuse.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_POINT_DIFFUSE_G);

	strValue = CW2CT(to_wstring(vDiffuse.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_POINT_DIFFUSE_B);

	strValue = CW2CT(to_wstring(vDiffuse.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_POINT_DIFFUSE_A);

	strValue = CW2CT(to_wstring(vDiffuse.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);



	////////////////		Diffuse End		//////////////////



	//----------------------------		Ambient		-------------------------------------------

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Ambient", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Ambient R Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_POINT_AMBIENT_R);

	strValue = CW2CT(to_wstring(vAmbient.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_POINT_AMBIENT_G);

	strValue = CW2CT(to_wstring(vAmbient.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Diffuse B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_POINT_AMBIENT_B);

	strValue = CW2CT(to_wstring(vAmbient.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_POINT_AMBIENT_A);

	strValue = CW2CT(to_wstring(vAmbient.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	////////////////		Ambient End		//////////////////



	//----------------------------		Specular		-------------------------------------------

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Specular", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Ambient R Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_POINT_SPECULAR_R);

	strValue = CW2CT(to_wstring(vSpecular.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_POINT_SPECULAR_G);

	strValue = CW2CT(to_wstring(vSpecular.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_POINT_SPECULAR_B);

	strValue = CW2CT(to_wstring(vSpecular.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_POINT_SPECULAR_A);

	strValue = CW2CT(to_wstring(vSpecular.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);

	////////////////		Ambient End		//////////////////


	// Range
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Range", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);





	// Specular Range Edit Control
	float fPointLightRange = pPointLight->GetRange();
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_POINT_RANGE);

	strValue = CW2CT(to_wstring(fPointLightRange).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::SpotLightLayOut(CComponent * pComponent)
{
	CSpotLight* pSpotLight = dynamic_cast<CSpotLight*>(pComponent);

	Vector4		vDiffuse = pSpotLight->GetDiffuseColor();
	Vector4		vAmbient = pSpotLight->GetAmbientColor();
	Vector4		vSpecular = pSpotLight->GetSpecularColor();
	CString		strValue;
	CEdit*		pEdit;

	//  Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;
	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Spot Light", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);


	////////////		 Diffuse			/////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Diffuse", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse R EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_SPOT_DIFFUSE_R);

	strValue = CW2CT(to_wstring(vDiffuse.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_SPOT_DIFFUSE_G);

	strValue = CW2CT(to_wstring(vDiffuse.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_SPOT_DIFFUSE_B);

	strValue = CW2CT(to_wstring(vDiffuse.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_SPOT_DIFFUSE_A);

	strValue = CW2CT(to_wstring(vDiffuse.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);



	////////////////		Diffuse End		//////////////////



	//----------------------------		Ambient		-------------------------------------------

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Ambient", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Ambient R Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_SPOT_AMBIENT_R);

	strValue = CW2CT(to_wstring(vAmbient.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_SPOT_AMBIENT_G);

	strValue = CW2CT(to_wstring(vAmbient.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Diffuse B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_SPOT_AMBIENT_B);

	strValue = CW2CT(to_wstring(vAmbient.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_SPOT_AMBIENT_A);

	strValue = CW2CT(to_wstring(vAmbient.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	////////////////		Ambient End		//////////////////



	//----------------------------		Specular		-------------------------------------------

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Specular", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Ambient R Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_SPOT_SPECULAR_R);

	strValue = CW2CT(to_wstring(vSpecular.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_SPOT_SPECULAR_G);

	strValue = CW2CT(to_wstring(vSpecular.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_SPOT_SPECULAR_B);

	strValue = CW2CT(to_wstring(vSpecular.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_LIGHT_SPOT_SPECULAR_A);

	strValue = CW2CT(to_wstring(vSpecular.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);

	////////////////		Ambient End		//////////////////


	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::CameraMoveLayOut(CComponent * pComponent)
{
	CCameraMove*	pCameraMove = dynamic_cast<CCameraMove*>(pComponent);
	CGameObject*	pTargetObj = pCameraMove->GetPlayer();
	CString		strValue;
	CEdit*		pEdit;
	float		fSpeed = pCameraMove->GetSpeed();

	// Camera Move Static
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Camera Move", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);



	// Target Obj Tag Static
	//m_rComponentRect.left = 5;
	//m_rComponentRect.top = m_rComponentRect.bottom + 5;
	//
	//m_rComponentRect.right = 100;
	//m_rComponentRect.bottom = m_rComponentRect.top + 20;
	//GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"TargetObj Tag", WS_CHILD | WS_VISIBLE | SS_LEFT,
	//	m_rComponentRect);
	//
	//
	//// TargetTag Edit Control
	//m_rComponentRect.left = m_rComponentRect.right + 5;
	//m_rComponentRect.right = m_rComponentRect.left + 100;
	//pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
	//	ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_CAMERA_TARGET);
	//
	//
	//vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	//size_t iSize = pComponentControl.size();
	//
	//for (size_t i = 0; i < iSize; ++i)
	//{
	//	if (pCameraMove == pComponentControl[i]->pComponent)
	//	{
	//		if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_CAMERA_TARGET)
	//		{
	//
	//			if (pTargetObj == NULL)
	//			{
	//				pComponentControl[i]->hWnd->SetWindowTextW(L"None Target");
	//			}
	//			else
	//			{
	//				pComponentControl[i]->hWnd->SetWindowTextW(CA2CT(pTargetObj->GetTag().c_str()));
	//			}
	//		}
	//	}
	//}
	//
	//SAFE_RELEASE(pTargetObj);


	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Move Speed", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Move Speed Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 100;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_CAMERA_MOVESPEED);

	strValue = CW2CT(to_wstring(fSpeed).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::RectColliderLayOut(CComponent * pComponent)
{
	CRectCollider2D*	pRectCollider2D = dynamic_cast<CRectCollider2D*>(pComponent);

	CEdit*		pEdit;
	CString		strData;
	Vector3		vObjPivot = pRectCollider2D->GetPivot();
	Vector3		vObjScale = pRectCollider2D->GetScale();

	//  Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"RectCollider2D", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);


	////////////		 Pivot			/////////////////
	// Pivot Static
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Pivot", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// X
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"X", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Pivot X EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_RECTCOLLIDE2D_PIVOT_X);

	strData = CW2CT(to_wstring(vObjPivot.x).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	// Y
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Y", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Pivot Y Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_RECTCOLLIDE2D_PIVOT_Y);

	strData = CW2CT(to_wstring(vObjPivot.y).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	// Z
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Z", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Pivot Z Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_RECTCOLLIDE2D_PIVOT_Z);

	strData = CW2CT(to_wstring(vObjPivot.z).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);

	////////////////		Pivot End		//////////////////


	////////////		 Scale			/////////////////
	// Scale Static
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Scale", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// X
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"X", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Scale X EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_RECTCOLLIDE2D_SCALE_X);

	strData = CW2CT(to_wstring(vObjScale.x).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	// Y
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Y", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Scale Y Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_RECTCOLLIDE2D_SCALE_Y);

	strData = CW2CT(to_wstring(vObjScale.y).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	// Z
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Z", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Scale Z Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_RECTCOLLIDE2D_SCALE_Z);

	strData = CW2CT(to_wstring(vObjScale.z).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);

	////////////////		Diffuse End		//////////////////

	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::OutLineLayOut(CComponent * pComponent)
{
	COutLine*	pOutLine = dynamic_cast<COutLine*>(pComponent);

	CEdit*		pEdit;
	CString		strData;
	Vector3		vOffset = pOutLine->GetOutlineOffset();
	Vector4		vColor = pOutLine->GetOutlineColor();
	float		fSize = pOutLine->GetOutlineSize();

	//  OutLine Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Out Line", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

	////////////		 Offset			/////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Offset", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// X
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"X", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Offset X EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_OUTLINE_OFFSET_X);

	strData = CW2CT(to_wstring(vOffset.x).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	// Y
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Y", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Offset Y Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_OUTLINE_OFFSET_Y);

	strData = CW2CT(to_wstring(vOffset.y).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	// Z
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Z", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Offset Z Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_OUTLINE_OFFSET_Z);

	strData = CW2CT(to_wstring(vOffset.z).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	////////////////		Offset End		//////////////////


	////////////		 Color			/////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Color", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Color R EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_OUTLINE_COLOR_R);

	strData = CW2CT(to_wstring(vColor.x).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Color G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_OUTLINE_COLOR_G);

	strData = CW2CT(to_wstring(vColor.y).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Color B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_OUTLINE_COLOR_B);

	strData = CW2CT(to_wstring(vColor.z).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Color A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_OUTLINE_COLOR_A);

	strData = CW2CT(to_wstring(vColor.w).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);



	////////////////		Color End		//////////////////

	// Size
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Size", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Size Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_OUTLINE_SIZE);

	strData = CW2CT(to_wstring(fSize).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);



	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);

}

void CInspector::TrailLayOut(CComponent * pComponent)
{
	CTrail*	pTrail = dynamic_cast<CTrail*>(pComponent);

	CEdit*		pEdit;
	CString		strData;
	float		fSize = pTrail->GetSize();


	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Trail", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);


	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Size", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_TRAIL_SIZE);


	strData = CW2CT(to_wstring(fSize).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);



	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);

}

void CInspector::DistortionLayOut(CComponent * pComponent)
{
	CDistortion*	pDistortion = dynamic_cast<CDistortion*>(pComponent);

	CEdit*		pEdit;
	CString		strData;

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Distortion", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::FireLayOut(CComponent * pComponent)
{
	CFire*	pDistortion = dynamic_cast<CFire*>(pComponent);

	CEdit*		pEdit;
	CString		strData;

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Fire", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::WaterLayOut(CComponent * pComponent)
{
	CWater*	pDistortion = dynamic_cast<CWater*>(pComponent);

	CEdit*		pEdit;
	CString		strData;

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Water", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::DecalLayOut(CComponent * pComponent)
{
	CDecal*	pDecal = dynamic_cast<CDecal*>(pComponent);

	CEdit*		pEdit;
	CString		strData;

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Decal", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::BillboardLayOut(CComponent * pComponent)
{
	CBillboard*	pBillboard = dynamic_cast<CBillboard*>(pComponent);

	CEdit*		pEdit;
	CString		strData;

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Billboard", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::SphereColliderLayOut(CComponent * pComponent)
{
	CSphereCollider*	pSphereCollider = dynamic_cast<CSphereCollider*>(pComponent);

	CEdit*		pEdit;
	CString		strData;
	Vector3		vLocalPos = pSphereCollider->GetLocalPosition();
	float		fRadius = pSphereCollider->GetRadius();

	// SphereCollider Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"SphereCollider", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);



	////////////////////		 Local Transform			////////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Local", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// X
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"X", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Local X EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_SPHERECOLLIDER_LOCAL_X);

	strData = CW2CT(to_wstring(vLocalPos.x).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);

	// Y
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Y", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Local Y EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_SPHERECOLLIDER_LOCAL_Y);

	strData = CW2CT(to_wstring(vLocalPos.y).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);

	// Z
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Z", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Local Z EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_SPHERECOLLIDER_LOCAL_Z);

	strData = CW2CT(to_wstring(vLocalPos.z).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	////////////////////	Local Transform End		////////////////////////////



	////////////		 Radius			/////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Radius", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Radius EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_SPHERECOLLIDER_RADIUS);

	strData = CW2CT(to_wstring(fRadius).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	////////////////////	Radius End		////////////////////////////

	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::BoxColliderLayOut(CComponent * pComponent)
{
	CBoxCollider*	pBoxCollider = dynamic_cast<CBoxCollider*>(pComponent);

	CEdit*		pEdit;
	CString		strData;
	Vector3		vLocalPos = pBoxCollider->GetLocalPosition();
	Vector3		vVolume = pBoxCollider->GetVolume();

	// BoxCollider Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"BoxCollider", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);


	////////////////////		 Local			////////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Local", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// X
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"X", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Local X EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_BOXCOLLIDER_LOCAL_X);

	strData = CW2CT(to_wstring(vLocalPos.x).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);

	// Y
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Y", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Local Y EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_BOXCOLLIDER_LOCAL_Y);

	strData = CW2CT(to_wstring(vLocalPos.y).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);

	// Z
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Z", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Local Z EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_BOXCOLLIDER_LOCAL_Z);

	strData = CW2CT(to_wstring(vLocalPos.z).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	////////////////////	Center Transform End		////////////////////////////


	////////////////////		 Volume			////////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Volume", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// X
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"X", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Local X EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_BOXCOLLIDER_VOLUME_X);

	strData = CW2CT(to_wstring(vVolume.x).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);

	// Y
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Y", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Local Y EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_BOXCOLLIDER_VOLUME_Y);

	strData = CW2CT(to_wstring(vVolume.y).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);

	// Z
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Z", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Local Z EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_BOXCOLLIDER_VOLUME_Z);

	strData = CW2CT(to_wstring(vVolume.z).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	////////////////////	Volume Transform End		////////////////////////////


	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::CircleCollider2DLayOut(CComponent * pComponent)
{
	CCircleCollider2D* pCircleCollider2D = dynamic_cast<CCircleCollider2D*>(pComponent);

	CEdit*		pEdit;
	CString		strData;
	float		fRadius = pCircleCollider2D->GetRadius();

	// CircleCollider2D Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"CircleCollider2D", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);


	////////////////////		 Radius			////////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Radius", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Radius EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_CIRCLECOLLIDER2D_RADIUS);

	strData = CW2CT(to_wstring(fRadius).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::AnimatorLayOut(CComponent * pComponent)
{
	CAnimator* pAnimator = dynamic_cast<CAnimator*>(pComponent);

	// Animator Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Animator", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::SpriteRendererLayOut(CComponent * pComponent)
{
	CSpriteRenderer* pSpriteRenderer = dynamic_cast<CSpriteRenderer*>(pComponent);

	Vector4		vDiffuse = pSpriteRenderer->GetDiffuse();
	int			iZOrder = pSpriteRenderer->GetZOrder();
	CString		strValue;
	CEdit*		pEdit;

	// Sprite Renderer Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Sprite Renderer", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);


	m_rComponentRect.left = m_rComponentRect.right + 20;
	m_rComponentRect.right = m_rComponentRect.left + 100;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Load Mesh", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_MESHLOAD_BUTTON);


	// ZOrder Static
	m_rComponentRect.left = 5;
	m_rComponentRect.right = 100;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Z Order", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// ZOrder Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_SPRITERENDERER_ZORDER);

	strValue = CW2CT(to_wstring(iZOrder).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// Shader Key
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Select Shader", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 200;
	GET_SINGLE(CEditorArchive)->CreateComboBox(pComponent, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
		m_rComponentRect, CCT_SHADER, IDC_EDIT_SHADER_COMBOBOX);



	// Material
	m_rComponentRect.left = 5;
	m_rComponentRect.right = 100;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Material", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Material Diffuse Texture Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 165;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Load Diffuse Texture", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_MATERIAL_DIFFUSE_TEXTURE_BUTTON);



	// Diffuse Color
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Diffuse Color", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);



	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse R Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_SPRITERENDERER_DIFFUSE_R);

	strValue = CW2CT(to_wstring(vDiffuse.x).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);



	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);



	// Diffuse G Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_SPRITERENDERER_DIFFUSE_G);

	strValue = CW2CT(to_wstring(vDiffuse.y).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);




	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Diffuse B Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_SPRITERENDERER_DIFFUSE_B);

	strValue = CW2CT(to_wstring(vDiffuse.z).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	// Diffuse A Edit Control
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_SPRITERENDERER_DIFFUSE_A);

	strValue = CW2CT(to_wstring(vDiffuse.w).c_str());
	strValue.TrimRight(L"0");
	strValue.TrimRight(L".");
	pEdit->SetWindowTextW(strValue);


	//////////////////		Diffuse End		//////////////////


	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::PostProcessingLayOut(CComponent * pComponent)
{
	CPostProcessing* pPostProcessing = dynamic_cast<CPostProcessing*>(pComponent);

	CEdit*		pEdit;
	CButton*	pButton;
	CString		strData;
	Vector4		vFogColor = pPostProcessing->GetFogColor();
	float		fFogMinDistance = pPostProcessing->GetFogMinDistance();
	float		fFogMaxDistance = pPostProcessing->GetFogMaxDistance();
	uint32_t	iPostProcessFlags = pPostProcessing->GetPostProcessingFlags();
	uint32_t	iBlurSampleCount = pPostProcessing->GetBlurSampleCount();
	float		fDepthOfFieldMinRange = pPostProcessing->GetDepthOfFieldMinRange();
	float		fDepthOfFieldMaxRange = pPostProcessing->GetDepthOfFieldMaxRange();
	float		fDepthOfFieldMaxSigma = pPostProcessing->GetDepthOfFieldMaxSigma();
	float		fHDRExposure = pPostProcessing->GetHDRExposure();
	float		fHDRWhiteThreshold = pPostProcessing->GetHDRWhiteThreshold();

	// PostProcessing Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"PostProcessing", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

	////////////////////		 Postprocessing	Flags		////////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Flag", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);



	// Anitaliasing CheckButton
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 140;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;

	pButton = (CButton*)GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Anitaliasing", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		m_rComponentRect, IDC_BUTTON_POSTPROCESSING_ANTIALIASING);

	pButton->SetCheck((bool)(iPostProcessFlags & PT_ANTIALIASING));


	// Eye Adaption CheckButton
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 135;

	pButton = (CButton*)GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Eye Adaption", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		m_rComponentRect, IDC_BUTTON_POSTPROCESSING_EYE_ADAPTION);

	pButton->SetCheck((bool)(iPostProcessFlags & PT_EYE_ADAPTION));


	// Bloom CheckButton
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 135;

	pButton = (CButton*)GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Bloom", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		m_rComponentRect, IDC_BUTTON_POSTPROCESSING_BLOOM);

	pButton->SetCheck((bool)(iPostProcessFlags & PT_BLOOM));

	// Fog CheckButton
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.right = 140;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;

	pButton = (CButton*)GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Fog", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		m_rComponentRect, IDC_BUTTON_POSTPROCESSING_FOG);

	pButton->SetCheck((bool)(iPostProcessFlags & PT_FOG));


	// HDR CheckButton
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 135;

	pButton = (CButton*)GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"HDR", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		m_rComponentRect, IDC_BUTTON_POSTPROCESSING_HDR);

	pButton->SetCheck((bool)(iPostProcessFlags & PT_HDR));


	// Motion Blur CheckButton
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 135;

	pButton = (CButton*)GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Motion Blur", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		m_rComponentRect, IDC_BUTTON_POSTPROCESSING_MOTION_BLUR);

	pButton->SetCheck((bool)(iPostProcessFlags & PT_MOTION_BLUR));


	// Ambient Occlution CheckButton
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.right = 140;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;

	pButton = (CButton*)GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Ambient Occlution", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		m_rComponentRect, IDC_BUTTON_POSTPROCESSING_AMBIENT_OCCLUSION);

	pButton->SetCheck((bool)(iPostProcessFlags & PT_AMBIENT_OCCLUSION));



	// Depth of Field CheckButton
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 135;

	pButton = (CButton*)GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Depth of Field", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		m_rComponentRect, IDC_BUTTON_POSTPROCESSING_DEPTH_OF_FIELD);

	pButton->SetCheck((bool)(iPostProcessFlags & PT_DEPTH_OF_FIELD));


	////////////////////		 Fog Distance			////////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Fog", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Fog Distance", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Min
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 30;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Min", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Fog Distance Min EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSTPROCESSING_FOG_MIN);

	strData = CW2CT(to_wstring(fFogMinDistance).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);



	// Max
	m_rComponentRect.left = m_rComponentRect.right + 15;
	m_rComponentRect.right = m_rComponentRect.left + 30;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Max", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Fog Distance Max EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSTPROCESSING_FOG_MAX);

	strData = CW2CT(to_wstring(fFogMaxDistance).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);



	////////////////////		 Fog Color			////////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Fog Color", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// R
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"R", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Fog Color R EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSTPROCESSING_FOG_R);

	strData = CW2CT(to_wstring(vFogColor.x).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);



	// G
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"G", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Fog Color G EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSTPROCESSING_FOG_G);

	strData = CW2CT(to_wstring(vFogColor.y).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);

	// B
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"B", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Fog Color B EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSTPROCESSING_FOG_B);

	strData = CW2CT(to_wstring(vFogColor.z).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	// A
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 10;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"A", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Fog Color A EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSTPROCESSING_FOG_A);

	strData = CW2CT(to_wstring(vFogColor.w).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	////////////////////		 Blur Sample Count			////////////////////

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Blur", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 140;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Blur Sample Count", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Blur Sample Count EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSTPROCESSING_SAMPLECOUNT);

	strData = CW2CT(to_wstring(iBlurSampleCount).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	////////////////////		Depth of Field			////////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Depth of Field", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Blur Sample Count EditControl
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 60;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Range", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Min
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 30;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Min", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// DOF Min Range EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSTPROCESSING_DEPTH_OF_FIELD_MIN);

	strData = CW2CT(to_wstring(fDepthOfFieldMinRange).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	// Max
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 30;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Max", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// DOF Max Range EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSTPROCESSING_DEPTH_OF_FIELD_MAX);

	strData = CW2CT(to_wstring(fDepthOfFieldMaxRange).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	// Sigma
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 80;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Max Sigma", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// DOF Sigma EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSTPROCESSING_DEPTH_OF_FIELD_SIGMA);

	strData = CW2CT(to_wstring(fDepthOfFieldMaxSigma).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	////////////////////		HDR			////////////////////
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"HDR", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// HDR Exposure 
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 80;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Exposure", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// HDR Exposure EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 80;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSTPROCESSING_HDR_EXPOSURE);

	strData = CW2CT(to_wstring(fHDRExposure).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);



	// HDR White Threshold
	m_rComponentRect.left = m_rComponentRect.right + 15;
	m_rComponentRect.right = m_rComponentRect.left + 110;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"White Threshold", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// HDR White Threshold EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 80;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_POSTPROCESSING_HDR_WHITETHRESHOLD);

	strData = CW2CT(to_wstring(fHDRWhiteThreshold).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);


	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::CameraLayOut(CComponent * pComponent)
{
	CCamera* pCamera = dynamic_cast<CCamera*>(pComponent);

	// Camera Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Camera", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);



	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::CardinalSplineLayOut(CComponent * pComponent)
{
	CCardinalSpline* pCardinalSpline = dynamic_cast<CCardinalSpline*>(pComponent);

	// Animator Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"CardinalSpline", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);




	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::UpdateRigidbody()
{
	CRigidbody* pRigidbody = dynamic_cast<CRigidbody*>(GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent());
	
	
	CButton* pKinematic = dynamic_cast<CButton*>(GET_SINGLE(CEditorArchive)->GetComponentControl(pRigidbody, IDC_BUTTON_RIGIDBODY_IS_KINEMATIC));
	if (GetFocus() == pKinematic)
	{
		bool bKinematic = (bool)pKinematic->GetCheck();
		pRigidbody->SetKinematic(bKinematic);
	}


	CButton* pGravity = dynamic_cast<CButton*>(GET_SINGLE(CEditorArchive)->GetComponentControl(pRigidbody, IDC_BUTTON_RIGIDBODY_USE_GRAVITY));
	if (GetFocus() == pGravity)
	{
		bool bGravity = (bool)pGravity->GetCheck();
		pRigidbody->SetUseGravity(bGravity);
	}

	CString strValue;
	wstring strValueTranslater;
	
	CEdit* pMass = dynamic_cast<CEdit*>(GET_SINGLE(CEditorArchive)->GetComponentControl(pRigidbody, IDC_EDIT_RIGIDBODY_MASS));
	if (GetFocus() == pMass)
	{
		pMass->GetWindowTextW(strValue);
		strValueTranslater = strValue.GetString();
		float fMass = (float)atof(string(strValueTranslater.begin(), strValueTranslater.end()).c_str());
		pRigidbody->SetMass(fMass);
	}
	CEdit* pDrag = dynamic_cast<CEdit*>(GET_SINGLE(CEditorArchive)->GetComponentControl(pRigidbody, IDC_EDIT_RIGIDBODY_DRAG));
	if (GetFocus() == pDrag)
	{
		pDrag->GetWindowTextW(strValue);
		strValueTranslater = strValue.GetString();
		float fDrag = (float)atof(string(strValueTranslater.begin(), strValueTranslater.end()).c_str());
		pRigidbody->SetDrag(fDrag);
	}

	CEdit* pAngularDrag = dynamic_cast<CEdit*>(GET_SINGLE(CEditorArchive)->GetComponentControl(pRigidbody, IDC_EDIT_RIGIDBODY_ANGULAR_DRAG));
	if (GetFocus() == pAngularDrag)
	{
		pAngularDrag->GetWindowTextW(strValue);
		strValueTranslater = strValue.GetString();
		float fAngularDrag = (float)atof(string(strValueTranslater.begin(), strValueTranslater.end()).c_str());
		pRigidbody->SetAngularDrag(fAngularDrag);
	}

	DrawInspector();
}

void CInspector::EffectLayOut(CComponent * pComponent)
{
	CEffect*	pDecal = dynamic_cast<CEffect*>(pComponent);

	CEdit*		pEdit;
	CString		strData;

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Effect", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::GameSystemLayOut(CComponent * pComponent)
{
	CGameSystem* pGameSystem = dynamic_cast<CGameSystem*>(pComponent);

	CEdit*		pEdit;
	CString		strData;
	uint32_t	iSceneID = pGameSystem->GetSceneID();

	// Camera Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"GameSystem", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"SceneID", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	
	// Color R EditControl
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	pEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(pComponent, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_GAMESYSTEM_SCENE_ID);

	strData = CW2CT(to_wstring(iSceneID).c_str());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pEdit->SetWindowTextW(strData);

	// Line
	m_rComponentRect.top = m_rComponentRect.bottom + 5;
	m_rComponentRect.bottom = m_rComponentRect.top + 5;

	m_DC->MoveTo(0, m_rComponentRect.bottom);
	m_DC->LineTo(420, m_rComponentRect.bottom);
}

void CInspector::PortalLayOut(CComponent * pComponent)
{
	CPortal* pPortal = dynamic_cast<CPortal*>(pComponent);

	CEdit*		pEdit;
	CString		strData;

	// Portal Static 생성
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 200;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Portal", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	// Delete Button
	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 50;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

	// Portal Scene Type
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;
	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Select Scene", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 200;
	GET_SINGLE(CEditorArchive)->CreateComboBox(pComponent, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
		m_rComponentRect, CCT_SCENE, IDC_EDIT_SCENE_COMBOBOX);
}


void CInspector::LoadTerrainColliderHeightTexture()
{
	CFileDialog	dlg(TRUE, L"", L"", OFN_HIDEREADONLY,
		L"bmp(*.bmp)|*.bmp|AllFiles(*.*)|*.*||");

	if (dlg.DoModal() == IDOK)
	{
		CString	strFullPath = dlg.GetPathName();
		
		char* pRelativeDirectory = NULL;
		char* pPathKey = NULL;
		GET_SINGLE(CPathManager)->ExtractRelativeDirectoryFromFullPath(CT2CA(strFullPath.GetString()), &pRelativeDirectory, &pPathKey);
		string strReleativePath(pRelativeDirectory);
		
		CGameObject* pGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

		CTerrainCollider* pCollider = pGameObject->GetComponent<CTerrainCollider>();
		if (pCollider)
		{
			pCollider->LoadTerrain(30.0f, Vector3::One, pRelativeDirectory, pRelativeDirectory, pPathKey);
		}
		SAFE_RELEASE(pCollider);

		//CTerrain* pTerrain = pGameObject->GetComponent<CTerrain>();
		//
		//if (pTerrain)
		//{
		//	float fMaxHeight = pTerrain->GetMaxHeight();
		//
		//	CTerrainCollider* pCollider = pGameObject->GetComponent<CTerrainCollider>();
		//	if (pCollider)
		//	{
 		//		pCollider->LoadTerrain(fMaxHeight, Vector3::One, pRelativeDirectory, pRelativeDirectory, pPathKey);
		//	}
		//	SAFE_RELEASE(pCollider);
		//}
		//else
		//{
		//	GET_SINGLE(CDebug)->Log(L"Terrain 컴포넌트가 존재하지 않습니다.\nTerrain Collider 컴포넌트는 Terrain 컴포넌트가 존재해야 합니다.");
		//}
		//SAFE_RELEASE(pTerrain);		
	}

	DrawInspector();
}

void CInspector::EditTerrainColliderHeightTexture()
{
}

void CInspector::SplashLoadScene()
{
	CFileDialog	dlg(TRUE, L"SSSScene", L"", OFN_HIDEREADONLY,
		L"SSSScene(*.SSSScene)|*.*|AllFiles(*.*)|*.*||");

	if (dlg.DoModal() == IDOK)
	{
		CString	strFullPath = dlg.GetPathName();

		vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
		CSplash* pComponent = (CSplash*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();
		
		char* pRelativeDirectory = NULL;
		char* pPathKey = NULL;
		GET_SINGLE(CPathManager)->ExtractRelativeDirectoryFromFullPath(CT2CA(strFullPath.GetString()), &pRelativeDirectory, &pPathKey);

		string strReleativePath(pRelativeDirectory);
		

		pComponent->SetPath(wstring(strReleativePath.begin(), strReleativePath.end()));
		pComponent->SetPathKey(pPathKey);
	}

	DrawInspector();
}

void CInspector::SplashClearScene()
{
}

void CInspector::EditDirectionalLight()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CDirectionalLight* pDirLight = (CDirectionalLight*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	Vector4		vDiffuse;
	Vector4		vAmbient;
	Vector4		vSpecular;
	CString		strData;
	float		fData;

	size_t iSize = pComponentControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pDirLight == pComponentControl[i]->pComponent)
		{
			// ID값으로 해당 핸들 값을 가져오면 된다.
			// Diffuse
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_DIRECTIONAL_DIFFUSE_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_DIRECTIONAL_DIFFUSE_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.y = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_DIRECTIONAL_DIFFUSE_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.z = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_DIRECTIONAL_DIFFUSE_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.w = fData;
			}

			// Ambient
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_DIRECTIONAL_AMBIENT_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_DIRECTIONAL_AMBIENT_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_DIRECTIONAL_AMBIENT_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.z = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_DIRECTIONAL_AMBIENT_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.w = fData;
			}

			// Specular
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_DIRECTIONAL_SPECULAR_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_DIRECTIONAL_SPECULAR_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_DIRECTIONAL_SPECULAR_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.z = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_DIRECTIONAL_SPECULAR_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.w = fData;
			}
		}
	}

	pDirLight->SetLightColor(vDiffuse, vAmbient, vSpecular);
}

void CInspector::EditPointLight()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CPointLight* pPointLight = (CPointLight*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	Vector4		vDiffuse;
	Vector4		vAmbient;
	Vector4		vSpecular;
	CString		strData;
	float		fData;
	float		fPointLightRange;

	size_t iSize = pComponentControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pPointLight == pComponentControl[i]->pComponent)
		{
			// ID값으로 해당 핸들 값을 가져오면 된다.
			// Diffuse
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_POINT_DIFFUSE_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_POINT_DIFFUSE_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.y = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_POINT_DIFFUSE_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.z = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_POINT_DIFFUSE_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.w = fData;
			}

			// Ambient
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_POINT_AMBIENT_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_POINT_AMBIENT_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_POINT_AMBIENT_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.z = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_POINT_AMBIENT_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.w = fData;
			}

			// Specular
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_POINT_SPECULAR_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_POINT_SPECULAR_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_POINT_SPECULAR_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.z = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_POINT_SPECULAR_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.w = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_POINT_RANGE)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				fPointLightRange = fData;
			}
		}
	}

	pPointLight->SetLightColor(vDiffuse, vAmbient, vSpecular);
	pPointLight->SetRange(fPointLightRange);
}

void CInspector::EditSpotLight()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CPointLight* pSpotLight = (CPointLight*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	Vector4		vDiffuse;
	Vector4		vAmbient;
	Vector4		vSpecular;
	CString		strData;
	float		fData;

	size_t iSize = pComponentControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pSpotLight == pComponentControl[i]->pComponent)
		{
			// ID값으로 해당 핸들 값을 가져오면 된다.
			// Diffuse
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_SPOT_DIFFUSE_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_SPOT_DIFFUSE_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.y = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_SPOT_DIFFUSE_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.z = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_SPOT_DIFFUSE_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.w = fData;
			}

			// Ambient
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_SPOT_AMBIENT_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_SPOT_AMBIENT_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_SPOT_AMBIENT_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.z = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_SPOT_AMBIENT_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.w = fData;
			}

			// Specular
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_SPOT_SPECULAR_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_SPOT_SPECULAR_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_SPOT_SPECULAR_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.z = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_LIGHT_SPOT_SPECULAR_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.w = fData;
			}
		}
	}

	pSpotLight->SetLightColor(vDiffuse, vAmbient, vSpecular);
}

void CInspector::EditTransform()
{
	if (m_pSelectObject)
	{
		Vector3		vPosition;
		Vector3		vScale;
		Vector3		vRotation;
		CString		strData;

#pragma region Position
		{
			// Edit Control의 문자열을 읽어와서 float으로 변환해준다.
			CWnd* pWnd = GetDlgItem(IDC_EDIT_POSITION_X);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				float fValue = (float)atof((CStringA)strData);
				vPosition.x = fValue;
			}

			pWnd = GetDlgItem(IDC_EDIT_POSITION_Y);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				float fValue = (float)atof((CStringA)strData);
				vPosition.y = fValue;
			}

			pWnd = GetDlgItem(IDC_EDIT_POSITION_Z);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				float fValue = (float)atof((CStringA)strData);
				vPosition.z = fValue;
			}
		}
#pragma endregion

#pragma region Scale
		{
			CWnd* pWnd = GetDlgItem(IDC_EDIT_SCALE_X);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				float fValue = (float)atof((CStringA)strData);
				vScale.x = fValue;
			}

			pWnd = GetDlgItem(IDC_EDIT_SCALE_Y);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				float fValue = (float)atof((CStringA)strData);
				vScale.y = fValue;
			}

			pWnd = GetDlgItem(IDC_EDIT_SCALE_Z);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				float fValue = (float)atof((CStringA)strData);
				vScale.z = fValue;
			}
		}
#pragma endregion

#pragma region Rotation
		{
			CWnd* pWnd = GetDlgItem(IDC_EDIT_ROTATION_X);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				float fValue = (float)atof((CStringA)strData);
				vRotation.x = fValue;
			}

			pWnd = GetDlgItem(IDC_EDIT_ROTATION_Y);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				float fValue = (float)atof((CStringA)strData);
				vRotation.y = fValue;
			}
			pWnd = GetDlgItem(IDC_EDIT_ROTATION_Z);
			if (pWnd)
			{
				pWnd->GetWindowTextW(strData);
				float fValue = (float)atof((CStringA)strData);
				vRotation.z = fValue;
			}
		}
#pragma endregion


		

		CTransform*	pObjectTr = m_pSelectObject->GetTransform();
		CGizmo*	pGizmo = GET_SINGLE(CEditorArchive)->GetGizmo();
		if (pGizmo)
		{
			vRotation *= D2R;
			if (!m_bLocalTrans)
			{
				pObjectTr->SetWorldPosition(vPosition);
				pObjectTr->SetWorldScale(vScale);
				pObjectTr->SetWorldRotation(vRotation);
				pGizmo->UpdateGizmoTransform();
			}
			else
			{
				pObjectTr->SetLocalPosition(vPosition);
				pObjectTr->SetLocalScale(vScale);
				pObjectTr->SetLocalRotation(vRotation);
				pGizmo->UpdateGizmoLocalTransform();
			}
		}
		SAFE_RELEASE(pObjectTr);

		SAFE_RELEASE(pGizmo);

		GET_SINGLE(CEditorArchive)->SetTransformState(true);
	}
}

void CInspector::EditMaterialColor()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CMeshRenderer* pMeshRenderer = (CMeshRenderer*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();
	CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
	CSpinButtonCtrl*	pContainerSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SPIN_MATERIAL_CONTAINER_SIZE);
	CSpinButtonCtrl*	pSubsetSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SPIN_MATERIAL_SUBSET_SIZE);

	uint32_t	iContainer = pContainerSpin->GetPos();
	uint32_t	iSubset = pSubsetSpin->GetPos();

	Vector4		vDiffuse;
	Vector4		vAmbient;
	Vector4		vSpecular;
	Vector4		vEmissive;
	Vector4		vAlpha;
	CString		strData;
	float		fData;

	size_t iSize = pComponentControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pMeshRenderer == pComponentControl[i]->pComponent)
		{
			// ID값으로 해당 핸들 값을 가져오면 된다.
			// Diffuse
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_DIFFUSE_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.x = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_DIFFUSE_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.y = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_DIFFUSE_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.z = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_DIFFUSE_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.w = fData;
			}

			// Ambient
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_AMBIENT_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_AMBIENT_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_AMBIENT_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.z = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_AMBIENT_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAmbient.w = fData;
			}

			// Specular
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_SPECULAR_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_SPECULAR_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_SPECULAR_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.z = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_SPECULAR_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vSpecular.w = fData;
			}



			// Emissive
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_EMISSIVE_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vEmissive.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_EMISSIVE_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vEmissive.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_EMISSIVE_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vEmissive.z = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_EMISSIVE_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vEmissive.w = fData;
			}



			// Alpha
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_ALPHA_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAlpha.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_ALPHA_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAlpha.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_ALPHA_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAlpha.z = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_MATERIAL_ALPHA_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vAlpha.w = fData;
			}
		}
	}

	pMaterial->SetDiffuseColor(vDiffuse, iContainer, iSubset);
	pMaterial->SetAmbientColor(vAmbient, iContainer, iSubset);
	pMaterial->SetSpecularColor(vSpecular, iContainer, iSubset);
	pMaterial->SetEmissiveColor(vEmissive, iContainer, iSubset);
	pMaterial->SetAlphaColor(vAlpha, iContainer, iSubset);

	SAFE_RELEASE(pMaterial);
}

void CInspector::EditShaderChange()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CMeshRenderer* pMeshRenderer = (CMeshRenderer*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	size_t iSize = pComponentControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pMeshRenderer == pComponentControl[i]->pComponent)
		{
			// ID값으로 해당 핸들 값을 가져오면 된다.
			// Diffuse
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_SHADER_COMBOBOX)
			{
				CComboBox* pShaderComboBox = (CComboBox*)(pComponentControl[i]->hWnd);
				int		iShaderIndex = pShaderComboBox->GetCurSel();
				if (iShaderIndex >= 0)
				{
					CString		strData;
					pShaderComboBox->GetLBText(iShaderIndex, strData);

					if (strData == L"Render Target")
					{
						pMeshRenderer->SetShader(UI_RENDERTARGET_SHADER);
					}

					else if (strData == L"Standard Color")
					{
						pMeshRenderer->SetShader(STANDARD_COLOR_SHADER);
					}

					else if (strData == L"Standard Tex")
					{
						pMeshRenderer->SetShader(STANDARD_TEXTURE_SHADER);
					}

					else if (strData == L"Colored Tex")
					{
						pMeshRenderer->SetShader(COLORED_TEXTURE_SHADER);
					}

					else if (strData == L"Collider")
					{
						pMeshRenderer->SetShader(COLLIDER_SHADER);
					}

					else if (strData == L"UI")
					{
						pMeshRenderer->SetShader(UI_TEXTURE_SHADER);
					}

					else if (strData == L"UI Collider")
					{
						pMeshRenderer->SetShader(UI_COLLIDER_SHADER);
					}

					else if (strData == L"UI Grid")
					{
						pMeshRenderer->SetShader(UI_GRID_SHADER);
					}

					else if (strData == L"Standard Color Normal")
					{
						pMeshRenderer->SetShader(STANDARD_COLOR_NORMAL_SHADER);
						pMeshRenderer->SetInputLayout(POS_COLOR_NORMAL_LAYOUT);
					}

					else if (strData == L"Standard Texture Normal")
					{
						pMeshRenderer->SetShader(STANDARD_TEXTURE_NORMAL_SHADER);
						pMeshRenderer->SetInputLayout(POS_UV_NORMAL_LAYOUT);
					}

					else if (strData == L"Standard Bump")
					{
						pMeshRenderer->SetShader(STANDARD_BUMP_SHADER);
						pMeshRenderer->SetInputLayout(BUMP_LAYOUT);
					}

					else if (strData == L"Rim Light")
					{
						pMeshRenderer->SetShader(RIM_LIGHT_SHADER);
					}

					else if (strData == L"Out line")
					{
						pMeshRenderer->SetShader(OUTLINE_SHADER);
					}

					else if (strData == L"StandardBumpAnimShader")
					{
						pMeshRenderer->SetShader(STANDARD_BUMP_ANIM_SHADER);
						pMeshRenderer->SetInputLayout(BUMP_ANIM_LAYOUT);
					}

					else if (strData == L"StandardTextureNormalAnimShader")
					{
						pMeshRenderer->SetShader(STANDARD_TEXTURE_NORMAL_ANIM_SHADER);
						pMeshRenderer->SetInputLayout(POS_TEXTURE_NORMAL_ANIM_LAYOUT);
					}
				}
			}
		}
	}
}

void CInspector::EditPortalSceneChange()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CPortal* pPortal = (CPortal*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	size_t iSize = pComponentControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pPortal == pComponentControl[i]->pComponent)
		{
			// ID값으로 해당 핸들 값을 가져오면 된다.
			// Diffuse
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_SCENE_COMBOBOX)
			{
				CComboBox* pSceneComboBox = (CComboBox*)(pComponentControl[i]->hWnd);
				int		iSceneIndex = pSceneComboBox->GetCurSel();
				if (iSceneIndex >= 0)
				{
					CString		strData;
					pSceneComboBox->GetLBText(iSceneIndex, strData);

					if (strData == L"Terny Day")
					{
						pPortal->SetSceneType(PLS_TERNY_DAY);
					}
					else if (strData == L"Terny Night")
					{
						pPortal->SetSceneType(PLS_TERNY_NIGHT);
					}
					else if (strData == L"Kings Road Day")
					{
						pPortal->SetSceneType(PLS_KINGSROAD_DAY);
					}
					else if (strData == L"Kings Road Night")
					{
						pPortal->SetSceneType(PLS_KINGSROAD_NIGHT);
					}
					else if (strData == L"Battle Field")
					{
						pPortal->SetSceneType(PLS_BATTLFIELD_DAY);
					}
				}
			}
		}
	}
}

void CInspector::EditCameraMove()
{
	CScene*	pScene = m_pSelectObject->GetScene();
	CGameObject*	pMainCamera = pScene->GetMainCameraObject();

	CString		strData;
	float		fSpeed;

	if (m_pSelectObject == pMainCamera)
	{
		CCameraMove* pCameraMove = pMainCamera->GetComponent<CCameraMove>();
		CEdit*	pSpeedEdit = (CEdit*)GET_SINGLE(CEditorArchive)->GetComponentControl(pCameraMove, IDC_EDIT_CAMERA_MOVESPEED);
		pSpeedEdit->GetWindowTextW(strData);
		fSpeed = (float)atof((CStringA)strData);
		pCameraMove->SetSpeed(fSpeed);
		SAFE_RELEASE(pCameraMove);
	}
	else
	{ 
		vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
		CCameraMove* pCameraMove = (CCameraMove*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

		size_t iSize = pComponentControl.size();

		for (size_t i = 0; i < iSize; ++i)
		{
			if (pCameraMove == pComponentControl[i]->pComponent)
			{
				if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_CAMERA_MOVESPEED)
				{
					pComponentControl[i]->hWnd->GetWindowTextW(strData);
					fSpeed = (float)atof((CStringA)strData);
					break;
				}
			}
		}

		pCameraMove->SetSpeed(fSpeed);
	}

	

	SAFE_RELEASE(pMainCamera);
	SAFE_RELEASE(pScene);
}

void CInspector::EditRectCollider2D()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CRectCollider2D* pRectCollider2D = (CRectCollider2D*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	CEdit*		pEdit;
	CString		strData;
	Vector3		vObjPivot;
	Vector3		vObjScale;
	float		fData;

	size_t iSize = pComponentControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pRectCollider2D == pComponentControl[i]->pComponent)
		{
			// ID값으로 해당 핸들 값을 가져오면 된다.
			// Pivot
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_RECTCOLLIDE2D_PIVOT_X)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vObjPivot.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_RECTCOLLIDE2D_PIVOT_Y)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vObjPivot.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_RECTCOLLIDE2D_PIVOT_Z)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vObjPivot.z = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_RECTCOLLIDE2D_SCALE_X)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vObjScale.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_RECTCOLLIDE2D_SCALE_Y)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vObjScale.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_RECTCOLLIDE2D_SCALE_Z)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vObjScale.z = fData;
			}
		}
	}

	pRectCollider2D->SetPivot(vObjPivot);
	pRectCollider2D->SetScale(vObjScale);
}

void CInspector::EditOutLine()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	COutLine* pOutLine = (COutLine*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	CEdit*		pEdit;
	CString		strData;
	Vector3		vOffset;
	Vector4		vColor;
	float		fSize;
	float		fData;

	size_t iSize = pComponentControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pOutLine == pComponentControl[i]->pComponent)
		{
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_OUTLINE_OFFSET_X)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vOffset.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_OUTLINE_OFFSET_Y)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vOffset.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_OUTLINE_OFFSET_Z)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vOffset.z = fData;
			}
			// Color
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_OUTLINE_COLOR_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vColor.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_OUTLINE_COLOR_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vColor.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_OUTLINE_COLOR_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vColor.z = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_OUTLINE_COLOR_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vColor.w = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_OUTLINE_SIZE)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				fSize = fData;
			}
		}
	}

	pOutLine->SetOutlineColor(vColor);
	pOutLine->SetOutlineOffset(vOffset);
	pOutLine->SetOutlineSize(fSize);
}

void CInspector::EditSphereCollider()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CSphereCollider* pSphereCollider = (CSphereCollider*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	CEdit*		pEdit;
	CString		strData;
	float		fData;
	float		fRadius;
	Vector3		vLocalPos;

	size_t iSize = pComponentControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pSphereCollider == pComponentControl[i]->pComponent)
		{
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_SPHERECOLLIDER_RADIUS)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				fRadius = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_SPHERECOLLIDER_LOCAL_X)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vLocalPos.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_SPHERECOLLIDER_LOCAL_Y)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vLocalPos.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_SPHERECOLLIDER_LOCAL_Z)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vLocalPos.z = fData;
			}
		}
	}

	pSphereCollider->SetLocalPosition(vLocalPos);
	pSphereCollider->SetRadius(fRadius);
}

void CInspector::EditBoxCollider()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CBoxCollider* pBoxCollider = (CBoxCollider*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	CString		strData;
	float		fData;
	Vector3		vLocalPos;
	Vector3		vVolume;

	size_t iSize = pComponentControl.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pBoxCollider == pComponentControl[i]->pComponent)
		{
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_BOXCOLLIDER_VOLUME_X)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vVolume.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_BOXCOLLIDER_VOLUME_Y)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vVolume.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_BOXCOLLIDER_VOLUME_Z)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vVolume.z = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_BOXCOLLIDER_LOCAL_X)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vLocalPos.x = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_BOXCOLLIDER_LOCAL_Y)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vLocalPos.y = fData;
			}
			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_BOXCOLLIDER_LOCAL_Z)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vLocalPos.z = fData;
			}
		}
	}

	pBoxCollider->SetLocalPosition(vLocalPos);
	pBoxCollider->SetVolume(vVolume);
}

void CInspector::EditCircleCollider2D()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CCircleCollider2D* pCircleCollider2D = (CCircleCollider2D*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	CString		strData;
	float		fData;
	float		fRadius;

	size_t iSize = pComponentControl.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pCircleCollider2D == pComponentControl[i]->pComponent)
		{
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_CIRCLECOLLIDER2D_RADIUS)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				fRadius = fData;
			}
		}
	}

	pCircleCollider2D->SetRadius(fRadius);
}

void CInspector::EditPostProcessingFog()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CPostProcessing* pPostProcessing = (CPostProcessing*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	CString		strData;
	float		fData;
	Vector4		vFogColor;
	float		fFogMinDistance;
	float		fFogMaxDistance;

	size_t iSize = pComponentControl.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pPostProcessing == pComponentControl[i]->pComponent)
		{
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_POSTPROCESSING_FOG_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vFogColor.x = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_POSTPROCESSING_FOG_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vFogColor.y = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_POSTPROCESSING_FOG_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vFogColor.z = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_POSTPROCESSING_FOG_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vFogColor.w = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_POSTPROCESSING_FOG_MIN)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				fFogMinDistance = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_POSTPROCESSING_FOG_MAX)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				fFogMaxDistance = fData;
			}
		}
	}

	pPostProcessing->SetFogColor(vFogColor);
	pPostProcessing->SetFogDistance(fFogMinDistance, fFogMaxDistance);
}

void CInspector::EditPostProcessingBlur()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CPostProcessing* pPostProcessing = (CPostProcessing*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	CString		strData;
	float		fData;
	uint32_t	iBlurSampleCount;

	size_t iSize = pComponentControl.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pPostProcessing == pComponentControl[i]->pComponent)
		{
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_POSTPROCESSING_SAMPLECOUNT)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				iBlurSampleCount = fData;
				break;
			}
		}
	}

	pPostProcessing->SetBlurSampleCount(iBlurSampleCount);
}

void CInspector::EditPostProcessingFlag()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CPostProcessing* pPostProcessing = (CPostProcessing*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	size_t iSize = pComponentControl.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pPostProcessing == pComponentControl[i]->pComponent)
		{
			HWND pCurrentFocusHandle = GetFocus()->GetSafeHwnd();
			if (pCurrentFocusHandle == GetDlgItem(IDC_BUTTON_POSTPROCESSING_ANTIALIASING)->GetSafeHwnd())
			{
				pPostProcessing->ToggleFlags(PT_ANTIALIASING);
				break;
			}

			else if (pCurrentFocusHandle == GetDlgItem(IDC_BUTTON_POSTPROCESSING_EYE_ADAPTION)->GetSafeHwnd())
			{
				pPostProcessing->ToggleFlags(PT_EYE_ADAPTION);
				break;
			}

			else if (pCurrentFocusHandle == GetDlgItem(IDC_BUTTON_POSTPROCESSING_BLOOM)->GetSafeHwnd())
			{
				pPostProcessing->ToggleFlags(PT_BLOOM);
				break;
			}

			else if (pCurrentFocusHandle == GetDlgItem(IDC_BUTTON_POSTPROCESSING_FOG)->GetSafeHwnd())
			{
				pPostProcessing->ToggleFlags(PT_FOG);
				break;
			}

			else if (pCurrentFocusHandle == GetDlgItem(IDC_BUTTON_POSTPROCESSING_HDR)->GetSafeHwnd())
			{
				pPostProcessing->ToggleFlags(PT_HDR);
				break;
			}

			else if (pCurrentFocusHandle == GetDlgItem(IDC_BUTTON_POSTPROCESSING_MOTION_BLUR)->GetSafeHwnd())
			{
				pPostProcessing->ToggleFlags(PT_MOTION_BLUR);
				break;
			}

			else if (pCurrentFocusHandle == GetDlgItem(IDC_BUTTON_POSTPROCESSING_AMBIENT_OCCLUSION)->GetSafeHwnd())
			{
				pPostProcessing->ToggleFlags(PT_AMBIENT_OCCLUSION);
				break;
			}

			else if (pCurrentFocusHandle == GetDlgItem(IDC_BUTTON_POSTPROCESSING_DEPTH_OF_FIELD)->GetSafeHwnd())
			{
				pPostProcessing->ToggleFlags(PT_DEPTH_OF_FIELD);
				break;
			}
		}
	}
}

void CInspector::EditPostProcessingDOF()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CPostProcessing* pPostProcessing = (CPostProcessing*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	CString		strData;
	float		fData;
	float		fDepthOfFieldMinRange;
	float		fDepthOfFieldMaxRange;
	float		fDepthOfFieldMaxSigma;

	size_t iSize = pComponentControl.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pPostProcessing == pComponentControl[i]->pComponent)
		{
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_POSTPROCESSING_DEPTH_OF_FIELD_MIN)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				fDepthOfFieldMinRange = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_POSTPROCESSING_DEPTH_OF_FIELD_MAX)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				fDepthOfFieldMaxRange = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_POSTPROCESSING_DEPTH_OF_FIELD_SIGMA)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				fDepthOfFieldMaxSigma = fData;
			}
		}
	}

	pPostProcessing->SetDepthOfField(fDepthOfFieldMinRange, fDepthOfFieldMaxRange, fDepthOfFieldMaxSigma);
}

void CInspector::EditPostProcessingHDR()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CPostProcessing* pPostProcessing = (CPostProcessing*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	CString		strData;
	float		fData;
	float		fHDRExposure;
	float		fHDRWhiteThreshold;

	size_t iSize = pComponentControl.size();
	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pPostProcessing == pComponentControl[i]->pComponent)
		{
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_POSTPROCESSING_HDR_EXPOSURE)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				fHDRExposure = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_POSTPROCESSING_HDR_WHITETHRESHOLD)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				fHDRWhiteThreshold = fData;
			}
		}
	}

	pPostProcessing->SetHDRExposure(fHDRExposure);
	pPostProcessing->SetHDRWhiteThreshold(fHDRWhiteThreshold);
}

void CInspector::EditSpriteRenderer()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CSpriteRenderer* pSpriteRenderer = (CSpriteRenderer*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	Vector4		vDiffuse;
	CString		strData;
	float		fData;
	int			iZOrder;

	size_t iSize = pComponentControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pSpriteRenderer == pComponentControl[i]->pComponent)
		{
			// ID값으로 해당 핸들 값을 가져오면 된다.
			// Diffuse
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_SPRITERENDERER_DIFFUSE_R)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.x = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_SPRITERENDERER_DIFFUSE_G)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.y = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_SPRITERENDERER_DIFFUSE_B)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.z = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_SPRITERENDERER_DIFFUSE_A)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				vDiffuse.w = fData;
			}

			else if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_SPRITERENDERER_ZORDER)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				iZOrder = fData;
			}
		}
	}

	pSpriteRenderer->SetDiffuse(vDiffuse);
	pSpriteRenderer->SetZOrder(iZOrder);
}


void CInspector::EditTrail()
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CTrail* pTrail = (CTrail*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();

	CString		strData;
	float		fData = 0.f;

	float		fSize = 0.f;

	size_t iSize = pComponentControl.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// 컴포넌트 비교
		if (pTrail == pComponentControl[i]->pComponent)
		{
			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_EDIT_TRAIL_SIZE)
			{
				pComponentControl[i]->hWnd->GetWindowTextW(strData);
				fData = (float)atof((CStringA)strData);
				fSize = fData;
			}
		}
	}

	pTrail->SetSize(fSize);
}


void CInspector::EditNormalScale(NMHDR * pNMHDR, LRESULT * pResult)
{
	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
	CMeshRenderer* pMeshRenderer = (CMeshRenderer*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();
	CMaterial*	pMaterial = pMeshRenderer->GetMaterial();

	float		fNormalScale;

	CSliderCtrl*	pSlider = (CSliderCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SLIDER_MATERIAL_NORMAL_SCALE);
	fNormalScale = pSlider->GetPos();

	CSpinButtonCtrl*	pContainerSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SPIN_MATERIAL_CONTAINER_SIZE);
	CSpinButtonCtrl*	pSubsetSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SPIN_MATERIAL_SUBSET_SIZE);
	
	int iContainer = pContainerSpin->GetPos();
	int iSubset = pSubsetSpin->GetPos();

	pMaterial->SetNormalScale(fNormalScale / 100, iContainer, iSubset);

	CEdit*	pNormalEdit = (CEdit*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_EDIT_MATERIAL_NORMAL_SCALE);
	CString strNormal =	CW2CT(to_wstring(fNormalScale).c_str());

	pNormalEdit->SetWindowTextW(strNormal);
	SAFE_RELEASE(pMaterial);
}


void CInspector::ChangeTransform()
{
	if (!m_bLocalTrans)
	{
		m_bLocalTrans = true;
		CGizmo*	pGizmo = GET_SINGLE(CEditorArchive)->GetGizmo();
		pGizmo->UpdateGizmoLocalTransform();
		pGizmo->SetLocalMode(true);
		SAFE_RELEASE(pGizmo);

	}
	else
	{
		m_bLocalTrans = false;
		CGizmo*	pGizmo = GET_SINGLE(CEditorArchive)->GetGizmo();
		pGizmo->UpdateGizmoTransform();
		pGizmo->SetLocalMode(false);
		SAFE_RELEASE(pGizmo);
	}
		
	DrawInspector();

}

void CInspector::LoadMesh()
{// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CFileDialog	dlg(TRUE, L"mesh", L"Default", OFN_HIDEREADONLY,
		L"Mesh(*.*)|*.*|AllFiles(*.*)|*.*||");

	if (dlg.DoModal() == IDOK)
	{
		CString	strFullPath = dlg.GetPathName();

		vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
		CMeshRenderer* pMeshRenderer = (CMeshRenderer*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();
		CMaterial*	pMaterial = pMeshRenderer->GetMaterial();

		CMesh* pMesh = GET_SINGLE(CResourcesManager)->LoadMesh("", CT2CW(strFullPath), NO_USE_PATH);

		CButton* pOverrideMaterial = (CButton*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_BUTTON_OVERRIDE_MATERIAL);
		
		bool bOverride = (bool)pOverrideMaterial->GetCheck();

		pMeshRenderer->SetMesh(pMesh, bOverride);

		SAFE_RELEASE(pMesh);
		SAFE_RELEASE(pMaterial);
	}
}

// Mesh Renderer Save Button Click Event
void CInspector::SaveMesh()
{
	CGameObject* pSelectedGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
	if (pSelectedGameObject)
	{
		CMeshRenderer* pRenderer = pSelectedGameObject->GetComponent<CMeshRenderer>();

		if (pRenderer)
		{
			CMesh* pMesh = pRenderer->GetMesh();
			if (pMesh)
			{
				string strKey = pMesh->GetKey();
				CFileDialog	dlg(FALSE, L"SSSMesh", wstring(strKey.begin(), strKey.end()).c_str(), OFN_HIDEREADONLY,
					L"SSSMesh(*.SSSMesh)|*.*|AllFiles(*.*)|*.*||");

				if (dlg.DoModal() == IDOK)
				{
					CString	strFullPath = dlg.GetPathName();										
					pMesh->Save(strFullPath, NO_USE_PATH);
				}
			}
		}
		SAFE_RELEASE(pRenderer);
	}
}

void CInspector::LoadMaterialTexture()
{// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CFileDialog	dlg(TRUE, L"meta", L"Default", OFN_HIDEREADONLY,
		L"Material(*.tga)|*.tga|AllFiles(*.*)|*.*||");


	if (dlg.DoModal() == IDOK)
	{
		CString	strFullPath = dlg.GetPathName();

		CMeshRenderer* pMeshRenderer = (CMeshRenderer*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();
		CMaterial*	pMaterial = pMeshRenderer->GetMaterial();

		CSpinButtonCtrl*	pContainerSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SPIN_MATERIAL_CONTAINER_SIZE);
		CSpinButtonCtrl*	pSubsetSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SPIN_MATERIAL_SUBSET_SIZE);

		uint32_t	iContainer = pContainerSpin->GetPos();
		uint32_t	iSubset = pSubsetSpin->GetPos();

		char* pFileKey = NULL;
		char* pFilePath = NULL;

		GET_SINGLE(CPathManager)->ExtractRelativeDirectoryFromFullPath((CT2CA)strFullPath, &pFileKey, &pFilePath);		
		
		HWND pCurrentFocusHandle = GetFocus()->GetSafeHwnd();
				
		if (pCurrentFocusHandle == GetDlgItem(IDC_EDIT_MATERIAL_DIFFUSE_TEXTURE_BUTTON)->GetSafeHwnd())
		{
			pMaterial->SetDiffuseTextureFromFullPath(pFileKey, strFullPath, iContainer, iSubset);
			CEdit* pControl = (CEdit*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_EDIT_MATERIAL_DIFFUSE_KEY);
			CString strFilekey;
			strFilekey = pFileKey;
			pControl->SetWindowTextW(strFilekey);
		}
		else if (pCurrentFocusHandle == GetDlgItem(IDC_EDIT_MATERIAL_NORMAL_TEXTURE_BUTTON)->GetSafeHwnd())
		{
			pMaterial->SetNormalTextureFromFullPath(pFileKey, strFullPath, iContainer, iSubset);
			CEdit* pControl = (CEdit*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_EDIT_MATERIAL_NORMAL_KEY);
			CString strFilekey;
			strFilekey = pFileKey;
			pControl->SetWindowTextW(strFilekey);
		}
		else if (pCurrentFocusHandle == GetDlgItem(IDC_EDIT_MATERIAL_SPECULAR_TEXTURE_BUTTON)->GetSafeHwnd())
		{
			pMaterial->SetSpecularTextureFromFullPath(pFileKey, strFullPath, iContainer, iSubset);
			CEdit* pControl = (CEdit*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_EDIT_MATERIAL_SPECULAR_KEY);
			CString strFilekey;
			strFilekey = pFileKey;
			pControl->SetWindowTextW(strFilekey);
		}
		else if (pCurrentFocusHandle == GetDlgItem(IDC_EDIT_MATERIAL_ALPHA_TEXTURE_BUTTON)->GetSafeHwnd())
		{
			pMaterial->SetAlphaTextureFromFullPath(pFileKey, strFullPath, iContainer, iSubset);
			CEdit* pControl = (CEdit*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_EDIT_MATERIAL_ALPHA_KEY);
			CString strFilekey;
			strFilekey = pFileKey;
			pControl->SetWindowTextW(strFilekey);
		}

		SAFE_RELEASE(pMaterial);
	}
}

void CInspector::ClearMaterialTexture()
{
	CMeshRenderer* pMeshRenderer = (CMeshRenderer*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();
	CMaterial*	pMaterial = pMeshRenderer->GetMaterial();

	HWND pCurrentFocusHandle = GetFocus()->GetSafeHwnd();


	CSpinButtonCtrl*	pContainerSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SPIN_MATERIAL_CONTAINER_SIZE);
	CSpinButtonCtrl*	pSubsetSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SPIN_MATERIAL_SUBSET_SIZE);

	uint32_t	iContainer = pContainerSpin->GetPos();
	uint32_t	iSubset = pSubsetSpin->GetPos();

	if (pCurrentFocusHandle == GetDlgItem(IDC_EDIT_MATERIAL_DIFFUSE_CLEAR_BUTTON)->GetSafeHwnd())
	{
		pMaterial->SetDiffuseTexture(NULL, iContainer, iSubset);
		CEdit* pControl = (CEdit*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_EDIT_MATERIAL_DIFFUSE_KEY);
		pControl->SetWindowTextW(L"NONE");
	}
	else if (pCurrentFocusHandle == GetDlgItem(IDC_EDIT_MATERIAL_NORMAL_CLEAR_BUTTON)->GetSafeHwnd())
	{	
		pMaterial->SetNormalTexture(NULL, iContainer, iSubset);
		CEdit* pControl = (CEdit*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_EDIT_MATERIAL_NORMAL_KEY);
		pControl->SetWindowTextW(L"NONE");
	}
	else if (pCurrentFocusHandle == GetDlgItem(IDC_EDIT_MATERIAL_SPECULAR_CLEAR_BUTTON)->GetSafeHwnd())
	{
		pMaterial->SetSpecularTexture(NULL, iContainer, iSubset);
		CEdit* pControl = (CEdit*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_EDIT_MATERIAL_SPECULAR_KEY);
		pControl->SetWindowTextW(L"NONE");
	}
	else if (pCurrentFocusHandle == GetDlgItem(IDC_EDIT_MATERIAL_ALPHA_CLEAR_BUTTON)->GetSafeHwnd())
	{
		pMaterial->SetAlphaTexture(NULL, iContainer, iSubset);
		CEdit* pControl = (CEdit*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_EDIT_MATERIAL_ALPHA_KEY);
		pControl->SetWindowTextW(L"NONE");
	}

	SAFE_RELEASE(pMaterial);
}

void CInspector::RemoveComponent()
{
	CComponent* pComponent = GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();
	
	CGameObject* pGameObject = pComponent->GetGameObject(); 
	pGameObject->RemoveComponentFromList(pComponent);
	pComponent->DestroyImmediate();


	SAFE_RELEASE(pGameObject);



	DrawInspector();
}

void CInspector::AddContainer()
{
	CMeshRenderer* pMeshRenderer = (CMeshRenderer*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();
	CWnd* pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer,IDC_SPIN_MATERIAL_CONTAINER_SIZE);
	if (pComponentControl)
	{
		CSpinButtonCtrl* pControl = ((CSpinButtonCtrl*)pComponentControl);
		CMesh* pMesh = pMeshRenderer->GetMesh();
		size_t iContainerCount = pMesh->GetContainerCount();
		CMaterial* pMaterial = pMeshRenderer->GetMaterial();

		while (pMaterial->GetContainerCount() < iContainerCount)
		{
			pMaterial->AddContainer();
		}
		pControl->SetRange(0, pMaterial->GetContainerCount() - 1);
		SAFE_RELEASE(pMaterial);
	}
	else
	{
		DrawInspector();
		return;
	}
	DrawInspector();
	return;
}

void CInspector::AddSubset()
{
//	vector<COMPONENTCONTROL*> pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl();
//	CMeshRenderer* pMeshRenderer = (CMeshRenderer*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();
//	CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
//
//	size_t iSize = pComponentControl.size();
//	CString		strData;
//	float		fData;
//	int			iContainerSize = 0;
//
//	for (size_t i = 0; i < iSize; ++i)
//	{
//		// 컴포넌트 비교
//		if (pMeshRenderer == pComponentControl[i]->pComponent)
//		{
//			// ID값으로 해당 핸들 값을 가져오면 된다.
//			// Diffuse
//			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_SPIN_MATERIAL_CONTAINER_SIZE)
//			{
//				CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)pComponentControl[i]->hWnd;
//				iContainerSize = pSpin->GetPos();
//				pMaterial->AddSubset(iContainerSize - 1);
//			}
//		}
//	}
//
//	for (size_t i = 0; i < iSize; ++i)
//	{
//		// 컴포넌트 비교
//		if (pMeshRenderer == pComponentControl[i]->pComponent)
//		{
//			// ID값으로 해당 핸들 값을 가져오면 된다.
//			// Diffuse
//			if (pComponentControl[i]->hWnd->GetDlgCtrlID() == IDC_SPIN_MATERIAL_SUBSET_SIZE)
//			{
//				CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)pComponentControl[i]->hWnd;
//				int iSubSetSize = pMaterial->GetSubsetCount(iContainerSize - 1);
//				pSpin->SetRange(1, iSubSetSize);
//				pSpin->SetPos(iSubSetSize);
//			}
//		}
//	}
//	SAFE_RELEASE(pMaterial);
}

void CInspector::EditGameSystem()
{
	CGameObject* pObject= GET_SINGLE(CEditorArchive)->GetSelectObject();

	if (pObject)
	{
		CGameSystem* pComponent = pObject->GetComponent<CGameSystem>();

		if (pComponent)
		{
			CWnd* pComponentControl = GET_SINGLE(CEditorArchive)->GetComponentControl(pComponent, IDC_EDIT_GAMESYSTEM_SCENE_ID);
			if (pComponentControl)
			{
				CString		strData;
				uint32_t	iSceneID;

				if (pComponentControl)
				{
					CEdit* pControl = ((CEdit*)pComponentControl);
					pControl->GetWindowTextW(strData);
					wstring wstrBuffer(strData);
					string strBuffer(wstrBuffer.begin(), wstrBuffer.end());
					iSceneID = (uint32_t)atoi(strBuffer.c_str());
				}
				pComponent->SetSceneID(iSceneID);
			}
		}
		SAFE_RELEASE(pComponent);
	}

	
	DrawInspector();
}


// Object Tag Killfocus ( Tag 포커스 벗어났을때 )
void CInspector::OnEnKillfocusEditObjectTag()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString		strData;
	GetDlgItem(IDC_EDIT_OBJECT_TAG)->GetWindowTextW(strData);

	string strObjTag;
	strObjTag = CT2CA(strData);

	// 선택 중이던 오브젝트의 태그를 변경
	//m_pSelectObject->SetTag(strObjTag);
	CGameObject*	pSelectObj = GET_SINGLE(CEditorArchive)->GetSelectObject();
	if (pSelectObj)
	{
		pSelectObj->SetTag(strObjTag);
		GET_SINGLE(CEditorArchive)->GetHierarchy()->SetHierarchyItemTag(strData);
	}

	UpdateData(FALSE);
}

void CInspector::OnDeltaposSpin1(NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMUPDOWN	pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	CMeshRenderer* pMeshRenderer = (CMeshRenderer*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();
	CMaterial*	pMaterial = pMeshRenderer->GetMaterial();

	int			iContainerSize = pMaterial->GetContainerCount();
	int			iSubsetSize = 0;
	bool		bUp = false;

	CSpinButtonCtrl* pContainerSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SPIN_MATERIAL_CONTAINER_SIZE);
	if (pNMUpDown->iDelta < 0)
	{
		if(pContainerSpin->GetPos() - 1 >= 0)
			iSubsetSize = pMaterial->GetSubsetCount(pContainerSpin->GetPos() - 1);
	}
	else if (pNMUpDown->iDelta > 0)
	{
		bUp = true;
		if(pContainerSpin->GetPos() + 1 < iContainerSize)
			iSubsetSize = pMaterial->GetSubsetCount(pContainerSpin->GetPos() + 1);
	}

	
	
	CSpinButtonCtrl* pSubsetSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SPIN_MATERIAL_SUBSET_SIZE);

	if (iSubsetSize != 0)
	{
		pSubsetSpin->SetRange(0, iSubsetSize - 1);
		pSubsetSpin->SetPos(iSubsetSize - 1);
		if(bUp)
			ContainerUpdate(pContainerSpin->GetPos() + 1, iSubsetSize - 1);
		else
			ContainerUpdate(pContainerSpin->GetPos() - 1, iSubsetSize - 1);
	}
	else if (pContainerSpin->GetPos() == 0 || pContainerSpin->GetPos() + 1 == iContainerSize)
	{
		iSubsetSize = pMaterial->GetSubsetCount(pContainerSpin->GetPos());

		if (iSubsetSize > 0)
			iSubsetSize--;
		pSubsetSpin->SetRange(0, iSubsetSize);
		pSubsetSpin->SetPos(iSubsetSize);
		ContainerUpdate(pContainerSpin->GetPos(), pSubsetSpin->GetPos());
	}

	SAFE_RELEASE(pMaterial);
	*pResult = 0;
}

void CInspector::OnDeltaposSpin2(NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMUPDOWN	pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	CMeshRenderer* pMeshRenderer = (CMeshRenderer*)GET_SINGLE(CEditorArchive)->GetCurrentControlledComponent();
	CMaterial*	pMaterial = pMeshRenderer->GetMaterial();
		
	CSpinButtonCtrl* pContainerSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SPIN_MATERIAL_CONTAINER_SIZE);
	int iSubsetSize = pMaterial->GetSubsetCount(pContainerSpin->GetPos());



	CSpinButtonCtrl* pSubsetSpin = (CSpinButtonCtrl*)GET_SINGLE(CEditorArchive)->GetComponentControl(pMeshRenderer, IDC_SPIN_MATERIAL_SUBSET_SIZE);
	int			iSubset = pSubsetSpin->GetPos();

	if (pNMUpDown->iDelta < 0)
	{
		if (pSubsetSpin->GetPos() - 1 >= 0)
			iSubset = pSubsetSpin->GetPos() - 1;
	}
	else if (pNMUpDown->iDelta > 0)
	{
		if (pSubsetSpin->GetPos() + 1 < iSubsetSize)
			iSubset = pSubsetSpin->GetPos() + 1;
	}

	ContainerUpdate(pContainerSpin->GetPos(), iSubset);

	SAFE_RELEASE(pMaterial);
	*pResult = 0;
}


/*----Terrain-Funtion-----*/
void CInspector::TerrainLayOut(CComponent * pComponent)
{
	m_pTerrain = dynamic_cast<CTerrain*>(pComponent);
	float MaxHeight = m_pTerrain->GetMaxHeight();
	float MinHeight = m_pTerrain->GetMinHeight();
	CString		strData;

	//  terrain 이름
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 100;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;

	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Terrain", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 10;
	m_rComponentRect.right = m_rComponentRect.left + 70;
	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

	//HeightMap 불러오기 버튼
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 10;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 25;

	GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"Load HeightMap", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_BUTTON_HEIGHTMAP);

	//HeightMap 저장 버튼
	m_rComponentRect.left = m_rComponentRect.right + 20;
	m_rComponentRect.right = m_rComponentRect.left + 150;

	GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"Save HeightMap", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_BUTTON_SAVE_HEIGHT);

	//최대높이 이름
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 10;

	m_rComponentRect.right = 90;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;

	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Max Height", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);


	//최대높이 값
	CEdit*	pMaxHeightValue;

	m_rComponentRect.left = m_rComponentRect.right + 10;
	m_rComponentRect.right = m_rComponentRect.left + 60;

	pMaxHeightValue = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_HEIGHTMAX);

	strData.Format(_T("%f"), MaxHeight);
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pMaxHeightValue->SetWindowTextW(strData);
	//SetDlgItemInt(IDC_EDIT_HEIGHTMAX, MaxHeight);

	//Min Height
	m_rComponentRect.left = m_rComponentRect.right + 10;
	m_rComponentRect.right = m_rComponentRect.left + 75;

	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Min Height", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	//최소높이 값
	CEdit*	pMinHeightValue;

	m_rComponentRect.left = m_rComponentRect.right + 10;
	m_rComponentRect.right = m_rComponentRect.left + 60;

	pMinHeightValue = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_HEIGHTMIN);

	strData.Format(_T("%f"), MinHeight);
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pMinHeightValue->SetWindowTextW(strData);


	//디테일 이름 
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;

	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Detail Level", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	//디테일 값
	CEdit*	pDetailValue;

	m_rComponentRect.left = m_rComponentRect.right + 10;
	m_rComponentRect.right = m_rComponentRect.left + 60;

	float Detail = (int)m_pTerrain->GetDetailLevel();

	pDetailValue = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_DETAIL);

	strData.Format(_T("%f"), Detail);
	strData.TrimRight(L"0");
	strData.TrimRight(L".");
	pDetailValue->SetWindowTextW(strData);

	//Normal Update Button 
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 140;
	m_rComponentRect.bottom = m_rComponentRect.top + 25;

	GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"Normal Update", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_BUTTON_TERRAIN_NORMAL);

	//NavigationMesh 유무 이름 
	m_rComponentRect.left = m_rComponentRect.right + 30;
	m_rComponentRect.right = m_rComponentRect.left + 140;

	CButton*	pNaviMeshButton = (CButton*)GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"Navigation Mesh", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		m_rComponentRect, IDC_BUTTON_NAVIGATION);

	if (m_bNaviMesh)
		pNaviMeshButton->SetCheck(1);
	else
		pNaviMeshButton->SetCheck(0);

}

void CInspector::TerrainEditorLayout(CComponent* pComponent)
{
	m_pTerrainEditor = dynamic_cast<CTerrainEditor*>(pComponent);

	CString strData;

	//  terrain Editor 이름
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 15;

	m_rComponentRect.right = 150;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;

	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"TerrainEditor", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 70;

	GET_SINGLE(CEditorArchive)->CreateButton(pComponent, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_EDIT_COMPONENT_ERASE_BUTTON);

	//Brush 이름
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 130;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;

	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Brush        Radius", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	//브러쉬 반지름 나오는 곳
	CEdit*	pTerrainHeightValue;

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 40;

	pTerrainHeightValue = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_BRUSH);

	strData.Format(_T("%f"), m_pTerrainEditor->GetBrushRadius());
	strData.TrimRight(L"0");
	strData.TrimRight(L".");

	pTerrainHeightValue->SetWindowTextW(strData);

	//Splat 라디오 버튼
	m_rComponentRect.left = m_rComponentRect.right + 30;
	m_rComponentRect.right = m_rComponentRect.left + 60;

	GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"Splat", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		m_rComponentRect, IDC_RADIO_BUTTON_SPLATE);

	//Height 라디오 버튼
	m_rComponentRect.left = m_rComponentRect.right + 20;
	m_rComponentRect.right = m_rComponentRect.left + 60;

	GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"Height", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		m_rComponentRect, IDC_RADIO_BUTTON_HEIGHT);

	//None 라디오 버튼
	m_rComponentRect.left = m_rComponentRect.right + 20;
	m_rComponentRect.right = m_rComponentRect.left + 60;

	GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"None", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		m_rComponentRect, IDC_RADIO_BUTTON_NONE);

	//CurrentSplateCount
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 130;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;

	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"CurrentSplateCount", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	//CurrentSplate Edit
	CEdit*	pCurrentSplateEdit;

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 40;

	pCurrentSplateEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_CURRENT_SPLATE);

	float CurrentSplateCount = (int)m_pTerrainEditor->GetCurrentSplate();

	strData.Format(_T("%f"), CurrentSplateCount);
	strData.TrimRight(L"0");
	strData.TrimRight(L".");

	pCurrentSplateEdit->SetWindowTextW(strData);
	
	//알파bmp 저장 버튼
	m_rComponentRect.left = m_rComponentRect.right + 30;
	m_rComponentRect.right = m_rComponentRect.left + 90;

	GET_SINGLE(CEditorArchive)->CreateButton(m_pSelectObject, L"Save Alpha", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_rComponentRect, IDC_BUTTON_SAVE_ALPHA);

	//Brush Power
	m_rComponentRect.left = 5;
	m_rComponentRect.top = m_rComponentRect.bottom + 5;

	m_rComponentRect.right = 130;
	m_rComponentRect.bottom = m_rComponentRect.top + 20;

	GET_SINGLE(CEditorArchive)->CreateStatic(pComponent, L"Brush Power", WS_CHILD | WS_VISIBLE | SS_LEFT,
		m_rComponentRect);

	m_rComponentRect.left = m_rComponentRect.right + 5;
	m_rComponentRect.right = m_rComponentRect.left + 40;

	pCurrentSplateEdit = (CEdit*)GET_SINGLE(CEditorArchive)->CreateEdit(m_pSelectObject, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		ES_AUTOHSCROLL | SS_LEFT, m_rComponentRect, IDC_EDIT_BRUSH_POWER);

	float BrushPower = m_pTerrainEditor->GetBrushPower();

	strData.Format(_T("%f"), BrushPower);

	pCurrentSplateEdit->SetWindowTextW(strData);

}

void CInspector::EditMaxHeight()
{
	CString		strData;
	GetDlgItem(IDC_EDIT_HEIGHTMAX)->GetWindowTextW(strData);
	float MaxHeight = (float)atof((CStringA)strData);

	m_pTerrain->SetMaxHeight(MaxHeight);
}

void CInspector::EditMinHeight()
{
	CString		strData;
	GetDlgItem(IDC_EDIT_HEIGHTMIN)->GetWindowTextW(strData);
	float MinHeight = (float)atof((CStringA)strData);

	m_pTerrain->SetMinHeight(MinHeight);
}

void CInspector::EditDetailLevel()
{
	CString		strData;
	GetDlgItem(IDC_EDIT_DETAIL)->GetWindowTextW(strData);
	float Detail = (float)atof((CStringA)strData);

	m_pTerrain->SetDetailLevel(Detail);
}

void CInspector::EditBrushRadius()
{
	CString		strData;
	GetDlgItem(IDC_EDIT_BRUSH)->GetWindowTextW(strData);
	float Radius = (float)atof((CStringA)strData);

	m_pTerrainEditor->SetBrushRadius(Radius);

}

void CInspector::EditBrushPower()
{
	CString		strData;
	GetDlgItem(IDC_EDIT_BRUSH_POWER)->GetWindowTextW(strData);
	float Radius = (float)atof((CStringA)strData);

	m_pTerrainEditor->SetBrushPower(Radius);
}

void CInspector::ClickSplate()
{
	m_pTerrainEditor->SetBrushOn(false);
}

void CInspector::ClickHeight()
{
	m_pTerrainEditor->SetBrushOn(true);
}

void CInspector::ClickNone()
{
	m_pTerrainEditor->SetBrushOn(true,true);
}

void CInspector::LoadHeightMap()
{
	CFileDialog	dlg(TRUE, L"bmp", L"Default", OFN_HIDEREADONLY,
		L"BMP(*.*)|*.*|AllFiles(*.*)|*.*||");

	if (dlg.DoModal() == IDOK)
	{
		CString	sPath = dlg.GetFileTitle();
		CString	strPath = dlg.GetPathName();

		char	strPath1[MAX_PATH] = {};
		sprintf(strPath1, "%S", sPath);

		char	strPath2[MAX_PATH] = {};
		sprintf(strPath2, "%S", strPath);

		m_pTerrain->LoadHeightMap(strPath1, (float)m_pTerrain->GetDetailLevel(), strPath2, NO_USE_PATH);

		Vector2 vSize = m_pTerrain->GetTerrainSize();

		CGameObject* pObject = m_pTerrain->GetGameObject();

		CBoxCollider* pCollider = pObject->GetComponent<CBoxCollider>();
		if (pCollider)
		{
			pCollider->SetVolume(Vector3(vSize.x, 0.01f, vSize.y));
			pCollider->SetLocalPosition(0.0f, 0.0f, 0.0f);
			SAFE_RELEASE(pCollider);
		}
		SAFE_RELEASE(pObject);
	}
}

void CInspector::SaveHeightMapForBMP()
{
	CString		strData;
	GetDlgItem(IDC_BUTTON_SAVE_HEIGHT)->GetWindowTextW(strData);

	float CurrentSplate = (float)atof((CStringA)strData);

	int SplateMaxCount = m_pTerrain->GetSplatCount();

	if (CurrentSplate < SplateMaxCount && SplateMaxCount > 0 && CurrentSplate >= 0)
	{
		CFileDialog	dlg(FALSE, L"bmp", L"Default", OFN_HIDEREADONLY,
			L"BMP(*.*)|*.*|AllFiles(*.*)|*.*||");

		if (dlg.DoModal() == IDOK)
		{
			wstring	strPath = dlg.GetPathName();

			m_pTerrain->SaveHeightMapForBMP(strPath.c_str());
		}
	}
}

void CInspector::EditCurrentSplate()
{
	CString		strData;
	GetDlgItem(IDC_EDIT_CURRENT_SPLATE)->GetWindowTextW(strData);

	float CurrentSplate = (float)atof((CStringA)strData);

	int SplateMaxCount = m_pTerrain->GetSplatCount();

	if (CurrentSplate < SplateMaxCount && CurrentSplate >= 0)
		m_pTerrainEditor->SetCurrentSplate(CurrentSplate);
	else
		m_pTerrainEditor->SetCurrentSplate(0);
}

void CInspector::SaveAlphaTexture()
{
	CString		strData;
	GetDlgItem(IDC_EDIT_CURRENT_SPLATE)->GetWindowTextW(strData);

	float CurrentSplate = (float)atof((CStringA)strData);

	int SplateMaxCount = m_pTerrain->GetSplatCount();

	if (CurrentSplate < SplateMaxCount && SplateMaxCount > 0 && CurrentSplate >= 0)
	{
		CFileDialog	dlg(FALSE, L"bmp", L"Default", OFN_HIDEREADONLY,
			L"BMP(*.*)|*.*|AllFiles(*.*)|*.*||");

		if (dlg.DoModal() == IDOK)
		{
			wstring	strPath = dlg.GetPathName();

			m_pTerrain->SaveAlphaTexture(CurrentSplate, strPath.c_str());
		}
	}
}


void CInspector::SetNavigationCheckBox()
{
	m_bNaviMesh = !m_bNaviMesh;

	m_pTerrain->SetNavigationMesh(m_bNaviMesh);
}

void CInspector::UpdateNormal()
{
	m_pTerrain->UpdateNormal();

	return;
}

void CInspector::SaveTerrain()
{
	CFileDialog	dlg(TRUE, L"Terrain", L"Default", OFN_HIDEREADONLY,
		L"TERRAIN(*.*)|*.*|AllFiles(*.*)|*.*||");

	if (dlg.DoModal() == IDOK)
	{
		wstring	sPath = L"Terrain\\"; // Terrain에 저장할 터레인.Terrain 파일명
		wstring	strPath = dlg.GetFileName();
		sPath += strPath;

		wstring sPath2 = L"Terrains\\";
		wstring	strMeshName = dlg.GetFileTitle();
		wstring strPath2 = L".SSSMesh";
		sPath2 += strMeshName;
		sPath2 += strPath2;

		m_pTerrain->SaveTerrainData(sPath.c_str(), sPath2.c_str());
	}
}

void CInspector::LoadTerrain()
{
	CFileDialog	dlg(TRUE, L"Terrain", L"Default", OFN_HIDEREADONLY,
		L"TERRAIN(*.*)|*.*|AllFiles(*.*)|*.*||");

	if (dlg.DoModal() == IDOK)
	{
		wstring	sPath = L"Terrains\\"; // Terrain에 저장할 터레인.Terrain 파일명
		wstring	strPath = dlg.GetFileName();
		sPath += strPath;

		wstring sPath2 = L"terrains\\";
		wstring	strMeshName = dlg.GetFileTitle();
		wstring strPath2 = L".SSSMesh";
		sPath2 += strMeshName;
		sPath2 += strPath2;

		m_pTerrain->LoadTerrainData(sPath.c_str(), sPath2.c_str());
	}
}

//UI
void CInspector::LoadUITexture()
{
	CFileDialog	dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY,
		L"(*.BMP, *.PNG) | *.BMP;*.PNG; | All Files(*.*)|*.*||");

	if (dlg.DoModal() == IDOK)
	{
		CString	strPath = dlg.GetFileName();
		string strFileName;

		strFileName = strtok((CT2CA)strPath, ".");

		//포멧 검사
		wchar_t	strExt[_MAX_EXT] = {};
		_wsplitpath_s(strPath, NULL, 0, NULL, 0, NULL, 0, strExt, _MAX_EXT);
		//문자열 형변환
		char	strExt1[_MAX_EXT] = {};
		WideCharToMultiByte(CP_ACP, 0, strExt, -1, strExt1, lstrlenW(strExt), 0, 0);
		_strupr_s(strExt1);

		if (strcmp(strExt1, ".PNG") == 0 || strcmp(strExt1, ".BMP") == 0)
		{
			CSpriteRenderer* pRenderer = m_pSelectObject->GetComponent<CSpriteRenderer>();
			if (pRenderer == nullptr)
			{
				assert(false);
				return;
			}
			CMaterial* pMaterial = pRenderer->GetMaterial();
			pMaterial->SetSampler(SAMPLER_LINEAR);

			//파일의 이름을 키 값으로(확장자 제외) 파일을 가져온다.
			pMaterial->SetDiffuseTexture(strFileName, strPath);
			pMaterial->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

			SAFE_RELEASE(pRenderer);
			SAFE_RELEASE(pMaterial);
		}
	}
	else
		return;
}

void CInspector::AddRadioButton()
{
	CUIRadioButton* pButton = m_pSelectObject->GetComponent<CUIRadioButton>();

	pButton->AddButton();

	SAFE_RELEASE(pButton);
}

void CInspector::LoadAlphaMaskTexture()
{
	//CFileDialog	dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY,
	//	L"(*.PNG) | *.PNG; | All Files(*.*)|*.*||");
	//
	//if (dlg.DoModal() == IDOK)
	//{
	//	CString	strPath = dlg.GetFileName();
	//	string strFileName;
	//
	//	strFileName = strtok((CT2CA)strPath, ".");
	//
	//	//포멧 검사
	//	wchar_t	strExt[_MAX_EXT] = {};
	//	_wsplitpath_s(strPath, NULL, 0, NULL, 0, NULL, 0, strExt, _MAX_EXT);
	//	//문자열 형변환
	//	char	strExt1[_MAX_EXT] = {};
	//	WideCharToMultiByte(CP_ACP, 0, strExt, -1, strExt1, lstrlenW(strExt), 0, 0);
	//	_strupr_s(strExt1);
	//
	//	if (strcmp(strExt1, ".PNG") == 0)
	//	{
	//		CAlphaMask* pMask = m_pSelectObject->GetComponent<CAlphaMask>();
	//		//CSpriteRenderer* pRenderer = m_pSelectObject->AddComponent<CSpriteRenderer>("UIRenderer");
	//
	//		pMask->SetMaskTexture(strFileName, strPath);
	//		//파일의 이름을 키 값으로(확장자 제외) 파일을 가져온다.
	//
	//		SAFE_RELEASE(pMask);
	//
	//	}
	//}
	//else
	//	return;
}

// Layer ComboBox SelChange Event
void CInspector::OnCbnSelchangeCombo1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int		iBoxSelCurSel = m_LayerCombo.GetCurSel();
	CString SelLayer;
	m_LayerCombo.GetLBText(iBoxSelCurSel, SelLayer);
	string strSelLayer = CT2CA(SelLayer);

	CGameObject*	pSelectObject = GET_SINGLE(CEditorArchive)->GetSelectObject();
	if (pSelectObject)
	{
		CScene*	pScene = pSelectObject->GetScene();
		CLayer*	pSelLayer;
		vector<CLayer*>*	pvecLayer = pScene->GetLayers();
		vector<CLayer*>::iterator	iter;
		vector<CLayer*>::iterator	iterEnd = pvecLayer->end();
		for (iter = pvecLayer->begin(); iter != iterEnd;)
		{
			if ((*iter)->GetTag() == strSelLayer)
			{
				pSelLayer = (*iter);
				pSelectObject->SetLayer(pSelLayer);
				break;
			}
		}
	}
}

void CInspector::SetLocalTransformfromMesh()
{
	CGameObject* pSelectedGameObject = GET_SINGLE(CEditorArchive)->GetSelectObject();

	if (pSelectedGameObject)
	{
		CMeshRenderer* pRenderer = pSelectedGameObject->GetComponent<CMeshRenderer>();
		if (pRenderer)
		{
			CMesh* pMesh = pRenderer->GetMesh();
			if (pMesh)
			{
				pRenderer->SetLocalTransformFromMesh();
			}
		}
		SAFE_RELEASE(pRenderer);
	}
}

void CInspector::OnBnClickedStaticCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CGameObject*	pSelectObj = GET_SINGLE(CEditorArchive)->GetSelectObject();
	if (pSelectObj)
	{
		if (m_StaticCheck.GetCheck())
		{
			pSelectObj->SetStatic(true);
		}
		else
		{
			pSelectObj->SetStatic(false);
		}
	}
}
