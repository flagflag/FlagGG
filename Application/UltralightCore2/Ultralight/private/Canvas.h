/******************************************************************************
 *  This file is a part of Ultralight, an ultra-portable web-browser engine.  *
 *                                                                            *
 *  See <https://ultralig.ht> for licensing and more.                         *
 *                                                                            *
 *  (C) 2023 Ultralight, Inc.                                                 *
 *****************************************************************************/
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/Geometry.h>
#include <Ultralight/private/Paint.h>
#include <Ultralight/private/Path.h>
#include <Ultralight/RefPtr.h>
#include <Ultralight/Bitmap.h>
#include <Ultralight/RenderTarget.h>
#include <Ultralight/private/Font.h>

namespace ultralight {

class Painter;
class Surface;
class GPUDriver;
class Texture;
class Image;
class RenderTexture;
class VideoFrame;

struct UExport Glyph {
  uint32_t index;
  float kerning_x;
  float advance_x;
  float lsb_delta;
  float rsb_delta;
};

///
/// Canvas is used for drawing geometry to an offscreen surface. We currently
/// support both CPU/raster and GPU/command-list implementations.
///
/// This class is mostly used to implement WebCore's GraphicsContext interface.
///
class UExport Canvas : public RefCounted {
public:
  // Create a new Canvas optionally backed by a Surface.
  // Width and height are in pixels
  static RefPtr<Canvas> Create(uint32_t width, uint32_t height, BitmapFormat format, Surface* surface);

  // Returns true if the render texture was replaced (needs full repaint)
  // Width and height are in pixels
  virtual bool Resize(uint32_t width, uint32_t height) = 0;

  virtual void SetDeviceScaleHint(double device_scale_hint) = 0;
  virtual double DeviceScaleHint() const = 0;

  // Resets all commands, draw list, and state to initial conditions.
  // Also resets memory pools for canvases with local allocators.
  virtual void Reset() = 0;

  // Locks the surface for drawing (CPU only)
  virtual void LockSurface() = 0;

  // Gets locked pixel buffer, if any (CPU only)
  virtual void* locked_pixels() = 0;

  // Flush active drawing commands to pixels, composites any active
  // transparency layers. Also unlocks surface. (CPU only)
  virtual void FlushSurface() = 0;

  // Get the underlying Surface (CPU only)
  virtual Surface* surface() = 0;

  // Performs pixel scroll, returns the portion of the rect to be repainted.
  // (CPU only)
  virtual IntRect Scroll(const IntRect& rect, int dx, int dy) = 0;

  // Clears the entire canvas to 0 (alpha channel is cleared too)
  virtual void Clear() = 0;

  // Returns true if the render texture was replaced (needs full repaint)
  virtual bool ShrinkIfNeeded() = 0;

  // Releases the active backing render texture (GPU only)
  virtual void RecycleRenderTexture() = 0;

  // Width in pixels
  virtual uint32_t width() const = 0;

  // Height in pixels
  virtual uint32_t height() const = 0;

  // Pixel format
  virtual BitmapFormat format() const = 0;

  // Save and Restore Drawing State
  virtual void Save() = 0;
  virtual void Restore() = 0;

  virtual void SetCompositeOp(CompositeOp op) = 0;
  virtual CompositeOp composite_op() const = 0;

  virtual void SetBlendMode(BlendMode mode) = 0;
  virtual BlendMode blend_mode() const = 0;

  virtual void SetAlpha(float alpha) = 0;
  virtual float alpha() const = 0;

  // Clip Methods
  virtual void SetClip(const Rect& rect, bool inverse) = 0;
  virtual void SetClip(const RoundedRect& rrect, bool inverse) = 0;
  virtual void SetClip(RefPtr<Path> path, FillRule rule, bool inverse) = 0;
  virtual void SetClipEmpty() = 0;
  virtual Rect GetClipBounds() const = 0;

