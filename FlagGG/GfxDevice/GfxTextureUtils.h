#pragma once

#include "Export.h"
#include "Core/BaseTypes.h"
#include "Graphics/GraphicsDef.h"

namespace FlagGG
{

struct TextureDetail
{
	bool isCompressed_;   // 是否是压缩格式
	UInt8 bitsPerPixel_;  // 纹理每像素的bits数
	UInt8 blockWidth_;    // 块压缩纹理，每块的宽度
	UInt8 blockHeight_;   // 块压缩纹理，每块的高度
	UInt8 blockSize_;     // 快压缩纹理，每块的大小
	UInt8 minBlockX_;
	UInt8 minBlockY_;
	UInt8 depthBits_;     // 深度的bits数
	UInt8 stencilBits_;   // 模板的bits数
	UInt8 rBits_;         // 纹理r通道的bits数
	UInt8 gBits_;         // 纹理g通道的bits数
	UInt8 bBits_;         // 纹理b通道的bits数
	UInt8 aBits_;         // 纹理a通道的bits数
	UInt8 encodingType_;  // 纹理编码类型 
};

struct TextureMipInfo
{
	UInt32 width_;
	UInt32 height_;
	UInt32 depth_;
};

class FlagGG_API GfxTextureUtils
{
public:
	// 获取纹理格式的细节
	static const TextureDetail& GetTextureDetail(TextureFormat format);

	// 获取纹理level层mip的信息
	static TextureMipInfo GetTextureMipInfo(TextureFormat format, UInt32 width, UInt32 height, UInt32 depth, UInt32 level);

	// 根据宽度获取行数据大小
	static UInt32 GetRowDataSize(TextureFormat format, Int32 width);

	// 
	static UInt32 CheckMaxLevels(UInt32 width, UInt32 height, UInt32 requestedLevels);

	static UInt32 CheckMaxLevels(UInt32 width, UInt32 height, UInt32 depth, UInt32 requestedLevels);
};

}
