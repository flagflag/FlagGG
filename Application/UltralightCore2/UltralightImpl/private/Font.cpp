#include "Ultralight/private/Font.h"
#include "Ultralight/private/PlatformFont.h"
#include "Ultralight/private/util/RefCountedImpl.h"
#include "Ultralight/Bitmap.h"
#include "UltralightImpl/UltralightInnerType.h"

#include <Container/HashMap.h>
#include <Container/Ptr.h>
#include <Graphics/Texture2D.h>
#include <Memory/Memory.h>

using namespace FlagGG;

namespace ultralight
{

static const UInt32 PER_TEXTURE_SIZE = 1024;

static void BoxFilter(unsigned char* dest, size_t destSize, const unsigned char* src, size_t srcSize)
{
	const int filterSize = /*oversampling_*/1;

	assert(filterSize > 0);
	assert(destSize == srcSize + filterSize - 1);

	if (filterSize == 1)
	{
		memcpy(dest, src, srcSize);
		return;
	}

	// "accumulator" holds the total value of filterSize samples. We add one sample
	// and remove one sample per step (with special cases for left and right edges).
	int accumulator = 0;

	// The divide might make these inner loops slow. If so, some possible optimizations:
	// a) Turn it into a fixed-point multiply-and-shift rather than an integer divide;
	// b) Make this function a template, with the filter size a compile-time constant.

	int i = 0;

	if (srcSize < filterSize)
	{
		for (; i < srcSize; ++i)
		{
			accumulator += src[i];
			dest[i] = accumulator / filterSize;
		}

		for (; i < filterSize; ++i)
		{
			dest[i] = accumulator / filterSize;
		}
	}
	else
	{
		for (; i < filterSize; ++i)
		{
			accumulator += src[i];
			dest[i] = accumulator / filterSize;
		}

		for (; i < srcSize; ++i)
		{
			accumulator += src[i];
			accumulator -= src[i - filterSize];
			dest[i] = accumulator / filterSize;
		}
	}

	for (; i < srcSize + filterSize - 1; ++i)
	{
		accumulator -= src[i - filterSize];
		dest[i] = accumulator / filterSize;
	}
}

class FontImpl : public Font, public RefCountedImpl<FontImpl>
{
public:
	REF_COUNTED_IMPL(FontImpl);

	struct PlatformGlyphInfo
	{
		bool isValid_{};
		double advance_{};
		double width_{};
		double height_{};
		double bearing_{};

		RefPtr<Bitmap> bitmap_;
		Point offset_;
		float lsbDelta_{};
		float rsbDelta_{};

		Texture2D* texture_;
		Vector4 uv_;
	};

	FontImpl(void* platform_data, uint64_t font_file_hash, float font_size)
		: fontSize_(font_size)
		, deviceScale_(1.0f)
	{
		glyphSize_ = fontSize_ + 4;
		perTextureSize_ = glyphSize_ * 64;

		platformFont_ = platform_font_factory()->CreatePlatformFont(platform_data);
		platformFont_->SetFontSize(font_size);
	}

	~FontImpl() override
	{
		if (platformFont_)
		{
			platform_font_factory()->DestroyPlatformFont(platformFont_);
			platformFont_ = nullptr;
		}
	}

	// Font size in pixels
	virtual float font_size() const override
	{
		return fontSize_;
	}

	virtual float device_scale_hint() const override
	{
		return deviceScale_;
	}

	virtual void set_device_scale_hint(float scale) override
	{
		deviceScale_ = scale;
	}

	virtual bool HasGlyph(uint32_t index) const override
	{
		return GetGlyphInfo(index).isValid_;
	}

	// Pre-scaled to layout units
	virtual double GetGlyphAdvance(uint32_t glyph_index) override
	{
		return GetGlyphInfo(glyph_index).advance_;
	}

	// Pre-scaled to layout units
	virtual double GetGlyphWidth(uint32_t glyph_index) override
	{
		return GetGlyphInfo(glyph_index).width_;
	}

	// Pre-scaled to layout units
	virtual double GetGlyphHeight(uint32_t glyph_index) override
	{
		return GetGlyphInfo(glyph_index).height_;
	}

	// Pre-scaled to layout units
	virtual double GetGlyphBearing(uint32_t glyph_index) override
	{
		return GetGlyphInfo(glyph_index).bearing_;
	}

	// Get bitmap for scaled glyph
	virtual bool GetGlyphBitmap(uint32_t glyph_index, double device_scale,
		double scale, double& out_scale, RefPtr<Bitmap>& out_bitmap,
		Point& out_bitmap_offset, float& out_lsb_delta, float& out_rsb_delta) override
	{
		const auto& glyphInfo = GetGlyphInfo(glyph_index);
		if (glyphInfo.isValid_)
		{
			out_scale = 1.0;
			out_bitmap = glyphInfo.bitmap_;
			out_bitmap_offset = glyphInfo.offset_;
			out_lsb_delta = glyphInfo.lsbDelta_;
			out_rsb_delta = glyphInfo.rsbDelta_;
			return true;
		}
		return false;
	}

