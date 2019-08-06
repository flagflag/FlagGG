#include "Graphics/Texture2D.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/GraphicsDef.h"
#include "Math/Math.h"
#include "Log.h"

#include <d3dx11.h>

namespace FlagGG
{
	namespace Graphics
	{
		Texture2D::Texture2D(Core::Context* context) :
			Texture(context)
		{ }

		bool Texture2D::Create()
		{
			Release();

			if (!width_ || !height_)
			{
				return false;
			}

			levels_ = CheckMaxLevels(width_, height_, requestedLevels_);

			D3D11_TEXTURE2D_DESC textureDesc;
			memset(&textureDesc, 0, sizeof textureDesc);
			textureDesc.Format = (DXGI_FORMAT)(sRGB_ ? GetSRGBFormat(format_) : format_);

			if (multiSample_ > 1 && RenderEngine::Instance()->CheckMultiSampleSupport(textureDesc.Format, multiSample_))
			{
				multiSample_ = 1;
				autoResolve_ = false;
			}

			if (usage_ == TEXTURE_DEPTHSTENCIL)
			{
				levels_ = 1;
			}
			else if (usage_ == TEXTURE_RENDERTARGET && levels_ != 1 && multiSample_ == 1)
			{
				textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
			}

			textureDesc.Width = (UINT)width_;
			textureDesc.Height = (UINT)height_;
			textureDesc.MipLevels = (multiSample_ == 1 && usage_ != TEXTURE_DYNAMIC) ? levels_ : 1;
			textureDesc.ArraySize = 1;
			textureDesc.SampleDesc.Count = (UINT)multiSample_;
			textureDesc.SampleDesc.Quality = RenderEngine::Instance()->GetMultiSampleQuality(textureDesc.Format, multiSample_);

			textureDesc.Usage = usage_ == TEXTURE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			if (usage_ == TEXTURE_RENDERTARGET)
			{
				textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			}
			else if (usage_ == TEXTURE_DEPTHSTENCIL)
			{
				textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
			}
			textureDesc.CPUAccessFlags = usage_ == TEXTURE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

			if (usage_ == TEXTURE_DEPTHSTENCIL && multiSample_ > 1 && RenderEngine::Instance()->GetDevice()->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_1)
			{
				textureDesc.BindFlags &= ~D3D11_BIND_SHADER_RESOURCE;
			}

			ID3D11Texture2D* texture2D = nullptr;
			HRESULT hr = RenderEngine::Instance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &texture2D);
			if (FAILED(hr))
			{
				FLAGGG_LOG_ERROR("Failed to create texture2d.");
				SAFE_RELEASE(texture2D);
				return false;
			}

			ResetHandler(texture2D);

			if (multiSample_ > 1 && autoResolve_)
			{
				textureDesc.MipLevels = levels_;
				textureDesc.SampleDesc.Count = 1;
				textureDesc.SampleDesc.Quality = 0;
				if (levels_ != 1)
				{
					textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
				}

				HRESULT hr = RenderEngine::Instance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, (ID3D11Texture2D**)&resolveTexture_);
				if (FAILED(hr))
				{
					FLAGGG_LOG_ERROR("Failed to create resolve texture.");
					SAFE_RELEASE(resolveTexture_);
					return false;
				}
			}

			if (textureDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
				memset(&resourceViewDesc, 0, sizeof resourceViewDesc);
				resourceViewDesc.Format = (DXGI_FORMAT)GetSRVFormat(textureDesc.Format);
				resourceViewDesc.ViewDimension = (multiSample_ > 1 && !autoResolve_) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
				resourceViewDesc.Texture2D.MipLevels = usage_ != TEXTURE_DYNAMIC ? (UINT)levels_ : 1;

				ID3D11Resource* viewObject = resolveTexture_ ? resolveTexture_ : GetObject<ID3D11Resource>();
				hr = RenderEngine::Instance()->GetDevice()->CreateShaderResourceView(viewObject, &resourceViewDesc, &shaderResourceView_);
				if (FAILED(hr))
				{
					FLAGGG_LOG_ERROR("Failed to create shader resource view.");
					SAFE_RELEASE(shaderResourceView_);
					return false;
				}
			}

