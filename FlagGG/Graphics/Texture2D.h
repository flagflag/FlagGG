//
// 引擎层2D纹理
//

#pragma once

#include "Graphics/Texture.h"
#include "Resource/Image.h"

#include <string>

namespace FlagGG
{

#define USE_DDS

class FlagGG_API Texture2D : public Texture
{
public:
	Texture2D(Context* context);

	/**
	 * 设置纹理大小
	 * width       - 纹理宽度
	 * height      - 纹理高度
	 * format      - 纹理格式
	 * usage       - 纹理用法
	 * multiSample - 多重采样
	 * autoResolve - 自动resolve
	 */
	bool SetSize(Int32 width, Int32 height, TextureFormat format, TextureUsage usage = TEXTURE_STATIC, Int32 multiSample = 1, bool autoResolve = true);

	/**
	 * 设置纹理区域数据
	 * level  - 纹理mip层级
	 * x      - 纹理像素坐标x
	 * y      - 纹理像素坐标y
	 * width  - 纹理数据宽度
	 * height - 纹理数据高度
	 * data   - 纹理数据buffer
	 */
	bool SetData(UInt32 level, Int32 x, Int32 y, Int32 width, Int32 height, const void* data);

	// 设置纹理数据
	bool SetData(Image* image, bool useAlpha = false);

	// 从GPU回读纹理数据
	bool GetData(UInt32 level, void* dest);

	// 从GPU回读纹理数据，以Image格式返回数据
	SharedPtr<Image> GetImage();

protected:
	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;
};

}
