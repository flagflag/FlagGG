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
#include <Ultralight/platform/Config.h>

namespace ultralight {
class Bitmap;
class Path;

class UExport PlatformFont {
public:
  inline virtual ~PlatformFont() {}

  virtual void SetFontSize(float font_size_px) = 0;

  virtual bool GetGlyphMetrics(uint32_t glyph_index, FontHinting hinting,
    double& advance, double& width, double& height, double& bearing) = 0;

  virtual bool RenderGlyph(uint32_t glyph_index, FontHinting hinting, RefPtr<Bitmap>& out_bitmap,
      Point& out_offset, float& out_lsb_delta, float& out_rsb_delta) = 0;

  virtual bool GetGlyphPath(uint32_t glyph_index, RefPtr<Path>& out_path) = 0;
};

class UExport PlatformFontFactory {
public:
  inline virtual ~PlatformFontFactory() {}

  virtual PlatformFont* CreatePlatformFont(void* data) = 0;

  virtual void DestroyPlatformFont(PlatformFont* platform_font) = 0;
};

UExport PlatformFontFactory* platform_font_factory();
UExport void set_platform_font_factory(PlatformFontFactory* factory);

}  // namespace ultralight
