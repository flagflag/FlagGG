#include "Probe.h"
#include "Graphics/RenderEngine.h"
#include "Resource/ResourceCache.h"

namespace FlagGG
{

Probe::Probe()
	: diffuseIntensity_(0.3f)
	, specularIntensity_(0.3f)
{
	shaderConstants_[0] = Vector4(0.274252, 0.245369, -0.0620501, 0.701207);
	shaderConstants_[1] = Vector4(0.353687, 0.365217, -0.0740966, 0.796212);
	shaderConstants_[2] = Vector4(0.381397, 0.505522, -0.0679338, 0.825213);
	shaderConstants_[3] = Vector4(0.102849, -0.0645187, -0.0388975, -0.0904622);
	shaderConstants_[4] = Vector4(0.121485, -0.0651856, -0.0317596, -0.10881);
	shaderConstants_[5] = Vector4(0.127528, -0.0572494, -0.0096643, -0.118525);
	shaderConstants_[6] = Vector4(0.104437, 0.112997, 0.0955683, 1);

	iblCube_ = GetSubsystem<ResourceCache>()->GetResource<TextureCube>("Textures/daySpecularHDR.dds");
	iblCube_->SetAddressMode(TEXTURE_COORDINATE_U, TEXTURE_ADDRESS_CLAMP);
	iblCube_->SetAddressMode(TEXTURE_COORDINATE_V, TEXTURE_ADDRESS_CLAMP);
	iblCube_->SetAddressMode(TEXTURE_COORDINATE_W, TEXTURE_ADDRESS_CLAMP);
	iblCube_->SetFilterMode(TEXTURE_FILTER_TRILINEAR);
}

void Probe::SetDiffuseIntensity(float diffuseIntensity)
{
	diffuseIntensity_ = diffuseIntensity;
}

void Probe::SetSpecularIntensity(float specularIntensity)
{
	specularIntensity_ = specularIntensity;
}

void Probe::SetArea(const BoundingBox& bbox)
{
	worldBoundingBox_ = bbox;
}

void Probe::ApplyRender(RenderEngine* renderEngine)
{
	auto& engineShaderParameters = renderEngine->GetShaderParameters();
	engineShaderParameters.SetValue(SP_ENV_CUBE_ANGLE, Vector4(0, 1, 0, 1));
	engineShaderParameters.SetValue(SP_SH_INTENSITY, diffuseIntensity_);
	engineShaderParameters.SetValue(SP_IBL_INTENSITY, specularIntensity_);
	engineShaderParameters.SetValue(SP_AMBIENT_OCCLUSION_INTENSITY, 1.0f);
	engineShaderParameters.SetValue(SP_SHAR, shaderConstants_[0]);
	engineShaderParameters.SetValue(SP_SHAG, shaderConstants_[1]);
	engineShaderParameters.SetValue(SP_SHAB, shaderConstants_[2]);
	engineShaderParameters.SetValue(SP_SHBR, shaderConstants_[3]);
	engineShaderParameters.SetValue(SP_SHBG, shaderConstants_[4]);
	engineShaderParameters.SetValue(SP_SHBB, shaderConstants_[5]);
	engineShaderParameters.SetValue(SP_SHC, shaderConstants_[6]);
	renderEngine->SetDefaultTexture(TEXTURE_CLASS_IBL, iblCube_);
}

}
