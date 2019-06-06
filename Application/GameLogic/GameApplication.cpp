#include <Graphics/RenderEngine.h>
#include <Log.h>

#include "GameApplication.h"

static int Begin(lua_State* L)
{
	FLAGGG_LOG_INFO("start excute lua script[main.lua].");
	return 0;
}

static int End(lua_State* L)
{
	FLAGGG_LOG_INFO("end excute lua script[main.lua].");
	return 0;
}

GameApplication::GameApplication(LJSONValue commandParam) :
	commandParam_(commandParam),
	luaVM_(new LuaVM()),
	network_(new Network())
{ }

void GameApplication::Start()
{
	GameEngine::Start();

	CreateScene();
	SetupWindow();
	OpenLuaVM();

	context_->RegisterVariable<LuaVM>(luaVM_, "LuaVM");
	context_->RegisterVariable<Network>(network_, "Network");

	logModule_ = new LuaLog(context_);
	networkModule_ = new LuaNetwork(context_);

	context_->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, GameApplication::Update, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_DOWN, GameApplication::OnKeyDown, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_UP, GameApplication::OnKeyUp, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_DOWN, GameApplication::OnMouseDown, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_UP, GameApplication::OnMouseUp, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_MOVE, GameApplication::OnMouseMove, this));
	context_->RegisterEvent(EVENT_HANDLER(Application::WINDOW_CLOSE, GameApplication::WindowClose, this));

	if (commandParam_.Contains("FrameRate"))
	{
		SetFrameRate(commandParam_["FrameRate"].ToDouble());
	}

	FLAGGG_LOG_INFO("start application.");
}

void GameApplication::Stop()
{
	WindowDevice::UnregisterWinMessage(window_);

	GameEngine::Stop();

	FLAGGG_LOG_INFO("end application.");
}

void GameApplication::Update(float timeStep)
{
	luaVM_->CallEvent("update", timeStep);
}

void GameApplication::CreateScene()
{
	scene_ = new FlagGG::Scene::Scene(context_);
	scene_->Start();

	mainHero_ = new Unit(context_);
	mainHero_->Load("Unit/MainHero.ljson");
	mainHero_->SetPosition(Vector3(0, -5, 5));
	mainHero_->PlayAnimation("Animation/Kachujin_Walk.ani");
	scene_->AddChild(mainHero_);

	terrain_ = new Terrain(context_);
	terrain_->SetRange(20, 20);
	terrain_->SetPosition(Vector3(-10, -5, 0));
	scene_->AddChild(terrain_);
}

void GameApplication::SetupWindow()
{
	cameraOpt_ = new CameraOperation(context_);

	IntRect rect(100, 100, 1000, 1000);

	renderTexture_[0] = new Texture2D(context_);
	renderTexture_[0]->SetNumLevels(1);
	renderTexture_[0]->SetSize(rect.Width(), rect.Height(), RenderEngine::GetRGBFormat(), TEXTURE_RENDERTARGET);
	renderTexture_[1] = new Texture2D(context_);
	renderTexture_[1]->SetNumLevels(1);
	renderTexture_[1]->SetSize(rect.Width(), rect.Height(), RenderEngine::GetDepthStencilFormat(), TEXTURE_DEPTHSTENCIL);

	SharedPtr<Viewport> viewport(new Viewport());
	viewport->Resize(rect);
	viewport->SetCamera(cameraOpt_->camera_);
	viewport->SetScene(scene_);
	viewport->SetRenderTarget(renderTexture_[0]->GetRenderSurface());
	viewport->SetDepthStencil(renderTexture_[1]->GetRenderSurface());
	viewports_.Push(viewport);

	window_ = new Window(context_, nullptr, rect);
	window_->Show();
	window_->GetViewport()->SetCamera(cameraOpt_->camera_);
	window_->GetViewport()->SetScene(scene_);

	WindowDevice::RegisterWinMessage(window_);
}

void GameApplication::OpenLuaVM()
{
	luaVM_->Open();
	if (!luaVM_->IsOpen())
	{
		FLAGGG_LOG_ERROR("open lua vm failed.");

		return;
	}

	luaVM_->RegisterCEvents(
	{
		C_LUA_API_PROXY(Begin, "main_begin"),
		C_LUA_API_PROXY(Begin, "main_end")
	});

	const String luaCodePath = commandParam_["CodePath"].GetString();

	if (!luaVM_->Execute(luaCodePath + "/main.lua"))
	{
		return;
	}
}

void GameApplication::OnKeyDown(KeyState* keyState, unsigned keyCode)
{
	luaVM_->CallEvent("on_key_down", keyCode);
}

void GameApplication::OnKeyUp(KeyState* keyState, unsigned keyCode)
{
	if (keyCode == VK_F11)
	{
		SharedPtr<Image> image = renderTexture_[0]->GetImage();
		image->SavePNG("E:\\FlagGG_Scene.png");
	}

	luaVM_->CallEvent("on_key_up", keyCode);
}

void GameApplication::OnMouseDown(KeyState* keyState, MouseKey mouseKey)
{
	luaVM_->CallEvent("on_mouse_down", static_cast<uint32_t>(mouseKey));
}

void GameApplication::OnMouseUp(KeyState* keyState, MouseKey mouseKey)
{
	luaVM_->CallEvent("on_mouse_up", static_cast<uint32_t>(mouseKey));
}

void GameApplication::OnMouseMove(KeyState* keyState, const Vector2& delta)
{
	luaVM_->CallEvent("on_mouse_move");
}

void GameApplication::WindowClose(void* window)
{
	isRunning_ = false;
}
