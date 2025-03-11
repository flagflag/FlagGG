#include "Texture2DArray.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/GraphicsDef.h"
#include "GfxDevice/GfxTexture.h"
#include "Resource/ResourceCache.h"
#include "Core/ObjectFactory.h"
#include "Log.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(Texture2DArray);

Texture2DArray::Texture2DArray()
	: Texture()
{

}

bool Texture2DArray::SetSize(UInt32 layers, Int32 width, Int32 height, TextureFormat format, TextureUsage usage, Int32 multiSample, bool autoResolve)
{
	if (width <= 0 || height <= 0)
	{
		FLAGGG_LOG_ERROR("Zero or negative texture dimensions.");
		return false;
	}

	multiSample = Clamp(multiSample, 1, 16);
	if (multiSample == 1)
	{
		autoResolve = false;
	}
	else if (multiSample > 1 && usage < TEXTURE_RENDERTARGET)
	{
		FLAGGG_LOG_ERROR("Multisampling is only supported for rendertarget or depth-stencil textures");
		return false;
	}

	const UInt32 currentLayers = layers ? layers : gfxTexture_->GetDesc().layers_;

	layerMemoryUse_.Resize(currentLayers);
	for (unsigned i = 0; i < currentLayers; ++i)
		layerMemoryUse_[i] = 0;

	gfxTexture_->SetFormat(format);
	gfxTexture_->SetWidth(width);
	gfxTexture_->SetHeight(height);
	gfxTexture_->SetDepth(1u);
	gfxTexture_->SetLayers(currentLayers);
	gfxTexture_->SetCube(false);
	gfxTexture_->SetMultiSample(multiSample);
	gfxTexture_->SetAutoResolve(autoResolve);
	gfxTexture_->SetUsage(usage);
	gfxTexture_->Apply(nullptr);

	return true;
}

bool Texture2DArray::SetData(UInt32 layer, UInt32 level, Int32 x, Int32 y, Int32 width, Int32 height, const void* data)
{
	gfxTexture_->UpdateTextureSubRegion(data, layer, level, x, y, width, height);
	return true;
}

bool Texture2DArray::SetData(UInt32 layer, Image* image, bool useAlpha)
{
	if (!image)
	{
		FLAGGG_LOG_ERROR("Null image, can not load texture.");
		return false;
	}

	// Use a shared ptr for managing the temporary mip images created during this function
	SharedPtr<Image> mipImage;
	UInt32 memoryUse = sizeof(Texture2DArray);
	MaterialQuality quality = RenderEngine::Instance().GetTextureQuality();

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

		UInt8* levelData = image->GetData();
		Int32 levelWidth = image->GetWidth();
		Int32 levelHeight = image->GetHeight();
		TextureFormat format = TEXTURE_FORMAT_UNKNOWN;

		// Discard unnecessary mip levels
		for (UInt32 i = 0; i < mipsToSkip_[quality]; ++i)
		{
			mipImage = image->GetNextLevel(); image = mipImage;
			levelData = image->GetData();
			levelWidth = image->GetWidth();
			levelHeight = image->GetHeight();
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

		// Create the texture array when layer 0 is being loaded, check that rest of the layers are same size & format
		if (!layer)
		{
			// If image was previously compressed, reset number of requested levels to avoid error if level count is too high for new size
			//if (IsCompressed() && requestedLevels_ > 1)
			//	requestedLevels_ = 0;

			// Create the texture array (the number of layers must have been already set)
			if (!SetSize(0, levelWidth, levelHeight, format))
			{
				return false;
			}
		}
		else
		{
			if (levelWidth != desc.width_ || levelHeight != desc.height_ || format != desc.format_)
			{
				FLAGGG_LOG_ERROR("Texture array layer does not match size or format of layer 0");
				return false;
			}
		}

		for (UInt32 i = 0; i < desc.levels_; ++i)
		{
			SetData(layer, i, 0, 0, levelWidth, levelHeight, levelData);
			memoryUse += levelWidth * levelHeight * components;

			if (i < desc.levels_ - 1)
			{
				mipImage = image->GetNextLevel(); image = mipImage;
				levelData = image->GetData();
				levelWidth = image->GetWidth();
				levelHeight = image->GetHeight();
			}
		}
	}
	else
	{
		Int32 width = image->GetWidth();
		Int32 height = image->GetHeight();
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
		while (mipsToSkip && (width / (1 << mipsToSkip) < 4 || height / (1 << mipsToSkip) < 4))
			--mipsToSkip;
		width /= (1 << mipsToSkip);
		height /= (1 << mipsToSkip);

		// Create the texture array when layer 0 is being loaded, assume rest of the layers are same size & format
		if (!layer)
		{
			SetNumLevels(Max((levels - mipsToSkip), 1U));
			if (!SetSize(0, width, height, format))
			{
				return false;
			}
		}
		else
		{
			if (width != desc.width_ || height != desc.height_ || format != desc.format_)
			{
				FLAGGG_LOG_ERROR("Texture array layer does not match size or format of layer 0");
				return false;
			}
		}

		for (UInt32 i = 0; i < desc.levels_ && i < levels - mipsToSkip; ++i)
		{
			CompressedLevel level = image->GetCompressedLevel(i + mipsToSkip);
			if (!needDecompress)
			{
				SetData(layer, i, 0, 0, level.width_, level.height_, level.data_);
				memoryUse += level.rows_ * level.rowSize_;
			}
			else
			{
				UInt8* rgbaData = new UInt8[level.width_ * level.height_ * 4];
				level.Decompress(rgbaData);
				SetData(layer, i, 0, 0, level.width_, level.height_, rgbaData);
				memoryUse += level.width_ * level.height_ * 4;
				delete[] rgbaData;
			}
		}
	}

	layerMemoryUse_[layer] = memoryUse;
	UInt32 totalMemoryUse = sizeof(Texture2DArray) + layerMemoryUse_.Capacity() * sizeof(UInt32);
	for (UInt32 i = 0; i < desc.layers_; ++i)
		totalMemoryUse += layerMemoryUse_[i];
	SetMemoryUse(totalMemoryUse);

	return true;
}

bool Texture2DArray::GetData(UInt32 layer, UInt32 level, void* dest)
{
	return false;
}

SharedPtr<Image> Texture2DArray::GetImage()
{
	return nullptr;
}

bool Texture2DArray::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	Image image;
	if (!image.LoadStream(stream))
	{
		FLAGGG_LOG_ERROR("Failed to load image.");
		return false;
	}

	Image* currentImage = &image;
	UInt32 layers = 0;

	while (currentImage)
	{
		++layers;
		currentImage = currentImage->GetNextSibling();
	}

	gfxTexture_->SetLayers(layers);
	currentImage = &image;

	for (UInt32 layer = 0; layer < layers; ++layer)
	{
		SetData(layer, currentImage);
		currentImage = currentImage->GetNextSibling();
	}

	if (gfxTexture_)
		gfxTexture_->SetGpuTag(GetName());

	return true;
}

bool Texture2DArray::EndLoad()
{
	SetGpuTag(GetName());
	return true;
}

}
