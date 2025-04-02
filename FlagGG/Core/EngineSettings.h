//
// 引擎设置
//

#pragma once

#include "Export.h"
#include "Core/Subsystem.h"
#include "GfxDevice/GfxDeviceDefines.h"

namespace FlagGG
{

enum class ClusterLightType
{
	None = 0,
	ClusterLight,
	ClusterLightApprox,
};

enum class AmbientOcclusionType
{
	None = 0,
	Hardware,
	Software,
};

enum class AmbientOcclusionQuality
{
	VERY_LOW = 0,
	LOW,
	MEDIUM,
	HIGH,
	VERY_HIGH,
};

enum class OcclusionCullingType
{
	None = 0,
	CpuCulling,
	HiZCulling,
};

enum class ScreenSpaceReflectionsType
{
	None = 0,
	SSPR,      // 屏幕空间平面反射
	SSR,       // 屏幕空间反射
};

struct FlagGG_API EngineSettings : public Subsystem<EngineSettings>
{
	RendererType rendererType_{ RENDERER_TYPE_D3D11 };
	ClusterLightType clusterLightType_{ ClusterLightType::None };
	AmbientOcclusionType aoType_{ AmbientOcclusionType::Software };
	AmbientOcclusionQuality AOQuality_{ AmbientOcclusionQuality::MEDIUM };
	OcclusionCullingType occlusionCullingType_{ OcclusionCullingType::HiZCulling };
	ScreenSpaceReflectionsType screenSpaceReflectionsType_{ ScreenSpaceReflectionsType::None };
};

}
