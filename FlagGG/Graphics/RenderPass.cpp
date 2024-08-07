#include "RenderPass.h"
#include "Scene/DrawableComponent.h"
#include "Scene/Camera.h"
#include "Scene/Light.h"
#include "Scene/Node.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Material.h"
#include "Graphics/Texture2D.h"
#include "Graphics/TextureCube.h"
#include "Resource/Image.h"
#include "Resource/ResourceCache.h"
#include "GfxDevice/GfxDevice.h"
#include "GfxDevice/GfxRenderSurface.h"
#include "GfxDevice/GfxTexture.h"

namespace FlagGG
{

/*********************************************************/
/*                       RenderPass                      */
/*********************************************************/

RenderPass::RenderPass()
{

}

RenderPass::~RenderPass()
{

}

/*********************************************************/
/*                    ShadowRenderPass                   */
/*********************************************************/

ShadowRenderPass::ShadowRenderPass()
{
	rasterizerState_.scissorTest_ = false;
	rasterizerState_.fillMode_ = FILL_SOLID;
	rasterizerState_.cullMode_ = CULL_FRONT;
	rasterizerState_.blendMode_ = BLEND_REPLACE;

	depthStencilState_.depthWrite_ = true;
	depthStencilState_.depthTestMode_ = COMPARISON_LESS_EQUAL;
	depthStencilState_.stencilTest_ = false;
}

ShadowRenderPass::~ShadowRenderPass()
{

}

void ShadowRenderPass::Clear()
{
	for (auto& it : shadowRenderContextMap_)
	{
		it.second_.renderBatchQueue_.renderBatches_.Clear();
		it.second_.renderBatchQueue_.renderBatchGroups_.Clear();
	}
}

void ShadowRenderPass::CollectBatch(RenderPassContext* context)
{
	auto it = shadowRenderContextMap_.Find(context->light_);
	if (it == shadowRenderContextMap_.End())
	{
		it = shadowRenderContextMap_.Insert(MakePair(context->light_, ShadowRenderContext()));
		it->second_.light_ = context->light_;
	}

	auto& renderBatches = it->second_.renderBatchQueue_.renderBatches_;

	for (const auto& renderContext : context->drawable_->GetRenderContext())
	{
		if (!renderContext.material_)
			continue;
		auto it = renderContext.material_->GetRenderPass().Find(RENDER_PASS_TYPE_SHADOW);
		if (it != renderContext.material_->GetRenderPass().End())
		{
			auto& renderBatch = renderBatches.EmplaceBack(renderContext);
			renderBatch.renderPassType_ = RENDER_PASS_TYPE_SHADOW;
			renderBatch.renderPassInfo_ = &(it->second_);
			renderBatch.vertexShader_ = it->second_.GetVertexShader();
			renderBatch.pixelShader_ = it->second_.GetPixelShader();
		}
	}
}

void ShadowRenderPass::SortBatch()
{

}

void ShadowRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	RenderEngine* renderEngine = GetSubsystem<RenderEngine>();

	for (auto& it : shadowRenderContextMap_)
	{
		auto& renderBatches = it.second_.renderBatchQueue_.renderBatches_;
		for (auto& renderBatch : renderBatches)
		{
			renderEngine->SetRasterizerState(renderBatch.renderPassInfo_->GetRasterizerState());
			GfxDevice::GetDevice()->SetDepthBias(-0.25f, -5);
			renderEngine->SetDepthStencilState(depthStencilState_);
			renderEngine->SetShaderParameter(shadowCamera, renderBatch);
			renderEngine->SetShaders(renderBatch.vertexShader_, renderBatch.pixelShader_);
			renderEngine->SetVertexBuffers(renderBatch.geometry_->GetVertexBuffers());
			renderEngine->SetIndexBuffer(renderBatch.geometry_->GetIndexBuffer());
			renderEngine->SetPrimitiveType(renderBatch.geometry_->GetPrimitiveType());
			renderEngine->DrawCallIndexed(renderBatch.geometry_->GetIndexStart(), renderBatch.geometry_->GetIndexCount());
		}
	}
}

