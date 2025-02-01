/******************************************************************************
 *  This file is a part of Ultralight, an ultra-portable web-browser engine.  *
 *                                                                            *
 *  See <https://ultralig.ht> for licensing and more.                         *
 *                                                                            *
 *  (C) 2023 Ultralight, Inc.                                                 *
 *****************************************************************************/
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/RefPtr.h>
#include <Ultralight/Geometry.h>

namespace ultralight {

class Bitmap;
class Path;
struct TextureInfo;

class UExport Font : public RefCounted {
public:
  // Font size in pixels
  virtual float font_size() const = 0;

  virtual float device_scale_hint() const = 0;

  virtual void set_device_scale_hint(float scale) = 0;

  virtual bool HasGlyph(uint32_t index) const = 0;

  // Pre-scaled to layout units
  virtual double GetGlyphAdvance(uint32_t glyph_index) = 0;

  // Pre-scaled to layout units
  virtual double GetGlyphWidth(uint32_t glyph_index) = 0;

  // Pre-scaled to layout units
  virtual double GetGlyphHeight(uint32_t glyph_index) = 0;

  // Pre-scaled to layout units
  virtual double GetGlyphBearing(uint32_t glyph_index) = 0;

  // Get bitmap for scaled glyph
  virtual bool GetGlyphBitmap(uint32_t glyph_index, double device_scale, 
    double scale, double& out_scale, RefPtr<Bitmap>& out_bitmap, 
    Point& out_bitmap_offset, float& out_lsb_delta, float& out_rsb_delta) = 0;
  
  // Get texture for scaled glyph (creates one if doesn't exist)
  virtual bool GetGlyphTexture(uint32_t glyph_index, double device_scale,
    double scale, double& out_scale, TextureInfo& out_texture_info,
    Point& out_bitmap_offset, float& out_lsb_delta, float& out_rsb_delta) = 0;

  // Get path for scaled glyph
  virtual bool GetGlyphPath(uint32_t glyph_index, RefPtr<Path>& out_path) = 0;

  virtual uint64_t bytes_used() = 0;

protected:
  static RefPtr<Font> Create(void* platform_data, uint64_t font_file_hash, float font_size);

  Font();
  virtual ~Font();
  Font(const Font&);
  void operator=(const Font&);

  friend class FontCache;
};

} // namespace ultralight
