#pragma once

#include "Graphics/GPUBuffer.h"

namespace FlagGG
{
	namespace Graphics
	{
		class FlagGG_API ConstantBuffer : public GPUBuffer
		{
		public:
			UInt32 GetBindFlags() override;
		};
	}
}
