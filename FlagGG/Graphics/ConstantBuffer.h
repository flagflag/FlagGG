#pragma once

#include "Graphics/GPUBuffer.h"

namespace FlagGG
{
	namespace Graphics
	{
		class FlagGG_API ConstantBuffer : public GPUBuffer
		{
		public:
			uint32_t GetBindFlags() override;
		};
	}
}
