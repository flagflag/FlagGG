#pragma once

#include "Graphics/GPUBuffer.h"

namespace FlagGG
{
	namespace Graphics
	{
		class IndexBuffer : public GPUBuffer
		{
		public:
			bool SetSize(uint32_t indexSize, uint32_t indexCount);

			uint32_t GetIndexSize() const;

			uint32_t GetIndexCount() const;

		protected:

			uint32_t indexSize_{ 0 };
			uint32_t indexCount_{ 0 };
		};
	}
}

