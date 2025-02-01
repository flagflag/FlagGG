#include "Ultralight/platform/Surface.h"

namespace ultralight
{

Surface::Surface() = default;

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

}

BitmapSurface::~BitmapSurface()
{

}

uint32_t BitmapSurface::width() const
{
	return 0;
}

uint32_t BitmapSurface::height() const
{
	return 0;
}

uint32_t BitmapSurface::row_bytes() const
{
	return 0;
}

size_t BitmapSurface::size() const
{
	return 0;
}

void* BitmapSurface::LockPixels()
{
	return nullptr;
}

void BitmapSurface::UnlockPixels()
{

}

void BitmapSurface::Resize(uint32_t width, uint32_t height)
{

}

RefPtr<Bitmap> BitmapSurface::bitmap()
{
	return nullptr;
}

UExport SurfaceFactory* GetBitmapSurfaceFactory()
{
	return nullptr;
}

}
