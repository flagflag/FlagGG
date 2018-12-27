#include "Graphics/WinViewport.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Batch3D.h"
#include "Graphics/Shader.h"
#include "Graphics/VertexFormat.h"
#include "Graphics/Camera.h"
#include "Math/Matrix4.h"

#include <fstream>

using namespace FlagGG::Graphics;
using namespace FlagGG::Math;

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
	//std::ifstream stream;
	//stream.open("../demo/demo2/vertex.txt", std::ios::in);
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
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		buffer[i] *= 0.5f;
	//	}
	//	
	//	batch.AddBlob(buffer, 8 * 4);
	//}

	//stream.close();

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

struct MatrixBufferType
{
	Matrix4 world;
	Matrix4 view;
	Matrix4 projection;
};

class InputDemo : public Input
{
public:
	InputDemo() :
		camera_(AIRCRAFT)
	{
		camera_.Walk(-5.0);
		camera_.Fly(1.0);
		camera_.Strafe(1.0f);
		camera_.Pitch(-0.005f);
		camera_.Yaw(0.0025f);
	}

	void OnKeyDown(KeyState* keyState, unsigned keyCode) override
	{
		printf("OnKeyDown keyCode = %u\n", keyCode);

		if (keyCode == 'W' || keyCode == 'w')
		{
			camera_.Walk(0.1f);
		}

		if (keyCode == 'S' || keyCode == 's')
		{
			camera_.Walk(-0.1f);
		}

		if (keyCode == 'A' || keyCode == 'a')
		{
			camera_.Strafe(-0.1f);
		}

		if (keyCode == 'D' || keyCode == 'd')
		{
			camera_.Strafe(0.1f);
		}

		if (keyCode == 'R' || keyCode == 'r')
		{
			camera_.Fly(0.1f);
		}

		if (keyCode == 'F' || keyCode == 'f')
		{
			camera_.Fly(-0.1f);
		}



		if (keyCode == VK_UP)
		{
			camera_.Pitch(0.0025);
		}

		if (keyCode == VK_DOWN)
		{
			camera_.Pitch(-0.0025);
		}

		if (keyCode == VK_LEFT)
		{
			camera_.Yaw(0.0025);
		}

		if (keyCode == VK_RIGHT)
		{
			camera_.Yaw(-0.0025);
		}

		if (keyCode == 'N' || keyCode == 'n')
		{
			camera_.Roll(0.0025);
		}

		if (keyCode == 'M' || keyCode == 'm')
		{
			camera_.Roll(-0.0025);
		}
	}

	void OnKeyUp(KeyState* keyState, unsigned keyCode) override
	{
		printf("OnKeyUp keyCode = %u\n", keyCode);
	}

	void OnMouseDown(KeyState* keyState, MouseKey mouseKey) override
	{
		if (mouseKey == MOUSE_LEFT)
		{
			mouseDown_ = true;
		}
	}

	void OnMouseUp(KeyState* keyState, MouseKey mouseKey) override
	{
		if (mouseKey == MOUSE_LEFT)
		{
			mouseDown_ = false;
		}
	}

	void OnMouseMove(KeyState* keyState, const Vector2& delta) override
	{
		if (mouseDown_)
		{
			printf("mouse delta (%lf, %lf)\n", delta.x_, delta.y_);
		}
	}

	Camera camera_;

private:
	bool mouseDown_{ false };
};

void Demo2Run()
{
	WindowDevice::Initialize();
	RenderEngine::Initialize();

	Texture2D texture(L"../Demo/Demo2/texture.dds");
	texture.Initialize();

	Batch3D batch(DRAW_TRIANGLE, &texture, nullptr);
	Batch3D grid(DRAW_LINE, &texture, nullptr); //地板的batch
	LoadVertexData(batch, grid);

	Shader vs(L"../Shader/3D_VS.hlsl", VS);
	Shader ps(L"../Shader/3D_PS.hlsl", PS);
	vs.Initialize();
	ps.Initialize();

	VertexFormat format(vs.GetByteCode(), VERTEX3D);
	format.Initialize();

	WinViewport viewport(nullptr, 100, 100, 500, 500);

	viewport.Initialize();
	viewport.Show();

	InputDemo inputDemo;
	viewport.SetInput(&inputDemo);

	RenderContext context(&grid, &vs, &ps, &format);
	context.batchs_.emplace_back(&batch);

	WindowDevice::RegisterWinMessage(&viewport);

	while (true)
	{
		Sleep(16);

		WindowDevice::Update();

		RenderEngine::UpdateMatrix(&inputDemo.camera_);

		viewport.Render(&context);
	}

	WindowDevice::UnregisterWinMessage(&viewport);

	WindowDevice::Uninitialize();
	RenderEngine::Uninitialize();
}