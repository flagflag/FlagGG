#include "Graphics/Texture2D.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/GraphicsDef.h"
#include "Math/Math.h"
#include "Log.h"
#include "bgfx/bgfx.h"
#include "bimg/bimg.h"
#include "bimg/decode.h"
#include "bx/allocator.h"

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

			if (usage_ == TEXTURE_DEPTHSTENCIL)
			{
				// �ж�һ�������ͼ��ʽ�ĺϷ���
				if (format_ <= bgfx::TextureFormat::UnknownDepth)
				{
					FLAGGG_LOG_ERROR("Texture2D's usage is depth stencil, but the format is invalid.");
					return false;
				}

				levels_ = 1;
			}

			bool hasMips = levels_ > 1;
			UInt16 numLayers = layers_;
			bgfx::TextureFormat::Enum format = (bgfx::TextureFormat::Enum)format_;
			UInt64 flags = GetFlags();

			if (!bgfx::isTextureValid(0, false, numLayers, format, flags))
			{
				FLAGGG_LOG_ERROR("texture is valid.");
				return false;
			}

			const bgfx::Memory* mem = internalData_ ? (const bgfx::Memory*)internalData_ : nullptr;

			bgfx::TextureHandle texHandle = bgfx::createTexture2D((UInt16)width_, (UInt16)height_, hasMips, numLayers, format, flags, mem);

			internalData_ = nullptr;

			ResetHandler(texHandle);

			return true;
		}

		bool Texture2D::SetSize(Int32 width, Int32 height, UInt32 format,
			TextureUsage usage/* = TEXTURE_STATIC*/, Int32 multiSample/* = 1*/, bool autoResolve/* = true*/)
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
				renderSurface_ = new RenderSurface(this);
			}

			width_ = width;
			height_ = height;
			format_ = format;
			depth_ = 1;
			multiSample_ = multiSample;
			autoResolve_ = autoResolve;

			return Create();
		}

		bool Texture2D::SetData(UInt32 level, Int32 x, Int32 y, Int32 width, Int32 height, const void* mipData, UInt32 mipDataSize)
		{
			if (!mipData)
			{
				FLAGGG_LOG_ERROR("Texture2D ==> set nullptr data.");
				return false;
			}

			if (level >= levels_)
			{
				FLAGGG_LOG_ERROR("Texture2D ==> illegal mip level.");
				return false;
			}

			const bgfx::Memory* mem = bgfx::copy(mipData, mipDataSize);
			bgfx::updateTexture2D(GetSrcHandler<bgfx::TextureHandle>(), 0, level, x, y, width, height, mem);

			return true;
		}

		bool Texture2D::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
		{
			Initialize();

			UInt32 dataSize = stream->GetSize();
			Container::SharedArrayPtr<char> data(new char[dataSize]);
			if (stream->ReadStream(data.Get(), dataSize) != dataSize)
			{
				FLAGGG_LOG_ERROR("Failed to load image.");
				return false;
			}

			bimg::ImageContainer* imageContainer = bimg::imageParse(RenderEngine::Instance()->GetDefaultAllocator(), data.Get(), dataSize);

			if (!imageContainer)
			{
				return false;
			}

			if (imageContainer->m_cubeMap)
			{
				BX_ASSERT(false, "Can't convert texturecube to texturen2d.");
				return false;
			}

			if (1 < imageContainer->m_depth)
			{
				BX_ASSERT(false, "Can't convert texture3d to texturen2d.");
				return false;
			}

			if (1 != imageContainer->m_numLayers)
			{
				BX_ASSERT(false, "Texture2d's layers must be 1.");
				return false;
			}

			SetNumLevels(imageContainer->m_numMips);
			SetNumLayers(imageContainer->m_numLayers);
			SetAddressMode(COORD_U, ADDRESS_CLAMP);
			SetAddressMode(COORD_V, ADDRESS_CLAMP);
			const bgfx::Memory* mem = bgfx::makeRef(imageContainer->m_data, imageContainer->m_size, ImageReleaseCb, imageContainer);
			SetInternalData(mem);

			if (!SetSize(imageContainer->m_width, imageContainer->m_height, imageContainer->m_format))
			{
				return false;
			}

			bgfx::setName(GetSrcHandler<bgfx::TextureHandle>(), GetName().CString());

			return true;
		}

		bool Texture2D::EndLoad()
		{
			return true;
		}

		bool Texture2D::GetData(UInt32 level, void* dest)
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

			int numFrame = bgfx::readTexture(GetSrcHandler<bgfx::TextureHandle>(), dest, level);
			while (bgfx::frame() <= numFrame) {}

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

		RenderSurface* Texture2D::GetRenderSurface() const
		{
			return renderSurface_;
		}

		RenderSurface* Texture2D::GetRenderSurface(UInt32 index) const
		{
			return renderSurface_;
		}
	}
}