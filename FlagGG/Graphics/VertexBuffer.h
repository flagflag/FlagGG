#pragma once

#include "Graphics/GraphicsDef.h"
#include "Graphics/GPUObject.h"
#include "Container/Vector.h"

namespace FlagGG
{
	namespace Graphics
	{
		struct VertexElement
		{
			VertexElement();

			VertexElement(VertexElementType vertexElementType, VertexElementSemantic vertexElementSemantic);

			VertexElementType		vertexElementType_;
			VertexElementSemantic	vertexElementSemantic_;
		};

		class VertexBuffer : public GPUObject
		{
		public:
			bool IsValid() override;

			bool SetSize(uint32_t vertexCount, const Container::PODVector<VertexElement>& vertexElements);

			void* Lock(uint32_t start, uint32_t count);

			void Unlock();

		protected:
			void Initialize() override;

		private:
			Container::PODVector<VertexElement> vertexElements_;

			uint32_t vertexSize_{ 0 };
			uint32_t vertexCount_{ 0 };
		};
	}
}
