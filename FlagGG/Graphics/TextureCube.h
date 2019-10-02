#pragma once

#include "Graphics/GraphicsDef.h"
#include "Graphics/Texture.h"
#include "Resource/Image.h"

#include <string>

namespace FlagGG
{
	namespace Graphics
	{
		class FlagGG_API TextureCube : public Texture
		{
		public:
			TextureCube(Core::Context* context);

			bool SetSize(uint32_t size, uint32_t format, TextureUsage usage = TEXTURE_STATIC, int32_t multiSample = 1);

			bool SetData(CubeMapFace face, uint32_t level, int32_t x, int32_t y, int32_t width, int32_t height, const void* data);

			bool SetData(CubeMapFace face, FlagGG::Resource::Image* image, bool useAlpha = false);

			bool GetData(CubeMapFace face, uint32_t level, void* dest);

			Container::SharedPtr<FlagGG::Resource::Image> GetImage(CubeMapFace face);

			RenderSurface* GetRenderSurface() const override;

			RenderSurface* GetRenderSurface(uint32_t index) const override;

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

			bool Create() override;

		private:
			Container::SharedPtr<RenderSurface> renderSurfaces_[MAX_CUBEMAP_FACES];

			uint32_t faceMemoryUse_[MAX_CUBEMAP_FACES];
		};
	}
}
