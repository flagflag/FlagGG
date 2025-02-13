#include <Config/LJSONFile.h>
#include <Log.h>
#include <FileSystem/FileManager.h>
#include <FileSystem/FileSystemArchive/DefaultFileSystemArchive.h>
#include <Resource/ResourceCache.h>
#include <Utility/SystemHelper.h>
#include <Graphics/Shader.h>
#include <Graphics/Texture2D.h>
#include <Graphics/ShaderHelper.h>
#include <GfxDevice/GfxDevice.h>
#include <GfxDevice/GfxProgram.h>
#include <Core/EngineSettings.h>
#include <Resource/ResourceCache.h>
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
	auto* cache = GetSubsystem<ResourceCache>();
	cache->GetResource<Texture2D>("Textures/WaterNoise.dds");
	cache->GetResource<Texture2D>("tiles/Textures/Mix/BaseColor_Terrain.png");
	cache->GetResource<Texture2D>("tiles/Textures/Mix/MixColor_Terrain.png");
	cache->GetResource<Texture2D>("Editor/TextureStyleGroup/noise03.tga");
	cache->GetResource<Texture2D>("Editor/TextureStyleGroup/noise04.tga");
	cache->GetResource<Texture2D>("Textures/WaterNormal.png");
	cache->GetResource<Texture2D>("Textures/WaterReflection.dds");
}

void ShaderCompileTest()
{
	SharedPtr<Shader> shader;

	INIT_SHADER_VARIATION(shader, "Shader/SSAO/ScreenSpaceAmbientOcclusionVS.hlsl", VS, Vector<String>({}));
	for (UInt32 i = 0; i <= 4; ++i)
	{
		INIT_SHADER_VARIATION(shader, "Shader/SSAO/ScreenSpaceAmbientOcclusionSetupPS.hlsl", PS, Vector<String>({ ToString("SHADER_QUALITY=%d", i) }));
		INIT_SHADER_VARIATION(shader, "Shader/SSAO/ScreenSpaceAmbientOcclusionMainPS.hlsl", PS, Vector<String>({ ToString("SHADER_QUALITY=%d", i), "USE_AO_SETUP_AS_INPUT=1", "USE_UPSAMPLE=0" }));
		INIT_SHADER_VARIATION(shader, "Shader/SSAO/ScreenSpaceAmbientOcclusionMainPS.hlsl", PS, Vector<String>({ ToString("SHADER_QUALITY=%d", i), "USE_AO_SETUP_AS_INPUT=0", "USE_UPSAMPLE=1" }));
	}
}

int main()
{
	//
	GetSubsystem<EngineSettings>()->rendererType_ = RENDERER_TYPE_VULKAN;

	GetSubsystem<AssetFileManager>()->AddArchive(new DefaultFileSystemArchive(GetLocalFileSystem(), GetProgramDir() + "Res"));

	ShaderCompileTest();

	UniformTest();

	GfxBufferTest();

	GfxTextureTest();
	
	return 0;
}

IMPLEMENT_MODULE_USD("UniformTest");
