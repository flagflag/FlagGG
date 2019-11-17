#pragma once

#include "Container/Vector.h"
#include "Container/Ptr.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"

namespace FlagGG
{
	namespace Graphics
	{
		class FlagGG_API Geometry : public Container::RefCounted
		{
		public:
			void SetPrimitiveType(PrimitiveType type);
			
			void SetVertexBuffer(uint32_t index, VertexBuffer* vertexBuffer);

			void SetIndexBuffer(IndexBuffer* indexBuffer);

			void SetDataRange(uint32_t indexStart, uint32_t indexCount);

			void SetLodDistance(float distance);

			const Container::Vector<Container::SharedPtr<VertexBuffer>>& GetVertexBuffers() const;

			VertexBuffer* GetVertexBuffer(uint32_t index) const;

			IndexBuffer* GetIndexBuffer() const;

			float GetLodDistance() const;

			PrimitiveType GetPrimitiveType() const;

			uint32_t GetIndexStart() const;

			uint32_t GetIndexCount() const;

		private:
			PrimitiveType primitiveType_;

			Container::Vector<Container::SharedPtr<VertexBuffer>> vertexBuffers_;

			Container::SharedPtr<IndexBuffer> indexBuffer_;

			uint32_t indexStart_;
			uint32_t indexCount_;
			uint32_t vertexStart_;
			uint32_t vertexCount_;
			
			float lodDistance_;
		};
	}
}
