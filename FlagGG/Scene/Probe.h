//
// 环境光探针
//

#pragma once

#include "Scene/DrawableComponent.h"
#include "Math/SphericalHarmonicsL2.h"
#include "Graphics/TextureCube.h"

namespace FlagGG
{

class RenderEngine;

class FlagGG_API Probe : public DrawableComponent
{
	OBJECT_OVERRIDE(Probe, DrawableComponent);
public:
	Probe();

	// 获取DrawableFlags
	virtual UInt32 GetDrawableFlags() const { return DRAWABLE_PROBE; }

	void SetArea(const BoundingBox& bbox);

	void ApplyRender(RenderEngine* renderEngine);

private:
	Vector4 shaderConstants_[SphericalHarmonicsL2::kVec4Count];
	SharedPtr<TextureCube> iblCube_;
};

}
