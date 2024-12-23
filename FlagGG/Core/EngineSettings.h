//
// 引擎设置
//

#pragma once

#include "Core/Subsystem.h"
#include "GfxDevice/GfxDeviceDefines.h"

namespace FlagGG
{

struct EngineSettings : public Subsystem<EngineSettings>
{
	RendererType rendererType_{ RENDERER_TYPE_D3D11 };
};

}
