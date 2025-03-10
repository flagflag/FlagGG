#include <Graphics/Window.h>
#include <Graphics/RenderEngine.h>
#include <Graphics/Texture2D.h>
#include <Graphics/Batch3D.h>
#include <Graphics/Shader.h>
#include <Math/Matrix4.h>
#include <Core/Context.h>
#include <Core/DeviceEvent.h>
#include <Container/Ptr.h>
#include <Core/Function.h>
#include <GameEngine.h>
#include <UI/UISystem.h>
#include <UI/UIElement.h>
#include <Core/EventDefine.h>
#include <Core/EventManager.h>
#include <Core/EngineSettings.h>
#include <FileSystem/FileHandle/LocalFileHandle.h>
#include <FileSystem/FileManager.h>
#include <FileSystem/FileSystemArchive/DefaultFileSystemArchive.h>
#include <Memory/MemoryHook.h>
#include <WebUISystem/UIView.h>
#include <GfxDevice/GfxSwapChain.h>

#include <fstream>

using namespace FlagGG;

static LJSONValue CommandParam;

class UITestApp : public GameEngine
{
public:
	void Start() override
	{
		GameEngine::Start();

		GetSubsystem<AssetFileManager>()->AddArchive(new DefaultFileSystemArchive(GetLocalFileSystem(), GetProgramDir() + "ResForSCE"));

		IntRect rect = GetDesktopRect();
		window_ = new Window(nullptr, rect);
		window_->Show();

		WindowDevice::RegisterWinMessage(window_);

		GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_UP, UITestApp::OnKeyUp, this));

		if (CommandParam.Contains("html_url"))
		{
			CreateWebUIFromURL(CommandParam["html_url"].GetString());
		}
		else if (CommandParam.Contains("html_path"))
		{
			CreateWebUIFromFile(CommandParam["html_path"].GetString());
		}
		else
		{
			CreateUIElement();
		}
	}

	void Stop() override
	{
		WindowDevice::UnregisterWinMessage(window_);

		GameEngine::Stop();
	}

	void CreateUIElement()
	{
		window_->CreateUIElement();
		
		auto* uiRoot = window_->GetUIElement();
		uiRoot->SetColor(Color::RED);
		{
			auto* panel = uiRoot->CreateChild();
			panel->SetColor(Color::GREEN);
			panel->SetWidthPercent(1.0f);
			panel->SetHeightPercent(1.0f);
		}
		{
			auto* panel = uiRoot->CreateChild();
			panel->SetColor(Color::BLUE);
			panel->SetWidthPercent(1.0f);
			panel->SetHeightPercent(1.0f);
		}
	}

	void CreateWebUIFromURL(const String& url)
	{
#if 1
		uiView_ = new UIView(window_);
		uiView_->LoadUrl(url);
#else
		uiView_ = new UIView(window_->GetWidth(), window_->GetHeight());
		uiView_->LoadUrl(url);

		//window_->CreateUIElement();
		//auto* uiRoot = window_->GetUIElement();
		//uiRoot->SetColor(Color::WHITE);
		//uiRoot->SetTexture(uiView_->GetRenderTexture());

		GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(Frame::RENDER_UPDATE, UITestApp::RenderUpdate, this));
#endif
	}

	void RenderUpdate(Real timeStep)
	{
		window_->GetSwapChain()->CopyData(uiView_->GetRenderTexture()->GetGfxTextureRef());
	}

	void CreateWebUIFromFile(const String& htmlPath)
	{
		FlagGG::LocalFileHandle localFile;
		if (localFile.Open(htmlPath, FlagGG::FileMode::FILE_READ))
		{
			FlagGG::String fileContent;
			fileContent.Resize(localFile.Size());
			localFile.Read(&fileContent[0], fileContent.Length());

			uiView_ = new UIView(window_);
			uiView_->LoadHTML(fileContent);
		}
	}

	void OnKeyUp(KeyState* keyState, UInt32 keyCode)
	{
		if (keyState->GetSender() == window_ && keyCode == VK_ESCAPE)
		{
			isRunning_ = false;
		}
	}

private:
	SharedPtr<Window> window_;

	SharedPtr<UIView> uiView_;

	Vector<SharedPtr<Batch>> batches_;
};

void Run()
{
	String rendererType = CommandParam["renderer_type"].GetString();
	if (rendererType == "dx11")
		GetSubsystem<EngineSettings>()->rendererType_ = RENDERER_TYPE_D3D11;
	else if (rendererType == "gl")
		GetSubsystem<EngineSettings>()->rendererType_ = RENDERER_TYPE_OPENGL;
	else if (rendererType == "metal")
		GetSubsystem<EngineSettings>()->rendererType_ = RENDERER_TYPE_METAL;
	else if (rendererType == "vk")
		GetSubsystem<EngineSettings>()->rendererType_ = RENDERER_TYPE_VULKAN;
	if (CommandParam.Contains("cluster_light"))
		GetSubsystem<EngineSettings>()->clusterLightType_ = (ClusterLightType)ToUInt(CommandParam["cluster_light"].GetString());;

	UITestApp app;
	app.Run();
}

int main(int argc, const char* argv[])
{
	if (ParseCommand(argv + 1, argc - 1, CommandParam))
	{
		Run();
	}

	return 0;
}

IMPLEMENT_MODULE_USD("UITest");
