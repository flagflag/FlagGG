#include "Graphics/VertexBuffer.h"
#include "Graphics/RenderEngine.h"
#include "Log.h"

namespace FlagGG
{
	namespace Graphics
	{
		bool VertexBuffer::IsValid()
		{
			return !!GetHandler();
		}

		void VertexBuffer::Initialize()
		{
		}

		bool VertexBuffer::SetSize(uint32_t vertexCount, const Container::PODVector<VertexElement>& vertexElements)
		{
			vertexSize_ = GetVertexSize(vertexElements);
			vertexCount_ = vertexCount;
			vertexElements_ = vertexElements;

			D3D11_BUFFER_DESC bufferDesc;
			memset(&bufferDesc, 0, sizeof(bufferDesc));
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //D3D11_USAGE_DEFAULT
			bufferDesc.ByteWidth = vertexSize_ * vertexCount_;

			ID3D11Buffer* buffer;
			HRESULT hr = RenderEngine::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &buffer);
			if (FAILED(hr))
			{
				SAFE_RELEASE(buffer);
				FLAGGG_LOG_ERROR("Failed to create vertex buffer.");
				return false;
			}

			ResetHandler(buffer);
		}

		void* VertexBuffer::Lock(uint32_t start, uint32_t count)
		{
			D3D11_MAPPED_SUBRESOURCE mappedData;
			memset(&mappedData, 0, sizeof mappedData);
			HRESULT hr = RenderEngine::GetDeviceContext()->Map(GetObject<ID3D11Buffer>(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			if (FAILED(hr) || !mappedData.pData)
			{
				FLAGGG_LOG_ERROR("Failed to Map buffer data.");
				return nullptr;
			}
			return mappedData.pData;
		}

		void VertexBuffer::Unlock()
		{
			RenderEngine::GetDeviceContext()->Unmap(GetObject<ID3D11Buffer>(), 0);
		}

		uint32_t VertexBuffer::GetVertexSize() const
		{
			return vertexSize_;
		}

		uint32_t VertexBuffer::GetVertexCount() const
		{
			return vertexCount_;
		}

		const Container::PODVector<VertexElement>& VertexBuffer::GetElements() const
		{
			return vertexElements_;
		}

		Container::PODVector<VertexElement> VertexBuffer::GetElements(uint32_t elementMask)
		{
			Container::PODVector<VertexElement> vertexElements;
			for (uint32_t i = 0; i < MAX_DEFAULT_VERTEX_ELEMENT; ++i)
			{
				if (elementMask & (1u << i))
				{
					vertexElements.Push(DEFAULT_VERTEX_ELEMENT[i]);
				}
			}
			return vertexElements;
		}

		uint32_t VertexBuffer::GetVertexSize(const Container::PODVector<VertexElement>& elements)
		{
			uint32_t vertexSize = 0u;
			for (const auto& element : elements)
			{
				vertexSize += VERTEX_ELEMENT_TYPE_SIZE[element.vertexElementType_];
			}
			return vertexSize;
		}
	}
}
