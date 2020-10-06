#pragma once

#include "GPUBuffer.h"
#include "Container/Vector.h"
#include "bgfx/bgfx.h"

namespace FlagGG
{
	namespace Graphics
	{
		class FlagGG_API VertexBuffer : public GPUBuffer
		{
		public:
			bool SetSize(UInt32 vertexCount, const Container::PODVector<VertexElement>& vertexElements);

			bool SetSize(UInt32 vertexCount, const bgfx::VertexLayout& layout);

			UInt32 GetVertexSize() const;

			UInt32 GetVertexCount() const;

			const Container::PODVector<VertexElement>& GetElements() const;

			static Container::PODVector<VertexElement> GetElements(UInt32 elementMask);

			static UInt32 GetVertexSize(const Container::PODVector<VertexElement>& elements);

		protected:
			void Create(const bgfx::Memory* mem, bool dynamic) override;

			void UpdateBuffer(const bgfx::Memory* mem) override;

			void UpdateOffset();

		private:
			Container::PODVector<VertexElement> vertexElements_;

			bool useBgfxLayout_{ false };
			bgfx::VertexLayout layout_;

			UInt32 vertexSize_{ 0 };
			UInt32 vertexCount_{ 0 };
		};
	}
}
