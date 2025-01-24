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
#include <Memory/MemoryHook.h>

#include <fstream>

using namespace FlagGG;

class UITestApp : public GameEngine
{
public:
	void Start() override
	{
		GameEngine::Start();

		IntRect rect = GetDesktopRect();
		window_ = new Window(nullptr, rect);
		window_->Show();

		WindowDevice::RegisterWinMessage(window_);

		GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_UP, UITestApp::OnKeyUp, this));

		CreateUIElement();
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

	void OnKeyUp(KeyState* keyState, UInt32 keyCode)
	{
		if (keyCode == VK_ESCAPE)
		{
			isRunning_ = false;
		}
	}

private:
	SharedPtr<Window> window_;

	Vector<SharedPtr<Batch>> batches_;
};

static LJSONValue CommandParam;

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
		GetSubsystem<EngineSettings>()->clusterLightEnabled_ = true;

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