bool ShadowRenderPass::HasAnyBatch() const
{
	for (auto& it : shadowRenderContextMap_)
	{
		if (it.second_.renderBatchQueue_.renderBatches_.Size())
			return true;
		if (it.second_.renderBatchQueue_.renderBatchGroups_.Size())
			return true;
	}

	return false;
}

/*********************************************************/
/*                     LitRenderPass                     */
/*********************************************************/

LitRenderPass::LitRenderPass()
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

LitRenderPass::~LitRenderPass()
{

}

void LitRenderPass::Clear()
{
	for (auto& it : litRenderContextMap_)
	{
		it.second_.renderBatchQueue_.renderBatches_.Clear();
		it.second_.renderBatchQueue_.renderBatchGroups_.Clear();
	}
}

void LitRenderPass::CollectBatch(RenderPassContext* context)
{
	auto it = litRenderContextMap_.Find(context->light_);
	if (it == litRenderContextMap_.End())
	{
		it = litRenderContextMap_.Insert(MakePair(context->light_, LitRenderContext()));
		it->second_.light_ = context->light_;
	}

	auto& renderBatches = it->second_.renderBatchQueue_.renderBatches_;

	for (const auto& renderContext : context->drawable_->GetRenderContext())
	{
		if (!renderContext.material_)
			continue;
		auto it = renderContext.material_->GetRenderPass().Find(RENDER_PASS_TYPE_FORWARD_LIT);
		if (it != renderContext.material_->GetRenderPass().End())
		{
			auto& renderBatch = renderBatches.EmplaceBack(renderContext);
			renderBatch.renderPassType_ = RENDER_PASS_TYPE_FORWARD_LIT;
			renderBatch.renderPassInfo_ = &(it->second_);
			renderBatch.vertexShader_ = it->second_.GetVertexShader();
			renderBatch.pixelShader_ = it->second_.GetPixelShader();
		}
	}
}

void LitRenderPass::SortBatch()
{

}

void LitRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	RenderEngine* renderEngine = GetSubsystem<RenderEngine>();

	for (auto& it : litRenderContextMap_)
	{
		Light* light = it.first_;
		Node* lightCameraNode = light->GetNode();

		auto& engineShaderParameters = renderEngine->GetShaderParameters();
		engineShaderParameters.SetValue(SP_LIGHT_POS, lightCameraNode->GetWorldPosition());
		engineShaderParameters.SetValue(SP_LIGHT_DIR, -lightCameraNode->GetWorldDirection());
		engineShaderParameters.SetValue(SP_LIGHT_COLOR, light->GetEffectiveColor());
		engineShaderParameters.SetValue(SP_LIGHT_VIEW_MATRIX, shadowCamera->GetViewMatrix());
		engineShaderParameters.SetValue(SP_LIGHT_PROJVIEW_MATRIX, shadowCamera->GetProjectionMatrix() * shadowCamera->GetViewMatrix());
		engineShaderParameters.SetValue(SP_ENV_CUBE_ANGLE, Vector4(0, 1, 0, 1));
		engineShaderParameters.SetValue(SP_SH_INTENSITY, 0.3f);
		engineShaderParameters.SetValue(SP_IBL_INTENSITY, 0.3f);
		engineShaderParameters.SetValue(SP_AMBIENT_OCCLUSION_INTENSITY, 1.0f);
		engineShaderParameters.SetValue(SP_SHAR, shaderConstants_[0]);
		engineShaderParameters.SetValue(SP_SHAG, shaderConstants_[1]);
		engineShaderParameters.SetValue(SP_SHAB, shaderConstants_[2]);
		engineShaderParameters.SetValue(SP_SHBR, shaderConstants_[3]);
		engineShaderParameters.SetValue(SP_SHBG, shaderConstants_[4]);
		engineShaderParameters.SetValue(SP_SHBB, shaderConstants_[5]);
		engineShaderParameters.SetValue(SP_SHC, shaderConstants_[6]);
		renderEngine->SetDefaultTextures(TEXTURE_CLASS_IBL, iblCube_);
		auto shadowMap = renderEngine->GetDefaultTexture(TEXTURE_CLASS_SHADOWMAP);
		if (shadowMap)
		{
			engineShaderParameters.SetValue(SP_SHADOWMAP_PIXEL_TEXELS, Vector2(1.f / shadowMap->GetWidth(), 1.f / shadowMap->GetHeight()));
		}

		auto& renderBatches = it.second_.renderBatchQueue_.renderBatches_;
		for (auto& renderBatch : renderBatches)
		{
			renderEngine->DrawBatch(camera, renderBatch);
		}
	}
}

