#include <Core/EventManager.h>
#include <Graphics/RenderEngine.h>
#include <Graphics/Batch2D.h>
#include <Graphics/RenderPipline.h>
#include <Scene/Light.h>
#include <Scene/Probe.h>
#include <Scene/Octree.h>
#include <Scene/StaticMeshComponent.h>
#include <Scene/OceanComponent.h>
#include <Scene/PrefabLoader.h>
#include <Physics/Physics.h>
#include <Log.h>
#include <IOFrame/Buffer/StringBuffer.h>
#include <FileSystem/FileManager.h>
#include <FileSystem/FileSystemArchive/DefaultFileSystemArchive.h>
#include <Config/LJSONFile.h>
#include <Core/EngineSettings.h>
#include <Core/Profiler.h>
#if FLAGGG_PHYSICS
#include <Physics/Physics.h>
#endif

#include "GameApplication.h"
#include "GamePlay/ThirdPersonPerspective.h"
#include "GamePlay/GamePlayOnline.h"
#include "GamePlay/GamePlayLocal.h"

#define ONLINE_GAME 0

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

GameApplication::GameApplication(LJSONValue commandParam, SetupFinish setupFinish) :
	commandParam_(commandParam),
	setupFinish_(setupFinish),
	luaVM_(new LuaVM())
{ }

void GameApplication::Start()
{
	GameEngine::Start();

	GetSubsystem<AssetFileManager>()->AddArchive(new DefaultFileSystemArchive(GetLocalFileSystem(), GetProgramDir() + "ResForSCE"));

	CreateScene();
	SetupWindow();
	OpenLuaVM();
	CreateNetwork();

	GetSubsystem<Context>()->RegisterVariable<LuaVM>(luaVM_, "LuaVM");
	GetSubsystem<Context>()->RegisterVariable<Network>(tcpNetwork_, NETWORK_TYPE_NAME[NETWORK_TYPE_TCP]);
	GetSubsystem<Context>()->RegisterVariable<Network>(udpNetwork_, NETWORK_TYPE_NAME[NETWORK_TYPE_UDP]);
	GetSubsystem<Context>()->RegisterVariable<Network>(webNetwork_, NETWORK_TYPE_NAME[NETWORK_TYPE_WEB]);

	perspective_ = new ThirdPersonPerspective(true, !commandParam_.Contains("camera_opt"));
	perspective_->SetSyncMode(
#if ONLINE_GAME
		SyncMode_State
#else
		SyncMode_Local
#endif
	);
	perspective_->SetCamera(camera_);
	perspective_->SetWindow(window_);
	perspective_->SetNode(mainHero_);
	perspective_->Reset();

#if ONLINE_GAME
	gameplay_ = new GamePlayOnline();
#else
	gameplay_ = new GamePlayLocal();
#endif
	gameplay_->Initialize(scene_);
	GetSubsystem<Context>()->RegisterVariable<GamePlayBase>(gameplay_, "GamePlayBase");

	logModule_ = new LuaLog();
	networkModule_ = new LuaNetwork();
	gameplayModule_ = new LuaGamePlay(scene_);

	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, GameApplication::Update, this));
#ifdef _WIN32
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_DOWN, GameApplication::OnKeyDown, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_UP, GameApplication::OnKeyUp, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_DOWN, GameApplication::OnMouseDown, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_UP, GameApplication::OnMouseUp, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_MOVE, GameApplication::OnMouseMove, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(Application::WINDOW_CLOSE, GameApplication::WindowClose, this));
#endif

	if (commandParam_.Contains("FrameRate"))
	{
		SetFrameRate(commandParam_["FrameRate"].ToDouble());
	}

	FLAGGG_LOG_INFO("start application.");

	String luaCodePath;
	if (commandParam_.Contains("CodePath"))
		luaCodePath = commandParam_["CodePath"].GetString();
	else
		luaCodePath = GetProgramDir() + "Res/Script/lua/";
	luaVM_->SetLoaderPath(luaCodePath);
#if ONLINE_GAME
	if (!luaVM_->Execute(luaCodePath + "Client/main.lua"))
	{
		FLAGGG_LOG_ERROR("Failed to execute main.lua.");
	}
