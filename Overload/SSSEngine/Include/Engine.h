#pragma once

#include <Windows.h>
#include <string>
#include <string.h>
#include <list>
#include <vector>
#include <unordered_map>
#include <map>
#include <assert.h>
#include <crtdbg.h>
#include <conio.h>
#include <time.h>
#include <random>
#include <functional>
#include <algorithm>
#include <queue>
#include <process.h>
#include <thread>
#include <mutex>

//Font
#include <d2d1.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "imm32")

//Input
//#include <dinput.h>
//#pragma comment(lib, "dinput8.lib")
//#pragma comment(lib, "dxguid.lib")

using namespace std;

#include "Types.h"

SSS_BEGIN

#define PREFAB_PLAYER_RELATIVE_PATH									L"Release\\Prefab\\Player.SSSPrefab"
#define PREFAB_CAMERA_GIMBAL_RELATIVE_PATH						L"Release\\Prefab\\CameraGimbal.SSSPrefab"

#define SCENE_TERNY_DAY_RELATIVE_PATH								L"Release\\Scene\\TernyDay.SSSScene"
#define PREFAB_TERRAIN_TERNY_DAY_RELATIVE_PATH				L"Release\\Prefab\\TerrainTerny.SSSPrefab"

#define SCENE_TERNY_NIGHT_RELATIVE_PATH							L"Release\\Scene\\TernyNight.SSSScene"
#define PREFAB_TERRAIN_TERNY_NIGHT_RELATIVE_PATH			L"Release\\Prefab\\TerrainTerny.SSSPrefab"

#define SCENE_KINGSROAD_DAY_RELATIVE_PATH						L"Release\\Scene\\KingsRoadDay.SSSScene"
#define PREFAB_TERRAIN_KINGSROAD_DAY_RELATIVE_PATH		L"Release\\Prefab\\TerrainKingsRoad.SSSPrefab"

#define SCENE_KINGSROAD_NIGHT_RELATIVE_PATH					L"Release\\Scene\\KingsRoadNight.SSSScene"
#define PREFAB_TERRAIN_KINGSROAD_NIGHT_RELATIVE_PATH	L"Release\\Prefab\\TerrainKingsRoad.SSSPrefab"

#define SCENE_BATTLEFIELD_DAY_RELATIVE_PATH						L"Release\\Scene\\BattleField.SSSScene"
#define PREFAB_TERRAIN_BATTLEFIELD_DAY_RELATIVE_PATH		L"Release\\Prefab\\TerrainBattleField.SSSPrefab"



#define PARTICLE_MAX 1024
#define HEALTHBAR_MAX 512


// Path Key
#define NO_USE_PATH		"NoPath"
#define	ROOT_PATH			"RootPath"
#define	SHADER_PATH		"ShaderPath"
#define	TEXTURE_PATH		"TexturePath"
#define MESH_PATH			"MeshPath"
#define DATA_PATH			"DataPath"
#define SOUND_PATH			"SoundPath"

// Shader Key
#define UI_RENDERTARGET_SHADER								"RenderTargetShader"
#define	STANDARD_COLOR_SHADER								"StandardColorShader"
#define	STANDARD_TEXTURE_SHADER							"StandardTexShader"
#define COLORED_TEXTURE_SHADER								"ColoredTexShader"
#define	COLLIDER_SHADER											"ColliderShader"
#define UI_TEXTURE_SHADER										"UIShader"
#define UI_COLLIDER_SHADER										"UIColliderShader"
#define UI_GRID_SHADER												"UIGridShader"
#define	STANDARD_COLOR_NORMAL_SHADER					"StandardColorNormalShader"
#define	STANDARD_TEXTURE_NORMAL_SHADER				"StandardTextureNormalShader"
#define STANDARD_BUMP_SHADER									"StandardBumpShader"
#define STANDARD_BUMP_ANIM_SHADER						"StandardBumpAnimShader"
#define STANDARD_TEXTURE_NORMAL_ANIM_SHADER	"StandardTextureNormalAnimShader"

#define	SKYBOX_SHADER			"SkyboxShader"
#define RIM_LIGHT_SHADER		"RimLightShader"
#define OUTLINE_SHADER			"OutlineShader"
#define TERRAIN_SHADER			"TerrainShader"
#define BILLBOARD_SHADER		"BillboardShader"

