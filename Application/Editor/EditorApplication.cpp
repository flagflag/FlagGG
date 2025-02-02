#include "EditorApplication.h"
#include "Importer.h"
#include "CSharpExport/CSharpExport.h"

#include <Core/EventManager.h>
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

EditorApplication::EditorApplication()
	: GameEngine()
	, libRuntime_(nullptr)
{

}

void EditorApplication::Start()
{
	InitCSharpEnv();

	GameEngine::Start();

	CreateScene();

	SetupWindow();

	SetFrameRate(60);

	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, EditorApplication::Update, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_UP, EditorApplication::OnKeyUp, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(InputEvent::MOUSE_UP, EditorApplication::OnMouseUp, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(Frame::END_FRAME, EditorApplication::OnRender, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(Application::WINDOW_CLOSE, EditorApplication::WindowClose, this));

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
	OnTick(timeStep);
}

void EditorApplication::InitCSharpEnv()
{
	if (DotNet_Init())
	{
		libRuntime_ = DotNet_InitLibRuntime("ScriptModule");

		DotNetFunction<void()> mathTest;
		if (mathTest.Bind(libRuntime_, "FlagGG.ScriptInterface", "MathTest"))
		{
			mathTest();
		}
	}
	else
	{
		FLAGGG_LOG_ERROR("Load dot net failed.");
	}
}

void EditorApplication::OnTick(Real timeStep)
{
	//uiElementsTick();
}

void EditorApplication::OnRender(Real timeStep)
{
	//uiElementsRender();
}

void EditorApplication::CreateScene()
{
	scene_ = new Scene();
	scene_->Start();

	scene_->CreateComponent<Octree>();

	auto* cameraNode = new Node();
	scene_->AddChild(cameraNode);
	auto* camera = cameraNode->CreateComponent<Camera>();
	camera->SetNearClip(1.0f);
	camera->SetFarClip(1000000000.0f);
	cameraOpt_ = new CameraOperation(camera);

	SharedPtr<Node> unit(new Node());
	StaticMeshComponent* staticMeshComp = unit->CreateComponent<StaticMeshComponent>();
	staticMeshComp->SetModel(GetSubsystem<ResourceCache>()->GetResource<Model>("Model/SpaceColony.mdl"));
	staticMeshComp->SetMaterial(GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/StaticModel.ljson"));
	unit->SetScale(Vector3(0.001, 0.001, 0.001));
	scene_->AddChild(unit);

#if 0
	SharedPtr<Node> importNode = Importer::ImportScene("D:/ThirdModels/Kitbash3D - Utopia/Kitbash3d_Utopia.FBX");
	if (importNode)
	{
		importNode_ = importNode;
		importNode->SetRotation(Quaternion(-90.0f, Vector3(1.0f, 0.0f, 0.0f)));
		importNode_->SetScale(Vector3(0.001, 0.001, 0.001));
		scene_->AddChild(importNode_);
	}
#endif

	SharedPtr<Node> lightNode(new Node());
	lightNode->CreateComponent<Light>();
	lightNode->SetPosition(Vector3(0, 2, 0));
	lightNode->SetRotation(Quaternion(45.0f, Vector3(1.0f, 0.0f, 0.0f))); // 绕着x轴旋转45度，朝下
	scene_->AddChild(lightNode);

	SharedPtr<Node> skyBox(new Node());
	StaticMeshComponent* meshComp = skyBox->CreateComponent<StaticMeshComponent>();
	meshComp->SetModel(GetSubsystem<ResourceCache>()->GetResource<Model>("Model/Box.mdl"));
	meshComp->SetMaterial(GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/Skybox.ljson"));
	skyBox->SetScale(Vector3(100000, 100000, 100000));
	skyBox->SetTranspent(true);
	scene_->AddChild(skyBox);
}

void EditorApplication::SetupWindow()
{
	IntRect rect = GetDesktopRect();

	// 创建一张shaderMap
	static SharedPtr<Texture2D> shadowMap_;
	shadowMap_ = new Texture2D();
	shadowMap_->SetNumLevels(1);
	shadowMap_->SetSize(rect.Width(), rect.Height(), RenderEngine::GetRGBFormat(), TEXTURE_RENDERTARGET);
	// shadowMap_->Initialize();
	RenderEngine::Instance().SetDefaultTextures(TEXTURE_CLASS_SHADOWMAP, shadowMap_);

	window_ = new Window(nullptr, rect);
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
		SharedPtr<Node> importNode = Importer::ImportScene("D:/ThirdModels/Kitbash3D-Space Colony/Kitbash3d_SpaceColony.FBX");
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

void EditorApplication::OnMouseUp(KeyState* keyState, MouseKey mouseKey, const IntVector2& mousePos)
{
	if (mouseKey == MOUSE_RIGHT)
	{
		IntVector2 mousePos = window_->GetMousePos();
		Ray ray = cameraOpt_->GetCamera()->GetScreenRay((float)mousePos.x_ / window_->GetWidth(), (float)mousePos.y_ / window_->GetHeight());
		PODVector<RayQueryResult> results;
		RayOctreeQuery query(results, ray, RAY_QUERY_AABB);
		scene_->GetComponent<Octree>()->Raycast(query);
		if (query.results_.Size() > 0u)
		{
			const auto& ret = query.results_[0];	
			SharedPtr<Node> axesNode(new Node());
			axesNode->SetScale(Vector3(1000.0f, 1000.0f, 1000.0f));
			auto* meshComp = axesNode->CreateComponent<StaticMeshComponent>();
			meshComp->SetModel(GetSubsystem<ResourceCache>()->GetResource<Model>("Model/Axes.mdl"));
			meshComp->SetMaterial(GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/StaticModel.ljson"));
			ret.node_->AddChild(axesNode);
		}
	}
}

void EditorApplication::WindowClose(void* window)
{
	isRunning_ = false;
}

