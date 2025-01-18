#include "GfxTextureOGL.h"
#include "GfxRenderSurfaceOGL.h"
#include "Log.h"

namespace FlagGG
{

struct OpenGLTextureFormatInfo
{
	GLenum internalFmt_;
	GLenum internalFmtSrgb_;
	GLenum fmt_;
	GLenum fmtSrgb_;
	GLenum type_;
	bool supported_;
};

static const OpenGLTextureFormatInfo openglTextureFormat[] =
{
	{ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,            GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,       GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,            GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,            GL_ZERO,                         false }, // BC1
	{ GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,            GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,       GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,            GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,            GL_ZERO,                         false }, // BC2
	{ GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,            GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,       GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,            GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,            GL_ZERO,                         false }, // BC3
	{ GL_COMPRESSED_LUMINANCE_LATC1_EXT,           GL_ZERO,                                      GL_COMPRESSED_LUMINANCE_LATC1_EXT,           GL_COMPRESSED_LUMINANCE_LATC1_EXT,           GL_ZERO,                         false }, // BC4
	{ GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT,     GL_ZERO,                                      GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT,     GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT,     GL_ZERO,                         false }, // BC5
	{ GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,     GL_ZERO,                                      GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,     GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,     GL_ZERO,                         false }, // BC6H
	{ GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,           GL_ZERO,                                      GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,           GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,           GL_ZERO,                         false }, // BC7
	{ GL_ETC1_RGB8_OES,                            GL_ZERO,                                      GL_ETC1_RGB8_OES,                            GL_ETC1_RGB8_OES,                            GL_ZERO,                         false }, // ETC1
	{ GL_COMPRESSED_RGB8_ETC2,                     GL_ZERO,                                      GL_COMPRESSED_RGB8_ETC2,                     GL_COMPRESSED_RGB8_ETC2,                     GL_ZERO,                         false }, // ETC2
	{ GL_COMPRESSED_RGBA8_ETC2_EAC,                GL_COMPRESSED_SRGB8_ETC2,                     GL_COMPRESSED_RGBA8_ETC2_EAC,                GL_COMPRESSED_RGBA8_ETC2_EAC,                GL_ZERO,                         false }, // ETC2A
	{ GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_ZERO,                         false }, // ETC2A1
	{ GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,          GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT,          GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,          GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,          GL_ZERO,                         false }, // PTC12
	{ GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,          GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT,          GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,          GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,          GL_ZERO,                         false }, // PTC14
	{ GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,         GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT,    GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,         GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,         GL_ZERO,                         false }, // PTC12A
	{ GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,         GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT,    GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,         GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,         GL_ZERO,                         false }, // PTC14A
	{ GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG,         GL_ZERO,                                      GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG,         GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG,         GL_ZERO,                         false }, // PTC22
	{ GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG,         GL_ZERO,                                      GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG,         GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG,         GL_ZERO,                         false }, // PTC24
	{ GL_ATC_RGB_AMD,                              GL_ZERO,                                      GL_ATC_RGB_AMD,                              GL_ATC_RGB_AMD,                              GL_ZERO,                         false }, // ATC
	{ GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,              GL_ZERO,                                      GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,              GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,              GL_ZERO,                         false }, // ATCE
	{ GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,          GL_ZERO,                                      GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,          GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,          GL_ZERO,                         false }, // ATCI
	{ GL_COMPRESSED_RGBA_ASTC_4x4_KHR,             GL_COMPRESSED_SRGB8_ASTC_4x4_KHR,             GL_COMPRESSED_RGBA_ASTC_4x4_KHR,             GL_COMPRESSED_RGBA_ASTC_4x4_KHR,             GL_ZERO,                         false }, // ASTC4x4
	{ GL_COMPRESSED_RGBA_ASTC_5x5_KHR,             GL_COMPRESSED_SRGB8_ASTC_5x5_KHR,             GL_COMPRESSED_RGBA_ASTC_5x5_KHR,             GL_COMPRESSED_RGBA_ASTC_5x5_KHR,             GL_ZERO,                         false }, // ASTC5x5
	{ GL_COMPRESSED_RGBA_ASTC_6x6_KHR,             GL_COMPRESSED_SRGB8_ASTC_6x6_KHR,             GL_COMPRESSED_RGBA_ASTC_6x6_KHR,             GL_COMPRESSED_RGBA_ASTC_6x6_KHR,             GL_ZERO,                         false }, // ASTC6x6
	{ GL_COMPRESSED_RGBA_ASTC_8x5_KHR,             GL_COMPRESSED_SRGB8_ASTC_8x5_KHR,             GL_COMPRESSED_RGBA_ASTC_8x5_KHR,             GL_COMPRESSED_RGBA_ASTC_8x5_KHR,             GL_ZERO,                         false }, // ASTC8x5
	{ GL_COMPRESSED_RGBA_ASTC_8x6_KHR,             GL_COMPRESSED_SRGB8_ASTC_8x6_KHR,             GL_COMPRESSED_RGBA_ASTC_8x6_KHR,             GL_COMPRESSED_RGBA_ASTC_8x6_KHR,             GL_ZERO,                         false }, // ASTC8x6
	{ GL_COMPRESSED_RGBA_ASTC_10x5_KHR,            GL_COMPRESSED_SRGB8_ASTC_10x5_KHR,            GL_COMPRESSED_RGBA_ASTC_10x5_KHR,            GL_COMPRESSED_RGBA_ASTC_10x5_KHR,            GL_ZERO,                         false }, // ASTC10x5
	{ GL_ZERO,                                     GL_ZERO,                                      GL_ZERO,                                     GL_ZERO,                                     GL_ZERO,                         false }, // Unknown
	{ GL_ZERO,                                     GL_ZERO,                                      GL_ZERO,                                     GL_ZERO,                                     GL_ZERO,                         false }, // R1
	{ GL_ALPHA,                                    GL_ZERO,                                      GL_ALPHA,                                    GL_ALPHA,                                    GL_UNSIGNED_BYTE,                false }, // A8
	{ GL_R8,                                       GL_ZERO,                                      GL_RED,                                      GL_RED,                                      GL_UNSIGNED_BYTE,                false }, // R8
	{ GL_R8I,                                      GL_ZERO,                                      RED_INTEGER,                                 GL_RED_INTEGER,                              GL_BYTE,                         false }, // R8I
	{ GL_R8UI,                                     GL_ZERO,                                      RED_INTEGER,                                 GL_RED_INTEGER,                              GL_UNSIGNED_BYTE,                false }, // R8U
	{ GL_R8_SNORM,                                 GL_ZERO,                                      GL_RED,                                      GL_RED,                                      GL_BYTE,                         false }, // R8S
	{ GL_R16,                                      GL_ZERO,                                      GL_RED,                                      GL_RED,                                      GL_UNSIGNED_SHORT,               false }, // R16
	{ GL_R16I,                                     GL_ZERO,                                      RED_INTEGER,                                 GL_RED_INTEGER,                              GL_SHORT,                        false }, // R16I
	{ GL_R16UI,                                    GL_ZERO,                                      RED_INTEGER,                                 GL_RED_INTEGER,                              GL_UNSIGNED_SHORT,               false }, // R16U
	{ GL_R16F,                                     GL_ZERO,                                      GL_RED,                                      GL_RED,                                      GL_HALF_FLOAT,                   false }, // R16F
	{ GL_R16_SNORM,                                GL_ZERO,                                      GL_RED,                                      GL_RED,                                      GL_SHORT,                        false }, // R16S
	{ GL_R32I,                                     GL_ZERO,                                      RED_INTEGER,                                 GL_RED_INTEGER,                              GL_INT,                          false }, // R32I
	{ GL_R32UI,                                    GL_ZERO,                                      RED_INTEGER,                                 GL_RED_INTEGER,                              GL_UNSIGNED_INT,                 false }, // R32U
	{ GL_R32F,                                     GL_ZERO,                                      GL_RED,                                      GL_RED,                                      GL_FLOAT,                        false }, // R32F
	{ GL_RG8,                                      GL_ZERO,                                      GL_RG,                                       GL_RG,                                       GL_UNSIGNED_BYTE,                false }, // RG8
	{ GL_RG8I,                                     GL_ZERO,                                      RG_INTEGER,                                  GL_RG_INTEGER,                               GL_BYTE,                         false }, // RG8I
	{ GL_RG8UI,                                    GL_ZERO,                                      RG_INTEGER,                                  GL_RG_INTEGER,                               GL_UNSIGNED_BYTE,                false }, // RG8U
	{ GL_RG8_SNORM,                                GL_ZERO,                                      GL_RG,                                       GL_RG,                                       GL_BYTE,                         false }, // RG8S
	{ GL_RG16,                                     GL_ZERO,                                      GL_RG,                                       GL_RG,                                       GL_UNSIGNED_SHORT,               false }, // RG16
	{ GL_RG16I,                                    GL_ZERO,                                      RG_INTEGER,                                  GL_RG_INTEGER,                               GL_SHORT,                        false }, // RG16I
	{ GL_RG16UI,                                   GL_ZERO,                                      RG_INTEGER,                                  GL_RG_INTEGER,                               GL_UNSIGNED_SHORT,               false }, // RG16U
	{ GL_RG16F,                                    GL_ZERO,                                      GL_RG,                                       GL_RG,                                       GL_FLOAT,                        false }, // RG16F
	{ GL_RG16_SNORM,                               GL_ZERO,                                      GL_RG,                                       GL_RG,                                       GL_SHORT,                        false }, // RG16S
	{ GL_RG32I,                                    GL_ZERO,                                      RG_INTEGER,                                  GL_RG_INTEGER,                               GL_INT,                          false }, // RG32I
	{ GL_RG32UI,                                   GL_ZERO,                                      RG_INTEGER,                                  GL_RG_INTEGER,                               GL_UNSIGNED_INT,                 false }, // RG32U
	{ GL_RG32F,                                    GL_ZERO,                                      GL_RG,                                       GL_RG,                                       GL_FLOAT,                        false }, // RG32F
	{ GL_RGB8,                                     GL_SRGB8,                                     GL_RGB,                                      GL_RGB,                                      GL_UNSIGNED_BYTE,                false }, // RGB8
	{ GL_RGB8I,                                    GL_ZERO,                                      RGB_INTEGER,                                 GL_RGB_INTEGER,                              GL_BYTE,                         false }, // RGB8I
	{ GL_RGB8UI,                                   GL_ZERO,                                      RGB_INTEGER,                                 GL_RGB_INTEGER,                              GL_UNSIGNED_BYTE,                false }, // RGB8U
	{ GL_RGB8_SNORM,                               GL_ZERO,                                      GL_RGB,                                      GL_RGB,                                      GL_BYTE,                         false }, // RGB8S
	{ GL_RGB9_E5,                                  GL_ZERO,                                      GL_RGB,                                      GL_RGB,                                      GL_UNSIGNED_INT_5_9_9_9_REV,     false }, // RGB9E5F
	{ GL_RGBA8,                                    GL_SRGB8_ALPHA8,                              GL_BGRA,                                     GL_BGRA,                                     GL_UNSIGNED_BYTE,                false }, // BGRA8
	{ GL_RGBA8,                                    GL_SRGB8_ALPHA8,                              GL_RGBA,                                     GL_RGBA,                                     GL_UNSIGNED_BYTE,                false }, // RGBA8
	{ GL_RGBA8I,                                   GL_ZERO,                                      RGBA_INTEGER,                                GL_RGBA_INTEGER,                             GL_BYTE,                         false }, // RGBA8I
	{ GL_RGBA8UI,                                  GL_ZERO,                                      RGBA_INTEGER,                                GL_RGBA_INTEGER,                             GL_UNSIGNED_BYTE,                false }, // RGBA8U
	{ GL_RGBA8_SNORM,                              GL_ZERO,                                      GL_RGBA,                                     GL_RGBA,                                     GL_BYTE,                         false }, // RGBA8S
	{ GL_RGBA16,                                   GL_ZERO,                                      GL_RGBA,                                     GL_RGBA,                                     GL_UNSIGNED_SHORT,               false }, // RGBA16
	{ GL_RGBA16I,                                  GL_ZERO,                                      RGBA_INTEGER,                                GL_RGBA_INTEGER,                             GL_SHORT,                        false }, // RGBA16I
	{ GL_RGBA16UI,                                 GL_ZERO,                                      RGBA_INTEGER,                                GL_RGBA_INTEGER,                             GL_UNSIGNED_SHORT,               false }, // RGBA16U
	{ GL_RGBA16F,                                  GL_ZERO,                                      GL_RGBA,                                     GL_RGBA,                                     GL_HALF_FLOAT,                   false }, // RGBA16F
	{ GL_RGBA16_SNORM,                             GL_ZERO,                                      GL_RGBA,                                     GL_RGBA,                                     GL_SHORT,                        false }, // RGBA16S
	{ GL_RGBA32I,                                  GL_ZERO,                                      RGBA_INTEGER,                                GL_RGBA_INTEGER,                             GL_INT,                          false }, // RGBA32I
	{ GL_RGBA32UI,                                 GL_ZERO,                                      RGBA_INTEGER,                                GL_RGBA_INTEGER,                             GL_UNSIGNED_INT,                 false }, // RGBA32U
	{ GL_RGBA32F,                                  GL_ZERO,                                      GL_RGBA,                                     GL_RGBA,                                     GL_FLOAT,                        false }, // RGBA32F
	{ GL_RGB565,                                   GL_ZERO,                                      GL_RGB,                                      GL_RGB,                                      GL_UNSIGNED_SHORT_5_6_5,         false }, // R5G6B5
	{ GL_RGBA4,                                    GL_ZERO,                                      GL_BGRA,                                     GL_BGRA,                                     GL_UNSIGNED_SHORT_4_4_4_4_REV,   false }, // RGBA4
	{ GL_RGB5_A1,                                  GL_ZERO,                                      GL_BGRA,                                     GL_BGRA,                                     GL_UNSIGNED_SHORT_1_5_5_5_REV,   false }, // RGB5A1
	{ GL_RGB10_A2,                                 GL_ZERO,                                      GL_RGBA,                                     GL_RGBA,                                     GL_UNSIGNED_INT_2_10_10_10_REV,  false }, // RGB10A2
	{ GL_R11F_G11F_B10F,                           GL_ZERO,                                      GL_RGB,                                      GL_RGB,                                      GL_UNSIGNED_INT_10F_11F_11F_REV, false }, // RG11B10F
	{ GL_ZERO,                                     GL_ZERO,                                      GL_ZERO,                                     GL_ZERO,                                     GL_ZERO,                         false }, // UnknownDepth
	{ GL_DEPTH_COMPONENT16,                        GL_ZERO,                                      GL_DEPTH_COMPONENT,                          GL_DEPTH_COMPONENT,                          GL_UNSIGNED_SHORT,               false }, // D16
	{ GL_DEPTH_COMPONENT24,                        GL_ZERO,                                      GL_DEPTH_COMPONENT,                          GL_DEPTH_COMPONENT,                          GL_UNSIGNED_INT,                 false }, // D24
	{ GL_DEPTH24_STENCIL8,                         GL_ZERO,                                      GL_DEPTH_STENCIL,                            GL_DEPTH_STENCIL,                            GL_UNSIGNED_INT_24_8,            false }, // D24S8
	{ GL_DEPTH_COMPONENT32,                        GL_ZERO,                                      GL_DEPTH_COMPONENT,                          GL_DEPTH_COMPONENT,                          GL_UNSIGNED_INT,                 false }, // D32
	{ GL_DEPTH_COMPONENT32F,                       GL_ZERO,                                      GL_DEPTH_COMPONENT,                          GL_DEPTH_COMPONENT,                          GL_FLOAT,                        false }, // D16F
	{ GL_DEPTH_COMPONENT32F,                       GL_ZERO,                                      GL_DEPTH_COMPONENT,                          GL_DEPTH_COMPONENT,                          GL_FLOAT,                        false }, // D24F
	{ GL_DEPTH_COMPONENT32F,                       GL_ZERO,                                      GL_DEPTH_COMPONENT,                          GL_DEPTH_COMPONENT,                          GL_FLOAT,                        false }, // D32F
	{ GL_STENCIL_INDEX8,                           GL_ZERO,                                      GL_STENCIL_INDEX,                            GL_STENCIL_INDEX,                            GL_UNSIGNED_BYTE,                false }, // D0S8
};

GfxTextureOpenGL::GfxTextureOpenGL()
	: GfxTexture()
{

}

GfxTextureOpenGL::~GfxTextureOpenGL()
{
	ReleaseTexture();
}

void GfxTextureOpenGL::CreateTexture2D()
{
	oglTarget_ = textureDesc_.layers_ <= 1 ? GL_TEXTURE_2D : GL_TEXTURE_2D_ARRAY;
	oglInternalFormat_ = textureDesc_.sRGB_ ? openglTextureFormat[textureDesc_.format_].internalFmtSrgb_ : openglTextureFormat[textureDesc_.format_].internalFmt_;
	oglFormat_ = textureDesc_.sRGB_ ? openglTextureFormat[textureDesc_.format_].fmtSrgb_ : openglTextureFormat[textureDesc_.format_].fmt_;
	oglType_ = openglTextureFormat[textureDesc_.format_].type_;
	GL::GenTextures(1, &oglTexture_);
	GL::BindTexture(oglTarget_, oglTexture_);
	GL::PixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (textureDesc_.layers_ <= 1)
		GL::TexStorage2D(oglTarget_, textureDesc_.levels_, oglInternalFormat_, textureDesc_.width_, textureDesc_.height_);
	else
		GL::TexStorage3D(oglTarget_, textureDesc_.levels_, oglInternalFormat_, textureDesc_.width_, textureDesc_.height_, textureDesc_.layers_);
	GL::BindTexture(oglTarget_, 0);

	if (textureDesc_.usage_ == TEXTURE_RENDERTARGET || textureDesc_.usage_ == TEXTURE_DEPTHSTENCIL)
	{
		SharedPtr<GfxRenderSurfaceOpenGL> renderSurface(new GfxRenderSurfaceOpenGL(this, oglFormat_, textureDesc_.width_, textureDesc_.height_));
		gfxRenderSurfaces_.Push(renderSurface);
	}
}

void GfxTextureOpenGL::CreateTexture3D()
{
	oglTarget_ = GL_TEXTURE_3D;
	oglInternalFormat_ = textureDesc_.sRGB_ ? openglTextureFormat[textureDesc_.format_].internalFmtSrgb_ : openglTextureFormat[textureDesc_.format_].internalFmt_;
	oglFormat_ = textureDesc_.sRGB_ ? openglTextureFormat[textureDesc_.format_].fmtSrgb_ : openglTextureFormat[textureDesc_.format_].fmt_;
	oglType_ = openglTextureFormat[textureDesc_.format_].type_;
	GL::GenTextures(1, &oglTexture_);
	GL::BindTexture(oglTarget_, oglTexture_);
	GL::PixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GL::TexStorage3D(oglTarget_, textureDesc_.levels_, oglInternalFormat_, textureDesc_.width_, textureDesc_.height_, textureDesc_.depth_);
	GL::BindTexture(oglTarget_, 0);
}

void GfxTextureOpenGL::CreateTextureCube()
{
	oglTarget_ = textureDesc_.layers_ <= 1 ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_CUBE_MAP_ARRAY;
	oglInternalFormat_ = textureDesc_.sRGB_ ? openglTextureFormat[textureDesc_.format_].internalFmtSrgb_ : openglTextureFormat[textureDesc_.format_].internalFmt_;
	oglFormat_ = textureDesc_.sRGB_ ? openglTextureFormat[textureDesc_.format_].fmtSrgb_ : openglTextureFormat[textureDesc_.format_].fmt_;
	oglType_ = openglTextureFormat[textureDesc_.format_].type_;
	GL::GenTextures(1, &oglTexture_);
	GL::BindTexture(oglTarget_, oglTexture_);
	GL::PixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (textureDesc_.layers_ <= 1)
		GL::TexStorage2D(oglTarget_, textureDesc_.levels_, oglInternalFormat_, textureDesc_.width_, textureDesc_.height_);
	else
		GL::TexStorage3D(oglTarget_, textureDesc_.levels_, oglInternalFormat_, textureDesc_.width_, textureDesc_.height_, textureDesc_.layers_);
	GL::BindTexture(oglTarget_, 0);
}

void GfxTextureOpenGL::ReleaseTexture()
{
	if (oglTarget_ == 0)
		return;

	GL::BindTexture(oglTarget_, 0);
	GL::DeleteTextures(1, &oglTexture_);
	oglTexture_ = 0;
	oglTexture_ = 0;
	oglInternalFormat_ = 0;
	oglFormat_ = 0;
	oglType_ = 0;
	gfxRenderSurfaces_.Clear();
}

void GfxTextureOpenGL::Apply(const void* initialDataPtr)
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

void GfxTextureOpenGL::UpdateTexture(const void* dataPtr)
{

}

void GfxTextureOpenGL::UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height)
{
	if (!oglTexture_)
	{
		FLAGGG_LOG_ERROR("Gfx texture invalid.");
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

	const TextureMipInfo mipInfo = GetMipInfo(level);
	if (x < 0 || x + width > mipInfo.width_ || y < 0 || y + height > mipInfo.height_ || width <= 0 || height <= 0)
	{
		FLAGGG_LOG_ERROR("Texture2D ==> illegal dimensions.");
		return;
	}

	GL::BindTexture(oglTarget_, oglTexture_);

	if (IsCompressed())
	{
		const TextureDetail& detail = GfxTextureUtils::GetTextureDetail(textureDesc_.format_);
		const UInt32 imageSize = (mipInfo.width_ / detail.blockWidth_) * (mipInfo.height_ / detail.blockHeight_) * detail.blockSize_;

		if (oglTarget_ == GL_TEXTURE_2D)
			GL::CompressedTexSubImage2D(oglTarget_, level, x, y, width, height, oglFormat_, imageSize, dataPtr);
		else if (oglTarget_ == GL_TEXTURE_2D_ARRAY)
			GL::CompressedTexSubImage3D(oglTarget_, level, x, y, index, width, height, 1, oglFormat_, imageSize, dataPtr);
		else if (oglTarget_ == GL_TEXTURE_CUBE_MAP)
			GL::CompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, level, x, y, width, height, oglFormat_, imageSize, dataPtr);
		else
			FLAGGG_LOG_ERROR("Gfx texture format conflict.");
	}
	else
	{
		if (oglTarget_ == GL_TEXTURE_2D)
			GL::TexSubImage2D(oglTarget_, level, x, y, width, height, oglFormat_, oglType_, dataPtr);
		else if (oglTarget_ == GL_TEXTURE_2D_ARRAY)
			GL::TexSubImage3D(oglTarget_, level, x, y, index, width, height, 1, oglFormat_, oglType_, dataPtr);
		else if (oglTarget_ == GL_TEXTURE_CUBE_MAP)
			GL::TexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, level, x, y, width, height, oglFormat_, oglType_, dataPtr);
		else
			FLAGGG_LOG_ERROR("Gfx texture format conflict.");
	}

	GL::BindTexture(oglTarget_, 0);
}

