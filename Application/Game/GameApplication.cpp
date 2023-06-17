#ifdef _WIN32
#include <Graphics/RenderEngine.h>
#include <Graphics/Batch2D.h>
#include <Scene/Light.h>
#include <Scene/Octree.h>
#include <Scene/StaticMeshComponent.h>
#include <Scene/OceanComponent.h>
#endif
#include <Log.h>
#include <IOFrame/Buffer/StringBuffer.h>

#include "GameApplication.h"
#include "GamePlay/ThirdPersonPerspective.h"
#include "GamePlay/GamePlayOnline.h"
#include "GamePlay/GamePlayLocal.h"

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
	Create2DBatch();
	OpenLuaVM();
	CreateNetwork();

	context_->RegisterVariable<LuaVM>(luaVM_, "LuaVM");
	context_->RegisterVariable<Network>(tcpNetwork_, NETWORK_TYPE_NAME[NETWORK_TYPE_TCP]);
	context_->RegisterVariable<Network>(udpNetwork_, NETWORK_TYPE_NAME[NETWORK_TYPE_UDP]);
	context_->RegisterVariable<Network>(webNetwork_, NETWORK_TYPE_NAME[NETWORK_TYPE_WEB]);

	perspective_ = new ThirdPersonPerspective(context_);
	perspective_->SetSyncMode(/*SyncMode_State*/SyncMode_Local);
	perspective_->SetCamera(camera_);
	perspective_->SetWindow(window_);
	perspective_->SetNode(mainHero_);
	perspective_->Reset();

#if 0
	gameplay_ = new GamePlayOnline(context_);
#else
	gameplay_ = new GamePlayLocal(context_);
#endif
	gameplay_->Initialize(scene_);
	context_->RegisterVariable<GamePlayBase>(gameplay_, "GamePlayBase");

	logModule_ = new LuaLog(context_);
	networkModule_ = new LuaNetwork(context_);
	gameplayModule_ = new LuaGamePlay(context_);

	context_->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, GameApplication::Update, this));
#ifdef _WIN32
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

	String luaCodePath;
	if (commandParam_.Contains("CodePath"))
		luaCodePath = commandParam_["CodePath"].GetString();
	else
		luaCodePath = GetProgramDir() + "Res/Script/lua/";
	luaVM_->SetLoaderPath(luaCodePath);
#if 0
	if (!luaVM_->Execute(luaCodePath + "main.lua"))
	{
		FLAGGG_LOG_ERROR("Failed to execute main.lua.");
	}
#else
	if (!luaVM_->Execute(luaCodePath + "local_game.lua"))
	{
		FLAGGG_LOG_ERROR("Failed to execute local_game.lua.");
	}
#endif
}

void GameApplication::Stop()
{
#ifdef _WIN32
	WindowDevice::UnregisterWinMessage(window_);
#endif

	GameEngine::Stop();

	FLAGGG_LOG_INFO("end application.");
}

