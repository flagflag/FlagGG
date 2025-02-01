#include "Ultralight/private/Image.h"
#include "Ultralight/Bitmap.h"
#include "Ultralight/private/util/RefCountedImpl.h"

namespace ultralight
{

class ImageImpl : public Image, public RefCountedImpl<ImageImpl>
{
public:
	REF_COUNTED_IMPL(ImageImpl);

	ImageImpl()
	{

	}

	~ImageImpl() override
	{

	}

	virtual void SetFrame(uint32_t frame_id, uint32_t frame_count, RefPtr<Bitmap> bitmap,
		bool is_data_complete) override
	{
		frameId_ = frame_id;
		frameCount_ = frame_count;
		bitmap_ = bitmap;
	}

	virtual uint32_t frame_count() const override
	{
		return frameCount_;
	}

	virtual bool GetFrameSize(uint32_t frame_id, uint32_t& width, uint32_t& height) override
	{
		return false;
	}

	virtual void ClearFrame(uint32_t frame_id) override
	{

	}

private:
	uint32_t frameId_{};

	uint32_t frameCount_{};

	RefPtr<Bitmap> bitmap_;
};

RefPtr<Image> Image::Create()
{
	return RefPtr<Image>(new ImageImpl());
}

}
