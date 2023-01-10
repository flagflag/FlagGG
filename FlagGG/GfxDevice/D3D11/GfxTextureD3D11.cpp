#include "GfxTextureD3D11.h"
#include "GfxDeviceD3D11.h"
#include "GfxRenderSurfaceD3D11.h"
#include "GfxD3D11Defines.h"
#include "Log.h"

namespace FlagGG
{

GfxTextureD3D11::GfxTextureD3D11()
	: GfxTexture()
{

}

GfxTextureD3D11::~GfxTextureD3D11()
{
	ReleaseTexture();
}

UInt32 GfxTextureD3D11::GetSRGBFormat(UInt32 format)
{
	if (format == DXGI_FORMAT_R8G8B8A8_UNORM)
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	else if (format == DXGI_FORMAT_BC1_UNORM)
		return DXGI_FORMAT_BC1_UNORM_SRGB;
	else if (format == DXGI_FORMAT_BC2_UNORM)
		return DXGI_FORMAT_BC2_UNORM_SRGB;
	else if (format == DXGI_FORMAT_BC3_UNORM)
		return DXGI_FORMAT_BC3_UNORM_SRGB;
	return format;
}

UInt32 GfxTextureD3D11::GetDSVFormat(UInt32 format)
{
	if (format == DXGI_FORMAT_R24G8_TYPELESS)
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	else if (format == DXGI_FORMAT_R16_TYPELESS)
		return DXGI_FORMAT_D16_UNORM;
	else if (format == DXGI_FORMAT_R32_TYPELESS)
		return DXGI_FORMAT_D32_FLOAT;
	return format;
}

UInt32 GfxTextureD3D11::GetSRVFormat(UInt32 format)
{
	if (format == DXGI_FORMAT_R24G8_TYPELESS)
		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	else if (format == DXGI_FORMAT_R16_TYPELESS)
		return DXGI_FORMAT_R16_UNORM;
	else if (format == DXGI_FORMAT_R32_TYPELESS)
		return DXGI_FORMAT_R32_FLOAT;
	return format;
}

void GfxTextureD3D11::ReleaseTexture()
{
	D3D11_SAFE_RELEASE(d3d11Texture2D_);
	D3D11_SAFE_RELEASE(d3d11Texture3D_);
}

void GfxTextureD3D11::CreateTexture2D()
{
	GfxDeviceD3D11* gfxDevice = GfxDeviceD3D11::Instance();
	ID3D11Device* d3d11Device = gfxDevice->GetD3D11Device();

	D3D11_TEXTURE2D_DESC textureDesc;
	memset(&textureDesc, 0, sizeof textureDesc);
	textureDesc.Format = (DXGI_FORMAT)(textureDesc_.sRGB_ ? GetSRGBFormat(textureDesc_.format_) : textureDesc_.format_);

	if (textureDesc_.multiSample_ > 1 && gfxDevice->CheckMultiSampleSupport(textureDesc.Format, textureDesc_.multiSample_))
	{
		textureDesc_.multiSample_ = 1;
		textureDesc_.autoResolve_ = false;
	}

	if (textureDesc_.usage_ == TEXTURE_DEPTHSTENCIL)
	{
		textureDesc_.levels_ = 1;
	}
	else if (textureDesc_.usage_ == TEXTURE_RENDERTARGET && textureDesc_.levels_ != 1 && textureDesc_.multiSample_ == 1)
	{
		textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	textureDesc.Width = (UINT)textureDesc_.width_;
	textureDesc.Height = (UINT)textureDesc_.height_;
	textureDesc.MipLevels = (textureDesc_.multiSample_ == 1 && textureDesc_.usage_ != TEXTURE_DYNAMIC) ? textureDesc_.levels_ : 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = (UINT)textureDesc_.multiSample_;
	textureDesc.SampleDesc.Quality = gfxDevice->GetMultiSampleQuality(textureDesc.Format, textureDesc_.multiSample_);
	textureDesc.Usage = textureDesc_.usage_ == TEXTURE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (textureDesc_.usage_ == TEXTURE_RENDERTARGET)
	{
		textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	else if (textureDesc_.usage_ == TEXTURE_DEPTHSTENCIL)
	{
		textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	}
	textureDesc.CPUAccessFlags = textureDesc_.usage_ == TEXTURE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

	if (textureDesc_.usage_ == TEXTURE_DEPTHSTENCIL && textureDesc_.multiSample_ > 1 && d3d11Device->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_1)
	{
		textureDesc.BindFlags &= ~D3D11_BIND_SHADER_RESOURCE;
	}

	HRESULT hr = d3d11Device->CreateTexture2D(&textureDesc, nullptr, &d3d11Texture2D_);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("Failed to create texture2d.");
		D3D11_SAFE_RELEASE(d3d11Texture2D_);
		return;
	}

	if (textureDesc_.multiSample_ > 1 && textureDesc_.autoResolve_)
	{
		textureDesc.MipLevels = textureDesc_.levels_;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		if (textureDesc_.levels_ != 1)
		{
			textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}

		HRESULT hr = d3d11Device->CreateTexture2D(&textureDesc, nullptr, (ID3D11Texture2D**)&resolveTexture_);
		if (FAILED(hr))
		{
			FLAGGG_LOG_ERROR("Failed to create resolve texture.");
			D3D11_SAFE_RELEASE(resolveTexture_);
			return;
		}
	}

	if (textureDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
		memset(&resourceViewDesc, 0, sizeof resourceViewDesc);
		resourceViewDesc.Format = (DXGI_FORMAT)GetSRVFormat(textureDesc.Format);
		resourceViewDesc.ViewDimension = (textureDesc_.multiSample_ > 1 && !textureDesc_.autoResolve_) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		resourceViewDesc.Texture2D.MipLevels = textureDesc_.usage_ != TEXTURE_DYNAMIC ? (UINT)textureDesc_.levels_ : 1;

		ID3D11Resource* viewObject = resolveTexture_ ? resolveTexture_ : d3d11Texture2D_;
		hr = d3d11Device->CreateShaderResourceView(viewObject, &resourceViewDesc, &shaderResourceView_);
		if (FAILED(hr))
		{
			FLAGGG_LOG_ERROR("Failed to create shader resource view.");
			D3D11_SAFE_RELEASE(shaderResourceView_);
			return;
		}
	}

	if (textureDesc_.usage_ == TEXTURE_RENDERTARGET)
	{
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		memset(&renderTargetViewDesc, 0, sizeof renderTargetViewDesc);
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = textureDesc_.multiSample_ > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;

		ID3D11RenderTargetView* renderTargetView;
		hr = d3d11Device->CreateRenderTargetView(d3d11Texture2D_, &renderTargetViewDesc, &renderTargetView);
		if (FAILED(hr))
		{
			FLAGGG_LOG_ERROR("Failed to create rendertarget view.");
			D3D11_SAFE_RELEASE(renderTargetView);
			return;
		}

		SharedPtr<GfxRenderSurfaceD3D11> renderSurface(new GfxRenderSurfaceD3D11());
		renderSurface->SetRenderTargetView(renderTargetView);
		gfxRenderSurfaces_.Push(renderSurface);
	}
	else if (textureDesc_.usage_ == TEXTURE_DEPTHSTENCIL)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		memset(&depthStencilViewDesc, 0, sizeof depthStencilViewDesc);
		depthStencilViewDesc.Format = (DXGI_FORMAT)GetDSVFormat(textureDesc.Format);
		depthStencilViewDesc.ViewDimension = textureDesc_.multiSample_ > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;

		ID3D11DepthStencilView* depthStencilView;
		hr = d3d11Device->CreateDepthStencilView(d3d11Texture2D_, &depthStencilViewDesc, &depthStencilView);
		if (FAILED(hr))
		{
			FLAGGG_LOG_ERROR("Failed to create depth-stencil view.");
			D3D11_SAFE_RELEASE(depthStencilView);
			return;
		}

		SharedPtr<GfxRenderSurfaceD3D11> renderSurface(new GfxRenderSurfaceD3D11());
		renderSurface->SetDepthStencilView(depthStencilView);
		gfxRenderSurfaces_.Push(renderSurface);

		//if (d3d11Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0)
		//{
		//	depthStencilViewDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
		//	hr = d3d11Device->CreateDepthStencilView(d3d11Texture2D_, &depthStencilViewDesc,
		//		(ID3D11DepthStencilView**)&renderSurface_->readOnlyView_);
		//	if (FAILED(hr))
		//	{
		//		FLAGGG_LOG_ERROR("Failed to create read-only depth-stencil view.");
		//		D3D11_SAFE_RELEASE(renderSurface_->readOnlyView_);
		//	}
		//}
	}
}

void GfxTextureD3D11::CreateTexture3D()
{
	GfxDeviceD3D11* gfxDevice = GfxDeviceD3D11::Instance();
	ID3D11Device* d3d11Device = gfxDevice->GetD3D11Device();

	D3D11_TEXTURE3D_DESC textureDesc;
	memset(&textureDesc, 0, sizeof textureDesc);
	textureDesc.Width = (UINT)textureDesc_.width_;
	textureDesc.Height = (UINT)textureDesc_.height_;
	textureDesc.Depth = (UINT)textureDesc_.depth_;
	textureDesc.MipLevels = textureDesc_.usage_ != TEXTURE_DYNAMIC ? textureDesc_.levels_ : 1;
	textureDesc.Format = (DXGI_FORMAT)(textureDesc_.sRGB_ ? GetSRGBFormat(textureDesc_.format_) : textureDesc_.format_);
	textureDesc.Usage = textureDesc_.usage_ == TEXTURE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = textureDesc_.usage_ == TEXTURE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

	HRESULT hr = d3d11Device->CreateTexture3D(&textureDesc, nullptr, &d3d11Texture3D_);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("Failed to create texture3d.");
		D3D11_SAFE_RELEASE(d3d11Texture3D_);
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
	memset(&resourceViewDesc, 0, sizeof resourceViewDesc);
	resourceViewDesc.Format = (DXGI_FORMAT)GetSRVFormat(textureDesc.Format);
	resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	resourceViewDesc.Texture3D.MipLevels = textureDesc_.usage_ != TEXTURE_DYNAMIC ? (UINT)textureDesc_.levels_ : 1;

	hr = d3d11Device->CreateShaderResourceView(d3d11Texture3D_, &resourceViewDesc, &shaderResourceView_);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("Failed to create shader resource view.");
		return;
	}
}

