#include "Ultralight/private/Image.h"
#include "Ultralight/Bitmap.h"
#include "Ultralight/private/util/RefCountedImpl.h"

#include <Graphics/Texture2D.h>
#include <Container/HashMap.h>

using namespace FlagGG;

namespace ultralight
{

// 内部实现使用引擎Texture2D
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
		frameCount_ = frame_count;

		bitmap->SwapRedBlueChannels();
		
		auto& texture = frameIdToTexture_[frame_id];
		if (!texture)
			texture = new Texture2D();

		texture->SetSize(bitmap->width(), bitmap->height(),
			bitmap->format() == BitmapFormat::BGRA8_UNORM_SRGB ? TEXTURE_FORMAT_RGBA8 : TEXTURE_FORMAT_R8);
		
		const void* data = ((const Bitmap*)bitmap.get())->LockPixels();
		texture->SetData(0, 0, 0, bitmap->width(), bitmap->height(), data);
		((const Bitmap*)bitmap.get())->UnlockPixels();
	}

	virtual uint32_t frame_count() const override
	{
		return frameCount_;
	}

	virtual bool GetFrameSize(uint32_t frame_id, uint32_t& width, uint32_t& height) override
	{
		auto it = frameIdToTexture_.Find(frame_id);
		if (it == frameIdToTexture_.End())
			return false;

		width = it->second_->GetWidth();
		height = it->second_->GetHeight();

		return true;
	}

	virtual void ClearFrame(uint32_t frame_id) override
	{
		frameIdToTexture_.Erase(frame_id);
	}

	Texture2D* GetInnerTexture(uint32_t frameId)
	{
		auto it = frameIdToTexture_.Find(frameId);
		if (it != frameIdToTexture_.End())
			return it->second_;
		return nullptr;
	}

private:
	// 缓存FrameId => Texture2D
	HashMap<uint32_t, SharedPtr<Texture2D>> frameIdToTexture_;

	uint32_t frameCount_{};
};

RefPtr<Image> Image::Create()
{
	return RefPtr<Image>(new ImageImpl());
}

extern Texture2D* GetImageInnerTexture(Image* image, uint32_t frame_id)
{
	auto* imageImpl = dynamic_cast<ImageImpl*>(image);
	return imageImpl ? imageImpl->GetInnerTexture(frame_id) : nullptr;
}

}