#else
	if (!luaVM_->Execute(luaCodePath + "local_game.lua"))
	{
		FLAGGG_LOG_ERROR("Failed to execute local_game.lua.");
	}
	if (commandParam_.Contains("unit_test"))
	{
		const String unitTestLua = commandParam_["unit_test"].GetString();
		if (!luaVM_->Execute(luaCodePath + unitTestLua))
		{
			FLAGGG_LOG_STD_ERROR("Failed to execute %s.", unitTestLua.CString());
		}
	}
#endif

	if (setupFinish_)
	{
		setupFinish_(window_->GetHandle());
	}

	// ShowPrefab("deco\\Chinoiserie\\SM_CH_stone_lantern_A\\model.prefab");
	// ShowPrefab("deco\\Building\\Arch_01\\model.prefab");
}

void GameApplication::Stop()
{
#ifdef _WIN32
	WindowDevice::UnregisterWinMessage(window_);
#endif

	GameEngine::Stop();

	uiView_.Reset();

	FLAGGG_LOG_INFO("end application.");
}

void GameApplication::ShowPrefab(const String& prefabPath)
{
	if (previewPrefab_)
	{
		scene_->RemoveChild(previewPrefab_);
		previewPrefab_.Reset();
	}

	previewPrefab_ = LoadPrefab(prefabPath);
	ASSERT(previewPrefab_);
	if (previewPrefab_)
	{
		previewPrefab_->SetScale(Vector3(0.01, 0.01, 0.01));
		previewPrefab_->SetPosition(Vector3(10, 0, 0));
		scene_->AddChild(previewPrefab_);
	}
}

void GameApplication::Update(float timeStep)
{
	// luaVM_->CallEvent("update", timeStep);

	static float dissolveTime = 0.f;
	dissolveTime += timeStep * 0.25;

	if (dissolveTime > 1.0f)
		dissolveTime = 0.0f;

	forwarder_.Execute();

	if (dissolveHero_)
	{
		Material* material = dissolveHero_->GetMaterial();
		if (material)
		{
			material->GetShaderParameters()->SetValue("dissolveTime", dissolveTime);
		}
	}

	gameplay_->FrameUpdate(timeStep);
}

void GameApplication::CreateScene()
{
#ifdef _WIN32
	scene_ = new Scene();
	scene_->Start();

	scene_->CreateComponent<Octree>();
#if FLAGGG_PHYSICS
	scene_->CreateComponent<PhysicsScene>();
#endif

	auto* cameraNode = new Node();
	cameraNode->SetName("MainCamera");
	scene_->AddChild(cameraNode);
	camera_ = cameraNode->CreateComponent<Camera>();
	camera_->SetZUp(true);
	camera_->SetNearClip(1.0f);
	camera_->SetFarClip(1e5f);
	if (commandParam_.Contains("close_reversez"))
		camera_->SetReverseZ(false);

#if 0
	mainHero_ = new CEUnit();
	mainHero_->Load("Unit/MainHero.ljson");
	mainHero_->PlayAnimation("Animation/Warrior_Attack.ani", true);
	mainHero_->SetPosition(Vector3(4, 0, 0));
	mainHero_->SetName("MainHero");
#else
	mainHero_ = new SCEUnit();
#if !ONLINE_GAME
	// mainHero_->Load("characters1/jianke_c96b/model.prefab");
	// mainHero_->PlayAnimation("characters1/jianke_c96b/anim/idle.ani", true);
#endif
	mainHero_->SetPosition(Vector3(0, 0, 0));
	mainHero_->SetName("MainHero");

#endif
	scene_->AddChild(mainHero_);

	dissolveHero_ = new Unit();
	dissolveHero_->Load("Unit/DissolveHero.ljson");
	dissolveHero_->SetPosition(Vector3(20, 0, 0));
	dissolveHero_->SetRotation(Quaternion(90, Vector3(1.0f, 0.0f, 0.0f)));
	dissolveHero_->PlayAnimation("Animation/Kachujin_Walk.ani", true);
	dissolveHero_->SetName("DissolveHero");
	scene_->AddChild(dissolveHero_);

	SharedPtr<Node> lightNode(new Node());
	Light* light = lightNode->CreateComponent<Light>();
	light->SetNearClip(0.1f);
	light->SetFarClip(1000000000.0f);
	light->SetOrthographic(true);
	light->SetBrightness(3.f);
	lightNode->SetPosition(Vector3(-10, 10, 20));
	lightNode->SetDirection(Vector3(1.0f, 1.0f, -1.0f));
	lightNode->SetName("MainLight");
	scene_->AddChild(lightNode);
#endif

	auto* probeNode = new Node();
	probeNode->SetName("MainProbe");
	auto* probe = probeNode->CreateComponent<Probe>();
	probe->Load("SkyAmbientProbe/day");
	probe->SetArea(BoundingBox(-1e5, 1e5));
	scene_->AddChild(probeNode);

	skybox_ = new Unit();
	skybox_->Load("Unit/Skybox.ljson");
	skybox_->SetScale(Vector3(100000, 100000, 100000));
	skybox_->SetName("Skybox");
	skybox_->SetTranspent(true);
	skybox_->SetRotation(Quaternion(Vector3(0.f, 0.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)).Inverse());
	scene_->AddChild(skybox_);
}

