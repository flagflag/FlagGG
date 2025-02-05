#include "Ultralight/private/Canvas.h"
#include "Ultralight/private/util/RefCountedImpl.h"
#include "Ultralight/private/Image.h"
#include "Ultralight/private/VideoFrame.h"
#include "Ultralight/platform/Surface.h"
#include "UltralightImpl/UltralightInnerType.h"
#include "UltralightImpl/ThreadCall.h"
#include "RenderContext.h"

#include <GfxDevice/GfxDevice.h>
#include <GfxDevice/GfxSwapChain.h>
#include <Graphics/Texture2D.h>
#include <Graphics/Batch2D.h>
#include <Graphics/Window.h>
#include <UI/UISystem.h>

using namespace FlagGG;

namespace ultralight
{

extern Texture2D* GetImageInnerTexture(Image* image, uint32_t frameId);

#if !APP_CORE_FOR_ENGINE
struct FrameRenderData : public FlagGG::RefCounted
{
	VertexVector vertexVector_;
	FlagGG::Vector<SharedPtr<Batch>> uiBatches_;
};
#endif

class CanvasImpl : public Canvas, public RefCountedImpl<CanvasImpl>
{
public:
	REF_COUNTED_IMPL(CanvasImpl);

	CanvasImpl(uint32_t width, uint32_t height, BitmapFormat format, Surface* surface)
		: width_(width)
		, height_(height)
		, format_(format)
		, surface_(surface)
	{
		mat_.SetIdentity();

#if !APP_CORE_FOR_ENGINE
		if (!surface)
		{
			RefPtr<Canvas> thisRef(this);
			GetSubsystem<ThreadRenderCall>()->Forward([thisRef, this]()
			{
				renderTarget_ = new Texture2D();
				renderTarget_->SetNumLevels(1);
				renderTarget_->SetSize(width_, height_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
				window_ = new FlagGG::Window(nullptr, FlagGG::IntRect(0, 0, width_, height_));
				window_->Show();
			});
		}
#endif
	}

	~CanvasImpl() override
	{
#if !APP_CORE_FOR_ENGINE
		auto renderTarget = renderTarget_;
		auto window = window_;
		GetSubsystem<ThreadRenderCall>()->SetRenderCall(renderTarget_.Get(), nullptr);
		GetSubsystem<ThreadRenderCall>()->Forward([renderTarget, window]()
		{
		});
#endif
	}

	// Returns true if the render texture was replaced (needs full repaint)
	// Width and height are in pixels
	virtual bool Resize(uint32_t width, uint32_t height) override
	{
		width_ = width;
		height_ = height;

		if (surface_)
		{
			surface_->Resize(width, height);
		}

#if !APP_CORE_FOR_ENGINE
		if (renderTarget_)
		{
			RefPtr<Canvas> thisRef(this);
			GetSubsystem<ThreadRenderCall>()->Forward([thisRef, this]()
			{
				renderTarget_->SetSize(width_, height_, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
			});
		}
#endif

		return false;
	}

	virtual void SetDeviceScaleHint(double device_scale_hint) override
	{
		deviceScaleHint_ = device_scale_hint;
	}

	virtual double DeviceScaleHint() const override
	{
		return deviceScaleHint_;
	}

	// Resets all commands, draw list, and state to initial conditions.
	// Also resets memory pools for canvases with local allocators.
	virtual void Reset() override
	{
		if (surfaceLockedPixel_)
		{
			surfaceLockedPixel_ = nullptr;
			surface_->UnlockPixels();
		}

		mat_.SetIdentity();

#if !APP_CORE_FOR_ENGINE
		if (!frameRenderData_)
		{
			frameRenderData_ = new FrameRenderData();
		}
#endif
	}

	// Locks the surface for drawing (CPU only)
	virtual void LockSurface() override
	{
		if (surface_)
		{
			surfaceLockedPixel_ = surface_->LockPixels();
		}
	}

	// Gets locked pixel buffer, if any (CPU only)
	virtual void* locked_pixels() override
	{
		return surfaceLockedPixel_;
	}

	// Flush active drawing commands to pixels, composites any active
	// transparency layers. Also unlocks surface. (CPU only)
	virtual void FlushSurface() override
	{
		if (surfaceLockedPixel_)
		{
			surfaceLockedPixel_ = nullptr;
			surface_->UnlockPixels();
		}

#if !APP_CORE_FOR_ENGINE
		if (frameRenderData_ && frameRenderData_->uiBatches_.Size())
		{
			RefPtr<Canvas> thisRef(this);

			struct TempRenderData : public FlagGG::RefCounted
			{
				VertexVector vertexVector_;
				FlagGG::Vector<SharedPtr<Batch>> uiBatches_;
			};

			SharedPtr<FrameRenderData> frameRenderData = frameRenderData_;
			frameRenderData_.Reset();

			GetSubsystem<ThreadRenderCall>()->SetRenderCall(renderTarget_.Get(), [thisRef, frameRenderData, this]()
			{
				if (renderTarget_)
				{
					auto* gfxDevice = FlagGG::GfxDevice::GetDevice();
					gfxDevice->ResetRenderTargets();
					gfxDevice->SetRenderTarget(0, renderTarget_->GetRenderSurface());
					gfxDevice->Clear(CLEAR_COLOR, Color::BLACK);

					GetSubsystem<UISystem>()->RenderWebKit(renderTarget_->GetRenderSurface(), FlagGG::Rect(0, 0, width_, height_), frameRenderData->uiBatches_);

					window_->GetSwapChain()->CopyData(renderTarget_->GetGfxTextureRef());
					window_->GetSwapChain()->Present();
				}
			});

			frameRenderData_ = new FrameRenderData();
		}
#endif
	}

	// Get the underlying Surface (CPU only)
	virtual Surface* surface() override
	{
		return surface_;
	}

	// Performs pixel scroll, returns the portion of the rect to be repainted.
	// (CPU only)
	virtual IntRect Scroll(const IntRect& rect, int dx, int dy) override
	{
		return IntRect();
	}

	// Clears the entire canvas to 0 (alpha channel is cleared too)
	virtual void Clear() override
	{

	}

	// Returns true if the render texture was replaced (needs full repaint)
	virtual bool ShrinkIfNeeded() override
	{
		return false;
	}

	// Releases the active backing render texture (GPU only)
	virtual void RecycleRenderTexture() override
	{

	}

	// Width in pixels
	virtual uint32_t width() const override
	{
		return width_;
	}

	// Height in pixels
	virtual uint32_t height() const override
	{
		return height_;
	}

	// Pixel format
	virtual BitmapFormat format() const override
	{
		return format_;
	}

	// Save and Restore Drawing State
	virtual void Save() override
	{

	}

	virtual void Restore() override
	{

	}

	virtual void SetCompositeOp(CompositeOp op) override
	{
		compositeOp_ = op;
	}

	virtual CompositeOp composite_op() const override
	{
		return compositeOp_;
	}

	virtual void SetBlendMode(BlendMode mode) override
	{
		blendMode_ = mode;
	}

	virtual BlendMode blend_mode() const override
	{
		return blendMode_;
	}

	virtual void SetAlpha(float alpha) override
	{
		alpha_ = alpha;
	}

	virtual float alpha() const override
	{
		return alpha_;
	}

	// Clip Methods
	virtual void SetClip(const Rect& rect, bool inverse) override
	{
		clipRect_ = rect;
	}

	virtual void SetClip(const RoundedRect& rrect, bool inverse) override
	{
		clipRoundedRect_ = rrect;
	}

	virtual void SetClip(RefPtr<Path> path, FillRule rule, bool inverse) override
	{

	}

	virtual void SetClipEmpty() override
	{

	}

	virtual Rect GetClipBounds() const override
	{
		return clipRect_;
	}

	// Transform Methods
	virtual void Transform(const Matrix& matrix) override
	{
		mat_.Transform(matrix);
	}

	virtual void SetMatrix(const Matrix& matrix) override
	{
		mat_ = matrix;
	}

	virtual Matrix GetMatrix() const override
	{
		return mat_;
	}

	virtual void SetRootMatrix(const Matrix& matrix) override
	{

	}

	virtual void BeginTransparencyLayer(float opacity) override
	{

	}

	virtual void EndTransparencyLayer() override
	{

	}

	// Whether or not blending is enabled, default is true.
	virtual void set_blending_enabled(bool val) override
	{
		blendEnable_ = val;
	}

	virtual bool blending_enabled() const override
	{
		return blendEnable_;
	}

	// Whether or not scissor test is enabled, default is false.
	virtual void set_scissor_enabled(bool val) override
	{
		scissorEnable_ = val;
	}

	virtual bool scissor_enabled() const override
	{
		return scissorEnable_;
	}

	// Units in pixels
	virtual void SetScissorRect(const IntRect& rect) override
	{
		scissorRect_ = rect;
	}

	virtual IntRect GetScissorRect() override
	{
		return scissorRect_;
	}

	// Get the render target info (GPU only)
	virtual RenderTarget render_target() const override
	{
		RenderTarget renderTarget;
		renderTarget.is_empty = false;
		renderTarget.width = width_;
		renderTarget.height = height_;
		renderTarget.texture_width = width_;
		renderTarget.texture_height = height_;
		renderTarget.texture_format = format_;
		renderTarget.uv_coords.left = 0;
		renderTarget.uv_coords.top = 0;
		renderTarget.uv_coords.right = 1;
		renderTarget.uv_coords.bottom = 1;
		return renderTarget;
	}

	// Get the active backing render texture (GPU only)
	virtual RefPtr<RenderTexture> render_texture() const override
	{
		return nullptr;
	}

	// Drawing
	virtual void DrawRect(const Rect& rect, const Paint& paint) override
	{
#if APP_CORE_FOR_ENGINE
		SharedPtr<BatchWebKit> batch(new BatchWebKit(GetSubsystem<ultralight::RenderContext>()->GetCallStackVertexVector()));
#else
		SharedPtr<BatchWebKit> batch(new BatchWebKit(&(frameRenderData_->vertexVector_)));
#endif

		if (scissorEnable_)
			batch->SetScissorRect(FlagGG::IntRect(scissorRect_.left, scissorRect_.top, scissorRect_.right, scissorRect_.bottom));
		else
			batch->SetScissorRect(FlagGG::IntRect::ZERO);
		batch->SetBlendMode(blendEnable_ ? BLEND_ALPHA : BLEND_REPLACE);

		const Rect rectFinal = mat_.Apply(rect);

		float x1 = rectFinal.left;
		float y1 = rectFinal.top;
		float x2 = rectFinal.right;
		float y2 = rectFinal.bottom;

		FlagGG::Color color = UltralightColorGetFloat4(paint.color);
		UInt32 color32 = color.ToUInt();

		batch->AddTriangle(
			Vector2(x1, y1), Vector2(x1, y2), Vector2(x2, y2),
			Vector2(0, 0), Vector2(0, 1), Vector2(1, 1),
			color32,
			color32,
			color32
		);

		batch->AddTriangle(
			Vector2(x1, y1), Vector2(x2, y2), Vector2(x2, y1),
			Vector2(0, 0), Vector2(1, 1), Vector2(1, 0),
			color32,
			color32,
			color32
		);

#if APP_CORE_FOR_ENGINE
		GetSubsystem<ultralight::RenderContext>()->GetCallStackBatches()->Push(batch);
#else
		frameRenderData_->uiBatches_.Push(batch);
#endif
	}

	virtual void DrawLine(const Point& p0, const Point& p1, const Paint& paint) override
	{

	}

	virtual void DrawEllipse(const Rect& rect, const Paint& paint) override
	{

	}

	virtual void DrawRoundedRect(const RoundedRect& rrect, const Paint& paint,
		float stroke_width, Color stroke_color) override
	{
#if APP_CORE_FOR_ENGINE
		SharedPtr<BatchWebKit> batch(new BatchWebKit(GetSubsystem<ultralight::RenderContext>()->GetCallStackVertexVector()));
#else
		SharedPtr<BatchWebKit> batch(new BatchWebKit(&(frameRenderData_->vertexVector_)));
#endif

		if (scissorEnable_)
			batch->SetScissorRect(FlagGG::IntRect(scissorRect_.left, scissorRect_.top, scissorRect_.right, scissorRect_.bottom));
		else
			batch->SetScissorRect(FlagGG::IntRect::ZERO);
		batch->SetBlendMode(blendEnable_ ? BLEND_ALPHA : BLEND_REPLACE);

		const Rect rectFinal = mat_.Apply(rrect.rect);

		float x1 = rectFinal.left;
		float y1 = rectFinal.top;
		float x2 = rectFinal.right;
		float y2 = rectFinal.bottom;

		FlagGG::Color color = UltralightColorGetFloat4(paint.color);
		UInt32 color32 = color.ToUInt();

		Vector4 data0(/*FillType_Rounded_Rect*/7, y1, x2 - x1, y2 - y1);
		Vector4 data1(rrect.radii_x);
		Vector4 data2(rrect.radii_y);
		Vector4 data3(stroke_width, 0.0f, 0.0f, 0.0f);
		Vector4 data4 = UltralightColorGetFloat4(stroke_color);

		batch->AddTriangle(
			Vector2(x1, y1), Vector2(x1, y2), Vector2(x2, y2),
			Vector2(0, 0), Vector2(0, 1), Vector2(1, 1),
			color32,
			color32,
			color32,
			data0,
			data1,
			data2,
			data3,
			data4
		);

		batch->AddTriangle(
			Vector2(x1, y1), Vector2(x2, y2), Vector2(x2, y1),
			Vector2(0, 0), Vector2(1, 1), Vector2(1, 0),
			color32,
			color32,
			color32,
			data0,
			data1,
			data2,
			data3,
			data4
		);

#if APP_CORE_FOR_ENGINE
		GetSubsystem<ultralight::RenderContext>()->GetCallStackBatches()->Push(batch);
#else
		frameRenderData_->uiBatches_.Push(batch);
#endif
	}

	virtual void DrawBoxShadow(const Rect& paint_rect, const RoundedRect& rrect, const RoundedRect& clip_rrect,
		bool inset, const Point& offset, float radius, const Paint& paint) override
	{

	}

	virtual void FillPath(RefPtr<Path> path, const Paint& paint, FillRule fill_rule) override
	{

	}

	virtual void StrokePath(RefPtr<Path> path, const Paint& paint, float stroke_width,
		LineCap line_cap = kLineCap_Butt, LineJoin line_join = kLineJoin_Miter, int miter_limit = 10,
		const float* dash_array = nullptr, size_t dash_array_size = 0, float dash_offset = 0.0f) override
	{

	}

	virtual void DrawImage(RefPtr<Image> image, uint32_t cur_frame,
		const Rect& src, const Rect& dest, const Paint& paint) override
	{
#if APP_CORE_FOR_ENGINE
		SharedPtr<BatchWebKit> batch(new BatchWebKit(GetSubsystem<ultralight::RenderContext>()->GetCallStackVertexVector()));
#else
		SharedPtr<BatchWebKit> batch(new BatchWebKit(&(frameRenderData_->vertexVector_)));
#endif

		if (scissorEnable_)
			batch->SetScissorRect(FlagGG::IntRect(scissorRect_.left, scissorRect_.top, scissorRect_.right, scissorRect_.bottom));
		else
			batch->SetScissorRect(FlagGG::IntRect::ZERO);
		batch->SetBlendMode(blendEnable_ ? BLEND_ALPHA : BLEND_REPLACE);
		auto* texture = GetImageInnerTexture(image.get(), cur_frame);
		batch->SetTexture(texture);

		const Rect rectFinal = mat_.Apply(dest);

		float x1 = rectFinal.left;
		float y1 = rectFinal.top;
		float x2 = rectFinal.right;
		float y2 = rectFinal.bottom;

		float uvX1 = src.left / texture->GetWidth();
		float uvY1 = src.top / texture->GetHeight();
		float uvX2 = src.right / texture->GetWidth();
		float uvY2 = src.bottom / texture->GetHeight();

		UInt32 color32 = FlagGG::Color::WHITE.ToUInt();

		Vector4 data0(/*FillType_Image*/1, 0, 0, 0);

		batch->AddTriangle(
			Vector2(x1, y1), Vector2(x1, y2), Vector2(x2, y2),
			Vector2(uvX1, uvY1), Vector2(uvX1, uvY2), Vector2(uvX2, uvY2),
			color32,
			color32,
			color32,
			data0
		);

		batch->AddTriangle(
			Vector2(x1, y1), Vector2(x2, y2), Vector2(x2, y1),
			Vector2(uvX1, uvY1), Vector2(uvX2, uvY2), Vector2(uvX2, uvY1),
			color32,
			color32,
			color32,
			data0
		);

#if APP_CORE_FOR_ENGINE
		GetSubsystem<ultralight::RenderContext>()->GetCallStackBatches()->Push(batch);
#else
		frameRenderData_->uiBatches_.Push(batch);
#endif
	}

	virtual void DrawPattern(RefPtr<Image> image, uint32_t cur_frame,
		const Rect& src, const Rect& dest, const Matrix& transform) override
	{
#if APP_CORE_FOR_ENGINE
		SharedPtr<BatchWebKit> batch(new BatchWebKit(GetSubsystem<ultralight::RenderContext>()->GetCallStackVertexVector()));
#else
		SharedPtr<BatchWebKit> batch(new BatchWebKit(&(frameRenderData_->vertexVector_)));
#endif

		if (scissorEnable_)
			batch->SetScissorRect(FlagGG::IntRect(scissorRect_.left, scissorRect_.top, scissorRect_.right, scissorRect_.bottom));
		else
			batch->SetScissorRect(FlagGG::IntRect::ZERO);
		batch->SetBlendMode(blendEnable_ ? BLEND_ALPHA : BLEND_REPLACE);
		auto* texture = GetImageInnerTexture(image.get(), cur_frame);
		batch->SetTexture(texture);

		const Rect rectFinal = mat_.Apply(dest);

		float x1 = rectFinal.left;
		float y1 = rectFinal.top;
		float x2 = rectFinal.right;
		float y2 = rectFinal.bottom;

		float uvX1 = src.left / texture->GetWidth();
		float uvY1 = src.top / texture->GetHeight();
		float uvX2 = src.right / texture->GetWidth();
		float uvY2 = src.bottom / texture->GetHeight();

		batch->vector_[0] = Vector4(uvX1, uvY1, uvX2, uvY2);
		batch->vector_[1] = Vector4(0, 0, src.right - src.left, src.bottom - src.top);
		batch->vector_[2] = Vector4(transform.data[0][0], transform.data[0][1], transform.data[1][0], transform.data[1][1]);
		batch->vector_[3] = Vector4(transform.data[3][0], transform.data[3][1], 0, 0);

		UInt32 color32 = FlagGG::Color::WHITE.ToUInt();

		Vector4 data0(/*FillType_Pattern_Image*/2, 0, 0, 0);

		batch->AddTriangle(
			Vector2(x1, y1), Vector2(x1, y2), Vector2(x2, y2),
			Vector2(uvX1, uvY1), Vector2(uvX1, uvY2), Vector2(uvX2, uvY2),
			color32,
			color32,
			color32,
			data0
		);

		batch->AddTriangle(
			Vector2(x1, y1), Vector2(x2, y2), Vector2(x2, y1),
			Vector2(uvX1, uvY1), Vector2(uvX2, uvY2), Vector2(uvX2, uvY1),
			color32,
			color32,
			color32,
			data0
		);

#if APP_CORE_FOR_ENGINE
		GetSubsystem<ultralight::RenderContext>()->GetCallStackBatches()->Push(batch);
#else
		frameRenderData_->uiBatches_.Push(batch);
#endif
	}

	virtual void DrawGlyphs(RefPtr<Font> font, const Paint& paint, const Point& origin, Glyph* glyphs, size_t num_glyphs, const Point& offset) override
	{
		SharedPtr<BatchWebKit> batch;

		Point finalOrigin = mat_.Apply(origin + offset);

		Texture2D* lastGlyphTexture = nullptr;

		double outScale;
		RefPtr<Bitmap> bitmap;
		Point bitmapOffset;    // => vertex offset
		float lsbDelta;
		float rsbDelta;
		TextureInfo texInfo;

		for (UInt32 i = 0; i < num_glyphs; ++i)
		{
			if (!font->GetGlyphTexture(glyphs[i].index, 1.0, 1.0, outScale, texInfo, bitmapOffset, lsbDelta, rsbDelta))
			{
				finalOrigin.x += glyphs[i].advance_x;
				continue;
			}

			if (texInfo.texture_ != lastGlyphTexture)
			{
#if APP_CORE_FOR_ENGINE
				batch = new BatchWebKit(GetSubsystem<ultralight::RenderContext>()->GetCallStackVertexVector());
#else
				batch = new BatchWebKit(&(frameRenderData_->vertexVector_));
#endif
				batch->SetTexture(texInfo.texture_);
				if (scissorEnable_)
					batch->SetScissorRect(FlagGG::IntRect(scissorRect_.left, scissorRect_.top, scissorRect_.right, scissorRect_.bottom));
				else
					batch->SetScissorRect(FlagGG::IntRect::ZERO);
				batch->SetBlendMode(blendEnable_ ? BLEND_ALPHA : BLEND_REPLACE);
			}

			float x1 = finalOrigin.x + bitmapOffset.x;
			float y1 = finalOrigin.y + bitmapOffset.y;
			float x2 = x1 + font->GetGlyphWidth(glyphs[i].index);
			float y2 = y1 + font->GetGlyphHeight(glyphs[i].index);

			FlagGG::Color color = UltralightColorGetFloat4(paint.color);
			UInt32 color32 = color.ToUInt();

			Vector4 data0(/*FillType_Glyph*/11, 1, 0, 0);

			batch->AddTriangle(
				Vector2(x1, y1), Vector2(x1, y2), Vector2(x2, y2),
				Vector2(texInfo.uv_.x_, texInfo.uv_.y_), Vector2(texInfo.uv_.x_, texInfo.uv_.w_), Vector2(texInfo.uv_.z_, texInfo.uv_.w_),
				color32,
				color32,
				color32,
				data0
			);

			batch->AddTriangle(
				Vector2(x1, y1), Vector2(x2, y2), Vector2(x2, y1),
				Vector2(texInfo.uv_.x_, texInfo.uv_.y_), Vector2(texInfo.uv_.z_, texInfo.uv_.w_), Vector2(texInfo.uv_.z_, texInfo.uv_.y_),
				color32,
				color32,
				color32,
				data0
			);

			ASSERT(font->GetGlyphAdvance(glyphs[i].index) == glyphs[i].advance_x);
			finalOrigin.x += glyphs[i].advance_x;

			if (texInfo.texture_ != lastGlyphTexture)
			{
#if APP_CORE_FOR_ENGINE
				GetSubsystem<ultralight::RenderContext>()->GetCallStackBatches()->Push(batch);
#else
				frameRenderData_->uiBatches_.Push(batch);
#endif
			}

			lastGlyphTexture = texInfo.texture_;
		}
	}

	virtual void DrawGradient(Gradient* gradient, const Rect& dest) override
	{
#if APP_CORE_FOR_ENGINE
		SharedPtr<BatchWebKit> batch(new BatchWebKit(GetSubsystem<ultralight::RenderContext>()->GetCallStackVertexVector()));
#else
		SharedPtr<BatchWebKit> batch(new BatchWebKit(&(frameRenderData_->vertexVector_)));
#endif

		if (scissorEnable_)
			batch->SetScissorRect(FlagGG::IntRect(scissorRect_.left, scissorRect_.top, scissorRect_.right, scissorRect_.bottom));
		else
			batch->SetScissorRect(FlagGG::IntRect::ZERO);
		batch->SetBlendMode(blendEnable_ ? BLEND_ALPHA : BLEND_REPLACE);

		const Rect rectFinal = mat_.Apply(dest);

		float x1 = rectFinal.left;
		float y1 = rectFinal.top;
		float x2 = rectFinal.right;
		float y2 = rectFinal.bottom;
		float width = x2 - x1;
		float height = y2 - y1;

		UInt32 color32 = 0;

		Vector4 data0(/*FillType_Pattern_Gradient*/3, gradient->stops.size(), gradient->is_radial ? 1 : 0, 0);
		Vector4 data1(gradient->p0.x / width, gradient->p0.y / height, gradient->p1.x / width, gradient->p1.y / height);
		Vector4 data2;
		Vector4 data3to6[4];
		for (UInt32 i = 0; i < gradient->stops.size(); ++i)
		{
			data2[i] = gradient->stops[i].stop;
			data3to6[i] = UltralightColorGetFloat4(gradient->stops[i].color);
		}

		batch->AddTriangle(
			Vector2(x1, y1), Vector2(x1, y2), Vector2(x2, y2),
			Vector2(0, 0), Vector2(0, 1), Vector2(1, 1),
			color32,
			color32,
			color32,
			data0,
			data1,
			data2,
			data3to6[0],
			data3to6[1],
			data3to6[2],
			data3to6[3]
		);

		batch->AddTriangle(
			Vector2(x1, y1), Vector2(x2, y2), Vector2(x2, y1),
			Vector2(0, 0), Vector2(1, 1), Vector2(1, 0),
			color32,
			color32,
			color32,
			data0,
			data1,
			data2,
			data3to6[0],
			data3to6[1],
			data3to6[2],
			data3to6[3]
		);

#if APP_CORE_FOR_ENGINE
		GetSubsystem<ultralight::RenderContext>()->GetCallStackBatches()->Push(batch);
#else
		frameRenderData_->uiBatches_.Push(batch);
#endif
	}

	virtual void DrawVideoFrame(RefPtr<VideoFrame> video_frame, const Rect& dest) override
	{

	}

	// DrawCanvas* methods form a draw dependency between 'canvas' and this Surface's parent canvas.
	// Painter will sort canvases by draw dependencies and draw them in correct order.
	virtual void DrawCanvas(RefPtr<Canvas> canvas, const Rect& src, const Rect& dest, const Paint& paint) override
	{

	}

	virtual void DrawCanvasPattern(RefPtr<Canvas> canvas, const Rect& src_uv, const Rect& src, const Rect& dest, const Matrix& transform) override
	{

	}

	// Deprecated, this will be removed soon.
	virtual void DrawBoxDecorations(const Rect& layout_rect, const RoundedRect& outer_rrect,
		const RoundedRect& inner_rrect, BorderPaint border_top, BorderPaint border_right,
		BorderPaint border_bottom, BorderPaint border_left, Color fill_color) override
	{

	}

protected:
	virtual void Draw() override
	{

	}

private:
	uint32_t width_;

	uint32_t height_;

	BitmapFormat format_;
	
	Surface* surface_;

	void* surfaceLockedPixel_{};

	float deviceScaleHint_{ 1.0 };

	CompositeOp compositeOp_{};

	BlendMode blendMode_{};

	float alpha_{ 1.0f };

	Matrix mat_;

	bool blendEnable_{ true };

	bool scissorEnable_{ false };

	IntRect scissorRect_;

	Rect clipRect_;

	RoundedRect clipRoundedRect_;

#if !APP_CORE_FOR_ENGINE
// for gpu rendering:
	SharedPtr<Texture2D> renderTarget_;

	SharedPtr<FlagGG::Window> window_;

	SharedPtr<FrameRenderData> frameRenderData_;
#endif
};

Canvas::Canvas() = default;

Canvas::~Canvas() = default;

Canvas::Canvas(const Canvas&)
{

}

void Canvas::operator=(const Canvas&)
{

}

RefPtr<Canvas> Canvas::Create(uint32_t width, uint32_t height, BitmapFormat format, Surface* surface)
{
	return AdoptRef<Canvas>(*new CanvasImpl(width, height, format, surface));
}

}
