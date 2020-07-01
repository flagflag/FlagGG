#include <Core/EventDefine.h>
#include <Core/DeviceEvent.h>
#include <Math/Rect.h>
#include <Scene/Octree.h>
#include <Scene/Light.h>
#include <Scene/StaticMeshComponent.h>
#include <Scene/SkeletonMeshComponent.h>
#include <Scene/AnimationComponent.h>
#include <Graphics/Material.h>
#include <Graphics/Model.h>
#include <Scene/Animation.h>

#include <Graphics/VertexBuffer.h>
#include <Graphics/IndexBuffer.h>
#include <Graphics/Geometry.h>
#include <Container/Vector.h>

#include "BgfxApp.h"

BgfxApp::BgfxApp(const FlagGG::Config::LJSONValue& command)
{

}

static Model* CreateStaticModel(Context* context)
{
	auto* cache = context->GetVariable<FlagGG::Resource::ResourceCache>("ResourceCache");

	Model* model = new Model(context);
	auto stream = cache->GetFile("bgfx/meshes/cube.bin");
	model->LoadFile(stream);

	return model;
}

void BgfxApp::Start()
{
	GameEngine::Start();

	context_->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, BgfxApp::Update, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_MOVE, BgfxApp::OnMouseMove, this));

// Scene
	scene_ = new FlagGG::Scene::Scene(context_);
	scene_->Start();
	scene_->CreateComponent<Octree>();

	auto* cameraNode = new Node();
	// cameraNode->SetPosition(Vector3(0, 0, -30));
	scene_->AddChild(cameraNode);
	camera_ = cameraNode->CreateComponent<Camera>();
	camera_->SetNearClip(0.1f);
	camera_->SetFarClip(1000000000.0f);

	auto* lightNode = new Node();
	Light* light = lightNode->CreateComponent<Light>();
	light->SetNearClip(0.1f);
	light->SetFarClip(1000000000.0f);
	light->SetOrthographic(true);
	lightNode->SetPosition(Vector3(0, 2, -1));
	lightNode->SetRotation(Quaternion(45.0f, Vector3(1.0f, 0.0f, 0.0f))); // 绕着x轴旋转45度，朝下
	scene_->AddChild(lightNode);

	auto* modelNode = new Node();
	auto* meshComp = modelNode->CreateComponent<StaticMeshComponent>();
	meshComp->SetModel(CreateStaticModel(context_));
	meshComp->SetMaterial(cache_->GetResource<Material>("bgfx/materials/BgfxStaticModel.ljson"));
	//auto* meshComp = modelNode->CreateComponent<SkeletonMeshComponent>();
	//meshComp->SetModel(cache_->GetResource<Model>(""));
	//meshComp->SetMaterial(cache_->GetResource<Material>("Materials/Warrior.ljson"));
	//auto* animComp = modelNode->CreateComponent<AnimationComponent>();
	//animComp->SetAnimation(cache_->GetResource<Animation>("Warrior_Idle.ani"));
	//animComp->Play(true);
	modelNode->SetPosition(Vector3(0, 0, 5));
	modelNode->SetRotation(Quaternion(180, Vector3(0.0f, 1.0f, 0.0f)));
	scene_->AddChild(modelNode);

// Window
	IntRect rect(0, 400, 500, 900);

	window_ = new Window(context_, nullptr, rect);
	window_->Show();
	window_->GetViewport()->SetCamera(camera_);
	window_->GetViewport()->SetScene(scene_);

	WindowDevice::RegisterWinMessage(window_);
}

void BgfxApp::Stop()
{
	WindowDevice::UnregisterWinMessage(window_);

	GameEngine::Stop();
}

void BgfxApp::Update(float timeStep)
{
	static const float walkSpeed_{ 5.0f };
	float walkDelta = timeStep * walkSpeed_;

	if (GetKeyState('W') < 0 || GetKeyState('w') < 0)
		camera_->Walk(walkDelta);

	if (GetKeyState('S') < 0 || GetKeyState('s') < 0)
		camera_->Walk(-walkDelta);

	if (GetKeyState('A') < 0 || GetKeyState('a') < 0)
		camera_->Strafe(-walkDelta);

	if (GetKeyState('D') < 0 || GetKeyState('d') < 0)
		camera_->Strafe(walkDelta);

	if (GetKeyState('E') < 0 || GetKeyState('e') < 0)
		camera_->Fly(walkDelta);

	if (GetKeyState('Q') < 0 || GetKeyState('Q') < 0)
		camera_->Fly(-walkDelta);

	if (GetKeyState('N') < 0 || GetKeyState('n') < 0)
		camera_->Roll(0.000020);

	if (GetKeyState('M') < 0 || GetKeyState('m') < 0)
		camera_->Roll(-0.000020);
}

void BgfxApp::OnMouseMove(KeyState* keyState, const Vector2& delta)
{
	static const float rate_{ 0.00005f };

 	camera_->Yaw(-delta.x_ * rate_);
 	camera_->Pitch(-delta.y_ * rate_);
}
