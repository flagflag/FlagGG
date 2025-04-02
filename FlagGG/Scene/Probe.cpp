#include "Probe.h"
#include "Graphics/RenderEngine.h"
#include "Resource/ResourceCache.h"
#include "Resource/XMLFile.h"

namespace FlagGG
{

Probe::Probe()
	: shaderConstants_{}
	, diffuseIntensity_(0.3f)
	, specularIntensity_(0.3f)
{
}

void Probe::Load(const String& path)
{
	if (auto* xmlFile = GetSubsystem<ResourceCache>()->GetResource<XMLFile>(path + "/sh.xml"))
	{
		auto root = xmlFile->GetRoot();
		shaderConstants_[0] = root.GetVector4("SHAr");
		shaderConstants_[1] = root.GetVector4("SHAg");
		shaderConstants_[2] = root.GetVector4("SHAb");
		shaderConstants_[3] = root.GetVector4("SHBr");
		shaderConstants_[4] = root.GetVector4("SHBg");
		shaderConstants_[5] = root.GetVector4("SHBb");
		shaderConstants_[6] = root.GetVector4("SHC");
	}

	specularCube_ = GetSubsystem<ResourceCache>()->GetResource<TextureCube>(path + "/specular.dds");
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
	renderEngine->SetDefaultTexture(TEXTURE_CLASS_IBL, specularCube_);
}

}
