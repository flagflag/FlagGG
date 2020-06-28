#include "Graphics/Texture3D.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/GraphicsDef.h"
#include "Math/Math.h"
#include "Log.h"

#include <d3dx11.h>

namespace FlagGG
{
	namespace Graphics
	{

		bool Texture3D::Create()
		{
			//Release();

			//if (!width_ || !height_ || !depth_)
			//{
			//	return false;
			//}

			//levels_ = CheckMaxLevels(width_, height_, depth_, requestedLevels_);

			//D3D11_TEXTURE3D_DESC textureDesc;
			//memset(&textureDesc, 0, sizeof textureDesc);
			//textureDesc.Width = (UINT)width_;
			//textureDesc.Height = (UINT)height_;
			//textureDesc.Depth = (UINT)depth_;
			//textureDesc.MipLevels = usage_ != TEXTURE_DYNAMIC ? levels_ : 1;
			//textureDesc.Format = (DXGI_FORMAT)(sRGB_ ? GetSRGBFormat(format_) : format_);
			//textureDesc.Usage = usage_ == TEXTURE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
			//textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			//textureDesc.CPUAccessFlags = usage_ == TEXTURE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

			//ID3D11Texture3D* texture3D = nullptr;
			//HRESULT hr = RenderEngine::Instance()->GetDevice()->CreateTexture3D(&textureDesc, nullptr, &texture3D);
			//if (FAILED(hr))
			//{
			//	FLAGGG_LOG_ERROR("Failed to create texture3d.");
			//	SAFE_RELEASE(texture3D);
			//	return false;
			//}

			//ResetHandler(texture3D);

			//D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
			//memset(&resourceViewDesc, 0, sizeof resourceViewDesc);
			//resourceViewDesc.Format = (DXGI_FORMAT)GetSRVFormat(textureDesc.Format);
			//resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
			//resourceViewDesc.Texture3D.MipLevels = usage_ != TEXTURE_DYNAMIC ? (UINT)levels_ : 1;
			//
			//hr = RenderEngine::Instance()->GetDevice()->CreateShaderResourceView(GetObject<ID3D11Resource>(), &resourceViewDesc, &shaderResourceView_);
			//if (FAILED(hr))
			//{
			//	FLAGGG_LOG_ERROR("Failed to create shader resource view.");
			//	return false;
			//}

			return true;
		}

		bool Texture3D::SetSize(Int32 width, Int32 height, Int32 depth, UInt32 format, TextureUsage usage/* = TEXTURE_STATIC*/)
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

			usage_ = usage;

			width_ = width;
			height_ = height;
			depth_ = depth;
			format_ = format;

			return Create();
		}

		bool Texture3D::SetData(UInt32 level, Int32 x, Int32 y, Int32 z, Int32 width, Int32 height, Int32 depth, const void* data)
		{
			//if (!data)
			//{
			//	FLAGGG_LOG_ERROR("Texture3D ==> set nullptr data.");
			//	return false;
			//}

			//if (level >= levels_)
			//{
			//	FLAGGG_LOG_ERROR("Texture3D ==> illegal mip level.");
			//	return false;
			//}

			//Int32 levelWidth = GetLevelWidth(level);
			//Int32 levelHeight = GetLevelHeight(level);
			//Int32 levelDepth = GetLevelDepth(level);
			//if (x < 0 || x + width > levelWidth || y < 0 || y + height >levelHeight || z < 0 || z + depth > levelDepth || width <= 0 || height <= 0 || depth <= 0)
			//{
			//	FLAGGG_LOG_ERROR("Texture3D ==> illegal dimensions.");
			//	return false;
			//}

			//if (IsCompressed())
			//{
			//	x &= ~3;
			//	y &= ~3;
			//	width += 3;
			//	width &= 0xfffffffc;
			//	height += 3;
			//	height &= 0xfffffffc;
			//}

			//const uint8_t* src = static_cast<const uint8_t*>(data);
			//UInt32 rowSize = GetRowDataSize(width);
			//UInt32 rowStart = GetRowDataSize(x);
			//UInt32 subResource = D3D11CalcSubresource(level, 0, levels_);

			//if (usage_ == TEXTURE_DYNAMIC)
			//{
			//	if (IsCompressed())
			//	{
			//		height = (height + 3) >> 2;
			//		y >>= 2;
			//	}

			//	D3D11_MAPPED_SUBRESOURCE mappedData;
			//	mappedData.pData = nullptr;

			//	HRESULT hr = RenderEngine::Instance()->GetDeviceContext()->Map(GetObject<ID3D11Resource>(),
			//		subResource, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

			//	if (FAILED(hr) || !mappedData.pData)
			//	{
			//		FLAGGG_LOG_ERROR("Failed to update texture resource.");
			//		return false;
			//	}
			//	else
			//	{
			//		for (Int32 page = 0; page < depth; ++page)
			//		{
			//			for (Int32 row = 0; row < height; ++row)
			//				memcpy((uint8_t*)mappedData.pData + (page + z) * mappedData.DepthPitch + (row + y) * mappedData.RowPitch + rowStart,
			//					src + row * rowSize, rowSize);
			//		}

			//		RenderEngine::Instance()->GetDeviceContext()->Unmap(GetObject<ID3D11Resource>(), subResource);
			//	}
			//}
			//else
			//{
			//	if (IsCompressed())
			//		levelHeight = (levelHeight + 3) >> 2;

			//	D3D11_BOX destBox;
			//	destBox.left = (UINT)x;
			//	destBox.right = (UINT)(x + width);
			//	destBox.top = (UINT)y;
			//	destBox.bottom = (UINT)(y + height);
			//	destBox.front = (UINT)z;
			//	destBox.back = (UINT)(z + depth);

			//	RenderEngine::Instance()->GetDeviceContext()->UpdateSubresource(GetObject<ID3D11Resource>(), subResource, &destBox, data, rowSize, levelHeight * rowSize);
			//}

			return true;
		}

