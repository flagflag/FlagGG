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
	UInt32 GetDrawableFlags() const override { return DRAWABLE_PROBE; }

	// 加载
	void Load(const String& path);

	// 设置漫反射强度
	void SetDiffuseIntensity(float diffuseIntensity);

	// 设置镜面反射强度
	void SetSpecularIntensity(float specularIntensity);

	// 设置探针影响范围
	void SetArea(const BoundingBox& bbox);

	// 应用shader参数（在渲染阶段被调用）
	void ApplyRender(RenderEngine* renderEngine);

private:
	// 预计算漫反射球谐
	Vector4 shaderConstants_[SphericalHarmonicsL2::kVec4Count];

	// 预计算高光辐照度
	SharedPtr<TextureCube> specularCube_;

	// 漫反射强度
	float diffuseIntensity_;

	// 镜面反射强度
	float specularIntensity_;
};

}
