#include "Ultralight/platform/Surface.h"

#include <Graphics/Texture2D.h>
#include <Graphics/TextureCache.h>
#include <GfxDevice/GfxTextureUtils.h>
#include <GfxDevice/GfxTexture.h>

using namespace FlagGG;

namespace ultralight
{

Surface::Surface()
{
	dirty_bounds_.SetEmpty();
}

Surface::~Surface() = default;

void Surface::set_dirty_bounds(const IntRect& bounds)
{
	dirty_bounds_ = bounds;
}

IntRect Surface::dirty_bounds() const
{
	return dirty_bounds_;
}

void Surface::ClearDirtyBounds()
{
	dirty_bounds_.SetEmpty();
}

SurfaceFactory::~SurfaceFactory() = default;


struct BitmapSurfaceData
{
	SharedPtr<Texture2D> texture_;
	RefPtr<Bitmap> bitmap_;
};

BitmapSurface::BitmapSurface(uint32_t width, uint32_t height)
{
	auto* data = new BitmapSurfaceData();
	data->texture_ = GetSubsystem<TextureCache>()->GetTexture2D(width, height, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
	data->bitmap_ = Bitmap::Create(width, height, BitmapFormat::BGRA8_UNORM_SRGB);

	impl_ = data;

	dirty_bounds_ = IntRect{ 0, 0, int32_t(width), int32_t(height) };
}

BitmapSurface::~BitmapSurface()
{
	if (impl_)
	{
		delete reinterpret_cast<BitmapSurfaceData*>(impl_);
		impl_ = nullptr;
	}
}

uint32_t BitmapSurface::width() const
{
	return reinterpret_cast<BitmapSurfaceData*>(impl_)->texture_->GetWidth();
}

uint32_t BitmapSurface::height() const
{
	return reinterpret_cast<BitmapSurfaceData*>(impl_)->texture_->GetHeight();
}

uint32_t BitmapSurface::row_bytes() const
{
	return GfxTextureUtils::GetRowDataSize(TEXTURE_FORMAT_RGBA8, width());
}

size_t BitmapSurface::size() const
{
	return GfxTextureUtils::GetDataSize(TEXTURE_FORMAT_RGBA8, width(), height());
}

void* BitmapSurface::LockPixels()
{
	return reinterpret_cast<BitmapSurfaceData*>(impl_)->bitmap_->LockPixels();
}

void BitmapSurface::UnlockPixels()
{
	auto* data = reinterpret_cast<BitmapSurfaceData*>(impl_);
	data->bitmap_->UnlockPixels();

	data->texture_->SetData(0, 0, 0, data->texture_->GetWidth(), data->texture_->GetHeight(), data->bitmap_->raw_pixels());
}

void BitmapSurface::Resize(uint32_t width, uint32_t height)
{
	dirty_bounds_ = IntRect{ 0, 0, int32_t(width), int32_t(height) };

	auto* data= reinterpret_cast<BitmapSurfaceData*>(impl_);
	data->texture_ = GetSubsystem<TextureCache>()->GetTexture2D(width, height, TEXTURE_FORMAT_RGBA8, TEXTURE_RENDERTARGET);
	data->bitmap_ = Bitmap::Create(width, height, BitmapFormat::BGRA8_UNORM_SRGB);
}

RefPtr<Bitmap> BitmapSurface::bitmap()
{
	return reinterpret_cast<BitmapSurfaceData*>(impl_)->bitmap_;
}

class BitmapSurfaceFactory : public SurfaceFactory
{
public:
	///
	/// Create a native Surface with a certain width and height (in pixels).
	///
	virtual Surface* CreateSurface(uint32_t width, uint32_t height) override
	{
		return new BitmapSurface(width, height);
	}

	///
	/// Destroy a native Surface previously created by CreateSurface().
	///
	virtual void DestroySurface(Surface* surface) override
	{
		delete surface;
	}

	static Texture2D* GetInnerTexture(BitmapSurface* bitmapSurface)
	{
		return reinterpret_cast<BitmapSurfaceData*>(bitmapSurface->impl_)->texture_;
	}
};

UExport SurfaceFactory* GetBitmapSurfaceFactory()
{
	static BitmapSurfaceFactory factory;
	return &factory;
}

extern Texture2D* GetSurfaceInnerTexture(Surface* surface)
{
	if (!surface)
		return nullptr;

	auto* bitmapSurface = dynamic_cast<BitmapSurface*>(surface);
	if (!bitmapSurface)
		return nullptr;

	return BitmapSurfaceFactory::GetInnerTexture(bitmapSurface);
}

}
