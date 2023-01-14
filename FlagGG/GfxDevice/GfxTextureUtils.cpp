#include "GfxTextureUtils.h"
#include "Math/Math.h"
#include "Log.h"

namespace FlagGG
{

TextureDetail TEXTURE_FORMAT_DETAIL[] =
{
	//  +--------------------------------------------- bits per pixel
	//  |   +----------------------------------------- block width
	//  |   |  +-------------------------------------- block height
	//  |   |  |   +---------------------------------- block size
	//  |   |  |   |  +------------------------------- min blocks x
	//  |   |  |   |  |  +---------------------------- min blocks y
	//  |   |  |   |  |  |   +------------------------ depth bits
	//  |   |  |   |  |  |   |  +--------------------- stencil bits
	//  |   |  |   |  |  |   |  |   +---+---+---+----- r, g, b, a bits
	//  |   |  |   |  |  |   |  |   r   g   b   a  +-- encoding type
	//  |   |  |   |  |  |   |  |   |   |   |   |  |
	{   4,  4, 4,  8, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // BC1
	{   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // BC2
	{   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // BC3
	{   4,  4, 4,  8, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // BC4
	{   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // BC5
	{   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_FLOAT }, // BC6H
	{   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // BC7
	{   4,  4, 4,  8, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // ETC1
	{   4,  4, 4,  8, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // ETC2
	{   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // ETC2A
	{   4,  4, 4,  8, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // ETC2A1
	{   2,  8, 4,  8, 2, 2,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // PTC12
	{   4,  4, 4,  8, 2, 2,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // PTC14
	{   2,  8, 4,  8, 2, 2,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // PTC12A
	{   4,  4, 4,  8, 2, 2,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // PTC14A
	{   2,  8, 4,  8, 2, 2,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // PTC22
	{   4,  4, 4,  8, 2, 2,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // PTC24
	{   4,  4, 4,  8, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // ATC
	{   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // ATCE
	{   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // ATCI
	{   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // ASTC4x4
	{   6,  5, 5, 16, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // ASTC5x5
	{   4,  6, 6, 16, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // ASTC6x6
	{   4,  8, 5, 16, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // ASTC8x5
	{   3,  8, 6, 16, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // ASTC8x6
	{   3, 10, 5, 16, 1, 1,  0, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // ASTC10x5
	{   0,  0, 0,  0, 0, 0,  0, 0,  0,  0,  0,  0, MAX_TEXTURE_ENCODING_TYPE }, // Unknown
	{   1,  8, 1,  1, 1, 1,  0, 0,  1,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // R1
	{   8,  1, 1,  1, 1, 1,  0, 0,  0,  0,  0,  8, TEXTURE_ENCODING_UNORM }, // A8
	{   8,  1, 1,  1, 1, 1,  0, 0,  8,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // R8
	{   8,  1, 1,  1, 1, 1,  0, 0,  8,  0,  0,  0, TEXTURE_ENCODING_INT }, // R8I
	{   8,  1, 1,  1, 1, 1,  0, 0,  8,  0,  0,  0, TEXTURE_ENCODING_UINT }, // R8U
	{   8,  1, 1,  1, 1, 1,  0, 0,  8,  0,  0,  0, TEXTURE_ENCODING_SNORM }, // R8S
	{  16,  1, 1,  2, 1, 1,  0, 0, 16,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // R16
	{  16,  1, 1,  2, 1, 1,  0, 0, 16,  0,  0,  0, TEXTURE_ENCODING_INT }, // R16I
	{  16,  1, 1,  2, 1, 1,  0, 0, 16,  0,  0,  0, TEXTURE_ENCODING_UINT }, // R16U
	{  16,  1, 1,  2, 1, 1,  0, 0, 16,  0,  0,  0, TEXTURE_ENCODING_FLOAT }, // R16F
	{  16,  1, 1,  2, 1, 1,  0, 0, 16,  0,  0,  0, TEXTURE_ENCODING_SNORM }, // R16S
	{  32,  1, 1,  4, 1, 1,  0, 0, 32,  0,  0,  0, TEXTURE_ENCODING_INT }, // R32I
	{  32,  1, 1,  4, 1, 1,  0, 0, 32,  0,  0,  0, TEXTURE_ENCODING_UINT }, // R32U
	{  32,  1, 1,  4, 1, 1,  0, 0, 32,  0,  0,  0, TEXTURE_ENCODING_FLOAT }, // R32F
	{  16,  1, 1,  2, 1, 1,  0, 0,  8,  8,  0,  0, TEXTURE_ENCODING_UNORM }, // RG8
	{  16,  1, 1,  2, 1, 1,  0, 0,  8,  8,  0,  0, TEXTURE_ENCODING_INT }, // RG8I
	{  16,  1, 1,  2, 1, 1,  0, 0,  8,  8,  0,  0, TEXTURE_ENCODING_UINT }, // RG8U
	{  16,  1, 1,  2, 1, 1,  0, 0,  8,  8,  0,  0, TEXTURE_ENCODING_SNORM }, // RG8S
	{  32,  1, 1,  4, 1, 1,  0, 0, 16, 16,  0,  0, TEXTURE_ENCODING_UNORM }, // RG16
	{  32,  1, 1,  4, 1, 1,  0, 0, 16, 16,  0,  0, TEXTURE_ENCODING_INT }, // RG16I
	{  32,  1, 1,  4, 1, 1,  0, 0, 16, 16,  0,  0, TEXTURE_ENCODING_UINT }, // RG16U
	{  32,  1, 1,  4, 1, 1,  0, 0, 16, 16,  0,  0, TEXTURE_ENCODING_FLOAT }, // RG16F
	{  32,  1, 1,  4, 1, 1,  0, 0, 16, 16,  0,  0, TEXTURE_ENCODING_SNORM }, // RG16S
	{  64,  1, 1,  8, 1, 1,  0, 0, 32, 32,  0,  0, TEXTURE_ENCODING_INT }, // RG32I
	{  64,  1, 1,  8, 1, 1,  0, 0, 32, 32,  0,  0, TEXTURE_ENCODING_UINT }, // RG32U
	{  64,  1, 1,  8, 1, 1,  0, 0, 32, 32,  0,  0, TEXTURE_ENCODING_FLOAT }, // RG32F
	{  24,  1, 1,  3, 1, 1,  0, 0,  8,  8,  8,  0, TEXTURE_ENCODING_UNORM }, // RGB8
	{  24,  1, 1,  3, 1, 1,  0, 0,  8,  8,  8,  0, TEXTURE_ENCODING_INT }, // RGB8I
	{  24,  1, 1,  3, 1, 1,  0, 0,  8,  8,  8,  0, TEXTURE_ENCODING_UINT }, // RGB8U
	{  24,  1, 1,  3, 1, 1,  0, 0,  8,  8,  8,  0, TEXTURE_ENCODING_SNORM }, // RGB8S
	{  32,  1, 1,  4, 1, 1,  0, 0,  9,  9,  9,  5, TEXTURE_ENCODING_FLOAT }, // RGB9E5F
	{  32,  1, 1,  4, 1, 1,  0, 0,  8,  8,  8,  8, TEXTURE_ENCODING_UNORM }, // BGRA8
	{  32,  1, 1,  4, 1, 1,  0, 0,  8,  8,  8,  8, TEXTURE_ENCODING_UNORM }, // RGBA8
	{  32,  1, 1,  4, 1, 1,  0, 0,  8,  8,  8,  8, TEXTURE_ENCODING_INT }, // RGBA8I
	{  32,  1, 1,  4, 1, 1,  0, 0,  8,  8,  8,  8, TEXTURE_ENCODING_UINT }, // RGBA8U
	{  32,  1, 1,  4, 1, 1,  0, 0,  8,  8,  8,  8, TEXTURE_ENCODING_SNORM }, // RGBA8S
	{  64,  1, 1,  8, 1, 1,  0, 0, 16, 16, 16, 16, TEXTURE_ENCODING_UNORM }, // RGBA16
	{  64,  1, 1,  8, 1, 1,  0, 0, 16, 16, 16, 16, TEXTURE_ENCODING_INT }, // RGBA16I
	{  64,  1, 1,  8, 1, 1,  0, 0, 16, 16, 16, 16, TEXTURE_ENCODING_UINT }, // RGBA16U
	{  64,  1, 1,  8, 1, 1,  0, 0, 16, 16, 16, 16, TEXTURE_ENCODING_FLOAT }, // RGBA16F
	{  64,  1, 1,  8, 1, 1,  0, 0, 16, 16, 16, 16, TEXTURE_ENCODING_SNORM }, // RGBA16S
	{ 128,  1, 1, 16, 1, 1,  0, 0, 32, 32, 32, 32, TEXTURE_ENCODING_INT }, // RGBA32I
	{ 128,  1, 1, 16, 1, 1,  0, 0, 32, 32, 32, 32, TEXTURE_ENCODING_UINT }, // RGBA32U
	{ 128,  1, 1, 16, 1, 1,  0, 0, 32, 32, 32, 32, TEXTURE_ENCODING_FLOAT }, // RGBA32F
	{  16,  1, 1,  2, 1, 1,  0, 0,  5,  6,  5,  0, TEXTURE_ENCODING_UNORM }, // R5G6B5
	{  16,  1, 1,  2, 1, 1,  0, 0,  4,  4,  4,  4, TEXTURE_ENCODING_UNORM }, // RGBA4
	{  16,  1, 1,  2, 1, 1,  0, 0,  5,  5,  5,  1, TEXTURE_ENCODING_UNORM }, // RGB5A1
	{  32,  1, 1,  4, 1, 1,  0, 0, 10, 10, 10,  2, TEXTURE_ENCODING_UNORM }, // RGB10A2
	{  32,  1, 1,  4, 1, 1,  0, 0, 11, 11, 10,  0, TEXTURE_ENCODING_UNORM }, // RG11B10F
	{   0,  0, 0,  0, 0, 0,  0, 0,  0,  0,  0,  0, MAX_TEXTURE_ENCODING_TYPE }, // UnknownDepth
	{  16,  1, 1,  2, 1, 1, 16, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // D16
	{  24,  1, 1,  3, 1, 1, 24, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // D24
	{  32,  1, 1,  4, 1, 1, 24, 8,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // D24S8
	{  32,  1, 1,  4, 1, 1, 32, 0,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // D32
	{  16,  1, 1,  2, 1, 1, 16, 0,  0,  0,  0,  0, TEXTURE_ENCODING_FLOAT }, // D16F
	{  24,  1, 1,  3, 1, 1, 24, 0,  0,  0,  0,  0, TEXTURE_ENCODING_FLOAT }, // D24F
	{  32,  1, 1,  4, 1, 1, 32, 0,  0,  0,  0,  0, TEXTURE_ENCODING_FLOAT }, // D32F
	{   8,  1, 1,  1, 1, 1,  0, 8,  0,  0,  0,  0, TEXTURE_ENCODING_UNORM }, // D0S8
};

bool GfxTextureUtils::IsCompressed(TextureFormat format)
{
	return format < TEXTURE_FORMAT_UNKNOWN;
}

const TextureDetail& GfxTextureUtils::GetTextureDetail(TextureFormat format)
{
	return TEXTURE_FORMAT_DETAIL[format];
}

TextureMipInfo GfxTextureUtils::GetTextureMipInfo(TextureFormat format, UInt32 width, UInt32 height, UInt32 depth, UInt32 levels, UInt32 level)
{
	TextureMipInfo mipInfo{};

	if (!IsCompressed(format))
	{
		mipInfo.width_ = Max(width >> level, 1u);
		mipInfo.height_ = Max(height >> level, 1u);
		mipInfo.depth_ = Max(depth >> level, 1u);
	}
	else
	{
		const TextureDetail& detail = GetTextureDetail(format);

		for (UInt32 i = 0; i <= level && i < levels; ++i)
		{
			if (i > 0)
			{
				width >>= 1;
				height >>= 1;
				depth >>= 1;
			}

			width = Max((UInt32)detail.blockWidth_ * detail.minBlockX_, ((width + detail.blockWidth_ - 1) / detail.blockWidth_) * detail.blockWidth_);
			height = Max((UInt32)detail.blockHeight_ * detail.minBlockY_, ((height + detail.blockHeight_ - 1) / detail.blockHeight_) * detail.blockHeight_);
			depth = Max(1u, depth);
		}

		mipInfo.width_ = width;
		mipInfo.height_ = height;
		mipInfo.depth_ = depth;
	}

	return mipInfo;
}

UInt32 GfxTextureUtils::GetRowDataSize(TextureFormat format, Int32 width)
{
/*
      | bits per pixel |  block width |  block height |  block size |
---------------------------------------------------------------------
BC1   |       4        |       4      |       4       |       8     |
ASTC4 |       8        |       4      |       4       |      16     |
RGBA  |      32        |       1      |       1       |       4     |
---------------------------------------------------------------------
*/

	const TextureDetail& detail = GetTextureDetail(format);
	return (width + detail.blockWidth_ - 1) / detail.blockWidth_ * detail.blockSize_;
}

UInt32 GfxTextureUtils::CheckMaxLevels(UInt32 width, UInt32 height, UInt32 requestedLevels)
{
	UInt32 maxLevels = 1;
	while (width > 1 || height > 1)
	{
		++maxLevels;
		width = width > 1 ? (width >> 1u) : 1;
		height = height > 1 ? (height >> 1u) : 1;
	}

	if (!requestedLevels || maxLevels < requestedLevels)
	{
		return maxLevels;
	}

	return requestedLevels;
}

UInt32 GfxTextureUtils::CheckMaxLevels(UInt32 width, UInt32 height, UInt32 depth, UInt32 requestedLevels)
{
	UInt32 maxLevels = 1;
	while (width > 1 || height > 1)
	{
		++maxLevels;
		width = width > 1 ? (width >> 1u) : 1;
		height = height > 1 ? (height >> 1u) : 1;
		depth = depth > 1 ? (depth >> 1u) : 1;
	}

	if (!requestedLevels || maxLevels < requestedLevels)
	{
		return maxLevels;
	}

	return requestedLevels;
}

}
