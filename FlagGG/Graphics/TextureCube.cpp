#include "Graphics/TextureCube.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/GraphicsDef.h"
#include "GfxDevice/GfxTexture.h"
#include "Config/LJSONFile.h"
#include "Resource/ResourceCache.h"
#include "Core/ObjectFactory.h"
#include "Math/Math.h"
#include "Log.h"

#include <d3dx11.h>

namespace FlagGG
{

REGISTER_TYPE_FACTORY(TextureCube);

TextureCube::TextureCube() :
	Texture()
{ }

bool TextureCube::SetSize(UInt32 size, TextureFormat format,
	TextureUsage usage/* = TEXTURE_STATIC*/, Int32 multiSample/* = 1*/)
{
	if (size <= 0)
	{
		FLAGGG_LOG_ERROR("Zero or negative texture dimensions.");
		return false;
	}

	multiSample = Clamp(multiSample, 1, 16);
	if (multiSample > 1 && usage < TEXTURE_RENDERTARGET)
	{
		FLAGGG_LOG_ERROR("Multisampling is only supported for rendertarget or depth-stencil textures");
		return false;
	}

	if (usage >= TEXTURE_RENDERTARGET)
	{
		for (UInt32 i = 0; i < MAX_CUBEMAP_FACES; ++i)
		{
			faceMemoryUse_[i] = 0u;
		}
	}

	gfxTexture_->SetFormat(format);
	gfxTexture_->SetWidth(size);
	gfxTexture_->SetHeight(size);
	gfxTexture_->SetDepth(1u);
	gfxTexture_->SetLayers(0);
	gfxTexture_->SetCube(true);
	gfxTexture_->SetMultiSample(multiSample);
	gfxTexture_->SetAutoResolve(false);
	gfxTexture_->SetUsage(usage);
	gfxTexture_->Apply(nullptr);

	return true;
}

bool TextureCube::SetData(CubeMapFace face, UInt32 level, Int32 x, Int32 y, Int32 width, Int32 height, const void* data)
{
	gfxTexture_->UpdateTextureSubRegion(data, face, level, x, y, width, height);
	return true;
}

bool TextureCube::SetData(CubeMapFace face, Image* image, bool useAlpha/* = false*/)
{
	if (!image)
	{
		FLAGGG_LOG_ERROR("Null image, can not load texture.");
		return false;
	}

	SharedPtr<Image> mipImage;
	UInt32 memoryUse = sizeof(TextureCube);
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

		if (levelWidth != levelHeight)
		{
			FLAGGG_LOG_ERROR("Cube texture width not equal to height.");
			return false;
		}

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

		// Create the texture when face 0 is being loaded, check that rest of the faces are same size & format
		if (!face)
		{
			// If image was previously compressed, reset number of requested levels to avoid error if level count is too high for new size
			//if (IsCompressed() && desc.requestedLevels_ > 1)
			//	requestedLevels_ = 0;

			if (!SetSize(levelWidth, format))
			{
				return false;
			}
		}
		else
		{
			if (levelWidth != desc.width_ || format != desc.format_)
			{
				FLAGGG_LOG_ERROR("Cube texture face does not match size or format of face 0.");
				return false;
			}
		}

		for (UInt32 i = 0; i < desc.levels_; ++i)
		{
			SetData(face, i, 0, 0, levelWidth, levelHeight, levelData);
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

		if (width != height)
		{
			FLAGGG_LOG_ERROR("Cube texture width not equal to height.");
			return false;
		}

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

		// Create the texture when face 0 is being loaded, assume rest of the faces are same size & format
		if (!face)
		{
			SetNumLevels(Max((levels - mipsToSkip), 1U));
			if (!SetSize(width, format))
			{
				return false;
			}
		}
		else
		{
			if (width != desc.width_ || format != desc.format_)
			{
				FLAGGG_LOG_ERROR("Cube texture face does not match size or format of face 0.");
				return false;
			}
		}

		for (UInt32 i = 0; i < desc.levels_ && i < levels - mipsToSkip; ++i)
		{
			CompressedLevel level = image->GetCompressedLevel(i + mipsToSkip);
			if (!needDecompress)
			{
				SetData(face, i, 0, 0, level.width_, level.height_, level.data_);
				memoryUse += level.rows_ * level.rowSize_;
			}
			else
			{
				UInt8* rgbaData = new UInt8[level.width_ * level.height_ * 4];
				level.Decompress(rgbaData);
				SetData(face, i, 0, 0, level.width_, level.height_, rgbaData);
				memoryUse += level.width_ * level.height_ * 4;
				delete[] rgbaData;
			}
		}
	}

	faceMemoryUse_[face] = memoryUse;
	UInt32 finalMemoryUse = 0u;
	for (UInt32 i = 0; i < MAX_CUBEMAP_FACES; ++i)
		finalMemoryUse += faceMemoryUse_[i];
	SetMemoryUse(finalMemoryUse);

	return true;
}

bool TextureCube::LoadDDS(IOFrame::Buffer::IOBuffer* stream)
{
	Image image;
	if (!image.LoadStream(stream))
	{
		FLAGGG_LOG_ERROR("Failed to load cube texture image.");
		return false;
	}

	Image* currentImage = &image;

	for (unsigned face = FACE_POSITIVE_X; face < MAX_CUBEMAP_FACES; ++face)
	{
		SetData((CubeMapFace)face, currentImage);
		currentImage = currentImage->GetNextSibling();
	}

	return true;
}

bool TextureCube::LoadMulti2DFace(IOFrame::Buffer::IOBuffer* stream)
{
	LJSONFile imageConfig;
	if (!imageConfig.LoadStream(stream))
	{
		FLAGGG_LOG_ERROR("Failed to load cube texture config.");
		return false;
	}

	const LJSONValue& value = imageConfig.GetRoot();
	if (!value.IsArray())
	{
		FLAGGG_LOG_ERROR("Illegal cube texture config.");
		return false;
	}

	for (UInt32 i = 0; i < value.Size() && i < MAX_CUBEMAP_FACES; ++i)
	{
		auto* cache = GetSubsystem<ResourceCache>();
		Image* image = cache->GetResource<Image>(value[i].GetString());
		if (!image)
		{
			return false;
		}

		if (!SetData(static_cast<CubeMapFace>(i), image))
		{
			return false;
		}
	}

	return true;
}

bool TextureCube::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	String fileID;
	fileID.Resize(4);
	if (stream->ReadStream(&fileID[0], 4) == 4)
	{
		stream->Seek(stream->GetIndex() - 4);

		if (fileID == "DDS ")
		{
			return LoadDDS(stream);
		}
	}
	return LoadMulti2DFace(stream);
}