void GfxTextureOpenGL::UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 z, UInt32 width, UInt32 height, UInt32 depth)
{
	if (!oglTexture_)
	{
		FLAGGG_LOG_ERROR("Gfx texture invalid.");
		return;
	}

	if (oglTarget_ != GL_TEXTURE_3D)
	{
		FLAGGG_LOG_ERROR("Gfx texture format conflict.");
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

	const TextureMipInfo mipInfo = GetMipInfo(level);
	if (x < 0 || x + width > mipInfo.width_ || y < 0 || y + height > mipInfo.height_ || z < 0 || z + depth > mipInfo.depth_ || width <= 0 || height <= 0 || depth <= 0)
	{
		FLAGGG_LOG_ERROR("Texture3D ==> illegal dimensions.");
		return;
	}

	GL::BindTexture(oglTarget_, oglTexture_);

	if (IsCompressed())
	{
		const TextureDetail& detail = GfxTextureUtils::GetTextureDetail(textureDesc_.format_);
		const UInt32 imageSize = (mipInfo.width_ / detail.blockWidth_) * (mipInfo.height_ / detail.blockHeight_) * detail.blockSize_;

		GL::CompressedTexSubImage3D(oglTarget_, level, x, y, z, width, height, depth, oglFormat_, imageSize, dataPtr);
	}
	else
	{
		GL::TexSubImage3D(oglTarget_, level, x, y, z, width, height, depth, oglFormat_, oglType_, dataPtr);
	}

	GL::BindTexture(oglTarget_, 0);
}

GfxRenderSurface* GfxTextureOpenGL::GetRenderSurface() const
{
	return gfxRenderSurfaces_.Size() ? gfxRenderSurfaces_[0] : nullptr;
}

GfxRenderSurface* GfxTextureOpenGL::GetRenderSurface(UInt32 index) const
{
	return index < gfxRenderSurfaces_.Size() ? gfxRenderSurfaces_[index] : nullptr;
}

}
