#pragma once

#include "Graphics/GPUBuffer.h"

namespace FlagGG
{
	namespace Graphics
	{
		class ConstBuffer : public GPUBuffer
		{
		public:
			uint32_t GetBindFlags() override;
		};
	}
}
