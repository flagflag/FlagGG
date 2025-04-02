//
// 屏幕空间反射
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

class ScreenSpaceReflections : public IScreenSpaceReflections
{
	OBJECT_OVERRIDE(ScreenSpaceReflections, IScreenSpaceReflections);
public:
	explicit ScreenSpaceReflections();

	~ScreenSpaceReflections() override;

	void RenderSSR(const ScreenSpaceReflectionsInputData& inputData) override;

	Texture2D* GetGBufferSSR() const override { return GBufferSSR_; }

private:
	// 屏幕空间反射GBuffer
	SharedPtr<Texture2D> GBufferSSR_;

	// SSR shader
	SharedPtr<Shader> SSRShaderVS_;
	SharedPtr<Shader> SSRShaderPS_;

	// ssr参数
	SharedPtr<ShaderParameters> SSRParams_;
};

}