#define ALPHAMASK_SHADER		"AlphaMaskShader"
#define SLIDER_SHADER			"SliderShader"
#define SPAWNINGPOOL_SHADER "SpawningPoolShader"
#define HEALTHBAR_SHADER "HealthBarShader"
#define LOADING_SHADER		"LoadingShader"


// InputLayout Key	
#define	POS_COLOR_LAYOUT								"PosColorLayout"
#define	POS_COLOR_NORMAL_LAYOUT				"PosColorNormalLayout"
#define	POS_UV_LAYOUT									"PosUVLayout"
#define	POS_UV_NORMAL_LAYOUT						"PosUVNormalLayout"
#define	POS_LAYOUT											"PosLayout"
#define	BUMP_LAYOUT										"BumpLayout"
#define BUMP_ANIM_LAYOUT								"BumpAnimLayout"
#define POS_TEXTURE_NORMAL_ANIM_LAYOUT	"PosTextureNormalAnimLayout"
#define PARTICLE_LAYOUT									"ParticleLayout"

//Test Layoyt
#define POS_TEX_SIZE					"PosTexSize"

//Constant Buffer Key
#define TRANSFORM_CBUFFER		"TransformConstantBuffer"
#define	LIGHT_CBUFFER					"LightCBuffer"
#define COLLIDER_CBUFFER			"ColliderConstantBuffer"
#define	MATERIAL_CBUFFER			"MaterialConstantBuffer"
#define	ANIMATION2D_CBUFFER		"Animation2DCBuffer"
#define	GRID_CBUFFER					"GridCBuffer"
#define POSTPROCESS_CBUFFER	"PostProcessCBuffer"
#define BILLBOARD_CBUFFER			"BillboardCBuffer"
#define Effect_CBUFFER			"EffectCBuffer"
#define FX_OUTLINE_CBUFFER		"OutlineCBuffer"
#define SPAWNINGPOOL_CBUFFER	"SpawningPoolCBuffer"
#define HEALTHBAR_CBUFFER			"HealthBarCBuffer"
#define LOADING_CBUFFER			"LoadingCBuffer"

// Sampler State Key
#define	SAMPLER_LINEAR			"SamplerLinear"
#define SAMPLER_POINT			"SamplerPoint"
#define SAMPLER_POINT_LOOP	"SamplerPointLoop"

// Default Engine Mesh Key
#define ENGINE_MESH_CUBE_COLOR				"EngineCubeColorMesh"
#define	ENGINE_MESH_TRIANGLE_COLOR		"EngineTriangleColorMesh"
#define	ENGINE_MESH_RECTANGLE_COLOR		"EngineRectColorMesh"
#define	ENGINE_MESH_CIRCLE_COLOR			"EngineStripRectColorMesh"

#define	ENGINE_MESH_ORTHO_TRIANGLE_COLOR		"EngineOrthoTriangleColorMesh"
#define	ENGINE_MESH_ORTHO_RECTANGLE_COLOR	"EngineOrthoRectColorMesh"
#define	ENGINE_MESH_ORTHO_CIRCLE_COLOR			"EngineOrthoStripRectColorMesh"

#define	ENGINE_MESH_ORTHORECT_TEX	"EngineOrthoRectTexMesh"
#define	ENGINE_MESH_RECTANGLE_TEX	"EngineRectTexMesh"
#define	ENGINE_MESH_PYRAMID_COLOR_NORMAL	"EnginePyramidColorNormal"
#define ENGINE_MESH_BILLBOARD "EngineMeshBillboard"
#define	ENGINE_MESH_FRAME_ISO_COLOR			"EngineFrameIsoColorMesh"
#define ENGINE_MESH_PARTICLE "EngineMeshParticle"
#define ENGINE_MESH_HEALTHBAR "EngineMeshHealthBar"

