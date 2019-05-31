#include "Graphics/VertexBuffer.h"
#include "Log.h"

namespace FlagGG
{
	namespace Graphics
	{
		bool VertexBuffer::SetSize(uint32_t vertexCount, const Container::PODVector<VertexElement>& vertexElements)
		{
			vertexSize_ = GetVertexSize(vertexElements);
			vertexCount_ = vertexCount;
			vertexElements_ = vertexElements;

			if (!GPUBuffer::SetSize(vertexSize_ * vertexCount_))
			{
				return false;
			}

			UpdateOffset();
		}

		void VertexBuffer::UpdateOffset()
		{
			uint32_t offset = 0;
			for (auto& element : vertexElements_)
			{
				element.offset_ = offset;
				offset += VERTEX_ELEMENT_TYPE_SIZE[element.vertexElementType_];
			}
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