	// Get texture for scaled glyph (creates one if doesn't exist)
	virtual bool GetGlyphTexture(uint32_t glyph_index, double device_scale,
		double scale, double& out_scale, TextureInfo& out_texture_info,
		Point& out_bitmap_offset, float& out_lsb_delta, float& out_rsb_delta) override
	{
		const auto& glyphInfo = GetGlyphInfo(glyph_index);
		if (glyphInfo.isValid_)
		{
			out_scale = 1.0;
			out_texture_info.texture_ = glyphInfo.texture_;
			out_texture_info.uv_ = glyphInfo.uv_;
			out_bitmap_offset = glyphInfo.offset_;
			out_lsb_delta = glyphInfo.lsbDelta_;
			out_rsb_delta = glyphInfo.rsbDelta_;
			return true;
		}
		return false;
	}

	// Get path for scaled glyph
	virtual bool GetGlyphPath(uint32_t glyph_index, RefPtr<Path>& out_path) override
	{
		return false;
	}

	virtual uint64_t bytes_used() override
	{
		return 0;
	}

	const PlatformGlyphInfo& GetGlyphInfo(uint32_t glyphIndex) const
	{
		auto it = glyphMap_.Find(glyphIndex);
		if (it == glyphMap_.End())
		{
			PlatformGlyphInfo glyphInfo;
			if (platformFont_->GetGlyphMetrics(glyphIndex, FontHinting::Normal, glyphInfo.advance_, glyphInfo.width_, glyphInfo.height_, glyphInfo.bearing_) &&
				platformFont_->RenderGlyph(glyphIndex, FontHinting::Normal, glyphInfo.bitmap_, glyphInfo.offset_, glyphInfo.lsbDelta_, glyphInfo.rsbDelta_))
			{
				glyphInfo.isValid_ = true;

				if (currentTexture_.Null() || texY_ > currentTexture_->GetHeight())
				{
					texX_ = 2;
					texY_ = 2;

					currentTexture_ = MakeShared<Texture2D>();
					currentTexture_->SetNumLevels(1);
					currentTexture_->SetAddressMode(TEXTURE_COORDINATE_U, TEXTURE_ADDRESS_BORDER);
					currentTexture_->SetAddressMode(TEXTURE_COORDINATE_V, TEXTURE_ADDRESS_BORDER);
					currentTexture_->SetAddressMode(TEXTURE_COORDINATE_W, TEXTURE_ADDRESS_BORDER);
					currentTexture_->SetFilterMode(TEXTURE_FILTER_BILINEAR);
					currentTexture_->SetSize(perTextureSize_, perTextureSize_, TEXTURE_FORMAT_A8, /*TEXTURE_DYNAMIC*/TEXTURE_STATIC);
					PODVector<unsigned char> buffer(perTextureSize_ * perTextureSize_);
					Memory::Memzero(&buffer[0], buffer.Size());
					currentTexture_->SetData(0, 0, 0, perTextureSize_, perTextureSize_, buffer.Buffer());
					textures_.Push(currentTexture_);
				}

				if (glyphInfo.bitmap_->width() < glyphSize_ &&
					glyphInfo.bitmap_->height() < glyphSize_)
				{
					glyphInfo.width_ = glyphInfo.bitmap_->width();
					glyphInfo.height_ = glyphInfo.bitmap_->height();

					const void* data = glyphInfo.bitmap_->LockPixels();
#if 0
					filterData_.Resize(glyphInfo.bitmap_->size());
					for (uint32_t row = 0; row < glyphInfo.bitmap_->height(); ++row)
					{
						const uint8_t* src = (const uint8_t*)data + glyphInfo.bitmap_->row_bytes() * row;
						uint8_t* dest = (&filterData_[0]) + glyphInfo.bitmap_->row_bytes() * row;
						BoxFilter(dest, glyphInfo.bitmap_->row_bytes(), src, glyphInfo.bitmap_->row_bytes());
					}
					currentTexture_->SetData(0, texX_, texY_, glyphInfo.bitmap_->width(), glyphInfo.bitmap_->height(), filterData_.Buffer());
#else
					currentTexture_->SetData(0, texX_, texY_, glyphInfo.bitmap_->width(), glyphInfo.bitmap_->height(), data);
#endif
					glyphInfo.bitmap_->UnlockPixels();

					glyphInfo.texture_ = currentTexture_;
					glyphInfo.uv_ = Vector4(float(texX_) / perTextureSize_, float(texY_) / perTextureSize_,
						float(texX_ + glyphInfo.bitmap_->width()) / perTextureSize_, float(texY_ + glyphInfo.bitmap_->height()) / perTextureSize_);

					texX_ += glyphSize_;
					if (texX_ == currentTexture_->GetWidth())
					{
						texX_ = 2;
						texY_ += glyphSize_;
					}
				}
				else
				{
					glyphInfo.isValid_ = false;
				}
			}
			it = glyphMap_.Insert(MakePair(glyphIndex, glyphInfo));
		}
		
		return it->second_;
	}

private:
	PlatformFont* platformFont_;

	float fontSize_;

	UInt32 glyphSize_;

	UInt32 perTextureSize_;

	float deviceScale_;

	mutable HashMap<uint32_t, PlatformGlyphInfo> glyphMap_;

	mutable Vector<SharedPtr<Texture2D>> textures_;

	mutable SharedPtr<Texture2D> currentTexture_;
	mutable UInt32 texX_{};
	mutable UInt32 texY_{};

	mutable PODVector<unsigned char> filterData_;
};

Font::Font() = default;

Font::~Font() = default;

Font::Font(const Font&)
{

}

void Font::operator=(const Font&)
{

}

RefPtr<Font> Font::Create(void* platform_data, uint64_t font_file_hash, float font_size)
{
	return RefPtr<Font>(new FontImpl(platform_data, font_file_hash, font_size));
}

}