void GfxTextureD3D11::CreateTextureCube()
{
	GfxDeviceD3D11* gfxDevice = GfxDeviceD3D11::Instance();
	ID3D11Device* d3d11Device = gfxDevice->GetD3D11Device();

	D3D11_TEXTURE2D_DESC textureDesc;
	memset(&textureDesc, 0, sizeof textureDesc);
	textureDesc.Format = (DXGI_FORMAT)(textureDesc_.sRGB_ ? GetSRGBFormat(textureDesc_.format_) : textureDesc_.format_);

	if (textureDesc_.multiSample_ > 1 && gfxDevice->CheckMultiSampleSupport(textureDesc.Format, textureDesc_.multiSample_))
	{
		textureDesc_.multiSample_ = 1;
		textureDesc_.autoResolve_ = false;
	}

	if (textureDesc_.usage_ == TEXTURE_RENDERTARGET && textureDesc_.levels_ != 1 && textureDesc_.multiSample_ == 1)
	{
		textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	textureDesc.Width = (UINT)textureDesc_.width_;
	textureDesc.Height = (UINT)textureDesc_.height_;
	textureDesc.MipLevels = (textureDesc_.multiSample_ == 1 && textureDesc_.usage_ != TEXTURE_DYNAMIC) ? textureDesc_.levels_ : 1;
	textureDesc.ArraySize = MAX_CUBEMAP_FACES;
	textureDesc.SampleDesc.Count = (UINT)textureDesc_.multiSample_;
	textureDesc.SampleDesc.Quality = gfxDevice->GetMultiSampleQuality(textureDesc.Format, textureDesc_.multiSample_);
	textureDesc.Usage = textureDesc_.usage_ == TEXTURE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (textureDesc_.usage_ == TEXTURE_RENDERTARGET)
	{
		textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	else if (textureDesc_.usage_ == TEXTURE_DEPTHSTENCIL)
	{
		textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	}
	textureDesc.CPUAccessFlags = textureDesc_.usage_ == TEXTURE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

	if (textureDesc_.multiSample_ < 2)
	{
		textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	HRESULT hr = d3d11Device->CreateTexture2D(&textureDesc, nullptr, &d3d11Texture2D_);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("Failed to create texture2d.");
		D3D11_SAFE_RELEASE(d3d11Texture2D_);
		return;
	}

	if (textureDesc_.multiSample_ > 1)
	{
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
		if (textureDesc_.levels_ != 1)
		{
			textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}

		HRESULT hr = d3d11Device->CreateTexture2D(&textureDesc, nullptr, (ID3D11Texture2D**)&resolveTexture_);
		if (FAILED(hr))
		{
			FLAGGG_LOG_ERROR("Failed to create resolve texture.");
			D3D11_SAFE_RELEASE(resolveTexture_);
			return;
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
	memset(&resourceViewDesc, 0, sizeof resourceViewDesc);
	resourceViewDesc.Format = (DXGI_FORMAT)GetSRVFormat(textureDesc.Format);
	resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	resourceViewDesc.Texture2D.MipLevels = textureDesc_.usage_ != TEXTURE_DYNAMIC ? (UINT)textureDesc_.levels_ : 1;

	ID3D11Resource* viewObject = resolveTexture_ ? resolveTexture_ : d3d11Texture2D_;
	hr = d3d11Device->CreateShaderResourceView(viewObject, &resourceViewDesc, &shaderResourceView_);
	if (FAILED(hr))
	{
		FLAGGG_LOG_ERROR("Failed to create shader resource view.");
		D3D11_SAFE_RELEASE(shaderResourceView_);
		return;
	}

	if (textureDesc_.usage_ == TEXTURE_RENDERTARGET)
	{
		for (UInt32 i = 0; i < MAX_CUBEMAP_FACES; ++i)
		{
			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
			memset(&renderTargetViewDesc, 0, sizeof renderTargetViewDesc);
			renderTargetViewDesc.Format = textureDesc.Format;
			if (textureDesc_.multiSample_ > 1)
			{
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
				renderTargetViewDesc.Texture2DMSArray.ArraySize = 1;
				renderTargetViewDesc.Texture2DMSArray.FirstArraySlice = i;
			}
			else
			{
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				renderTargetViewDesc.Texture2DArray.ArraySize = 1;
				renderTargetViewDesc.Texture2DArray.FirstArraySlice = i;
				renderTargetViewDesc.Texture2DArray.MipSlice = 0;
			}

			ID3D11RenderTargetView* renderTargetView;
			hr = d3d11Device->CreateRenderTargetView(d3d11Texture2D_, &renderTargetViewDesc, &renderTargetView);
			if (FAILED(hr))
			{
				FLAGGG_LOG_ERROR("Failed to create rendertarget view.");
				D3D11_SAFE_RELEASE(renderTargetView);
				return;
			}

			SharedPtr<GfxRenderSurfaceD3D11> renderSurface(new GfxRenderSurfaceD3D11());
			renderSurface->SetRenderTargetView(renderTargetView);
			gfxRenderSurfaces_.Push(renderSurface);
		}
	}
}

void GfxTextureD3D11::Apply(const void* initialDataPtr)
{
	ReleaseTexture();

	if (!textureDesc_.width_ || !textureDesc_.height_)
	{
		FLAGGG_LOG_ERROR("Texture's width or height is invalid.");
		return;
	}

	textureDesc_.levels_ = GfxTextureUtils::CheckMaxLevels(textureDesc_.width_, textureDesc_.height_, textureDesc_.requestedLevels_);

	if (textureDesc_.depth_ > 1)
	{
		CreateTexture3D();
	}
	else if (textureDesc_.isCube_)
	{
		CreateTextureCube();
	}
	else
	{
		CreateTexture2D();
	}
}

void GfxTextureD3D11::UpdateTexture(const void* dataPtr)
{

}

void GfxTextureD3D11::UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height)
{
	if (!d3d11Texture2D_)
	{
		FLAGGG_LOG_ERROR("Gfx texture invalid or format conflict.");
		return;
	}

	if (!dataPtr)
	{
		FLAGGG_LOG_ERROR("Texture2D ==> set nullptr data.");
		return;
	}

	if (level >= textureDesc_.levels_)
	{
		FLAGGG_LOG_ERROR("Texture2D ==> illegal mip level.");
		return;
	}

	const TextureMipInfo& mipInfo = GetMipInfo(level);
	if (x < 0 || x + width > mipInfo.width_ || y < 0 || y + height > mipInfo.height_ || width <= 0 || height <= 0)
	{
		FLAGGG_LOG_ERROR("Texture2D ==> illegal dimensions.");
		return;
	}

	const UInt8* src = static_cast<const UInt8*>(dataPtr);
	UInt32 rowSize = GfxTextureUtils::GetRowDataSize(textureDesc_.format_, width);
	UInt32 rowStart = GfxTextureUtils::GetRowDataSize(textureDesc_.format_, x);
	UInt32 subResource = D3D11CalcSubresource(level, index, textureDesc_.levels_);

	if (textureDesc_.usage_ == TEXTURE_DYNAMIC)
	{
		D3D11_MAPPED_SUBRESOURCE mappedData;
		mappedData.pData = nullptr;

		HRESULT hr = GfxDeviceD3D11::Instance()->GetD3D11DeviceContext()->Map(d3d11Texture2D_,
			subResource, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		if (FAILED(hr) || !mappedData.pData)
		{
			FLAGGG_LOG_ERROR("Failed to update texture resource.");
			return;
		}
		else
		{
			for (Int32 row = 0; row < height; ++row)
				memcpy((UInt8*)mappedData.pData + (row + y) * mappedData.RowPitch + rowStart, src + row * rowSize, rowSize);

			GfxDeviceD3D11::Instance()->GetD3D11DeviceContext()->Unmap(d3d11Texture2D_, subResource);
		}
	}
	else
	{
		D3D11_BOX destBox;
		destBox.left = (UINT)x;
		destBox.right = (UINT)(x + width);
		destBox.top = (UINT)y;
		destBox.bottom = (UINT)(y + height);
		destBox.front = 0;
		destBox.back = 1;

		GfxDeviceD3D11::Instance()->GetD3D11DeviceContext()->UpdateSubresource(d3d11Texture2D_, subResource, &destBox, dataPtr, rowSize, 0);
	}
}

void GfxTextureD3D11::UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 z, UInt32 width, UInt32 height, UInt32 depth)
{
	if (!d3d11Texture3D_)
	{
		FLAGGG_LOG_ERROR("Gfx texture invalid or format conflict.");
		return;
	}

	if (!dataPtr)
	{
		FLAGGG_LOG_ERROR("Texture3D ==> set nullptr data.");
		return;
	}

	if (level >= textureDesc_.levels_)
	{
		FLAGGG_LOG_ERROR("Texture3D ==> illegal mip level.");
		return;
	}

	const TextureMipInfo& mipInfo = GetMipInfo(level);
	if (x < 0 || x + width > mipInfo.width_ || y < 0 || y + height > mipInfo.height_ || z < 0 || z + depth > mipInfo.depth_ || width <= 0 || height <= 0 || depth <= 0)
	{
		FLAGGG_LOG_ERROR("Texture3D ==> illegal dimensions.");
		return;
	}

	const UInt8* src = static_cast<const UInt8*>(dataPtr);
	UInt32 rowSize = GfxTextureUtils::GetRowDataSize(textureDesc_.format_, width);
	UInt32 rowStart = GfxTextureUtils::GetRowDataSize(textureDesc_.format_, x);
	UInt32 subResource = D3D11CalcSubresource(level, index, textureDesc_.levels_);

	if (textureDesc_.usage_ == TEXTURE_DYNAMIC)
	{
		D3D11_MAPPED_SUBRESOURCE mappedData;
		mappedData.pData = nullptr;

		HRESULT hr = GfxDeviceD3D11::Instance()->GetD3D11DeviceContext()->Map(d3d11Texture3D_,
			subResource, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		if (FAILED(hr) || !mappedData.pData)
		{
			FLAGGG_LOG_ERROR("Failed to update texture resource.");
			return;
		}
		else
		{
			for (Int32 page = 0; page < depth; ++page)
			{
				for (Int32 row = 0; row < height; ++row)
					memcpy((UInt8*)mappedData.pData + (page + z) * mappedData.DepthPitch + (row + y) * mappedData.RowPitch + rowStart,
						src + row * rowSize, rowSize);
			}

			GfxDeviceD3D11::Instance()->GetD3D11DeviceContext()->Unmap(d3d11Texture3D_, subResource);
		}
	}
	else
	{
		D3D11_BOX destBox;
		destBox.left = (UINT)x;
		destBox.right = (UINT)(x + width);
		destBox.top = (UINT)y;
		destBox.bottom = (UINT)(y + height);
		destBox.front = (UINT)z;
		destBox.back = (UINT)(z + depth);

		GfxDeviceD3D11::Instance()->GetD3D11DeviceContext()->UpdateSubresource(d3d11Texture3D_, subResource, &destBox, dataPtr, rowSize, mipInfo.height_ * rowSize);
	}
}

GfxRenderSurface* GfxTextureD3D11::GetRenderSurface() const
{
	return gfxRenderSurfaces_.Size() ? gfxRenderSurfaces_[0] : nullptr;
}

GfxRenderSurface* GfxTextureD3D11::GetRenderSurface(UInt32 index) const
{
	return index < gfxRenderSurfaces_.Size() ? gfxRenderSurfaces_[index] : nullptr;
}

}
