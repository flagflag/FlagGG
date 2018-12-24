#include "Graphics/WinViewport.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Batch3D.h"

#include <fstream>

using namespace FlagGG::Graphics;
using namespace FlagGG::Math;

void LoadVertexData(Batch3D& bath)
{
	std::ifstream stream;
	stream.open("../Demo/Demo2/vertex.txt", std::ios::in);
	if (!stream.is_open())
	{
		puts("load vertex data failed.");

		return;
	}

	Vector3 v[3], n[3];
	Vector2 uv[3];

	while (!stream.eof())
	{
		for (int i = 0; i < 3; ++i)
		{
			stream >> v[i].x_ >> v[i].y_ >> v[i].z_;
		}

		for (int i = 0; i < 2; ++i)
		{
			stream >> uv[i].x_ >> uv[i].y_;
		}

		for (int i = 0; i < 3; ++i)
		{
			stream >> n[i].x_ >> n[i].y_ >> n[i].z_;
		}

		
	}
}

void Demo2Run()
{
	WindowDevice::Initialize();
	RenderEngine::Initialize();

	Texture2D texture(L"../Demo/Demo2/texture.png");
	texture.Initialize();

	Batch3D bath(&texture, nullptr);
	LoadVertexData(bath);

	WinViewport viewport(nullptr, 100, 100, 500, 500);

	viewport.Initialize();
	viewport.Show();

	while (true)
	{
		Sleep(16);

		WindowDevice::Update();

		//viewport.Render(&bath);
	}

	WindowDevice::Uninitialize();
	RenderEngine::Uninitialize();
}