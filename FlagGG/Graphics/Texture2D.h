#ifndef __TEXTURE2D__
#define __TEXTURE2D__

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

			~Texture2D() override = default;

			bool SetSize(int32_t width, int32_t height, uint32_t format, TextureUsage usage = TEXTURE_STATIC, int32_t multiSample = 1, bool autoResolve = true);

			bool SetData(uint32_t level, int32_t x, int32_t y, int32_t width, int32_t height, const void* data);

			bool SetData(FlagGG::Resource::Image* image, bool useAlpha = false);

			bool GetData(uint32_t level, void* dest);

			Container::SharedPtr<FlagGG::Resource::Image> GetImage();

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

			bool Create() override;

			void Release();
		};
	}
}

#endif