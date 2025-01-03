#include <Config/LJSONFile.h>
#include <Log.h>
#include <FileSystem/FileManager.h>
#include <FileSystem/FileSystemArchive/DefaultFileSystemArchive.h>
#include <Resource/ResourceCache.h>
#include <Utility/SystemHelper.h>
#include <Graphics/Shader.h>
#include <GfxDevice/GfxDevice.h>
#include <GfxDevice/GfxSwapChain.h>
#include <Graphics/Texture2D.h>
#include <Graphics/RenderEngine.h>
#include <Graphics/Window.h>
#include <Scene/Node.h>
#include <Scene/Camera.h>
#include <Utility/SystemHelper.h>
#include <Memory/MemoryHook.h>

using namespace FlagGG;

int main()
{
	GetSubsystem<AssetFileManager>()->AddArchive(new DefaultFileSystemArchive(GetLocalFileSystem(), GetProgramDir() + "Res"));

	auto* cache = GetSubsystem<ResourceCache>();
	ShaderCode* shaderCode = cache->GetResource<ShaderCode>("Shader/UnitTest/DepthTest.hlsl");

	auto* vsShader = shaderCode->GetShader(VS, {});
	auto* psShader = shaderCode->GetShader(PS, {});

	auto renderTarget = MakeShared<Texture2D>();
	renderTarget->SetNumLevels(1);
	renderTarget->SetSize(128, 128, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);

	auto depthStencil = MakeShared<Texture2D>();
	depthStencil->SetNumLevels(1);
	depthStencil->SetSize(128, 128, TEXTURE_FORMAT_D24S8, TEXTURE_DEPTHSTENCIL);

	auto cameraNode = MakeShared<Node>();
	auto camera = cameraNode->CreateComponent<Camera>();

	auto* renderEngine = GetSubsystem<RenderEngine>();
	renderEngine->Initialize();

	auto* gfxDevice = GfxDevice::GetDevice();

	WindowDevice::Initialize();
	auto window = new Window(nullptr, IntRect(0, 0, 128, 128));

	while (true)
	{
		WindowDevice::Update();

		gfxDevice->SetRenderTarget(0, renderTarget->GetRenderSurface());
		gfxDevice->SetDepthStencil(depthStencil->GetRenderSurface());
		gfxDevice->SetViewport(IntRect(0, 0, 128, 128));
		gfxDevice->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL, Color::BLACK, 0.25f, 0u);

		gfxDevice->SetDepthStencil(nullptr);
		gfxDevice->SetCullMode(CULL_NONE);
		gfxDevice->SetFillMode(FILL_SOLID);
		gfxDevice->SetDepthTestMode(COMPARISON_ALWAYS);
		gfxDevice->SetDepthWrite(false);
		gfxDevice->SetTexture(0, depthStencil->GetGfxTextureRef());
		gfxDevice->SetSampler(0, depthStencil->GetGfxSamplerRef());
		gfxDevice->SetShaders(vsShader->GetGfxRef(), psShader->GetGfxRef());

		renderEngine->DrawQuad(camera);

		window->GetSwapChain()->CopyData(renderTarget->GetGfxTextureRef());

		window->GetSwapChain()->Present();

		FlagGG::Sleep(16u);
	}

	return 0;
}

IMPLEMENT_MODULE_USD("DepthTest");
