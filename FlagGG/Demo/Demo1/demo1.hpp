#include "Graphics/WinViewport.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Batch2D.h"

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

	Batch2D bath[2] = {
		Batch2D(&texture[0], nullptr),
		Batch2D(&texture[1], nullptr)
	};

	bath[0].AddTriangle(
		Vector2(1.0f, 1.0f), Vector2(1.0f, -1.0f), Vector2(-1.0f, -1.0f),
		Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f),
		0
		);
	bath[0].AddTriangle(
		Vector2(-1.0f, -1.0f), Vector2(-1.0f, 1.0f), Vector2(1.0f, 1.0f),
		Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f),
		0
		);

	bath[1].AddTriangle(
		Vector2(1.0f, 1.0f), Vector2(1.0f, -1.0f), Vector2(-1.0f, -1.0f),
		Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f),
		0
		);
	bath[1].AddTriangle(
		Vector2(-1.0f, -1.0f), Vector2(-1.0f, 1.0f), Vector2(1.0f, 1.0f),
		Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f),
		0
		);

	WinViewport* viewport[] = {
		new WinViewport(nullptr, 100, 100, 500, 500),
		new WinViewport(nullptr, 600, 100, 500, 500)
	};

	viewport[0]->Initialize();
	viewport[0]->Show();

	viewport[1]->Initialize();
	viewport[1]->Show();

	while (true)
	{
		Sleep(16);

		WindowDevice::Update();

		viewport[0]->Render(&bath[0]);
		viewport[1]->Render(&bath[1]);
	}

	WindowDevice::Uninitialize();
	RenderEngine::Uninitialize();
}