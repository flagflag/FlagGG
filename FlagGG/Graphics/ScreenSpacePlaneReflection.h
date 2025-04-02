//
// 屏幕空间平面反射
//

#pragma once

#include "Core/Object.h"
#include "Container/Ptr.h"
#include "Graphics/IScreenSpaceReflection.h"

namespace FlagGG
{

class Texture2D;
class Camera;
class Shader;
class ShaderParameters;

struct ScreenSpacePlaneReflectionsSettings
{
	float normalDistortionStrength_;
	float distanceFromPlaneFadeoutStart_;
	float distanceFromPlaneFadeoutEnd_;
	float angleFromPlaneFadeStart_;
	float angleFromPlaneFadeEnd_;
};

class ScreenSpacePlaneReflections : public IScreenSpaceReflections
{
	OBJECT_OVERRIDE(ScreenSpacePlaneReflections, IScreenSpaceReflections);
public:
	explicit ScreenSpacePlaneReflections();

	~ScreenSpacePlaneReflections() override;

	void RenderSSR(const ScreenSpaceReflectionsInputData& inputData) override;

	Texture2D* GetGBufferSSR() const override { return GBufferSSR_; }

protected:
	void SetShaderParameters(const ScreenSpaceReflectionsInputData& inputData);

private:
	// 屏幕空间平面反射GBuffer
	SharedPtr<Texture2D> GBufferSSR_;

	// SSR shader
	SharedPtr<Shader> SSRShaderVS_;
	SharedPtr<Shader> SSRShaderPS_;

	// ssr参数
	SharedPtr<ShaderParameters> SSRParams_;

	// 平面反射设置
	ScreenSpacePlaneReflectionsSettings settings_;
};

}
