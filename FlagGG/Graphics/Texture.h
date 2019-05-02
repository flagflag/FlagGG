#ifndef __TEXTURE__
#define __TEXTURE__

#include "Export.h"

#include "Graphics/GPUObject.h"
#include "Graphics/RenderTarget.h"
#include "Resource/Resource.h"

#include <stdint.h>

namespace FlagGG
{
	namespace Graphics
	{
		static const int MAX_TEXTURE_QUALITY_LEVELS = 3;

		enum TextureUsage
		{
			TEXTURE_STATIC = 0,
			TEXTURE_DYNAMIC,
			TEXTURE_RENDERTARGET,
			TEXTURE_DEPTHSTENCIL
		};

		//Base Class, ID3D11Texture2D or ID3D11Texture3D
		class FlagGG_API Texture : public GPUObject, public Resource::Resource
		{
		public:
			Texture(Core::Context* context);

			~Texture() override;

			bool IsValid() override;

			void SetNumLevels(uint32_t levels);

			int32_t GetWidth() const;

			int32_t GetHeight() const;

			int32_t GetDepth() const;

			int32_t GetLevelWidth(uint32_t level) const;

			int32_t GetLevelHeight(uint32_t level) const;

			int32_t GetLevelDepth(uint32_t level) const;

			uint32_t GetRowDataSize(int32_t width) const;

			bool IsCompressed() const;

			friend class WinViewport;

		protected:
			void Initialize() override;

			virtual bool Create() { return false; }

			static uint32_t CheckMaxLevels(int32_t width, int32_t height, uint32_t requestedLevels);
			static uint32_t Texture::GetSRGBFormat(uint32_t format);
			static uint32_t GetDSVFormat(uint32_t format);
			static uint32_t GetSRVFormat(uint32_t format);

		protected:
			uint32_t levels_{ 0 };
			uint32_t requestedLevels_{ 0 };

			int32_t width_{ 0 };
			int32_t height_{ 0 };
			int32_t depth_{ 0 };
			uint32_t format_{ 0 };
			int32_t multiSample_{ 1 };
			bool autoResolve_{ false };

			bool sRGB_{ false };

			TextureUsage usage_{ TEXTURE_STATIC };

			Container::SharedPtr<RenderTarget> renderTarget_;

			uint32_t mipsToSkip_[MAX_TEXTURE_QUALITY_LEVELS];

			ID3D11Resource* resolveTexture_{ nullptr };

			ID3D11ShaderResourceView* shaderResourceView_{ nullptr };

			ID3D11SamplerState* sampler_{ nullptr };
		};
	}
}

#endif