//
// 抽象图形层纹理采样器
//

#pragma once

#include "GfxDevice/GfxObject.h"
#include "Graphics/GraphicsDef.h"

namespace FlagGG
{

struct SamplerDesc
{
	TextureFilterMode filterMode_;
	TextureAddressMode addresMode_[MAX_TEXTURE_COORDINATE];
	ComparisonFunc comparisonFunc_;
};

class GfxSampler : public GfxObject
{
	OBJECT_OVERRIDE(GfxSampler, GfxObject);
public:
	explicit GfxSampler();

	~GfxSampler() override;

	// 设置Filter
	virtual void SetFilterMode(TextureFilterMode filterMode);

	// 设置寻址方式
	virtual void SetAddressMode(TextureCoordinate coord, TextureAddressMode addressMode);

	// 设置比较方式
	virtual void SetComparisonFunc(ComparisonFunc comparisonFunc);

	// 获取采样器描述
	const SamplerDesc& GetDesc() const { return desc_; }

	// Hash
	UInt32 GetHash() const;

private:
	SamplerDesc desc_;

	mutable bool hashDirty_{true};
	mutable UInt32 hashValue_{};
};

}