bool TextureCube::EndLoad()
{
	SetGpuTag(GetName());
	return true;
}

bool TextureCube::GetData(CubeMapFace face, UInt32 level, void* dest)
{
	//if (!Texture::IsValid())
	//{
	//	FLAGGG_LOG_ERROR("Invalid TextureCube cannot get data.");
	//	return false;
	//}

	//if (!dest)
	//{
	//	FLAGGG_LOG_ERROR("Null destination for getting data.");
	//	return false;
	//}

	//if (level >= levels_)
	//{
	//	FLAGGG_LOG_ERROR("Illegal mip level for getting data.");
	//	return false;
	//}

	//if (multiSample_ > 1 && !autoResolve_)
	//{
	//	FLAGGG_LOG_ERROR("Can not get data from multisampled texture without autoresolve.");
	//	return false;
	//}

	//Int32 levelWidth = GetLevelWidth(level);
	//Int32 levelHeight = GetLevelHeight(level);

	//D3D11_TEXTURE2D_DESC textureDesc;
	//memset(&textureDesc, 0, sizeof textureDesc);
	//textureDesc.Width = (UINT)levelWidth;
	//textureDesc.Height = (UINT)levelHeight;
	//textureDesc.MipLevels = 1;
	//textureDesc.ArraySize = 1;
	//textureDesc.Format = (DXGI_FORMAT)format_;
	//textureDesc.SampleDesc.Count = 1;
	//textureDesc.SampleDesc.Quality = 0;
	//textureDesc.Usage = D3D11_USAGE_STAGING;
	//textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	//ID3D11Texture2D* stagingTexture = nullptr;
	//HRESULT hr = RenderEngine::Instance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &stagingTexture);
	//if (FAILED(hr))
	//{
	//	FLAGGG_LOG_ERROR("Failed to create staging texture for GetData", hr);
	//	SAFE_RELEASE(stagingTexture);
	//	return false;
	//}

	//ID3D11Resource* srcResource = (ID3D11Resource*)(resolveTexture_ ? resolveTexture_ : GetObject<ID3D11Resource>());
	//UInt32 srcSubResource = D3D11CalcSubresource(level, face, levels_);

	//D3D11_BOX srcBox;
	//srcBox.left = 0;
	//srcBox.right = (UINT)levelWidth;
	//srcBox.top = 0;
	//srcBox.bottom = (UINT)levelHeight;
	//srcBox.front = 0;
	//srcBox.back = 1;
	//RenderEngine::Instance()->GetDeviceContext()->CopySubresourceRegion(stagingTexture, 0, 0, 0, 0, srcResource,
	//	srcSubResource, &srcBox);

	//D3D11_MAPPED_SUBRESOURCE mappedData;
	//mappedData.pData = nullptr;
	//UInt32 rowSize = GetRowDataSize(levelWidth);
	//UInt32 numRows = (UInt32)(IsCompressed() ? (levelHeight + 3) >> 2 : levelHeight);

	//hr = RenderEngine::Instance()->GetDeviceContext()->Map((ID3D11Resource*)stagingTexture, 0, D3D11_MAP_READ, 0, &mappedData);
	//if (FAILED(hr) || !mappedData.pData)
	//{
	//	FLAGGG_LOG_ERROR("Failed to map staging texture for GetData", hr);
	//	SAFE_RELEASE(stagingTexture);
	//	return false;
	//}

	//for (UInt32 row = 0; row < numRows; ++row)
	//{
	//	memcpy((uint8_t*)dest + row * rowSize, (uint8_t*)mappedData.pData + row * mappedData.RowPitch, rowSize);
	//}

	//RenderEngine::Instance()->GetDeviceContext()->Unmap((ID3D11Resource*)stagingTexture, 0);
	//SAFE_RELEASE(stagingTexture);

	//return true;
	return false;
}

SharedPtr<Image> TextureCube::GetImage(CubeMapFace face)
{
	//SharedPtr<Image> image(new Image());
	//if (format_ != RenderEngine::GetRGBAFormat() && format_ != RenderEngine::GetRGBFormat())
	//{
	//	FLAGGG_LOG_ERROR("Unsupported texture format, can not convert to Image");
	//	return nullptr;
	//}

	//// image->SetSize(width_, height_, GetComponents());
	//if (format_ == RenderEngine::GetRGBAFormat())
	//	image->SetSize(width_, height_, 4);
	//else if (format_ == RenderEngine::GetRGBFormat())
	//	image->SetSize(width_, height_, 3);
	//GetData(face, 0, image->GetData());

	//return image;
	return nullptr;
}

}