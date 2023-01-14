#include "GfxSampler.h"

namespace FlagGG
{

GfxSampler::GfxSampler()
	: desc_{ TEXTURE_FILTER_BILINEAR, { TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP }, COMPARISON_NEVER }
{

}

GfxSampler::~GfxSampler()
{

}

void GfxSampler::SetFilterMode(TextureFilterMode filterMode)
{
	desc_.filterMode_ = filterMode;
	hashDirty_ = true;
}

void GfxSampler::SetAddressMode(TextureCoordinate coord, TextureAddressMode addressMode)
{
	desc_.addresMode_[coord] = addressMode;
	hashDirty_ = true;
}

void GfxSampler::SetComparisonFunc(ComparisonFunc comparisonFunc)
{
	desc_.comparisonFunc_ = comparisonFunc;
	hashDirty_ = true;
}

UInt32 GfxSampler::GetHash() const
{
	if (hashDirty_)
	{
		hashValue_ = desc_.comparisonFunc_;
		hashValue_ = (hashValue_ << 3) | desc_.filterMode_;
		hashValue_ = (hashValue_ << 2) | desc_.addresMode_[TEXTURE_COORDINATE_U];
		hashValue_ = (hashValue_ << 2) | desc_.addresMode_[TEXTURE_COORDINATE_V];
		hashValue_ = (hashValue_ << 2) | desc_.addresMode_[TEXTURE_COORDINATE_W];

		hashDirty_ = false;
	}
	return hashValue_;
}

}
