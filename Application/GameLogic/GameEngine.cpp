#include "GameEngine.h"

#include <Graphics/RenderEngine.h>
#include <Graphics/Batch3D.h>
#include <Graphics/Shader.h>
#include <Graphics/VertexFormat.h>
#include <Graphics/Camera.h>
#include <Graphics/Material.h>
#include <Math/Matrix4.h>
#include <Core/Contex.h>
#include <Container/Ptr.h>
#include <Core/Function.h>
#include <Resource/ResourceCache.h>
#include <Utility/SystemHelper.h>

using namespace FlagGG::Graphics;
using namespace FlagGG::Math;
using namespace FlagGG::Core;
using namespace FlagGG::Resource;

void AddGrid(Batch3D& batch)
{
	float lineWidth = 0.01;
	float space = 0.1;
	int count = 1.0f / space * 2;

	Vector3 v1(-1.0f, -1.0f, -1.0f);
	Vector3 v2(-1.0f, -1.0f, 1.0f);

	Vector3 v3(-1.0f, -1.0f, -1.0f);
	Vector3 v4(1.0f, -1.0f, -1.0f);

	for (int i = 0; i < count; ++i)
	{
		Vector3 d1(space * i, 0.0f, 0.0f);

		batch.AddLine(
			v1 + d1, v2 + d1,
			Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
			0);

		Vector3 d2(0.0f, 0.0f, space * i);

		batch.AddLine(
			v3 + d2, v4 + d2,
			Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
			0);
	}
}

void LoadVertexData(Batch3D& batch, Batch3D& grid)
{
	AddGrid(grid);

	// 正面
	{
		batch.AddTriangle(
			Vector3(0.5f, 0.5f, -0.5f), Vector3(0.5f, -0.5f, -0.5f), Vector3(-0.5f, -0.5f, -0.5f),
			Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f),
			Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f),
			0);
		batch.AddTriangle(
			Vector3(-0.5f, -0.5f, -0.5f), Vector3(-0.5f, 0.5f, -0.5f), Vector3(0.5f, 0.5f, -0.5f),
			Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f),
			Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f),
			0);
	}

	// 上面
	{
		batch.AddTriangle(
			Vector3(0.5f, 0.5f, 0.5f), Vector3(0.5f, 0.5f, -0.5f), Vector3(-0.5f, 0.5f, -0.5f),
			Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f),
			Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f),
			0);
		batch.AddTriangle(
			Vector3(-0.5f, 0.5f, -0.5f), Vector3(-0.5f, 0.5f, 0.5f), Vector3(0.5f, 0.5f, 0.5f),
			Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f),
			Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f),
			0);
	}

	// 右面
	{
		batch.AddTriangle(
			Vector3(0.5f, 0.5f, 0.5f), Vector3(0.5f, -0.5f, 0.5f), Vector3(0.5f, -0.5f, -0.5f),
			Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f),
			Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f),
			0);
		batch.AddTriangle(
			Vector3(0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f),
			Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f),
			Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f),
			0);
	}

	// 背面
	{
		batch.AddTriangle(
			Vector3(-0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, 0.5f), Vector3(0.5f, -0.5f, 0.5f),
			Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f),
			Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f),
			0);
		batch.AddTriangle(
			Vector3(0.5f, -0.5f, 0.5f), Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, 0.5f, 0.5f),
			Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f),
			Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f),
			0);
	}

	// 下面
	{
		batch.AddTriangle(
			Vector3(-0.5f, -0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, -0.5f, -0.5f),
			Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f),
			Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f),
			0);
		batch.AddTriangle(
			Vector3(0.5f, -0.5f, -0.5f), Vector3(0.5f, -0.5f, 0.5f), Vector3(-0.5f, -0.5f, 0.5f),
			Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f),
			Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f),
			0);
	}

	// 左面
	{
		batch.AddTriangle(
			Vector3(-0.5f, 0.5f, -0.5f), Vector3(-0.5f, -0.5f, -0.5f), Vector3(-0.5f, -0.5f, 0.5f),
			Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f),
			Vector3(-1.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f),
			0);
		batch.AddTriangle(
			Vector3(-0.5f, -0.5f, 0.5f), Vector3(-0.5f, 0.5f, 0.5f), Vector3(-0.5f, 0.5f, -0.5f),
			Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f),
			Vector3(-1.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f),
			0);
	}
}

void GameEngine::SetFrameRate(float rate)
{
	frameRate_ = rate;
}

void GameEngine::Start()
{
	WindowDevice::Initialize();
	RenderEngine::Initialize();

	context_ = new Context();
	input_ = new Input(context_);
	cache_ = new ResourceCache(context_);
	context_->RegisterVariable<Input>(input_.Get(), "input");
	context_->RegisterVariable<ResourceCache>(cache_.Get(), "ResourceCache");

	cache_->AddResourceDir("C:/MyC++Projects/flag/Res");

	cameraOpt_ = new CameraOperation(context_);

	SharedPtr<Material> material = cache_->GetResource<Material>("Materials/Model.ljson");
	SharedPtr<Texture> texture = material->GetTexture();
	SharedPtr<Shader> vs = material->GetVSShader();
	SharedPtr<Shader> ps = material->GetPSShader();

	SharedPtr<Batch3D> batch(new Batch3D(DRAW_TRIANGLE, texture, nullptr));
	SharedPtr<Batch3D> grid(new Batch3D(DRAW_LINE, texture, nullptr)); //地板的batch
	LoadVertexData(*batch, *grid);

	SharedPtr<VertexFormat> format(new VertexFormat(vs->GetByteCode(), VERTEX3D));
	format->Initialize();

	renderContext_ = new RenderContext(grid, vs, ps, format);
	renderContext_->batchs_.Push(batch);

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
	viewport->SetRenderContext(renderContext_);
	viewport->SetRenderTarget(renderTexture[0]->GetRenderSurface());
	viewport->SetDepthStencil(renderTexture[1]->GetRenderSurface());
	viewports_.Push(viewport);

	window_ = new Window(context_, nullptr, rect);
	window_->Show();
	window_->GetViewport()->SetCamera(cameraOpt_->camera_);
	window_->GetViewport()->SetRenderContext(renderContext_);

	WindowDevice::RegisterWinMessage(window_);

	context_->RegisterEvent(EVENT_HANDLER(InputEvent::KEY_UP, GameEngine::OnKeyUp, this));
}

void GameEngine::Run()
{
	Start();

	while (true)
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
