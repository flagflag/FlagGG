#ifndef __TEXTURE__
#define __TEXTURE__

#include "Export.h"

#include "Graphics/GPUObject.h"
#include "Graphics/RenderSurface.h"
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

			void Initialize() override;

			// 设置贴图的最大lod
			void SetNumLevels(UInt32 levels);

			void SetNumLayers(UInt32 layers);

			Int32 GetWidth() const;

			Int32 GetHeight() const;

			Int32 GetDepth() const;

			Int32 GetLevelWidth(UInt32 level) const;

			Int32 GetLevelHeight(UInt32 level) const;

			Int32 GetLevelDepth(UInt32 level) const;

			UInt32 GetRowDataSize(Int32 width) const;

			bool IsCompressed() const;

			UInt32 GetComponents() const;

			virtual RenderSurface* GetRenderSurface() const { return nullptr; }

			virtual RenderSurface* GetRenderSurface(UInt32 index) const { return nullptr; }

			friend class RenderEngine;

		protected:

			virtual bool Create() { return false; }
			void Release();

			static UInt32 CheckMaxLevels(Int32 width, Int32 height, UInt32 requestedLevels);
			static UInt32 CheckMaxLevels(Int32 width, Int32 height, Int32 depth, UInt32 requestedLevels);
			static UInt32 GetSRGBFormat(UInt32 format);
			static UInt32 GetDSVFormat(UInt32 format);
			static UInt32 GetSRVFormat(UInt32 format);

		protected:
			UInt32 levels_{ 0 };
			UInt32 requestedLevels_{ 0 };

			UInt32 layers_{ 0u };

			Int32 width_{ 0 };
			Int32 height_{ 0 };
			Int32 depth_{ 0 };
			UInt32 format_{ 0 };
			Int32 multiSample_{ 1 };
			bool autoResolve_{ false };

			bool sRGB_{ false };

			TextureUsage usage_{ TEXTURE_STATIC };

			UInt32 mipsToSkip_[MAX_TEXTURE_QUALITY_LEVELS];
		};
	}
}

#endif