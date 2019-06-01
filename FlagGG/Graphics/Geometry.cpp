#include "Graphics/Geometry.h"

namespace FlagGG
{
	namespace Graphics
	{
		void Geometry::SetPrimitiveType(PrimitiveType type)
		{
			primitiveType_ = type;
		}

		void Geometry::SetVertexBuffer(uint32_t index, VertexBuffer* vertexBuffer)
		{
			if (index >= vertexBuffers_.Size())
				vertexBuffers_.Resize(index + 1);

			vertexBuffers_[index] = vertexBuffer;
		}

		void Geometry::SetIndexBuffer(IndexBuffer* indexBuffer)
		{
			indexBuffer_ = indexBuffer;
		}

		void Geometry::SetDataRange(uint32_t indexStart, uint32_t indexCount)
		{
			indexStart_ = indexStart;
			indexCount_ = indexCount;
			vertexStart_ = 0;
			vertexCount_ = 0;
		}

		void Geometry::SetLodDistance(float distance)
		{
			lodDistance_ = distance;
		}

		const Container::Vector<Container::SharedPtr<VertexBuffer>>& Geometry::GetVertexBuffers() const
		{
			return vertexBuffers_;
		}

		VertexBuffer* Geometry::GetVertexBuffer(uint32_t index) const
		{
			if (index >= vertexBuffers_.Size())
				return nullptr;
			return vertexBuffers_[index];
		}

		IndexBuffer* Geometry::GetIndexBuffer() const
		{
			return indexBuffer_;
		}

		float Geometry::GetLodDistance() const
		{
			return lodDistance_;
		}

		PrimitiveType Geometry::GetPrimitiveType() const
		{
			return primitiveType_;
		}

		uint32_t Geometry::GetIndexStart() const
		{
			return indexStart_;
		}

		uint32_t Geometry::GetIndexCount() const
		{
			return indexCount_;
		}
	}
}
