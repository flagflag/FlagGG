#include "Graphics/TextureCube.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/GraphicsDef.h"
#include "Core/Context.h"
#include "Config/LJSONFile.h"
#include "Resource/ResourceCache.h"
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
		TextureCube::TextureCube(Core::Context* context) :
			Texture(context)
		{ }

		bool TextureCube::Create()
		{
			Release();

			if (!width_ || !height_)
			{
				return false;
			}

			levels_ = CheckMaxLevels(width_, height_, requestedLevels_);

			if (usage_ == TEXTURE_DEPTHSTENCIL)
			{
				// 判断一下深度贴图格式的合法性
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

			// width_ == height_ == size
			bgfx::TextureHandle texHandle = bgfx::createTextureCube(width_, hasMips, numLayers, format, flags, mem);

			internalData_ = nullptr;
			
			ResetHandler(texHandle);

			return true;
		}

		bool TextureCube::SetSize(UInt32 size, UInt32 format,
			TextureUsage usage/* = TEXTURE_STATIC*/, Int32 multiSample/* = 1*/)
		{
			if (size <= 0)
			{
				FLAGGG_LOG_ERROR("Zero or negative texture dimensions.");
				return false;
			}

			multiSample = Math::Clamp(multiSample, 1, 16);
			if (multiSample > 1 && usage < TEXTURE_RENDERTARGET)
			{
				FLAGGG_LOG_ERROR("Multisampling is only supported for rendertarget or depth-stencil textures");
				return false;
			}

			for (UInt32 i = 0; i < MAX_CUBEMAP_FACES; ++i)
			{
				renderSurfaces_[i].Reset();
			}

			usage_ = usage;

			if (usage >= TEXTURE_RENDERTARGET)
			{
				for (UInt32 i = 0; i < MAX_CUBEMAP_FACES; ++i)
				{
					renderSurfaces_[i] = new RenderSurface(this);
					faceMemoryUse_[i] = 0u;
				}
			}

			width_ = size;
			height_ = size;
			format_ = format;
			depth_ = 1;
			multiSample_ = multiSample;
			autoResolve_ = multiSample > 1;

			return Create();
		}

		bool TextureCube::SetData(CubeMapFace face, UInt32 level, Int32 x, Int32 y, Int32 width, Int32 height, const void* mipData, UInt32 mipDataSize)
		{
			if (!mipData)
			{
				FLAGGG_LOG_ERROR("TextureCube ==> set nullptr data.");
				return false;
			}

			if (level >= levels_)
			{
				FLAGGG_LOG_ERROR("TextureCube ==> illegal mip level.");
				return false;
			}

			const bgfx::Memory* mem = bgfx::copy(mipData, mipDataSize);
			bgfx::updateTextureCube(GetSrcHandler<bgfx::TextureHandle>(), 0, face, level, x, y, width, height, mem);

			return true;
		}

		bool TextureCube::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
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

			if (!imageContainer->m_cubeMap)
			{
				BX_ASSERT(false, "Can't convert textureother to texturecube");
				return false;
			}

			if (1 < imageContainer->m_depth)
			{
				BX_ASSERT(false, "Can't convert texturen3d to texturecube");
				return false;
			}

			SetNumLevels(imageContainer->m_numMips);
			SetNumLayers(imageContainer->m_numLayers);
			SetAddressMode(COORD_U, ADDRESS_CLAMP);
			SetAddressMode(COORD_V, ADDRESS_CLAMP);
			SetAddressMode(COORD_W, ADDRESS_CLAMP);
			const bgfx::Memory* mem = bgfx::makeRef(imageContainer->m_data, imageContainer->m_size, ImageReleaseCb, imageContainer);
			SetInternalData(mem);

			if (!SetSize(imageContainer->m_width, imageContainer->m_format))
			{
				return false;
			}

			bgfx::setName(GetSrcHandler<bgfx::TextureHandle>(), GetName().CString());
			
			return true;
		}

		bool TextureCube::EndLoad()
		{
			return true;
		}

		bool TextureCube::GetData(CubeMapFace face, UInt32 level, void* dest)
		{
			if (!Texture::IsValid())
			{
				FLAGGG_LOG_ERROR("Invalid TextureCube cannot get data.");
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

			// width_ == height_ == size
			bgfx::readTexture(GetSrcHandler<bgfx::TextureHandle>(), (char*)dest + levels_ * width_, level);

			return true;
		}

		Container::SharedPtr<FlagGG::Resource::Image> TextureCube::GetImage(CubeMapFace face)
		{
			Container::SharedPtr<FlagGG::Resource::Image> image(new FlagGG::Resource::Image(context_));
			if (format_ != RenderEngine::GetRGBAFormat() && format_ != RenderEngine::GetRGBFormat())
			{
				FLAGGG_LOG_ERROR("Unsupported texture format, can not convert to Image");
				return nullptr;
			}

			// image->SetSize(width_, height_, GetComponents());
			if (format_ == RenderEngine::GetRGBAFormat())
				image->SetSize(width_, height_, 4);
			else if (format_ == RenderEngine::GetRGBFormat())
				image->SetSize(width_, height_, 3);
			GetData(face, 0, image->GetData());

			return image;
		}

		RenderSurface* TextureCube::GetRenderSurface() const
		{
			return renderSurfaces_[0];
		}

		RenderSurface* TextureCube::GetRenderSurface(UInt32 index) const
		{
			return renderSurfaces_[index < MAX_CUBEMAP_FACES ? index : MAX_CUBEMAP_FACES];
		}
	}
}