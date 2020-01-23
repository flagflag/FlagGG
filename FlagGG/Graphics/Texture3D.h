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

			bool SetSize(Int32 width, Int32 height, Int32 depth, UInt32 format, TextureUsage usage = TEXTURE_STATIC);

			bool SetData(UInt32 level, Int32 x, Int32 y, Int32 z, Int32 width, Int32 height, Int32 depth, const void* data);

			bool SetData(FlagGG::Resource::Image* image, bool useAlpha = false);

			bool GetData(UInt32 level, void* dest);

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

			bool Create() override;
		};
	}
}
