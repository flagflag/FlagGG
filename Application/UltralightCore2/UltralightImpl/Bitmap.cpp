#include "Ultralight/Bitmap.h"

#include "Ultralight/private/util/RefCountedImpl.h"

#include <Memory/Memory.h>

using namespace FlagGG;

namespace ultralight
{

class BitmapImpl : public Bitmap, public RefCountedImpl<BitmapImpl>
{
public:
	REF_COUNTED_IMPL(BitmapImpl);

	BitmapImpl()
		: pixelData_(nullptr)
		, width_(0u)
		, height_(0u)
		, format_(BitmapFormat::A8_UNORM)
		, readonlyPixels_(nullptr)
		, rowBytes_(0u)
	{

	}

	BitmapImpl(uint32_t width, uint32_t height, BitmapFormat format)
		: width_(width)
		, height_(height)
		, format_(format)
		, readonlyPixels_(nullptr)
	{
		rowBytes_ = width * bpp();
		pixelData_ = new char[size()];
	}

	BitmapImpl(uint32_t width, uint32_t height, BitmapFormat format, uint32_t alignment)
		: width_(width)
		, height_(height)
		, format_(format)
		, readonlyPixels_(nullptr)
	{
		rowBytes_ = width * bpp();
		pixelData_ = new char[size()];
	}

	BitmapImpl(uint32_t width, uint32_t height, BitmapFormat format,
		uint32_t row_bytes, const void* pixels, size_t size,
		bool should_copy)
		: width_(width)
		, height_(height)
		, format_(format)
	{
		if (should_copy)
		{
			readonlyPixels_ = nullptr;
			pixelData_ = new char[width * height * bpp()];
			rowBytes_ = width * bpp();

			if (row_bytes != rowBytes_)
			{
				const uint32_t minRowBytes = rowBytes_ < row_bytes ? rowBytes_ : row_bytes;

				const char* srcPixel = (const char*)pixels;
				char* destPixel = pixelData_;

				for (uint32_t row = 0; row < height; ++row)
				{
					srcPixel += row_bytes;
					destPixel += rowBytes_;
					Memory::Memcpy(destPixel, srcPixel, minRowBytes);
				}
			}
			else
			{
				Memory::Memcpy(pixelData_, pixels, width * height * bpp());
			}
		}
		else
		{
			pixelData_ = nullptr;
			readonlyPixels_ = (const char*)pixels;
			rowBytes_ = row_bytes;
		}
	}

	~BitmapImpl() override
	{
		if (pixelData_)
		{
			delete[] pixelData_;
		}
	}

	BitmapImpl& operator=(BitmapImpl&& rhs)
	{
		std::swap(pixelData_, rhs.pixelData_);
		std::swap(width_, rhs.width_);
		std::swap(height_, rhs.height_);
		std::swap(format_, rhs.format_);
		std::swap(rowBytes_, rhs.rowBytes_);
		std::swap(readonlyPixels_, rhs.readonlyPixels_);

		return *this;
	}

	///
	/// Get the width in pixels.
	///
	virtual uint32_t width() const override
	{
		return width_;
	}

	///
	/// Get the height in pixels.
	///
	virtual uint32_t height() const override
	{
		return height_;
	}

	///
	/// Get the bounds as an IntRect
	///
	virtual IntRect bounds() const override
	{
		IntRect rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = width_;
		rect.bottom = height_;
		return rect;
	}

	///
	/// Get the pixel format.
	///
	virtual BitmapFormat format() const override
	{
		return format_;
	}

	///
	/// Get the number of bytes per pixel.
	///
	virtual uint32_t bpp() const override
	{
		return format_ == BitmapFormat::A8_UNORM ? 1u : 4u;
	}

	///
	/// Get the number of bytes between each row of pixels.
	///
	/// @note  This value is usually calculated as width * bytes_per_pixel (bpp) but it may be larger
	///        due to alignment rules in the allocator.
	///
	virtual uint32_t row_bytes() const override
	{
		return rowBytes_;
	}

	///
	/// Get the size in bytes of the pixel buffer.
	///
	/// @note  Size is calculated as row_bytes() * height().
	///
	virtual size_t size() const override
	{
		return row_bytes() * height();
	}

	///
	/// Whether or not this Bitmap owns the pixel buffer and will destroy it at the end of its
	/// lifetime.
	///
	virtual bool owns_pixels() const override
	{
		return pixelData_;
	}

