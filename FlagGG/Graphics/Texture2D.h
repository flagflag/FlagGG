#pragma once

#include "Graphics/Texture.h"
#include "Resource/Image.h"

#include <string>

namespace FlagGG
{
	namespace Graphics
	{
#define USE_DDS

		class FlagGG_API Texture2D : public Texture
		{
		public:
			Texture2D(Core::Context* context);

			bool SetSize(Int32 width, Int32 height, UInt32 format, TextureUsage usage = TEXTURE_STATIC, Int32 multiSample = 1, bool autoResolve = true);

			bool SetData(UInt32 level, Int32 x, Int32 y, Int32 width, Int32 height, const void* data);

			bool SetData(FlagGG::Resource::Image* image, bool useAlpha = false);

			bool GetData(UInt32 level, void* dest);

			Container::SharedPtr<FlagGG::Resource::Image> GetImage();

			RenderSurface* GetRenderSurface() const override;

			RenderSurface* GetRenderSurface(UInt32 index) const override;

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

			bool Create() override;

		private:
			Container::SharedPtr<RenderSurface> renderSurface_;
		};
	}
}