bool LitRenderPass::HasAnyBatch() const
{
	for (auto& it : litRenderContextMap_)
	{
		if (it.second_.renderBatchQueue_.renderBatches_.Size())
			return true;
		if (it.second_.renderBatchQueue_.renderBatchGroups_.Size())
			return true;
	}

	return false;
}

/*********************************************************/
/*                    WaterRenderPass                    */
/*********************************************************/

WaterRenderPass::WaterRenderPass()
{

}

WaterRenderPass::~WaterRenderPass()
{

}

void WaterRenderPass::Clear()
{
	renderBatchQueue_.renderBatches_.Clear();
	renderBatchQueue_.renderBatchGroups_.Clear();
}

void WaterRenderPass::CollectBatch(RenderPassContext* context)
{
	auto& renderBatches = renderBatchQueue_.renderBatches_;
	for (const auto& renderContext : context->drawable_->GetRenderContext())
	{
		if (!renderContext.material_)
			continue;
		auto it = renderContext.material_->GetRenderPass().Find(RENDER_PASS_TYPE_FORWARD_WATER);
		if (it != renderContext.material_->GetRenderPass().End())
		{
			auto& renderBatch = renderBatches.EmplaceBack(renderContext);
			renderBatch.renderPassType_ = RENDER_PASS_TYPE_FORWARD_WATER;
			renderBatch.renderPassInfo_ = &(it->second_);
			renderBatch.vertexShader_ = it->second_.GetVertexShader();
			renderBatch.pixelShader_ = it->second_.GetPixelShader();
		}
	}
}

void WaterRenderPass::SortBatch()
{

}

void WaterRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	auto* gfxDevice = GfxDevice::GetDevice();
	auto* gfxRenderSurface = gfxDevice->GetRenderTarget(0);
	if (!gfxRenderSurface)
		return;
	auto* gfxRenderTexture = gfxRenderSurface->GetOwnerTexture();
	if (!gfxRenderTexture)
		return;

	if (!refractionTexture_)
	{
		refractionTexture_ = new Texture2D();
	}

	const auto& rtDesc = gfxRenderTexture->GetDesc();
	if (rtDesc.width_ != refractionTexture_->GetWidth() ||
		rtDesc.height_ != refractionTexture_->GetHeight())
	{
		refractionTexture_->SetNumLevels(1);
		refractionTexture_->SetAddressMode(TEXTURE_COORDINATE_U, TEXTURE_ADDRESS_CLAMP);
		refractionTexture_->SetAddressMode(TEXTURE_COORDINATE_V, TEXTURE_ADDRESS_CLAMP);
		refractionTexture_->SetAddressMode(TEXTURE_COORDINATE_W, TEXTURE_ADDRESS_CLAMP);
		refractionTexture_->SetSize(rtDesc.width_, rtDesc.height_, rtDesc.format_, TEXTURE_DYNAMIC);
	}

	auto* gfxRefractionTexture = refractionTexture_->GetGfxTextureRef();
	if (gfxRefractionTexture)
	{
		gfxRefractionTexture->UpdateTexture(gfxRenderTexture);
	}

	RenderEngine* renderEngine = GetSubsystem<RenderEngine>();
	renderEngine->SetDefaultTextures(TEXTURE_CLASS_DIFFUSE, refractionTexture_);

	auto& renderBatches = renderBatchQueue_.renderBatches_;
	for (auto& renderBatch : renderBatches)
	{
		renderEngine->DrawBatch(camera, renderBatch);
	}

	renderEngine->SetDefaultTextures(TEXTURE_CLASS_DIFFUSE, nullptr);
}

