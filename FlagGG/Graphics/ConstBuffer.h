#pragma once

#include "Graphics/GPUBuffer.h"

namespace FlagGG
{
	namespace Graphics
	{
		class ConstBuffer : GPUBuffer
		{
		public:
			uint32_t GetBindFlags() override;
		};
	}
}
