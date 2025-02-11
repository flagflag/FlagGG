//
// 抽象图形层纹理
//

#pragma once

#include "GfxDevice/GfxObject.h"
#include "GfxDevice/GfxTextureUtils.h"
#include "Graphics/GraphicsDef.h"
#include "Container/FlagSet.h"

namespace FlagGG
{

FLAGGG_FLAGSET(TextureBind, TextureBindFlags);

struct TextureDesc
{
	TextureFormat format_{};
	UInt32 levels_{};
	UInt32 requestedLevels_{};
	UInt32 width_{};
	UInt32 height_{};
	UInt32 depth_{};
	UInt32 layers_{};
	bool isCube_{};
	UInt32 multiSample_{};
	bool autoResolve_{};
	bool sRGB_{};
	bool subResourceViewEnable_{};   // 是否为纹理子资源创建视图（例如：为纹理的不同Mips创建TextureView）
	TextureUsage usage_{};
	TextureBindFlags bindFlags_{};
	StorageMode storageMode_{};
};

class GfxShaderResourceView;
class GfxRenderSurface;

class FlagGG_API GfxTexture : public GfxObject
{
	OBJECT_OVERRIDE(GfxTexture, GfxObject);
public:
	explicit GfxTexture();

	~GfxTexture() override;

	// 设置纹理格式
	virtual void SetFormat(TextureFormat format);

	// 设置纹理mips
	virtual void SetNumLevels(UInt32 levels);

	// 设置纹理宽度
	virtual void SetWidth(UInt32 width);

	// 设置纹理高度
	virtual void SetHeight(UInt32 height);

	// 设置纹理深度
	virtual void SetDepth(UInt32 depth);

	// 设置纹理Array
	virtual void SetLayers(UInt32 layers);

	// 设置纹理Cube
	virtual void SetCube(bool cube);

	// 是否为纹理子资源创建视图
	virtual void SetSubResourceViewEnabled(bool enable);

	// 设置纹理多重采样
	virtual void SetMultiSample(UInt32 multiSample);

	// 设置纹理auto resolve
	virtual void SetAutoResolve(bool autoResolve);

	// 设置纹理srgb
	virtual void SetSRGB(bool sRGB);

	// 设置纹理用法
	virtual void SetUsage(TextureUsage usage);

	// 设置纹理绑定类型
	virtual void SetBind(TextureBindFlags bindFlags);

	// 应用当前设置
	virtual void Apply(const void* initialDataPtr);

	// 更新纹理数据
	virtual void UpdateTexture(const void* dataPtr);

	// 更新2D纹理区域
	virtual void UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height);

	// 更新3D纹理区域
	virtual void UpdateTextureSubRegion(const void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 z, UInt32 width, UInt32 height, UInt32 depth);

	// 更新纹理数据
	virtual void UpdateTexture(GfxTexture* gfxTexture);

	// 回读GPU数据
	virtual bool ReadBack(void* dataPtr, UInt32 index, UInt32 level);

	// 回去GPU数据（某个Rect区域）
	virtual bool ReadBackSubRegion(void* dataPtr, UInt32 index, UInt32 level, UInt32 x, UInt32 y, UInt32 width, UInt32 height);

	// 获取子资源的视图
	virtual GfxShaderResourceView* GetSubResourceView(UInt32 index, UInt32 level);

	// 获取texute2d的render surface
	virtual GfxRenderSurface* GetRenderSurface() const;

	// 获取render surface
	// 1.TextureArray，index传入array的下标
	// 2.TextureCube，index传入cube的face
	virtual GfxRenderSurface* GetRenderSurface(UInt32 index, UInt32 level) const;


	// 获取纹理描述
	const TextureDesc& GetDesc() const { return textureDesc_; }

	// 获取纹理细节
	const TextureDetail& GetDetail() const;

	// 获取纹理mip信息
	TextureMipInfo GetMipInfo(UInt32 level) const;

	// 纹理是否压缩
	bool IsCompressed() const;

protected:
	TextureDesc textureDesc_;
};

}
