#pragma once

#include "Graphics/GraphicsDef.h"
#include "Graphics/GPUObject.h"
#include "Container/Vector.h"
#include "Container/RefCounted.h"

namespace FlagGG
{
	namespace Graphics
	{
		class VertexBuffer : public GPUObject, public Container::RefCounted
		{
		public:
			bool IsValid() override;

			bool SetSize(uint32_t vertexCount, const Container::PODVector<VertexElement>& vertexElements);

			void* Lock(uint32_t start, uint32_t count);

			void Unlock();

			uint32_t GetVertexSize() const;

			uint32_t GetVertexCount() const;

			const Container::PODVector<VertexElement>& GetElements() const;

			static Container::PODVector<VertexElement> GetElements(uint32_t elementMask);

			static uint32_t GetVertexSize(const Container::PODVector<VertexElement>& elements);

		protected:
			void Initialize() override;

			void UpdateOffset();

		private:
			Container::PODVector<VertexElement> vertexElements_;

			uint32_t vertexSize_{ 0 };
			uint32_t vertexCount_{ 0 };
		};
	}
}
