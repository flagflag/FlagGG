#include "VertexFormat.h"
#include "RenderEngine.h"

#include <vector>

namespace FlagGG
{
	namespace Graphics
	{
		VertexFormat::VertexFormat(ID3DBlob* shaderCode, VertexType vertexType) :
			shaderCode_(shaderCode),
			vertexType_(vertexType)
		{
		}

		void VertexFormat::Initialize()
		{
			if (shaderCode_)
			{
				std::vector<D3D11_INPUT_ELEMENT_DESC> inputDesc =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};

				if (vertexType_ == VERTEX3D)
				{
					D3D11_INPUT_ELEMENT_DESC Normal =
					{
						"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0
					};

					inputDesc.emplace_back(Normal);
				}

				unsigned int totalLayoutElements = inputDesc.size();

				ID3D11InputLayout* inputLayout;

				HRESULT hr = RenderEngine::GetDevice()->CreateInputLayout(
					&inputDesc[0],
					totalLayoutElements,
					shaderCode_->GetBufferPointer(),
					shaderCode_->GetBufferSize(),
					&inputLayout
					);
				if (hr != 0)
				{
					puts("CreateInputLayout failed.");

					SAFE_RELEASE(inputLayout);

					return;
				}

				ResetHandler(inputLayout);
			}
		}

		bool VertexFormat::IsValid()
		{
			return GetHandler() != nullptr;
		}
	}
}