void GameApplication::SetupWindow()
{
#ifdef _WIN32
	if (commandParam_.Contains("NoWindow"))
		return;

	IntRect desktopRect = GetDesktopRect();
	IntVector2 desktopSize(desktopRect.Width(), desktopRect.Height());
	IntRect rect(desktopSize.x_ * 0.1, desktopSize.y_ * 0.1, desktopSize.x_ * 0.1 + 1728, desktopSize.y_ * 0.1 + 1080);

	// 创建一张shaderMap
	shadowMap_ = new Texture2D();
	shadowMap_->SetNumLevels(1);
	shadowMap_->SetComparisonFunc(COMPARISON_LESS_EQUAL);
	shadowMap_->SetFilterMode(TEXTURE_FILTER_BILINEAR);
	shadowMap_->SetAddressMode(TEXTURE_COORDINATE_U, TEXTURE_ADDRESS_CLAMP);
	shadowMap_->SetAddressMode(TEXTURE_COORDINATE_V, TEXTURE_ADDRESS_CLAMP);
	shadowMap_->SetAddressMode(TEXTURE_COORDINATE_W, TEXTURE_ADDRESS_CLAMP);
	shadowMap_->SetSize(rect.Width(), rect.Height(), RenderEngine::GetReadableDepthFormat(), TEXTURE_DEPTHSTENCIL);
	RenderEngine::Instance().SetDefaultTexture(TEXTURE_CLASS_SHADOWMAP, shadowMap_);

	window_ = new Window(nullptr, rect);
	window_->Show();
	window_->GetViewport()->SetCamera(camera_);
	window_->GetViewport()->SetScene(scene_);
	if (commandParam_.Contains("render_pipline"))
	{
		const String type = commandParam_["render_pipline"].GetString();
		if (type == "deferred")
			window_->GetViewport()->SetRenderPipline(new DeferredRenderPipline());
		else if (type == "forward")
			window_->GetViewport()->SetRenderPipline(new ForwardRenderPipline());
	}

	WindowDevice::RegisterWinMessage(window_);

	GetSubsystem<Context>()->RegisterVariable<Window>(window_, "MainWindow");

#if 0
	if (auto buffer = GetSubsystem<AssetFileManager>()->OpenFileReader("WebUI/GameEntry.html"))
	{
		String htmlContent;
		buffer->ToString(htmlContent);

		uiView_ = new UIView(window_);
		uiView_->LoadHTML(htmlContent);
		uiView_->SetBackgroundTransparency(0.0f);
	}
#else
	if (commandParam_.Contains("web_ui"))
	{
		uiView_ = new UIView(window_);
		uiView_->LoadUrl("file:///WebUI/GameEntry.html");
		uiView_->SetBackgroundTransparency(0.0f);
	}
#endif
#endif
}

int GameApplication::SetupWebUI(LuaVM* luaVM)
{
	if (!uiView_)
	{
		uiView_ = new UIView(window_);
		uiView_->LinkLuaVM(luaVM);
	}
	return 0;
}

int GameApplication::LoadWebUI(LuaVM* luaVM)
{
	const String url = luaVM->Get<String>(1);
	uiView_->LoadUrl(url);
	uiView_->SetBackgroundTransparency(0.0f);
	return 0;
}

int GameApplication::GetWebView(LuaVM* luaVM)
{
	lua_pushlightuserdata(*luaVM, uiView_->GetWebView());
	return 1;
}