	///
	/// Lock the pixel buffer for reading/writing (safe version, automatically unlocks).
	///
	/// @return  A managed container that can be used to access the pixels (LockedPixels::data()).
	///          This container will automatically unlock the pixels when it goes out of scope.
	///
	virtual LockedPixels<RefPtr<Bitmap>> LockPixelsSafe() const override
	{
		RefPtr<Bitmap> thisRef((Bitmap*)this);
		return LockedPixels<RefPtr<Bitmap>>(thisRef);
	}

	///
	/// Lock the pixel buffer for reading/writing.
	///
	/// @return  A pointer to the pixel buffer.
	///
	virtual void* LockPixels() override
	{
		return pixelData_;
	}

	///
	/// Unlock the pixel buffer.
	///
	virtual void UnlockPixels() override
	{

	}

	///
	/// Lock the pixel buffer for reading/writing. (const)
	///
	/// @return  A const pointer to the pixel buffer.
	///
	virtual const void* LockPixels() const override
	{
		return pixelData_ ? pixelData_ : readonlyPixels_;
	}

	///
	/// Unlock the pixel buffer. (const)
	///
	virtual void UnlockPixels() const override
	{

	}

	///
	/// Get the raw pixel buffer.
	///
	/// @note  You should only call this if pixels are already locked.
	///
	virtual void* raw_pixels() override
	{
		return pixelData_;
	}

	///
	/// Whether or not this Bitmap is empty (no pixels allocated).
	///
	virtual bool IsEmpty() const override
	{
		return width_ == 0 || height_ == 0;
	}

	///
	/// Erase the Bitmap (set all pixels to 0).
	///
	virtual void Erase() override
	{
		uint32_t actualRowBytes = width_ * bpp();
		if (actualRowBytes != rowBytes_)
		{
			for (uint32_t row = 0; row < height_; ++row)
			{
				Memory::Memzero(pixelData_ + row * rowBytes_, actualRowBytes);
			}
		}
		else
		{
			Memory::Memzero(pixelData_, size());
		}
	}

	///
	/// Assign another bitmap to this one.
	///
	/// @param  bitmap  The bitmap to copy from.
	///
	virtual void Set(RefPtr<Bitmap> bitmap) override
	{
		const void* copyPixels = ((const Bitmap*)bitmap.get())->LockPixels();
		BitmapImpl newBitmap(bitmap->width(), bitmap->height(), bitmap->format(), bitmap->row_bytes(), copyPixels, bitmap->size(), true);
		((const Bitmap*)bitmap.get())->UnlockPixels();

		(*this) = std::move(newBitmap);
	}

	///
	/// Draw another bitmap to this bitmap.
	///
	/// @note  Formats do not need to match. Bitmap formats will be converted to one another
	///        automatically. Note that when converting from BGRA8 to A8, only the Blue channel will
	///        be used.
	///
	/// @param  src_rect    The source rectangle, relative to src bitmap.
	///
	/// @param  dest_rect   The destination rectangle, relative to this bitmap.
	///
	/// @param  src         The source bitmap.
	///
	/// @param  pad_repeat  Whether or not we should pad the drawn bitmap by one pixel of repeated
	///                     edge pixels from the source bitmap.
	///
	/// @return  Whether or not the operation succeeded (this can fail if the src_rect and/or
	///          dest_rect are invalid).
	///
	virtual bool DrawBitmap(IntRect src_rect, IntRect dest_rect, RefPtr<Bitmap> src, bool pad_repeat) override
	{
		if (src_rect.width() != dest_rect.width())
			return false;

		if (src_rect.height() != dest_rect.height())
			return false;

		if (format() != src->format())
			return false;

		const auto* bitmapImpl = dynamic_cast<BitmapImpl*>(src.get());

		const uint32_t copyRowBytes = src_rect.width() * bpp();
		const uint32_t copyHeight   = src_rect.height();
		const uint32_t srcRowBytes  = bitmapImpl->row_bytes();
		const uint32_t dstRowBytes  = row_bytes();

		const char* srcPixel = (const char*)bitmapImpl->LockPixels() + src_rect.x() * bpp()  + src_rect.y()  * srcRowBytes;
		char* destPixel      = (char*)LockPixels()                   + dest_rect.x() * bpp() + dest_rect.y() * dstRowBytes;

		for (uint32_t row = 0; row < copyHeight; ++row)
		{
			Memory::Memcpy(destPixel, srcPixel, copyRowBytes);
			srcPixel  += srcRowBytes;
			destPixel += dstRowBytes;
		}

		bitmapImpl->UnlockPixels();
		UnlockPixels();

		return true;
	}

