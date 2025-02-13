//
// 屏幕空间反射
//

#pragma once

#include "Core/Object.h"
#include "Container/Ptr.h"
#include "Math/Vector2.h"

namespace FlagGG
{

class Texture2D;
class Camera;
class Shader;
class ShaderParameters;

struct ScreenSpaceReflectionsInputData
{
	Texture2D* GBufferA_;
	Texture2D* GBufferB_;
	Texture2D* GBufferC_;
	Texture2D* screenDepthTexture_;
	Texture2D* HiZMap_;
	Camera* camera_;
	IntVector2 renderSolution_;
};

class ScreenSpaceReflections : public Object
{
	OBJECT_OVERRIDE(ScreenSpaceReflections, Object);
public:
	explicit ScreenSpaceReflections();

	~ScreenSpaceReflections() override;

	void RenderSSR(const ScreenSpaceReflectionsInputData& inputData);

	Texture2D* GetGBufferSSR() { return GBufferSSR_; }

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
