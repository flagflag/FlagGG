#include "Graphics/VertexFormat.h"
#include "Graphics/RenderEngine.h"
#include "Container/Vector.h"
#include "Log.h"

#include <vector>

namespace FlagGG
{

static const DXGI_FORMAT d3dElementFormats[] =
{
	DXGI_FORMAT_R32_SINT,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R8G8B8A8_UINT,
	DXGI_FORMAT_R8G8B8A8_UNORM
};

VertexFormat::VertexFormat(Shader* VSShader, VertexBuffer** vertexBuffer)
{
	PODVector<D3D11_INPUT_ELEMENT_DESC> elementDescs;
	UInt32 preSize = 0;

	for (UInt32 i = 0; i < MAX_VERTEX_BUFFER_COUNT && vertexBuffer[i]; ++i)
	{
		const PODVector<VertexElement>& elements = vertexBuffer[i]->GetElements();
		bool isExisting = false;
				
		for (const auto& element : elements)
		{
			const char* semName = VERTEX_ELEMENT_SEM_NAME[element.vertexElementSemantic_];

			for (UInt32 j = 0; j < preSize; ++j)
			{
				if (elementDescs[j].SemanticName == semName && elementDescs[j].SemanticIndex == element.index_)
				{
					isExisting = true;
					elementDescs[j].InputSlot = i;
					elementDescs[j].AlignedByteOffset = element.offset_;
					elementDescs[j].InputSlotClass = element.perInstance_ ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
					break;
				}
			}

			if (isExisting)
				continue;

			D3D11_INPUT_ELEMENT_DESC desc;
			desc.SemanticName = semName;
			desc.SemanticIndex = element.index_;
			desc.Format = d3dElementFormats[element.vertexElementType_];
			desc.InputSlot = i;
			desc.AlignedByteOffset = element.offset_;
			desc.InputSlotClass = element.perInstance_ ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = element.perInstance_ ? 1 : 0;
			elementDescs.Push(desc);
		}

		preSize = elementDescs.Size();
	}

	if (elementDescs.Empty())
		return;

	ID3DBlob* shaderBlob = VSShader->GetByteCode();
	if (!shaderBlob)
	{
		FLAGGG_LOG_ERROR("Failed to create vertext layout, shader code is nullptr.");
		return;
	}

	ID3D11InputLayout* inputLayout;
	HRESULT hr = RenderEngine::Instance()->GetDevice()->CreateInputLayout(&elementDescs[0], elementDescs.Size(),
		shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &inputLayout);
	if (FAILED(hr))
	{
		SAFE_RELEASE(inputLayout);
		FLAGGG_LOG_ERROR("Failed to create vertex layout.");
		return;
	}

	ResetHandler(inputLayout);
}

void VertexFormat::Initialize()
{
}

bool VertexFormat::IsValid()
{
	return GetHandler() != nullptr;
}

}
