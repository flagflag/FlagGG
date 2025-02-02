#include "Ultralight/platform/Surface.h"

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


BitmapSurface::BitmapSurface(uint32_t width, uint32_t height)
{
	RefPtr<Bitmap> bitmap = Bitmap::Create(width, height, BitmapFormat::BGRA8_UNORM_SRGB);
	auto* bitmapPtr = bitmap.get();
	bitmapPtr->AddRef();
	impl_ = bitmapPtr;
	dirty_bounds_ = IntRect{ 0, 0, int32_t(width), int32_t(height) };
}

BitmapSurface::~BitmapSurface()
{
	if (impl_)
	{
		((Bitmap*)impl_)->Release();
		impl_ = nullptr;
	}
}

uint32_t BitmapSurface::width() const
{
	return ((Bitmap*)impl_)->width();
}

uint32_t BitmapSurface::height() const
{
	return ((Bitmap*)impl_)->height();
}

uint32_t BitmapSurface::row_bytes() const
{
	return ((Bitmap*)impl_)->row_bytes();
}

size_t BitmapSurface::size() const
{
	return ((Bitmap*)impl_)->size();
}

void* BitmapSurface::LockPixels()
{
	return ((Bitmap*)impl_)->LockPixels();
}

void BitmapSurface::UnlockPixels()
{
	((Bitmap*)impl_)->UnlockPixels();
}

void BitmapSurface::Resize(uint32_t width, uint32_t height)
{
	dirty_bounds_ = IntRect{ 0, 0, int32_t(width), int32_t(height) };
}

RefPtr<Bitmap> BitmapSurface::bitmap()
{
	return RefPtr<Bitmap>((Bitmap*)impl_);
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
};

UExport SurfaceFactory* GetBitmapSurfaceFactory()
{
	static BitmapSurfaceFactory factory;
	return &factory;
}

}
