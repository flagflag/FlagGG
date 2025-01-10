#include <Config/LJSONFile.h>
#include <Log.h>
#include <FileSystem/FileManager.h>
#include <FileSystem/FileSystemArchive/DefaultFileSystemArchive.h>
#include <Resource/ResourceCache.h>
#include <Utility/SystemHelper.h>
#include <Graphics/Shader.h>
#include <GfxDevice/GfxDevice.h>
#include <GfxDevice/GfxProgram.h>
#include <Core/EngineSettings.h>
#include <Memory/MemoryHook.h>

using namespace FlagGG;

void UniformTest()
{
	auto* cache = GetSubsystem<ResourceCache>();
	ShaderCode* shaderCode = cache->GetResource<ShaderCode>("Shader/UnitTest/UniformTest.hlsl");

	auto* vsShader = shaderCode->GetShader(VS, {});
	auto* psShader = shaderCode->GetShader(PS, {});

	SharedPtr<GfxProgram> program(GfxDevice::GetDevice()->CreateProgram());
	program->Link(vsShader->GetGfxRef(), psShader->GetGfxRef());
}

void GfxBufferTest()
{
	auto* device = GfxDevice::GetDevice();

	{
		PODVector<Vector4> vec4Buffer(4);
		SharedPtr<GfxBuffer> buffer(device->CreateBuffer());
		buffer->SetStride(sizeof(Vector4));
		buffer->SetSize(sizeof(Vector4) * 4);
		buffer->SetBind(BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
		buffer->SetAccess(BUFFER_ACCESS_NONE);
		buffer->SetUsage(BUFFER_USAGE_STATIC);
		buffer->Apply(vec4Buffer.Buffer());
	}

	{
		PODVector<Vector4> vec4Buffer(4);
		SharedPtr<GfxBuffer> buffer(device->CreateBuffer());
		buffer->SetStride(sizeof(Vector4));
		buffer->SetSize(sizeof(Vector4) * 4);
		buffer->SetBind(BUFFER_BIND_COMPUTE_READ | BUFFER_BIND_COMPUTE_WRITE);
		buffer->SetAccess(BUFFER_ACCESS_WRITE);
		buffer->SetUsage(BUFFER_USAGE_STATIC);
		buffer->Apply(vec4Buffer.Buffer());
	}
}

void GfxTextureTest()
{
	auto* device = GfxDevice::GetDevice();

	SharedPtr<GfxTexture> texture(device->CreateTexture());
	texture->SetWidth(128);
	texture->SetHeight(128);
}

int main()
{
	//
	GetSubsystem<EngineSettings>()->rendererType_ = RENDERER_TYPE_VULKAN;

	GetSubsystem<AssetFileManager>()->AddArchive(new DefaultFileSystemArchive(GetLocalFileSystem(), GetProgramDir() + "Res"));

	UniformTest();

	GfxBufferTest();

	GfxTextureTest();
	
	return 0;
}

IMPLEMENT_MODULE_USD("UniformTest");
