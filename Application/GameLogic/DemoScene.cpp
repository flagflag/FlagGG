#include "DemoScene.h"

#include <Graphics/RenderEngine.h>
#include <Graphics/Texture2D.h>
#include <Graphics/Batch3D.h>
#include <Graphics/Shader.h>
#include <Graphics/VertexFormat.h>
#include <Graphics/Camera.h>
#include <Math/Matrix4.h>
#include <Core/Contex.h>
#include <Container/Ptr.h>
#include <Core/Function.h>

using namespace FlagGG::Graphics;
using namespace FlagGG::Math;
using namespace FlagGG::Core;

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

void DemoScene::Start()
{
	WindowDevice::Initialize();
	RenderEngine::Initialize();

	context_ = new Context();
	Input* input = new Input(context_);
	context_->RegisterVariable<Input>(input, "input");

	cameraOpt_ = new CameraOperation(context_);

	SharedPtr<Texture2D> texture(new Texture2D(L"../../../Samples/Direct3D_Cube/texture.dds"));
	texture->Initialize();

	SharedPtr<Batch3D> batch(new Batch3D(DRAW_TRIANGLE, texture, nullptr));
	SharedPtr<Batch3D> grid(new Batch3D(DRAW_LINE, texture, nullptr)); //地板的batch
	LoadVertexData(*batch, *grid);

	SharedPtr<Shader> vs(new Shader(L"../../../FlagGG/Shader/3D_VS.hlsl", VS));
	SharedPtr<Shader> ps(new Shader(L"../../../FlagGG/Shader/3D_PS.hlsl", PS));
	vs->Initialize();
	ps->Initialize();

	SharedPtr<VertexFormat> format(new VertexFormat(vs->GetByteCode(), VERTEX3D));
	format->Initialize();

	viewport_ = new WinViewport(context_, nullptr, 100, 100, 500, 500);

	viewport_->Initialize();
	viewport_->Show();
	viewport_->SetCamera(cameraOpt_->camera_);

	renderContext_ = new RenderContext(grid, vs, ps, format);
	renderContext_->batchs_.Push(batch);

	WindowDevice::RegisterWinMessage(viewport_.Get());
}

void DemoScene::Run()
{
	Start();

	auto lastTime = GetTickCount();

	while (true)
	{
		auto currTime = GetTickCount();
		float timeStep = ((float)currTime - lastTime) / 1000.0f;
		lastTime = currTime;

		context_->SendEvent<Frame::FRAME_BEGIN_HANDLER>(Frame::FRAME_BEGIN, timeStep);

		WindowDevice::Update();

		context_->SendEvent<Frame::LOGIC_UPDATE_HANDLER>(Frame::LOGIC_UPDATE, timeStep);

		viewport_->Render(renderContext_);

		context_->SendEvent<Frame::FRAME_END_HANDLER>(Frame::FRAME_END, timeStep);
	}

	Stop();
}

void DemoScene::Stop()
{
	WindowDevice::UnregisterWinMessage(viewport_.Get());

	WindowDevice::Uninitialize();
	RenderEngine::Uninitialize();
}
