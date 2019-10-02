#pragma once

#include "Graphics/Texture.h"
#include "Resource/Image.h"

namespace FlagGG
{
	namespace Graphics
	{
		class FlagGG_API Texture3D : public Texture
		{
		public:
			Texture3D(Core::Context* context);

			bool SetSize(int32_t width, int32_t height, int32_t depth, uint32_t format, TextureUsage usage = TEXTURE_STATIC);

			bool SetData(uint32_t level, int32_t x, int32_t y, int32_t z, int32_t width, int32_t height, int32_t depth, const void* data);

			bool SetData(FlagGG::Resource::Image* image, bool useAlpha = false);

			bool GetData(uint32_t level, void* dest);

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

			bool Create() override;
		};
	}
}
