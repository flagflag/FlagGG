#include "Graphics/Texture3D.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/GraphicsDef.h"
#include "GfxDevice/GfxTexture.h"
#include "Math/Math.h"
#include "Log.h"

#include <d3dx11.h>

namespace FlagGG
{

bool Texture3D::SetSize(Int32 width, Int32 height, Int32 depth, TextureFormat format, TextureUsage usage/* = TEXTURE_STATIC*/)
{
	if (width <= 0 || height <= 0 || depth <= 0)
	{
		FLAGGG_LOG_ERROR("Zero or negative texture dimensions.");
		return false;
	}

	if (usage >= TEXTURE_RENDERTARGET)
	{
		FLAGGG_LOG_ERROR("Rendertarget or depth-stencil usage not supported for 3D textures.");
		return false;
	}

	gfxTexture_->SetFormat(format);
	gfxTexture_->SetWidth(width);
	gfxTexture_->SetHeight(height);
	gfxTexture_->SetDepth(depth);
	gfxTexture_->SetMultiSample(1u);
	gfxTexture_->SetAutoResolve(false);
	gfxTexture_->SetUsage(usage);
	gfxTexture_->Apply(nullptr);

	return true;
}

bool Texture3D::SetData(UInt32 level, Int32 x, Int32 y, Int32 z, Int32 width, Int32 height, Int32 depth, const void* data)
{
	gfxTexture_->UpdateTextureSubRegion(data, 0u, level, x, y, z, width, height, depth);
	return true;
}

bool Texture3D::SetData(Image* image, bool useAlpha/* = false*/)
{
	if (!image)
	{
		FLAGGG_LOG_ERROR("Null image, can not load texture.");
		return false;
	}

	SharedPtr<Image> mipImage;
	UInt32 memoryUse = sizeof(Texture3D);
	MaterialQuality quality = RenderEngine::Instance()->GetTextureQuality();

	const TextureDesc& desc = gfxTexture_->GetDesc();

	if (!image->IsCompressed())
	{
		// Convert unsuitable formats to RGBA
		UInt32 components = image->GetComponents();
		if ((components == 1 && !useAlpha) || components == 2 || components == 3)
		{
			mipImage = image->ConvertToRGBA(); image = mipImage;
			if (!image)
				return false;
			components = image->GetComponents();
		}

		uint8_t* levelData = image->GetData();
		Int32 levelWidth = image->GetWidth();
		Int32 levelHeight = image->GetHeight();
		Int32 levelDepth = image->GetDepth();
		TextureFormat format = TEXTURE_FORMAT_UNKNOWN;

		// Discard unnecessary mip levels
		for (UInt32 i = 0; i < mipsToSkip_[quality]; ++i)
		{
			mipImage = image->GetNextLevel(); image = mipImage;
			levelData = image->GetData();
			levelWidth = image->GetWidth();
			levelHeight = image->GetHeight();
			levelDepth = image->GetDepth();
		}

		switch (components)
		{
		case 1:
			format = TEXTURE_FORMAT_A8;
			break;

		case 4:
			format = TEXTURE_FORMAT_RGBA8;
			break;

		default: break;
		}

		// If image was previously compressed, reset number of requested levels to avoid error if level count is too high for new size
		//if (IsCompressed() && desc.requestedLevels_ > 1)
		//	requestedLevels_ = 0;
		if (!SetSize(levelWidth, levelHeight, levelDepth, format))
		{
			return false;
		}

		for (UInt32 i = 0; i < desc.levels_; ++i)
		{
			SetData(i, 0, 0, 0, levelWidth, levelHeight, levelDepth, levelData);
			memoryUse += levelWidth * levelHeight * levelDepth * components;

			if (i < desc.levels_ - 1)
			{
				mipImage = image->GetNextLevel(); image = mipImage;
				levelData = image->GetData();
				levelWidth = image->GetWidth();
				levelHeight = image->GetHeight();
				levelDepth = image->GetDepth();
			}
		}
	}
	else
	{
		Int32 width = image->GetWidth();
		Int32 height = image->GetHeight();
		Int32 depth = image->GetDepth();
		UInt32 levels = image->GetNumCompressedLevels();
		TextureFormat format = RenderEngine::GetFormat(image->GetCompressedFormat());
		bool needDecompress = false;

		if (format == TEXTURE_FORMAT_UNKNOWN)
		{
			format = TEXTURE_FORMAT_RGBA8;
			needDecompress = true;
		}

		UInt32 mipsToSkip = mipsToSkip_[quality];
		if (mipsToSkip >= levels)
			mipsToSkip = levels - 1;
		while (mipsToSkip && (width / (1 << mipsToSkip) < 4 || height / (1 << mipsToSkip) < 4 || depth / (1 << mipsToSkip) < 4))
			--mipsToSkip;
		width /= (1 << mipsToSkip);
		height /= (1 << mipsToSkip);
		depth /= (1 << mipsToSkip);

		SetNumLevels(Max((levels - mipsToSkip), 1U));
		if (!SetSize(width, height, depth, format))
		{
			return false;
		}

		for (UInt32 i = 0; i < desc.levels_ && i < levels - mipsToSkip; ++i)
		{
			CompressedLevel level = image->GetCompressedLevel(i + mipsToSkip);
			if (!needDecompress)
			{
				SetData(i, 0, 0, 0, level.width_, level.height_, level.depth_, level.data_);
				memoryUse += level.depth_ * level.rows_ * level.rowSize_;
			}
			else
			{
				uint8_t* rgbaData = new uint8_t[level.width_ * level.height_ * level.depth_ * 4];
				level.Decompress(rgbaData);
				SetData(i, 0, 0, 0, level.width_, level.height_, level.depth_, rgbaData);
				memoryUse += level.width_ * level.height_  * level.depth_ * 4;
				delete[] rgbaData;
			}
		}
	}

	SetMemoryUse(memoryUse);

	return true;
}

bool Texture3D::GetData(UInt32 level, void* dest)
{
	return false;
}

bool Texture3D::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	return false;
}

bool Texture3D::EndLoad()
{
	return false;
}

}
