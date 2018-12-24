#include "Graphics/WinViewport.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Batch2D.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Shader.h"
#include "Graphics/VertexFormat.h"

void Demo1Run()
{
	using namespace FlagGG::Graphics;
	using namespace FlagGG::Math;

	WindowDevice::Initialize();
	RenderEngine::Initialize();

	Texture2D texture[2] = {
		//Texture2D(L"E:\\Res\\UI\\image\\HeroIcon\\heroHead_10003_icon.png"),
		//Texture2D(L"E:\\Res\\UI\\image\\HeroIcon\\heroHead_10007_icon.png")
		Texture2D(L"E:\\heroHead_10003_icon.dds"),
		Texture2D(L"E:\\heroHead_10009_icon.dds")
	};
	texture[0].Initialize();
	texture[1].Initialize();

	Batch2D batch[2] = {
		Batch2D(&texture[0], nullptr),
		Batch2D(&texture[1], nullptr)
	};

	batch[0].AddTriangle(
		Vector2(1.0f, 1.0f), Vector2(1.0f, -1.0f), Vector2(-1.0f, -1.0f),
		Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f),
		0
		);
	batch[0].AddTriangle(
		Vector2(-1.0f, -1.0f), Vector2(-1.0f, 1.0f), Vector2(1.0f, 1.0f),
		Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f),
		0
		);

	batch[1].AddTriangle(
		Vector2(1.0f, 1.0f), Vector2(1.0f, -1.0f), Vector2(-1.0f, -1.0f),
		Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f),
		0
		);
	batch[1].AddTriangle(
		Vector2(-1.0f, -1.0f), Vector2(-1.0f, 1.0f), Vector2(1.0f, 1.0f),
		Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f),
		0
		);

	Shader vs(L"../Shader/Texture.hlsl", VS);
	Shader ps(L"../Shader/Texture.hlsl", PS);
	vs.Initialize();
	ps.Initialize();

	VertexFormat format(vs.GetByteCode(), VERTEX2D);
	format.Initialize();

	WinViewport* viewport[] = {
		new WinViewport(nullptr, 100, 100, 500, 500),
		new WinViewport(nullptr, 600, 100, 500, 500)
	};

	viewport[0]->Initialize();
	viewport[0]->Show();

	viewport[1]->Initialize();
	viewport[1]->Show();

	RenderContext context[2] = {
		RenderContext(&batch[0], &vs, &ps, &format),
		RenderContext(&batch[1], &vs, &ps, &format)
	};

	while (true)
	{
		Sleep(16);

		WindowDevice::Update();

		viewport[0]->Render(&context[0]);
		viewport[1]->Render(&context[1]);
	}

	WindowDevice::Uninitialize();
	RenderEngine::Uninitialize();
}