  // Transform Methods
  virtual void Transform(const Matrix& matrix) = 0;
  virtual void SetMatrix(const Matrix& matrix) = 0;
  virtual Matrix GetMatrix() const = 0;
  virtual void SetRootMatrix(const Matrix& matrix) = 0;

  virtual void BeginTransparencyLayer(float opacity) = 0;
  virtual void EndTransparencyLayer() = 0;

  // Whether or not blending is enabled, default is true.
  virtual void set_blending_enabled(bool val) = 0;
  virtual bool blending_enabled() const = 0;

  // Whether or not scissor test is enabled, default is false.
  virtual void set_scissor_enabled(bool val) = 0;
  virtual bool scissor_enabled() const = 0;

  // Units in pixels
  virtual void SetScissorRect(const IntRect& rect) = 0;
  virtual IntRect GetScissorRect() = 0;

  // Get the render target info (GPU only)
  virtual RenderTarget render_target() const = 0;

  // Get the active backing render texture (GPU only)
  virtual RefPtr<RenderTexture> render_texture() const = 0;

  // Drawing
  virtual void DrawRect(const Rect& rect, const Paint& paint) = 0;
  virtual void DrawLine(const Point& p0, const Point& p1, const Paint& paint) = 0;
  virtual void DrawEllipse(const Rect& rect, const Paint& paint) = 0;
  virtual void DrawRoundedRect(const RoundedRect& rrect, const Paint& paint,
    float stroke_width, Color stroke_color) = 0;
  virtual void DrawBoxShadow(const Rect& paint_rect, const RoundedRect& rrect, const RoundedRect& clip_rrect,
    bool inset, const Point& offset, float radius, const Paint& paint) = 0;
  virtual void FillPath(RefPtr<Path> path, const Paint& paint, FillRule fill_rule) = 0;
  virtual void StrokePath(RefPtr<Path> path, const Paint& paint, float stroke_width,
    LineCap line_cap = kLineCap_Butt, LineJoin line_join = kLineJoin_Miter, int miter_limit = 10,
    const float* dash_array = nullptr, size_t dash_array_size = 0, float dash_offset = 0.0f) = 0;
  virtual void DrawImage(RefPtr<Image> image, uint32_t cur_frame,
    const Rect& src, const Rect& dest, const Paint& paint) = 0;
  virtual void DrawPattern(RefPtr<Image> image, uint32_t cur_frame,
    const Rect& src, const Rect& dest, const Matrix& transform) = 0;
  virtual void DrawGlyphs(RefPtr<Font> font, const Paint& paint, const Point& origin, Glyph* glyphs, size_t num_glyphs, const Point& offset) = 0;
  virtual void DrawGradient(Gradient* gradient, const Rect& dest) = 0;
  virtual void DrawVideoFrame(RefPtr<VideoFrame> video_frame, const Rect& dest) = 0;

  // DrawCanvas* methods form a draw dependency between 'canvas' and this Surface's parent canvas.
  // Painter will sort canvases by draw dependencies and draw them in correct order.
  virtual void DrawCanvas(RefPtr<Canvas> canvas, const Rect& src, const Rect& dest, const Paint& paint) = 0;
  virtual void DrawCanvasPattern(RefPtr<Canvas> canvas, const Rect& src_uv, const Rect& src, const Rect& dest, const Matrix& transform) = 0;

  // Deprecated, this will be removed soon.
  virtual void DrawBoxDecorations(const Rect& layout_rect, const RoundedRect& outer_rrect,
    const RoundedRect& inner_rrect, BorderPaint border_top, BorderPaint border_right,
    BorderPaint border_bottom, BorderPaint border_left, Color fill_color) = 0;

protected:
  virtual void Draw() = 0;

  Canvas();
  virtual ~Canvas();
  Canvas(const Canvas&);
  void operator=(const Canvas&);

  friend class GPUContext;
};

}  // namespace ultralight
