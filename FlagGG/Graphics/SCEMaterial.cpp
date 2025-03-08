#include "SCEMaterial.h"
#include "Core/ObjectFactory.h"
#include "Resource/ResourceCache.h"
#include "Graphics/Texture2D.h"
#include "ResourceTranslation/SCE/SCEResourceTranslation.h"

namespace FlagGG
{

REGISTER_TYPE_FACTORY(SCEMaterial);

SCEMaterial::SCEMaterial()
	: skinnedMaterial_(false)
{

}

SCEMaterial::SCEMaterial(bool skinnedMaterial)
	: skinnedMaterial_(skinnedMaterial)
{

}

SCEMaterial::~SCEMaterial() = default;

bool SCEMaterial::BeginLoad(IOFrame::Buffer::IOBuffer* stream)
{
	GenericMaterialDescription materialDesc;
	if (!GetSubsystem<SCEResourceTranslation>()->LoadMaterial(stream, materialDesc))
	{
		return false;
	}

	auto* cache = GetSubsystem<ResourceCache>();

	// Set shader parameters
	{
		CreateShaderParameters();
		auto param = GetShaderParameters();
		param->AddParametersDefine<Color>("colorFactor");
		param->AddParametersDefine<float>("metallicFactor");
		param->AddParametersDefine<float>("roughnessFactor");
		param->AddParametersDefine<float>("metalMul0or1");
		param->AddParametersDefine<float>("emissiveMul");
		param->AddParametersDefine<Color>("emissiveColor");
		for (auto& it : materialDesc.shaderParameters_)
		{
			if (it.first_ == "ColorFactor")
			{
				param->SetValue<Color>("colorFactor", ToColor(it.second_));
			}
			else if (it.first_ == "MetallicFactor")
			{
				param->SetValue<float>("metallicFactor", ToFloat(it.second_));
			}
			else if (it.first_ == "RoughnessFactor")
			{
				param->SetValue<float>("roughnessFactor", ToFloat(it.second_));
			}
			else if (it.first_ == "MetalMul0or1")
			{
				param->SetValue<float>("metalMul0or1", ToFloat(it.second_));
			}
			else if (it.first_ == "Emissive_Mul")
			{
				param->SetValue<float>("emissiveMul", ToFloat(it.second_));
			}
			else if (it.first_ == "Color_Emissive")
			{
				param->SetValue<Color>("emissiveColor", ToColor(it.second_));
			}
		}
	}
	// Set textures
	for (auto& it : materialDesc.textureDescs_)
	{
		if (it.first_ < MAX_TEXTURE_CLASS)
		{
			auto& textureDesc = it.second_;
			if (auto* existingTexture = cache->GetExistingResource<Texture2D>(textureDesc.image_->GetName()))
			{
				SetTexture((TextureClass)it.first_, existingTexture);
			}
			else
			{
				SharedPtr<Texture2D> texture = MakeShared<Texture2D>();
				texture->SetSRGB(textureDesc.srgb_);
				texture->SetAddressMode(TEXTURE_COORDINATE_U, textureDesc.addresMode_[TEXTURE_COORDINATE_U]);
				texture->SetAddressMode(TEXTURE_COORDINATE_V, textureDesc.addresMode_[TEXTURE_COORDINATE_V]);
				texture->SetAddressMode(TEXTURE_COORDINATE_W, textureDesc.addresMode_[TEXTURE_COORDINATE_W]);
				texture->SetFilterMode(textureDesc.filterMode_);
				texture->SetData(textureDesc.image_);
				texture->SetName(textureDesc.image_->GetName());
				cache->AddManualResource(texture);
				SetTexture((TextureClass)it.first_, texture);
			}
		}
	}
	// Create pass
	for (auto& it : materialDesc.passDescs_)
	{
		if (it.name_ == "litbase" && it.pixelShaderName_.ToLower() == "shaderlab/model/defaultps")
		{
			const String meshTypeDefine = skinnedMaterial_ ? "SKINNED" : "STATIC";
			{
				auto& pass = CreateOrGetRenderPass(RENDER_PASS_TYPE_FORWARD_LIT);
				pass.SetRasterizerState(it.rasterizerState_);
				pass.SetDepthStencilState(it.depthStencilState_);
				auto shaderCode = cache->GetResource<ShaderCode>("Shader/SCE/Default.hlsl");
				pass.SetVertexShader(shaderCode->GetShader(VS, { meshTypeDefine, "SHADOW" }));
				Vector<String> pixelDefines = it.pixelDefines_.Split(' ');
				pixelDefines.Push("SHADOW");
				pixelDefines.Push("DIRLIGHT");
				pass.SetPixelShader(shaderCode->GetShader(PS, pixelDefines));
				pass.SetDepthTestMode(COMPARISON_LESS_EQUAL);
			}
			{
				auto& pass = CreateOrGetRenderPass(RENDER_PASS_TYPE_DEFERRED_BASE);
				pass.SetRasterizerState(it.rasterizerState_);
				pass.SetDepthStencilState(it.depthStencilState_);
				auto shaderCode = cache->GetResource<ShaderCode>("Shader/SCE/Default.hlsl");
				pass.SetVertexShader(shaderCode->GetShader(VS, { meshTypeDefine }));
				Vector<String> pixelDefines = it.pixelDefines_.Split(' ');
				pixelDefines.Push("DEFERRED_BASEPASS");
				pass.SetPixelShader(shaderCode->GetShader(PS, pixelDefines));
				pass.SetDepthTestMode(COMPARISON_EQUAL);
				pass.SetDepthWrite(false);
			}
			{
				auto& pass = CreateOrGetRenderPass(RENDER_PASS_TYPE_SHADOW);
				auto shaderCode = cache->GetResource<ShaderCode>("Shader/Shadow.hlsl");
				pass.SetVertexShader(shaderCode->GetShader(VS, { meshTypeDefine }));
				pass.SetPixelShader(shaderCode->GetShader(PS, { it.pixelDefines_.Split(' ') }));
			}
			{
				auto& pass = CreateOrGetRenderPass(RENDER_PASS_TYPE_DEPTH);
				auto shaderCode = cache->GetResource<ShaderCode>("Shader/Depth.hlsl");
				pass.SetVertexShader(shaderCode->GetShader(VS, { meshTypeDefine }));
				pass.SetPixelShader(shaderCode->GetShader(PS, { it.pixelDefines_.Split(' ') }));
				pass.SetDepthTestMode(COMPARISON_LESS_EQUAL);
			}
			break;
		}
	}

	return true;
}

bool SCEMaterial::EndLoad()
{
	return true;
}

}
