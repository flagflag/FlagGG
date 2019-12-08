#include "EditorApplication.h"
#include "Importer.h"

#include <Math/Rect.h>
#include <Utility/SystemHelper.h>
#include <Graphics/Model.h>
#include <Graphics/Material.h>
#include <Graphics/Texture2D.h>
#include <Graphics/RenderEngine.h>
#include <Scene/StaticMeshComponent.h>
#include <Scene/SkeletonMeshComponent.h>
#include <Scene/Light.h>
#include <Scene/Octree.h>
#include <Log.h>

using namespace FlagGG::Math;
using namespace FlagGG::Utility;
using namespace FlagGG::Graphics;

void EditorApplication::Start()
{
	GameEngine::Start();

	CreateScene();

	SetupWindow();

	SetFrameRate(60);

	context_->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, EditorApplication::Update, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_UP, EditorApplication::OnKeyUp, this));
	context_->RegisterEvent(EVENT_HANDLER(Application::WINDOW_CLOSE, EditorApplication::WindowClose, this));

	FLAGGG_LOG_INFO("Start Editor.");
}

void EditorApplication::Stop()
{
	WindowDevice::UnregisterWinMessage(window_);

	GameEngine::Stop();

	FLAGGG_LOG_INFO("End Editor.");
}

void EditorApplication::Update(float timeStep)
{

}

void EditorApplication::CreateScene()
{
	scene_ = new FlagGG::Scene::Scene(context_);
	scene_->Start();

	scene_->CreateComponent<Octree>();

	auto* cameraNode = new Node();
	scene_->AddChild(cameraNode);
	auto* camera = cameraNode->CreateComponent<Camera>();
	camera->SetNearClip(1.0f);
	camera->SetFarClip(1000000000.0f);
	cameraOpt_ = new CameraOperation(context_, camera);

	SharedPtr<Node> unit(new Node());
	StaticMeshComponent* staticMeshComp = unit->CreateComponent<StaticMeshComponent>();
	staticMeshComp->SetModel(cache_->GetResource<Model>("Model/SpaceColony.mdl"));
	staticMeshComp->SetMaterial(cache_->GetResource<Material>("Materials/StaticModel.ljson"));
	unit->SetScale(Vector3(0.001, 0.001, 0.001));
	scene_->AddChild(unit);

	//Importer::SetContext(context_);
	//SharedPtr<Node> importNode = Importer::ImportScene("G:/Kitbash3D - Utopia/Kitbash3d_Utopia.FBX");
	//if (importNode)
	//{
	//	importNode_ = importNode;
	//	importNode->SetRotation(Quaternion(-90.0f, Vector3(1.0f, 0.0f, 0.0f)));
	//	importNode_->SetScale(Vector3(0.001, 0.001, 0.001));
	//	scene_->AddChild(importNode_);
	//}

	SharedPtr<Node> lightNode(new Node());
	lightNode->CreateComponent<Light>();
	lightNode->SetPosition(Vector3(0, 2, 0));
	lightNode->SetRotation(Quaternion(45.0f, Vector3(1.0f, 0.0f, 0.0f))); // 绕着x轴旋转45度，朝下
	scene_->AddChild(lightNode);

	SharedPtr<Node> skyBox(new Node());
	StaticMeshComponent* meshComp = skyBox->CreateComponent<StaticMeshComponent>();
	meshComp->SetModel(cache_->GetResource<Model>("Model/Box.mdl"));
	meshComp->SetMaterial(cache_->GetResource<Material>("Materials/Skybox.ljson"));
	skyBox->SetScale(Vector3(100000, 100000, 100000));
	scene_->AddChild(skyBox);
}

void EditorApplication::SetupWindow()
{
	IntRect rect = SystemHelper::GetDesktopRect();

	// 创建一张shaderMap
	static SharedPtr<Texture2D> shadowMap_;
	shadowMap_ = new Texture2D(context_);
	shadowMap_->SetNumLevels(1);
	shadowMap_->SetSize(rect.Width(), rect.Height(), RenderEngine::GetRGBFormat(), TEXTURE_RENDERTARGET);
	shadowMap_->Initialize();
	RenderEngine::Instance()->SetDefaultTextures(TEXTURE_CLASS_SHADOWMAP, shadowMap_);

	window_ = new Window(context_, nullptr, rect);
	window_->Show();
	window_->GetViewport()->SetCamera(cameraOpt_->GetCamera());
	window_->GetViewport()->SetScene(scene_);

	WindowDevice::RegisterWinMessage(window_);
}

void EditorApplication::OnKeyUp(KeyState* keyState, unsigned keyCode)
{
	// 导入场景
	if (keyCode == VK_F1)
	{
		Importer::SetContext(context_);
		SharedPtr<Node> importNode = Importer::ImportScene("G:/Kitbash3D-Space Colony/Kitbash3d_SpaceColony.FBX");
		if (importNode)
		{
			importNode_ = importNode;
			importNode_->SetPosition(Vector3(80, -2, 5));
			importNode_->SetScale(Vector3(0.001, 0.001, 0.001));
			scene_->AddChild(importNode_);
		}
	}

	if (keyCode == VK_F2)
	{
		Vector3 scale = importNode_->GetScale();
		Vector3 newScale = scale + Vector3(1, 1, 1);
		importNode_->SetScale(newScale);
		char buffer[256] = { 0 };
		sprintf(buffer, "(%lf,%lf,%lf)=>(%lf,%lf,%lf).", scale.x_, scale.y_, scale.z_, newScale.x_, newScale.y_, newScale.z_);
		MessageBoxA(nullptr, buffer, "Model Scale", 0);
	}

	if (keyCode == VK_F3)
	{
		Vector3 scale = importNode_->GetScale();
		Vector3 newScale = scale - Vector3(1, 1, 1);
		importNode_->SetScale(newScale);
		char buffer[256] = { 0 };
		sprintf(buffer, "(%lf,%lf,%lf)=>(%lf,%lf,%lf).", scale.x_, scale.y_, scale.z_, newScale.x_, newScale.y_, newScale.z_);
		MessageBoxA(nullptr, buffer, "Model Scale", 0);
	}
}

void EditorApplication::WindowClose(void* window)
{
	isRunning_ = false;
}

