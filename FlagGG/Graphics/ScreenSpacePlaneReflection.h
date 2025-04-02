//
// ��Ļ�ռ�ƽ�淴��
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
	// ��Ļ�ռ�ƽ�淴��GBuffer
	SharedPtr<Texture2D> GBufferSSR_;

	// SSR shader
	SharedPtr<Shader> SSRShaderVS_;
	SharedPtr<Shader> SSRShaderPS_;

	// ssr����
	SharedPtr<ShaderParameters> SSRParams_;

	// ƽ�淴������
	ScreenSpacePlaneReflectionsSettings settings_;
};

}