		bool Texture3D::SetData(FlagGG::Resource::Image* image, bool useAlpha/* = false*/)
		{
			//if (!image)
			//{
			//	FLAGGG_LOG_ERROR("Null image, can not load texture.");
			//	return false;
			//}

			//Container::SharedPtr<FlagGG::Resource::Image> mipImage;
			//UInt32 memoryUse = sizeof(Texture3D);
			//MaterialQuality quality = RenderEngine::Instance()->GetTextureQuality();

			//if (!image->IsCompressed())
			//{
			//	// Convert unsuitable formats to RGBA
			//	UInt32 components = image->GetComponents();
			//	if ((components == 1 && !useAlpha) || components == 2 || components == 3)
			//	{
			//		mipImage = image->ConvertToRGBA(); image = mipImage;
			//		if (!image)
			//			return false;
			//		components = image->GetComponents();
			//	}

			//	uint8_t* levelData = image->GetData();
			//	Int32 levelWidth = image->GetWidth();
			//	Int32 levelHeight = image->GetHeight();
			//	Int32 levelDepth = image->GetDepth();
			//	UInt32 format = 0;

			//	// Discard unnecessary mip levels
			//	for (UInt32 i = 0; i < mipsToSkip_[quality]; ++i)
			//	{
			//		mipImage = image->GetNextLevel(); image = mipImage;
			//		levelData = image->GetData();
			//		levelWidth = image->GetWidth();
			//		levelHeight = image->GetHeight();
			//		levelDepth = image->GetDepth();
			//	}

			//	switch (components)
			//	{
			//	case 1:
			//		format = RenderEngine::GetAlphaFormat();
			//		break;

			//	case 4:
			//		format = RenderEngine::GetRGBAFormat();
			//		break;

			//	default: break;
			//	}

			//	// If image was previously compressed, reset number of requested levels to avoid error if level count is too high for new size
			//	if (IsCompressed() && requestedLevels_ > 1)
			//		requestedLevels_ = 0;
			//	if (!SetSize(levelWidth, levelHeight, levelDepth, format))
			//	{
			//		return false;
			//	}

			//	for (UInt32 i = 0; i < levels_; ++i)
			//	{
			//		SetData(i, 0, 0, 0, levelWidth, levelHeight, levelDepth, levelData);
			//		memoryUse += levelWidth * levelHeight * levelDepth * components;

			//		if (i < levels_ - 1)
			//		{
			//			mipImage = image->GetNextLevel(); image = mipImage;
			//			levelData = image->GetData();
			//			levelWidth = image->GetWidth();
			//			levelHeight = image->GetHeight();
			//			levelDepth = image->GetDepth();
			//		}
			//	}
			//}
			//else
			//{
			//	Int32 width = image->GetWidth();
			//	Int32 height = image->GetHeight();
			//	Int32 depth = image->GetDepth();
			//	UInt32 levels = image->GetNumCompressedLevels();
			//	UInt32 format = RenderEngine::GetFormat(image->GetCompressedFormat());
			//	bool needDecompress = false;

			//	if (!format)
			//	{
			//		format = RenderEngine::GetRGBAFormat();
			//		needDecompress = true;
			//	}

			//	UInt32 mipsToSkip = mipsToSkip_[quality];
			//	if (mipsToSkip >= levels)
			//		mipsToSkip = levels - 1;
			//	while (mipsToSkip && (width / (1 << mipsToSkip) < 4 || height / (1 << mipsToSkip) < 4 || depth / (1 << mipsToSkip) < 4))
			//		--mipsToSkip;
			//	width /= (1 << mipsToSkip);
			//	height /= (1 << mipsToSkip);
			//	depth /= (1 << mipsToSkip);

			//	SetNumLevels(Math::Max((levels - mipsToSkip), 1U));
			//	if (!SetSize(width, height, depth, format))
			//	{
			//		return false;
			//	}

			//	for (UInt32 i = 0; i < levels_ && i < levels - mipsToSkip; ++i)
			//	{
			//		FlagGG::Resource::CompressedLevel level = image->GetCompressedLevel(i + mipsToSkip);
			//		if (!needDecompress)
			//		{
			//			SetData(i, 0, 0, 0, level.width_, level.height_, level.depth_, level.data_);
			//			memoryUse += level.depth_ * level.rows_ * level.rowSize_;
			//		}
			//		else
			//		{
			//			uint8_t* rgbaData = new uint8_t[level.width_ * level.height_ * level.depth_ * 4];
			//			level.Decompress(rgbaData);
			//			SetData(i, 0, 0, 0, level.width_, level.height_, level.depth_, rgbaData);
			//			memoryUse += level.width_ * level.height_  * level.depth_ * 4;
			//			delete[] rgbaData;
			//		}
			//	}
			//}

			//SetMemoryUse(memoryUse);

			return true;
		}

		bool Texture3D::GetData(UInt32 level, void* dest)
		{
			return true;
		}

		bool Texture3D::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
			return true;
		}

		bool Texture3D::EndLoad()
		{
			return true;
		}
	}
}
