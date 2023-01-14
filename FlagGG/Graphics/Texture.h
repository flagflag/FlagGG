//
// 引擎层纹理基类
//

#pragma once

#include "Export.h"

#include "Graphics/GraphicsDef.h"
#include "Resource/Resource.h"

#include <stdint.h>

namespace FlagGG
{

class GfxTexture;
class GfxSampler;
class GfxRenderSurface;

static const int MAX_TEXTURE_QUALITY_LEVELS = 3;

class FlagGG_API Texture : public Resource
{
public:
	Texture(Context* context);

	~Texture() override;

	// 设置纹理mips层数
	void SetNumLevels(UInt32 levels);

	// 设置Filter
	void SetFilterMode(TextureFilterMode filterMode);

	// 设置寻址方式
	void SetAddressMode(TextureCoordinate coord, TextureAddressMode addressMode);

	// 设置比较方式
	void SetComparisonFunc(ComparisonFunc comparisonFunc);

	// 获取纹理宽度
	Int32 GetWidth() const;

	// 获取纹理高度
	Int32 GetHeight() const;

	// 获取纹理深度（2D纹理返回1，3D纹理返回实际深度）
	Int32 GetDepth() const;

	// 纹理是否是压缩
	bool IsCompressed() const;

	// 获取纹理Components数。例如：RGBA8，components=4
	UInt32 GetComponents() const;

	// 获取Surface
	GfxRenderSurface* GetRenderSurface() const;

	// 获取Surface。如果是Cube纹理index填写Face（详见枚举类型CubeMapFace）。如果是Array纹理index填写数组下标。
	GfxRenderSurface* GetRenderSurface(UInt32 index) const;

	// 获取GfxTexture引用
	GfxTexture* GetGfxTextureRef() const { return gfxTexture_; }

	// 获取GfxSampler引用
	GfxSampler* GetGfxSamplerRef() const { return gfxSampler_; }

	friend class RenderEngine;

protected:
	UInt32 mipsToSkip_[MAX_TEXTURE_QUALITY_LEVELS];

	SharedPtr<GfxTexture> gfxTexture_;

	SharedPtr<GfxSampler> gfxSampler_;
};

}
