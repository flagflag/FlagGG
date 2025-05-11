#include "GfxDevice.h"
#if FLAGGG_D3D11
#include "D3D11/GfxDeviceD3D11.h"
#endif
#if FLAGGG_OPENGL
#include "OpenGL/GfxDeviceOGL.h"
#endif
#if FLAGGG_METAL
#include "Metal/GfxDeviceMetal.h"
#endif
#if FLAGGG_VULKAN
#include "Vulkan/GfxDeviceVulkan.h"
#endif
#include "FileSystem/FileSystem.h"
#include "Utility/SystemHelper.h"
#include "Core/EngineSettings.h"

namespace FlagGG
{

static GfxDevice* CreateOrGetDevice()
{
	struct GfxDeviceInit 
	{
		GfxDeviceInit()
		{
			const String rootDir = GetProgramDir();
			switch (GetSubsystem<EngineSettings>()->rendererType_)
			{
#if FLAGGG_D3D11
			case RENDERER_TYPE_D3D11:
				device_ = GetSubsystem<GfxDeviceD3D11>();
				break;
#endif

#if FLAGGG_OPENGL
			case RENDERER_TYPE_OPENGL:
				device_ = GetSubsystem<GfxDeviceOpenGL>();
				break;
#endif

#if FLAGGG_METAL
			case RENDERER_TYPE_METAL:
				device_ = GetSubsystem<GfxDeviceMetal>();
				break;
#endif

#if FLAGGG_VULKAN
			case RENDERER_TYPE_VULKAN:
				device_ = GetSubsystem<GfxDeviceVulkan>();
				break;
#endif

			default:
				throw "Invalid renderer type.";
				break;
			}
		}

		GfxDevice* device_;
	};

	GfxDeviceInit _;
	return _.device_;
}

GfxDevice* GfxDevice::CreateDevice()
{
	return CreateOrGetDevice();
}

void GfxDevice::DestroyDevice()
{

}

GfxDevice* GfxDevice::GetDevice()
{
	return CreateOrGetDevice();
}

}