int GameApplication::SetCameraMoveSpeed(LuaVM* luaVM)
{
	perspective_->SetMoveSpeed(luaVM->Get<float>(1));
	return 0;
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
		C_LUA_API_PROXY(Begin, "main_end"),
	});

	luaVM_->RegisterCPPEvents(
		"app",
		this,
		{
			LUA_API_PROXY(GameApplication, SetupWebUI, "setup_web_ui"),
			LUA_API_PROXY(GameApplication, LoadWebUI, "load_web_ui"),
			LUA_API_PROXY(GameApplication, GetWebView, "get_web_view"),
			LUA_API_PROXY(GameApplication, SetCameraMoveSpeed, "set_camera_move_speed"),
		});
}

void GameApplication::CreateNetwork()
{
	tcpNetwork_ = new Network(NETWORK_TYPE_TCP);
	udpNetwork_ = new Network(NETWORK_TYPE_UDP);
	webNetwork_ = new Network(NETWORK_TYPE_WEB);

	gameProtoDstr_ = new GameProtoDistributor();
}

#ifdef _WIN32
void GameApplication::OnKeyDown(KeyState* keyState, UInt32 keyCode)
{
	// luaVM_->CallEvent("on_key_down", keyCode);
}

void GameApplication::OnKeyUp(KeyState* keyState, UInt32 keyCode)
{
	if (keyCode == VK_F1)
	{
		simpleParticle_ = new ParticleActor();
		simpleParticle_->SetPosition(Vector3(10, 0, 2));
		simpleParticle_->SetScale(Vector3(0.01, 0.01, 0.01));
		if (auto stream = GetSubsystem<AssetFileManager>()->OpenFileReader("Unit/ParticleActor.ljson"))
		{
			simpleParticle_->LoadStream(stream);
		}
		scene_->AddChild(simpleParticle_);
	}

	if (keyCode == VK_ESCAPE)
	{
		isRunning_ = false;
	}

	if (keyCode == VK_F2)
	{
		if (uiView_)
		{
			if (Equals(uiView_->GetBackgroundTransparency(), 1.0f))
			{
				uiView_->SetBackgroundTransparency(0.0f);
			}
			else
			{
				uiView_->SetBackgroundTransparency(1.0f);
			}
		}
	}

	if (keyCode == VK_F3)
	{
		if (GetSubsystem<EngineSettings>()->aoType_ == AmbientOcclusionType::Software)
			GetSubsystem<EngineSettings>()->aoType_ = AmbientOcclusionType::None;
		else
			GetSubsystem<EngineSettings>()->aoType_ = AmbientOcclusionType::Software;
	}

	if (keyCode == VK_F4)
	{
		const String& ret = GetSubsystem<Profiler>()->PrintData();
		printf("%s\n", ret.CString());
	}

	if (keyCode == VK_F5)
	{
		Shader::ReCompileAllShader();
	}

	// luaVM_->CallEvent("on_key_up", keyCode);
}

void GameApplication::OnMouseDown(KeyState* keyState, MouseKey mouseKey, const IntVector2& mousePos)
{
	// luaVM_->CallEvent("on_mouse_down", static_cast<uint32_t>(mouseKey));
}

void GameApplication::OnMouseUp(KeyState* keyState, MouseKey mouseKey, const IntVector2& mousePos)
{
	// luaVM_->CallEvent("on_mouse_up", static_cast<uint32_t>(mouseKey));
	if (mouseKey == MOUSE_RIGHT)
	{
		//IntVector2 mousePos = window_->GetMousePos();
		//Ray ray = camera_->GetScreenRay((float)mousePos.x_ / window_->GetWidth(), (float)mousePos.y_ / window_->GetHeight());
		//PODVector<RayQueryResult> results;
		//RayOctreeQuery query(results, ray, RAY_QUERY_AABB, 250.0f);
		//scene_->GetComponent<Octree>()->Raycast(query);
		//if (query.results_.Size() > 0u)
		//{
		//	const auto& ret = query.results_[0];
		//	auto* meshComp = ret.node_->CreateComponent<StaticMeshComponent>();
		//	meshComp->SetModel(GetSubsystem<ResourceCache>()->GetResource<Model>("Model/Axes.mdl"));
		//	meshComp->SetMaterial(GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/StaticModel.ljson"));
		//}
	}
}

void GameApplication::OnMouseMove(KeyState* keyState, const IntVector2& mousePos, const Vector2& delta)
{
	// luaVM_->CallEvent("on_mouse_move");
}

void GameApplication::WindowClose(void* window)
{
	isRunning_ = false;
}
#endif
