#include "GfxDevice.h"
#include "D3D11/GfxDeviceD3D11.h"
#include "OpenGL/GfxDeviceOGL.h"
#include "FileSystem/FileSystem.h"
#include "Utility/SystemHelper.h"

namespace FlagGG
{

static GfxDevice* CreateOrGetDevice()
{
	struct GfxDeviceInit 
	{
		GfxDeviceInit()
		{
			const String rootDir = GetProgramDir();
			if (GetLocalFileSystem()->FileExists(rootDir + "/gl.txt"))
				device_ = GetSubsystem<GfxDeviceOpenGL>();
			else
				device_ = GetSubsystem<GfxDeviceD3D11>();
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