			if (usage_ == TEXTURE_RENDERTARGET)
			{
				D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
				memset(&renderTargetViewDesc, 0, sizeof renderTargetViewDesc);
				renderTargetViewDesc.Format = textureDesc.Format;
				renderTargetViewDesc.ViewDimension = multiSample_ > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;

				ID3D11RenderTargetView* renderTargetView;
				hr = RenderEngine::Instance()->GetDevice()->CreateRenderTargetView(GetObject<ID3D11Resource>(), &renderTargetViewDesc, &renderTargetView);
				if (FAILED(hr))
				{
					FLAGGG_LOG_ERROR("Failed to create rendertarget view.");
					SAFE_RELEASE(renderTargetView);
					return false;
				}
				renderSurface_->ResetHandler(renderTargetView);
			}
			else if (usage_ == TEXTURE_DEPTHSTENCIL)
			{
				D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
				memset(&depthStencilViewDesc, 0, sizeof depthStencilViewDesc);
				depthStencilViewDesc.Format = (DXGI_FORMAT)GetDSVFormat(textureDesc.Format);
				depthStencilViewDesc.ViewDimension = multiSample_ > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;

				ID3D11DepthStencilView* depthStencilView;
				hr = RenderEngine::Instance()->GetDevice()->CreateDepthStencilView(GetObject<ID3D11Resource>(), &depthStencilViewDesc, &depthStencilView);
				if (FAILED(hr))
				{
					FLAGGG_LOG_ERROR("Failed to create depth-stencil view.");
					SAFE_RELEASE(depthStencilView);
					return false;
				}
				renderSurface_->ResetHandler(depthStencilView);

				if (RenderEngine::Instance()->GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0)
				{
					depthStencilViewDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
					hr = RenderEngine::Instance()->GetDevice()->CreateDepthStencilView(GetObject<ID3D11Resource>(), &depthStencilViewDesc,
						(ID3D11DepthStencilView**)&renderSurface_->readOnlyView_);
					if (FAILED(hr))
					{
						FLAGGG_LOG_ERROR("Failed to create read-only depth-stencil view.");
						SAFE_RELEASE(renderSurface_->readOnlyView_);
					}
				}
			}

