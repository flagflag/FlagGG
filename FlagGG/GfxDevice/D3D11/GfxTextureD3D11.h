#pragma once

#include "GfxDevice/GfxTexture.h"
#include "Container/Vector.h"
#include "Container/Ptr.h"

#include <d3d11.h>

namespace FlagGG
{

class GfxRenderSurfaceD3D11;

class GfxTextureD3D11 : public GfxTexture
{
	OBJECT_OVERRIDE(GfxTextureD3D11, GfxTexture);
public:
	explicit GfxTextureD3D11();

	~GfxTextureD3D11() override;

	// 应用当前设置
	void Apply(const void* initialDataPtr) override;

	// 更新纹理
	void UpdateTexture(const void* dataPtr) override;

	// 更新2D纹理区域
	void UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height) override;

	// 更新3D纹理区域
	void UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 z, UInt32 width, UInt32 height, UInt32 depth) override;


	// 获取texute2d的render surface
	virtual GfxRenderSurface* GetRenderSurface() const;

	// 获取render surface
	// 1.TextureArray，index传入array的下标
	// 2.TextureCube，index传入cube的face
	virtual GfxRenderSurface* GetRenderSurface(UInt32 index) const;

	// 获取ID3D11ShaderResourceView*
	ID3D11ShaderResourceView* GetD3D11ShaderResourceView() const { return shaderResourceView_; }

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

	// render surfaces
	Vector<SharedPtr<GfxRenderSurfaceD3D11>> gfxRenderSurfaces_;
};

}
