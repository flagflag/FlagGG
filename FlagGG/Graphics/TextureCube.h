//
// 引擎层Cube纹理
//

#pragma once

#include "Graphics/GraphicsDef.h"
#include "Graphics/Texture.h"
#include "Resource/Image.h"

#include <string>

namespace FlagGG
{

class FlagGG_API TextureCube : public Texture
{
	OBJECT_OVERRIDE(TextureCube, Texture);
public:
	TextureCube();

	/**
	 * 设置纹理大小
	 * size        - 纹理宽高
	 * format      - 纹理格式
	 * usage       - 纹理用法
	 * multiSample - 多重采样
	 */
	bool SetSize(UInt32 size, TextureFormat format, TextureUsage usage = TEXTURE_STATIC, Int32 multiSample = 1);

	/**
	 * 设置纹理区域数据
	 * face   - Cube face
	 * level  - 纹理mip层级
	 * x      - 纹理像素坐标x
	 * y      - 纹理像素坐标y
	 * width  - 纹理数据宽度
	 * height - 纹理数据高度
	 * data   - 纹理数据buffer
	 */
	bool SetData(CubeMapFace face, UInt32 level, Int32 x, Int32 y, Int32 width, Int32 height, const void* data);

	// 设置纹理数据
	bool SetData(CubeMapFace face, Image* image, bool useAlpha = false);

	// 从GPU回读纹理数据
	bool GetData(CubeMapFace face, UInt32 level, void* dest);

	// 从GPU回读纹理数据，以Image格式返回数据
	SharedPtr<Image> GetImage(CubeMapFace face);

protected:
	bool LoadDDS(IOFrame::Buffer::IOBuffer* stream);

	bool LoadMulti2DFace(IOFrame::Buffer::IOBuffer* stream);

	bool BeginLoad(IOFrame::Buffer::IOBuffer* stream) override;

	bool EndLoad() override;

private:
	UInt32 faceMemoryUse_[MAX_CUBEMAP_FACES];
};

}