void GameApplication::Update(float timeStep)
{
	// luaVM_->CallEvent("update", timeStep);

	static float dissolveTime = 0.f;
	dissolveTime += timeStep * 0.25;

	if (dissolveTime > 1.0f)
		dissolveTime = 0.0f;

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

void GameApplication::Create2DBatch()
{
	Vector<SharedPtr<Batch>> batches;

	SharedPtr<Batch2D> batch(new Batch2D());
	batch->AddTriangle(
		Vector2(-1, -1), Vector2(1, -1), Vector2(1, 1),
		Vector2(0, 0), Vector2(1, 0), Vector2(1, 1),
		0u
		);
	batch->AddTriangle(
		Vector2(-1, -1), Vector2(1, 1), Vector2(-1, 1),
		Vector2(0, 0), Vector2(1, 1), Vector2(0, 1),
		0u
	);
	batch->SetTexture(renderTexture_[0]);
	batches.Push(batch);
	RenderEngine::Instance()->PostRenderBatch(batches);
}

void GameApplication::CreateScene()
{
#ifdef _WIN32
	scene_ = new Scene(context_);
	scene_->Start();

	scene_->CreateComponent<Octree>();

	auto* cameraNode = new Node();
	scene_->AddChild(cameraNode);
	camera_ = cameraNode->CreateComponent<Camera>();
	camera_->SetNearClip(0.1f);
	camera_->SetFarClip(1e5f);

	auto* reflectionNode = new Node();
	cameraNode->AddChild(reflectionNode);
	reflectionCamera_ = reflectionNode->CreateComponent<Camera>();
	reflectionCamera_->SetNearClip(0.1f);
	reflectionCamera_->SetFarClip(1e5f);
	reflectionCamera_->SetViewMask(0xff00);
	reflectionCamera_->SetUseReflection(true);

	mainHero_ = new CEUnit(context_);
	mainHero_->Load("Unit/MainHero.ljson");
	mainHero_->PlayAnimation("Animation/Warrior_Attack.ani", true);
	mainHero_->SetPosition(Vector3(0, -4, 10));
	mainHero_->SetName("MainHero");
	scene_->AddChild(mainHero_);

	dissolveHero_ = new Unit(context_);
	dissolveHero_->Load("Unit/DissolveHero.ljson");
	dissolveHero_->SetPosition(Vector3(0, 0, 10));
	dissolveHero_->SetRotation(Quaternion(180, Vector3(0.0f, 1.0f, 0.0f)));
	dissolveHero_->PlayAnimation("Animation/Kachujin_Walk.ani", true);
	dissolveHero_->SetName("DissolveHero");
	scene_->AddChild(dissolveHero_);

	terrain_ = new Terrain(context_);
	terrain_->Create(64);
	terrain_->SetScale(Vector3(1, 0.4, 1));
	terrain_->SetPosition(Vector3(-80, -5, 0));
	terrain_->SetName("Terrain");
	scene_->AddChild(terrain_);

#if 0
	auto* lightNode = new Unit(context_);
	lightNode->Load("Unit/MainHero.ljson");
	lightNode->PlayAnimation("Animation/Kachujin_Walk.ani", true);
#else
	auto* lightNode = new Node();
#endif
	Light* light = lightNode->CreateComponent<Light>();
	light->SetNearClip(0.1f);
	light->SetFarClip(1000000000.0f);
	light->SetOrthographic(true);
	lightNode->SetPosition(Vector3(0, 2, -1));
	lightNode->SetRotation(Quaternion(45.0f, Vector3(1.0f, 0.0f, 0.0f))); // 绕着x轴旋转45度，朝下
	scene_->AddChild(lightNode);
#endif

	skybox_ = new Unit(context_);
	skybox_->Load("Unit/Skybox.ljson");
	skybox_->SetScale(Vector3(100000, 100000, 100000));
	skybox_->SetName("Skybox");
	skybox_->SetTranspent(true);
	scene_->AddChild(skybox_);

	waterDown_ = new Unit(context_);
	// waterDown_->Load("Unit/WaterDown.ljson");
	waterDown_->SetPosition(Vector3(0, -5, 10));
	waterDown_->SetScale(Vector3(10, 10, 10));
	waterDown_->SetName("WaterDown");
	waterDown_->SetTranspent(true);
	auto* comp = waterDown_->GetComponent<StaticMeshComponent>();
	if (comp)
		comp->SetViewMask(0xff0000); // 不进入水反射
	scene_->AddChild(waterDown_);

	water_ = new Unit(context_);
	// water_->Load("Unit/Ocean.ljson");
	water_->SetPosition(Vector3(0, -4, 10));
	water_->SetScale(Vector3(0.1, 0.1, 0.1));
	water_->SetName("Water");
	scene_->AddChild(water_);

	Plane waterPlane(water_->GetWorldRotation() * Vector3::UP, water_->GetWorldPosition());
	reflectionCamera_->SetReflectionPlane(waterPlane);
}

void GameApplication::SetupWindow()
{
#ifdef _WIN32
	if (commandParam_.Contains("NoWindow"))
		return;

	IntRect rect = GetDesktopRect();

	// 创建一张shaderMap
	shadowMap_ = new Texture2D(context_);
	shadowMap_->SetNumLevels(1);
	shadowMap_->SetSize(rect.Width(), rect.Height(), RenderEngine::GetRGBFormat(), TEXTURE_RENDERTARGET);
	RenderEngine::Instance()->SetDefaultTextures(TEXTURE_CLASS_SHADOWMAP, shadowMap_);

	renderTexture_[0] = new Texture2D(context_);
	renderTexture_[0]->SetNumLevels(1);
	renderTexture_[0]->SetSize(rect.Width(), rect.Height(), RenderEngine::GetRGBFormat(), TEXTURE_RENDERTARGET);

	renderTexture_[1] = new Texture2D(context_);
	renderTexture_[1]->SetNumLevels(1);
	renderTexture_[1]->SetSize(rect.Width(), rect.Height(), RenderEngine::GetDepthStencilFormat(), TEXTURE_DEPTHSTENCIL);

	rttTexture_[0] = new Texture2D(context_);
	rttTexture_[0]->SetNumLevels(1);
	rttTexture_[0]->SetSize(rect.Width(), rect.Height(), RenderEngine::GetRGBFormat(), TEXTURE_RENDERTARGET);

	rttTexture_[1] = new Texture2D(context_);
	rttTexture_[1]->SetNumLevels(1);
	rttTexture_[1]->SetSize(rect.Width(), rect.Height(), RenderEngine::GetDepthStencilFormat(), TEXTURE_DEPTHSTENCIL);

	SharedPtr<Viewport> rttViewport(new Viewport());
	rttViewport->Resize(IntRect(0, 0, rect.Width(), rect.Height()));
	rttViewport->SetCamera(reflectionCamera_);
	rttViewport->SetScene(scene_);
	rttViewport->SetRenderTarget(rttTexture_[0]->GetRenderSurface());
	rttViewport->SetDepthStencil(rttTexture_[1]->GetRenderSurface());
	viewports_.Push(rttViewport);

	auto* waterComp = water_->GetComponent<StaticMeshComponent>();
	if (waterComp)
	{
		waterComp->SetViewMask(0xff);
		auto* waterMaterial = waterComp->GetMaterial();
		if (waterMaterial)
		{
			waterMaterial->SetTexture(TEXTURE_CLASS_DIFFUSE, rttTexture_[0]);
			waterComp->SetMaterial(waterMaterial);
		}
	}
	auto* dynamicWaterComp = water_->GetComponent<OceanComponent>();
	if (dynamicWaterComp)
	{
		dynamicWaterComp->SetViewMask(0xff);
		auto* waterMaterial = waterComp->GetMaterial();
		if (waterMaterial)
		{
			waterMaterial->SetTexture(TEXTURE_CLASS_DIFFUSE, rttTexture_[0]);
			dynamicWaterComp->SetMaterial(waterMaterial);
		}
	}

	SharedPtr<Viewport> viewport(new Viewport());
	viewport->Resize(IntRect(0, 0, rect.Width(), rect.Height()));
	viewport->SetCamera(camera_);
	viewport->SetScene(scene_);
	viewport->SetRenderTarget(renderTexture_[0]->GetRenderSurface());
	viewport->SetDepthStencil(renderTexture_[1]->GetRenderSurface());
	viewports_.Push(viewport);

	window_ = new Window(context_, nullptr, rect);
	window_->Show();
	window_->GetViewport()->SetCamera(camera_);
	window_->GetViewport()->SetScene(scene_);

	WindowDevice::RegisterWinMessage(window_);

	input_->HideMouse();
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

	gameProtoDstr_ = new GameProtoDistributor(context_);
}

#ifdef _WIN32
void GameApplication::OnKeyDown(KeyState* keyState, UInt32 keyCode)
{
	// luaVM_->CallEvent("on_key_down", keyCode);
}

void GameApplication::OnKeyUp(KeyState* keyState, UInt32 keyCode)
{
	if (keyCode == VK_F11)
	{
		SharedPtr<Image> image = renderTexture_[0]->GetImage();
		image->SavePNG("E:\\FlagGG_Scene.png");
	}

	if (keyCode == VK_F9)
	{
		static bool flag = false;
		if (!flag)
		{
			window_->GetViewport()->SetCamera(reflectionCamera_);
			viewports_[1]->SetCamera(reflectionCamera_);
		}
		else
		{
			window_->GetViewport()->SetCamera(camera_);
			viewports_[1]->SetCamera(camera_);
		}
		flag = !flag;
	}

	if (keyCode == VK_ESCAPE)
	{
		isRunning_ = false;
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
	if (mouseKey == MOUSE_RIGHT)
	{
		IntVector2 mousePos = window_->GetMousePos();
		Ray ray = camera_->GetScreenRay((float)mousePos.x_ / window_->GetWidth(), (float)mousePos.y_ / window_->GetHeight());
		PODVector<RayQueryResult> results;
		RayOctreeQuery query(results, ray, RAY_QUERY_AABB, 250.0f);
		scene_->GetComponent<Octree>()->Raycast(query);
		if (query.results_.Size() > 0u)
		{
			const auto& ret = query.results_[0];
			auto* meshComp = ret.node_->CreateComponent<StaticMeshComponent>();
			meshComp->SetModel(cache_->GetResource<Model>("Model/Axes.mdl"));
			meshComp->SetMaterial(cache_->GetResource<Material>("Materials/StaticModel.ljson"));
		}
	}
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
