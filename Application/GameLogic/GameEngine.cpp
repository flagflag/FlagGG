#include "GameEngine.h"

#include <Graphics/RenderEngine.h>
#include <Graphics/Batch3D.h>
#include <Graphics/Shader.h>
#include <Graphics/VertexFormat.h>
#include <Graphics/Camera.h>
#include <Graphics/Material.h>
#include <Scene/SkeletonMeshComponent.h>
#include <Math/Matrix4.h>
#include <Core/Context.h>
#include <Container/Ptr.h>
#include <Core/Function.h>
#include <Resource/ResourceCache.h>
#include <Utility/SystemHelper.h>

using namespace FlagGG::Math;

void GameEngine::SetFrameRate(float rate)
{
	frameRate_ = rate;
}

void GameEngine::CreateCoreObject()
{
	context_ = new Context();
	input_ = new Input(context_);
	cache_ = new ResourceCache(context_);
	cache_->AddResourceDir("../../../Res");
	context_->RegisterVariable<Input>(input_.Get(), "input");
	context_->RegisterVariable<ResourceCache>(cache_.Get(), "ResourceCache");
}

void GameEngine::CreateScene()
{
	SharedPtr<Model> model = cache_->GetResource<Model>("Model/Kachujin.mdl");
	SharedPtr<Material> material = cache_->GetResource<Material>("Materials/Model.ljson");
	SharedPtr<SkeletonMeshComponent> skeletonMeshComponent(new SkeletonMeshComponent());
	SharedPtr<Node> node(new Node());
	node->AddComponent(skeletonMeshComponent);
	skeletonMeshComponent->SetModel(model);
	skeletonMeshComponent->SetMaterial(material);
	scene_ = new Scene(context_);
	scene_->AddChild(node);
	scene_->Start();
}

void GameEngine::SetupWindow()
{
	cameraOpt_ = new CameraOperation(context_);

	IntRect rect(100, 100, 1000, 1000);

	renderTexture[0] = new Texture2D(context_);
	renderTexture[0]->SetNumLevels(1);
	renderTexture[0]->SetSize(rect.Width(), rect.Height(), RenderEngine::GetRGBFormat(), TEXTURE_RENDERTARGET);
	renderTexture[1] = new Texture2D(context_);
	renderTexture[1]->SetNumLevels(1);
	renderTexture[1]->SetSize(rect.Width(), rect.Height(), RenderEngine::GetDepthStencilFormat(), TEXTURE_DEPTHSTENCIL);

	SharedPtr<Viewport> viewport(new Viewport());
	viewport->Resize(rect);
	viewport->SetCamera(cameraOpt_->camera_);
	viewport->SetScene(scene_);
	viewport->SetRenderTarget(renderTexture[0]->GetRenderSurface());
	viewport->SetDepthStencil(renderTexture[1]->GetRenderSurface());
	viewports_.Push(viewport);

	window_ = new Window(context_, nullptr, rect);
	window_->Show();
	window_->GetViewport()->SetCamera(cameraOpt_->camera_);
	window_->GetViewport()->SetScene(scene_);

	WindowDevice::RegisterWinMessage(window_);
}

void GameEngine::Start()
{
	WindowDevice::Initialize();
	RenderEngine::Initialize();

	CreateCoreObject();
	CreateScene();
	SetupWindow();

	context_->RegisterEvent(EVENT_HANDLER(Application::WINDOW_CLOSE, GameEngine::WindowClose, this));
	context_->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_UP, GameEngine::OnKeyUp, this));
}

void GameEngine::Run()
{
	Start();

	isRunning_ = true;

	while (isRunning_)
	{
		uint32_t deltaTime = timer_.GetMilliSeconds(true);
		float timeStep = (float)deltaTime / 1000.0f;

		context_->SendEvent<Frame::FRAME_BEGIN_HANDLER>(Frame::FRAME_BEGIN, timeStep);

		WindowDevice::Update();

		context_->SendEvent<Frame::LOGIC_UPDATE_HANDLER>(Frame::LOGIC_UPDATE, timeStep);

		window_->Render();

		for (const auto& viewport : viewports_)
		{
			RenderEngine::Render(viewport);
		}

		context_->SendEvent<Frame::FRAME_END_HANDLER>(Frame::FRAME_END, timeStep);

		float sleepTime = 1000.0f / frameRate_ - timer_.GetMilliSeconds(false);
		if (sleepTime > 0.0f)
		{
			SystemHelper::Sleep(sleepTime);
		}
	}

	Stop();
}

void GameEngine::WindowClose(void* window)
{
	isRunning_ = false;
}

void GameEngine::OnKeyUp(KeyState* keyState, unsigned keyCode)
{
	if (keyCode == VK_F11)
	{
		SharedPtr<Image> image = renderTexture[0]->GetImage();
		image->SavePNG("E:\\FlagGG_Scene.png");
	}
}

void GameEngine::Stop()
{
	WindowDevice::UnregisterWinMessage(window_);

	WindowDevice::Uninitialize();
	RenderEngine::Uninitialize();
}