#pragma once

#include "GPUBuffer.h"
#include "Container/Vector.h"

namespace FlagGG
{
	namespace Graphics
	{
		class FlagGG_API VertexBuffer : public GPUBuffer
		{
		public:
			uint32_t GetBindFlags() override;

			bool SetSize(uint32_t vertexCount, const Container::PODVector<VertexElement>& vertexElements);

			uint32_t GetVertexSize() const;

			uint32_t GetVertexCount() const;

			const Container::PODVector<VertexElement>& GetElements() const;

			static Container::PODVector<VertexElement> GetElements(uint32_t elementMask);

			static uint32_t GetVertexSize(const Container::PODVector<VertexElement>& elements);

		protected:
			void UpdateOffset();

		private:
			Container::PODVector<VertexElement> vertexElements_;

			uint32_t vertexSize_{ 0 };
			uint32_t vertexCount_{ 0 };
		};
	}
}
