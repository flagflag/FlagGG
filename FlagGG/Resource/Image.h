#pragma once

#include "Resource/Resource.h"
#include "Container/ArrayPtr.h"
#include "Container/Vector.h"
#include "Math/Color.h"
#include "Math/Rect.h"

#include <stdint.h>

namespace FlagGG
{
	namespace Resource
	{
		enum CompressedFormat
		{
			CF_NONE = 0,
			CF_RGBA,
			CF_DXT1,
			CF_DXT3,
			CF_DXT5,
			CF_ETC1,
			CF_PVRTC_RGB_2BPP,
			CF_PVRTC_RGBA_2BPP,
			CF_PVRTC_RGB_4BPP,
			CF_PVRTC_RGBA_4BPP,
		};

		struct FlagGG_API CompressedLevel
		{
			bool Decompress(uint8_t* dest);

			uint8_t* data_{};

			CompressedFormat format_{ CF_NONE };

			int32_t width_{};

			int32_t height_{};

			int32_t depth_{};

			uint32_t blockSize_{};

			uint32_t dataSize_{};

			uint32_t rowSize_{};

			uint32_t rows_{};
		};

		class FlagGG_API Image : public FlagGG::Resource::Resource
		{
		public:
			explicit Image(Core::Context* context);

			~Image() override;

			bool SetSize(int width, int height, unsigned components);

			bool SetSize(int width, int height, int depth, unsigned components);

			void SetData(const unsigned char* pixelData);

			void SetPixel(int x, int y, const Math::Color& color);

			void SetPixel(int x, int y, int z, const Math::Color& color);

			void SetPixelInt(int x, int y, unsigned uintColor);

			void SetPixelInt(int x, int y, int z, unsigned uintColor);

			bool FlipHorizontal();

			bool FlipVertical();

			bool Resize(int width, int height);

			void Clear(const Math::Color& color);

			void ClearInt(unsigned uintColor);

			bool SaveBMP(const Container::String& fileName) const;

			bool SavePNG(const Container::String& fileName) const;

			bool SaveTGA(const Container::String& fileName) const;

			bool SaveJPG(const Container::String& fileName, int quality) const;

			bool SaveDDS(const Container::String& fileName) const;

			bool IsCubemap() const { return cubemap_; }

			bool IsArray() const { return array_; }

			bool IsSRGB() const { return sRGB_; }

			Math::Color GetPixel(int x, int y) const;

			Math::Color GetPixel(int x, int y, int z) const;

			unsigned GetPixelInt(int x, int y) const;

			unsigned GetPixelInt(int x, int y, int z) const;

			Math::Color GetPixelBilinear(float x, float y) const;

			Math::Color GetPixelTrilinear(float x, float y, float z) const;

			int GetWidth() const { return width_; }

			int GetHeight() const { return height_; }

			int GetDepth() const { return depth_; }

			unsigned GetComponents() const { return components_; }

			unsigned char* GetData() const { return data_; }

			bool IsCompressed() const;

			CompressedFormat GetCompressedFormat() const { return compressedFormat_; }

			unsigned GetNumCompressedLevels() const { return numCompressedLevels_; }

			Container::SharedPtr<Image> GetNextLevel() const;

			Container::SharedPtr<Image> GetNextSibling() const { return nextSibling_; }

			Container::SharedPtr<Image> ConvertToRGBA() const;

			CompressedLevel GetCompressedLevel(unsigned index) const;

			Image* GetSubimage(const Math::IntRect& rect) const;

			void PrecalculateLevels();

			bool HasAlphaChannel() const;

			bool SetSubimage(const Image* image, const Math::IntRect& rect);

			void CleanupLevels();

			void GetLevels(Container::PODVector<Image*>& levels);

			void GetLevels(Container::PODVector<const Image*>& levels) const;

		protected:
			bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

			bool EndLoad() override;

			bool Save(IOFrame::Buffer::IOBuffer* stream) const;

			static unsigned char* GetImageData(IOFrame::Buffer::IOBuffer* stream, int& width, int& height, unsigned& components);

			static void FreeImageData(unsigned char* pixelData);

		private:
			int32_t width_{ 0 };

			int32_t height_{ 0 };

			int32_t depth_{ 0 };

			uint32_t components_{ 0 };

			uint32_t numCompressedLevels_{ 0 };

			bool cubemap_{ false };

			bool array_{ false };

			bool sRGB_{ false };

			CompressedFormat compressedFormat_{ CF_NONE };

			Container::SharedArrayPtr<uint8_t> data_;

			Container::SharedPtr<Image> nextLevel_;

			Container::SharedPtr<Image> nextSibling_;
		};
	}
}
