#pragma once
#include "Graphics/GraphicsDef.h"
#include "Graphics/GPUObject.h"
#include "Container/RefCounted.h"

namespace FlagGG
{
	namespace Graphics
	{
		class GPUBuffer : public GPUObject, public Container::RefCounted
		{
		public:
			bool IsValid() override;

			bool SetSize(uint32_t byteCount);

			void* Lock(uint32_t start, uint32_t count);

			void Unlock();

		protected:
			void Initialize() override;
		};
	}
}