/*********************************************************/
/*                    AlphaRenderPass                    */
/*********************************************************/

AlphaRenderPass::AlphaRenderPass()
{

}

AlphaRenderPass::~AlphaRenderPass()
{

}

void AlphaRenderPass::Clear()
{
	renderBatchQueue_.renderBatches_.Clear();
	renderBatchQueue_.renderBatchGroups_.Clear();
}

void AlphaRenderPass::CollectBatch(RenderPassContext* context)
{
	auto& renderBatches = renderBatchQueue_.renderBatches_;
	for (const auto& renderContext : context->drawable_->GetRenderContext())
	{
		if (!renderContext.material_)
			continue;
		auto it = renderContext.material_->GetRenderPass().Find(RENDER_PASS_TYPE_FORWARD_ALPHA);
		if (it != renderContext.material_->GetRenderPass().End())
		{
			auto& renderBatch = renderBatches.EmplaceBack(renderContext);
			renderBatch.renderPassType_ = RENDER_PASS_TYPE_FORWARD_ALPHA;
			renderBatch.renderPassInfo_ = &(it->second_);
			renderBatch.vertexShader_ = it->second_.GetVertexShader();
			renderBatch.pixelShader_ = it->second_.GetPixelShader();
		}
	}
}

void AlphaRenderPass::SortBatch()
{

}

void AlphaRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	RenderEngine* renderEngine = GetSubsystem<RenderEngine>();

	auto& renderBatches = renderBatchQueue_.renderBatches_;
	for (auto& renderBatch : renderBatches)
	{
		renderEngine->DrawBatch(camera, renderBatch);
	}
}

/*********************************************************/
/*                 DeferredBaseRenderPass                */
/*********************************************************/

DeferredBaseRenderPass::DeferredBaseRenderPass()
{

}

DeferredBaseRenderPass::~DeferredBaseRenderPass()
{

}

void DeferredBaseRenderPass::Clear()
{
	renderBatchQueue_.renderBatches_.Clear();
	renderBatchQueue_.renderBatchGroups_.Clear();
}

void DeferredBaseRenderPass::CollectBatch(RenderPassContext* context)
{
	auto& renderBatches = renderBatchQueue_.renderBatches_;
	for (const auto& renderContext : context->drawable_->GetRenderContext())
	{
		if (!renderContext.material_)
			continue;
		auto it = renderContext.material_->GetRenderPass().Find(RENDER_PASS_TYPE_DEFERRED_BASE);
		if (it != renderContext.material_->GetRenderPass().End())
		{
			auto& renderBatch = renderBatches.EmplaceBack(renderContext);
			renderBatch.renderPassType_ = RENDER_PASS_TYPE_DEFERRED_BASE;
			renderBatch.renderPassInfo_ = &(it->second_);
			renderBatch.vertexShader_ = it->second_.GetVertexShader();
			renderBatch.pixelShader_ = it->second_.GetPixelShader();
		}
	}
}

void DeferredBaseRenderPass::SortBatch()
{

}

void DeferredBaseRenderPass::RenderBatch(Camera* camera, Camera* shadowCamera, UInt32 layer)
{
	RenderEngine* renderEngine = GetSubsystem<RenderEngine>();

	auto& renderBatches = renderBatchQueue_.renderBatches_;
	for (auto& renderBatch : renderBatches)
	{
		renderEngine->DrawBatch(camera, renderBatch);
	}
}

}
