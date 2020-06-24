#pragma once

#include "Graphics/GPUBuffer.h"

namespace FlagGG
{
	namespace Graphics
	{
		class FlagGG_API IndexBuffer : public GPUBuffer
		{
		public:
			bool SetSize(UInt32 indexSize, UInt32 indexCount);

			UInt32 GetIndexSize() const;

			UInt32 GetIndexCount() const;

		protected:
			void Create(const bgfx::Memory* mem, bool dynamic) override;

			void UpdateBuffer(const bgfx::Memory* mem) override;

		protected:
			UInt32 indexSize_{ 0 };
			UInt32 indexCount_{ 0 };
		};
	}
}

