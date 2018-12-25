#include "Graphics/WinViewport.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Batch3D.h"
#include "Graphics/Shader.h"
#include "Graphics/VertexFormat.h"

#include <fstream>

using namespace FlagGG::Graphics;
using namespace FlagGG::Math;

void LoadVertexData(Batch3D& batch)
{
	//std::ifstream stream;
	//stream.open("../Demo/Demo2/vertex.txt", std::ios::in);
	//if (!stream.is_open())
	//{
	//	puts("load vertex data failed.");

	//	return;
	//}

	//while (!stream.eof())
	//{
	//	float buffer[8];
	//	for (int i = 0; i < 8; ++i)
	//	{
	//		stream >> buffer[i];
	//	}
	//	
	//	batch.AddBlob(buffer, 8 * 4);
	//}

	//stream.close();

	//batch.AddTriangle(
	//	Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f, -1.0f, 0.0f), Vector3(-1.0f, -1.0f, 0.0f),
	//	Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f),
	//	Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f),
	//	0);
	batch.AddTriangle(
		Vector3(1.0f, 1.0f, 0.0f), Vector3(-1.0f, -1.0f, 0.0f), Vector3(1.0f, -1.0f, 0.0f),
		Vector2(1.0f, 1.0f), Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f),
		Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f),
		0);
}

void Demo2Run()
{
	WindowDevice::Initialize();
	RenderEngine::Initialize();

	Texture2D texture(L"../Demo/Demo2/texture.dds");
	texture.Initialize();

	Batch3D batch(&texture, nullptr);
	LoadVertexData(batch);

	Shader vs(L"../Shader/3D_VS.hlsl", VS);
	Shader ps(L"../Shader/3D_PS.hlsl", PS);
	vs.Initialize();
	ps.Initialize();

	VertexFormat format(vs.GetByteCode(), VERTEX3D);
	format.Initialize();

	WinViewport viewport(nullptr, 100, 100, 500, 500);

	viewport.Initialize();
	viewport.Show();

	RenderContext context(&batch, &vs, &ps, &format);

	while (true)
	{
		Sleep(16);

		WindowDevice::Update();

		viewport.Render(&context);
	}

	WindowDevice::Uninitialize();
	RenderEngine::Uninitialize();
}