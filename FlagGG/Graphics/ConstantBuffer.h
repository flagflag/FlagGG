#pragma once

#include "Graphics/GPUBuffer.h"

namespace FlagGG
{
	namespace Graphics
	{
		class FlagGG_API ConstantBuffer : public GPUBuffer
		{
		public:
			void Create(const bgfx::Memory* mem, bool dynamic) override;

			void UpdateBuffer(const bgfx::Memory* mem) override;
		};
	}
}
