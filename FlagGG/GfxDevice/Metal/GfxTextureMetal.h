//
// Metal图形层纹理
//

#pragma once

#include "GfxDevice/GfxTexture.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"
#include "mtlpp/mtlpp.hpp"

namespace FlagGG
{

class GfxRenderSurfaceMetal;

class GfxTextureMetal : public GfxTexture
{
	OBJECT_OVERRIDE(GfxTextureMetal, GfxTexture);
public:
	explicit GfxTextureMetal();

	~GfxTextureMetal() override;

	// 应用当前设置
	void Apply(const void* initialDataPtr) override;

	// 更新纹理
	void UpdateTexture(const void* dataPtr) override;

	// 更新2D纹理区域
	void UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height) override;

	// 更新3D纹理区域
	void UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 z, UInt32 width, UInt32 height, UInt32 depth) override;

	// 更新纹理数据
	void UpdateTexture(GfxTexture* gfxTexture) override;


	// 获取texute2d的render surface
	GfxRenderSurface* GetRenderSurface() const override;

	// 获取render surface
	// 1.TextureArray，index传入array的下标
	// 2.TextureCube，index传入cube的face
	GfxRenderSurface* GetRenderSurface(UInt32 index, UInt32 level) const override;

	// 获取Metal纹理
	mtlpp::Texture& GetMetalTexture() { return mtlTexture_; }

	// 转成metal PixelFormat
	static mtlpp::PixelFormat ToMetalPixelFormat(TextureFormat format);

private:
	mtlpp::Texture mtlTexture_;

	// render surfaces
	Vector<SharedPtr<GfxRenderSurfaceMetal>> gfxRenderSurfaces_;
};

}
