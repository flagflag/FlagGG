#pragma once

#include "Export.h"
#include "Resource/Image.h"

namespace FlagGG
{
	namespace Resource
	{
		FlagGG_API void DecompressImageDXT(unsigned char* rgba, const void* blocks, int width, int height, int depth, CompressedFormat format);

		FlagGG_API void DecompressImageETC(unsigned char* rgba, const void* blocks, int width, int height);

		FlagGG_API void DecompressImagePVRTC(unsigned char* rgba, const void* blocks, int width, int height, CompressedFormat format);

		FlagGG_API void FlipBlockVertical(unsigned char* dest, const unsigned char* src, CompressedFormat format);

		FlagGG_API void FlipBlockHorizontal(unsigned char* dest, const unsigned char* src, CompressedFormat format);
	}
}