	///
	/// Write this Bitmap out to a PNG image.
	///
	/// @param  path  The filepath to write to (opened with fopen())
	///
	/// @param  convert_to_rgba  The PNG format expects RGBA format but our bitmap is stored as BGRA,
	///                          set this to true to perform the conversion automatically.
	///
	/// @param  convert_to_straight_alpha  The PNG format expects semi-transparent values to be
	///                                    stored as straight alpha instead of premultiplied alpha,
	///                                    set this to true to perform the conversion automatically.
	///
	/// @return  Whether or not the operation succeeded.
	///
	virtual bool WritePNG(const char* path, bool convert_to_rgba = true,
		bool convert_to_straight_alpha = true) const override
	{
		return false;
	}

	///
	/// Make a resized copy of this bitmap by writing to a pre-allocated destination bitmap.
	///
	/// @param  destination  The bitmap to store the result in, the width and height of the
	///                      destination will be used.
	///
	/// @param  high_quality  Whether or not a high quality resampling will be used during the
	///                       resize. (Otherwise, just uses fast nearest-neighbor sampling)
	///
	/// @return  Whether or not the operation succeeded. This operation is only valid if both formats
	///          are BitmapFormat::BGRA8_UNORM_SRGB and the source and destination are non-empty.
	///
	virtual bool Resample(RefPtr<Bitmap> destination, bool high_quality) override
	{
		return false;
	}

	///
	/// Convert a BGRA bitmap to RGBA bitmap and vice-versa by swapping the red and blue channels.
	///
	/// @note  Only valid if the format is BitmapFormat::BGRA8_UNORM_SRGB
	///
	virtual void SwapRedBlueChannels() override
	{
		if (format_ == BitmapFormat::BGRA8_UNORM_SRGB)
		{
			uint32_t* currPixel = (uint32_t*)pixelData_;
			uint32_t* tailPixel = currPixel + width_ * height_;

			while (currPixel < tailPixel)
			{
				uint32_t channelA = (*currPixel) & 0xFF;
				uint32_t channelB = (*currPixel) & 0xFF0000;

				(*currPixel) = (*currPixel) ^ channelA | (channelB >> 16);
				(*currPixel) = (*currPixel) ^ channelB | (channelA << 16);

				++currPixel;
			}
		}
	}

	///
	/// Convert a BGRA bitmap from premultiplied alpha (the default) to straight alpha.
	///
	/// @note  Only valid if the format is BitmapFormat::BGRA8_UNORM_SRGB
	///
	virtual void ConvertToStraightAlpha() override
	{

	}

	///
	/// Convert a BGRA bitmap from straight alpha to premultiplied alpha.
	///
	/// @note  Only valid if the format is BitmapFormat::BGRA8_UNORM_SRGB
	///
	virtual void ConvertToPremultipliedAlpha() override
	{

	}

private:
	char* pixelData_;

	uint32_t width_;

	uint32_t height_;

	BitmapFormat format_;

	uint32_t rowBytes_;

	const char* readonlyPixels_;
};

Bitmap::Bitmap() = default;

Bitmap::~Bitmap() = default;

Bitmap::Bitmap(const Bitmap&)
{

}

void Bitmap::operator=(const Bitmap&)
{

}

RefPtr<Bitmap> Bitmap::Create()
{
	return AdoptRef<Bitmap>(*new BitmapImpl());
}

RefPtr<Bitmap> Bitmap::Create(uint32_t width, uint32_t height, BitmapFormat format)
{
	return AdoptRef<Bitmap>(*new BitmapImpl(width, height, format));
}

RefPtr<Bitmap> Bitmap::Create(uint32_t width, uint32_t height, BitmapFormat format, uint32_t alignment)
{
	return AdoptRef<Bitmap>(*new BitmapImpl(width, height, format, alignment));
}

RefPtr<Bitmap> Bitmap::Create(uint32_t width, uint32_t height, BitmapFormat format,
	uint32_t row_bytes, const void* pixels, size_t size,
	bool should_copy)
{
	return AdoptRef<Bitmap>(*new BitmapImpl(width, height, format, row_bytes, pixels, size, should_copy));
}

RefPtr<Bitmap> Bitmap::Create(const Bitmap& bitmap)
{
	const void* copyPixels = bitmap.LockPixels();
	RefPtr<Bitmap> newBitmap = AdoptRef<Bitmap>(*new BitmapImpl(bitmap.width(), bitmap.height(), bitmap.format(), bitmap.row_bytes(), copyPixels, bitmap.size(), true));
	bitmap.UnlockPixels();
	return newBitmap;
}

}