			return true;
		}

		void Texture2D::Release()
		{
			SAFE_RELEASE(resolveTexture_);
			SAFE_RELEASE(shaderResourceView_);
		}

		bool Texture2D::SetSize(int32_t width, int32_t height, uint32_t format,
			TextureUsage usage/* = TEXTURE_STATIC*/, int32_t multiSample/* = 1*/, bool autoResolve/* = true*/)
		{
			if (width <= 0 || height <= 0)
			{
				FLAGGG_LOG_ERROR("Zero or negative texture dimensions.");
				return false;
			}

			multiSample = Math::Clamp(multiSample, 1, 16);
			if (multiSample == 1)
			{
				autoResolve = false;
			}
			else if (multiSample > 1 && usage < TEXTURE_RENDERTARGET)
			{
				FLAGGG_LOG_ERROR("Multisampling is only supported for rendertarget or depth-stencil textures");
				return false;
			}

			if (multiSample > 1 && !autoResolve)
			{
				requestedLevels_ = 1;
			}

			renderSurface_.Reset();
			
			usage_ = usage;

			if (usage >= TEXTURE_RENDERTARGET)
			{
				renderSurface_ = new RenderSurface();
			}

			width_ = width;
			height_ = height;
			format_ = format;
			depth_ = 1;
			multiSample_ = multiSample;
			autoResolve_ = autoResolve;

			return Create();
		}

		bool Texture2D::SetData(uint32_t level, int32_t x, int32_t y, int32_t width, int32_t height, const void* data)
		{
			if (!data)
			{
				FLAGGG_LOG_ERROR("Texture2D ==> set nullptr data.");
				return false;
			}

			if (level >= levels_)
			{
				FLAGGG_LOG_ERROR("Texture2D ==> illegal mip level.");
				return false;
			}

			int32_t levelWidth = GetLevelWidth(level);
			int32_t levelHeight = GetLevelHeight(level);
			if (x < 0 || x + width > levelWidth || y < 0 || y + height > levelHeight || width <= 0 || height <= 0)
			{
				FLAGGG_LOG_ERROR("Texture2D ==> illegal dimensions.");
				return false;
			}

			if (IsCompressed())
			{
				x &= ~3;
				y &= ~3;
				width += 3;
				width &= 0xfffffffc;
				height += 3;
				height &= 0xfffffffc;
			}

			const uint8_t* src = static_cast<const uint8_t*>(data);
			uint32_t rowSize = GetRowDataSize(width);
			uint32_t rowStart = GetRowDataSize(x);
			uint32_t subResource = D3D11CalcSubresource(level, 0, levels_);

			if (usage_ == TEXTURE_DYNAMIC)
			{
				if (IsCompressed())
				{
					height = (height + 3) >> 2;
					y >>= 2;
				}

				D3D11_MAPPED_SUBRESOURCE mappedData;
				mappedData.pData = nullptr;

				HRESULT hr = RenderEngine::Instance()->GetDeviceContext()->Map(GetObject<ID3D11Resource>(),
					subResource, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

				if (FAILED(hr) || !mappedData.pData)
				{
					FLAGGG_LOG_ERROR("Failed to update texture resource.", hr);
					return false;
				}
				else
				{
					for (int32_t row = 0; row < height; ++row)
					{
						memcpy((uint8_t*)mappedData.pData + (row + y) * mappedData.RowPitch + rowStart, src + row * rowSize, rowSize);
						RenderEngine::Instance()->GetDeviceContext()->Unmap(GetObject<ID3D11Resource>(), subResource);
					}
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

				RenderEngine::Instance()->GetDeviceContext()->UpdateSubresource(GetObject<ID3D11Resource>(), subResource, &destBox, data, rowSize, 0);
			}

			return true;
		}

		bool Texture2D::SetData(FlagGG::Resource::Image* image, bool useAlpha/* = false*/)
		{
			if (!image)
			{
				FLAGGG_LOG_ERROR("Null image, can not load texture.");
				return false;
			}

			Container::SharedPtr<FlagGG::Resource::Image> mipImage;
			uint32_t memoryUse = sizeof(Texture2D);
			MaterialQuality quality = RenderEngine::Instance()->GetTextureQuality();

			if (!image->IsCompressed())
			{
				// Convert unsuitable formats to RGBA
				unsigned components = image->GetComponents();
				if ((components == 1 && !useAlpha) || components == 2 || components == 3)
				{
					mipImage = image->ConvertToRGBA(); image = mipImage;
					if (!image)
						return false;
					components = image->GetComponents();
				}

				unsigned char* levelData = image->GetData();
				int levelWidth = image->GetWidth();
				int levelHeight = image->GetHeight();
				unsigned format = 0;

				// Discard unnecessary mip levels
				for (unsigned i = 0; i < mipsToSkip_[quality]; ++i)
				{
					mipImage = image->GetNextLevel(); image = mipImage;
					levelData = image->GetData();
					levelWidth = image->GetWidth();
					levelHeight = image->GetHeight();
				}

				switch (components)
				{
				case 1:
					format = RenderEngine::GetAlphaFormat();
					break;

				case 4:
					format = RenderEngine::GetRGBAFormat();
					break;

				default: break;
				}

				// If image was previously compressed, reset number of requested levels to avoid error if level count is too high for new size
				if (IsCompressed() && requestedLevels_ > 1)
					requestedLevels_ = 0;
				if (!SetSize(levelWidth, levelHeight, format))
				{
					return false;
				}

				for (unsigned i = 0; i < levels_; ++i)
				{
					SetData(i, 0, 0, levelWidth, levelHeight, levelData);
					memoryUse += levelWidth * levelHeight * components;

					if (i < levels_ - 1)
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
				int width = image->GetWidth();
				int height = image->GetHeight();
				unsigned levels = image->GetNumCompressedLevels();
				unsigned format = RenderEngine::GetFormat(image->GetCompressedFormat());
				bool needDecompress = false;

				if (!format)
				{
					format = RenderEngine::GetRGBAFormat();
					needDecompress = true;
				}

				unsigned mipsToSkip = mipsToSkip_[quality];
				if (mipsToSkip >= levels)
					mipsToSkip = levels - 1;
				while (mipsToSkip && (width / (1 << mipsToSkip) < 4 || height / (1 << mipsToSkip) < 4))
					--mipsToSkip;
				width /= (1 << mipsToSkip);
				height /= (1 << mipsToSkip);

				SetNumLevels(Math::Max((levels - mipsToSkip), 1U));
				if (!SetSize(width, height, format))
				{
					return false;
				}

				for (unsigned i = 0; i < levels_ && i < levels - mipsToSkip; ++i)
				{
					FlagGG::Resource::CompressedLevel level = image->GetCompressedLevel(i + mipsToSkip);
					if (!needDecompress)
					{
						SetData(i, 0, 0, level.width_, level.height_, level.data_);
						memoryUse += level.rows_ * level.rowSize_;
					}
					else
					{
						unsigned char* rgbaData = new unsigned char[level.width_ * level.height_ * 4];
						level.Decompress(rgbaData);
						SetData(i, 0, 0, level.width_, level.height_, rgbaData);
						memoryUse += level.width_ * level.height_ * 4;
						delete[] rgbaData;
					}
				}
			}

			SetMemoryUse(memoryUse);

			return true;
		}

		bool Texture2D::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
			Initialize();

			FlagGG::Resource::Image image(context_);
			if (!image.LoadFile(stream))
			{
				FLAGGG_LOG_ERROR("Failed to load image.");
				return false;
			}

			return SetData(&image);
		}

		bool Texture2D::EndLoad()
		{
			return true;
		}

		bool Texture2D::GetData(uint32_t level, void* dest)
		{
			if (!Texture::IsValid())
			{
				FLAGGG_LOG_ERROR("Invalid Texture2D cannot get data.");
				return false;
			}

			if (!dest)
			{
				FLAGGG_LOG_ERROR("Null destination for getting data.");
				return false;
			}

			if (level >= levels_)
			{
				FLAGGG_LOG_ERROR("Illegal mip level for getting data.");
				return false;
			}

			if (multiSample_ > 1 && !autoResolve_)
			{
				FLAGGG_LOG_ERROR("Can not get data from multisampled texture without autoresolve.");
				return false;
			}

			int32_t levelWidth = GetLevelWidth(level);
			int32_t levelHeight = GetLevelHeight(level);

			D3D11_TEXTURE2D_DESC textureDesc;
			memset(&textureDesc, 0, sizeof textureDesc);
			textureDesc.Width = (UINT)levelWidth;
			textureDesc.Height = (UINT)levelHeight;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = (DXGI_FORMAT)format_;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_STAGING;
			textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

			ID3D11Texture2D* stagingTexture = nullptr;
			HRESULT hr = RenderEngine::Instance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &stagingTexture);
			if (FAILED(hr))
			{
				FLAGGG_LOG_ERROR("Failed to create staging texture for GetData", hr);
				SAFE_RELEASE(stagingTexture);
				return false;
			}

			ID3D11Resource* srcResource = (ID3D11Resource*)(resolveTexture_ ? resolveTexture_ : GetObject<ID3D11Resource>());
			unsigned srcSubResource = D3D11CalcSubresource(level, 0, levels_);

			D3D11_BOX srcBox;
			srcBox.left = 0;
			srcBox.right = (UINT)levelWidth;
			srcBox.top = 0;
			srcBox.bottom = (UINT)levelHeight;
			srcBox.front = 0;
			srcBox.back = 1;
			RenderEngine::Instance()->GetDeviceContext()->CopySubresourceRegion(stagingTexture, 0, 0, 0, 0, srcResource,
				srcSubResource, &srcBox);

			D3D11_MAPPED_SUBRESOURCE mappedData;
			mappedData.pData = nullptr;
			unsigned rowSize = GetRowDataSize(levelWidth);
			unsigned numRows = (unsigned)(IsCompressed() ? (levelHeight + 3) >> 2 : levelHeight);

			hr = RenderEngine::Instance()->GetDeviceContext()->Map((ID3D11Resource*)stagingTexture, 0, D3D11_MAP_READ, 0, &mappedData);
			if (FAILED(hr) || !mappedData.pData)
			{
				FLAGGG_LOG_ERROR("Failed to map staging texture for GetData", hr);
				SAFE_RELEASE(stagingTexture);
				return false;
			}
			
			for (unsigned row = 0; row < numRows; ++row)
			{
				memcpy((unsigned char*)dest + row * rowSize, (unsigned char*)mappedData.pData + row * mappedData.RowPitch, rowSize);
			}

			RenderEngine::Instance()->GetDeviceContext()->Unmap((ID3D11Resource*)stagingTexture, 0);
			SAFE_RELEASE(stagingTexture);

			return true;		
		}

		Container::SharedPtr<FlagGG::Resource::Image> Texture2D::GetImage()
		{
			Container::SharedPtr<FlagGG::Resource::Image> image(new FlagGG::Resource::Image(context_));
			if (format_ != RenderEngine::GetRGBAFormat() && format_ != RenderEngine::GetRGBFormat())
			{
				FLAGGG_LOG_ERROR("Unsupported texture format, can not convert to Image");
				return nullptr;
			}

			image->SetSize(width_, height_, GetComponents());
			GetData(0, image->GetData());

			return image;
		}
	}
}