//
// 软件环境光遮蔽渲染器
//

#pragma once

#include "Graphics/AmbientOcclusionRendering.h"

namespace FlagGG
{

class Shader;
class ShaderParameters;

struct AmbientOcclusionSettings
{
	float ambientOcclusionRadius_{ 200.0f };         // ao半径
	float ambientOcclusionPower_{ 2 };               // ao强度
	float ambientOcclusionBias_{ 3 };                // ao偏移
	float ambientOcclusionDistance_{ 80.0f };        // ao距离
	float ambientOcclusionIntensity_{ 0.5f };        // ao强度
	float ambientOcclusionFadeRadius_{ 5000.0f };    // 过度半径
	float ambientOcclusionFadeDistance_{ 8000.0f };  // 过度距离
	float ambientOcclusionMipScale_{ 1.7f };         //
	float ambientOcclusionMipThreshold_{ 0.01 };     //
	float ambientOcclusionMipBlend_{ 0.6 };          //
};

class AmbientOcclusionRenderingSoftware : public AmbientOcclusionRendering
{
public:
	AmbientOcclusionRenderingSoftware();

	~AmbientOcclusionRenderingSoftware() override;

	// 渲染环境关遮蔽
	void RenderAO(const AmbientOcclusionInputData& inputData) override;

	// 获取环境管遮蔽纹理
	Texture2D* GetAmbientOcclusionTexture() const override;

protected:
	void AllocAOTexture(const IntVector2& renderSolution);

	void SetSSAOShaderParameters(float fov, const IntVector2& screenSize, const IntVector2& targetSize);

private:
	// vs
	SharedPtr<Shader> SSAOVS_;
	// setup pass ps
	SharedPtr<Shader> SSAOSetupPS_;
	// step pass ps
	SharedPtr<Shader> SSAOStepPS_;
	// final pass ps
	SharedPtr<Shader> SSAOFinalPS_;

	//
	SharedPtr<ShaderParameters> shaderParameters_;
	// 随机法线
	SharedPtr<Texture2D> randomNormals_;
	// 降采样normal
	SharedPtr<Texture2D> downsampledNormal_;
	// 降采样ao
	SharedPtr<Texture2D> downsampledAO_;
	// ao
	SharedPtr<Texture2D> aoTexture_;

	// AO设置
	AmbientOcclusionSettings settings_;
};

}
