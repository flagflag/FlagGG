//
// D3D11图形层纹理
//

#pragma once

#include "GfxDevice/GfxTexture.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"

#include <d3d11.h>

namespace FlagGG
{

class GfxShaderResourceViewD3D11;
class GfxRenderSurfaceD3D11;

class GfxTextureD3D11 : public GfxTexture
{
	OBJECT_OVERRIDE(GfxTextureD3D11, GfxTexture);
public:
	explicit GfxTextureD3D11();

	~GfxTextureD3D11() override;

	// 设置Gpu tag
	void SetGpuTag(const String& gpuTag) override;

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

	// 回读GPU数据
	bool ReadBack(void* dataPtr, UInt32 index, UInt32 level) override;

	// 回去GPU数据（某个Rect区域）
	bool ReadBackSubRegion(void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height) override;

	// 获取子资源的视图
	GfxShaderResourceView* GetSubResourceView(UInt32 index, UInt32 level) override;

	// 获取texute2d的render surface
	GfxRenderSurface* GetRenderSurface() const override;

	// 获取render surface
	// 1.TextureArray，index传入array的下标
	// 2.TextureCube，index传入cube的face
	GfxRenderSurface* GetRenderSurface(UInt32 index, UInt32 level) const override;

	// 获取ID3D11ShaderResourceView*
	ID3D11ShaderResourceView* GetD3D11ShaderResourceView() const { return shaderResourceView_; }

	// 获取ID3D11Resource
	ID3D11Resource* GetD3D11Resource() const;

protected:
	void CreateTexture2D();
	void CreateTexture3D();
	void CreateTextureCube();
	void ReleaseTexture();

private:
	// texture 2d
	ID3D11Texture2D* d3d11Texture2D_{};

	// texture 3d
	ID3D11Texture3D* d3d11Texture3D_{};

	// resolve texture
	ID3D11Resource* resolveTexture_{};

	// shader resource view
	ID3D11ShaderResourceView* shaderResourceView_{};

	// texture sub resource view
	Vector<SharedPtr<GfxShaderResourceViewD3D11>> gfxTextureViews_;

	// render surfaces
	Vector<SharedPtr<GfxRenderSurfaceD3D11>> gfxRenderSurfaces_;
};

}
