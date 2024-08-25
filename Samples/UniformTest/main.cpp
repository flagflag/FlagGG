#include <Config/LJSONFile.h>
#include <Log.h>
#include <FileSystem/FileManager.h>
#include <FileSystem/FileSystemArchive/DefaultFileSystemArchive.h>
#include <Resource/ResourceCache.h>
#include <Utility/SystemHelper.h>
#include <Graphics/Shader.h>
#include <GfxDevice/GfxDevice.h>
#include <GfxDevice/GfxProgram.h>

using namespace FlagGG;

int main()
{
	GetSubsystem<AssetFileManager>()->AddArchive(new DefaultFileSystemArchive(GetLocalFileSystem(), GetProgramDir() + "Res"));

	auto* cache = GetSubsystem<ResourceCache>();
	ShaderCode* shaderCode = cache->GetResource<ShaderCode>("Shader/UnitTest/UniformTest.hlsl");

	auto* vsShader = shaderCode->GetShader(VS, {});
	auto* psShader = shaderCode->GetShader(PS, {});

	SharedPtr<GfxProgram> program(GfxDevice::GetDevice()->CreateProgram());
	program->Link(vsShader->GetGfxRef(), psShader->GetGfxRef());
	
	return 0;
}