#define PRIMITIVE_MESH_CIRCLE "PrimitiveMeshCircle"
#define PRIMITIVE_MESH_SPHERE "PrimitiveMeshSphere"
#define PRIMITIVE_MESH_BOX "PrimitiveMeshBox"
#define PRIMITIVE_MESH_CUBE "PrimitiveMeshCube"
#define PRIMITIVE_MESH_PYRAMID "PrimitiveMeshPyramid"
#define PRIMITIVE_MESH_CYLINDER "PrimitiveMeshCylinder"
#define PRIMITIVE_MESH_CAPSULE "PrimitiveMeshCapsule"
#define PRIMITIVE_MESH_DOUGHNUT "PrimitiveMeshDoughnut"
#define MESH_SKYBOX	"MeshSkybox"
#define GIZMO_MESH_ARROW_X "GizmoMeshArrowX"
#define GIZMO_MESH_ARROW_Y "GizmoMeshArrowY"
#define GIZMO_MESH_ARROW_Z "GizmoMeshArrowZ"
#define GIZMO_MESH_DISK "GizmoMeshDisk"

//Debug Mesh

#define ENGINE_MESH_DEBUG_RAY	"EngineDebugRayMesh"
#define ENGINE_MESH_FRAME_RECT_COLOR		"EngineFrameRectColorMesh"
#define ENGINE_MESH_FRAME_CIRCLE_COLOR		"EngineFrameCircleColorMesh"


// Render State Key
#define	ALPHA_BLEND		"AlphaBlend"
#define	ALPHA_BLEND_DISABLE		"AlphaBlendDisable"
#define	DEPTH_DISABLE		"DepthDisable"
#define	DEPTH_ENABLE		"DepthEnable"
#define	DEPTH_SKYBOX		"DepthSkybox"
#define	CULLNONE				"CullNone"
#define	CULLBACK				"CullBack"
#define	CULLFRONT			"CullFront"
#define	WIREFRAME			"WireFrame"



//Extra Input
#define WHEEL_SCROLL_UP		300
#define WHEEL_SCROLL_DOWN	301


//void WideCharacterToMultibyte(const wchar_t* pString, char* __out pOutput)
//{
//	WideCharToMultiByte(CP_ACP, 0, pString, -1, pOutput, lstrlenW(pString), NULL, NULL);
//}
//
//void MultiByteToWideCharacter(const char* pString, wchar_t* __out pOutput)
//{
//	int strLength = strlen(pString);
//	MultiByteToWideChar(CP_ACP, 0, pString, -1, pOutput, strLength);
//}

template<typename is, typename to>
static bool Convertible()
{
	return is_convertible <is, to>()::value;
}

static float Lerp(float fBegin, float fEnd, float fRate)
{
	return (1 - fRate) * fBegin + fRate * fEnd;	
}
static float Slerp(float fBegin, float fEnd, float fRate)
{
	
}


static float RandomBetweenZeroToOne()
{
	int i = rand() % 10001;
	return (float)i * 0.0001f;
}

static float RandomRange(float fMin, float fMax)
{
	float fRange = fMax - fMin;
	float fValue = RandomBetweenZeroToOne() * fRange;
	fValue += fMin;

	return fValue;
}




template <typename T>
void Safe_Delete_VecList(T& p)
{
	T::iterator	iter;
	T::iterator	iterEnd = p.end();

	for (iter = p.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE((*iter));
	}

	p.clear();
}

template <typename T>
void Safe_Delete_Map(T& p)
{
	T::iterator	iter;
	T::iterator	iterEnd = p.end();

	for (iter = p.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}

	p.clear();
}

template <typename T>
void Safe_Release_VecList(T& p)
{
	T::iterator	iter;
	T::iterator	iterEnd = p.end();

	for (iter = p.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE((*iter));
	}

	p.clear();
}

template <typename T>
void Safe_Release_Map(T& p)
{
	T::iterator	iter;
	T::iterator	iterEnd = p.end();

	for (iter = p.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE(iter->second);
	}

	p.clear();
}

typedef struct _tagResolution
{
	unsigned int	iWidth;
	unsigned int	iHeight;

	_tagResolution() :
		iWidth(0),
		iHeight(0)
	{
	}

	_tagResolution(const _tagResolution& res)
	{
		*this = res;
	}

	_tagResolution(unsigned int iW, unsigned int iH) :
		iWidth(iW),
		iHeight(iH)
	{
	}
}RESOLUTION, *PRESOLUTION;


SSS_END
