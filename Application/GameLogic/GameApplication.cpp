#ifdef WIN32
#include <Graphics/RenderEngine.h>
#include <Scene/Light.h>
#endif
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
	luaVM_(new LuaVM())
{ }

void GameApplication::Start()
{
	GameEngine::Start();

	CreateScene();
	SetupWindow();
	OpenLuaVM();
	CreateNetwork();

	context_->RegisterVariable<LuaVM>(luaVM_, "LuaVM");
	context_->RegisterVariable<Network>(tcpNetwork_, NETWORK_TYPE_NAME[NETWORK_TYPE_TCP]);
	context_->RegisterVariable<Network>(udpNetwork_, NETWORK_TYPE_NAME[NETWORK_TYPE_UDP]);
	context_->RegisterVariable<Network>(webNetwork_, NETWORK_TYPE_NAME[NETWORK_TYPE_WEB]);

	logModule_ = new LuaLog(context_);
	networkModule_ = new LuaNetwork(context_);

	context_->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, GameApplication::Update, this));
#ifdef WIN32
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_DOWN, GameApplication::OnKeyDown, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_UP, GameApplication::OnKeyUp, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_DOWN, GameApplication::OnMouseDown, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_UP, GameApplication::OnMouseUp, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_MOVE, GameApplication::OnMouseMove, this));
	context_->RegisterEvent(EVENT_HANDLER(Application::WINDOW_CLOSE, GameApplication::WindowClose, this));
#endif

	if (commandParam_.Contains("FrameRate"))
	{
		SetFrameRate(commandParam_["FrameRate"].ToDouble());
	}

	FLAGGG_LOG_INFO("start application.");

	const String luaCodePath = commandParam_["CodePath"].GetString();
	luaVM_->SetLoaderPath(luaCodePath);
	if (!luaVM_->Execute(luaCodePath + "main.lua"))
	{
		FLAGGG_LOG_ERROR("Failed to execute main.lua.");
	}
}

void GameApplication::Stop()
{
#ifdef WIN32
	WindowDevice::UnregisterWinMessage(window_);
#endif

	GameEngine::Stop();

	FLAGGG_LOG_INFO("end application.");
}

void GameApplication::Update(float timeStep)
{
	// luaVM_->CallEvent("update", timeStep);
}

void GameApplication::CreateScene()
{
#ifdef WIN32
	scene_ = new FlagGG::Scene::Scene(context_);
	scene_->Start();

	mainHero_ = new Unit(context_);
	mainHero_->Load("Unit/MainHero.ljson");
	mainHero_->SetPosition(Vector3(0, -5, 10));
	mainHero_->PlayAnimation("Animation/Kachujin_Walk.ani", true);
	mainHero_->SetRotation(Quaternion(180, Vector3(0.0f, 1.0f, 0.0f)));
	scene_->AddChild(mainHero_);

	terrain_ = new Terrain(context_);
	terrain_->Create(64);
	terrain_->SetPosition(Vector3(-80, -5, 0));
	scene_->AddChild(terrain_);

	//water_ = new Unit(context_);
	//water_->Load("Unit/Water.ljson");
	//water_->SetPosition(Vector3(0, -5, 10));
	//water_->SetScale(Vector3(1000, 1000, 1000));
	//scene_->AddChild(water_);

#if 1
	auto* lightNode = new Unit(context_);
	lightNode->Load("Unit/MainHero.ljson");
	lightNode->PlayAnimation("Animation/Kachujin_Walk.ani", true);
#else
	auto* lightNode = new Node();
#endif
	lightNode->CreateComponent<Light>();
	lightNode->SetPosition(Vector3(0, 20, 0));
	lightNode->SetRotation(Quaternion(45.0f, Vector3(1.0f, 0.0f, 0.0f))); // 绕着x轴旋转45度，朝下
	scene_->AddChild(lightNode);
#endif
}

void GameApplication::SetupWindow()
{
#ifdef WIN32
	if (commandParam_.Contains("NoWindow"))
		return;

	cameraOpt_ = new CameraOperation(context_);

	IntRect desktopRect = SystemHelper::GetDesktopRect();
	uint32_t length = Min(desktopRect.Width(), desktopRect.Height());
	IntRect rect(IntVector2(0, 0), IntVector2(length, length));
	

	// 创建一张shaderMap
	shadowMap_ = new Texture2D(context_);
	shadowMap_->SetNumLevels(1);
	shadowMap_->SetSize(rect.Width(), rect.Height(), RenderEngine::GetRGBFormat(), TEXTURE_RENDERTARGET);
	shadowMap_->Initialize();
	RenderEngine::Instance()->SetDefaultTextures(TEXTURE_CLASS_SHADOWMAP, shadowMap_);

	renderTexture_[0] = new Texture2D(context_);
	renderTexture_[0]->SetNumLevels(1);
	renderTexture_[0]->SetSize(rect.Width(), rect.Height(), RenderEngine::GetRGBFormat(), TEXTURE_RENDERTARGET);
	renderTexture_[1] = new Texture2D(context_);
	renderTexture_[1]->SetNumLevels(1);
	renderTexture_[1]->SetSize(rect.Width(), rect.Height(), RenderEngine::GetDepthStencilFormat(), TEXTURE_DEPTHSTENCIL);

	SharedPtr<Viewport> viewport(new Viewport());
	viewport->Resize(IntRect(0, 0, rect.Width(), rect.Height()));
	viewport->SetCamera(cameraOpt_->GetCamera());
	viewport->SetScene(scene_);
	viewport->SetRenderTarget(renderTexture_[0]->GetRenderSurface());
	viewport->SetDepthStencil(renderTexture_[1]->GetRenderSurface());
	viewports_.Push(viewport);

	window_ = new Window(context_, nullptr, rect);
	window_->Show();
	window_->GetViewport()->SetCamera(cameraOpt_->GetCamera());
	window_->GetViewport()->SetScene(scene_);

	cameraOpt_->GetCamera()->SetNearClip(1.0f);
	cameraOpt_->GetCamera()->SetFarClip(1000000000.0f);

	WindowDevice::RegisterWinMessage(window_);
#endif
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
}

void GameApplication::CreateNetwork()
{
	tcpNetwork_ = new Network(context_, NETWORK_TYPE_TCP);
	udpNetwork_ = new Network(context_, NETWORK_TYPE_UDP);
	webNetwork_ = new Network(context_, NETWORK_TYPE_WEB);
}

#ifdef WIN32
void GameApplication::OnKeyDown(KeyState* keyState, unsigned keyCode)
{
	// luaVM_->CallEvent("on_key_down", keyCode);
}

void GameApplication::OnKeyUp(KeyState* keyState, unsigned keyCode)
{
	if (keyCode == VK_F11)
	{
		SharedPtr<Image> image = renderTexture_[0]->GetImage();
		image->SavePNG("E:\\FlagGG_Scene.png");
	}
	// luaVM_->CallEvent("on_key_up", keyCode);
}

void GameApplication::OnMouseDown(KeyState* keyState, MouseKey mouseKey)
{
	// luaVM_->CallEvent("on_mouse_down", static_cast<uint32_t>(mouseKey));
}

void GameApplication::OnMouseUp(KeyState* keyState, MouseKey mouseKey)
{
	// luaVM_->CallEvent("on_mouse_up", static_cast<uint32_t>(mouseKey));
}

void GameApplication::OnMouseMove(KeyState* keyState, const Vector2& delta)
{
	// luaVM_->CallEvent("on_mouse_move");
}

void GameApplication::WindowClose(void* window)
{
	isRunning_ = false;
}
#endif
