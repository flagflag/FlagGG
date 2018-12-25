#include "Graphics/WinViewport.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Batch3D.h"
#include "Graphics/Shader.h"
#include "Graphics/VertexFormat.h"
#include "Math/Matrix4.h"

#include <fstream>

using namespace FlagGG::Graphics;
using namespace FlagGG::Math;

void LoadVertexData(Batch3D& batch)
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

	batch.AddTriangle(
		Vector3(0.5f, 0.5f, 0.0f), Vector3(0.5f, -0.5f, 0.0f), Vector3(-0.5f, -0.5f, 0.0f),
		Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f),
		Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f),
		0);
	batch.AddTriangle(
		Vector3(-0.5f, -0.5f, 0.0f), Vector3(-0.5f, 0.5f, 0.0f), Vector3(0.5f, 0.5f, 0.0f),
		Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f),
		Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f),
		0);
}

struct MatrixBufferType
{
	Matrix4 world;
	Matrix4 view;
	Matrix4 projection;
};

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

	{
		ID3D11Buffer* buffer = nullptr;
		D3D11_BUFFER_DESC bufferDesc;
		memset(&bufferDesc, 0, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(MatrixBufferType);
		HRESULT hr = RenderEngine::GetDevice()->CreateBuffer(&bufferDesc, NULL, &buffer);
		if (hr != 0)
		{
			puts("CreateBuffer failed.");

			SAFE_RELEASE(buffer);

			return;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		unsigned int bufferNumber;

		RenderEngine::GetDeviceContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		memset(mappedResource.pData, 0, sizeof(MatrixBufferType));
		MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
		//dataPtr->world = Matrix4::MatrixTranslation(-0.5, 0.5, 0).Transpose();
		dataPtr->world = Matrix4::MatrixRotationZ(3.14f / 3.0f).Transpose();
		dataPtr->view = Matrix4::IDENTITY.Transpose();
		dataPtr->projection = Matrix4::IDENTITY.Transpose();

		RenderEngine::GetDeviceContext()->Unmap(buffer, 0);

		RenderEngine::GetDeviceContext()->VSSetConstantBuffers(0, 1, &buffer);
	}

	while (true)
	{
		Sleep(16);

		WindowDevice::Update();

		viewport.Render(&context);
	}

	WindowDevice::Uninitialize();
	RenderEngine::Uninitialize();
}