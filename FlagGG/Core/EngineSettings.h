//
// 引擎设置
//

#pragma once

#include "Export.h"
#include "Core/Subsystem.h"
#include "GfxDevice/GfxDeviceDefines.h"

namespace FlagGG
{

struct FlagGG_API EngineSettings : public Subsystem<EngineSettings>
{
	RendererType rendererType_{ RENDERER_TYPE_D3D11 };
	bool clusterLightEnabled_{ false };
	bool renderAO_{ true };
};

}
