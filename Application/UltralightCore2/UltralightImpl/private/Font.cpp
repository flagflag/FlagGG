#include "Ultralight/private/Font.h"
#include "Ultralight/private/util/RefCountedImpl.h"

namespace ultralight
{

class FontImpl : public Font, public RefCountedImpl<FontImpl>
{
public:
	REF_COUNTED_IMPL(FontImpl);

	FontImpl(void* platform_data, uint64_t font_file_hash, float font_size)
	{

	}

	~FontImpl() override
	{

	}

	// Font size in pixels
	virtual float font_size() const override
	{
		return 0.0f;
	}

	virtual float device_scale_hint() const override
	{
		return 0.0f;
	}

	virtual void set_device_scale_hint(float scale) override
	{

	}

	virtual bool HasGlyph(uint32_t index) const override
	{
		return false;
	}

	// Pre-scaled to layout units
	virtual double GetGlyphAdvance(uint32_t glyph_index) override
	{
		return 16.0f;
	}

	// Pre-scaled to layout units
	virtual double GetGlyphWidth(uint32_t glyph_index) override
	{
		return 0.0;
	}

	// Pre-scaled to layout units
	virtual double GetGlyphHeight(uint32_t glyph_index) override
	{
		return 0.0;
	}

	// Pre-scaled to layout units
	virtual double GetGlyphBearing(uint32_t glyph_index) override
	{
		return 0.0;
	}

	// Get bitmap for scaled glyph
	virtual bool GetGlyphBitmap(uint32_t glyph_index, double device_scale,
		double scale, double& out_scale, RefPtr<Bitmap>& out_bitmap,
		Point& out_bitmap_offset, float& out_lsb_delta, float& out_rsb_delta) override
	{
		return false;
	}

	// Get texture for scaled glyph (creates one if doesn't exist)
	virtual bool GetGlyphTexture(uint32_t glyph_index, double device_scale,
		double scale, double& out_scale, TextureInfo& out_texture_info,
		Point& out_bitmap_offset, float& out_lsb_delta, float& out_rsb_delta) override
	{
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
